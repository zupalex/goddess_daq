#!/bin/sh
# The data directory may or may not contain the final run folder name
# The merge directory must exist. 

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
	echo "Data directory $DATA_DIR does not exist!"
	exit 2
fi

#Try to automatically add the run folder
if [ -e $DATA_DIR/run$RUN ]; then
	DATA_DIR=$DATA_DIR/run$RUN
fi

#Check for merge folder
if [ ! -e $MERGE_DIR ]; then
	echo "Merged directory $MERGE_DIR does not exist!"
	exit 3
fi

#Create log folder if it exists.
if [ ! -e log ]; then
	mkdir log
fi

# data from DGS, new data format (firmware) 9/25/2012

#convert ldf to a format that can be merged.
if [ -e "$DATA_DIR/run$RUN.ldf" ]; then
	echo "Converting ldf to GEB format."
	ORNL_FILES=.run$RUN.geb
	./hribfConvert `ls $DATA_DIR/run$RUN.ldf` .run$RUN.geb
else
	echo "WARNING: No ORNL ldfs found!"
fi

#Check if previous command was successfull
if [ $? != 0 ]; then 
	exit 1
fi

DFMA_FILES=`ls $DATA_DIR/run_gsfma330_$RUN.* 2>/dev/null`
if [ $? != 0 ]; then
	echo "WARNING: No DFMA files found!"
fi
DGS_FILES=`ls $DATA_DIR/run_$RUN.dgs* 2>/dev/null`
if [ $? != 0 ]; then
	echo "WARNING: No DGS files found!"
fi

echo "GEBMerge started at `date`"
#Run the merge program
./GEBMerge chatfiles/GEBMerge.chat $MERGE_DIR/GEBMerged_run$RUN.gtd \
	$DFMA_FILES $DGS_FILES $ORNL_FILES \
	 > log/GEBMerge_run$RUN.log

#Remove intermediate converted file
rm -f .run$RUN.geb

echo "GEBMerge DONE at `date`"

tail log/GEBMerge_run$RUN.log -n 3

exit


