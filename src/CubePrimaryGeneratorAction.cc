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
    auto particle = G4ParticleTable::GetParticleTable()->FindParticle("e-");
    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0,0,-1));
    fParticleGun->SetParticleEnergy(2.5 * MeV);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() { delete fParticleGun; }

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) {
    auto det = static_cast<const DetectorConstruction*>(
        G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    
    // Рандомизируем X и Y в пределах размеров продукции
    G4double x0 = (G4UniformRand()-0.5) * det->GetProductSizeX();
    G4double y0 = (G4UniformRand()-0.5) * det->GetProductSizeY();
    
    // Считаем высоту источника над поверхностью
    G4double zSurface = (det->GetTrayWallThickness()/2.0) + (det->GetProductThickness()/2.0);
    G4double z0 = zSurface + det->GetSSD();
    
    fParticleGun->SetParticlePosition(G4ThreeVector(x0, y0, z0));
    fParticleGun->GeneratePrimaryVertex(event);
}

} // namespace cube