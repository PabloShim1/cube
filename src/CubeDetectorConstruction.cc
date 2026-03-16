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

    // Полипропилен (C3H6)n с насыпной плотностью 0.55 г/см3
    if (!G4Material::GetMaterial("Polypropylene_Powder", false)) {

        fFillMaterial = new G4Material("Polypropylene_Powder", fBulkDensity, 2);

        fFillMaterial->AddElement(nist->FindOrBuildElement("C"), 85.7*perCent);
        fFillMaterial->AddElement(nist->FindOrBuildElement("H"), 14.3*perCent);

    }
    else {

        fFillMaterial = G4Material::GetMaterial("Polypropylene_Powder");
    }
}

G4VPhysicalVolume* DetectorConstruction::Construct() {

    G4GeometryManager::GetInstance()->OpenGeometry();
    G4PhysicalVolumeStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4SolidStore::GetInstance()->Clean();

    ConstructMaterials();

    // WORLD
    G4Box* world_s = new G4Box("World", 1.5*m, 1.5*m, 1.5*m);
    auto world_logic = new G4LogicalVolume(world_s, fWorldMaterial, "World");
    auto world_p = new G4PVPlacement(0, G4ThreeVector(), world_logic, "World", 0, false, 0);

    // Технический зазор
    G4double gap = 10.0*micrometer;

    // PRODUCT
    G4Box* prod_s = new G4Box("Product_Solid",
                               fProductSizeX/2.,
                               fProductSizeY/2.,
                               fProductThickness/2.0);

    fProductLogic = new G4LogicalVolume(prod_s, fFillMaterial, "Product_Logic");

    new G4PVPlacement(0,
                      G4ThreeVector(0,0,0),
                      fProductLogic,
                      "Product_Phys",
                      world_logic,
                      false,
                      0);

    // TRAY BOTTOM
    G4double bottom_hz = fTrayWallThickness / 2.0;

    G4Box* bottom_s = new G4Box("TrayBottom",
                                (fProductSizeX + 2*fTrayWallThickness)/2.,
                                (fProductSizeY + 2*fTrayWallThickness)/2.,
                                bottom_hz);

    auto bottom_logic = new G4LogicalVolume(bottom_s,
                                            fContainerMaterial,
                                            "TrayBottom_Logic");

    G4double bottom_z_pos = -(fProductThickness/2.0 + gap + bottom_hz);

    new G4PVPlacement(0,
                      G4ThreeVector(0, 0, bottom_z_pos),
                      bottom_logic,
                      "TrayBottom_Phys",
                      world_logic,
                      false,
                      0);

    // WALLS X
    G4Box* wallX_s = new G4Box("TrayWallX",
                               fTrayWallThickness/2.0,
                               fProductSizeY/2.0,
                               fProductThickness/2.0);

    auto wallX_logic = new G4LogicalVolume(wallX_s,
                                           fContainerMaterial,
                                           "TrayWallX_Logic");

    new G4PVPlacement(0,
                      G4ThreeVector(-(fProductSizeX/2.0 + gap + fTrayWallThickness/2.0), 0, 0),
                      wallX_logic,
                      "WallX1",
                      world_logic,
                      false,
                      0);

    new G4PVPlacement(0,
                      G4ThreeVector((fProductSizeX/2.0 + gap + fTrayWallThickness/2.0), 0, 0),
                      wallX_logic,
                      "WallX2",
                      world_logic,
                      false,
                      0);

    // WALLS Y
    G4Box* wallY_s = new G4Box("TrayWallY",
                               (fProductSizeX + 2*fTrayWallThickness)/2.0,
                               fTrayWallThickness/2.0,
                               fProductThickness/2.0);

    auto wallY_logic = new G4LogicalVolume(wallY_s,
                                           fContainerMaterial,
                                           "TrayWallY_Logic");

    new G4PVPlacement(0,
                      G4ThreeVector(0, -(fProductSizeY/2.0 + gap + fTrayWallThickness/2.0), 0),
                      wallY_logic,
                      "WallY1",
                      world_logic,
                      false,
                      0);

    new G4PVPlacement(0,
                      G4ThreeVector(0, (fProductSizeY/2.0 + gap + fTrayWallThickness/2.0), 0),
                      wallY_logic,
                      "WallY2",
                      world_logic,
                      false,
                      0);

    // VISUALIZATION
    world_logic->SetVisAttributes(G4VisAttributes::GetInvisible());

    auto tVis = new G4VisAttributes(G4Color(1.0, 0.0, 0.0, 0.5));
    tVis->SetForceSolid(true);

    bottom_logic->SetVisAttributes(tVis);
    wallX_logic->SetVisAttributes(tVis);
    wallY_logic->SetVisAttributes(tVis);

    auto pVis = new G4VisAttributes(G4Color(1.0, 1.0, 1.0, 0.4));
    pVis->SetForceSolid(true);

    fProductLogic->SetVisAttributes(pVis);

    return world_p;
}

void DetectorConstruction::SetProductThickness(G4double val) {

    fProductThickness = val;

    G4RunManager::GetRunManager()->ReinitializeGeometry();
}

} // namespace cube