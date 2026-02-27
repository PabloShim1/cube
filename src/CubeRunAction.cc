#include "CubeRunAction.hh"
#include "CubePrimaryGeneratorAction.hh"
#include "CubeDetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"

#include <algorithm>
#include <sstream>
#include <vector>

namespace cube
{

RunAction::RunAction()
{
  auto analysis = G4AnalysisManager::Instance();
  analysis->SetDefaultFileType("root");
  analysis->SetFirstHistoId(0);
  analysis->SetFirstProfileId(0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run* run)
{
  const DetectorConstruction* detConstruction
      = static_cast<const DetectorConstruction*>
        (G4RunManager::GetRunManager()->GetUserDetectorConstruction());

  auto pos = detConstruction->GetCubePhys()->GetTranslation();
  auto xmin = pos.getX() - detConstruction->GetCubeSolid()->GetXHalfLength();
  auto xmax = pos.getX() + detConstruction->GetCubeSolid()->GetXHalfLength();
  auto ymin = pos.getY() - detConstruction->GetCubeSolid()->GetYHalfLength();
  auto ymax = pos.getY() + detConstruction->GetCubeSolid()->GetYHalfLength();
  auto zmin = pos.getZ() - detConstruction->GetCubeSolid()->GetZHalfLength();
  auto zmax = pos.getZ() + detConstruction->GetCubeSolid()->GetZHalfLength();

  const G4double dx = 1 * CLHEP:: cm ; // TODO вынести в гиперпараметры.
  const G4double dy = 1 * CLHEP:: cm; // TODO вынести в гиперпараметры.
  const G4double dz = 1 * CLHEP:: cm ; // TODO вынести в гиперпараметры.
  // TODO снимать первые z_hq с меньшим шагом. Создать отдельный регион под z_hq и заполнить zedges соответствующим образом.

  std::vector<G4double> xedges, yedges, zedges;

  for(G4double x = xmin; x < xmax; x += dx) {
    xedges.push_back(x);
  }
  for(G4double y = ymin; y < ymax; y += dy) {
    yedges.push_back(y);
  }
  for(G4double z = zmin; z < zmax; z += dz) {
    zedges.push_back(z);
  }

  xedges.push_back(xmax);
  yedges.push_back(ymax);
  zedges.push_back(zmax);


  for(auto i: {&xedges, &yedges, &zedges}) {
    std::sort(i->begin(), i->end());
    auto [first, last] = std::ranges::unique(*i);
    i->erase(first, last);
    i->shrink_to_fit();
  }

  std::stringstream ss;
  ss << "_cube_run_" << run->GetRunID();

  auto analysis = G4AnalysisManager::Instance();
  analysis->OpenFile(ss.str());
  analysis->CreateH3(
    "vdd", "Volume-Dose Distribution over the cube",
    xedges, yedges, zedges
  );
}

void RunAction::EndOfRunAction(const G4Run* run)
{
  G4AnalysisManager::Instance()->Write();
  G4AnalysisManager::Instance()->CloseFile();
  G4AnalysisManager::Instance()->Clear();
}

}
