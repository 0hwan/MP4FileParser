#ifndef ISOM_STSD_BOX_H
#define ISOM_STSD_BOX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bit_operation.h"
#include "isom_mp4_avc_box.h"
#include "3gpp_box.h"



/***************************************
Descriptors Tag
***************************************/
enum
{
    ISOM_ODF_OD_TAG			= 0x01,
    ISOM_ODF_IOD_TAG			= 0x02,
    ISOM_ODF_ESD_TAG			= 0x03,
    ISOM_ODF_DCD_TAG			= 0x04,
    ISOM_ODF_DSI_TAG			= 0x05,
    ISOM_ODF_SLC_TAG			= 0x06,
    ISOM_ODF_CI_TAG			= 0x07,
    ISOM_ODF_SCI_TAG			= 0x08,
    ISOM_ODF_IPI_PTR_TAG		        = 0x09,
    ISOM_ODF_IPMP_PTR_TAG		        = 0x0A,
    ISOM_ODF_IPMP_TAG			= 0x0B,
    ISOM_ODF_QOS_TAG			= 0x0C,
    ISOM_ODF_REG_TAG			= 0x0D,
    
    ISOM_ODF_ESD_INC_TAG		= 0x0E,
    ISOM_ODF_ESD_REF_TAG		= 0x0F,
    ISOM_ODF_ISOM_IOD_TAG		= 0x10,
    ISOM_ODF_ISOM_OD_TAG		= 0x11,
    ISOM_ODF_ISOM_IPI_PTR_TAG	= 0x12,
    /*END FILE FORMAT RESERVED*/
    
    ISOM_ODF_EXT_PL_TAG		= 0x13,
    ISOM_ODF_PL_IDX_TAG		= 0x14,
    
    ISOM_ODF_ISO_BEGIN_TAG	= 0x15,
    ISOM_ODF_ISO_END_TAG		= 0x3F,
    
    ISOM_ODF_CC_TAG			= 0x40,
    ISOM_ODF_KW_TAG			= 0x41,
    ISOM_ODF_RATING_TAG		= 0x42,
    ISOM_ODF_LANG_TAG			= 0x43,
    ISOM_ODF_SHORT_TEXT_TAG	= 0x44,
    ISOM_ODF_TEXT_TAG			= 0x45,
    ISOM_ODF_CC_NAME_TAG		= 0x46,
    ISOM_ODF_CC_DATE_TAG		= 0x47,
    ISOM_ODF_OCI_NAME_TAG		= 0x48,
    ISOM_ODF_OCI_DATE_TAG		= 0x49,
    ISOM_ODF_SMPTE_TAG		= 0x4A,
    
    ISOM_ODF_SEGMENT_TAG		= 0x4B,
    ISOM_ODF_MEDIATIME_TAG	= 0x4C,
    
    ISOM_ODF_IPMP_TL_TAG		= 0x60,
    ISOM_ODF_IPMP_TOOL_TAG	= 0x61,
    
    ISOM_ODF_ISO_RES_BEGIN_TAG	= 0x62,
    ISOM_ODF_ISO_RES_END_TAG		= 0xBF,
    
    ISOM_ODF_USER_BEGIN_TAG	= 0xC0,
    
    /*internal descriptor for mux input description*/
    ISOM_ODF_MUXINFO_TAG		= ISOM_ODF_USER_BEGIN_TAG,
    /*internal descriptor for bifs config input description*/
    ISOM_ODF_BIFS_CFG_TAG		= ISOM_ODF_USER_BEGIN_TAG + 1,
    /*internal descriptor for UI config input description*/
    ISOM_ODF_UI_CFG_TAG		= ISOM_ODF_USER_BEGIN_TAG + 2,
    /*internal descriptor for TextConfig description*/
    ISOM_ODF_TEXT_CFG_TAG		= ISOM_ODF_USER_BEGIN_TAG + 3,
    ISOM_ODF_TX3G_TAG			= ISOM_ODF_USER_BEGIN_TAG + 4,
    ISOM_ODF_ELEM_MASK_TAG	= ISOM_ODF_USER_BEGIN_TAG + 5,
    /*internal descriptor for LASeR config input description*/
    ISOM_ODF_LASER_CFG_TAG	= ISOM_ODF_USER_BEGIN_TAG + 6,
    ISOM_ODF_USER_END_TAG		= 0xFE,    
    ISOM_ODF_OCI_BEGIN_TAG	= 0x40,
    ISOM_ODF_OCI_END_TAG		= (ISOM_ODF_ISO_RES_BEGIN_TAG - 1),    
    ISOM_ODF_EXT_BEGIN_TAG	= 0x80,
    ISOM_ODF_EXT_END_TAG		= 0xFE,
};

/***************************************
stream type
***************************************/
enum
{
    /*!MPEG-4 Object Descriptor Stream*/
    ISOM_STREAM_OD		= 0x01,
    /*!MPEG-4 Object Clock Reference Stream*/
    ISOM_STREAM_OCR		= 0x02,
    /*!MPEG-4 Scene Description Stream*/
    ISOM_STREAM_SCENE		= 0x03,
    /*!Visual Stream (Video, Image or MPEG-4 SNHC Tools)*/
    ISOM_STREAM_VISUAL	= 0x04,
    /*!Audio Stream (Audio, MPEG-4 Structured-Audio Tools)*/
    ISOM_STREAM_AUDIO		= 0x05,
    /*!MPEG-7 Description Stream*/
    ISOM_STREAM_MPEG7		= 0x06,
    /*!MPEG-4 Intellectual Property Management and Protection Stream*/
    ISOM_STREAM_IPMP		= 0x07,
    /*!MPEG-4 Object Content Information Stream*/
    ISOM_STREAM_OCI		= 0x08,
    /*!MPEG-4 MPEGlet Stream*/
    ISOM_STREAM_MPEGJ		= 0x09,
    /*!MPEG-4 User Interaction Stream*/
    ISOM_STREAM_INTERACT	= 0x0A,
    /*!MPEG-4 IPMP Tool Stream*/
    ISOM_STREAM_IPMP_TOOL	= 0x0B,
    /*!MPEG-4 Font Data Stream*/
    ISOM_STREAM_FONT		= 0x0C,
    /*!MPEG-4 Streaming Text Stream*/
    ISOM_STREAM_TEXT		= 0x0D,
    /*!Nero Digital Subpicture Stream*/
    ISOM_STREAM_ND_SUBPIC = 0x38,
    
    ISOM_STREAM_PRIVATE_SCENE	= 0x20,
};

/***************************************
SLConfig Tag
***************************************/
enum
{
    SLPredef_Null = 0x01,
    SLPredef_MP4 = 0x02,
    /* means NO SL at all (for streams unable to handle AU reconstruction a timing)*/
    SLPredef_SkipSL = 0xF0
};



enum
{
    /*MP4 extensions*/
    ISOM_BOX_TYPE_DPND	= FOUR_CHARS_TO_INT( 'd', 'p', 'n', 'd' ),
    ISOM_BOX_TYPE_IODS	= FOUR_CHARS_TO_INT( 'i', 'o', 'd', 's' ),
    ISOM_BOX_TYPE_ESDS	= FOUR_CHARS_TO_INT( 'e', 's', 'd', 's' ),
    ISOM_BOX_TYPE_MPOD	= FOUR_CHARS_TO_INT( 'm', 'p', 'o', 'd' ),
    ISOM_BOX_TYPE_SYNC	= FOUR_CHARS_TO_INT( 's', 'y', 'n', 'c' ),
    ISOM_BOX_TYPE_IPIR	= FOUR_CHARS_TO_INT( 'i', 'p', 'i', 'r' ),
    ISOM_BOX_TYPE_SDHD	= FOUR_CHARS_TO_INT( 's', 'd', 'h', 'd' ),
    ISOM_BOX_TYPE_ODHD	= FOUR_CHARS_TO_INT( 'o', 'd', 'h', 'd' ),
    ISOM_BOX_TYPE_NMHD	= FOUR_CHARS_TO_INT( 'n', 'm', 'h', 'd' ),
    ISOM_BOX_TYPE_MP4S	= FOUR_CHARS_TO_INT( 'm', 'p', '4', 's' ),
    ISOM_BOX_TYPE_MP4A	= FOUR_CHARS_TO_INT( 'm', 'p', '4', 'a' ),
    ISOM_BOX_TYPE_MP4V	= FOUR_CHARS_TO_INT( 'm', 'p', '4', 'v' ),
    
    /*AVC / H264 extension*/
    ISOM_BOX_TYPE_AVCC	= FOUR_CHARS_TO_INT( 'a', 'v', 'c', 'C' ),
    ISOM_BOX_TYPE_BTRT	= FOUR_CHARS_TO_INT( 'b', 't', 'r', 't' ),
    ISOM_BOX_TYPE_M4DS	= FOUR_CHARS_TO_INT( 'm', '4', 'd', 's' ),
    ISOM_BOX_TYPE_AVC1	= FOUR_CHARS_TO_INT( 'a', 'v', 'c', '1' ),
    
    /*3GPP extensions*/
    ISOM_BOX_TYPE_DAMR	= FOUR_CHARS_TO_INT( 'd', 'a', 'm', 'r' ),
    ISOM_BOX_TYPE_D263	= FOUR_CHARS_TO_INT( 'd', '2', '6', '3' ),
    ISOM_BOX_TYPE_DEVC	= FOUR_CHARS_TO_INT( 'd', 'e', 'v', 'c' ),
    ISOM_BOX_TYPE_DQCP	= FOUR_CHARS_TO_INT( 'd', 'q', 'c', 'p' ),
    ISOM_BOX_TYPE_DSMV	= FOUR_CHARS_TO_INT( 'd', 's', 'm', 'v' ),

    /*3GPP(2) extension subtypes*/
    ISOM_SUBTYPE_3GP_H263		= FOUR_CHARS_TO_INT( 's', '2', '6', '3' ),
    ISOM_SUBTYPE_3GP_AMR		    = FOUR_CHARS_TO_INT( 's', 'a', 'm', 'r' ),
    ISOM_SUBTYPE_3GP_AMR_WB	    = FOUR_CHARS_TO_INT( 's', 'a', 'w', 'b' ),
    ISOM_SUBTYPE_3GP_EVRC		= FOUR_CHARS_TO_INT( 's', 'e', 'v', 'c' ),
    ISOM_SUBTYPE_K3G_EVRC		= FOUR_CHARS_TO_INT( 'e', 'v', 'r', 'c' ),
    ISOM_SUBTYPE_3GP_QCELP	    = FOUR_CHARS_TO_INT( 's', 'q', 'c', 'p' ),
	ISOM_SUBTYPE_3GP_SMV		    = FOUR_CHARS_TO_INT( 's', 's', 'm', 'v' ),
    
    /* ISO Base Media File Format Extensions for MPEG-21 */
    ISOM_BOX_TYPE_ENCA	= FOUR_CHARS_TO_INT( 'e', 'n', 'c', 'a' ),
    ISOM_BOX_TYPE_ENCV	= FOUR_CHARS_TO_INT( 'e', 'n', 'c', 'v' ),
    ISOM_BOX_TYPE_ENCT	= FOUR_CHARS_TO_INT( 'e', 'n', 'c', 't' ),
    ISOM_BOX_TYPE_ENCS	= FOUR_CHARS_TO_INT( 'e', 'n', 'c', 's' ),
    /* user  boxes */
    ISOM_BOX_TYPE_UUID	= FOUR_CHARS_TO_INT( 'u', 'u', 'i', 'd' ),
    ISOM_BOX_TYPE_SINF	= FOUR_CHARS_TO_INT( 's', 'i', 'n', 'f' ),
    
    ISOM_BOX_TYPE_CO64   = FOUR_CHARS_TO_INT( 'c', 'o', '6', '4' ),
};

//stsd define                               
#define    STSDPOS_VERSIONFLAGS                0
#define    STSDPOS_NUMENTRIES                  4
#define    STSDPOS_SAMPLETABLE                 8

#define    STSDDESCPOS_SIZE                    0
#define    STSDDESCPOS_DATAFORMAT              4
#define    STSDDESCPOS_HEADER                  8
#define    STSDDESCPOS_INDEX                   14
//stsd define end



typedef struct isom_uuid_box_t                isom_uuid_box_t;
typedef struct isom_unknown_box_t             isom_unknown_box_t;
typedef struct isom_sample_entry_box_t        isom_sample_entry_box_t;
typedef struct isom_track_referencetypebox_t  isom_track_referencetypebox_t;
typedef struct isom_full_box_t                isom_full_box_t;
typedef struct isom_box_t                     isom_box_t;
typedef struct isom_audio_sample_entry_box_t  isom_audio_sample_entry_box_t;
typedef struct isom_visual_sample_entry_box_t isom_visual_sample_entry_box_t;
typedef struct isom_descriptor_t              isom_descriptor_t;
typedef struct isom_mpeg_sample_entrybox_t    isom_mpeg_sample_entrybox_t;


struct isom_box_t {
    UInt32 type;
    UInt64 size;
};

struct isom_full_box_t {
    UInt32 type;
    UInt64 size;
    UInt8  version;
    UInt32 flags;
};

struct isom_uuid_box_t {
    UInt32 type;
    UInt64 size;
    UInt8 uuid[16];
};

struct isom_track_referencetypebox_t {
    
    UInt32 type;
    UInt64 size;
    UInt32 track_idcount;
    UInt32 *track_ids;
};

struct isom_sample_entry_box_t {
    UInt32 type;
    UInt64 size;
    UInt8 uuid[16];
    UInt16 data_reference_index;
    char reserved[ 6 ];	
};

struct isom_unknown_box_t {
    UInt32 type;
    UInt64 size;
    char *data;
    UInt32 dataSize;
};

struct isom_visual_sample_entry_box_t {
    UInt32 type;
    UInt64 size;
    UInt16 data_reference_index;			
    char reserved[ 6 ];					
    //    isom_protect_box *protection_info;
    UInt16 version;						
    UInt16 revision;					
    UInt32 vendor;						
    UInt32 temporal_quality;			
    UInt32 spacial_quality;				
    UInt16 Width, Height;				
    UInt32 horiz_res, vert_res;			
    UInt32 entry_data_size;				
    UInt16 frames_per_sample;			
    char compressor_name[33];	        
    UInt16 bit_depth;					
    SInt16 color_table_index;
};

struct isom_audio_sample_entry_box_t {
    UInt32 type;			
    UInt64 size;			
    UInt16 data_reference_index;				
    char reserved[ 6 ];						
    UInt16 version;					
    UInt16 revision;					
    UInt32 vendor;						
    UInt16 channel_count;				
    UInt16 bitspersample;				
    UInt16 compression_id;				
    UInt16 packet_size;				
    UInt16 samplerate_hi;				
    UInt16 samplerate_lo;
};

struct isom_descriptor_t {
    UInt8   tag;
};

struct isom_mpeg_sample_entrybox_t {
    UInt32      type;
    UInt64      size;
    UInt8       uuid[16];
    UInt16      datareference_index;
    char        reserved[ 6 ];
    isom_esd_box_t *esd;
    /*used for hinting when extracting the OD stream...*/
    isom_sl_config_descriptor_t *slc;
};

rs_status_t isom_init_box(isom_box_t **newbox, UInt32 type);

rs_status_t isom_set_box(isom_box_t *newbox, isom_resource_t *memory_source, UInt32 type);

void isom_delete_box(isom_box_t *box);

void isom_init_full_box(isom_box_t *a);

Bool16 isom_set_full_box(isom_box_t *box, isom_resource_t *data_resource);

Bool16 isom_set_visual_sample(isom_visual_sample_entry_box_t *ent, isom_resource_t *memory_source);

Bool16 isom_set_audio_sample_entry(isom_audio_sample_entry_box_t *ent, isom_resource_t *memory_source);

Bool16 isom_set_box_list(isom_box_t *parent,  isom_resource_t *memory_source, Bool16 (*add_box)(isom_box_t *par, isom_box_t *b));

rs_status_t isom_parse_box(isom_box_t **outbox, isom_resource_t *memory_resource);

Bool16 isom_set_odf_descriptor(char *raw_desc, UInt32 desc_size, isom_descriptor_t **out_desc);

Bool16 isom_init_odf_descriptor(isom_descriptor_t **desc, UInt8 tag);

Bool16 isom_set_odf_slc_predef(isom_sl_config_descriptor_t *sl);

void isom_delete_odf_avc_config(isom_avc_config_t *cfg);

Bool16 isom_create_odf_avc_config(isom_avc_config_t **cfg);

Bool16 isom_set_odf_descriptor_list(char *raw_list, UInt32 raw_size, ut_list_t *desc_list);

Bool16 isom_delete_odf_descriptor_list(ut_list_t *desc_list);

Bool16 isom_create_odf_esd(isom_descriptor_t **desc);

Bool16 isom_delete_odf_descriptor(isom_descriptor_t *desc);

Bool16 isom_parse_odf_descriptor(isom_resource_t *memory_source, isom_descriptor_t **desc, UInt32 *desc_size);

SInt32 isom_get_odf_field_size(UInt32 size_desc);

Bool16 isom_create_odf_descriptor(UInt8 tag, isom_descriptor_t **desc);

Bool16 isom_delete_odf_esd(isom_esd_t *esd);

Bool16 isom_delete_odf_dcd(isom_decoder_config_t *dcd);

Bool16 isom_delete_odf_slc(isom_sl_config_descriptor_t *sl);

Bool16 isom_delete_odf_default(isom_default_descriptor_t *dd);

Bool16 isom_set_descriptor_read_odf(isom_resource_t *memory_source, isom_descriptor_t *desc, UInt64 pos);

Bool16 isom_create_odf_dcd(isom_descriptor_t **desc);

Bool16 isom_create_odf_default(isom_descriptor_t **desc);

Bool16 isom_init_odf_slc(UInt8 predef, isom_descriptor_t **desc);

Bool16 isom_set_esd_read_odf(isom_resource_t *memory_source, isom_esd_t *f_desc, UInt64 pos);

Bool16 isom_set_dcd_read_odf(isom_resource_t *memory_source, isom_decoder_config_t *desc, UInt64 pos);

Bool16 isom_delete_odf_all_descriptor_list(ut_list_t *descList);

Bool16 isom_set_slc_read_odf(isom_resource_t *memory_source, isom_sl_config_descriptor_t *sl, UInt64 pos);

Bool16 isom_set_default_read_odf(isom_resource_t *memory_source, isom_default_descriptor_t *dd, UInt64 desc_size);

Bool16 isom_add_descriptor_to_esd(isom_esd_t *esd, isom_descriptor_t *desc);

UInt32 isom_get_sl_time_stamp_bytes_len(isom_sl_config_descriptor_t *sl);

isom_esd_t *isom_init_odf_desc_esd(UInt32 sl_predefined);

void isom_rewrite_avc_esdescriptor(isom_mpeg_visual_sample_entry_box_t *avc);

Bool16 isom_copy_odf_desc(isom_descriptor_t *in_desc, isom_descriptor_t **out_desc);

Bool16 isom_set_odf_write_descriptor(isom_descriptor_t *desc, char **out_encdesc, UInt32 *out_size);

Bool16 isom_set_high_odf_write_descriptor(isom_bit_operation_t *bo, isom_descriptor_t *desc);

Bool16 isom_set_esd_write_odf(isom_bit_operation_t *bo, isom_esd_t *esd);

Bool16 isom_get_odf_size_dcd(isom_decoder_config_t *dcd, UInt32 *outSize);

Bool16 isom_get_odf_size_slc(isom_sl_config_descriptor_t *sl, UInt32 *outSize);

Bool16 isom_get_odf_size_default(isom_default_descriptor_t *dd, UInt32 *out_size);

Bool16 isom_set_dcd_write_odf(isom_bit_operation_t *bs, isom_decoder_config_t *dcd);

Bool16 isom_set_slc_write_odf(isom_bit_operation_t *bo, isom_sl_config_descriptor_t *sl);

Bool16 isom_set_default_write_odf(isom_bit_operation_t *bo, isom_default_descriptor_t *dd);

Bool16 isom_get_odf_size_descriptor(isom_descriptor_t *desc, UInt32 *outSize);

Bool16 isom_set_base_descriptor_write_odf(isom_bit_operation_t *bo, UInt8 tag, UInt32 size);

Bool16 isom_set_url_string_write_odf(isom_bit_operation_t *bo, char *str);

Bool16 isom_set_descriptor_list_write_odf(isom_bit_operation_t *bo, ut_list_t *desc_list);

Bool16 isom_get_odf_size_descriptor_list(ut_list_t *descList, UInt32 *outSize);

Bool16 isom_get_odf_size_esd(isom_esd_t *esd, UInt32 *outSize);

UInt32 isom_get_odf_size_url_string(char *_string);


Bool16 isom_create_3gpa(isom_box_t **box, UInt32 type);

Bool16 isom_create_3gpv(isom_box_t **box, UInt32 type);

Bool16 isom_create_3gpc(isom_box_t **box, UInt32 type);

Bool16 isom_create_mp4a(isom_box_t **box, UInt32 type);

Bool16 isom_create_mp4v(isom_box_t **box, UInt32 type);

Bool16 isom_create_esds(isom_box_t **newbox);

Bool16 isom_create_avc1(isom_box_t **box, UInt32 type);

Bool16 isom_create_avcc(isom_box_t **box);

Bool16 isom_create_btrt(isom_box_t **box);

Bool16 isom_create_m4ds(isom_box_t **box);

Bool16 isom_set_3gpa(isom_box_t *box, isom_resource_t *memory_source);

Bool16 isom_set_3gpc(isom_box_t *box, isom_resource_t *memory_source);

Bool16 isom_set_3gpv(isom_box_t *box, isom_resource_t *memory_source);

Bool16 isom_set_mp4v_avc1(isom_box_t *box, isom_resource_t *memory_source);

Bool16 isom_set_mp4v(isom_box_t *box, isom_resource_t *memory_source);

Bool16 isom_add_mp4v_box(isom_box_t *s, isom_box_t *a);

Bool16 isom_set_mp4a(isom_box_t *box, isom_resource_t *memory_source);

Bool16 isom_add_mp4a_box(isom_box_t *s, isom_box_t *a);

Bool16 isom_set_esds(isom_box_t *box, isom_resource_t *memory_source);

Bool16 isom_set_avc1(isom_box_t *box, isom_resource_t *memory_source);

Bool16 isom_set_avcc(isom_box_t *box, isom_resource_t *memory_source);

void isom_delete_btrt(isom_box_t *s);

void isom_delete_avcc(isom_box_t *s);

Bool16 isom_set_btrt(isom_box_t *box, isom_resource_t *memory_source);

void isom_delete_m4ds(isom_box_t *box);

Bool16 isom_set_m4ds(isom_box_t *box, isom_resource_t *memory_source);

void isom_delete_mp4v(isom_box_t *box);

void isom_delete_mp4a(isom_box_t *box);

void isom_delete_esds(isom_box_t *s);

void isom_delete_default(isom_box_t *s);

void isom_delete_3gpa(isom_box_t *box);

void isom_delete_3gpv(isom_box_t *box);

void isom_delete_3gpc(isom_box_t *box);

Bool16 isom_create_mp4v_avc1(isom_box_t **box, UInt32 type);

void isom_init_video_sample_entry(isom_visual_sample_entry_box_t *ent);

void isom_init_audio_sample_entry(isom_audio_sample_entry_box_t *ptr);

#ifdef __cplusplus
}
#endif

#endif