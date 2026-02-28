import uproot
import numpy as np
import pandas as pd
import os

def analyze_runs(directory="build"):
    results = []
    # Ищем файлы в папке build (или текущей)
    files = [f for f in os.listdir(directory) if f.endswith(".root") and "Output_Run_" in f]
    files.sort(key=lambda x: int(x.split('_')[-1].split('.')[0]))

    if not files:
        print("ROOT-файлы не найдены. Проверьте путь.")
        return

    for file_name in files:
        path = os.path.join(directory, file_name)
        try:
            file = uproot.open(path)
            
            # Используем vdd (3D распределение) для более точного анализа
            if "vdd" in file:
                hist = file["vdd"]
            elif "pdd" in file:
                hist = file["pdd"]
            else:
                print(f"Гистограммы не найдены в {file_name}")
                continue

            values = hist.values()
            
            # Игнорируем пустые файлы
            if np.sum(values) == 0: 
                continue

            total_energy = np.sum(values)
            max_dose = np.max(values)
            
            # Берем только те ячейки, где доза > 0, чтобы найти реальный минимум
            active_values = values[values > 0] 
            min_dose = np.min(active_values) if len(active_values) > 0 else 0
            
            uniformity = max_dose / min_dose if min_dose > 0 else 0
            run_id = file_name.split('_')[-1].split('.')[0]

            results.append({
                "File": file_name,
                "Run_ID": int(run_id),
                "Max_Dose": round(max_dose, 5),
                "Min_Dose": round(min_dose, 5),
                "DUR (Max/Min)": round(uniformity, 3),
                "Total_Energy_Sum": round(total_energy, 4)
            })
            
        except Exception as e:
            print(f"Ошибка при чтении {file_name}: {e}")

    # Создаем таблицу
    df = pd.DataFrame(results)
    
    # Сохраняем в CSV (удобно для Excel)
    df.to_csv("dose_summary.csv", index=False)
    
    print("\nАнализ завершен. Результаты:")
    print("=" * 80)
    print(df.to_string(index=False))
    print("=" * 80)
    print("Таблица сохранена в файл: dose_summary.csv")

if __name__ == "__main__":
    # Если запускаешь из корня проекта, укажи папку build
    analyze_runs("build")