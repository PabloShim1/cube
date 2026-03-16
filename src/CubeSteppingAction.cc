#include "CubeSteppingAction.hh"
#include "CubeDetectorConstruction.hh"
#include "CubeEventAction.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Material.hh"

namespace cube {

SteppingAction::SteppingAction(EventAction* ea)
: G4UserSteppingAction(), fEventAction(ea) {}

SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step* step) {
    // 1. Получаем физический объем в начале шага
    auto prePoint = step->GetPreStepPoint();
    auto vPhys = prePoint->GetPhysicalVolume();

    // Если мы в пустоте - выходим
    if (!vPhys) return;

    // 2. ЖЕСТКАЯ ПРОВЕРКА: Считаем дозу ТОЛЬКО если мы внутри порошка.
    // Имя "Product_Phys" должно совпадать с тем, что у тебя в DetectorConstruction.cc
    if (vPhys->GetName() != "Product_Phys") return;

    // 3. Проверка на выделенную энергию
    G4double edep = step->GetTotalEnergyDeposit();
    if (edep <= 0.) return;

    // Достаем данные о детекторе
    auto det = static_cast<const DetectorConstruction*>(
        G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    
    auto analysis = G4AnalysisManager::Instance();
    G4double thickness = det->GetProductThickness();

    // Координаты шага
    G4ThreeVector prePos = prePoint->GetPosition();
    G4ThreeVector postPos = step->GetPostStepPoint()->GetPosition();
    
    // Матрица трансформации (чтобы понимать, где верх/низ конкретно этого объема)
    auto transform = prePoint->GetTouchableHandle()->GetHistory()->GetTopTransform();

    // 4. РАЗБИВАЕМ ШАГ НА МИКРО-ТОЧКИ (0.05 мм)
    // Это делает кривую плавной и позволяет точно отсечь границу с алюминием
    G4double stepLength = step->GetStepLength();
    int n_points = std::max(1, (int)(stepLength / (0.05 * mm)));
    G4double edep_per_point = edep / n_points;

    for (int i = 0; i < n_points; i++) {
        G4double fraction = (i + 0.5) / n_points;
        G4ThreeVector pointPos = prePos + fraction * (postPos - prePos);
        
        // Переводим точку в локальную систему координат порошка
        G4ThreeVector localPos = transform.TransformPoint(pointPos);
        
        // Глубина: считаем от поверхности (Z в локальных координатах идет от -Thick/2 до +Thick/2)
        G4double depth = (thickness / 2.0) - localPos.z();

        // 5. ФИНАЛЬНЫЙ ФИЛЬТР ГРАНИЦЫ
        // Записываем только если точка ГАРАНТИРОВАННО внутри порошка (не касаясь дна)
        if (depth >= 0.0 && depth < thickness) {
            
            // Записываем в 1D гистограмму (PDD) - ID 0
            analysis->FillH1(0, depth/mm, edep_per_point/MeV);

            // Записываем в 3D гистограмму (EdepMap) - ID 0
            // Используем локальные координаты для 3D карты
            analysis->FillH3(0, localPos.x()/mm, localPos.y()/mm, localPos.z()/mm, edep_per_point/MeV);
        }
    }
}

} // namespace cube