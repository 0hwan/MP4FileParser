#ifndef OS_HEADER_H
#define OS_HEADER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <limits.h>

#define kSInt16_Max USHRT_MAX
#define kUInt16_Max USHRT_MAX

#define kSInt32_Max LONG_MAX
#define kUInt32_Max ULONG_MAX

#define kSInt64_Max LONG_LONG_MAX
#define kUInt64_Max ULONG_LONG_MAX

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

#define DECODER_ERROR_CODE    1000
#define STREAM_ERROR_CODE    2000
typedef enum
{
    /* Common Code */
    /*! Operation success (no error).*/
    ISOM_OK								= 0,
    /*! Normal Error*/
    ISOM_ERROR                            = 1,
    /*! None OK*/
    ISOM_DECLINE                          = 2,
    /*! Unknown Error*/
    ISOM_UNKNOWN                          = 3,
    
    
    /*! Memory allocation failure.*/
    ISOM_MEMORY_ERROR					 = 1011,
    /*!One of the input parameter is not correct or cannot be used in the current operating mode of the framework.*/
    ISOM_BAD_PARAM						 = 1012,
    /*! file open failure */
    ISOM_FILE_OPEN_ERROR			         = 1013,    
    /*! Input/Output failure (disk access, system call failures)*/
    ISOM_FILE_IO_ERROR				     = 1014,
    /*! data_resource file read length fail*/
    ISOM_FILE_READ_SIZE_MISMATCH	         = 1015,
    /*! data resource memory read length fail*/
    ISOM_MEMORY_READ_SIZE_MISMATCH         = 1016,
    ISOM_DATA_RESOURCE_BAD_PARAM           = 1017,
    
    ISOM_SAMPLE_COUNT_MISMATCH             = 1018,
    ISOM_FILE_OFFSET_AU_SIZE_MISMATCH      = 1019,


    /* about MP4File */
    ISOM_FILE_BOX_SIZE_MISMATCH        = 1201,
    ISOM_FILE_INVALID_BOX_SIZE         = 1202,
    /*! An invalid MPEG-4 Object Descriptor was found*/
    ISOM_FILE_NOT_FOUND_MOOV_BOX       = 1211,
    ISOM_FILE_NOT_FOUND_MVHD_BOX       = 1212,
    ISOM_FILE_NOT_FOUND_TRAK_BOX       = 1213,
    ISOM_FILE_NOT_FOUND_TKHD_BOX       = 1214,
    ISOM_FILE_NOT_FOUND_MDIA_BOX       = 1215,
    ISOM_FILE_NOT_FOUND_MDHD_BOX       = 1216,
    ISOM_FILE_NOT_FOUND_HDLR_BOX       = 1217,
    ISOM_FILE_NOT_FOUND_MINF_BOX       = 1218,
    ISOM_FILE_NOT_FOUND_STBL_BOX       = 1219,
    ISOM_FILE_NOT_FOUND_STISOM_BOX     = 1220,
    ISOM_FILE_NOT_FOUND_CTISOM_BOX     = 1221,
    ISOM_FILE_NOT_FOUND_STSC_BOX       = 1222,
    ISOM_FILE_NOT_FOUND_STCO_BOX       = 1223,
    ISOM_FILE_NOT_FOUND_STSZ_BOX       = 1224,
    ISOM_FILE_NOT_FOUND_STSS_BOX       = 1225,
    ISOM_FILE_NOT_FOUND_STSD_BOX       = 1226,
    
    ISOM_FILE_INVALID_MOOV_BOX         = 1311,    
    ISOM_FILE_INVALID_MVHD_BOX         = 1312,
    ISOM_FILE_INVALID_TRAK_BOX         = 1313,
    ISOM_FILE_INVALID_TKHD_BOX         = 1314,
    ISOM_FILE_INVALID_MDIA_BOX         = 1315,
    ISOM_FILE_INVALID_MDHD_BOX         = 1316,
    ISOM_FILE_INVALID_HDLR_BOX         = 1317,
    ISOM_FILE_INVALID_MINF_BOX         = 1318,
    ISOM_FILE_INVALID_STBL_BOX         = 1319,
    ISOM_FILE_INVALID_STISOM_BOX       = 1320,
    ISOM_FILE_INVALID_CTISOM_BOX       = 1321,
    ISOM_FILE_INVALID_STSC_BOX         = 1322,
    ISOM_FILE_INVALID_STCO_BOX         = 1323,
    ISOM_FILE_INVALID_STSZ_BOX         = 1324,
    ISOM_FILE_INVALID_STSS_BOX         = 1325,
    ISOM_FILE_INVALID_STSD_BOX         = 1326,
    
    ISOM_CODEC_BOX_SIZE_MISMATCH          = 1401,
    ISOM_UNKNOWN_CODEC_BOX                = 1402,
    ISOM_CODEC_BOX_NULL                   = 1403,
    ISOM_ADD_CODEC_BOX_FAIL               = 1404,
    ISOM_MP4V_BOX_SET_FAIL                = 1451,
    ISOM_MP4A_BOX_SET_FAIL                = 1452,
    ISOM_ESDS_BOX_SET_FAIL                = 1453,
    ISOM_AVC1_BOX_SET_FAIL                = 1454,
    ISOM_AVCC_BOX_SET_FAIL                = 1455,
    ISOM_BTRT_BOX_SET_FAIL                = 1456,
    ISOM_M4DS_BOX_SET_FAIL                = 1457,
    ISOM_3GPA_BOX_SET_FAIL                = 1458,
    ISOM_3GPV_BOX_SET_FAIL                = 1459,
    ISOM_3GPC_BOX_SET_FAIL                = 1460,


    /* about decoding */
    ISOM_CODEC_COUNT_OVER                 = 2101,
    ISOM_UNKNOWN_CODEC_TYPE               = 2102,
    ISOM_INVALID_TRACK_DECODE_INFO        = 2103,
    ISOM_INVALID_TRACK_STREAM_INFO        = 2104,
    ISOM_INVALID_MP4A_DSI_CONFIG          = 2105,
    ISOM_INVALID_MP4V_DSI_CONFIG          = 2106,
    /* bad param error in isom_get_computed_dts()*/
    ISOM_COMPUTE_DTS_BAD_PARAM            = 2111,
    /* return 'fail' from isom_get_sample_number_around_dts_from_stts() in isom_get_computed_dts()*/
    ISOM_COMPUTE_DTS_AROUND_STTS_FAIL     = 2112,
    /* return 'fail' from isom_get_sample_dts_from_stts() in isom_get_computed_dts()*/
    ISOM_COMPUTE_DTS_RAP_FIND_STTS        = 2113,
    /* return 'fail' from isom_get_sample_dts_from_stts() in isom_get_computed_dts()*/
    ISOM_COMPUTE_DTS_STTS                 = 2114,


    /* about packetizing */
    /*! 3013 file open failure */
    ISOM_STREAM_FILE_OPEN_ERROR	        = ISOM_FILE_OPEN_ERROR                + STREAM_ERROR_CODE,      
    /*! 3014 Input/Output failure (disk access, system call failures)*/
    ISOM_STREAM_FILE_IO_ERROR 		     = ISOM_FILE_IO_ERROR                 + STREAM_ERROR_CODE,
    /*! 3015 data_resource file read length fail*/
    ISOM_STREAM_FILE_READ_SIZE_MISMATCH	= ISOM_FILE_READ_SIZE_MISMATCH        + STREAM_ERROR_CODE,
    /*! 3016 data resource memory read length fail*/
    ISOM_STREAM_MEMORY_READ_SIZE_MISMATCH = ISOM_MEMORY_READ_SIZE_MISMATCH      + STREAM_ERROR_CODE,
    /* 3017 */
    ISOM_STREAM_DATA_RESOURCE_BAD_PARAM   = ISOM_DATA_RESOURCE_BAD_PARAM        + STREAM_ERROR_CODE,
    /* 3018 */
    ISOM_STREAM_SAMPLE_COUNT_MISMATCH     = ISOM_SAMPLE_COUNT_MISMATCH          + STREAM_ERROR_CODE,
    /* 3019 */
    ISOM_STREAM_FILE_OFFSET_AU_SIZE_MISMATCH = ISOM_FILE_OFFSET_AU_SIZE_MISMATCH + STREAM_ERROR_CODE,

    /*!Indicates the end of a stream (Info).*/
    ISOM_EOS								= 3101,
    /*! The desired stream could not be found in the service*/
    ISOM_TRACK_STREAM_INFO_NOT_FOUND		= 3102,
    ISOM_PACKETIZE_MPEG4_ERROR            = 3103,
    /* return 'fail' from isom_get_sample_dts_from_stts() in isom_set_access_unit_info()*/
    ISOM_SET_ACCESS_UNIT_INFO_STTS_FAIL   = 3111,
    /* return 'fail' from isom_get_sample_cts_from_ctts() in isom_set_access_unit_info()*/
    ISOM_SET_ACCESS_UNIT_INFO_CTTS_FAIL   = 3112,
    /* return 'fail' from isom_get_sample_size_from_stsz() in isom_set_access_unit_info()*/
    ISOM_SET_ACCESS_UNIT_INFO_STSZ_FAIL    = 3113,
    /* return 'fail' from isom_get_sample_random_access_point_from_stss() in isom_set_access_unit_info()*/
    ISOM_SET_ACCESS_UNIT_INFO_STSS_RAP_FAIL = 3114,
    /* bad param error in  isom_get_access_unit_info()*/
    ISOM_GET_ACCESS_UNIT_INFO_BAD_PARAM     = 3115,
    /* bad param error in  isom_get_access_unit_info()*/
    ISOM_SET_ACCESS_UNIT_INFO_BAD_PARAM     = 3116,
    ISOM_GET_CHUNKNUMBER_FAIL               = 3117,
} rs_status_t;

/* Platform-specific components */
#if LINUX
    
    /* Defines */
    #define _64BITARG_ "q"

    /* paths */
    #define kEOLString "\n"
    #define kPathDelimiterString "/"
    #define kPathDelimiterChar '/'
    #define kPartialPathBeginsWithDelimiter 0

    /* Includes */
    #include <arpa/inet.h>

    #include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #include <fcntl.h>

    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include <ctype.h>
    
    /* Constants */
    #define QT_TIME_TO_LOCAL_TIME   (-2082844800)
    #define QT_PATH_SEPARATOR       '/'

    /* Typedefs */
    typedef unsigned int        PointerSizedInt;
    typedef unsigned char       UInt8;
    typedef signed char         SInt8;
    typedef unsigned short      UInt16;
    typedef signed short        SInt16;
    typedef unsigned long       UInt32;
    typedef signed long         SInt32;
    typedef signed long long    SInt64;
    typedef unsigned long long  UInt64;
    typedef float               Float32;
    typedef double              Float64;
    typedef UInt16              Bool16;
    typedef UInt8               Bool8;
    
    typedef unsigned long       FourCharCode;
    typedef FourCharCode        OSType;

    typedef unsigned char       boolean;
    #define true                1
    #define false               0


    #ifdef  FOUR_CHARS_TO_INT
    #error Conflicting Macro "FOUR_CHARS_TO_INT"
    #endif
    //#define FOUR_CHARS_TO_INT  ( c1, c2, c3, c4 ) (((c1)<<24)|((c2)<<16)|((c3)<<8)|(c4))
    #define FOUR_CHARS_TO_INT( c1, c2, c3, c4 )  ( c1 << 24 | c2 << 16 | c3 << 8 | c4 )

    #ifdef  TW0_CHARS_TO_INT
    #error Conflicting Macro "TW0_CHARS_TO_INT"
    #endif
        
    #define TW0_CHARS_TO_INT( c1, c2 )  ( c1 << 8 | c2 )

#elif WIN32

    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif

    /* Defines */
    #define _64BITARG_ "I64"

    /* paths */
    #define kEOLString "\r\n"
    #define kPathDelimiterString "\\"
    #define kPathDelimiterChar '\\'
    #define kPartialPathBeginsWithDelimiter 0
    
    #define crypt(buf, salt) ((char*)buf)
    
    /* Includes */
    #include <windows.h>
    #include <winsock2.h>
    #include <mswsock.h>
    #include <process.h>
    #include <ws2tcpip.h>
    #include <io.h>
    #include <direct.h>
    #include <errno.h>
    #include <sys/stat.h>

    #include <sys\types.h>
    #include <sys\timeb.h>

    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>

    #include "win32_header.h"

    
    #define R_OK 0
    #define W_OK 1
        
    // POSIX errorcodes
    #define ENOTCONN 1002
    #define EADDRINUSE 1004
    #define EINPROGRESS 1007
    #define ENOBUFS 1008
    #define EADDRNOTAVAIL 1009

    /* Constants */
    #define QT_TIME_TO_LOCAL_TIME   (-2082844800)
    #define QT_PATH_SEPARATOR       '/'

    /* Typedefs */
    typedef unsigned int        PointerSizedInt;
    typedef unsigned char       UInt8;
    typedef signed char         SInt8;
    typedef unsigned short      UInt16;
    typedef signed short        SInt16;
    typedef unsigned long       UInt32;
    typedef signed long         SInt32;
    typedef LONGLONG            SInt64;
    typedef ULONGLONG           UInt64;
    typedef float               Float32;
    typedef double              Float64;
    typedef UInt16              Bool16;
    typedef UInt8               Bool8;
    
    typedef unsigned long       FourCharCode;
    typedef FourCharCode        OSType;

    /* Typedefs */
    typedef unsigned char       boolean;
    #define true                1
    #define false               0


    #ifdef  FOUR_CHARS_TO_INT
    #error Conflicting Macro "FOUR_CHARS_TO_INT"
    #endif

    #define FOUR_CHARS_TO_INT( c1, c2, c3, c4 )  ( c1 << 24 | c2 << 16 | c3 << 8 | c4 )

    #ifdef  TW0_CHARS_TO_INT
    #error Conflicting Macro "TW0_CHARS_TO_INT"
    #endif
        
    #define TW0_CHARS_TO_INT( c1, c2 )  ( c1 << 8 | c2 )

    #define kSInt16_Max USHRT_MAX
    #define kUInt16_Max USHRT_MAX
    
    #define kSInt32_Max LONG_MAX
    #define kUInt32_Max ULONG_MAX
    
    #undef kSInt64_Max
    #define kSInt64_Max  9223372036854775807i64
    
    #undef kUInt64_Max
    #define kUInt64_Max  (kSInt64_Max * 2ULL + 1)

    //HANDLE _h_mutex;
#endif

    typedef SInt32 OS_Error;
    
    enum
    {
        OS_NoErr = (OS_Error) 0,
        OS_Err   = (OS_Error) -1, 
        OS_BadURLFormat = (OS_Error) -100,
        OS_NotEnoughSpace = (OS_Error) -101
    };

#ifdef __cplusplus
}
#endif

#endif
