#ifndef SAMPLE_DESCRIPTION_H
#define SAMPLE_DESCRIPTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "os_header.h"
#include "isom_file.h"
#include "rtp_streamer.h"
#include "sample_pck_func.h"
#include "isom_stsd_box.h"


enum
{
    /*forces MPEG-4 generic transport if MPEG-4 systems mapping is available*/
    RTP_PCK_FORCE_MPEG4 =	(1),
    /*Enables AUs concatenation in an RTP packet (if payload supports it) - this forces TS_RTP_PCK_SIGNAL_SIZE for MPEG-4*/
    RTP_PCK_USE_MULTI	=	(1<<1),
    /*if set, audio interleaving is used if payload supports it (forces RTP_PCK_USE_MULTI flag)
    THIS IS CURRENTLY NOT IMPLEMENTED*/
    RTP_PCK_USE_INTERLEAVING =	(1<<2),
    /*uses static RTP payloadID if any defined*/
    RTP_PCK_USE_STATIC_ID =	(1<<3),
    
    /*MPEG-4 generic transport option*/
    /*if flag set, RAP flag is signaled in RTP payload*/
    RTP_PCK_SIGNAL_RAP	=	(1<<4),
    /*if flag set, AU indexes are signaled in RTP payload*/
    RTP_PCK_SIGNAL_AU_IDX	=	(1<<5),
    /*if flag set, AU size is signaled in RTP payload*/
    RTP_PCK_SIGNAL_SIZE	=	(1<<6),
    /*if flag set, CTS is signaled in RTP payload - DTS is automatically set if needed*/
    RTP_PCK_SIGNAL_TS	=	(1<<7),
    
    /*setup payload for carouseling of systems streams*/
    RTP_PCK_AUTO_CAROUSEL = (1<<8),
    
    /*use LATM payload for AAC-LC*/
    RTP_PCK_USE_LATM_AAC	=	(1<<9),
    
    /*ISMACryp options*/
    /*signals that input data is selectively encrypted (eg not all input frames are encrypted) 
    - this is usually automatically set by hinter*/
    RTP_PCK_SELECTIVE_ENCRYPTION =	(1<<10),
    /*signals that each sample will have its own key indicator - ignored in non-multi modes
    if not set and key indicator changes, a new RTP packet will be forced*/
    RTP_PCK_KEY_IDX_PER_AU =	(1<<11),
};


enum
{
/*reserved, internal use in the lib. Indicates the track complies to MPEG-4 system
    specification, and the usual OD framework tools may be used*/
    ISOM_SUBTYPE_MPEG4		= FOUR_CHARS_TO_INT( 'M', 'P', 'E', 'G' ),
        
    /*reserved, internal use in the lib. Indicates the track is of GF_ISOM_SUBTYPE_MPEG4
    but it is encrypted.*/
    ISOM_SUBTYPE_MPEG4_CRYP	= FOUR_CHARS_TO_INT( 'E', 'N', 'C', 'M' ),
    
    /*AVC/H264 media type - not listed as an MPEG-4 type, ALTHOUGH this library automatically remaps
    GF_AVCConfig to MPEG-4 ESD*/
    ISOM_SUBTYPE_AVC_H264	= FOUR_CHARS_TO_INT( 'a', 'v', 'c', '1' ),
    
};

enum
{
    M4A_AAC_MAIN = 1,
    M4A_AAC_LC = 2,
    M4A_AAC_SSR = 3,
    M4A_AAC_LTP = 4,
    M4A_AAC_SBR = 5,
    M4A_AAC_SCALABLE = 6,
    M4A_TWINVQ = 7,
    M4A_CELP = 8, 
    M4A_HVXC = 9,
    M4A_TTSI = 12,
    M4A_MAIN_SYNTHETIC = 13,
    M4A_WAVETABLE_SYNTHESIS = 14,
    M4A_GENERAL_MIDI = 15,
    M4A_ALGO_SYNTH_AUDIO_FX = 16,
    M4A_ER_AAC_LC = 17,
    M4A_ER_AAC_LTP = 19,
    M4A_ER_AAC_SCALABLE = 20,
    M4A_ER_TWINVQ = 21,
    M4A_ER_BSAC = 22,
    M4A_ER_AAC_LD = 23,
    M4A_ER_CELP = 24,
    M4A_ER_HVXC = 25,
    M4A_ER_HILN = 26,
    M4A_ER_PARAMETRIC = 27,
    M4A_SSC = 28,
    M4A_PS = 29,
    M4A_LAYER1 = 32,
    M4A_LAYER2 = 33,
    M4A_LAYER3 = 34,
    M4A_DST = 35,
    M4A_ALS = 36,
};

/*
MPEG-4 video (14496-2)
*/

#define M4V_VO_START_CODE					0x00
#define M4V_VOL_START_CODE					0x20
#define M4V_VOP_START_CODE					0xB6
#define M4V_VISOBJ_START_CODE				0xB5
#define M4V_VOS_START_CODE					0xB0
#define M4V_GOV_START_CODE					0xB3
#define M4V_UDTA_START_CODE					0xB2


#define M2V_PIC_START_CODE					0x00
#define M2V_SEQ_START_CODE					0xB3
#define M2V_EXT_START_CODE					0xB5
#define M2V_GOP_START_CODE					0xB8

// list에서 저장된 코덱 정보를 꺼낼 때 
#define LIST_AVC1_INDEX                  0
#define LIST_AVCC_INDEX                  1

typedef struct m4v_parser_t        m4v_parser_t;
typedef struct m4a_decspec_info_t  m4a_decspec_info_t;
typedef struct m4v_decspecinfo_t   m4v_decspecinfo_t;

// TODO: 분석 후 구조체 이름 변경, 관련 함수명도 변경
struct m4v_parser_t {
    isom_bit_operation_t *bs;
    Bool16 mpeg12;
    UInt32 current_object_type;
    UInt32 current_object_start;
    UInt32 tc_dec, prev_tc_dec, tc_disp, prev_tc_disp;
};

/*get Audio type from dsi. return audio codec type:*/
struct m4a_decspec_info_t {
    UInt32 nb_chan;
    UInt32 base_object_type, base_sr, base_sr_index;
    /*SBR*/
    Bool16 has_sbr;
    UInt32 sbr_object_type, sbr_sr, sbr_sr_index;
    /*PL indication*/
    UInt8 audioPL;
};

struct m4v_decspecinfo_t {
    /*video PL*/
    UInt8 VideoPL; //profile_level_id
    UInt8 RAP_stream, objectType, has_shape, enh_layer;
    /*video resolution*/
    UInt16 width, height;
    /*pixel aspect ratio*/
    UInt8 par_num, par_den;    
    UInt16 clock_rate;
    UInt8 NumBitsTimeIncrement;
    UInt32 time_increment;
    /*for MPEG 1/2*/
    double fps;
};


rs_status_t rtpfile_init_track_sample_control_info(isom_box_trak_t *box_trak,
                                                   track_sample_control_info_t **track_sample_control_info);

rs_status_t rtpfile_set_track_decode_packetize_info(session_stream_info_t *session_stream_info,
                                                    track_stream_info_t *track_stream_info,
                                                    isom_box_trak_t *box_trak,
                                                    UInt32 usable_mtu_payload_size,
                                                    UInt32 decode_option);

UInt32 rtpfile_get_media_subtype(isom_box_trak_t *box_trak, UInt32 description_index);

// TODO : rtpfile_set_decode_option_use_mp4_generic_info 이름 상의
void rtpfile_set_decode_option_use_mp4_generic_info(track_stream_info_t *track_stream_info, UInt32 path_mtu, 
                   UInt32 stream_type, UInt32 oti, UInt32 pl_id,
                   UInt32 avg_size, UInt32 max_size, 
                   UInt32 abg_ts, UInt32 max_dts,
                   UInt32 iv_length, UInt32 ki_length,
                   char *pref_mode);

UInt32 ut_strnicmp(char *str, char *needle, UInt32 len);

Bool16 rtpfile_set_avc_bitrate_info(isom_box_trak_t *box_trak, UInt32 description_index, track_bitrate_info_t *bitrate_info);

rs_status_t rtpfile_get_avg_info(track_stream_info_t *track_stream_info, isom_box_trak_t *box_trak, UInt32 *avg_size, UInt32 *max_size, UInt32 *time_delta, UInt32 *max_cts_delta, UInt32 *const_duration, UInt32 *bandwidth);

UInt8 rtpfile_has_sync_points(isom_box_trak_t *box_trak);

UInt32 ut_encode_base64(char *_in, UInt32 in_size, char *_out, UInt32 out_size);

UInt32 ut_encode_base16(char *_in, UInt32 in_size, char *_out, UInt32 out_size);

rs_status_t rtpfile_get_payload_name(track_decode_info_t *rtpb, char *sz_payload_name, char *sz_media_name);

Bool16 rtpfile_get_visual_info(isom_box_trak_t *box_trak, UInt32 stream_description_index, UInt32 *width, UInt32 *height);

isom_decoder_config_t *rtpfile_get_decoder_config(isom_box_trak_t *box_trak, UInt32 stream_description_index);

Bool16 rtpfile_is_mp4_description(UInt32 entry_type);

Bool16 rtpfile_is_mp4_encrypted_description(UInt32 entry_type);

Bool16 rtpfile_get_esd_from_stsd(isom_box_stsd_t *stsd, UInt32 stream_description_index, isom_esd_t **out_esd, Bool16 true_desc_only);

Bool16 rtpfile_get_esd_from_trak(isom_box_trak_t *box_trak, UInt32 stream_description_index, isom_esd_t **out_esd);

Bool16 rtpfile_get_sample_description(isom_box_stsd_t *stsd, UInt32 stream_description_index, isom_sample_entry_box_t **out_entry, UInt32 *dataRefIndex);

Bool16 rtpfile_track_find_ref(isom_box_trak_t *box_trak, isom_track_referencetypebox_t **dpnd);

UInt32 rtpfile_get_m4a_profile_level_id(m4a_decspec_info_t *cfg);

Bool16 rtpfile_get_m4a_config(char *dsi, UInt32 dsi_size, m4a_decspec_info_t *cfg);

Bool16 rtpfile_get_m4v_config(char *rawdsi, UInt32 rawdsi_size, m4v_decspecinfo_t *dsi);

m4v_parser_t *rtpfile_create_m4v_parser(char *data, UInt32 data_size, Bool16 mpeg12video);

void rtpfile_delete_m4v_parser(m4v_parser_t *m4v);

Bool16 rtpfile_parse_m4v_config_mpeg4(m4v_parser_t *m4v, m4v_decspecinfo_t *dsi);

SInt32 rtpfile_read_m4v_object(m4v_parser_t *m4v);

Bool16 rtpfile_parse_m4v_config(m4v_parser_t *m4v, m4v_decspecinfo_t *dsi);

isom_avc_config_t *rtpfile_get_avc_config(isom_box_trak_t *box_trak, UInt32 description_index);

isom_avc_config_t *rtpfile_duplicate_avc_config(isom_avc_config_t *cfg);

void ts_initialize_sl(isom_sl_config_descriptor_t *slc);

#ifdef __cplusplus
}
#endif

#endif