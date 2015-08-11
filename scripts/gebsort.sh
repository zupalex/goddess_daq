#!/bin/sh

if [ $# -ne 2 ] 
  then
   echo "USAGE: gebsort.sh <merged_dir> <run>"
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
time ./GEBSort_nogeb -input disk $DIR/GEBMerged_run$RUN.gtd_000 -rootfile rootfiles/run$RUN.root RECREATE -chat chatfiles/GEBSort.chat > log/GEBSort_current.log
echo "GEBSort DONE at `date`"

mv log/GEBSort_current.log log/GEBSort_run$RUN.log

tail -n 5 log/GEBSort_run$RUN.log

#exit


