#ifndef RTP_STREAMER_H
#define RTP_STREAMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "os_header.h"
#include "bit_operation.h"
#include "isom_mp4_avc_box.h"
#include "isom_file.h"
#include "data_resource.h"


#define CRLF_ "\r\n"
enum 
{
    AVC_SDP_RFC_3984,
    AVC_SDP_IETF_DRAFT,
};

#define DEFAULT_VIDEO_RTP_RATE   90000
#define DEFAULT_VOICE_RTP_RATE   8000

enum 
{
    /*not defined*/
    RTP_PAYT_UNKNOWN,
    /*use generic MPEG-4 transport - RFC 3016 and RFC 3640*/
    RTP_PAYT_MPEG4,
    /*use generic MPEG-1/2 video transport - RFC 2250*/
    RTP_PAYT_MPEG12_VIDEO,
    /*use generic MPEG-1/2 audio transport - RFC 2250*/
    RTP_PAYT_MPEG12_AUDIO,
    /*use H263 transport - RFC 2429*/
    RTP_PAYT_H263,
    /*use AMR transport - RFC 3267*/
    RTP_PAYT_AMR,
    /*use AMR-WB transport - RFC 3267*/
    RTP_PAYT_AMR_WB,
    /*use QCELP transport - RFC 2658*/
    RTP_PAYT_QCELP,
    /*use EVRC/SMV transport - RFC 3558*/
    RTP_PAYT_EVRC_SMV,
    /*use 3GPP Text transport - no RFC yet, only draft*/
    RTP_PAYT_3GPP_TEXT,
    /*use H264 transport - no RFC yet, only draft*/
    RTP_PAYT_H264_AVC,
    /*use LATM for AAC-LC*/
    RTP_PAYT_LATM,
};

typedef struct rtp_header_info_t           rtp_header_info_t;
typedef struct rtp_mp4_es_generic_info_t   rtp_mp4_es_generic_info_t;
typedef struct rtp_packet_info_t           rtp_packet_info_t;
typedef struct track_sample_control_info_t track_sample_control_info_t;
//typedef struct rtp_packet_output_info_t    rtp_packet_output_info_t;
typedef struct rtp_packet_send_info_t      rtp_packet_send_info_t;
typedef struct access_unit_info_t          access_unit_info_t;
//typedef struct sample_object_t             sample_object_t;
typedef struct rtp_sl_header_t             rtp_sl_header_t;
typedef struct track_bitrate_info_t        track_bitrate_info_t;
typedef struct track_packetize_info_t      track_packetize_info_t;
typedef struct track_decode_info_t         track_decode_info_t;
typedef struct track_stream_info_t         track_stream_info_t;

typedef struct track_stream_info_entry_t   track_stream_info_entry_t;
typedef struct session_stream_info_t       session_stream_info_t;

/*
* Desc : RTP header info
*/
struct rtp_header_info_t {
    /*version, must be 2*/
    UInt8 version;
    /*padding bits in the payload*/
    UInt8 padding;
    /*header extension is defined*/
    UInt8 extension;
    /*number of CSRC (<=15)*/
    UInt8 csrc_count;
    /*Marker Bit*/
    UInt8 marker;
    /*payload type on 7 bits*/
    UInt8 payload_type;
    /*packet seq number*/
    UInt16 sequence_number;
    /*packet time stamp*/
    UInt32 timestamp;
    /*sync source identifier*/
    UInt32 ssrc;
    /*in our basic client, CSRC should always be NULL*/
    UInt32 csrc[16];
};

/*
* Desc : RTP Packet (rtp header + rtp payload) info
*/
struct rtp_packet_info_t {
    rtp_header_info_t  rtp_header_info;
    char              *rtp_payload_data; // payload header를 포함한 payload
    UInt32             rtp_payload_size;
};

/*
* Desc : RTP Payload Format for Transport of MPEG-4 Elementary Streams  (RFC3640 generic)
*/
struct rtp_mp4_es_generic_info_t {
    char     mode[30];
    char    *config;
    UInt32   config_size;
    /* Stream Type*/
    UInt8    stream_type;
    /* stream profile and level indication - for AVC/H264, 0xPPCCLL, with PP:profile, CC:compatibility, LL:level*/
    UInt32   pl_id;
    UInt32   constant_size;
    /*duration of AUs if constant, in RTP timescale*/
    UInt32   constant_duration;
    /* Object Type Indication */
    UInt8    object_type_indication;
    /*audio max displacement when interleaving (eg, de-interleaving window buffer max length) in RTP timescale*/
    UInt32   maxd_is_placement;
    /*de-interleaveBufferSize if not recomputable from maxDisplacement*/
    UInt32   deinterleave_buffer_size;
    /*The number of bits on which the AU-size field is encoded in the AU-header*/
    UInt32   size_length;
    /*The number of bits on which the AU-Index is encoded in the first AU-header*/
    UInt32   index_length;
    /*The number of bits on which the AU-Index-delta field is encoded in any non-first AU-header*/
    UInt32   index_delta_length;
    /*The number of bits on which the DTS-delta field is encoded in the AU-header*/
    UInt32   dts_delta_length;
    /*The number of bits on which the CTS-delta field is encoded in the AU-header*/
    UInt32   cts_delta_length;
    /*random access point flag present*/
    UInt32   random_access_indication;
    /*The number of bits on which the Stream-state field is encoded in the AU-header (systems only)*/
    UInt32   stream_state_indication;
    /*The number of bits that is used to encode the auxiliary-data-size field 
    (no normative usage of this section)*/
    UInt32   auxiliary_data_size_length;
    /*ISMACryp stuff*/
    UInt8    iv_length, iv_delta_length;
    UInt8    ki_length;
    UInt32   auh_first_min_len;
    UInt32   auh_min_len;
};

/*
* Desc : SL Header 구조체 RTP Packetizing 에서 필요한 정보만 사용.  
*/
struct rtp_sl_header_t { 
    UInt8  access_unit_start_flag;   //au packetizer start를 알림 (au에 대해 파싱이 끝나면 1로 변경)
    UInt8  access_unit_end_flag;     //Makerbit setting (au에 대한 파싱이 끝나면 0으로 셋팅)
    UInt8  padding_flag;             //packetizing 
    UInt8  is_RAP; //IS RAP frame
    UInt8  decoding_timestamp_flag;
    UInt8  composition_timestamp_flag;    
    UInt8  padding_bits;
    UInt16 packet_sequence_number;
    UInt16 au_sequence_number;
    UInt64 decoding_timestamp;
    UInt64 composition_timestamp;    //rtp packet에 적용 될 timestamp
    UInt16 accessunit_length;        // au size    
    UInt32 au_duration;
};

/*
* Desc : 완성된 RTP Packet Data 및 Packet 관련 정보
*/
struct rtp_packet_send_info_t {
    Bool16  has_packet;   // rtp packet setting flag
    UInt32  rtp_packet_size;
    UInt32  track_id;
    UInt64  DTS;        // (millisec, transmit_time(sec)으로 활용)
    char   *rtp_packet_data;
};

/*
* Desc : access unit 정보, media sample object
*/
//struct access_unit_info_t {
struct access_unit_info_t {
    UInt32  data_length;
    char   *data;                   // data with padding if requested
    UInt64  DTS;                    //현재까지 누적된 decoding time, Decoding Time Stamp
    UInt32  cts_offset;                // relative offset for composition if needed
    UInt8   is_RAP;  // Random Access Point : (0: not random access, 1: regular RAP)
};

/*
* Desc : packetize inof
*/
struct track_packetize_info_t {
    UInt32 readbytes_leftsize_in_au;                       //access unit offset
    UInt32 nalunit_size_info;                              // 14496-15 lengthSizeMinusOne field 참조
    UInt32 usable_mtu_payload_size;                        // Path MTU size without 12-bytes RTP header
    UInt32 first_sl_in_rtp;                                // set to 1 if firstSL in RTP packet
    UInt32 has_au_header;
    UInt32 au_count_in_packet;
    UInt32 bytes_in_rtp_payload;                           // multi packet인 경우 현재까지 준비된 payload의 사이즈
    UInt32 mp4_packetize_au_header_size;                   // auh_size -> mp4 packetize au header 계산에 사용 
    Bool16 avc_non_idr;                                    // avc non i-frame
    isom_bit_operation_t *bo_payload_header, *bo_payload_data; // TODO: bo_payload_header, bo_payload 의미(역할) 추후에 재정의. 분석 필요.
};

/*
* Desc : Codec별 sdp생성, packetizing 을 위해 decode정보를 담는 구조체 
*/
struct track_decode_info_t {   
    track_bitrate_info_t *track_bitrate_info;
    UInt32                rtp_timestamp;    
    UInt32                rfc_codec_type;   // RTP payload type (RFC type, NOT the RTP hdr payT)
    UInt32                decode_option;	// packetization flags
    UInt32                au_max_size;      // access unit max_size		
    UInt32                total_au_count;
    UInt32                bandwidth;    
    UInt32                nb_chan;          //sdp audio channel info
    
    /*rest of struct is for MPEG-4 Generic hinting */
    rtp_mp4_es_generic_info_t   rtp_mp4_es_generic_info; //RTP Payload Format for Transport of MPEG-4 Elementary Streams  (RFC3640 generic)
    rtp_sl_header_t             rtp_sl_header;
    isom_sl_config_descriptor_t sl_config_descriptor;      //SL conf and state
    UInt32 auh_size;
    UInt64 iv;                                         // TODO: IV 는 적절한 이름으로 변경
};

/*
* Desc :  h264, mp4v, mp4a Codec Box에서 Bitrate 정보를 저장하기위한 구조체
*/
struct track_bitrate_info_t {
    UInt32      buffer_sizedb;
    UInt32      max_bitrate;
    UInt32      avg_bitrate;
};

/*
* Desc :  box 구조체에서 임시변수 성격의 멤버들이 Cache의 경우 문제가 되므로 아래 control구조체를 사용함.  
*/
struct track_sample_control_info_t {
    UInt64      stts_current_dts;
    UInt32      stts_current_entry_index;
    UInt32      stts_first_samp_in_entry;

    UInt32      stss_last_sync_sample;
    UInt32      stss_last_sample_index;
        
    UInt32      stsc_current_index;
    UInt32      stsc_current_chunk;
    UInt32      stsc_first_sample_in_current_chunk;
    UInt32      stsc_ghost_number;

    UInt32      ctts_current_entry_index;
    UInt32      ctts_first_samp_in_entry;
};

/*
* Desc : track별 Stream 정보 
*/
struct track_stream_info_t {
    track_decode_info_t	        *track_decode_info;
    track_packetize_info_t      *track_packetize_info;
    track_sample_control_info_t *track_sample_control_info;
    access_unit_info_t          *au_info;
    isom_box_trak_t             *box_trak; 

    UInt32                       current_au_number;       // current sample number
    UInt32                       au_duration;             // au duration = sample_duration = sample dur * timescale    
    
    Bool16                       is_packetizing;        // 해당 track의 패키타이징 여부
    rtp_packet_info_t            rtp_packet_info;
    UInt64                       timestamp_random_offset; // timestamp random offset
    void                        *reserved;                // reserved (streamer rtp_session info)
	double                       ts_scale; // track_stream_info->track_decode_info->sl_config_descriptor.timestamp_resolution / track_timescale
    double                       microsec_ts_scale;
    UInt64                       microsec_dts;
    
};

/*
* Desc : track_stream_info_t 구조체를 Linked List로 저장하기위한 구조체
*/
struct track_stream_info_entry_t {
    UInt32                          track_stream_info_entry_id;
    track_stream_info_t            *track_stream_info;
    track_stream_info_entry_t      *next_track_stream_info_entry;
};

/*
* Desc : 최상위 구조체 
*/
struct session_stream_info_t {
    SInt32                      result_code;
    //char                       *content_path;   // file명 포함한 전체 path
    char                       *output_sdp;
    UInt8                       conf_payload_type;
    UInt32                      has_stream_info_track_count;				// stream 정보를 갖고 있는 track count
    UInt32                      rtp_port;   
    UInt32                      content_msec_duration;
    UInt32                      mtu_size;
    UInt64                      seek_dts;								    // video인 경우 i-frame 적용된 dts
    //isom_resource_t            *stream_data_resource;         
    isom_file_info_t           *mp4file_info;
    rtp_packet_send_info_t     *rtp_packet_send_info;
    
    track_stream_info_entry_t  *track_stream_info_list_entry;   
    track_stream_info_entry_t  *track_stream_info_list_head;   
    track_stream_info_entry_t  *track_stream_info_list_tail;  
};
    
rs_status_t rtpfile_next_track_stream_info(session_stream_info_t *session_stream_info, track_stream_info_t **track_stream_info, track_stream_info_t *last_found_track_stream_info);

rs_status_t rtpfile_find_track_stream_info(session_stream_info_t *session_stream_info,UInt32 track_id, track_stream_info_t **track_stream_info);

void rtpfile_delete_track_decode_packetize_info(track_stream_info_t *track_decode_info);

void rtpfile_delete_track_stream_info(session_stream_info_t *session_stream_info); 


rs_status_t rtpfile_initialize_mp4fileinfo(const char *content_path, isom_file_info_t **mp4file_info, isom_resource_t *data_resource);

rs_status_t rtpfile_init_session(void **new_session_stream_info, UInt32 mtu_size, 
                                    UInt8 conf_payload_type, isom_file_info_t *mp4file_info);

SInt32 rtpfile_delete_session(session_stream_info_t *session_stream_info);

char* rtpfile_get_content_path(session_stream_info_t *session_stream_info);
SInt32 rtpfile_get_result_code(session_stream_info_t *session_stream_info);
rs_status_t rtpfile_set_rtp_port(session_stream_info_t *session_stream_info, UInt32 rtp_port);
char* rtpfile_get_session_sdp(session_stream_info_t *session_stream_info);
rs_status_t rtpfile_set_track_cookies(session_stream_info_t *session_stream_info, UInt32 track_id, void *cookies);
SInt32 rtpfile_get_track_cookies(session_stream_info_t *session_stream_info, UInt32 track_id, void **cookies);
rs_status_t rtpfile_set_session_stream_info_by_seeking(session_stream_info_t *session_stream_info, double duration, UInt32 rap_find_t);
double rtpfile_get_first_packet_transmit_time(session_stream_info_t *session_stream_info);
UInt32 rtpfile_get_timestamp(session_stream_info_t *session_stream_info, UInt32 track_id);
UInt16 rtpfile_get_next_sequence_number(session_stream_info_t *session_stream_info, UInt32 track_id);
UInt32 rtpfile_get_timescale(session_stream_info_t *session_stream_info, UInt32 track_id);
rs_status_t rtpfile_get_rtp_packet(session_stream_info_t *session_stream_info);
rs_status_t rtpfile_set_drop_repeat_packets(session_stream_info_t *session_stream_info, UInt16 allow_repeat_packets);
double rtpfile_get_next_packet(session_stream_info_t *session_stream_info, char **packet, UInt32 *packet_length);
UInt32 rtpfile_get_last_packet_track_id(session_stream_info_t *session_stream_info);
UInt32 rtpfile_get_session_mili_duration(session_stream_info_t *session_stream_info);
UInt32 rtpfile_get_track_count(session_stream_info_t *session_stream_info);
UInt32 rtpfile_get_trackid(track_stream_info_t *track_stream_info);
track_stream_info_t* rtpfile_get_track_stream_info(session_stream_info_t *session_stream_info, UInt32 track_id);
UInt32 rtpfile_get_payload_type(track_stream_info_t *track_stream_info);
rs_status_t rtpfile_get_media_payload_name(track_stream_info_t *track_stream_info, char *sz_payload_name, char *sz_media_name);
UInt32 rtpfile_get_iv_length(track_stream_info_t *track_stream_info);
UInt32 rtpfile_get_bandwidth(track_stream_info_t *track_stream_info);
UInt32 rtpfile_get_timestamp_resolution(track_stream_info_t *track_stream_info);
UInt32 rtpfile_get_media_handler_type(track_stream_info_t *track_stream_info);
SInt32 rtpfile_get_visual_info_w_h(track_stream_info_t *track_stream_info, UInt32 *width, UInt32 *height);
UInt32 rtpfile_get_rfc_codec_type(track_stream_info_t *track_stream_info);
UInt32 rtpfile_get_auh_size(track_stream_info_t *track_stream_info);
isom_avc_config_t *ts_get_avc_config_default(track_stream_info_t *track_stream_info);
SInt32 rtpfile_get_avc_profile_level_id(isom_avc_config_t *avcc, char *sdp_plid);
rs_status_t rtpfile_get_avc_sps_rfc3984(isom_avc_config_t *avcc, char *sdp_sps);
rs_status_t rtpfile_get_avc_pps_rfc3984(isom_avc_config_t *avcc, char *sdp_pps);
rs_status_t rtpfile_get_avc_sps_custom_draft(isom_avc_config_t *avcc, char *sdp_sps);
rs_status_t rtpfile_get_avc_pps_custom_draft(isom_avc_config_t *avcc, char *sdp_pps);
rs_status_t rtpfile_delete_avc_config(isom_avc_config_t *avcc);
UInt32 rtpfile_get_mp4_profile_level_id(track_stream_info_t  *track_stream_info);
rs_status_t rtpfile_get_mp4v_dsi_config(track_stream_info_t  *track_stream_info, char *sz_config);
rs_status_t rtpfile_get_mp4a_dsi_config(track_stream_info_t  *track_stream_info, char *sz_config);
UInt32 rtpfile_get_sdp_media_bandwidth(track_stream_info_t  *track_stream_info);
rs_status_t rtpfile_set_sdp(session_stream_info_t *session_stream_info, UInt32 avc_ver);
rs_status_t rtpfile_init_packet_send_info(session_stream_info_t *session_stream_info, UInt32 path_mtu);
rs_status_t rtps_init_packet_send_info(session_stream_info_t *session_stream_info, UInt32 path_mtu);
rs_status_t rtpfile_add_track_stream_info(session_stream_info_t *session_stream_info, UInt32 track_id, UInt32 b_rand);
rs_status_t rtpfile_set_track_stream_info(session_stream_info_t *session_stream_info);
rs_status_t rtpfile_set_track_ssrc(session_stream_info_t *session_stream_info, UInt32 track_id, UInt32 ssrc);
rs_status_t rtpfile_process_rtp_packetizing(session_stream_info_t *session_stream_info, track_stream_info_t *track_stream_info,
                                  char *data, UInt32 data_size, UInt8 is_au_end, UInt32 full_au_size, UInt32 duration);
rs_status_t rtpfile_set_session_output_sdp(session_stream_info_t *session_stream_info, char *sdp_out);
UInt32 rtpfile_get_max_bitrate(track_stream_info_t *track_stream_info);
UInt32 rtpfile_get_avg_bitrate(track_stream_info_t *track_stream_info);
UInt32 rtpfile_get_buffer_sizedb(track_stream_info_t *track_stream_info);

#ifdef __cplusplus
}
#endif

#endif