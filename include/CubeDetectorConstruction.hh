#ifndef CubeDetectorConstruction_h
#define CubeDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4Material;

namespace cube {
  class DetectorMessenger;

  class DetectorConstruction : public G4VUserDetectorConstruction {
  public:
    DetectorConstruction();
    virtual ~DetectorConstruction();
    virtual G4VPhysicalVolume* Construct();

    // Геттеры для других классов (Generator, Stepping, Messenger)
    G4LogicalVolume* GetProductLogic() const { return fProductLogic; }
    G4VPhysicalVolume* GetTrayPhys() const { return fTrayPhys; }
    
    G4double GetProductThickness() const { return fProductThickness; }
    G4double GetTrayWallThickness() const { return fTrayWallThickness; }
    G4double GetSSD() const { return fSSD; }
    G4double GetProductSizeX() const { return fProductSizeX; }
    G4double GetProductSizeY() const { return fProductSizeY; }

    void SetProductThickness(G4double val);

  private:
    void ConstructMaterials();

    DetectorMessenger* fMessenger;

    G4double fProductThickness;
    G4double fTrayWallThickness;
    G4double fProductSizeX;
    G4double fProductSizeY;
    G4double fSSD;
    G4double fBulkDensity;

    G4LogicalVolume* fProductLogic = nullptr;
    G4VPhysicalVolume* fTrayPhys = nullptr;
    
    G4Material* fWorldMaterial = nullptr;
    G4Material* fContainerMaterial = nullptr;
    G4Material* fFillMaterial = nullptr;
  };
}
#endif