#include "CubePrimaryGeneratorAction.hh"
#include "CubeDetectorConstruction.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "Randomize.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"

namespace cube {
  PrimaryGeneratorAction::PrimaryGeneratorAction() : fBilateral(true) {
    fParticleGun = new G4ParticleGun(1);
    fParticleGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle("e-"));
    fParticleGun->SetParticleEnergy(10.*MeV);
  }

  PrimaryGeneratorAction::~PrimaryGeneratorAction() {
    delete fParticleGun;
  }

  void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
    auto det = static_cast<const DetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    G4double half_z = det->GetCubeThickness()/2.0;
    G4double ssd = det->GetSSD();

    G4double z_gun = half_z + ssd; 
    G4double current_z = z_gun;
    G4double pz = -1.0; 

    if (fBilateral && (anEvent->GetEventID() % 2 != 0)) {
        current_z = -z_gun; 
        pz = 1.0;           
    }

    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0,0,pz));
    G4double hx = det->GetCubeSolid()->GetXHalfLength();
    G4double hy = det->GetCubeSolid()->GetYHalfLength();
    fParticleGun->SetParticlePosition(G4ThreeVector((2*G4UniformRand()-1)*hx, (2*G4UniformRand()-1)*hy, current_z));
    fParticleGun->GeneratePrimaryVertex(anEvent);
  }
}