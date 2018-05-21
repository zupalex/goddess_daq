#include <stdlib.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <assert.h>
#include <zlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>

#ifdef __unix__
#include "sys/sysinfo.h"
#endif

#include "MergeManager.h"

using std::string;

#include <signal.h>
#include <execinfo.h>
#include <ucontext.h>
#include <unistd.h>

bool specialDebug = false;

/* This structure mirrors the one found in /usr/include/asm/ucontext.h */
typedef struct _sig_ucontext
{
		unsigned long uc_flags;
		struct ucontext *uc_link;
		stack_t uc_stack;
		struct sigcontext uc_mcontext;
		sigset_t uc_sigmask;
} sig_ucontext_t;

void crit_err_hdlr(int sig_num, siginfo_t * info, void * ucontext)
{
	void * array[50];
	void * caller_address;
	char ** messages;
	int size, i;
	sig_ucontext_t * uc;

	uc = (sig_ucontext_t *) ucontext;

	/* Get the address at the time the signal was raised */
#if defined(__i386__) // gcc specific
	caller_address = (void *) uc->uc_mcontext.eip; // EIP: x86 specific
#elif defined(__x86_64__) // gcc specific
	caller_address = (void *) uc->uc_mcontext.rip; // RIP: x86_64 specific
#endif

	fprintf(stderr, "signal %d (%s), address is %p from %p\n", sig_num, strsignal(sig_num), info->si_addr, (void *) caller_address);

	size = backtrace(array, 50);

	/* overwrite sigaction with caller's address */
	array[1] = caller_address;

	messages = backtrace_symbols(array, size);

	/* skip first stack frame (points here) */
	for (i = 1; i < size && messages != NULL; ++i)
	{
		fprintf(stderr, "[bt]: (%d) %s\n", i, messages[i]);

		char syscom[256];
		sprintf(syscom, "addr2line %p -e GEBMerge", array[i]);
		if (system(syscom) == -1)
		{
			cerr << "There was a problem here..." << endl;
		}
	}

	free(messages);

	exit(EXIT_FAILURE);
}

MSTAT* nstat;
CONTROL control;

bool printDebug = false;
bool printBytesCount = false;
bool printProgress = true;
bool alwaysCalcBytesDiff = false;
bool printWeirdTs = false;

EVENT* GTGetDiskEv(InDataInfo* inFile, EVENT* bufEvent, bool printInfo)
{
	if (printDebug) std::cerr << "File is " << (inFile->istream->is_open() ? "open" : "NOT open!") << "\n";
	/* returns: */
	/*   0: success */
	/*   1: header */
	/*   2: read trouble; end of file */
	/*   5: bad test pattern */

	/* declarations */

	MergeManager* theMergeManager = MergeManager::sinstance();

	if (printDebug) std::cerr << "Retreived theMergeManager...\n";

	int siz, i1;
	int storeNo = theMergeManager->Event.size();

	/* attempt to read a GEB header */

	if (printDebug) std::cerr << "*** Reading the header...\n";

	if (specialDebug)
	{
		cout << "GTGetDiskEv before reading gd" << endl;
		cout << "bufEvent = " << bufEvent << endl;
		cout << "bufEvent->key = " << bufEvent->key << endl;
		cout << "bufEvent->gd->length = " << bufEvent->gd->length << endl;
		cout << "bufEvent->gd->type = " << bufEvent->gd->type << endl;
		cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
	}

	int prev_size = bufEvent->gd->length;

	(inFile->istream)->read((char*) bufEvent->gd, sizeof(GebData));

	if (specialDebug)
	{
		cout << "GTGetDiskEv after reading gd" << endl;
		cout << "bufEvent = " << bufEvent << endl;
		cout << "bufEvent->key = " << bufEvent->key << endl;
		cout << "bufEvent->gd->length = " << bufEvent->gd->length << endl;
		cout << "bufEvent->gd->type = " << bufEvent->gd->type << endl;
		cout << endl;
		if (bufEvent->key > 0) cout << "inFile->istream = " << inFile->istream << "   /   theMergeManager->inData->at(bufEvent->key)->istream = "
				<< theMergeManager->inData->at(bufEvent->key)->istream << endl;
		cout << "************************************************" << endl;
		cout << "************************************************" << endl;
	}

	if (printDebug)
	{
		std::cerr << "=> Header read, now checking it...\n";
		std::cerr << "   - type is " << bufEvent->gd->type << " / length is " << bufEvent->gd->length << "\n";
		std::cerr << "   - timestamp is " << bufEvent->gd->timestamp << "\n";
	}

	if ((inFile->istream)->gcount() != sizeof(GebData) || (inFile->istream)->eof())
	{
		if (inFile->istream->eof())
		{
			if (printDebug)
			{
				std::cerr << "\n\n";
				std::cerr << "The file " << inFile->fileName << " (#" << inFile->fileNum << ") does not have anymore data"
						<< (inFile->istream->gcount() == 0 ? "" : " after reading the header") << "...\n";
				std::cerr << "--------> Last timestamp was " << bufEvent->gd->timestamp << "\n";
				theMergeManager->goBackToTop = false;
			}

			printf("\n===> Loop #%llu: The file %s (#%d) do not have anymore data%s. Last timestamp = %llu\n", theMergeManager->loopCounter, (inFile->fileName).c_str(),
					inFile->fileNum, (inFile->istream->gcount() == 0 ? "" : " after reading the header"), bufEvent->gd->timestamp);
		}

		printf("File #%i (%s): failed to read %lu bytes for header, got %li\n", inFile->fileNum, (inFile->fileName).c_str(), sizeof(GebData), (inFile->istream)->gcount());

		theMergeManager->RemoveFromInputList(inFile->fileName);

		if (inFile->istream->eof())
		{
			printf("The file %s has been removed from the list of files to treat... (files remaining: %lu)\n", (inFile->fileName).c_str(), theMergeManager->inData->size());
			if (printDebug) std::cerr << "The file has been removed from the list of files to treat... (files remaining: " << theMergeManager->inData->size() << ")\n\n";
		}

		return nullptr;
	}

	if (printDebug) std::cerr << "Header looks fine... Filling some stats...\n";

	theMergeManager->readBytesCount += sizeof(GebData);

	nstat->inbytes += (inFile->istream)->gcount();
	control.filesiz[storeNo] += (inFile->istream)->gcount();

	if (printDebug) std::cerr << "Header stats filled...\n";

	if (printInfo) printf("\ngot initial header, TS=%lli for storeNo=%i\n", bufEvent->gd->timestamp, storeNo);

	/* attempt to read payload */

	i1 = bufEvent->gd->length;

//	if (specialDebug) cout << bufEvent->gd->type << "    /    " << bufEvent->gd->length << endl;

	if (bufEvent->gd->type != inFile->type)
	{
		if (bufEvent->gd->type != inFile->type)
		{
			cerr << "bufEvent->gd->type has an unexpected value! >> " << bufEvent->gd->type << " =/= " << inFile->type << " in " << inFile->fileName << endl;
			cerr << "Forcing payload read size to " << prev_size << endl;
			i1 = prev_size;
			bufEvent->gd->length = i1;
		}

		unsigned long long currFilePos = (inFile->istream)->tellg();
		cerr << "Current position and remaining bytes in file: " << currFilePos << "   /   " << inFile->fileLength - currFilePos << endl;

		(inFile->istream)->read((char *) bufEvent->payload, i1);
		return GTGetDiskEv(inFile, bufEvent, printInfo);
//		specialDebug = true;
//		return nullptr;
	}
//
//	if(i1 > 2000)
//	{
//		cerr << "bufEvent->gd->length bigger than expected! >> " << bufEvent->gd->length << " > " << 2000 << endl;
//	}

	if (bufEvent->gd->type != inFile->type)
	{
		cout << "bufEvent->gd->type != inFile->type => " << bufEvent->gd->type << " != " << inFile->type << endl;
		specialDebug = true;
	}

	//     bufEvent->payload = new char[i1];

	if (printDebug) std::cerr << "Reading the payload...\n";

	(inFile->istream)->read((char *) bufEvent->payload, i1);

	if (printDebug) std::cerr << "Payload read... now checking it...\n";

	if ((inFile->istream)->gcount() != i1 || (inFile->istream)->eof())
	{
		if (inFile->istream->eof())
		{
			if (printDebug)
			{
				std::cerr << "\n\n";
				std::cerr << "The file " << inFile->fileName << " (#" << inFile->fileNum << ") does not have anymore data"
						<< (inFile->istream->gcount() == 0 ? "" : " after reading the payload") << "...\n";
				std::cerr << "--------> Last timestamp was " << bufEvent->gd->timestamp << "\n";
				theMergeManager->goBackToTop = false;
			}

			printf("\n===> Loop #%llu: The file %s (#%d) do not have anymore data%s. Last timestamp = %llu\n", theMergeManager->loopCounter, (inFile->fileName).c_str(),
					inFile->fileNum, (inFile->istream->gcount() == 0 ? "" : " after reading the payload"), bufEvent->gd->timestamp);

		}

		printf("File #%i (%s): failed to read %i bytes for payload, got %lu\n", inFile->fileNum, (inFile->fileName).c_str(), i1, (inFile->istream)->gcount());

		theMergeManager->RemoveFromInputList(inFile->fileName);

		if (inFile->istream->eof())
		{
			printf("The file %s has been removed from the list of files to treat... (files remaining: %lu)\n", (inFile->fileName).c_str(), theMergeManager->inData->size());
			if (printDebug) std::cerr << "The file has been removed from the list of files to treat... (files remaining: " << theMergeManager->inData->size() << ")\n\n";
		}

		return nullptr;
	}

	if (printDebug) std::cerr << "Payload seems fine... now filling some stats...\n";

	theMergeManager->readBytesCount += i1;

	nstat->inbytes += (inFile->istream)->gcount();
	control.filesiz[storeNo] += (inFile->istream)->gcount();
	control.fileEventsRead[storeNo]++;

	if (printDebug) std::cerr << "Payload stats filled...\n";

	if (printInfo) printf("read initial payload of siz=%i into  storeNo=%i\n", siz, storeNo);

	/* done */

	if (printDebug) std::cerr << "Done with GTGetDiskEv... Exiting...\n";

	return bufEvent;
}

int main(int argc, char **argv)
{
	//     std::cerr << "Entering main function of GEBMerge.cxx...\n";

	struct sigaction sigact;

	sigact.sa_sigaction = crit_err_hdlr;
	sigact.sa_flags = SA_RESTART | SA_SIGINFO;

	if (sigaction(SIGSEGV, &sigact, (struct sigaction *) NULL) != 0)
	{
		fprintf(stderr, "error setting signal handler for %d (%s)\n",
		SIGSEGV, strsignal(SIGSEGV));

		exit(EXIT_FAILURE);
	}

//	char* crash = 0;
//	sprintf(crash, "crash it damit!");
//	int* crash = 0;
//	*crash = 666;

#ifdef __unix__
	struct sysinfo memInfo;
#endif

	/* declarations */

	MergeManager* theMergeManager = MergeManager::sinstance();
	std::vector<InDataInfo*>* inData = theMergeManager->inData;
	std::ofstream* outData = &theMergeManager->outData;

	gzFile zoutData;
	int maxNoEvents = 0, nPoolEvents = 0;
	FILE *fp;
	int i1, i2;
	int nn, argcoffset = 0;
	float ehigain[NGE + 1], ehioffset[NGE + 1], r1;
	char str[STRLEN], str1[512];
	unsigned int seed = 0;
	struct tms timesThen;
	int i7, i8, reportinterval;
	int echo = 0, nni, nret;
	int size;
	int wosize = 0;
	int nprint;

	/* initialize random number generator etc */

	GetASeed(&seed);
	srand(seed);
	nstat = (MSTAT *) calloc(1, sizeof(MSTAT));
	bzero((char *) &control, sizeof(CONTROL));
	bzero((char *) nstat, sizeof(MSTAT));

	control.dtsfabort = 5;
	control.dtsbabort = 5;

	for (int i = 0; i < NCHANNELS; i++)
		nstat->id_hit[i] = 0;

	for (int i = 0; i <= NGE; i++)
	{
		ehigain[i] = 1;
		ehioffset[i] = 0;
	};

	/* open chat file */

	std::ifstream chatFile(argv[1]);

	if (!chatFile.is_open())
	{
		printf("error: could not open chat file: <%s>\n", argv[1]);
		exit(0);
	}

	printf("chat file: <%s> open\n", argv[1]);
	printf("\n");
	fflush( stdout);

	if (printDebug) std::cerr << "Reading chatfile...\n";

	/* read chatfile content and act */

	string readLine;

	nn = 0;
	while (std::getline(chatFile, readLine))
	{
		if (readLine.empty()) continue;

		if (echo) printf("chat->%s", readLine.c_str());
		fflush( stdout);

		/* attemp to interpret the line */

		if (readLine.find("echo") != string::npos)
		{
			echo = 1;
			if (echo) printf("will echo command lines\n");
			fflush( stdout);

		}
		else if (readLine[0] == 35)
		{
			/* '#' comment line, do nothing */
			nni--; /* don't count as instruction */

		}
		else if (readLine[0] == 59)
		{
			/* ';' comment line, do nothing */
			nni--; /* don't count as instruction */

		}
		else if (readLine[0] == 10)
		{
			/* empty line, do nothing */
			nni--; /* don't count as instruction */

		}
		else if (readLine.find("maxNoEvents") != string::npos)
		{
			nret = sscanf(readLine.c_str(), "%s %i", str1, &maxNoEvents);
			CheckNoArgs(nret, 2, readLine);
		}
		else if (readLine.find("reportinterval") != string::npos)
		{
			nret = sscanf(readLine.c_str(), "%s %i", str1, &reportinterval);
			CheckNoArgs(nret, 2, readLine);
		}
		else if (readLine.find("chunksiz") != string::npos)
		{
			nret = sscanf(readLine.c_str(), "%s %i", str1, &control.chunksiz);
			CheckNoArgs(nret, 2, readLine);
		}
		else if (readLine.find("bigbufsize") != string::npos)
		{
			nret = sscanf(readLine.c_str(), "%s %i", str1, &size);
			CheckNoArgs(nret, 2, readLine);
			assert(size <= MAXBIGBUFSIZ);
			r1 = (size * sizeof(EVENT) + (size + 1) * sizeof(int)) / 1024.0 / 1024.0;
			printf("sizeof(EVENT)= %lu\n", sizeof(EVENT));
			printf("will use a bigbuffer size of %i, or %7.3f MBytes\n", size, r1);
		}
		else if (readLine.find("nprint") != string::npos)
		{
			nret = sscanf(readLine.c_str(), "%s %i", str1, &nprint);
			CheckNoArgs(nret, 2, readLine);
			printf("will print information for first %i events\n", nprint);
		}
		else if (readLine.find("wosize") != string::npos)
		{
			nret = sscanf(readLine.c_str(), "%s %f", str1, &r1);
			r1 = (r1 / 100.0 * size);
			wosize = (int) r1;
			CheckNoArgs(nret, 2, readLine);
			printf("will use a bigbuffer wosize of %i\n", wosize);
			assert(wosize <= size);
		}
		else if (readLine.find("startTS") != string::npos)
		{
			nret = sscanf(readLine.c_str(), "%s %llu %llu ", str1, &control.startTS_lo, &control.startTS_hi);
			CheckNoArgs(nret, 3, readLine);
			printf("startTS from %lli to %lli\n", control.startTS_lo, control.startTS_hi);
			control.startTS = 1;
		}
		else if (readLine.find("zzipout") != string::npos)
		{
			nret = sscanf(readLine.c_str(), "%s", str1);
			CheckNoArgs(nret, 1, readLine);
			control.zzipout = 1;
			printf("will zzip output, %i\n", control.zzipout);
			fflush( stdout);

		}
		else if (readLine.find("TSlistelen") != string::npos)
		{
			nret = sscanf(readLine.c_str(), "%s %i %i %i", str1, &control.TSlistelen, &control.TSlist_lo, &control.TSlist_hi);
			CheckNoArgs(nret, 4, readLine);
		}
		else if (readLine.find("dts_min") != string::npos)
		{
			nret = sscanf(readLine.c_str(), "%s %lli", str1, &control.dts_min);
			CheckNoArgs(nret, 2, readLine);
			printf("control.dts_min=%lli\n", control.dts_min);
		}
		else if (readLine.find("dts_max") != string::npos)
		{
			nret = sscanf(readLine.c_str(), "%s %lli", str1, &control.dts_max);
			CheckNoArgs(nret, 2, readLine);
			printf("control.dts_max=%lli\n", control.dts_max);
		}
		else if (readLine.find("dtsfabort") != string::npos)
		{
			nret = sscanf(readLine.c_str(), "%s %i", str1, &control.dtsfabort);
			CheckNoArgs(nret, 2, readLine);
		}
		else if (readLine.find("dtsbabort") != string::npos)
		{
			nret = sscanf(readLine.c_str(), "%s %i", str1, &control.dtsbabort);
			CheckNoArgs(nret, 2, readLine);
		}
		else
		{
			/* --------------------------- */
			/* chatscript read error point */
			/* --------------------------- */

			printf("line %2.2i in chat script, option :%s \n__not understood\n", nn, readLine.c_str());
			printf("%i\n", readLine[0]);
			printf("aborting\n");
			fflush( stdout);
			exit(0);
		}

		/* read next line in chat script */

		nn++; /* line counter */
		nni++; /* instruction counter */
	}

	chatFile.close();

	/* extract and repeat parameters */

	printf("%s: will produce a max of %i events\n", argv[0], maxNoEvents);

	printf("%s: will write combined data to file \"%s\"\n", argv[0], argv[2]);

	/* offset for data file name reads, find  nfiles etc */

	argcoffset = 3;
	const int nfiles = (argc - argcoffset);
	printf("%s: we have %i datafiles to combine\n", argv[0], nfiles);
	fflush( stdout);

	nPoolEvents = nfiles;
	printf("%s: will keep a pool of %i events to combine from \n", argv[0], nPoolEvents);
	assert(nPoolEvents < MAXCOINEV);

	/* just to be sure */

	assert(nfiles == nPoolEvents);

	/************************/
	/* open all input files */
	/************************/

	std::vector<std::ifstream*> ifstreamArray;

	unsigned long long int totBytesCount = 0;

	for (int i = 0; i < nfiles; i++)
	{
		nn = i + argcoffset;

		std::ifstream* newIfstreamEntry = new std::ifstream;
		ifstreamArray.push_back(newIfstreamEntry);

		ifstreamArray[i]->open(argv[nn], std::ios_base::in);

		ifstreamArray[i]->seekg(0, ifstreamArray[i]->end);
		unsigned long long fileLength = ifstreamArray[i]->tellg();
		totBytesCount += fileLength;
		ifstreamArray[i]->seekg(0, ifstreamArray[i]->beg);

		InDataInfo* newInDataInfo = new InDataInfo(ifstreamArray[i]);

		if (!newInDataInfo->istream->is_open())
		{
			printf("could not open input data file [%i] %s, quit!\n", nn, argv[nn]);
			exit(1);
		}

		newInDataInfo->fileName = argv[nn];
		newInDataInfo->fileNum = i;
		newInDataInfo->fileLength = fileLength;

		if (newInDataInfo->fileName.find("dgs") != string::npos)
		{
			newInDataInfo->type = 14;
			newInDataInfo->typical_payload_length = 64;
		}
		else if (newInDataInfo->fileName.find("gsfma") != string::npos)
		{
			newInDataInfo->type = 16;
			newInDataInfo->typical_payload_length = 2000;
		}
		else if (newInDataInfo->fileName.find(".geb") != string::npos)
		{
			newInDataInfo->type = 19;
			newInDataInfo->typical_payload_length = 64;
		}
		else cerr << "Unknown file type" << endl;

		inData->push_back(newInDataInfo);

		printf("%s: input data file \"%s\", number %i, is open\n", argv[0], argv[nn], i);
		fflush( stdout);
		control.nOpenFiles++;
		control.fileActive[i] = 1;
	}

	if (printDebug)
	{
		for (unsigned int j = 0; j < theMergeManager->inData->size(); j++)
		{
			if (!theMergeManager->inData->at(j)->istream->is_open())
			{
				std::cerr << theMergeManager->inData->at(j)->fileName << " is NOT open!!\n";
			}
		}
	}

	/* ----------- */
	/* output file */
	/* ----------- */

	/*                        + name of data file */
	/*                        |                   */
	char outName[512];
	sprintf(outName, "%s_%3.3i", argv[2], control.chunkno);

	if (control.zzipout == 0)
	{
		outData->open(outName, std::ios_base::out | std::ios_base::binary);

		if (!outData->is_open())
		{
			printf("could not open output data file %s, quit!\n", outName);
			exit(1);
		}
	}
	else
	{
		int gzdfd = open(outName, O_WRONLY | O_CREAT | O_TRUNC, PMODE);
		zoutData = gzdopen(gzdfd, "w");

		if (zoutData == NULL)
		{
			printf("could not open output data file %s, quit!\n", outName);
			exit(1);
		}
	}

	printf("%s: output data file \"%s\" is open\n", argv[0], outName);
	fflush( stdout);

	/* -------------------- */
	/* read in the map file */
	/* -------------------- */

	if (printDebug) std::cerr << "Reading or creating the map.dat...\n";

	for (int i = 0; i < NCHANNELS; i++)
	{
		theMergeManager->tlkup[i] = NOTHING;
		theMergeManager->tid[i] = NOTHING;
	}

	fp = fopen("map.dat", "r");
	if (fp == NULL)
	{
		printf("need a \"map.dat\" file to run\n");
		int sysRet = system("./mkMap > map.dat");
		if (sysRet == -1)
		{
			std::cerr << "ERROR WHILE MAKING map.dat !!!!!!!!!" << std::endl;
			return -1;
		}
		printf("just made you one...\n");
		fp = fopen("map.dat", "r");
		assert(fp != NULL);
	};

	printf("\nmapping\n");

	i2 = fscanf(fp, "\n%i %i %i %s", &i1, &i7, &i8, str);
	printf("Successfully read %i items: %i %i %i %s\n", i2, i1, i7, i8, str);
	while (i2 == 4)
	{
		theMergeManager->tlkup[i1] = i7;
		theMergeManager->tid[i1] = i8;
		i2 = fscanf(fp, "\n%i %i %i %s", &i1, &i7, &i8, str);
		if (i2 == 4) printf("Successfully read %i items: %i %i %i %s\n", i2, i1, i7, i8, str);
	};
	fclose(fp);

	/* start timer */

	times((struct tms *) &timesThen);

	/* -------------------------------------------- */
	/* read until we have filled our pool of events */
	/* -------------------------------------------- */

	if (printDebug) std::cerr << "Starting the merge procedure... (theMergeManager->inData size is " << theMergeManager->inData->size() << ")\n";

	theMergeManager->loopCounter = 0;

	volatile unsigned long long int evCounter = 0;

	static unsigned int maxEventListSize = 50;

	vector<EVENT*> entriesList;

	auto gebev_comparator = [] ( EVENT* ev1, EVENT* ev2 )
	{
		return ev1->gd->timestamp < ev2->gd->timestamp;
	};

	list<EVENT*> eventQueue, sortingQueue;

	theMergeManager->readBytesCount = 0;

	map<unsigned long long int, unsigned long long int> tsOutputPosMap;

	theMergeManager->goBackToTop = false;

	unsigned long long int weirdTsCounter = 0;

	bool listInitialized = false;

	while (!listInitialized || !eventQueue.empty())
	{
		if (!listInitialized)
		{
			for (int fid = 0; fid < nfiles; fid++)
			{
				for (unsigned int evid = 0; evid < maxEventListSize; evid++)
				{
					EVENT* newEv = new EVENT();

					if (GTGetDiskEv(inData->at(fid), newEv, false) != nullptr)
					{
						newEv->key = fid;
						eventQueue.push_back(newEv);
					}
					else delete newEv;
				}
			}

			eventQueue.sort(gebev_comparator);

			listInitialized = true;

		}

		if (printProgress)
		{
			unsigned long long int outSize = outData->tellp();

			if (theMergeManager->loopCounter % 1000 == 0 || theMergeManager->loopCounter == 1 || alwaysCalcBytesDiff)
			{
				if (theMergeManager->loopCounter % 1000 == 0 || theMergeManager->loopCounter == 1)
				{
#ifdef __unix__
					sysinfo(&memInfo);

					long long totalPhysMem = memInfo.totalram;
					totalPhysMem *= memInfo.mem_unit;

					long long physMemUsed = memInfo.totalram - memInfo.freeram;
					physMemUsed *= memInfo.mem_unit;
#endif

					if (theMergeManager->goBackToTop)
					{
						for (int rl = 0; rl < 5; rl++)
							std::cerr << esc << "[1A";
						std::cerr << "\r" << std::flush;
					}

					string weirdTsWarningLevel;

					if (weirdTsCounter == 0) weirdTsWarningLevel = "[0m";
					else if (weirdTsCounter > 0 && weirdTsCounter <= 20) weirdTsWarningLevel = "[33;1m";
					else if (weirdTsCounter > 20 && weirdTsCounter <= 100) weirdTsWarningLevel = "[38;5;208m";
					else if (weirdTsCounter > 100) weirdTsWarningLevel = "[31;1m";

					std::cerr << esc << "[32;1m" << "Loop #" << theMergeManager->loopCounter << esc << "[0m \n";

					std::cerr << "Bytes read: " << std::right << std::setw(15) << theMergeManager->readBytesCount << " / " << std::left << std::setw(15) << totBytesCount;
					std::cerr << " ( " << std::fixed << std::showpoint << std::setprecision(2) << std::setw(6) << std::right
							<< (float) theMergeManager->readBytesCount / totBytesCount * 100. << "% )\n";

					std::cerr << "Output file size (bytes): " << std::left << std::setw(15) << outSize;

					std::cerr << "Events treated: " << std::setw(15) << evCounter << "\n";

					std::cerr << "Total amount of files: " << std::setw(4) << theMergeManager->inData->size() << " / Files treated: " << theMergeManager->pendingFiles.size()
							<< "\n";

#ifdef __unix
					std::cerr << "Memory used (MB): " << physMemUsed / 1000000 << " ( " << totalPhysMem / 1000000 << " total )\n";
#else
					std::cerr << "Memory used: " << "... memory info is not available for this platform. Go get a real computer.\n";
#endif

					theMergeManager->goBackToTop = true;
				}
			}
		}

		unsigned int coincCounter = 0;
		sortingQueue.clear();

		while (coincCounter < maxEventListSize)
		{
			if (eventQueue.empty()) break;

			evCounter++;
			coincCounter++;

			EVENT* readEv = eventQueue.front();

			int evtLength = readEv->gd->length;
			int fid = readEv->key;

			theMergeManager->outData.write((char*) readEv->gd, sizeof(GebData));
			theMergeManager->outData.write((char*) readEv->payload, evtLength);

			eventQueue.pop_front();

			if (GTGetDiskEv(inData->at(fid), readEv, false) != nullptr) sortingQueue.push_back(readEv);
		}

		sortingQueue.sort(gebev_comparator);

		auto evItr = eventQueue.begin();

		if (eventQueue.size() == 0 && sortingQueue.size() > 0) eventQueue = sortingQueue;
		else if (*evItr != nullptr)
		{
			for (auto qItr = sortingQueue.begin(); qItr != sortingQueue.end(); qItr++)
			{
				while (evItr != eventQueue.end() && (*evItr)->gd->timestamp < (*qItr)->gd->timestamp)
					evItr++;

				eventQueue.insert(evItr, *qItr);
			}
		}

		theMergeManager->loopCounter++;

		if (eventQueue.empty()) cerr << "Event queue is empty: finishing merging process..." << endl;
	}

	theMergeManager->outData.close();

	std::cerr << "\n\nDone Merging the files... Read a total of " << theMergeManager->readBytesCount << " out of " << totBytesCount << " bytes...\n\n";

	return 0;
}

