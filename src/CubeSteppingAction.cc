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
    // 1. Получаем текущий логический объем, в котором находится частица
    auto volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
    
    // 2. ОБЯЗАТЕЛЬНОЕ ИСПРАВЛЕНИЕ: 
    // Получаем актуальный указатель на ProductLogic ПРЯМО СЕЙЧАС.
    // Если геометрия была перестроена между ранами, старый fScoringVolume станет невалидным.
    auto det = static_cast<const DetectorConstruction*>(
        G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    fScoringVolume = det->GetProductLogic();

    // 3. ФИЛЬТР: Если частица НЕ в продукции (например, в алюминиевом Tray или в воздухе), выходим
    if (volume != fScoringVolume) return;

    // 4. Энерговыделение
    G4double edep = step->GetTotalEnergyDeposit();
    if (edep <= 0.) return;

    auto analysis = G4AnalysisManager::Instance();
    
    // Проверка активации гистограммы (ID 0 согласно CubeRunAction.cc)
    if (!analysis->GetH1Activation(0)) return; 

    // 5. Координаты и расчет глубины
    // Переводим глобальные координаты в локальные координаты объема Product
    G4ThreeVector worldPos = step->GetPreStepPoint()->GetPosition();
    G4ThreeVector localPos = step->GetPreStepPoint()->GetTouchableHandle()->GetHistory()
                             ->GetTopTransform().TransformPoint(worldPos);

    G4double thickness = det->GetProductThickness();

    // Расчет глубины от верхней поверхности продукции (+Z грань в локальной системе координат)
    // localPos.z() меняется от -thickness/2 до +thickness/2
    G4double depth = (thickness / 2.0) - localPos.z();

    // 6. Заполнение гистограмм
    // H3 - 3D распределение дозы (ID 0)
    analysis->FillH3(0, localPos.x(), localPos.y(), localPos.z(), edep);
    
    // H1 - Глубинное распределение дозы (PDD) (ID 0)
    analysis->FillH1(0, depth, edep); 
}

} // namespace cube