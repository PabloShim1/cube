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
    
    // Направление строго вниз (к дну контейнера)
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0,0,-1));
    fParticleGun->SetParticleEnergy(5 * MeV);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() { 
    delete fParticleGun; 
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) {
    auto det = static_cast<const DetectorConstruction*>(
        G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    
    // Равномерное распределение по площади поверхности продукции
    G4double x0 = (G4UniformRand()-0.5) * det->GetProductSizeX();
    G4double y0 = (G4UniformRand()-0.5) * det->GetProductSizeY();
    
    // В новой геометрии верхняя грань (вход пучка) находится в +fProductThickness/2
    G4double zSurface = det->GetProductThickness() / 2.0;
    
    // Источник находится на высоте SSD над верхней гранью
    G4double z0 = zSurface + det->GetSSD();
    
    fParticleGun->SetParticlePosition(G4ThreeVector(x0, y0, z0));
    fParticleGun->GeneratePrimaryVertex(event);
}

} // namespace cube