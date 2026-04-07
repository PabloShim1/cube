#ifndef CubePrimaryGeneratorAction_h
#define CubePrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"

// Предварительное объявление классов Geant4
class G4ParticleGun;
class G4Event;
class G4GenericMessenger; 

namespace cube
{

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction();
    virtual ~PrimaryGeneratorAction() override;

    // Метод генерации первичных частиц
    virtual void GeneratePrimaries(G4Event*) override;

  private:
    G4ParticleGun* fParticleGun;
    G4GenericMessenger* fMessenger; // Объект для управления командами из макроса

    // Флаг переключения режима: true - двустороннее, false - одностороннее
    G4bool fBilateral; 

    // Координаты и параметры источника
    G4double source_x, source_y, source_z; 
    G4double px, py, pz; 
    G4double size_x, size_y, size_z; 
};

}

#endif