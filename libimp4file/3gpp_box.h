#ifndef _3GPP_BOX_H
#define _3GPP_BOX_H

#ifdef __cplusplus
extern "C" {
#endif


#include "data_resource.h"
#include "misc_list.h"



typedef struct isom_3gp_config isom_3gp_config;
typedef struct isom_3gpp_config_box isom_3gpp_config_box;
typedef struct isom_3gpp_audio_sample_entry_box isom_3gpp_audio_sample_entry_box;
typedef struct isom_3gpp_visual_sample_entry_box isom_3gpp_visual_sample_entry_box;

/*Generic 3GP/3GP2 config record*/
struct isom_3gp_config
{
    /*GF_4CC record type, one fo the above GF_ISOM_SUBTYPE_3GP_ * subtypes*/
    UInt32 type;
    /*4CC vendor name*/
    UInt32 vendor;
    /*codec version*/
    UInt8 decoder_version;
    /*number of sound frames per IsoMedia sample, >0 and <=15. The very last sample may contain less frames. */
    UInt8 frames_per_sample;
    /*H263 ONLY - Level and profile*/
    UInt8 h263_level, h263_profile;    
    /*AMR(WB) ONLY - num of mode for the codec*/
    UInt16 amr_mode_set;
    /*AMR(WB) ONLY - changes in codec mode per sample*/
    UInt8 amr_mode_change_period;
};

struct isom_3gpp_config_box
{
    UInt32 type;			
	UInt64 size;			
    isom_3gp_config cfg;
};


struct isom_3gpp_visual_sample_entry_box
{
    UInt32 type;			
	UInt64 size;			
	UInt8 uuid[16];		
    UInt16 data_reference_index;				
    char reserved[ 6 ];					
//struct __tag_protect_box *protection_info;
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
    isom_3gpp_config_box *info;
};

struct isom_3gpp_audio_sample_entry_box
{
    UInt32 type;			
	UInt64 size;			
	UInt8 uuid[16];		
    UInt16 data_reference_index;				
    char reserved[ 6 ];					
	//struct isom_tag_protect_box *protection_info;
	UInt16 version;					
    UInt16 revision;					
    UInt32 vendor;						    
    UInt16 channel_count;				    
    UInt16 bitspersample;				
    UInt16 compression_id;				
    UInt16 packet_size;				
    UInt16 samplerate_hi;				
	UInt16 samplerate_lo;
    isom_3gpp_config_box *info;
};


#ifdef __cplusplus
}
#endif

#endif

