import os

'''

Submit batch job to slurm. Run this at ernest node.
This script reads beamOn.dat file from G4d2o/ directory and gets the run number, number of primaries (events), and output file directory. And then runs `submit_slurm.sh` script.

How to use:
    1. Change `beamOn.dat` in `G4d2o/` directory as necessary.
    2. Change line 15 of this script `G4d2oDir`.
    3. Run by: `python3 submit_jobs.py` at ernest node.

'''

G4d2oDir = '/home/kyungmip/G4d2o/' 


# Read run number, number of primaries, output dir from beamOn.dat
beamOnFile = G4d2oDir + 'beamOn.dat'

f = open(beamOnFile, mode = 'r')
lines = f.readlines()

beamDic = {'nRun': 0, 'nEvents': 0, 'outDir': 0, 'simDir': G4d2oDir }

for line in lines:
    line = line.split()
    line = [i.strip() for i in line]

    if 'Run-number' in line[1]:
        beamDic['nRun'] = line[0]
    elif 'primaries-to-generate' in line[1]:
        beamDic['nEvents'] = line[0]
    elif 'outputDir' in line[1]:
        beamDic['outDir'] = line[0]

# if the path is not absolute path, make it absolute path
if not G4d2oDir in beamDic['outDir']:
    beamDic['outDir'] = G4d2oDir + beamDic['outDir'] + '/'

# Run submit_slurm.sh script
runCode = 'sbatch submit_slurm.sh {nRun} {nEvents} {outDir} {simDir}'.format(**beamDic)
os.system(runCode)

print('\nSubmitted job for G4d2o simulation. Simulation settings defined in {}'.format(beamOnFile))
print('\nRun number: {}'.format(beamDic['nRun']))
print('Number of events: {}'.format(beamDic['nEvents']))
print('Simulation output file directory: {}\n'.format(beamDic['outDir']))
