#include "CubeDetectorConstruction.hh"
#include "CubeDetectorMessenger.hh"
#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4SubtractionSolid.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4GeometryManager.hh"

namespace cube {

DetectorConstruction::DetectorConstruction() {
    margin_x = 5*cm; margin_y = 5*cm; margin_z = 10*cm;
    fTrayWallThickness = 1.5*mm;
    fProductSizeX = 890.0*mm;
    fProductSizeY = 390.0*mm;
    fProductThickness = 48.0*mm; 
    source_surface_distance = 60*cm; 
    fMessenger = new DetectorMessenger(this);
}

DetectorConstruction::~DetectorConstruction() { delete fMessenger; }

void DetectorConstruction::ConstructMaterials() {
    G4NistManager* nist = G4NistManager::Instance();
    fWorldMaterial = nist->FindOrBuildMaterial("G4_AIR");
    fContainerMaterial = nist->FindOrBuildMaterial("G4_Al");
    // Используем воду как заполнитель для теста, она хорошо поглощает энергию
    fFillMaterial = nist->FindOrBuildMaterial("G4_WATER"); 
}

G4VPhysicalVolume* DetectorConstruction::Construct() {
    // 1. Полная очистка перед перестроением
    G4GeometryManager::GetInstance()->OpenGeometry();
    G4PhysicalVolumeStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4SolidStore::GetInstance()->Clean();

    ConstructMaterials();

    // 2. Расчет размеров
    G4double trayOuterZ = fProductThickness + fTrayWallThickness;
    G4double worldZ = trayOuterZ + margin_z + source_surface_distance;

    // Позиция центра лотка в мире
    cube_translate = G4ThreeVector(0, 0, -source_surface_distance/2.0);

    // 3. Создание Мира
    G4Box* world_solid = new G4Box("World", (fProductSizeX+margin_x)/2., (fProductSizeY+margin_y)/2., worldZ/2.);
    world_logic = new G4LogicalVolume(world_solid, fWorldMaterial, "WorldLogic");
    world_phys = new G4PVPlacement(0, G4ThreeVector(), world_logic, "WorldPhys", 0, false, 0);

    // 4. Создание Лотка (Tray)
    G4Box* tray_outer = new G4Box("TrayOuter", (fProductSizeX + 2*fTrayWallThickness)/2., (fProductSizeY + 2*fTrayWallThickness)/2., trayOuterZ/2.);
    G4Box* tray_inner = new G4Box("TrayInner", fProductSizeX/2., fProductSizeY/2., fProductThickness/2.);
    
    // Смещение полости внутри лотка, чтобы дно было целым
    G4double z_shift_inner = fTrayWallThickness / 2.0;
    G4SubtractionSolid* tray_solid = new G4SubtractionSolid("TraySolid", tray_outer, tray_inner, 0, G4ThreeVector(0,0,z_shift_inner));

    fTrayLogic = new G4LogicalVolume(tray_solid, fContainerMaterial, "TrayLogic");
    fTrayPhys = new G4PVPlacement(0, cube_translate, fTrayLogic, "TrayPhys", world_logic, false, 0, true);

    // 5. Создание Продукта (Product) - ТО, ЧТО МЫ СЧИТАЕМ
    G4Box* product_solid = new G4Box("Product", fProductSizeX/2., fProductSizeY/2., fProductThickness/2.);
    fProductLogic = new G4LogicalVolume(product_solid, fFillMaterial, "ProductLogic");
    
    // Размещаем продукт ПРЯМО в полости лотка
    fProductPhys = new G4PVPlacement(0, G4ThreeVector(0,0,z_shift_inner), fProductLogic, "ProductPhys", fTrayLogic, false, 0, true);

    SetUpVisAttributes();
    return world_phys;
}

void DetectorConstruction::SetProductThickness(G4double val) {
    fProductThickness = val;
    // Сообщаем менеджеру, что геометрия изменилась
    G4RunManager::GetRunManager()->ReinitializeGeometry();
}

void DetectorConstruction::SetUpVisAttributes() {
    world_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    fTrayLogic->SetVisAttributes(new G4VisAttributes(G4Colour(0.5, 0.5, 0.5, 0.3)));
    fProductLogic->SetVisAttributes(new G4VisAttributes(G4Colour(0, 0, 1, 0.6)));
}

}