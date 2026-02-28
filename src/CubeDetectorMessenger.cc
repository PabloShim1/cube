#include "CubeDetectorMessenger.hh"
#include "CubeDetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

namespace cube {
  DetectorMessenger::DetectorMessenger(DetectorConstruction* det) : fDetector(det) {
    fDirectory = new G4UIdirectory("/cube/geometry/");
    fDirectory->SetGuidance("Control of the tray geometry");

    fThicknessCmd = new G4UIcmdWithADoubleAndUnit("/cube/geometry/thickness", this);
    fThicknessCmd->SetGuidance("Set the thickness of the product layer");
    fThicknessCmd->SetParameterName("Thickness", false);
    fThicknessCmd->SetRange("Thickness > 0.");
    fThicknessCmd->SetUnitCategory("Length");
    fThicknessCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  }

  DetectorMessenger::~DetectorMessenger() {
    delete fThicknessCmd;
    delete fDirectory;
  }

  void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
    if (command == fThicknessCmd) {
      fDetector->SetProductThickness(fThicknessCmd->GetNewDoubleValue(newValue));
    }
  }
}
