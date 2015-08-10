#/bin/bash

YELLOW='\033[0;33m'
BLUE='\033[0;34m'
GREEN='\033[0;32m'
RESET='\033[0m'
warn=false

if [ $# -ne 1 ] 
  then
   echo "USAGE: copyRun.sh <run>"
  exit 1
fi

RUN=$1

DIR=/media/Pele/calib/run$RUN

if [ ! -e $DIR ]; then
	mkdir $DIR
fi

echo "Copying data files to ${DIR}"

printf "${BLUE}Copying DGS files${RESET}\n"
rsync -aP dgs1:/media/20140317_1604/user/gsfma330/run_$RUN.dgs* $DIR
if [ $? != 0 ]; then
	printf "${YELLOW}WARNING:${RESET} No DGS run files found!\n"
	warn=true
fi

printf "${BLUE}Copying DFMA files${RESET}\n"
rsync -aP nat2:/media/20150529c/user/gsfma330/run_gsfma330_$RUN* $DIR
if [ $? != 0 ]; then
	printf "${YELLOW}WARNING:${RESET} No DFMA run files found!\n"
	warn=true
fi

if [ ! -e $DIR/run$RUN.ldf ] || [ -e /media/4844678136/run$RUN.ldf ]; then 
	printf "${BLUE}Copying LDF files${RESET}\n"
	if [ ! -e /media/4844678136/run$RUN.ldf ]; then
		printf "${YELLOW}WARNING:${RESET} ORNL ldf file not found on memory stick!\n"
		warn=true
	else 
		mv /media/4844678136/run$RUN.ldf $DIR
	fi
fi

chmod a-w $DIR/*

if [ ! ${warn} ]; then
	printf "${GREEN}Completed${RESET}. Have a nice day.\n"
else
	printf "${YELLOW}Completed${RESET}. Possible errors occured.\n"
fi
