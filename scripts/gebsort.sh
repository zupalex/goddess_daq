#!/bin/sh

if [ $# -ne 2 ] 
  then
   echo "USAGE: gebmerge.sh <merged_dir> <run>"
  exit 1
fi

RUN=$2
DIR=$1

echo "GEBSort started sorting run $RUN at `date`"
./GEBSort_nogeb -input disk $DIR/GEBMerged_run$RUN.gtd -rootfile ./ROOT_FILES/run$RUN.root RECREATE -chat GEBSort.chat > ./LOG_FILES/GEBSort_run$RUN.log
echo "GEBSort DONE at `date`"

#exit


