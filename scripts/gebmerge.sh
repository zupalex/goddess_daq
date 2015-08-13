#!/bin/sh
# The data directory may or may not contain the final run folder name
# The merge directory must exist. 

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
RESET='\033[0m'

if [ $# -ne 3 ] 
  then
   echo "USAGE: gebmerge.sh <data_dir> <merge_dir> <run>"
  exit 1
fi

RUN=$3
MERGE_DIR=$2
DATA_DIR=$1

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

#Create log folder if it exists.
if [ ! -e log ]; then
	mkdir log
fi

# data from DGS, new data format (firmware) 9/25/2012

#convert ldf to a format that can be merged.
if [ -e "$DATA_DIR/run$RUN.ldf" ]; then
	printf "${BLUE}Converting ldf to GEB format.${RESET}\n"
	ORNL_FILES=.run$RUN.geb
	./hribfConvert `ls $DATA_DIR/run$RUN.ldf` .run$RUN.geb
else
	printf "${YELLOW}WARNING:${RESET} No ORNL ldfs found!\n"
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

echo "GEBMerge started at `date`"
#Run the merge program
./GEBMerge chatfiles/GEBMerge.chat $MERGE_DIR/GEBMerged_run$RUN.gtd \
	$DFMA_FILES $DGS_FILES $ORNL_FILES \
	 > log/GEBMerge_run$RUN.log

err=$?
#Remove intermediate converted file
rm -f .run$RUN.geb
if [ $err == 0 ]; then
	printf "${GREEN}GEBMerge DONE at `date`${RESET}\n"
else
	printf "${RED}GEBMerge FAILED at `date`${RESET}\n"
fi

tail log/GEBMerge_run$RUN.log -n 3

exit


