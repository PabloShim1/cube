#ifndef CubePrimaryGeneratorAction_h
#define CubePrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;
class G4Box;

namespace cube
{

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction();
    ~PrimaryGeneratorAction() override;

  public:
    void GeneratePrimaries(G4Event*) override;

  private:
    G4double source_x, source_y, source_z; // координаты центра плоского параллельного источника.
    G4double px, py, pz; // направление излучения плоского параллельного источника.
    G4double size_x, size_y, size_z; // размер плоского параллельного источника.

    G4ParticleGun* fParticleGun = nullptr;
};

}

#endif
