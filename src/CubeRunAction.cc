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

    // Фиксируем ID, чтобы они не скакали
    analysis->SetFirstH1Id(0);
    analysis->SetFirstH3Id(0);
}

RunAction::~RunAction() {}

void RunAction::BeginOfRunAction(const G4Run* run) {
    auto analysis = G4AnalysisManager::Instance();
    
    // 1. Очищаем старые дескрипторы
    analysis->Clear();

    // 2. Создаем гистограммы ЗДЕСЬ (как просил руководитель)
    // НО с твоим повышенным разрешением:
    
    // H3 (VDD) - 500 бинов по Z (шаг 0.4 мм)
    analysis->CreateH3(
        "vdd",
        "3D Dose Distribution",
        100, -500, 500,
        100, -250, 250,
        500, -100, 100  
    );

    // H1 (PDD) - 5000 бинов на 1000 мм (шаг 0.2 мм)
    analysis->CreateH1(
        "pdd",
        "PDD Profile",
        5000, 0., 1000.
    );

    // 3. Открываем файл
    G4String fileName = "Output_Run_" + std::to_string(run->GetRunID());
    analysis->OpenFile(fileName);
}

void RunAction::EndOfRunAction(const G4Run*) {
    auto analysis = G4AnalysisManager::Instance();
    // Сохраняем и закрываем
    analysis->Write();
    analysis->CloseFile();
}

} // namespace cube