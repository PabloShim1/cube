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
    
    // Явно начинаем ID с нуля для всех типов
    analysis->SetFirstHistoId(0);

    // СОЗДАЕМ ГИСТОГРАММЫ ОДИН РАЗ В КОНСТРУКТОРЕ
    // Делаем сетку с запасом (например, до 1000 мм), чтобы не менять её на лету
    // H3 (3D доза) -> получит ID 0
    analysis->CreateH3("vdd", "3D Dose", 100, -500, 500, 100, -250, 250, 100, -100, 100); 
    
    // H1 (Глубинное распределение) -> ТОЖЕ получит ID 0
    analysis->CreateH1("pdd", "PDD Profile", 1000, 0, 1000); 
}

RunAction::~RunAction() {}

void RunAction::BeginOfRunAction(const G4Run* run) {
    auto analysis = G4AnalysisManager::Instance();
    
    // Открываем файл. Имя будет Output_Run_0.root, Output_Run_1.root и т.д.
    std::stringstream ss;
    ss << "Output_Run_" << run->GetRunID();
    analysis->OpenFile(ss.str());

    // ВНИМАНИЕ: Мы НЕ вызываем SetH1/SetH3 здесь. 
    // Это предотвращает зависание при 32+ потоках.
}

void RunAction::EndOfRunAction(const G4Run*) {
    auto analysis = G4AnalysisManager::Instance();
    analysis->Write();
    analysis->CloseFile();
}

} // namespace cube