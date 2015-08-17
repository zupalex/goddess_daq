#/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
RESET='\033[0m'
warn=0

if [ $# -ne 2 ] 
  then
   echo "USAGE: copyRun.sh <destination> <run>"
  exit 1
fi

RUN=$2
DIR="$1"

if [ $DIR != *":"* ] && [ ! -e $DIR ]; then
	printf "${RED}ERROR:${RESET} Destination directory $DIR does not exist.\n"
	exit 1
fi

DIR="$1/run$RUN/"
if [ ! -e $DIR ]; then 
	mkdir $DIR 
else
	printf "${YELLOW}WARNING:${RESET} Run directory already created, data may have already been copied.\n"
fi

echo "Copying data files to ${DIR}"

printf "${BLUE}Copying DGS files${RESET}\n"
rsync -ahP --chmod=a-wx dgs1:/esata/esata1/user/gsfma330/run_$RUN.dgs* $DIR/
if [ $? != 0 ]; then
	printf "${YELLOW}WARNING:${RESET} No DGS run files found!\n"
	warn=1
fi

printf "${BLUE}Copying DFMA files${RESET}\n"
rsync -ahP --chmod=a-wx nat2:/media/20150529c/user/gsfma330/run_gsfma330_$RUN* $DIR/
if [ $? != 0 ]; then
	printf "${YELLOW}WARNING:${RESET} No DFMA run files found!\n"
	warn=1
fi

printf "${BLUE}Copying LDF files${RESET}\n"
	
if [ -e /media/4844678136/run$RUN.ldf ]; then
	rsync -ahP --chmod=a-wx /media/4844678136/run$RUN.ldf $DIR/
	if [ $? != 0 ]; then
		printf "${YELLOW}WARNING:${RESET} No ORNL ldf run file found!\n"
		warn=1
	fi
elif [ ! -e $DIR/run$RUN.ldf ]; then
	printf "${YELLOW}WARNING:${RESET} ORNL ldf file not found on memory stick!\n"
	warn=1
fi

if [ ${warn} != 1 ]; then
	printf "${GREEN}Completed${RESET}. Have a nice day.\n"
else
	printf "${YELLOW}Completed${RESET}. Possible errors occured.\n"
fi
