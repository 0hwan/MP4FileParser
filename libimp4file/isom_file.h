#ifndef ISOM_FILE_H
#define ISOM_FILE_H

#ifdef __cplusplus
extern "C" {
#endif


#include "isom_stsd_box.h"



//mvhd define start
#define    MVHDPOS_VERSIONFLAGS                 0
#define    MVHDPOS_CREATIONTIME                 4
#define    MVHDPOS_MODIFICATIONTIME             8
#define    MVHDPOS_TIMESCALE                   12
#define    MVHDPOS_DURATION                    16
#define    MVHDPOS_PREFERREDRATE               20
#define    MVHDPOS_PREFERREDVOLUME             24
#define    MVHDPOS_A                           36
#define    MVHDPOS_B                           40
#define    MVHDPOS_U                           44
#define    MVHDPOS_C                           48
#define    MVHDPOS_D                           52
#define    MVHDPOS_V                           56
#define    MVHDPOS_X                           60
#define    MVHDPOS_Y                           64
#define    MVHDPOS_W                           68
#define    MVHDPOS_PREVIEWTIME                 72
#define    MVHDPOS_PREVIEWDURATION             76
#define    MVHDPOS_POSTERTIME                  80
#define    MVHDPOS_SELECTIONTIME               84
#define    MVHDPOS_SELECTIONDURATION           88
#define    MVHDPOS_CURRENTTIME                 92
#define    MVHDPOS_NEXTTRACKID                 96
           
#define    MVHDPOSV1_CREATIONTIME               4 //+4 bytes
#define    MVHDPOSV1_MODIFICATIONTIME          12 //+4 bytes
#define    MVHDPOSV1_TIMESCALE                 20
#define    MVHDPOSV1_DURATION                  24 //+4 bytes
                                               
#define    MVHDPOSV1_PREFERREDRATE             20 + 12
#define    MVHDPOSV1_PREFERREDVOLUME           24 + 12
#define    MVHDPOSV1_A                         36 + 12
#define    MVHDPOSV1_B                         40 + 12
#define    MVHDPOSV1_U                         44 + 12
#define    MVHDPOSV1_C                         48 + 12
#define    MVHDPOSV1_D                         52 + 12
#define    MVHDPOSV1_V                         56 + 12
#define    MVHDPOSV1_X                         60 + 12
#define    MVHDPOSV1_Y                         64 + 12
#define    MVHDPOSV1_W                         68 + 12
#define    MVHDPOSV1_PREVIEWTIME               72 + 12
#define    MVHDPOSV1_PREVIEWDURATION           76 + 12
#define    MVHDPOSV1_POSTERTIME                80 + 12
#define    MVHDPOSV1_SELECTIONTIME             84 + 12
#define    MVHDPOSV1_SELECTIONDURATION         88 + 12
#define    MVHDPOSV1_CURRENTTIME               92 + 12
#define    MVHDPOSV1_NEXTTRACKID               96 + 12
//mvhd define end

//tkhd define start
#define    TKHDPOS_VERSIONFLAGS                0
#define    TKHDPOS_CREATIONTIME                4
#define    TKHDPOS_MODIFICATIONTIME            8
#define    TKHDPOS_TRACKID                    12
#define    TKHDPOS_DURATION                   20
#define    TKHDPOS_LAYER                      32
#define    TKHDPOS_ALTERNATEGROUP             34
#define    TKHDPOS_VOLUME                     36
#define    TKHDPOS_A                          40
#define    TKHDPOS_B                          44
#define    TKHDPOS_U                          48
#define    TKHDPOS_C                          52
#define    TKHDPOS_D                          56
#define    TKHDPOS_V                          60
#define    TKHDPOS_X                          64
#define    TKHDPOS_Y                          68
#define    TKHDPOS_W                          72
#define    TKHDPOS_TRACKWIDTH                 76
#define    TKHDPOS_TRACKHEIGHT                80
                                              
#define    TKHDPOSV1_CREATIONTIME              4
#define    TKHDPOSV1_MODIFICATIONTIME         12
#define    TKHDPOSV1_TRACKID                  20
#define    TKHDPOSV1_DURATION                 28
#define    TKHDPOSV1_LAYER                    44
#define    TKHDPOSV1_ALTERNATEGROUP           34 + 12
#define    TKHDPOSV1_VOLUME                   36 + 12
#define    TKHDPOSV1_A                        40 + 12
#define    TKHDPOSV1_B                        44 + 12
#define    TKHDPOSV1_U                        48 + 12
#define    TKHDPOSV1_C                        52 + 12
#define    TKHDPOSV1_D                        56 + 12
#define    TKHDPOSV1_V                        60 + 12
#define    TKHDPOSV1_X                        64 + 12
#define    TKHDPOSV1_Y                        68 + 12
#define    TKHDPOSV1_W                        72 + 12
#define    TKHDPOSV1_TRACKWIDTH               76 + 12
#define    TKHDPOSV1_TRACKHEIGHT              80 + 12
//tkhd define end                           
                                              
//hdlr define                               
#define    HDLRPOS_VERSIONFLAGS               0
#define    HDLRPOS_RESERVED1                  4
#define    HDLRPOS_HANDLERTYPE                8
#define    HDLRPOS_RESERVED2                  12
#define    HDLRPOS_NAMEUTF8                   24
// hdlr define end                          
                                            
//mdhd define                               
#define    MDHDPOS_VERSIONFLAGS                 0
#define    MDHDPOS_CREATIONTIME                 4
#define    MDHDPOS_MODIFICATIONTIME             8
#define    MDHDPOS_TIMESCALE                   12
#define    MDHDPOS_DURATION                    16
#define    MDHDPOS_LANGUAGE                    20
#define    MDHDPOS_QUALITY                     22
                                               
#define    MDHDPOSV1_CREATIONTIME               4
#define    MDHDPOSV1_MODIFICATIONTIME          12
#define    MDHDPOSV1_TIMESCALE                 20
#define    MDHDPOSV1_DURATION                  24
#define    MDHDPOSV1_LANGUAGE                  20 + 12
#define    MDHDPOSV1_QUALITY                   22 + 12
//mdhd define end                         

//dref define                               
#define    DREFPOS_VERSIONFLAGS                0
#define    DREFPOS_NUMREFS                     4
#define    DREFPOS_REFTABLE                    8

//stts define                               
#define    STTSPOS_VERSIONFLAGS                0
#define    STTSPOS_NUMENTRIES                  4
#define    STTSPOS_SAMPLETABLE                 8
//stts define end

//ctts define                               
#define    CTTSPOS_VERSIONFLAGS                0
#define    CTTSPOS_NUMENTRIES                  4
#define    CTTSPOS_SAMPLETABLE                 8
//ctts define end

//stsc define                               
#define    STSCPOS_VERSIONFLAGS                0
#define    STSCPOS_NUMENTRIES                  4
#define    STSCPOS_SAMPLETABLE                 8
//stsc define end

//stco define                               
#define    STCOPOS_VERSIONFLAGS                0
#define    STCOPOS_NUMENTRIES                  4
#define    STCOPOS_SAMPLETABLE                 8
//stco define end

//stsz define     
#define    STSZPOS_VERSIONFLAGS                0
#define    STSZPOS_SAMPLESIZE                  4
#define    STSZPOS_NUMENTRIES                  8
#define    STSZPOS_SAMPLETABLE                 12
//stsz define end

//stss define                               
#define    STSSPOS_VERSIONFLAGS                0
#define    STSSPOS_NUMENTRIES                  4
#define    STSSPOS_SAMPLETABLE                 8
//stss define end


enum
{
    /*base media types*/
    ISOM_TRACK_VISUAL	= FOUR_CHARS_TO_INT( 'v', 'i', 'd', 'e' ),
    ISOM_TRACK_AUDIO		= FOUR_CHARS_TO_INT( 's', 'o', 'u', 'n' ),
    ISOM_TRACK_HINT		= FOUR_CHARS_TO_INT( 'h', 'i', 'n', 't' ),
};


typedef struct isom_file_info_t      isom_file_info_t;
typedef struct isom_box_trak_entry_t isom_box_trak_entry_t;
typedef struct isom_box_moov_t       isom_box_moov_t;
typedef struct isom_box_mvhd_t       isom_box_mvhd_t;
typedef struct isom_box_trak_t       isom_box_trak_t;

typedef struct isom_box_tkhd_t       isom_box_tkhd_t;
typedef struct isom_box_mdia_t       isom_box_mdia_t;

typedef struct isom_box_hdlr_t       isom_box_hdlr_t;
typedef struct isom_box_mdhd_t       isom_box_mdhd_t;
typedef struct isom_box_minf_t       isom_box_minf_t;

typedef struct isom_box_dinf_t       isom_box_dinf_t;
typedef struct isom_box_dref_t       isom_box_dref_t;
typedef struct isom_data_ref_entry_t isom_data_ref_entry_t;
typedef struct isom_box_stbl_t       isom_box_stbl_t;

typedef struct isom_box_stss_t isom_box_stss_t;
typedef struct isom_box_stsz_t isom_box_stsz_t;
typedef struct isom_box_stco_t isom_box_stco_t;
typedef struct isom_box_stsc_t isom_box_stsc_t;
typedef struct isom_box_stsd_t isom_box_stsd_t;
typedef struct isom_box_ctts_t isom_box_ctts_t;
typedef struct isom_box_stts_t isom_box_stts_t;


struct isom_file_info_t {
    char             *content_path;
    UInt32            track_count;	//hint track�� ������ ��� track ����
    isom_resource_t	 *data_resource;
    
    isom_box_entry_t *box_entry;
    isom_box_entry_t *box_list_head;
    isom_box_entry_t *box_list_tail;
    
    isom_box_moov_t  *box_moov;
}; 

struct isom_box_moov_t {
    UInt32                 type;
    UInt64                 size;
    isom_box_mvhd_t         *box_mvhd;
    isom_box_trak_entry_t   *trak_list_entry;
    isom_box_trak_entry_t   *trak_list_head;
    isom_box_trak_entry_t   *trak_list_tail;
};

struct isom_box_mvhd_t {
    UInt32      type;
    UInt64      size;
    UInt8       version;
    UInt32      flags; // 24 bits in the low 3 bytes
    UInt64      creation_time, modification_time;
    UInt32      timescale;
    UInt64      duration;
    UInt32      preferred_rate;
    UInt16      preferred_volume;
    UInt32      fa, fb, fu, fc, fd, fv, fx, fy, fw;
    UInt32      preview_time, preview_duration, poster_time;
    UInt32      selection_time, selection_duration;
    UInt32      current_time;
    UInt32      next_track_id;
};

struct isom_box_trak_entry_t {
    UInt32					box_trak_entry_id;
    isom_box_trak_t 			*box_trak;  
    isom_box_trak_entry_t		*next_box_trak_entry;
};

struct isom_box_trak_t{
    UInt32         type;
    UInt64         size;
    UInt8          version;
    UInt32         flags; // 24 bits in the low 3 bytes
    isom_box_tkhd_t *box_tkhd;
    isom_box_mdia_t *box_mdia;
};

struct isom_box_tkhd_t{
    UInt32      type;
    UInt64      size;
    UInt8       version;
    UInt32      flags; // 24 bits in the low 3 bytes
    UInt64      creation_time, modification_time;
    UInt32      track_id;
    UInt32      freserved1;
    UInt64      duration;
    UInt32      freserved2, freserved3;
    UInt16      layer, alternate_group;
    UInt16      volume;
    UInt16      freserved4;
    UInt32      fa, fb, fu, fc, fd, fv, fx, fy, fw;
    UInt32      track_width, track_height;
};

struct isom_box_mdia_t{
    UInt32      type;
    UInt64      size;
    UInt8       version;
    UInt32      flags; // 24 bits in the low 3 bytes
    isom_box_hdlr_t *box_hdlr;
    isom_box_mdhd_t *box_mdhd;
    isom_box_minf_t *box_minf;
};

struct isom_box_hdlr_t{
    UInt32      type;
    UInt64      size;
    UInt8       version;
    UInt32      flags;
    UInt32      reserved1;
    UInt32      handler_type;
    UInt8       reserved2[12];
    char        *name_utf8;
};

struct isom_box_mdhd_t{
    UInt32      type;
    UInt64      size;
    UInt8       version;
    UInt32      flags; // 24 bits in the low 3 bytes
    UInt64      creation_time, modification_time;
    UInt32      timescale;
    UInt64      duration;
    UInt16      language;
    UInt16      quality;
    Float64     timescale_recip;
};

struct isom_box_minf_t{
    UInt32      type;
    UInt64      size;
    UInt8       version;
    UInt32      flags; // 24 bits in the low 3 bytes
    isom_box_stbl_t *box_stbl;
    isom_box_dinf_t *box_dinf;
};

struct isom_box_dinf_t{
    UInt32      type;
    UInt64      size;
    UInt8       version;
    UInt32      flags; // 24 bits in the low 3 bytes
    isom_box_dref_t *box_dref;
};

struct isom_data_ref_entry_t {
    // Data ref information
    UInt32          flags;
    OSType          reference_type;
    UInt32          data_size;
    char           *data;
    Bool16          is_entry_initialized;
    Bool16          is_file_open;
};

struct isom_box_dref_t{
    UInt32      type;
    UInt64      size;
    UInt8       version;
    UInt32      flags; // 24 bits in the low 3 bytes
    // data ref information
    UInt32      num_refs;
    isom_data_ref_entry_t *data_ref_entry;
};

struct isom_box_stbl_t{
    UInt32      type;
    UInt64      size;
    UInt8       version;
    UInt32      flags; // 24 bits in the low 3 bytes
    isom_box_stts_t *box_stts;
    isom_box_ctts_t *box_ctts;
    isom_box_stsc_t *box_stsc;
    isom_box_stco_t *box_stco;
    isom_box_stsz_t *box_stsz;
    isom_box_stss_t *box_stss;
    isom_box_stsd_t *box_stsd;
};

struct isom_box_stts_t{
    UInt32      type;
    UInt64      size;
    UInt8       version;
    UInt32      flags; // 24 bits in the low 3 bytes
    // buffer
    UInt64      current_dts;        // current dts
    UInt32      current_entry_index; // current entry
    UInt32      first_samp_in_entry; // entry�� ù��° ������ ��ü sample count
    //
    UInt32      numentries; // entry ����
    char       *time_to_sample_table; //box �� ����
};

struct isom_box_ctts_t{
    UInt32      type;
    UInt64      size;
    UInt8       version;
    UInt32      flags; 
    UInt64      current_cts;
    UInt32      current_entry_index;
    UInt32      first_samp_in_entry;
    UInt32      numentries;
    char       *time_to_sample_table; //box �� ���� 
};

struct isom_box_stsc_t{
    UInt32      type;
    UInt64      size;
    UInt8       version;
    UInt32      flags; // 24 bits in the low 3 bytes
    UInt32      current_index;
    /*first sample number in this chunk*/
    UInt32      first_sample_in_current_chunk;
    UInt32      current_chunk; // ����� ���� �ӽ� current chunk�� 
	UInt32      ghost_number;
    UInt32      numentries;
    char       *sample_to_chunk_table;
};

struct isom_box_stsd_t{
    UInt32       type;
    UInt64       size;
    UInt8        version;
    UInt32       flags; // 24 bits in the low 3 bytes
    
    UInt32       numentries;
    char        *sample_description_table;
    ut_list_t   *box_list;//
    // description entry in the above memory area
};

struct isom_box_stco_t{
    UInt32      type;
    UInt64      size;
    UInt8       version;
    UInt32      flags; // 24 bits in the low 3 bytes
    
    UInt32      numentries;
    UInt16      offset_size;
    char       *chunk_offset_table;
    void       *table; // longword-aligned version of the above
};

struct isom_box_stsz_t{
    UInt32      type;
    UInt64      size;
    UInt8       version;
    UInt32      flags; // 24 bits in the low 3 bytes
    UInt32      common_sample_size;
    UInt32      numentries;
    char       *sample_size_table;
    UInt32     *table; // longword-aligned version of the above
};

struct isom_box_stss_t{
    UInt32      type;
    UInt64      size;
    UInt8       version;
    UInt32      flags; // 24 bits in the low 3 bytes
    
    UInt32      numentries;
    char       *sync_sample_table;
    UInt32     *table; // longword-aligned version of the above
    UInt32      last_sync_sample;
    UInt32      last_sample_index;
};

/**
* isom_init_file_info : isom_file_info_t����ü �ʱ�ȭ(isom_libmp4file.c)
*		# box parsing, 
*       # box structure initialize and setting 
* 
* @param *content_path
* @param **mp4file_info
* @param *data_resource
* 
* @return rs_status_t 
*		# ISOM_OK: success
*/
rs_status_t isom_init_file_info(const char *content_path, isom_file_info_t** in_mp4file_info, isom_resource_t *data_resource);

/**
* isom_delete_file_info : isom_file_info_t ����ü �� ���� ��� ����ü memory ����(isom_libmp4file.c)
*		# box structure free
* 
* @param *mp4file_info
* 
* @return Bool16 
*		# 
*/
Bool16 isom_delete_file_info(isom_file_info_t *mp4file_info);

/**
* isom_dump_box_entry : debugging function isom_box_entry_t �� Ȯ��(isom_libmp4file.c)
* 
* @param *mp4file_info
*  
*/
void isom_dump_box_entry(isom_file_info_t *mp4file_info);

/**
* isom_generate_box_entry : box�� type, size, offset ���� (isom_libmp4file.c)
*       # moov box ���� ������ ����
*
* @param *mp4file_info
* 
* @return ISOM_OK: Success 
*		# 
*/
rs_status_t isom_generate_box_entry(isom_file_info_t *mp4file_info);

/**
* isom_find_box_entry : box�� type���� box_entry�� �˻� (isom_libmp4file.c)
*       # box�� offset���� ã������ ��� 
*
* @param *mp4file_info
* @param *box_path : ex) "moov", "moov:trak", ":trak"
* @param **box_entry : dest_box_entry
* @param *last_found_box_entry : source_box_entry
* 
* @return Success :ISOM_OK 
*		# 
*/
rs_status_t isom_find_box_entry(isom_file_info_t *mp4file_info, const char *box_path, isom_box_entry_t **box_entry, isom_box_entry_t *last_found_box_entry);

/**
* isom_delete_trak_box : trak box���� ������ ��� box ����ü �޸� ����(isom_libmp4file.c)
*       # isom_box_trak_entry_t �� ��� trak �޸� ����
*
* @param *mp4file_info
* 
* @return Bool16 
*		# 
*/
Bool16 isom_delete_trak_box(isom_file_info_t *mp4file_info);

/**
* isom_next_trak_box : isom_box_trak_entry_t �� List���� next isom_box_trak_t ���� ã������ ���(isom_libmp4file.c)
*
* @param *mp4file_info
* @param **box_trak : destination 
* @param *lastfound_box_trak : ������ �˻��� trak�� �������, ���ٸ� NULL
* 
* @return rs_status_t 
*		# 
*/
rs_status_t isom_next_trak_box(isom_file_info_t *mp4file_info, isom_box_trak_t **box_trak, isom_box_trak_t *lastfound_box_trak);

/**
* isom_find_track : track_id��  isom_box_trak_t find (isom_libmp4file.c)
*
* @param *mp4file_info
* @param track_id
* @param **box_trak : destination 
* 
* @return Bool16 
*		# 
*/
Bool16 isom_find_track(isom_file_info_t *mp4file_info, UInt32 track_id, isom_box_trak_t **box_trak);

/**
* isom_init_moov_box : isom_box_moov_t ����ü initialize �� setting (isom_libmp4file.c)
*
* @param *mp4file_info
* @param *box_entry : isom_find_box_entry�� �˻��� box_entry �� 
* @param **box_moov : initailize �� box ����ü (�Լ����ο��� malloc)
* 
* @return rs_status_t 
*		# 
*/
rs_status_t isom_init_moov_box(isom_file_info_t *mp4file_info, isom_box_entry_t *box_entry, isom_box_moov_t **box_moov);

/**
* isom_init_mvhd_box : isom_box_mvhd_t ����ü initialize �� setting (isom_libmp4file.c)
*
* @param *mp4file_info
* @param *box_entry : isom_find_box_entry�� �˻��� box_entry �� 
* @param **box_mvhd : initailize �� box ����ü (�Լ����ο��� malloc)
* 
* @return rs_status_t 
*		# 
*/
rs_status_t isom_init_mvhd_box(isom_file_info_t *mp4file_info, isom_box_entry_t *box_entry, isom_box_mvhd_t **box_mvhd);

/**
* isom_init_trak_box : isom_box_trak_t ����ü initialize �� setting (isom_libmp4file.c)
*
* @param *mp4file_info
* @param *box_entry : isom_find_box_entry�� �˻��� box_entry �� 
* @param **box_trak : initailize �� box ����ü (�Լ����ο��� malloc)
* 
* @return rs_status_t Success: ISOM_OK 
*		# 
*/
rs_status_t isom_init_trak_box(isom_file_info_t *mp4file_info, isom_box_entry_t *box_entry, isom_box_trak_t **box_trak);

/**
* isom_init_tkhd_box : isom_box_tkhd_t ����ü initialize �� setting (isom_libmp4file.c)
*
* @param *mp4file_info
* @param *box_entry : isom_find_box_entry�� �˻��� box_entry �� 
* @param **box_tkhd : initailize �� box ����ü (�Լ����ο��� malloc)
* 
* @return rs_status_t 
*		# 
*/
rs_status_t isom_init_tkhd_box(isom_file_info_t *mp4file_info, isom_box_entry_t *box_entry, isom_box_tkhd_t **box_tkhd);

/**
* isom_init_mdia_box : isom_box_mdia_t ����ü initialize �� setting (isom_libmp4file.c)
*
* @param *mp4file_info
* @param *box_entry : isom_find_box_entry�� �˻��� box_entry �� 
* @param **box_mdia : initailize �� box ����ü (�Լ����ο��� malloc)
* 
* @return rs_status_t 
*		# 
*/
rs_status_t isom_init_mdia_box(isom_file_info_t *mp4file_info, isom_box_entry_t *box_entry, isom_box_mdia_t **box_mdia);

/**
* isom_init_mdhd_box : isom_box_mdhd_t ����ü initialize �� setting (isom_libmp4file.c)
*
* @param *mp4file_info
* @param *box_entry : isom_find_box_entry�� �˻��� box_entry �� 
* @param **box_mdhd : initailize �� box ����ü (�Լ����ο��� malloc)
* 
* @return rs_status_t 
*		# 
*/
rs_status_t isom_init_mdhd_box(isom_file_info_t *mp4file_info, isom_box_entry_t *box_entry, isom_box_mdhd_t **box_mdhd);

/**
* isom_init_hdlr_box : isom_box_hdlr_t ����ü initialize �� setting (isom_libmp4file.c)
*
* @param *mp4file_info
* @param *box_entry : isom_find_box_entry�� �˻��� box_entry �� 
* @param **box_hdlr : initailize �� box ����ü (�Լ����ο��� malloc)
* 
* @return rs_status_t 
*		# 
*/
rs_status_t isom_init_hdlr_box(isom_file_info_t *mp4file_info, isom_box_entry_t *box_entry, isom_box_hdlr_t **box_hdlr);

/**
* isom_init_minf_box : isom_box_minf_t ����ü initialize �� setting (isom_libmp4file.c)
*
* @param *mp4file_info
* @param *box_entry : isom_find_box_entry�� �˻��� box_entry �� 
* @param **box_minf : initailize �� box ����ü (�Լ����ο��� malloc)
* 
* @return rs_status_t 
*		# 
*/
rs_status_t isom_init_minf_box(isom_file_info_t *mp4file_info, isom_box_entry_t *box_entry, isom_box_minf_t **box_minf);

/**
* isom_init_dinf_box : isom_box_dinf_t ����ü initialize �� setting (isom_libmp4file.c)
*
* @param *mp4file_info
* @param *box_entry : isom_find_box_entry�� �˻��� box_entry �� 
* @param **box_dinf : initailize �� box ����ü (�Լ����ο��� malloc)
* 
* @return rs_status_t 
*		# 
*/
rs_status_t isom_init_dinf_box(isom_file_info_t *mp4file_info, isom_box_entry_t *org_box_entry, isom_box_dinf_t **box_dinf);

/**
* isom_init_dref_box : isom_box_dref_t ����ü initialize �� setting (isom_libmp4file.c)
*
* @param *mp4file_info
* @param *box_entry : isom_find_box_entry�� �˻��� box_entry �� 
* @param **box_dref : initailize �� box ����ü (�Լ����ο��� malloc)
* 
* @return rs_status_t 
*		# 
*/
rs_status_t isom_init_dref_box(isom_file_info_t *mp4file_info, isom_box_entry_t *org_box_entry, isom_box_dref_t **box_dref);

/**
* isom_init_stbl_box : isom_box_stbl_t ����ü initialize �� setting (isom_libmp4file.c)
*
* @param *mp4file_info
* @param *box_entry : isom_find_box_entry�� �˻��� box_entry �� 
* @param **box_stbl : initailize �� box ����ü (�Լ����ο��� malloc)
* 
* @return rs_status_t 
*		# 
*/
rs_status_t isom_init_stbl_box(isom_file_info_t *mp4file_info, isom_box_entry_t *org_box_entry, isom_box_stbl_t **box_stbl);

/**
* isom_init_stts_box : isom_box_stts_t ����ü initialize �� setting (isom_libmp4file.c)
*
* @param *mp4file_info
* @param *box_entry : isom_find_box_entry�� �˻��� box_entry �� 
* @param **box_stts : initailize �� box ����ü (�Լ����ο��� malloc)
* 
* @return rs_status_t 
*		# 
*/
rs_status_t isom_init_stts_box(isom_file_info_t *mp4file_info, isom_box_entry_t *box_entry, isom_box_stts_t **box_stts);

/**
* isom_init_ctts_box : isom_box_ctts_t ����ü initialize �� setting (isom_libmp4file.c)
*
* @param *mp4file_info
* @param *box_entry : isom_find_box_entry�� �˻��� box_entry �� 
* @param **box_ctts : initailize �� box ����ü (�Լ����ο��� malloc)
* 
* @return rs_status_t 
*		# 
*/
rs_status_t isom_init_ctts_box(isom_file_info_t *mp4file_info, isom_box_entry_t *box_entry, isom_box_ctts_t **box_ctts);

/**
* isom_init_stsc_box : isom_box_stsc_t ����ü initialize �� setting (isom_libmp4file.c)
*
* @param *mp4file_info
* @param *box_entry : isom_find_box_entry�� �˻��� box_entry �� 
* @param **box_stsc : initailize �� box ����ü (�Լ����ο��� malloc)
* 
* @return rs_status_t 
*		# 
*/
rs_status_t isom_init_stsc_box(isom_file_info_t *mp4file_info, isom_box_entry_t *box_entry, isom_box_stsc_t **box_stsc);

/**
* isom_init_stsd_box : isom_box_stsd_t ����ü initialize �� setting (isom_libmp4file.c)
*
* @param *mp4file_info
* @param *box_entry : isom_find_box_entry�� �˻��� box_entry �� 
* @param **box_stsd : initailize �� box ����ü (�Լ����ο��� malloc)
* 
* @return rs_status_t 
*		# 
*/
rs_status_t isom_init_stsd_box(isom_file_info_t *mp4file_info, isom_box_entry_t *box_entry, isom_box_stsd_t **box_stsd);

/**
* isom_init_stco_box : isom_box_stco_t ����ü initialize �� setting (isom_libmp4file.c)
*
* @param *mp4file_info
* @param *box_entry : isom_find_box_entry�� �˻��� box_entry �� 
* @param **box_stco  : initailize �� box ����ü (�Լ����ο��� malloc)
* @param off_setsize : box���� stco�ΰ�� 4, co64�ΰ�� 8
* 
* @return rs_status_t 
*		# 
*/
rs_status_t isom_init_stco_box(isom_file_info_t *mp4file_info, isom_box_entry_t *box_entry, isom_box_stco_t **box_stco, UInt16 off_setsize);

/**
* isom_init_stsz_box : isom_box_stsz_t ����ü initialize �� setting (isom_libmp4file.c)
*
* @param *mp4file_info
* @param *box_entry : isom_find_box_entry�� �˻��� box_entry �� 
* @param **box_stsz : initailize �� box ����ü (�Լ����ο��� malloc)
* 
* @return rs_status_t 
*		# 
*/
rs_status_t isom_init_stsz_box(isom_file_info_t *mp4file_info, isom_box_entry_t *box_entry, isom_box_stsz_t **box_stsz);

/**
* isom_init_stss_box : isom_box_stss_t ����ü initialize �� setting (isom_libmp4file.c)
*
* @param *mp4file_info
* @param *box_entry : isom_find_box_entry�� �˻��� box_entry �� 
* @param **box_stss : initailize �� box ����ü (�Լ����ο��� malloc)
* 
* @return rs_status_t 
*		# 
*/
rs_status_t isom_init_stss_box(isom_file_info_t *mp4file_info, isom_box_entry_t *box_entry, isom_box_stss_t **box_stss);

/**
* isom_delete_box_list : stsd box ���� codec box �޸� ���� (isom_libmp4file.c)
*       # ut_list_t ����ü�� codec box ����ü�� ���� �Ǿ� ����. 
*
* @param *box_list
* 
* @return Bool16 
*		# 
*/
Bool16 isom_delete_box_list(ut_list_t *box_list);

/**
* isom_add_stsd_box : stsd box ���� codec box �� ut_list_t�� ���� (isom_libmp4file.c)
*       # isom_add_list �� ���  
*
* @param *stsd
* @param *box
* 
* @return Bool16 
*		# 
*/
Bool16 isom_add_stsd_box(isom_box_stsd_t *stsd, isom_box_t *box);

/**
* isom_delete_stsd_box : isom_box_stsd_t �޸� ���� (isom_libmp4file.c)
*       #  ���������� isom_delete_box_list ȣ��
*
* @param *box_stsd
* 
* @return Bool16 
*		# 
*/
Bool16 isom_delete_stsd_box(isom_box_stsd_t *box_stsd);

/**
* isom_create_default_box : stsd box ���� codec box �� type�� �� ��� ���Ǵ� defalut create �Լ�  (isom_libmp4file.c)
*       #  ���� ��� ����. �˼� ���� type�� ��� fail ó�� 
*
* @param *newbox
* @param *memory_source
* 
* @return Bool16 
*		# 
*/
Bool16 isom_create_default_box(isom_box_t **box);

/**
* isom_set_default_box : stsd box ���� codec box �� type�� �� ��� ���Ǵ� defalut setting �Լ�  (isom_libmp4file.c)
*       #  ���� ��� ����. �˼� ���� type�� ��� fail ó�� 
*
* @param *newbox
* @param *memory_source
* 
* @return Bool16 
*		# 
*/
Bool16 isom_set_default_box(isom_box_t *newbox, isom_resource_t *memory_source);




#ifdef __cplusplus
}
#endif

#endif