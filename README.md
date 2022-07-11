# G4d2o
Geant4 simulation for heavy water-based Cerenkov detector

# batch
**submit_jobs.py**

```
python3 submit_jobs.py
```


This script submits batch job with slurm. It reads `beamOn.dat` file from `G4d2o/` directory and takes the run number, number of events, and output file directory, and then runs `submit_slurm.sh` script with these parameters.


**How to use**
1. Change `beamOn.dat` in `G4d2o/` directory as necessary.
2. Change `line 15` of this script: change `G4d2oDir` variable as necessary.
3. At `ernest` node, run this script by `python3 submit_jobs.py`


