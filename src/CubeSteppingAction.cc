#include "CubeSteppingAction.hh"
#include "CubeDetectorConstruction.hh"
#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"

namespace cube {

SteppingAction::SteppingAction(EventAction* ea)
: G4UserSteppingAction(),
  fEventAction(ea),
  fScoringVolume(nullptr)
{}

SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step* step) {

    auto volume = step->GetPreStepPoint()
                        ->GetTouchableHandle()
                        ->GetVolume()
                        ->GetLogicalVolume();

    auto det = static_cast<const DetectorConstruction*>(
        G4RunManager::GetRunManager()->GetUserDetectorConstruction());

    fScoringVolume = det->GetProductLogic();

    if (volume != fScoringVolume) return;

    G4double edep = step->GetTotalEnergyDeposit();
    if (edep <= 0.) return;

    auto analysis = G4AnalysisManager::Instance();

    // -------------------------------------------------
    // ЛОКАЛЬНЫЕ КООРДИНАТЫ
    // -------------------------------------------------
    G4ThreeVector worldPos =
        step->GetPreStepPoint()->GetPosition();

    auto transform =
        step->GetPreStepPoint()
            ->GetTouchableHandle()
            ->GetHistory()
            ->GetTopTransform();

    G4ThreeVector localPos =
        transform.TransformPoint(worldPos);

    G4double thickness = det->GetProductThickness();

    // -------------------------------------------------
    // ГЛУБИНА ОТ ВЕРХНЕЙ ГРАНИ
    // -------------------------------------------------
    G4double depth =
        (thickness / 2.0) - localPos.z();

    // -------------------------------------------------
    // VDD (работает — не трогаем)
    // -------------------------------------------------
    analysis->FillH3(
        0,
        localPos.x() / mm,
        localPos.y() / mm,
        localPos.z() / mm,
        edep / MeV
    );

    // -------------------------------------------------
    // PDD (фикс)
    // -------------------------------------------------
    if (depth >= 0.0 && depth <= 1000.0 * mm) {
        analysis->FillH1(
            0,
            depth / mm,
            edep / MeV
        );
    }
}

} // namespace cube