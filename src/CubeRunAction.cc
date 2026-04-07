#include "CubeRunAction.hh"
#include "CubeDetectorConstruction.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "G4SystemOfUnits.hh"
#include <sstream> // Чтобы GetRunID не горел красным

namespace cube {
  RunAction::RunAction() {
    auto analysis = G4AnalysisManager::Instance();
    analysis->SetDefaultFileType("root");
    analysis->SetFirstHistoId(0);
  }

  RunAction::~RunAction() {}

  void RunAction::BeginOfRunAction(const G4Run* run) {
    const DetectorConstruction* det = static_cast<const DetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    G4double thickness = det->GetCubeThickness();
    G4double hx = det->GetCubeSolid()->GetXHalfLength();
    G4double hy = det->GetCubeSolid()->GetYHalfLength();
    G4double hz = det->GetCubeSolid()->GetZHalfLength();

    auto analysis = G4AnalysisManager::Instance();
    
    // 1D: 0 = Верх мешка, 240 = Низ мешка
    analysis->CreateH1("pdd", "Depth Dose (0=Top, 240=Bottom)", 240, 0, thickness/mm);
    
    // 3D
    analysis->CreateH3("vdd", "3D Dose", 50, -hx/mm, hx/mm, 50, -hy/mm, hy/mm, 50, -hz/mm, hz/mm);

    std::stringstream ss;
    ss << "Output_Run_" << run->GetRunID() << ".root";
    analysis->OpenFile(ss.str());
  }

  void RunAction::EndOfRunAction(const G4Run*) {
    auto analysis = G4AnalysisManager::Instance();
    analysis->Write();
    analysis->CloseFile();
    analysis->Clear(); 
  }
}