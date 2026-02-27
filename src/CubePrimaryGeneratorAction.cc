#include "CubeDetectorConstruction.hh"
#include "CubePrimaryGeneratorAction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

namespace cube
{

  PrimaryGeneratorAction::PrimaryGeneratorAction()
  {
    const DetectorConstruction *detConstruction = static_cast<const DetectorConstruction *>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());

    this->size_z = 0;
    this->size_x = detConstruction->GetCubeSolid()->GetXHalfLength();
    this->size_y = detConstruction->GetCubeSolid()->GetYHalfLength();

    auto cube_translate = detConstruction->GetCubePhys()->GetTranslation();
    this->source_x = cube_translate.getX();
    this->source_y = cube_translate.getY();
    this->source_z = cube_translate.getZ() + detConstruction->GetCubeSolid()->GetZHalfLength() + detConstruction->GetSSD();

    this->px = +0;
    this->py = +0;
    this->pz = -1;

    fParticleGun = new G4ParticleGun(1);
    fParticleGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle("e-"));
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(this->px, this->py, this->pz));
    fParticleGun->SetParticleEnergy(10. * MeV);
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  PrimaryGeneratorAction::~PrimaryGeneratorAction()
  {
    delete fParticleGun;
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent)
  {
    fParticleGun->SetParticlePosition(
        G4ThreeVector(
            this->source_x + (2 * G4UniformRand() - 1) * this->size_x,
            this->source_y + (2 * G4UniformRand() - 1) * this->size_y,
            this->source_z));

    fParticleGun->GeneratePrimaryVertex(anEvent);
  }

}
