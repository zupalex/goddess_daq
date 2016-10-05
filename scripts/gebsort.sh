#!/bin/sh

ReturnError()
{
    echo "-> USAGE: gebsort.sh <merged_dir> <run> <output_dir> <optional parameters (see list below)>"
    echo ""
    echo "You can specify either a single run number, or a list enclosed in \" \" (example: \"114 115 117 120\")"
    echo ""
    echo "-> Optional arguments can be put in any order"
    echo ""
    echo "-> suffix=[output_file_suffix] will append the specified suffix at the end of the output rootfile name"
    echo "-> nevents=[XXXX] command will treat XXXX events without having to modify the chat file / nevents=all will treat all the events in the merged file" 
    echo "-> config=[config_file_name] force the use of [config_file_name] instead of the default config file automatically determined from the run number provided"
    echo "-> nocalib=[mode] handles the calibration level."
    echo "           [mode]==1 will generate one tree sorted but not calibrated." 
    echo "           [mode]==2 will generate two sorted trees, one calibrated, the other one not"
    echo "           [mode]==-1 will not generate any sorted tree. Useful only if run with the unmapped tree is generated with the nomapping mode"
    echo "-> ignorethr=[mode] handles the thresholds ignore level."
    echo "             [mode]==0 will apply the threshold no matter what, even for the sorted uncalibrated tree." 
    echo "             [mode]==1 will not apply the threshold to the sorted uncalibrated tree but will apply it to the calibrated one"
    echo "             [mode]==2 won't apply any threshold to any tree"
    echo "-> sidetails=[mode] handles the detail level for the output Si Data."
    echo "           [mode]==0 won't generate any output for the Si detectors." 
    echo "           [mode]==1 will store only the sum of the energies per sector."
    echo "           [mode]==2 will store all the energies collected by each strips individually as well as the sum per sector."
    echo "-> nomapping will create an additional tree containing the raw data in pairs <channel, value>"
    echo "-> nohists will prevent histograms to be generated"
    echo "-> userfilter=[folder name] will create a \"cleaned\" merged file in the specified folder using the filters defined in scripts/UserEventFilter.cxx"
}

if [ $# -lt 3 ] 
then
    ReturnError
    exit 1
fi

INPUT_DIR=$1
OUTPUT_DIR=$3

OUTPUTSUFIX=""

NOCALIBFLAG=""
NOMAPPINGFLAG=""
NOHISTSFLAG=""
IGNORETHRFLAG=""
SIDETLVLFLAG=""

NEVENTSARG=""
FSTEVENTSARG=""
SPLITEVTARG=""
CONFIGFILEARG=""

USERFILTERDIR=""
USERFILTERARG=""

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

    elif [ "$arg" != "${arg##ignorethr=}" ]; then
	
	IGNTHRVAL="${arg##ignorethr=}"
	
	if [ $IGNTHRVAL -lt 0 -o $IGNTHRVAL -gt 2 ]; then
	    echo "INVALID VALUE SPECIFIED FOR ignorethr ARGUMENT!!"
	    ReturnError
	    exit 1
	fi
	
	IGNORETHRFLAG="-ignorethrs $IGNTHRVAL"
	echo "/!\\ will process the run with ignore thresholds level set to $IGNTHRVAL!\\"
	
    elif [ "$arg" != "${arg##nocalib=}" ]; then

	NOCALVAL="${arg##nocalib=}"

	if [ $NOCALVAL -lt -1 -o $NOCALVAL -gt 2 ]; then
	    echo "INVALID VALUE SPECIFIED FOR nocalib ARGUMENT!!"
	    ReturnError
	    exit 1
	fi

	NOCALIBFLAG="-nocalib $NOCALVAL"
	echo "/!\\ will process the run without applying the calibration parameters in mode $NOCALVAL/!\\"

    elif [ "$arg" != "${arg##sidetails=}" ]; then

	SIDETLVLVAL="${arg##sidetails=}"

	if [ $SIDETLVLVAL -lt 0 -o $SIDETLVLVAL -gt 2 ]; then
	    echo "INVALID VALUE SPECIFIED FOR nocalib ARGUMENT!!"
	    ReturnError
	    exit 1
	fi

	SIDETLVLFLAG="-siDetailLvl $SIDETLVLVAL"
	echo "/!\\ will process the run with the Si Detectors Output Details set to level $SIDETLVLVAL/!\\"
	
    elif [ "$arg" = "nomapping" ]; then

	NOMAPPINGFLAG="-nomapping"
	echo "raw tree with the pairs <channel, value> will be added to the file"

    elif [ "$arg" = "nohists" ]; then

	NOHISTSFLAG="-nohists"
	echo "no pre-made histograms will be written to the file..."

    elif [ "$arg" != "${arg##nevents=}" ]; then

	NEVENTS="${arg##nevents=}"

	echo "Number of events to treat set to $NEVENTS"

	if [ "$NEVENTS" = "all" ]; then
	
	    NEVENTS=99999999999999999999999999
	    
	elif [ $NEVENTS -lt 1 ]; then
	
	    echo "INVALID VALUE SPECIFIED FOR nevents ARGUMENT!!"
	    ReturnError
	    exit 1
	    
	fi

	NEVENTSARG="-nevents $NEVENTS"
	
    elif [ "$arg" != "${arg##config=}" ]; then

	CONFIGFILEARG="-config ${arg##config=}"
	echo "Forced use of the following config file: ${arg##config=}"
	
    elif [ "$arg" != "${arg##suffix=}" ]; then
    
	OUTPUTSUFFIX="${arg##suffix=}"
	echo "The following suffix will be append at the end of the output filename: $OUTPUTSUFFIX"
	
    elif [ "$arg" != "${arg##userfilter=}" ]; then

        USERFILTERDIR=${arg##userfilter=}
    
        if [ ! -e "$USERFILTERDIR" ]; then
	   echo "FOLDER SPECIFIED FOR THE CLEANED MERGED FILE DOES NOT EXIST."
	   ReturnError
	   exit 1	
        fi

	echo "Will generated a cleaned merged file in ${arg##userfilter=}"

    elif [ $COUNTER -gt 3 ]; then
	
	echo "ONE OR MORE INVALID ARGUMENT"
	ReturnError
	exit 1	
	
    fi
done

echo "The following runs will be treated:"

for run in `echo "$2"`
do
echo "run #$run"
done

for run in `echo "$2"`
do
    RUN=$run

    if [ ! -e $INPUT_DIR/GEBMerged_run$RUN.gtd_000 ]; then
	echo "ERROR: Merged file not found! => Requested $INPUT_DIR/GEBMerged_run$RUN.gtd_000"	
	exit 1
    fi
    
    echo "GEBSort started sorting run $RUN at `date`"
    if [ ! -e $OUTPUT_DIR/log ]; then
	mkdir $OUTPUT_DIR/log
    fi
    
    if [ "$USERFILTERDIR" != "" ]; then
        USERFILTERARG="-userfilter $USERFILTERDIR/GEBMerged_run$RUN.gtd_000"
    fi
    
    time ./GEBSort_nogeb -input disk $INPUT_DIR/GEBMerged_run$RUN.gtd_000 -rootfile $OUTPUT_DIR/run$RUN$OUTPUTSUFFIX.root RECREATE $NEVENTSARG $CONFIGFILEARG $NOCALIBFLAG $NOMAPPINGFLAG $NOHISTSFLAG $IGNORETHRFLAG $SIDETLVLFLAG $USERFILTERARG -chat chatfiles/GEBSort.chat | tee $OUTPUT_DIR/log/GEBSort_current.log > $OUTPUT_DIR/log/GEBSort_run$RUN.log
    echo "GEBSort DONE at `date`"
    
    #tail -n 5 $OUTPUT_DIR/log/GEBSort_run$RUN.log
done

#exit


