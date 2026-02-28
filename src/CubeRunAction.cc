#include "CubeRunAction.hh"
#include "CubeDetectorConstruction.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"

namespace cube {

RunAction::RunAction() : G4UserRunAction() {
    auto analysis = G4AnalysisManager::Instance();
    analysis->SetDefaultFileType("root");

    // Раздельные ID для разных типов гистограмм
    analysis->SetFirstH1Id(0);
    analysis->SetFirstH3Id(0);

    // ----------------------------
    // H3 (VDD)
    // ----------------------------
    analysis->CreateH3(
        "vdd",
        "3D Dose Distribution",
        100, -500, 500,
        100, -250, 250,
        100, -100, 100
    );

    // ----------------------------
    // H1 (PDD)
    // ----------------------------
    // 1000 мм диапазон
    analysis->CreateH1(
        "pdd",
        "PDD Profile",
        1000, 0., 1000.
    );
}

RunAction::~RunAction() {}

void RunAction::BeginOfRunAction(const G4Run* run) {
    auto analysis = G4AnalysisManager::Instance();
    G4String fileName = "Output_Run_" + std::to_string(run->GetRunID());
    analysis->OpenFile(fileName);
}

void RunAction::EndOfRunAction(const G4Run*) {
    auto analysis = G4AnalysisManager::Instance();
    analysis->Write();
    analysis->CloseFile();
}

} // namespace cube