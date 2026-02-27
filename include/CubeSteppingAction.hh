#ifndef CubeSteppingAction_h
#define CubeSteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class G4LogicalVolume;

namespace cube
{

class EventAction;

class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction(EventAction* eventAction);
    ~SteppingAction() override;

    // method from the base class
    void UserSteppingAction(const G4Step*) override;

  private:
    EventAction* fEventAction = nullptr;
    const G4LogicalVolume* fScoringVolume = nullptr;
};

}

#endif
