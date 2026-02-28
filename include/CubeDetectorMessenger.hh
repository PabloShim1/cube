#ifndef CubeDetectorMessenger_h
#define CubeDetectorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;

namespace cube {
  class DetectorConstruction;

  class DetectorMessenger : public G4UImessenger {
  public:
    DetectorMessenger(DetectorConstruction* det);
    ~DetectorMessenger();

    void SetNewValue(G4UIcommand* command, G4String newValue) override;

  private:
    DetectorConstruction* fDetector;
    G4UIdirectory* fDirectory;
    G4UIcmdWithADoubleAndUnit* fThicknessCmd;
  };
}
#endif
