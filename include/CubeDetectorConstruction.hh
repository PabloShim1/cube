#ifndef CubeDetectorConstruction_h
#define CubeDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Material.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

namespace cube {
  class DetectorMessenger;

  class DetectorConstruction : public G4VUserDetectorConstruction {
  public:
    DetectorConstruction();
    virtual ~DetectorConstruction();
    virtual G4VPhysicalVolume* Construct();

    // ПРОВЕРЬ ЭТИ СТРОЧКИ - они должны быть здесь!
    G4VPhysicalVolume* GetTrayPhys() const { return fTrayPhys; }
    G4VPhysicalVolume* GetProductPhys() const { return fProductPhys; }
    G4LogicalVolume* GetProductLogic() const { return fProductLogic; }
    
    G4double GetProductThickness() const { return fProductThickness; }
    G4double GetTrayWallThickness() const { return fTrayWallThickness; }
    G4double GetSSD() const { return source_surface_distance; }
    G4double GetProductSizeX() const { return fProductSizeX; }
    G4double GetProductSizeY() const { return fProductSizeY; }

    void SetProductThickness(G4double val);

  private:
    void ConstructMaterials();
    void SetUpVisAttributes();

    DetectorMessenger* fMessenger;

    G4double fProductThickness;
    G4double fTrayWallThickness;
    G4double fProductSizeX;
    G4double fProductSizeY;
    G4double source_surface_distance;
    G4double margin_x, margin_y, margin_z;

    G4VPhysicalVolume* world_phys = nullptr;
    G4LogicalVolume* world_logic = nullptr;
    G4VPhysicalVolume* fTrayPhys = nullptr;
    G4LogicalVolume* fTrayLogic = nullptr;
    G4VPhysicalVolume* fProductPhys = nullptr;
    G4LogicalVolume* fProductLogic = nullptr;

    G4Material* fWorldMaterial = nullptr;
    G4Material* fContainerMaterial = nullptr;
    G4Material* fFillMaterial = nullptr;

    G4ThreeVector cube_translate; 
  };
}
#endif