#include "CubeSteppingAction.hh"
#include "CubeEventAction.hh"
#include "CubeDetectorConstruction.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4AnalysisManager.hh"

namespace cube
{

SteppingAction::SteppingAction(EventAction* eventAction)
: fEventAction(eventAction)
{
    const DetectorConstruction* detConstruction
        = static_cast<const DetectorConstruction*>
        (G4RunManager::GetRunManager()->GetUserDetectorConstruction());

    this->fScoringVolume = detConstruction->GetCubeLogic();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    auto volume = step->GetPreStepPoint()
                      ->GetTouchableHandle()
                      ->GetVolume()
                      ->GetLogicalVolume();

    if (volume != this->fScoringVolume) {
        return;
    }

    auto pre = step->GetPreStepPoint()->GetPosition();
    auto post = step->GetPostStepPoint()->GetPosition();
    auto pos = pre + (post - pre) * G4UniformRand();
    auto edep = step->GetTotalEnergyDeposit();

    auto analysis = G4AnalysisManager::Instance();
    analysis->FillH3(0, pos.getX(), pos.getY(), pos.getZ(), edep);
}

}
