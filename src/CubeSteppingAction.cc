#include "CubeSteppingAction.hh"
#include "CubeDetectorConstruction.hh"
#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"

namespace cube {

SteppingAction::SteppingAction(EventAction* ea) : fEventAction(ea), fScoringVolume(nullptr) {}
SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step* step) {
    if (!fScoringVolume) {
        auto det = static_cast<const DetectorConstruction*>(
            G4RunManager::GetRunManager()->GetUserDetectorConstruction());
        fScoringVolume = det->GetProductLogic();
    }

    auto volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
    if (volume != fScoringVolume) return;

    G4double edep = step->GetTotalEnergyDeposit();
    if (edep <= 0.) return;

    G4ThreeVector pos = step->GetPreStepPoint()->GetPosition();
    auto analysis = G4AnalysisManager::Instance();
    
    // ПРОВЕРКА И ЗАПОЛНЕНИЕ
    // ID 0 — это наша 3D гистограмма vdd
    analysis->FillH3(0, pos.x(), pos.y(), pos.z(), edep);
    
    // ID 1 — это наша 1D гистограмма pdd
    analysis->FillH1(1, pos.z(), edep);
}

} // namespace cube