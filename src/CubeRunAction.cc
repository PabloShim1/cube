#include "CubeRunAction.hh"
#include "CubeDetectorConstruction.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"
#include "G4Box.hh"
#include <sstream>
#include <vector>

namespace cube {

RunAction::RunAction() {
  auto analysis = G4AnalysisManager::Instance();
  analysis->SetDefaultFileType("root");
  analysis->SetFirstHistoId(0); // Начинаем счет с нуля
}

RunAction::~RunAction() {}

void RunAction::BeginOfRunAction(const G4Run* run) {
  auto analysis = G4AnalysisManager::Instance();
  const DetectorConstruction* det = static_cast<const DetectorConstruction*>(
    G4RunManager::GetRunManager()->GetUserDetectorConstruction());

  // Получаем размеры продукта
  G4ThreeVector worldPos = det->GetTrayPhys()->GetTranslation() + det->GetProductPhys()->GetTranslation();
  auto solid = static_cast<G4Box*>(det->GetProductPhys()->GetLogicalVolume()->GetSolid());

  G4double xh = solid->GetXHalfLength(), yh = solid->GetYHalfLength(), zh = solid->GetZHalfLength();
  
  // Границы для сетки
  G4double xmin = worldPos.x() - xh, xmax = worldPos.x() + xh;
  G4double ymin = worldPos.y() - yh, ymax = worldPos.y() + yh;
  G4double zmin = worldPos.z() - zh, zmax = worldPos.z() + zh;

  // Создаем сетку
  std::vector<G4double> xe, ye, ze;
  for(G4double x = xmin; x <= xmax + 0.1*mm; x += 1.0*cm) xe.push_back(x);
  for(G4double y = ymin; y <= ymax + 0.1*mm; y += 1.0*cm) ye.push_back(y);
  for(G4double z = zmin; z <= zmax + 0.1*mm; z += 1.0*mm) ze.push_back(z);

  std::stringstream ss;
  ss << "Output_Run_" << run->GetRunID();
  analysis->OpenFile(ss.str());
  
  // ВАЖНО: ПОРЯДОК ОПРЕДЕЛЯЕТ ID
  // CreateH3 -> ID 0
  analysis->CreateH3("vdd", "Volume-Dose", xe, ye, ze);
  // CreateH1 -> ID 1
  analysis->CreateH1("pdd", "PDD Profile", ze);

  G4cout << "### Run " << run->GetRunID() << " started. Histograms 0(3D) and 1(1D) created." << G4endl;
}

void RunAction::EndOfRunAction(const G4Run*) {
  auto analysis = G4AnalysisManager::Instance();
  analysis->Write();
  analysis->CloseFile();
  analysis->Clear(); 
}

} // namespace cube