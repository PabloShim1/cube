#include "CubePrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"

namespace cube {

PrimaryGeneratorAction::PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction()
{
    fParticleGun = new G4ParticleGun(1);

    auto particleTable = G4ParticleTable::GetParticleTable();
    auto particle = particleTable->FindParticle("e-");  // или gamma

    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticleEnergy(6.0 * MeV);

    // ----------------------------
    // ПОЗИЦИЯ (над фантомом)
    // ----------------------------
    fParticleGun->SetParticlePosition(
        G4ThreeVector(0., 0., 600. * mm)
    );

    // ----------------------------
    // НАПРАВЛЕНИЕ (СВЕРХУ ВНИЗ)
    // ----------------------------
    fParticleGun->SetParticleMomentumDirection(
        G4ThreeVector(0., 0., -1.)
    );
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
    delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) {
    fParticleGun->GeneratePrimaryVertex(event);
}

} // namespace cube