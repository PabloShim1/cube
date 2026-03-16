import uproot
import numpy as np
import pandas as pd
import os
import sys

def analyze_runs(directory="."):
    # Если ты передал папку в аргументах (например: python3 analyze.py build)
    if len(sys.argv) > 1 and not sys.argv[1].startswith('-'):
        directory = sys.argv[1]

    N_PARTICLES = 10000000 
    BEAM_ENERGY_MEV = 5
    TOTAL_BEAM_ENERGY = N_PARTICLES * BEAM_ENERGY_MEV
    
    thicknesses = [5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51]
    results = []
    
    # Ищем файлы .root
    print(f"Ищу файлы в папке: {os.path.abspath(directory)}")
    files = [f for f in os.listdir(directory) if f.endswith(".root") and "Output_Run_" in f]
    files.sort(key=lambda x: int(x.split('_')[-1].split('.')[0]))

    if not files:
        print(f"Ошибка: Файлы .root не найдены в '{directory}'")
        print("Попробуй запустить так: python3 analyze.py build")
        return

    for i, file_name in enumerate(files):
        path = os.path.join(directory, file_name)
        try:
            current_thick = thicknesses[i] if i < len(thicknesses) else thicknesses[-1]
            file = uproot.open(path)
            
            hist = file["pdd"]
            values, edges = hist.to_numpy()
            bin_width = edges[1] - edges[0]
            
            limit_bin = int(current_thick / bin_width)
            product_data = values[:limit_bin]
            useful_data = product_data[product_data > 0]
            
            if len(useful_data) == 0: continue

            v_max = np.percentile(useful_data, 95)
            v_min = np.percentile(useful_data, 5)
            
            dur = v_max / v_min if v_min > 0 else float('inf')
            efficiency = (np.sum(product_data) / TOTAL_BEAM_ENERGY) * 100

            results.append({
                "File": file_name,
                "Thick(mm)": current_thick,
                "DUR (95/5%)": round(dur, 3),
                "Efficiency (%)": round(efficiency, 2),
                "Max Dose": round(np.max(product_data), 2)
            })
        except Exception as e:
            print(f"Ошибка в файле {file_name}: {e}")

    # Создаем таблицу
    df = pd.DataFrame(results)
    
    # ВЫВОД В ТЕРМИНАЛ
    print("\n" + "="*80)
    print(df.to_string(index=False))
    print("="*80)

    # ЗАПИСЬ В CSV
    output_name = "simulation_results.csv"
    df.to_csv(output_name, index=False)
    print(f"\n[ГОТОВО] Таблица сохранена в файл: {output_name}")

if __name__ == "__main__":
    analyze_runs()