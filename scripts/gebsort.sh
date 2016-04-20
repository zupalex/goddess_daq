#!/bin/bash

if [ $# -lt 3 ] 
then
    echo "-> USAGE: gebsort.sh <merged_dir> <run> <output_dir> <nevent=XXXX (optional)> <config=config_file_name (optional)> <nocalib (optional)> <nomapping (optional)>"
    echo ""
    echo "-> Optional arguments can be put in any order"
    echo ""
    echo "-> nevent=XXXX command will treat XXXX events without having to modify the chat file"
    echo "-> config=config_file_name will tell the program to use config_file_name instead of the default config file which is automatically determined according to the run number"
    echo "-> nocalib will generate the output tree but won't apply the calib parameters"
    echo "-> nomapping will create an additional tree containing the raw data in pairs <channel, value>"
    exit 1
fi

RUN=$2
INPUT_DIR=$1
OUTPUT_DIR=$3

NOCALIBFLAG=""
NOMAPPINGFLAG=""

NEVENTSARG=""
CONFIGFILEARG=""

if [ "$1" = "default" ]; then
    INPUT_DIR="/mnt/hgfs/GODDESS_MERGED/merged"
fi

if [ "$3" = "ana1" ]; then
    if [ ! -e "/mnt/hgfs/GODDESS_ANA1/rootfiles/sorted/log" ]; then
	mkdir log
    fi
    
    OUTPUT_DIR="/mnt/hgfs/GODDESS_ANA1/rootfiles/sorted"
fi

for arg in "$@" 
do
    if [ "$arg" = "nocalib" ]; then
	NOCALIBFLAG="-nocalib"
	echo "/!\\ will process the run without applying the calibration parameters /!\\"
    elif [ "$arg" = "nomapping" ]; then
	NOMAPPINGFLAG="-nomapping"
	echo "raw tree with the pairs <channel, value> will be added to the file"
    elif [[ "$arg" != "${arg/nevents=/}" ]]; then
	NEVENTS="${arg/nevents=/}"
	NEVENTSARG="-nevents $NEVENTS"
	echo "Number of events to treat set to $NEVENTS"
    elif [[ "$arg" != "${arg/config=/}" ]]; then
	CONFIGFILEARG="-config ${arg/config=/}"
    fi
done

if [ ! -e $INPUT_DIR/GEBMerged_run$RUN.gtd_000 ]; then
	echo "ERROR: Merged file not found! => Requested $INPUT_DIR/GEBMerged_run$RUN.gtd_000"	
	exit 1
fi

echo "GEBSort started sorting run $RUN at `date`"
if [ ! -e "$OUTPUT_DIR/log" ]; then
	mkdir log
fi

time ./GEBSort_nogeb -input disk $INPUT_DIR/GEBMerged_run$RUN.gtd_000 -rootfile $OUTPUT_DIR/run$RUN.root RECREATE $NEVENTSARG $CONFIGFILEARG $NOCALIBFLAG $NOMAPPINGFLAG -chat chatfiles/GEBSort.chat | tee log/GEBSort_current.log > log/GEBSort_run$RUN.log
echo "GEBSort DONE at `date`"

tail -n 5 log/GEBSort_run$RUN.log

#exit


