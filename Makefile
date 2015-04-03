

#PLTFMOPT=Makefile.$(shell uname)
#include $(PLTFMOPT)

ROOTINC = $(shell root-config --cflags)
ROOTLIB = $(shell root-config --new --libs)
 
CCFLAG   = -I. -I$(ROOTSYS)/include -L$(ROOTSYS)/lib -fpermissive
CC       = /usr/bin/g++ -D_FILE_OFFSET_BITS=64 -DLINUX $(CCENV)
cc       = /usr/bin/gcc -D_FILE_OFFSET_BITS=64 -DLINUX $(CCENV)

TARGETS = GTMerge3 zzip zunzip mkMap DGSSort 

all:		${TARGETS}

dgsHeader.c:	GTMerge.h
GTPrint.c:	GTMerge.h
GTMerge3.c:	GTMerge.h GTMerge_readnew.h pbuf.h
GTUtils.c:	GTMerge.h

#-------------------

DGSSort:	GTPrintCC.o DGSSort.o time_stampCC.o dgsHeaderCC.o GTPrintCC.o
		$(CC)$^  $(ROOTLIB) -o $@ -lz
#		rm core.*; $@ -chat DGStest.chat > DGStest.log
#		./go

DGSSort.o:	DGSSort.cxx GTMerge.h efftape.h gsII.h DGSSort.h\
		UserInclude.h UserChat.h UserStat.h UserFunctions.h \
		UserDeclare.h UserInit.h UserRawEv.h UserGoodEv.h \
		UserPreCond.h UserEv.h UserExit.h
		$(CC) $(CCFLAG) $(ROOTINC) -c DGSSort.cxx
		
time_stampCC.o:	time_stamp.c
		$(CC)  -c $(CCFLAG) -o time_stampCC.o time_stamp.c  

dgsHeaderCC.o:	dgsHeader.c
		$(CC) $(CCFLAG) -c -o $@ $^

GTPrintCC.o:	GTPrint.c
		$(CC) $(CCFLAG) -c -o $@  $^


#-------------------

GTMerge3:	GTMerge3.c spe_fun.o dgsHeader.o \
		GTPrint.o get_a_seed.o time_stamp.o GTUtils.o\
		spe_fun.o -lz
#		$(cc) $(CCFLAG) -o $@ -lm -lz $^
		$(cc) $(CCFLAG) -o $@ -lz -lm $^
#		$@ gtmerge.chat  DATA/merged.gtd `ls DATA/run*`
#		$@ gtmerge.chat  DATA/merged.gtd DATA/run_003.dgs5_000
#		./go
#		rm core.*; GTMerge3 gtmerge.chat DATA/merged.gtd DATA/run_39.chico_000		

GTMerge3.c:	GTMerge_readnew.h pbuf.h GTMerge.h

dgsHeader.o:	dgsHeader.c GTMerge.h
		$(cc) $(CCFLAG) -c $^

GTPrint.o:	GTPrint.c GTMerge.h
		$(cc) $(CCFLAG) -c $^

get_a_seed.o:	get_a_seed.c 
		$(cc) -c $(CCFLAG) $^

time_stamp.o:	time_stamp.c
		$(cc)  -c $(CCFLAG) $^

GTUtils.o:	GTUtils.c
		$(cc) $(CCFLAG) -c $^

spe_fun.o:	spe_fun.c 
		$(cc) -c $(CCFLAG) $^

listTS:		listTS.c spe_fun.o dgsHeader.o \
		GTPrint.o get_a_seed.o time_stamp.o GTUtils.o\
		spe_fun.o 
		$(cc) $(CCFLAG) -o $@ -lm -lz $^
#		listTS 2000000000 2000000000 0 20000000000 DATA/run_44.gtd03_000

#-------------------

fsplit:		fsplit.c GTUtils.o
		$(cc) $(CCFLAG) -o $@ $^ -lz

zzip:		zzip.c
		$(cc) $(CCFLAG) -o $@ $^ -lz

zunzip:		zunzip.c
		$(cc) $(CCFLAG) -o $@ $^ -lz

mkMap:		mkMap.c
		$(cc) $(CCFLAG) -o $@ $^ -lz






clean:		
		rm -f *.o
		rm -f core.*
		rm -f *%
		rm -f a.out
		rm -f *.gch
		rm -f ${TARGETS}

debug:		gdb -d ./ GTMerge3 `ls core.*`

