#include "CubePrimaryGeneratorAction.hh"
#include "CubeDetectorConstruction.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

namespace cube {
PrimaryGeneratorAction::PrimaryGeneratorAction() {
    fParticleGun = new G4ParticleGun(1);
    fParticleGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle("e-"));
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0,0,-1));
    fParticleGun->SetParticleEnergy(2.5 * MeV);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() { delete fParticleGun; }

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) {
    auto det = static_cast<const DetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    
    // Считаем строго по геттерам из DetectorConstruction
    G4double zCenter = det->GetTrayPhys()->GetTranslation().z() + (det->GetTrayWallThickness()/2.0);
    G4double zSource = zCenter + (det->GetProductThickness()/2.0) + det->GetSSD();
    
    G4double x0 = (G4UniformRand()-0.5) * det->GetProductSizeX();
    G4double y0 = (G4UniformRand()-0.5) * det->GetProductSizeY();

    fParticleGun->SetParticlePosition(G4ThreeVector(x0, y0, zSource));
    fParticleGun->GeneratePrimaryVertex(event);
}
}