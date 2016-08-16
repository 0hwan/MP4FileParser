#ifndef SAMPLE_PCK_FUNC_H
#define SAMPLE_PCK_FUNC_H

#ifdef __cplusplus
extern "C" {
#endif


#include "rtp_streamer.h"


rs_status_t rtpfile_get_computed_dts(track_stream_info_t *track_stream_info, isom_box_trak_t *box_trak, UInt64 dts, UInt32 rap_find_t, UInt64 *rap_dts, UInt32 *sample_number);

rs_status_t rtpfile_get_access_unit_info(session_stream_info_t *session_stream_info, track_stream_info_t *track_stream_info, UInt32 sampleNumber, access_unit_info_t *samp, UInt32 max_size);

rs_status_t rtpfile_get_sample_dts_from_stts(track_stream_info_t *track_stream_info, isom_box_stts_t *stts, UInt32 sample_number, UInt64 *dts);

rs_status_t rtpfile_get_sample_cts_from_ctts(track_stream_info_t *track_stream_info, isom_box_ctts_t *ctts, UInt32 sample_number, UInt32 *cts_offset);

rs_status_t rtpfile_get_sample_size_from_stsz(track_stream_info_t *track_stream_info, isom_box_stsz_t *stsz, UInt32 sample_number, UInt32 *size);

rs_status_t rtpfile_get_sample_number_around_dts_from_stts(track_stream_info_t *track_stream_info, isom_box_stts_t *stts, UInt32 *sample_number, UInt64 dur);

rs_status_t rtpfile_get_sample_random_access_point_from_stss(track_stream_info_t *track_stream_info, isom_box_stss_t *stss, UInt32 sample_number, UInt8 *is_rap, UInt32 *prev_rap, UInt32 *next_rap);

Bool16 rtpfile_get_chunknumber_and_offset_from_stco(track_stream_info_t *track_stream_info, isom_box_trak_t *box_trak, UInt32 sample_number, UInt64 *offset, UInt32 *chunk_number, UInt32 *desc_index, UInt8 *is_edited);

void rtpfile_get_ghost_num(track_stream_info_t *track_stream_info, isom_box_stco_t *stco, isom_box_stsc_t *stsc, UInt32 entry_index, UInt32 count, UInt32 first_chunk);

UInt32 rtpfile_get_sample_duration(track_stream_info_t *track_stream_info, isom_box_trak_t *box_trak, UInt32 sample_number);

rs_status_t rtpfile_init_access_unit_info(track_stream_info_t *track_stream_info);

rs_status_t rtpfile_set_access_unit_info(session_stream_info_t *session_stream_info, track_stream_info_t *track_stream_info, UInt32 sample_number, access_unit_info_t **samp, UInt32 *s_idx, Bool16 no_data, UInt64 *out_offset);

Bool16 rtpfile_set_rtp_payload_data(session_stream_info_t *session_stream_info, track_stream_info_t *track_stream_info, char *data, UInt32 data_size, Bool16 is_head);

Bool16 rtpfile_set_rtp_packet_send_info(session_stream_info_t *session_stream_info, track_stream_info_t *track_stream_info, rtp_header_info_t *rtp_header_info);

#ifdef __cplusplus
}
#endif

#endif 