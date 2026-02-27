import os

template ="""# THIS MACRO IS GENERATED AUTOMATICALLY,
# DO NOT MODIFY!

/run/initialize

/gun/particle e-
/gun/energy {energy} MeV

/run/printProgress {printProgress}
/run/beamOn {beamOn}
"""

beamOn = int(1e7)
# TODO проверка, что мы находимся в директории build

for e in range(11, 20):
    for ee in range(10):
        E = f"{e}.{ee}"
        directory = f"{E}-MeV"
        try:
            os.mkdir(directory)
        except:
            print(f"directory {directory} already exists")

        os.chdir(directory)
        print("current directory: ", os.getcwd())
        with open("run.mac", 'w') as f:
            f.write(template.format(energy=E, beamOn=beamOn, printProgress=beamOn//10))
        cmd = f"../cube ./run.mac"
        print(cmd)
        os.system(cmd)
        os.chdir("../")
