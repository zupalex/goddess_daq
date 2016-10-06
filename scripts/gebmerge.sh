#!/bin/sh
# The data directory may or may not contain the final run folder name
# The merge directory must exist. 

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
RESET='\033[0m'

ReturnError()
{
  echo "USAGE: gebmerge.sh <data_dir> <merge_dir> <run> <options>"
  echo "---------------------------------------------------------"
  echo "OPTIONS LIST:"
  echo "overwrite : force the overwrite of the output merged file if it already exists (won't prompt for it!)"
  echo "noldf     : will not convert and include the ldf file in the lsit of files to merge"
}
  
if [ $# -lt 3 ]; then
   ReturnError
   exit 1
fi

INPUT_DATA_DIR=$1
INPUT_MERGE_DIR=$2

NOLDFFLAG=0
OVERWRITEFLAG=0

for arg in "$@" 
do
COUNTER=$(($COUNTER + 1))

  if [ "$arg" = "noldf" ]; then
  
    NOLDFFLAG=1
    echo "/!\\ The ldf file won't be merged /!\\"

  elif [ "$arg" = "overwrite" ]; then

    OVERWRITEFLAG=1
      echo "/!\\ If the file existed, it has been overwritten /!\\"

  elif [ $COUNTER -gt 3 ]; then

    echo "ONE OR MORE INVALID ARGUMENT"
    ReturnError
    exit 1
    
  fi
done


echo "The following runs will be treated:"

for run in `echo "$3"`
do
	echo "run #$run"
done

for run in `echo "$3"`
do
DATA_DIR=$INPUT_DATA_DIR
MERGE_DIR=$INPUT_MERGE_DIR
RUN=$run

#Check for data folder
if [ ! -e $DATA_DIR ]; then
	printf "${RED}ERROR:${RESET} Data directory $DATA_DIR does not exist!\n"
	exit 2
fi

#Try to automatically add the run folder
if [ -e $DATA_DIR/run$RUN ]; then
	DATA_DIR=$DATA_DIR/run$RUN
fi

#Check for merge folder
if [ ! -e $MERGE_DIR ]; then
	printf "${RED}ERROR:${RESET} Merged directory $MERGE_DIR does not exist!\n"
	exit 3
fi

if [ -e $MERGE_DIR/GEBMerged_run$RUN.gtd_000 -a $OVERWRITEFLAG = 0 ]; then
	printf "${YELLOW}ERROR:${RESET} Merged file already exists! Remove manually to overwrite.\n"
	exit 4
fi

#Create log folder if it exists.
if [ ! -e $MERGE_DIR/log ]; then
	mkdir $MERGE_DIR/log
fi

# data from DGS, new data format (firmware) 9/25/2012

ORNL_FILES=""

#convert ldf to a format that can be merged.
if [ $NOLDFFLAG = 0 ]; then
  if [ -e "$DATA_DIR/run$RUN.ldf" ]; then
    printf "${BLUE}Converting ldf to GEB format.${RESET}\n"
    ORNL_FILES=$MERGE_DIR/.run$RUN.geb
    ./hribfConvert `ls $DATA_DIR/run$RUN.ldf` $MERGE_DIR/.run$RUN.geb
  else
    printf "${YELLOW}WARNING:${RESET} No ORNL ldfs found!\n"
  fi
fi

#Check if previous command was successfull
if [ $? != 0 ]; then 
	exit 1
fi

DFMA_FILES=`ls $DATA_DIR/run_gsfma330_$RUN.* 2>/dev/null`
if [ $? != 0 ]; then
	printf "${YELLOW}WARNING:${RESET} No DFMA files found!\n"
fi
DGS_FILES=`ls $DATA_DIR/run_$RUN.dgs* 2>/dev/null`
if [ $? != 0 ]; then
	printf "${YELLOW}WARNING:${RESET} No DGS files found!\n"
fi

echo "GEBMerge started for run #$RUN at `date`"
#Run the merge program
./GEBMerge chatfiles/GEBMerge.chat $MERGE_DIR/GEBMerged_run$RUN.gtd $DFMA_FILES $DGS_FILES $ORNL_FILES | tee $MERGE_DIR/log/GEBMerge_current.log > $MERGE_DIR/log/GEBMerged_run$RUN.log
	
err=$?
#Remove intermediate converted file
rm -f $MERGE_DIR/.run$RUN.geb

if [ $err = 0 ]; then
	printf "${GREEN}GEBMerge DONE at `date`${RESET}\n"
else
	printf "${RED}GEBMerge FAILED at `date`${RESET}\n"
fi

#tail $MERGE_DIR/log/GEBMerge_run$RUN.log -n 3
done

exit


