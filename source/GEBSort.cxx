#include "ProcessManagers.h"

using std::string;
using std::vector;

SortManager* SortManager::s_instance = nullptr;

int main ( int argc, char** argv )
{
    /*--------------*/
    /* declarations */
    /*--------------*/

    SortManager* theSortManager = SortManager::sinstance();
    PARS* Pars = theSortManager->execParams;
    
    int j, i, HaveChatFile = 0;
    char* p;
    char ChatFileName[STRLEN];
    char str2[STRLEN], str3[STRLEN];

    /*------*/
    /* help */
    /*------*/

    if ( argc < 2 )
    {
        printf ( "\n" );
        printf ( "use: %s -chat file [-help] [-version] .... TBD\n", argv[0] );
        printf ( "\n" );
        return ( 0 );
    };

    /* initialize */

    Pars->noCalib = 0;
    Pars->ignoreThresholds = 0;
    Pars->siDetailLvl = 1;
    Pars->noMapping = false;
    Pars->noHists = false;
    Pars->userFilter = "";
    Pars->InputSrc = NOTDEF;
    Pars->HaveRootFileName = 0;
    strcpy ( Pars->ConfigFile, "Uninitialized" );
    sprintf ( Pars->ROOTFileOption, "UNDEFINED" );
    Pars->GGMAX = 2000;
    Pars->ndetlimlo = 1;
    Pars->ndetlimhi = 8;
    Pars->fomlo[1] = 0;
    Pars->fomhi[1] = 2.0;
    Pars->UpdateRootFile = 0;
    Pars->UseShareMemFile = FALSE;
    Pars->StartMapAddress = 0;
    sprintf ( Pars->ShareMemFile, "GTSort.map" );

    for ( i = 0; i < MAXDETNO; i++ )
    {
        Pars->CCcal_offset[i] = 0;
        Pars->CCcal_gain[i] = 1.0;
    }
    for ( i = 0; i < MAXDETPOS; i++ )
    {
        for ( j = 0; j <= MAXCRYSTALNO; j++ )
        {
            Pars->SEGcal_gain[i][j] = 1.0;
            Pars->SEGcal_offset[i][j] = 0.0;
        }
    }

    /*--------------------*/
    /* Parse command line */
    /* and call GEBacq     */
    /*--------------------*/

    j = 1;                        /* current command line arg position */

    printf ( "we have %i arguments\n", argc );
    fflush ( stdout );

    if ( argc == 1 )
    {
        printf ( "help: see go file for examples of use of GEBSort" );
        exit ( 0 );
    };

    if ( argc > 1 )
        while ( j < argc )
        {
            printf ( "%i... %s\n", j, argv[j] );
            fflush ( stdout );

            if ( ( p = strstr ( argv[j], "-version" ) ) != NULL )
            {
                printf ( "try: svn info\n" );
                exit ( 0 );
            }
            else if ( ( p = strstr ( argv[j], "-help" ) ) != NULL )
            {
                printf ( "\n" );
                printf ( "GEBSort is documented on the WWW, URL:\n" );
                printf ( "\n" );
                printf ( "http://wiki.anl.gov/gretina_at_anl \n" );
                printf ( "\n" );
                exit ( 0 );

            }
            else if ( ( p = strstr ( argv[j], "-input" ) ) != NULL )
            {

                /* check that user specified enough arguments */

                j++;

                /* determine input source */

                strcpy ( str2, argv[j++] );


                if ( strcmp ( "disk", str2 ) == 0 )
                {
                    strcpy ( str3, argv[j++] );
//                strcpy (Pars->ROOTFileOption, argv[j++]);
                    printf ( "will take input from disk\n" );
                    strcpy ( Pars->GTSortInputFile, str3 );
                    Pars->InputSrc = DISK;
                    fflush ( stdout );
                }
                else if ( strcmp ( "geb", str2 ) == 0 )
                {
                    strcpy ( Pars->pHost, argv[j++] );
                    Pars->grouping = atol ( argv[j++] );
                    Pars->type = atol ( argv[j++] );
                    Pars->timeout = ( float ) atof ( argv[j++] );

                    printf ( "Pars->pHost=%s\n", Pars->pHost );
                    printf ( "Pars->grouping=%i\n", Pars->grouping );
                    printf ( "Pars->type=%i\n", Pars->type );
                    printf ( "Pars->timeout=%f\n", Pars->timeout );
                    Pars->InputSrc = GEB;
//                strcpy (Pars->ROOTFileOption, argv[j++]);
                    printf ( "root file option: %s\n", Pars->ROOTFileOption );
#if(HAVE_VXWORKS==0)
                    printf ( "oppsie... you cannot specify this option unless\n" );
                    printf ( "you have #define HAVE_VXWORKS 1 in GEBSort.h\n" );
                    printf ( "and ahev a VxWorks license, quit\n" );
                    exit ( 0 );
#endif
                }
                else
                {
                    printf ( "unknown input option: %s\n", str2 );
                    printf ( "aborting\n" );
                    fflush ( stdout );
                    exit ( 0 );
                };

                printf ( "\n" );

            }
            else if ( ( p = strstr ( argv[j], "-nevent" ) ) != NULL )
            {
                j++;
                sscanf ( argv[j++], "%llu", &Pars->nEvents );
                printf ( "Overriding the amount of events which will be treated: %llu\n", Pars->nEvents );
            }
            else if ( ( p = strstr ( argv[j], "-chat" ) ) != NULL )
            {
                j++;
                strcpy ( ChatFileName, argv[j++] );
                printf ( "will read sorting instructions from chatfile: %s\n", ChatFileName );
                int sysRet = system ( "ulimit -a" );
                if ( sysRet == -1 )
                {
                    std::cerr << "ERROR WHILE READING SORTING INSTRUCTIONS FROM CHATFILE !!!!!" << std::endl;
                    return -1;
                }
                HaveChatFile = 1;
            }
            else if ( ( p = strstr ( argv[j], "-config" ) ) != NULL )
            {
                j++;
                strcpy ( Pars->ConfigFile, argv[j++] );
                printf ( "will read config file from: %s ...\n", Pars->ConfigFile );
            }
            else if ( ( p = strstr ( argv[j], "-nocalib" ) ) != NULL )
            {
                j++;
                sscanf ( argv[j++], "%hd", &Pars->noCalib );

                if ( Pars->noCalib > 0 )
                {
                    string outMessage;

                    switch ( Pars->noCalib )
                    {
                    case -1:
                        outMessage = "no sorted tree will be written to the file";
                        break;
                    case 1:
                        outMessage = "only an uncalibrated sorted tree will be added to the file";
                        break;
                    case 2:
                        outMessage = "an uncalibrated sorted treewill be added to the file along with the calibrated one";
                        break;
                    }

                    printf ( "/!\\ will process the run without applying the calibration parameters and %s /!\\\n", outMessage.c_str() );
                }
            }
            else if ( ( p = strstr ( argv[j], "-siDetailLvl" ) ) != NULL )
            {
                j++;
                sscanf ( argv[j++], "%hu", &Pars->siDetailLvl );

                string outMessage;

                switch ( Pars->siDetailLvl )
                {
                case 0:
                    outMessage = "The si branch won't be written to the rootfile!!!!!";
                    break;
                case 1:
                    outMessage = "Si Detectors: Only the sum of the energies of each sectors will be written to the file";
                    break;
                case 2:
                    outMessage = "Si Detectors: All the energies will be written to the file as well as their sum";
                    break;
                }

                printf ( "%s \n", outMessage.c_str() );
            }
            else if ( ( p = strstr ( argv[j], "-ignorethrs" ) ) != NULL )
            {
                j++;
                sscanf ( argv[j++], "%hu", &Pars->ignoreThresholds );

                if ( Pars->ignoreThresholds > 0 )
                {
                    string outMessage;

                    switch ( Pars->noCalib )
                    {
                    case 1:
                        outMessage = "Thresholds won't be applied to the raw and/or sorted tree(s)";
                        break;
                    case 2:
                        outMessage = "No thresholds will be applied to any tree(s)";
                        break;
                    }

                    printf ( "%s\n", outMessage.c_str() );
                }
            }
            else if ( ( p = strstr ( argv[j], "-nomapping" ) ) != NULL )
            {
                j++;
                printf ( "raw tree with the pairs <channel, value> will be added to the file\n" );
                Pars->noMapping = true;
            }
            else if ( ( p = strstr ( argv[j], "-nohists" ) ) != NULL )
            {
                j++;
                printf ( "No pre-made histograms will be generated and written to the file\n" );
                Pars->noHists = true;
            }
            else if ( ( p = strstr ( argv[j], "-userfilter" ) ) != NULL )
            {
                j++;
                char filteredName[500];
                strcpy ( filteredName, argv[j++] );

                Pars->userFilter = filteredName;

                printf ( "An additional binary file will be generated using the user filters\n" );
            }
            else if ( ( p = strstr ( argv[j], "-rootfile" ) ) != NULL )
            {
                j++;
                strcpy ( Pars->ROOTFile, argv[j++] );
                printf ( "rootfile name specified on command line\n" );
                printf ( "__will store spectra in rootfile: %s\n", Pars->ROOTFile );
                Pars->HaveRootFileName = 1;
                Pars->UseRootFile = 1;
                if ( ( p = strstr ( argv[j], "RECREATE" ) ) != NULL )
                {
                    Pars->UpdateRootFile = FALSE;
                    printf ( "will recreate root file\n" );
                    sprintf ( Pars->ROOTFileOption, "RECREATE" );
                    j++;
                }
                else if ( ( p = strstr ( argv[j], "UPDATE" ) ) != NULL )
                {
                    Pars->UpdateRootFile = TRUE;
                    printf ( "will update root file\n" );
                    sprintf ( Pars->ROOTFileOption, "UPDATE" );
                    j++;
                }
                else
                {
                    printf ( " you must specify RECREATE or UPDATE after -rootfile\n" );
                    exit ( 0 );
                }
            }
            else if ( ( p = strstr ( argv[j], "-mapfile" ) ) != NULL )
            {
                j++;
                Pars->UseRootFile = 0;
                strcpy ( Pars->ShareMemFile, argv[j++] );
                sscanf ( argv[j++], "%i", &Pars->SizeShareMemFile );
                printf ( "will use shared memory file: %s\n", Pars->ShareMemFile );
                printf ( "__of max size: %i bytes\n", Pars->SizeShareMemFile );
                Pars->UseShareMemFile = 1;
                sscanf ( argv[j++], "0x%x", &Pars->StartMapAddress );
                printf ( "will start shared mem at address: 0x%8.8x\n", Pars->StartMapAddress );
//if(1)exit(0);
            }
            else
            {
                printf ( "command line argument not understood!\n" );

                printf ( "%s: I was called as: \n--->[", argv[0] );
                for ( i = 0; i < argc; i++ )
                {
                    printf ( "%s ", argv[i] );
                    fflush ( stdout );
                }
                printf ( "]\non " );
                time_stamp();
                exit ( 0 );

            }
        };

    /* checking if the config file has been specified. If not then we auto-assign a config file based on the run number */
    if ( strcmp ( Pars->ConfigFile, "Uninitialized" ) == 0 )
    {
        string inFileName = Pars->GTSortInputFile;

        short inRunPos1 = inFileName.find ( "run", 0 ) + 3;
        short inRunPos2 = inFileName.find ( ".gtd", 0 );

        int treatedRun = stoi ( inFileName.substr ( inRunPos1, inRunPos2 - inRunPos1 ) );

        printf ( "\nConfig File left has not been initialized... Trying to retrieve it for run# %d\n", treatedRun );

        vector<string> configFileList;
        configFileList.clear();

        std::FILE* dir_scan;
        char buff[512];

        dir_scan = popen ( "ls ./*.config", "r" );

        while ( fgets ( buff, sizeof ( buff ), dir_scan ) != NULL )
        {
            configFileList.push_back ( buff );
        }

        char backupConf[128];

        if ( strcmp ( Pars->ConfigFile, "Uninitialized" ) == 0 ) std::cerr << "Starting auto config file picker..." << std::endl;

        for ( unsigned short fItr = 0; fItr < configFileList.size(); fItr++ )
        {
            std::size_t fstRunStartPos = configFileList[fItr].find ( "runs", 0 );
            std::size_t separatorPos = configFileList[fItr].find ( "_to_", 0 );
            std::size_t sdRunEndPos = configFileList[fItr].find ( ".config", 0 );

            if ( fstRunStartPos == std::string::npos || separatorPos == std::string::npos || sdRunEndPos == std::string::npos ) continue;

            std::cerr << "Found config file: " << configFileList[fItr] << " ... ";

            int lowBound = std::stoi ( configFileList[fItr].substr ( fstRunStartPos + 4, separatorPos - fstRunStartPos + 4 ) );
            int upBound = std::stoi ( configFileList[fItr].substr ( separatorPos + 4, sdRunEndPos - separatorPos + 4 ) );

            if ( treatedRun >= lowBound && treatedRun <= upBound )
            {
                strcpy ( Pars->ConfigFile, ( configFileList[fItr].substr ( 2, sdRunEndPos + 5 ) ).c_str() ); // the last character we want is at the position sdRunEndPos + 7 but we start from position 2...

                std::cerr << "matching!" << std::endl;

                break;
            }

            std::cerr << "not matching... keep on looking..." << std::endl;

            if ( configFileList[fItr] == "goddess.config" ) strcpy ( backupConf, "goddess.config" );
        }

        if ( strcmp ( Pars->ConfigFile, "Uninitialized" ) == 0 )
        {
            std::cerr << "UNABLED TO RETRIEVE CONFIG FILE AUTOMATICALLY" << std::endl;
            std::cerr << "SPECIFY IT OR CHECK THAT ONE EXISTS FOR THE RUN YOU'RE TRYING TO TREAT" << std::endl;
            std::cerr << "Will now try to load the default config file \"goddess.config\"" << std::endl;

            if ( strcmp ( backupConf, "goddess.config" ) ) strcpy ( Pars->ConfigFile, "goddess.config" );
            else return -1;
        }

        printf ( "\nwill read config file from: %s ...\n", Pars->ConfigFile );

    }

    /* now start the sorter */
    if ( HaveChatFile )
    {
        theSortManager->GEBacq ( ChatFileName );
    }
    else
    {
        printf ( "you must specify a chat script\n" );
        return 0;
    }

}