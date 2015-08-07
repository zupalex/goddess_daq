#!/bin/sh

if [ $# -ne 2 ] 
  then
   echo "USAGE: gebmerge.sh <data_dir> <run>"
  exit 1
fi

RUN=$2
DIR=$1

# data from DGS, new data format (firmware) 9/25/2012

echo "GEBMerge started at `date`"

if [ ! -e gebfiles ]; then
	mkdir gebfiles
fi

./hribfConvert `ls $DIR/run$RUN.ldf` gebfiles/run$RUN.geb

if [ $? != 0 ]; then 
	exit 1
fi

if [ ! -e log ]; then
	mkdir log
fi
if [ ! -e merged ]; then
	mkdir merged
fi
./GEBMerge chatfiles/GEBMerge.chat merged/GEBMerged_run$RUN.gtd \
	`ls $DIR/run_gsfma330_$RUN.*` `ls $DIR/run_$RUN.dgs*` `ls gebfiles/run$RUN.geb` \
	 > log/GEBMerge_run$RUN.log

echo "GEBMerge DONE at `date`"

exit


