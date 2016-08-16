#ifndef PACKTIZER_3GPP_H
#define PACKTIZER_3GPP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "os_header.h"
#include "rtp_streamer.h"

/*rate sizes - note that these sizes INCLUDE the rate_type header byte*/
static const UInt32 QCELP_RATE_TO_SIZE [] = {0, 1, 1, 4, 2, 8, 3, 17, 4, 35, 5, 8, 14, 1};
static const UInt32 QCELP_RATE_TO_SIZE_NB = 7;
static const UInt32 SMV_EVRC_RATE_TO_SIZE[] = {0, 1, 1, 3, 2, 6, 3, 11, 4, 23, 5, 1};
static const UInt32 SMV_EVRC_RATE_TO_SIZE_NB = 6;
static const UInt32 AMR_FRAME_SIZE[16] = { 12, 13, 15, 17, 19, 20, 26, 31, 5, 0, 0, 0, 0, 0, 0, 0 };
static const UInt32 AMR_WB_FRAME_SIZE[16] = { 17, 23, 32, 36, 40, 46, 50, 58, 60, 5, 5, 0, 0, 0, 0, 0 };

rs_status_t rtpfile_packetize_h263(session_stream_info_t *session_stream_info, track_stream_info_t *track_stream_info, char *data, UInt32 data_size, UInt8 is_au_end, UInt32 full_au_size);

rs_status_t rtpfile_packetize_amr(session_stream_info_t *session_stream_info, track_stream_info_t *track_stream_info, char *data, UInt32 data_size, UInt8 is_au_end, UInt32 full_au_size);

rs_status_t rtpfile_packetize_qcelp(session_stream_info_t *session_stream_info, track_stream_info_t *track_stream_info, char *data, UInt32 data_size, UInt8 is_au_end, UInt32 full_au_size);

rs_status_t rtpfile_packetize_evrc_smv(session_stream_info_t *session_stream_info, track_stream_info_t *track_stream_info, char *data, UInt32 data_size, UInt8 is_au_end, UInt32 full_au_size);

#ifdef __cplusplus
}
#endif

#endif


