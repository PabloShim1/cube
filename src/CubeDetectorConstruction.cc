#include "CubeDetectorConstruction.hh"
#include "CubeDetectorMessenger.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4RunManager.hh"
#include "G4VisAttributes.hh"
#include "G4Color.hh"

namespace cube {

DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction() {
    fTrayWallThickness = 1.5*mm;
    fProductSizeX = 900.0*mm;
    fProductSizeY = 400.0*mm;
    fProductThickness = 48.0*mm; 
    fSSD = 60.0*cm; 
    fBulkDensity = 0.55 * g/cm3;
    fMessenger = new DetectorMessenger(this);
}

DetectorConstruction::~DetectorConstruction() { 
    delete fMessenger; 
}

void DetectorConstruction::ConstructMaterials() {
    G4NistManager* nist = G4NistManager::Instance();
    fWorldMaterial = nist->FindOrBuildMaterial("G4_AIR");
    fContainerMaterial = nist->FindOrBuildMaterial("G4_Al");

    if (!G4Material::GetMaterial("ProductMaterial", false)) {
        fFillMaterial = new G4Material("ProductMaterial", fBulkDensity, 3);
        fFillMaterial->AddElement(nist->FindOrBuildElement("C"), 45*perCent);
        fFillMaterial->AddElement(nist->FindOrBuildElement("H"), 7*perCent);
        fFillMaterial->AddElement(nist->FindOrBuildElement("O"), 48*perCent);
    } else {
        fFillMaterial = G4Material::GetMaterial("ProductMaterial");
    }
}

G4VPhysicalVolume* DetectorConstruction::Construct() {
    G4GeometryManager::GetInstance()->OpenGeometry();
    G4PhysicalVolumeStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4SolidStore::GetInstance()->Clean();

    ConstructMaterials();

    // 1. WORLD
    G4Box* world_s = new G4Box("World", 1.5*m, 1.5*m, 1.5*m);
    auto world_logic = new G4LogicalVolume(world_s, fWorldMaterial, "World");
    auto world_p = new G4PVPlacement(0, G4ThreeVector(), world_logic, "World", 0, false, 0);

    // 2. TRAY (Алюминиевый короб)
    G4double tray_hx = (fProductSizeX + 2*fTrayWallThickness)/2.;
    G4double tray_hy = (fProductSizeY + 2*fTrayWallThickness)/2.;
    G4double tray_hz = (fProductThickness + fTrayWallThickness)/2.;

    G4Box* tray_s = new G4Box("Tray", tray_hx, tray_hy, tray_hz);
    auto tray_logic = new G4LogicalVolume(tray_s, fContainerMaterial, "Tray");
    
    // Ставим лоток так, чтобы Z=0 был центром лотка
    fTrayPhys = new G4PVPlacement(0, G4ThreeVector(0,0,0), tray_logic, "Tray", world_logic, false, 0);

    // 3. PRODUCT (Вложен в лоток)
    G4Box* prod_s = new G4Box("Product", fProductSizeX/2., fProductSizeY/2., fProductThickness/2.);
    fProductLogic = new G4LogicalVolume(prod_s, fFillMaterial, "Product");
    
    // Смещение продукции внутри лотка (на дно)
    G4double prod_z_in_tray = fTrayWallThickness / 2.0;
    new G4PVPlacement(0, G4ThreeVector(0,0, prod_z_in_tray), fProductLogic, "Product", tray_logic, false, 0);

    // Визуализация
    world_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    auto tVis = new G4VisAttributes(G4Color(0.7, 0.7, 0.7, 0.3));
    tVis->SetForceSolid(true);
    tray_logic->SetVisAttributes(tVis);

    auto pVis = new G4VisAttributes(G4Color(0.0, 0.8, 0.0, 0.4));
    pVis->SetForceSolid(true);
    fProductLogic->SetVisAttributes(pVis);

    return world_p;
}

void DetectorConstruction::SetProductThickness(G4double val) {
    fProductThickness = val;
    // Уведомляем Geant4, что геометрия изменилась
    G4RunManager::GetRunManager()->ReinitializeGeometry();
}

} // namespace cube