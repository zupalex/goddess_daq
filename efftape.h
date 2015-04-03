/* $Id: efftape.h,v 1.1 2013/04/10 16:34:45 tl Exp $ */

/************************************************************************
       DEFINE TAPE HEADER RECORD
*************************************************************************/
typedef struct tape_header
    {
    unsigned short int RecordType;	/* tape header type= 1             */
    unsigned short int RecordLength;	/* number of bytes in this record  */
    unsigned short int RecordVer;	/* record version or subtype       */
    unsigned short int  ByteOrder_1;	/* to determine byte ordering      */
    unsigned short int  ByteOrder_2;	/* to determine byte ordering      */
    char    exp_title1[40];	        /* first part of exp title         */
    char    exp_title2[40];	        /* 2nd  part of exp title          */
    char    time[9];		        /* hh:mm:ss			   */
    char    date[9];		        /* yy/mm/dd			   */
    unsigned short int TapeNum;		/* tape number in this experiment  */
    unsigned short int TapeUnit;	/* specifies which unit wrote this tape */
}       TAPE_HEADER;

/************************************************************************
       DEFINE FILE HEADER RECORD
*************************************************************************/
typedef struct file_header
    {
    unsigned short int RecordType;	/* file header type= 2             */
    unsigned short int RecordLength;	/* number of bytes in this record  */
    unsigned short int RecordVer;	/* record version or subtype       */
    unsigned short int RunNumber;	/* run number in this experiment   */
    unsigned short int FileNumber;	/* file number in this experiment  */
    char    run_title1[40];		/* first part of run title         */
    char    run_title2[40];		/* 2nd  part of run title          */
}       FILE_HEADER;

/************************************************************************
       DEFINE EVENT DATA RECORD
*************************************************************************/
#define EB_CKSUM_NONE  0	/* no checksum                          */
#define EB_CKSUM_SUM16 1	/* 16 but simple checksum               */
#define EB_CKSUM_CRC16 2	/* crc16 checksum                       */
#define EB_HEADER_LEN  22       /* number of bytes in event header      */
#define EB_SIZE  (8192-11)	/* number of data words in event buffer */
typedef struct event_buffer
    {
    unsigned short int RecordType;		/* 1 event data type= 3              */
    unsigned short int RecordLength;		/* 2 number of bytes in this record  */
    unsigned short int RecordVer;		/* 3 record version or subtype       */
    unsigned short int HeaderBytes;		/* 4 number of bytes in header       */
    unsigned short int EffNumber;		/* 5 eff processor number            */
    unsigned short int StreamID;		/* 6 event stream ID                 */
    unsigned short int EffSequence;		/* 7 eff sequence number             */
    unsigned short int ModeFlags;		/* 8 event format flags              */
    unsigned short int DataLength;		/* 9 number of i*2 data words        */
    unsigned short int ChecksumType;		/* 10 type of checksum               */
    unsigned short int Checksum;		/* 11 checksum value                 */
    unsigned short int EventData[EB_SIZE];	/* event data area                   */
}       EVENT_BUFFER;

/************************************************************************
       DEFINE LOG DATA RECORD
*************************************************************************/
typedef struct log_record
    {
    unsigned short int RecordType;		/* 1 event data type= 4              */
    unsigned short int RecordLength;		/* 2 number of bytes in this record  */
    unsigned short int RecordVer;		/* 3 record version or subtype       */
    int     LineCount;				/* Number of text lines following    */
    char    Line1[80];
    char    Line2[80];
    char    Line3[80];
    char    LineN[80];
}       LOG_RECORD;

/************************************************************************
       DEFINE SCALER DATA RECORD
*************************************************************************/
typedef struct scaler_record
    {
    unsigned short int RecordType;		/* 1 event data type= 5              */
    unsigned short int RecordLength;		/* 2 number of bytes in this record  */
    unsigned short int RecordVer;		/* 3 record version or subtype       */
    int    ScalerCount;				/* Number of scalers to follow     */
    int    Scaler1_ID;				/* Scaler ID                       */
    int    Scaler1;				/* Scaler value                    */
    int    Scaler2_ID;				/* Scaler ID                       */
    int    Scaler2;				/* Scaler value                    */
}       SCALER_RECORD;


/************************************************************************
       DEFINE CALIBRATION RECORD
*************************************************************************/
typedef struct calibration_record
    {
    unsigned short int RecordType;		/* 1 event data type= 6              */
    unsigned short int RecordLength;		/* 2 number of bytes in this record  */
    unsigned short int RecordVer;		/* 3 record version or subtype       */
    /* to be determined */
}       CALIBRATION_RECORD;


/************************************************************************
       DEFINE SOURCE CODE RECORD
*************************************************************************/
typedef struct source_record
    {
    unsigned short int RecordType;		/* 1 event data type= 7              */
    unsigned short int RecordLength;		/* 2 number of bytes in this record  */
    unsigned short int RecordVer;		/* 3 record version or subtype       */
    int    LineCount;				/* Number of text lines following  */
    char    Line1[80];
    char    Line2[80];
    char    Line3[80];
    char    LineN[80];
}       SOURCE_RECORD;

/************************************************************************
       DEFINE FRONT END HISTOGRAM RECORD
*************************************************************************/
        typedef struct histogram_record
    {
    unsigned short int RecordType;		/* 1 event data type= 8              */
    unsigned short int RecordLength;		/* 2 number of bytes in this record  */
    unsigned short int RecordVer;		/* 3 record version or subtype       */
    /*to be determined ..if we use this at all*/
}       HISTOGRAM_RECORD;

