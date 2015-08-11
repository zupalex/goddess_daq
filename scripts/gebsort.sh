#!/bin/sh

if [ $# -ne 2 ] 
  then
   echo "USAGE: gebmerge.sh <merged_dir> <run>"
  exit 1
fi

RUN=$2
DIR=$1

echo "GEBSort started sorting run $RUN at `date`"
if [ ! -e log ]; then
	mkdir log
fi
if [ ! -e rootfiles ]; then
	mkdir rootfiles
fi
time ./GEBSort_nogeb -input disk $DIR/GEBMerged_run$RUN.gtd_000 -rootfile rootfiles/run$RUN.root RECREATE -chat chatfiles/GEBSort.chat > log/GEBSort_run$RUN.log
echo "GEBSort DONE at `date`"

tail -n 5 log/GEBSort_run$RUN.log

#exit


