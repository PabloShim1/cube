#ifndef CubeDetectorConstruction_h
#define CubeDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

#include "G4Material.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"

#include "G4Box.hh"
#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"

namespace cube
{

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction();
    ~DetectorConstruction() override;

  public:
    G4VPhysicalVolume* Construct() override;

  public:
    inline const G4Box* GetWorldSolid() const noexcept { return world_solid; };
    inline const G4Box* GetCubeSolid() const noexcept { return cube_solid; };

    inline const G4LogicalVolume* GetWorldLogic() const noexcept { return world_logic; };
    inline const G4LogicalVolume* GetCubeLogic() const noexcept { return cube_logic; };

    inline const G4VPhysicalVolume* GetWorldPhys() const noexcept { return world_phys; };
    inline const G4VPhysicalVolume* GetCubePhys() const noexcept { return cube_phys; };

    inline const G4double GetSSD() const noexcept { return source_surface_distance; };

  private:
    void ConstructMaterials();
    void ComputeTranslations(); // здесь же рассчитывается размер мира.

    void ConstructSolids();
    void ConstructLogicals();
    void ConstructPhysicals();
    void SetUpVisAttributes();

  private:
    G4double margin_x, margin_y, margin_z;
    G4double world_size_x, world_size_y, world_size_z;
    G4Material* world_material = nullptr;
    // --
    G4Box* world_solid = nullptr;
    G4LogicalVolume* world_logic = nullptr;
    G4VPhysicalVolume* world_phys = nullptr;

    G4double source_surface_distance;

    G4double cube_size_x, cube_size_y, cube_size_z;
    G4ThreeVector cube_translate;
    G4RotationMatrix* cube_rotation = nullptr;
    G4Material* cube_material = nullptr;
    // --
    G4Box* cube_solid = nullptr;
    G4LogicalVolume* cube_logic = nullptr;
    G4VPhysicalVolume* cube_phys = nullptr;

};

}

#endif
