#!/bin/bash
#SBATCH --job-name="G4d2o"
#SBATCH --nodes=1
#SBATCH --cpus-per-task=1
#SBATCH --partition=red
#SBATCH --mem-per-cpu=2000M
#SBATCH --time=120:00:00

# Arguments from submit_jobs.py
G4D2O_DIR=$4

# SIMULTION RUN SETTINGS: READ FROM beanOn.dat
RUNNUM=$1
NUMEVENTS=$2
OUT_DIR=$3


# Setting up some directories
mkdir -p "${G4D2O_DIR}/${OUT_DIR}"
USER_DIR="/home/"${USER}
JOB_SUBMIT_DIR=$(pwd)


## Source file that converts from slurm to PBS
source /usr/local/slurm/env_to_slurm.sh
source ${USER_DIR}/.bashrc
cd /scratch/slurm_${SLURM_JOB_ID}

DATE=$(date '+%m-%d-%Y')
LOG_DIR="${JOB_SUBMIT_DIR}/slurm-${SLURM_JOB_ID}-${DATE}"
mkdir -p ${LOG_DIR}


## Configure server that has files we need
SERVER=qcdegb
WORKDIR=/scratch/slurm_${SLURM_JOB_ID}
SCP="cp "
PERMDIR=${G4D2O_DIR}

SERVPERMDIR=${PERMDIR}
SERVPERMBINDIR=${PERMDIR}


##Source the environment
source ${PERMDIR}/jobSetup.sh

## Print some useful information out
/home/cornejo/pbs/output_job_info.sh

echo server is $SERVER
echo workdir is $WORKDIR
echo permdir is $PERMDIR
echo servpermdir is $SERVPERMDIR
echo hostname of node is `hostname`
echo ------------------------------------------------------
#echo -n 'Job is running on node '; cat NODEFI
echo ------------------------------------------------------
echo ' '
echo ' '
RUNMACRO="run.mac"
## List of macros that are needed
DEPENDSFILE="depends.dat"


currentstage='NONE'
printbanner()
{
  printf "\n############ %6s: %-20s #############\n" "$1" "$2"
}
printstart()
{
  printbanner "START" "$currentstage"
}

printend()
{
  printbanner "END" "$currentstage"
}


###############################################################
#                                                             #
#    Transfer files from server to local disk.                #
#                                                             #
###############################################################
stagein()
{

  currentstage='STAGE IN'
  printstart
  echo "In Work directory ${WORKDIR}"


  mkdir -p ${WORKDIR}
  cd ${WORKDIR}

  ## Copy over the binary
  ${SCP} ${SERVPERMBINDIR}/G4d2o .
  mkdir opticalData
  ${SCP} ${SERVPERMBINDIR}/opticalData/* opticalData/
  ${SCP} ${SERVPERMBINDIR}/beamOn0.dat .
  ${SCP} ${SERVPERMBINDIR}/beamOn.dat .

  echo "Directory listing of ${WORKDIR}"
  ls

  printend
}

############################################################
#                                                          #
#    Execute the run.  Do not run in the background.       #
#                                                          #
############################################################

runprogram()
{
  echo ' '
  echo ' ############ START: RUN PROGRAM #############'
  echo ' '
  cd ${WORKDIR}
  echo "In directory `pwd`"

  echo "Running ${NUMEVENTS} events"

  # sleep 120
  #find .
  cat run.mac
  echo "./G4d2o"
  ./G4d2o

  echo ' '
  echo ' ############ END: RUN PROGRAM #############'
  echo ' '
}

###########################################################
#                                                         #
#   Copy necessary files back to permanent directory.     #
#                                                         #
###########################################################

stageout()
{
  echo ' '
  echo ' ############ START: STAGE OUT #############'
  echo ' '
  cd ${WORKDIR}
  ls

  ## Copy output rootfile
  rootprefix="Sim_D2ODetector"
  rootfile="${rootprefix}${RUNNUM}.root"
  #${SCP} ${rootfile} ${SERVPERMDIR}/${OUT_DIR}${rootfile}
  mv "${OUT_DIR}/${rootfile}" "${SERVPERMDIR}/${OUT_DIR}/${rootfile}"
  echo "Output file: ${SERVPERMDIR}/${OUT_DIR}/${rootfile}"

  mv "${JOB_SUBMIT_DIR}/slurm-${SLURM_JOB_ID}.out" ${LOG_DIR}
  echo ' '
  echo ' ############ END: STAGE OUT #############'
  echo ' '
}

#####################################################################
#                                                                   #
#  React to early termination signal.                               #
#                                                                   #
#####################################################################

early()
{
  echo ' '
  echo ' ############ WARNING:  EARLY TERMINATION #############'
  echo ' '
}

trap 'early; stageout' 2 9 15


##################################################
#                                                #
#   Staging in, running the job, and staging out #
#   were specified above as functions.  Now      #
#   call these functions to perform the actual   #
#   file transfers and program execution.        #
#                                                #
##################################################

stagein
runprogram
stageout

###############################################################
#                                                             #
#   The epilogue script automatically deletes the directory   #
#   created on the local disk (including all files contained  #
#   therein.                                                  #
#                                                             #
###############################################################

exit 0

