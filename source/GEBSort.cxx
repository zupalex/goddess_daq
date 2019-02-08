#include "SortManager.h"

using std::string;
using std::vector;

TH2F* mkTH2F ( char* str1, char* str2, int n1, double lo1, double hi1, int n2, double lo2, double hi2, bool recreate )
{
    TH2F* tmppt = nullptr;

    if ( !recreate )
    {
        tmppt = ( TH2F* ) gROOT->FindObject ( str1 );

        if ( tmppt != nullptr ) printf ( "Found Object \"%s\", %p\n", str1, ( void* ) tmppt );
    }

    if ( tmppt == nullptr )
    {
        tmppt = new TH2F ( str1, str2, n1, lo1, hi1, n2, lo2, hi2 );
        printf ( "Created Object \"%s\", %p\n", str1, ( void* ) tmppt );
    }

    return tmppt;
}

TH2F* make2D ( const char* txt, int xln, int xlo, int xhi, int yln, int ylo, int yhi )
{
    char* str = ( char* ) calloc ( STRLEN, sizeof ( char ) );
    ;
    strcpy ( str, txt );
    TH2F* h2D;

//sprintf(str,txt);
    h2D = mkTH2F ( str, str, xln, xlo, xhi, yln, ylo, yhi );

    return h2D;
}

TH1D* mkTH1D ( char* str1, char* str2, int nn, double lo, double hi, bool recreate )
{
    TH1D* tmppt = nullptr;

    if ( !recreate )
    {
        tmppt = ( TH1D* ) gROOT->FindObject ( str1 );

        if ( tmppt != nullptr ) printf ( "Found Object \"%s\", %p\n", str1, ( void* ) tmppt );
    }

    if ( tmppt == nullptr )
    {
        tmppt = new TH1D ( str1, str2, nn, lo, hi );
        printf ( "Created Object \"%s\", %p\n, \"%s\"", str1, ( void* ) tmppt, str2 );
    }

    return tmppt;
}

TH1D* make1D ( const char* txt, int xln, int xlo, int xhi )
{
    char* str = ( char* ) calloc ( STRLEN, sizeof ( char ) );
    strcpy ( str, txt );
    double xlod, xhid;
    TH1D* h1D;

    xlod = xlo;
    xhid = xhi;

    h1D = mkTH1D ( str, str, xln, xlod, xhid );
    return h1D;
}

// ******************************************************************************************** //

int main ( int argc, char** argv )
{
    /*--------------*/
    /* declarations */
    /*--------------*/

    cout<<"Do you make it here?"<<endl;

    SortManager* theSortManager = SortManager::sinstance();
    PARS* pars = theSortManager->execParams;

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

    pars->noCalib = 0;
    pars->ignoreThresholds = 0;
    pars->siDetailLvl = 1;
    pars->noMapping = false;
    pars->noHists = false;
    pars->noDoppler = false;
    pars->userFilter = "";
    pars->triggerMode = "default";
    pars->InputSrc = NOTDEF;
    pars->HaveRootFileName = 0;
    pars->GammaProcessor = 0;
    pars->sphere_split = 0;

    strcpy ( pars->ConfigFile, "Uninitialized" );
    strcpy ( pars->GeomFile, "goddess.geom" );
    sprintf ( pars->ROOTFileOption, "UNDEFINED" );

    pars->sx3EnAdjustFile = "";
    pars->qqq5EnAdjustFile = "";

    pars->GGMAX = 2000;
    pars->ndetlimlo = 1;
    pars->ndetlimhi = 8;
    pars->fomlo[1] = 0;
    pars->fomhi[1] = 2.0;
    pars->UpdateRootFile = 0;
    pars->UseShareMemFile = FALSE;
    pars->StartMapAddress = 0;
    sprintf ( pars->ShareMemFile, "GTSort.map" );

    for ( i = 0; i < MAXDETNO; i++ )
    {
        pars->CCcal_offset[i] = 0;
        pars->CCcal_gain[i] = 1.0;
    }
    for ( i = 0; i < MAXDETPOS; i++ )
    {
        for ( j = 0; j <= MAXCRYSTALNO; j++ )
        {
            pars->SEGcal_gain[i][j] = 1.0;
            pars->SEGcal_offset[i][j] = 0.0;
        }
    }

    /*--------------------*/
    /* parse command line */
    /* and call GEBacq     */
    /*--------------------*/

    j = 1; /* current command line arg position */

    printf ( "we have %i arguments\n", argc );
    fflush ( stdout );

    if ( argc == 1 )
    {
        printf ( "help: see go file for examples of use of GEBSort" );
        exit ( 0 );
    };

    if ( argc > 1 ) while ( j < argc )
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
                printf ( "Coming soon...\n" );
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
//                strcpy (pars->ROOTFileOption, argv[j++]);
                    printf ( "will take input from disk\n" );
                    strcpy ( pars->GTSortInputFile, str3 );
                    pars->InputSrc = DISK;
                    fflush ( stdout );
                }
                else if ( strcmp ( "geb", str2 ) == 0 )
                {
                    strcpy ( pars->pHost, argv[j++] );
                    pars->grouping = atol ( argv[j++] );
                    pars->type = atol ( argv[j++] );
                    pars->timeout = ( float ) atof ( argv[j++] );

                    printf ( "pars->pHost=%s\n", pars->pHost );
                    printf ( "pars->grouping=%i\n", pars->grouping );
                    printf ( "pars->type=%i\n", pars->type );
                    printf ( "pars->timeout=%f\n", pars->timeout );
                    pars->InputSrc = GEB;
//                strcpy (pars->ROOTFileOption, argv[j++]);
                    printf ( "root file option: %s\n", pars->ROOTFileOption );
#if(HAVE_VXWORKS==0)
                    printf ( "oppsie... you cannot specify this option unless\n" );
                    printf ( "you have #define HAVE_VXWORKS 1 in GEBSort.h\n" );
                    printf ( "and have a VxWorks license, quit\n" );
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
                sscanf ( argv[j++], "%llu", &pars->nEvents );
                printf ( "Overriding the amount of events which will be treated: %llu\n", pars->nEvents );
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
                strcpy ( pars->ConfigFile, argv[j++] );
                printf ( "will read config file from: %s ...\n", pars->ConfigFile );
            }
            else if ( ( p = strstr ( argv[j], "-geom" ) ) != NULL )
            {
                j++;
                strcpy ( pars->GeomFile, argv[j++] );
                printf ( "will read geometry from: %s ...\n", pars->GeomFile );
            }
            else if ( ( p = strstr ( argv[j], "-sx3enadjust" ) ) != NULL )
            {
                j++;
                pars->sx3EnAdjustFile = ( string ) argv[j++];
                printf ( "will apply a correction to the SX3 energies based on the following file: %s ...\n", pars->sx3EnAdjustFile.c_str() );
            }
            else if ( ( p = strstr ( argv[j], "-GammaProcessor" ) ) != NULL )
            {
                j++;
                sscanf ( argv[j++], "%i", &pars->GammaProcessor );
                printf ( "The GammaProcessor will be: %i", pars->GammaProcessor );
            }
            else if ( ( p = strstr ( argv[j], "-sphere_split" ) ) != NULL )
            {
                j++;
                sscanf ( argv[j++], "%f", &pars->sphere_split);
                printf ( "The sphere split will be: %f", pars->sphere_split );
            }
            else if ( ( p = strstr ( argv[j], "-qqq5enadjust" ) ) != NULL )
            {
                j++;
                pars->qqq5EnAdjustFile = ( string ) argv[j++];
                printf ( "will apply a correction to the QQQ5 energies based on the following file: %s ...\n", pars->qqq5EnAdjustFile.c_str() );
            }
            else if ( ( p = strstr ( argv[j], "-nocalib" ) ) != NULL )
            {
                j++;
                sscanf ( argv[j++], "%hd", &pars->noCalib );

                if ( pars->noCalib > 0 )
                {
                    string outMessage;

                    switch ( pars->noCalib )
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
                sscanf ( argv[j++], "%hu", &pars->siDetailLvl );

                string outMessage;

                switch ( pars->siDetailLvl )
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
                sscanf ( argv[j++], "%hu", &pars->ignoreThresholds );

                if ( pars->ignoreThresholds > 0 )
                {
                    string outMessage;

                    switch ( pars->noCalib )
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
                pars->noMapping = true;
            }
            else if ( ( p = strstr ( argv[j], "-nohists" ) ) != NULL )
            {
                j++;
                printf ( "No pre-made histograms will be generated and written to the file\n" );
                pars->noHists = true;
            }
            else if ( ( p = strstr ( argv[j], "-nodoppler" ) ) != NULL )
            {
                j++;
                printf ( "Doppler correction won't be performed\n" );
                pars->noDoppler = true;
            }
            else if ( ( p = strstr ( argv[j], "-userfilter" ) ) != NULL )
            {
                j++;
                char filteredName[500];
                strcpy ( filteredName, argv[j++] );

                pars->userFilter = filteredName;

                printf ( "The UserEventFilter will be applied to generate the root file\n" );
            }
            else if ( ( p = strstr ( argv[j], "-triggermode" ) ) != NULL )
            {
                j++;
                char trigMode[500];
                strcpy ( trigMode, argv[j++] );

                pars->triggerMode = trigMode;

                printf ( "The TRIGGER MODE is %s\n", trigMode );
            }
            else if ( ( p = strstr ( argv[j], "-rootfile" ) ) != NULL )
            {
                j++;
                strcpy ( pars->ROOTFile, argv[j++] );
                printf ( "rootfile name specified on command line\n" );
                printf ( "__will store spectra in rootfile: %s\n", pars->ROOTFile );
                pars->HaveRootFileName = 1;
                pars->UseRootFile = 1;
                if ( ( p = strstr ( argv[j], "RECREATE" ) ) != NULL )
                {
                    pars->UpdateRootFile = FALSE;
                    printf ( "will recreate root file\n" );
                    sprintf ( pars->ROOTFileOption, "RECREATE" );
                    j++;
                }
                else if ( ( p = strstr ( argv[j], "UPDATE" ) ) != NULL )
                {
                    pars->UpdateRootFile = TRUE;
                    printf ( "will update root file\n" );
                    sprintf ( pars->ROOTFileOption, "UPDATE" );
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
                pars->UseRootFile = 0;
                strcpy ( pars->ShareMemFile, argv[j++] );
                sscanf ( argv[j++], "%i", &pars->SizeShareMemFile );
                printf ( "will use shared memory file: %s\n", pars->ShareMemFile );
                printf ( "__of max size: %i bytes\n", pars->SizeShareMemFile );
                pars->UseShareMemFile = 1;
                sscanf ( argv[j++], "0x%x", &pars->StartMapAddress );
                printf ( "will start shared mem at address: 0x%8.8x\n", pars->StartMapAddress );
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
                GetLocalTimeAndDate ( true );
                exit ( 0 );

            }
        };

    /* checking if the config file has been specified. If not then we auto-assign a config file based on the run number */
    if ( strcmp ( pars->ConfigFile, "Uninitialized" ) == 0 )
    {
        string inFileName = pars->GTSortInputFile;

        short inRunPos1 = inFileName.find ( "run", 0 ) + 3;
        short inRunPos2 = inFileName.find ( ".gtd", 0 );

        int treatedRun = stoi ( inFileName.substr ( inRunPos1, inRunPos2 - inRunPos1 ) );
        pars->runNumber = treatedRun;

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

        if ( strcmp ( pars->ConfigFile, "Uninitialized" ) == 0 ) std::cerr << "Starting auto config file picker..." << std::endl;

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
                strcpy ( pars->ConfigFile, ( configFileList[fItr].substr ( 2, sdRunEndPos + 5 ) ).c_str() ); // the last character we want is at the position sdRunEndPos + 7 but we start from position 2...

                std::cerr << "matching!" << std::endl;

                break;
            }

            std::cerr << "not matching... keep on looking..." << std::endl;

            if ( configFileList[fItr] == "goddess.config" ) strcpy ( backupConf, "goddess.config" );
        }

        if ( strcmp ( pars->ConfigFile, "Uninitialized" ) == 0 )
        {
            std::cerr << "UNABLED TO RETRIEVE CONFIG FILE AUTOMATICALLY" << std::endl;
            std::cerr << "SPECIFY IT OR CHECK THAT ONE EXISTS FOR THE RUN YOU'RE TRYING TO TREAT" << std::endl;
            std::cerr << "Will now try to load the default config file \"goddess.config\"" << std::endl;

            if ( strcmp ( backupConf, "goddess.config" ) ) strcpy ( pars->ConfigFile, "goddess.config" );
            else return -1;
        }

        printf ( "\nwill read config file from: %s ...\n", pars->ConfigFile );

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
