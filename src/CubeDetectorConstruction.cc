#include "CubeDetectorConstruction.hh"
#include "CubeDetectorMessenger.hh"
#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4SubtractionSolid.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4RunManager.hh"

namespace cube {

DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction() {
    fTrayWallThickness = 1.5*mm;
    fProductSizeX = 900.0*mm;
    fProductSizeY = 400.0*mm;
    fProductThickness = 48.0*mm; 
    source_surface_distance = 60*cm; 
    fBulkDensity = 0.55 * g/cm3; 
    fMessenger = new DetectorMessenger(this);
}

DetectorConstruction::~DetectorConstruction() { 
    delete fMessenger; 
}

void DetectorConstruction::ConstructMaterials() {
    G4NistManager* nist = G4NistManager::Instance();
    
    fWorldMaterial = G4Material::GetMaterial("G4_AIR", false);
    if(!fWorldMaterial) fWorldMaterial = nist->FindOrBuildMaterial("G4_AIR");

    fContainerMaterial = G4Material::GetMaterial("G4_Al", false);
    if(!fContainerMaterial) fContainerMaterial = nist->FindOrBuildMaterial("G4_Al");

    // Проверка кастомного материала
    fFillMaterial = G4Material::GetMaterial("ProductMaterial", false);
    if (!fFillMaterial) {
        G4Element* elH = nist->FindOrBuildElement("H");
        G4Element* elC = nist->FindOrBuildElement("C");
        G4Element* elO = nist->FindOrBuildElement("O");
        fFillMaterial = new G4Material("ProductMaterial", fBulkDensity, 3);
        fFillMaterial->AddElement(elC, 45*perCent);
        fFillMaterial->AddElement(elH, 7*perCent);
        fFillMaterial->AddElement(elO, 48*perCent);
    }
}

G4VPhysicalVolume* DetectorConstruction::Construct() {
    // Очистка памяти перед перестроением геометрии (важно для /geometry/thickness)
    G4GeometryManager::GetInstance()->OpenGeometry();
    G4PhysicalVolumeStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4SolidStore::GetInstance()->Clean();

    ConstructMaterials();

    // World
    G4Box* world_s = new G4Box("World", 1*m, 1*m, 1*m);
    world_logic = new G4LogicalVolume(world_s, fWorldMaterial, "World");
    auto world_p = new G4PVPlacement(0, G4ThreeVector(), world_logic, "World", 0, false, 0);

    // Tray
    G4double tray_z = fProductThickness + fTrayWallThickness;
    G4Box* tray_outer = new G4Box("TrayOuter", (fProductSizeX + 2*fTrayWallThickness)/2., 
                                               (fProductSizeY + 2*fTrayWallThickness)/2., 
                                               tray_z/2.);
    G4Box* tray_inner = new G4Box("TrayInner", fProductSizeX/2., fProductSizeY/2., fProductThickness/2. + 0.1*mm);
    
    G4double z_shift = fTrayWallThickness / 2.0;
    G4SubtractionSolid* tray_s = new G4SubtractionSolid("Tray", tray_outer, tray_inner, 0, G4ThreeVector(0,0,z_shift));
    fTrayLogic = new G4LogicalVolume(tray_s, fContainerMaterial, "Tray");
    fTrayPhys = new G4PVPlacement(0, G4ThreeVector(0, 0, -source_surface_distance/2.0), fTrayLogic, "Tray", world_logic, false, 0);

    // Product
    G4Box* prod_s = new G4Box("Product", fProductSizeX/2., fProductSizeY/2., fProductThickness/2.);
    fProductLogic = new G4LogicalVolume(prod_s, fFillMaterial, "Product");
    fProductPhys = new G4PVPlacement(0, G4ThreeVector(0,0,z_shift), fProductLogic, "Product", fTrayLogic, false, 0);

    return world_p;
}

void DetectorConstruction::SetProductThickness(G4double val) {
    fProductThickness = val;
    G4RunManager::GetRunManager()->ReinitializeGeometry();
}

} // namespace cube