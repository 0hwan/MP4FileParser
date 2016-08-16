#ifndef ISOM_MP4_AVC_BOX_H
#define ISOM_MP4_AVC_BOX_H

#ifdef __cplusplus
extern "C" {
#endif


#include "data_resource.h"
#include "misc_list.h"



typedef struct isom_mpeg_visual_sample_entry_box_t    isom_mpeg_visual_sample_entry_box_t;
typedef struct isom_esd_box_t                         isom_esd_box_t;
typedef struct isom_esd_t                             isom_esd_t;
typedef struct isom_sl_config_descriptor_t            isom_sl_config_descriptor_t;
typedef struct isom_avcconfiguration_box_t            isom_avcconfiguration_box_t;
typedef struct isom_avc_config_t                      isom_avc_config_t;
typedef struct isom_mpeg4_bitrate_box_t               isom_mpeg4_bitrate_box_t;
typedef struct isom_mpeg4_extension_descriptors_box_t isom_mpeg4_extension_descriptors_box_t;
typedef struct isom_decoder_config_t                  isom_decoder_config_t;
typedef struct isom_language_t                        isom_language_t;
typedef struct isom_registration_t                    isom_registration_t;
typedef struct isom_qos_descriptor_t                  isom_qos_descriptor_t;
typedef struct isom_ipi_ptr_t                         isom_ipi_ptr_t;
typedef struct isom_default_descriptor_t              isom_default_descriptor_t;
typedef struct isom_mpeg_audio_sample_entry_box_t     isom_mpeg_audio_sample_entry_box_t;
typedef struct isom_unknown_uuid_box_t                isom_unknown_uuid_box_t;
typedef struct isom_avc_config_slot_t                 isom_avc_config_slot_t;

struct isom_mpeg_visual_sample_entry_box_t {
    UInt32 type;
    UInt64 size;
    UInt16 data_reference_index;			
    char reserved[6];					
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
    isom_esd_box_t *esd;
    /*used for Publishing*/
    isom_sl_config_descriptor_t *slc;
    
    //avc extensions - we merged with regular 'mp4v' box to handle isma E&A signaling of AVC
    isom_avcconfiguration_box_t *avc_config;
    isom_mpeg4_bitrate_box_t *bitrate;
    //ext descriptors
    isom_mpeg4_extension_descriptors_box_t *descr;
    //internally emulated esd
    isom_esd_t *emul_esd;
    //iPod's hack
    isom_unknown_uuid_box_t *ipod_ext;    
};

struct isom_esd_box_t {
    UInt32      type;
    UInt64      size;
    UInt8       version;
    UInt32      flags;
    isom_esd_t *desc;
};

struct isom_esd_t {
    UInt8   tag;
    UInt16  esid;
    UInt16  ocr_esid;
    UInt16  depends_onesid;
    UInt8   stream_priority;
    char   *url_string;
    
    isom_decoder_config_t *decoder_config;
    isom_sl_config_descriptor_t *sl_config_descriptor;
    
    isom_ipi_ptr_t *ipi_ptr;
    isom_qos_descriptor_t *qos;
    isom_registration_t *reg_descriptor;
    //0 or 1 lang desc
    isom_language_t *lang_desc;
    
    ut_list_t *ipi_dataset;
    ut_list_t *ipmp_descriptor_pointers;
    ut_list_t *extension_descriptors;    
};

/*the Sync Layer config descriptor*/
struct isom_sl_config_descriptor_t {
    /*base descriptor*/
    UInt8 tag;    
    UInt8 predefined;
    UInt8 use_access_unit_start_flag;
    UInt8 use_access_unit_end_flag;
    UInt8 use_random_access_point_flag; // stbl->SyncSample->entryCount 1이상이면 1
    UInt8 has_random_access_units_only_flag; // stbl->SyncSample 존재안하면 1
    UInt8 use_padding_flag;
    UInt8 use_timestamps_flag;
    UInt8 use_idle_flag;
    UInt8 duration_flag;
    UInt32 timestamp_resolution; // sampling rate (ex. mpeg4v 90000, qcelp, evrc 8000)
    UInt32 ocr_resolution;
    UInt8 timestamp_length;
    UInt8 ocr_length;
    UInt8 au_length;
    UInt8 instant_bitrate_length;
    UInt8 degradation_priority_length;
    UInt8 au_seqnum_length; // total sample count의 2진수값의 길이
    UInt8 packet_seqnum_length;
    UInt32 timescale;
    UInt16 au_duration;
    UInt16 cu_duration;
    UInt64 startdts;
    UInt64 startcts;
};

struct isom_avc_config_t {
    UInt8       configuration_version;
    UInt8       avc_profile_indication;
    UInt8       profile_compatibility;
    UInt8       avc_level_indication; 
    UInt8       nal_unit_size;
    ut_list_t  *sequence_parameter_sets;
    ut_list_t  *picture_parameter_sets;
};

struct isom_avcconfiguration_box_t {
    UInt32      type;
    UInt64      size;
    isom_avc_config_t *config;
};

struct isom_mpeg4_bitrate_box_t {
    UInt32      type;
    UInt64      size;
    UInt32      buffer_sizedb;
    UInt32      max_bitrate;
    UInt32      avg_bitrate;
};

struct isom_mpeg4_extension_descriptors_box_t {
    UInt32      type;
    UInt64      size;
    ut_list_t    *descriptors;
};

struct isom_decoder_config_t {
    UInt8 tag;
    UInt8 object_type_indication;
    UInt8 stream_type;
    UInt8 up_stream;
    UInt32 buffer_sizedb;
    UInt32 max_bitrate;
    UInt32 avg_bitrate;
    isom_default_descriptor_t *decoder_specific_info;
    ut_list_t *profile_level_indication_index_descriptor;
};

struct isom_default_descriptor_t {
    UInt8    tag;
    UInt32   data_length;
    char    *data;
};

struct isom_ipi_ptr_t {
    UInt8       tag;
    UInt16      ipi_es_id;
};

struct isom_qos_descriptor_t {
    UInt8       tag;
    UInt8       predefined;
    ut_list_t    *qos_qualifiers;
};

/*Registration Descriptor*/
struct isom_registration_t {
    UInt8       tag;
    UInt32      format_identifier;
    UInt32      data_length;
    char       *additional_identification_info;
};

/*Language Descriptor*/
struct isom_language_t {
    UInt8       tag;
    UInt32      lang_code;
};

struct isom_unknown_uuid_box_t {
    UInt32      type;
    UInt64      size;
    UInt8       uuid[16];
    char       *data;
    UInt32      dataSize;
};

struct isom_mpeg_audio_sample_entry_box_t {
    UInt32 type;			
    UInt64 size;			
    UInt16 data_reference_index;				
    char reserved[ 6 ];					
    //ts_protect_box *protection_info;		
    UInt16 version;					
    UInt16 revision;					
    UInt32 vendor;						
    UInt16 channel_count;				
    UInt16 bits_per_sample;				
    UInt16 compression_id;				
    UInt16 packet_size;				
    UInt16 samplerate_hi;				
    UInt16 samplerate_lo;
    isom_esd_box_t *esd;
    isom_sl_config_descriptor_t *slc;
};

struct isom_avc_config_slot_t {
    UInt16 size;
    char  *data;
};


#ifdef __cplusplus
}
#endif

#endif