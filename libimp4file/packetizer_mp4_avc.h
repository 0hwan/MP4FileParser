#ifndef PACKETIZER_MP4_AVC_H
#define PACKETIZER_MP4_AVC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "os_header.h"
#include "rtp_streamer.h"

/*AVC NAL unit types*/
#define AVC_NALU_NON_IDR_SLICE   0x1
#define AVC_NALU_DP_A_SLICE      0x2
#define AVC_NALU_DP_B_SLICE      0x3
#define AVC_NALU_DP_C_SLICE      0x4
#define AVC_NALU_IDR_SLICE       0x5
#define AVC_NALU_SEI             0x6
#define AVC_NALU_SEQ_PARAM       0x7
#define AVC_NALU_PIC_PARAM       0x8
#define AVC_NALU_ACCESS_UNIT     0x9
#define AVC_NALU_END_OF_SEQ      0xa
#define AVC_NALU_END_OF_STREAM   0xb
#define AVC_NALU_FILLER_DATA     0xc
    
#define AVC_TYPE_P               0
#define AVC_TYPE_B               1
#define AVC_TYPE_I               2
#define AVC_TYPE_SP              3
#define AVC_TYPE_SI              4
#define AVC_TYPE2_P              5
#define AVC_TYPE2_B              6
#define AVC_TYPE2_I              7
#define AVC_TYPE2_SP             8
#define AVC_TYPE2_SI             9



UInt32 rtpfile_get_rtp_au_header_size(session_stream_info_t *session_stream_info, track_stream_info_t *track_stream_info, rtp_sl_header_t *slh);

rs_status_t rtpfile_packetize_mpeg4(session_stream_info_t *session_stream_info, track_stream_info_t *track_stream_info, char *data, UInt32 data_size, UInt8 is_au_end, UInt32 full_au_size);

UInt32 rtpfile_write_rtp_au_header(session_stream_info_t *session_stream_info, track_stream_info_t *track_stream_info, UInt32 payloadsize, UInt32 rtp_ts);

rs_status_t rtpfile_packetize_latm(session_stream_info_t *session_stream_info, track_stream_info_t *track_stream_info, char *data, UInt32 data_size, UInt32 duration);

void rtpfile_flush_latm(session_stream_info_t *session_stream_info, track_stream_info_t *track_stream_info);

rs_status_t rtpfile_packetize_avc(session_stream_info_t *session_stream_info, track_stream_info_t *track_stream_info, char *data, UInt32 data_size, UInt8 is_au_end);

#ifdef __cplusplus
}
#endif

#endif


