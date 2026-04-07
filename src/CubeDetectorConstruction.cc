#include "CubeDetectorConstruction.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

namespace cube {
  DetectorConstruction::DetectorConstruction() {
    cube_size_x = 10.0*cm; 
    cube_size_y = 10.0*cm; 
    cube_size_z = 10.0*cm;
    total_mass = 100.0*g;
    source_surface_distance = 60.0*cm - (cube_size_z/2.0);
  }

  DetectorConstruction::~DetectorConstruction() {} 

  void DetectorConstruction::ConstructMaterials() {
    G4NistManager* nist = G4NistManager::Instance();
    world_material = nist->FindOrBuildMaterial("G4_AIR");
    
    // Просто находим стандартную воду в базе NIST
    cube_material = nist->FindOrBuildMaterial("G4_WATER");
  }

  G4VPhysicalVolume* DetectorConstruction::Construct() {
    ConstructMaterials();
    
    G4double world_z = 2.0 * (source_surface_distance + cube_size_z);
    world_solid = new G4Box("World", cube_size_x, cube_size_y, world_z/2);
    world_logic = new G4LogicalVolume(world_solid, world_material, "World");
    world_phys = new G4PVPlacement(0, G4ThreeVector(), world_logic, "World", 0, false, 0);

    cube_solid = new G4Box("Cube", cube_size_x/2, cube_size_y/2, cube_size_z/2);
    cube_logic = new G4LogicalVolume(cube_solid, cube_material, "Cube");
    
    // Визуализация
    G4VisAttributes* visAtt = new G4VisAttributes(G4Colour(0.3, 0.8, 0.1, 0.5));
    visAtt->SetForceSolid(true);
    cube_logic->SetVisAttributes(visAtt);

    cube_phys = new G4PVPlacement(0, G4ThreeVector(0,0,0), cube_logic, "Cube", world_logic, false, 0);
    
    return world_phys;
  }
}