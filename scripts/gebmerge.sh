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
	./hribfConvert `ls $DIR/run$RUN.ldf` run$RUN.geb
	if [ $? != 0 ]; then 
		exit 1
	fi
  ./GEBMerge GEBMerge.chat Merged/GEBMerged_run$RUN.gtd `ls $DIR/run_gsfma330_$RUN.*` `ls $DIR/run_$RUN.dgs*` `ls run$RUN.geb` > LOG_FILES/GEBMerge_run$RUN.log
echo "GEBMerge DONE at `date`"

exit


