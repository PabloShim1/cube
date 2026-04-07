#include "CubeSteppingAction.hh"
#include "CubeDetectorConstruction.hh"
#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"

namespace cube {
  SteppingAction::SteppingAction(EventAction* event) : G4UserSteppingAction() {}

  SteppingAction::~SteppingAction() {} // Исправляет ошибку vtable

  void SteppingAction::UserSteppingAction(const G4Step* step) {
    auto det = static_cast<const DetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    
    // Проверка, что мы внутри куба
    if (step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume() != det->GetCubeLogic()) return;

    G4double edep = step->GetTotalEnergyDeposit();
    if (edep <= 0.) return;

    auto transform = step->GetPreStepPoint()->GetTouchableHandle()->GetHistory()->GetTopTransform();
    G4double thickness = det->GetCubeThickness();
    G4double stepLength = step->GetStepLength();
    
    // Дробление шага для точности
    int n_points = std::max(1, (int)(stepLength / (0.1 * mm)));
    auto analysis = G4AnalysisManager::Instance();

    for (int i = 0; i < n_points; i++) {
        G4ThreeVector worldPos = step->GetPreStepPoint()->GetPosition() + ((i+0.5)/n_points)*(step->GetPostStepPoint()->GetPosition() - step->GetPreStepPoint()->GetPosition());
        G4ThreeVector localPos = transform.TransformPoint(worldPos);
        
        // Глубина от ВЕРХНЕЙ грани (Z лок = +120мм)
        G4double depth = (thickness / 2.0) - localPos.z();

        if (depth >= 0 && depth <= thickness) {
            analysis->FillH1(0, depth/mm, (edep/n_points)/MeV);
        }
        analysis->FillH3(0, localPos.x()/mm, localPos.y()/mm, localPos.z()/mm, (edep/n_points)/MeV);
    }
  }
}