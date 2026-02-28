#include "CubeRunAction.hh"
#include "CubeDetectorConstruction.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"
#include <sstream>

namespace cube {

RunAction::RunAction() : G4UserRunAction() {
    auto analysis = G4AnalysisManager::Instance();
    analysis->SetDefaultFileType("root");
    
    // Начинаем отсчет с 0
    analysis->SetFirstHistoId(0);

    // Создаем гистограммы один раз. Они получат ID 0.
    analysis->CreateH3("vdd", "3D Dose", 100, -500, 500, 100, -250, 250, 100, -100, 100); 
    analysis->CreateH1("pdd", "PDD Profile", 1000, 0, 1000); 
}

RunAction::~RunAction() {}

void RunAction::BeginOfRunAction(const G4Run* run) {
    auto analysis = G4AnalysisManager::Instance();
    
    std::stringstream ss;
    ss << "Output_Run_" << run->GetRunID();
    analysis->OpenFile(ss.str());
    
    // В BeginOfRunAction больше НИЧЕГО не меняем, чтобы не сломать многопоточность
}

void RunAction::EndOfRunAction(const G4Run*) {
    auto analysis = G4AnalysisManager::Instance();
    analysis->Write();
    analysis->CloseFile();
}

} // namespace cube