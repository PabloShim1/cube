#include "CubeSteppingAction.hh"
#include "CubeDetectorConstruction.hh"
#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"

namespace cube {

SteppingAction::SteppingAction(EventAction* ea) 
: G4UserSteppingAction(), fEventAction(ea), fScoringVolume(nullptr) 
{}

SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step* step) {
    auto volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
    
    if (!fScoringVolume) {
        auto det = static_cast<const DetectorConstruction*>(
            G4RunManager::GetRunManager()->GetUserDetectorConstruction());
        fScoringVolume = det->GetProductLogic();
    }

    if (volume != fScoringVolume) return;

    G4double edep = step->GetTotalEnergyDeposit();
    if (edep <= 0.) return;

    auto analysis = G4AnalysisManager::Instance();
    
    // Координаты
    G4ThreeVector worldPos = step->GetPreStepPoint()->GetPosition();
    G4ThreeVector localPos = step->GetPreStepPoint()->GetTouchableHandle()->GetHistory()
                             ->GetTopTransform().TransformPoint(worldPos);

    auto det = static_cast<const DetectorConstruction*>(
        G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    G4double thickness = det->GetProductThickness();

    // Глубина от поверхности (+Z грань в локальных координатах)
    G4double depth = (thickness / 2.0) - localPos.z();

    // ЗАПОЛНЕНИЕ: Обрати внимание на ID 0 в обоих вызовах!
    analysis->FillH3(0, localPos.x(), localPos.y(), localPos.z(), edep);
    analysis->FillH1(0, depth, edep); 
}

} // namespace cube