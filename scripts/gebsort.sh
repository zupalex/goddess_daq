#!/bin/sh

ReturnError()
{
    echo "-> USAGE: gebsort.sh <merged_dir> <run> <output_dir> <nevent=[XXXX] (optional)> <config=[config_file_name] (optional)> <nocalib=[mode] (optional)> <nomapping (optional)>"
    echo ""
    echo "-> Optional arguments can be put in any order"
    echo ""
    echo "-> nevent=[XXXX] command will treat XXXX events without having to modify the chat file"
    echo "-> config=[config_file_name] force the use of [config_file_name] instead of the default config file automatically determined from the run number provided"
    echo "-> nocalib=[mode] handles the calibration level."
    echo "           [mode]==1 will generate one tree sorted but not calibrated." 
    echo "           [mode]==2 will generate two sorted trees, one calibrated, the other one not"
    echo "           [mode]==3 will not generate any sorted tree. Useful only if run with the unmapped tree is generated with the nomapping mode"
    echo "-> nomapping will create an additional tree containing the raw data in pairs <channel, value>"
}

if [ $# -lt 3 ] 
then
    ReturnError
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

COUNTER=0

for arg in "$@" 
do
COUNTER=$(($COUNTER + 1))

    if [ "$arg" = "nocalib" ]; then

	NOCALIBFLAG="-nocalib 1"
	echo "/!\\ will process the run without applying the calibration parameters in mode 1/!\\"

    elif [ "$arg" != "${arg##nocalib=}" ]; then

	NOCALVAL="${arg##nocalib=}"

	if [ $NOCALVAL -lt -1 -o $NOCALVAL -gt 2 ]; then
	    echo "INVALID VALUE SPECIFIED FOR nocalib ARGUMENT!!"
	    ReturnError
	    exit 1
	fi

	NOCALIBFLAG="-nocalib $NOCALVAL"
	echo "/!\\ will process the run without applying the calibration parameters in mode $NOCALVAL/!\\"

    elif [ "$arg" = "nomapping" ]; then

	NOMAPPINGFLAG="-nomapping"
	echo "raw tree with the pairs <channel, value> will be added to the file"

    elif [ "$arg" != "${arg##nevents=}" ]; then

	NEVENTS="${arg##nevents=}"

	if [ $NEVENTS -lt 1 ]; then
	    echo "INVALID VALUE SPECIFIED FOR nevents ARGUMENT!!"
	    ReturnError
	    exit 1
	fi

	NEVENTSARG="-nevents $NEVENTS"
	echo "Number of events to treat set to $NEVENTS"

    elif [ "$arg" != "${arg##config=}" ]; then

	CONFIGFILEARG="-config ${arg##config=}"
	echo "Forced use of the following config file: ${arg##config=}"

    elif [ $COUNTER -gt 3 ]; then
	
	echo "ONE OR MORE INVALID ARGUMENT"
	ReturnError
	exit 1	
	
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


