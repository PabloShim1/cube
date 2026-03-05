import uproot
import numpy as np
import pandas as pd
import os
import argparse

def analyze_runs(directory="build", mode="pdd"):
    # --- НАСТРОЙКИ ПУЧКА ---
    N_PARTICLES = 1000000  # Сюда впиши число из /run/beamOn
    BEAM_ENERGY_MEV = 5  # Энергия одного электрона в МэВ
    TOTAL_BEAM_ENERGY = N_PARTICLES * BEAM_ENERGY_MEV

    results = []
    if not os.path.exists(directory):
        print(f"Ошибка: Директория {directory} не найдена!")
        return

    files = [f for f in os.listdir(directory) if f.endswith(".root") and "Output_Run_" in f]
    files.sort(key=lambda x: int(x.split('_')[-1].split('.')[0]))

    for file_name in files:
        path = os.path.join(directory, file_name)
        try:
            file = uproot.open(path)
            if mode not in file: continue
                
            hist = file[mode]
            values = np.array(hist.values())
            total_mev = float(np.sum(values))
            
            if total_mev == 0: continue

            # 1. Максимум и Минимум
            max_mev = float(np.max(values))
            nonzero_indices = np.nonzero(values.flatten())[0]
            min_mev = float(values.flatten()[nonzero_indices[-1]]) if len(nonzero_indices) > 0 else 0.0

            # 2. DUR (Max/Min)
            dur = max_mev / min_mev if min_mev > 0 else float('inf')

            # 3. Efficiency (%) - КПД использования пучка
            efficiency = (total_mev / TOTAL_BEAM_ENERGY) * 100

            results.append({
                "File": file_name,
                "Mode": mode.upper(),
                "DUR (Max/Min)": dur,
                "Total E (MeV)": total_mev,
                "Efficiency (%)": efficiency
            })
            
        except Exception as e:
            print(f"Ошибка при обработке {file_name}: {e}")

    df = pd.DataFrame(results)
    pd.options.display.float_format = '{:.2f}'.format
    
    print(f"\nАНАЛИЗ: {mode.upper()} (Эффективность и Равномерность)")
    print("=" * 115)
    # Форматируем DUR и Efficiency
    formatters = {
        'DUR (Max/Min)': '{:,.2f}'.format,
        'Efficiency (%)': '{:,.2f}%'.format
    }
    print(df.to_string(index=False, formatters=formatters))
    print("=" * 115)
    
    df.to_csv(f"efficiency_report_{mode}.csv", index=False)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--mode", choices=["pdd", "vdd"], default="pdd")
    parser.add_argument("--dir", default="build")
    args = parser.parse_args()
    analyze_runs(directory=args.dir, mode=args.mode)