#include "CubeDetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"

namespace cube
{

  DetectorConstruction::DetectorConstruction()
  {
    this->margin_x = 5 * cm;
    this->margin_y = 5 * cm;
    this->margin_z = 5 * cm;

    this->cube_size_x = 58 * cm;
    this->cube_size_y = 37 * cm;
    this->cube_size_z = 13 * cm;

    // this->source_surface_distance = 100 * cm;
    this->source_surface_distance = 60*cm-this->cube_size_z;
  }

  DetectorConstruction::~DetectorConstruction()
  {
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void DetectorConstruction::ConstructMaterials()
  {
    G4NistManager *nist = G4NistManager::Instance();
    this->world_material = nist->FindOrBuildMaterial("G4_AIR");
    // this->cube_material = nist->BuildMaterialWithNewDensity("YOU_MATERIAL", "G4_POLYPROPELENE", 1.0 * g / cm3);
    this->cube_material = nist->BuildMaterialWithNewDensity("YOU_MATERIAL", "G4_CELLULOSE_CELLOPHANE", 0.189 * g / cm3);
    
  }

  void DetectorConstruction::ComputeTranslations()
  {
    this->world_size_x = this->margin_x + this->cube_size_x;
    this->world_size_y = this->margin_y + this->cube_size_y;
    this->world_size_z = this->margin_z + this->cube_size_z + this->source_surface_distance;

    this->cube_translate = G4ThreeVector(0, 0, -this->source_surface_distance / 2.0);
  }

  void DetectorConstruction::ConstructSolids()
  {
    this->world_solid = new G4Box("World", this->world_size_x / 2.0, this->world_size_y / 2.0, this->world_size_z / 2.0);
    this->cube_solid = new G4Box("Cube", this->cube_size_x / 2.0, this->cube_size_y / 2.0, this->cube_size_z / 2.0);
  }

  void DetectorConstruction::ConstructLogicals()
  {
    this->world_logic = new G4LogicalVolume(this->world_solid, this->world_material, "World");
    this->cube_logic = new G4LogicalVolume(this->cube_solid, this->cube_material, "Cube");
  }

  void DetectorConstruction::ConstructPhysicals()
  {
    this->world_phys = new G4PVPlacement(
        nullptr,
        G4ThreeVector(),
        this->world_logic,
        "World",
        nullptr,
        false,
        0,
        true);

    this->cube_phys = new G4PVPlacement(
        this->cube_rotation,
        this->cube_translate,
        this->cube_logic,
        "Cube",
        this->world_logic,
        false,
        0,
        true);
  }

  void DetectorConstruction::SetUpVisAttributes()
  {
    G4VisAttributes *world_vis = new G4VisAttributes();
    world_vis->SetForceWireframe(true);
    world_vis->SetVisibility(false);
    this->world_logic->SetVisAttributes(world_vis);

    G4VisAttributes *cube_vis = new G4VisAttributes(G4Colour(0., 0., 1., 0.5));
    cube_vis->SetVisibility(true);
    this->cube_logic->SetVisAttributes(cube_vis);
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  G4VPhysicalVolume *DetectorConstruction::Construct()
  {
    this->ConstructMaterials();
    this->ComputeTranslations();

    this->ConstructSolids();
    this->ConstructLogicals();
    this->ConstructPhysicals();

    this->SetUpVisAttributes();

    return this->world_phys;
  }

}
