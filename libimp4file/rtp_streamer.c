#include "rtp_streamer.h"
#include "sample_description.h"
#include "packetizer_3gpp.h"
#include "packetizer_mp4_avc.h"

/*
* Desc : 
*/
rs_status_t ts_init_mp4fileinfo(const char *content_path, isom_file_info_t **mp4file_info, isom_resource_t *data_resource)
{
    rs_status_t       result_code = ISOM_UNKNOWN;
    isom_file_info_t *tmp_mp4file_info = NULL;

    // check invalid parameter
    if (content_path == NULL || data_resource == NULL) {
        return ISOM_BAD_PARAM;
    }

    //isomedia file parsing
    if ((result_code = isom_init_file_info(content_path, &tmp_mp4file_info, data_resource)) != ISOM_OK) {
        return result_code;
    }
    *mp4file_info = tmp_mp4file_info;
    return ISOM_OK;
}

/*
* Desc : 
*/
rs_status_t rtpfile_init_session(void **new_session_stream_info,
                                   UInt32 mtu_size,
                                   UInt8 conf_payload_type,
                                   isom_file_info_t *mp4file_info)
{
    rs_status_t            result_code = ISOM_UNKNOWN;
    session_stream_info_t *session_stream_info = NULL;

    *new_session_stream_info = NULL;

    // check invalid parameter
    if (mtu_size <= 0 || conf_payload_type == 0 || mp4file_info == NULL) {
        return ISOM_BAD_PARAM;
    }

    session_stream_info = (session_stream_info_t *) malloc(sizeof(session_stream_info_t));
    memset(session_stream_info, 0x00, sizeof(session_stream_info_t));    

    //mp4file_info->data_resource = NULL;

    session_stream_info->mtu_size               = mtu_size;
    session_stream_info->conf_payload_type      = conf_payload_type;
    //session_stream_info->content_path           = strdup(content_path);
    session_stream_info->mp4file_info           = mp4file_info;
    session_stream_info->content_msec_duration  = (UInt32)((mp4file_info->box_moov->box_mvhd->duration * 1000)/mp4file_info->box_moov->box_mvhd->timescale);
    //session_stream_info->stream_data_resource   = data_resource;
    
    if ((result_code = rtpfile_init_packet_send_info(session_stream_info, session_stream_info->mtu_size)) != ISOM_OK) {
        rtpfile_delete_session(session_stream_info);
        return result_code;
    }
    if ((result_code = rtpfile_set_track_stream_info(session_stream_info)) != ISOM_OK) {
        rtpfile_delete_session(session_stream_info);
        return result_code;
    }
    session_stream_info->result_code = ISOM_OK;
  	*new_session_stream_info = session_stream_info;

    return ISOM_OK;
}

/*
* Desc : 
*/
SInt32 rtpfile_delete_session(session_stream_info_t *session_stream_info)
{
    if (session_stream_info) {
        /*
        if (session_stream_info->content_path){
            free(session_stream_info->content_path);
            session_stream_info->content_path = NULL;
        }
        */
        if (session_stream_info->output_sdp){
            free(session_stream_info->output_sdp);
            session_stream_info->output_sdp = NULL;
        }
        if (session_stream_info->rtp_packet_send_info) {
            if (session_stream_info->rtp_packet_send_info->rtp_packet_data) {
                free(session_stream_info->rtp_packet_send_info->rtp_packet_data);
                session_stream_info->rtp_packet_send_info->rtp_packet_data = NULL;
            }
            free(session_stream_info->rtp_packet_send_info);
            session_stream_info->rtp_packet_send_info = NULL;
        }

        //isom_delete_resource(session_stream_info->stream_data_resource);

        rtpfile_delete_track_stream_info(session_stream_info);
        
        if (session_stream_info->mp4file_info) {
            isom_delete_file_info(session_stream_info->mp4file_info);
        }
        free(session_stream_info);
        session_stream_info = NULL;
    }
    return ISOM_OK;
}

/*
* Desc : 
*/
char* rtpfile_get_content_path(session_stream_info_t *session_stream_info)
{
    if (!session_stream_info) {
        session_stream_info->result_code = ISOM_BAD_PARAM;
        return NULL;
    }

    if(!session_stream_info->mp4file_info) {
        session_stream_info->result_code = ISOM_BAD_PARAM;
        return NULL;
    }

    return session_stream_info->mp4file_info->content_path;
}

/*
* Desc : 
*/
SInt32 rtpfile_get_result_code(session_stream_info_t *session_stream_info)
{
    if (!session_stream_info) {
        session_stream_info->result_code = ISOM_BAD_PARAM;
    }
    return session_stream_info->result_code;
}

/*
* Desc : 
*/
rs_status_t rtpfile_set_rtp_port(session_stream_info_t *session_stream_info, UInt32 rtp_port)
{
    if (!session_stream_info) {
        session_stream_info->result_code = ISOM_BAD_PARAM;
        return ISOM_ERROR;
    }
    session_stream_info->rtp_port = rtp_port;
    session_stream_info->result_code = ISOM_OK;
    return ISOM_OK;
}

/*
* Desc : 
*/
char* rtpfile_get_session_sdp(session_stream_info_t *session_stream_info)
{
    if (!session_stream_info) {
        session_stream_info->result_code = ISOM_BAD_PARAM;
        return NULL;
    }

    if (session_stream_info->output_sdp) {
        session_stream_info->result_code = ISOM_OK;
        return session_stream_info->output_sdp;
    }
	return NULL;
}

/*
* Desc : 
*/
rs_status_t rtpfile_set_track_cookies(session_stream_info_t *session_stream_info, UInt32 track_id, void *cookies)
{
    rs_status_t          result_code = ISOM_UNKNOWN;
    track_stream_info_t *track_stream_info   = NULL;

    if (!session_stream_info) {
        session_stream_info->result_code = ISOM_BAD_PARAM;
        return ISOM_ERROR;
    }

    if ((result_code = rtpfile_find_track_stream_info(session_stream_info, track_id, &track_stream_info)) == ISOM_OK) {
        track_stream_info->reserved = cookies;
	}

    session_stream_info->result_code = result_code;
	return result_code;
}

/*
* Desc : 
*/
SInt32 rtpfile_get_track_cookies(session_stream_info_t *session_stream_info, UInt32 track_id, void **cookies)
{
    rs_status_t          result_code = ISOM_UNKNOWN;
    track_stream_info_t *track_stream_info   = NULL;

    if (!session_stream_info) {
        session_stream_info->result_code = ISOM_BAD_PARAM;
        return ISOM_ERROR;
    }

    if ((result_code = rtpfile_find_track_stream_info(session_stream_info, track_id, &track_stream_info)) == ISOM_OK) {
        *cookies = track_stream_info->reserved;
	}

    session_stream_info->result_code = result_code;
	return result_code;
}

/*
* Desc : seek지점의 duration에 맞는 access unit 정보를 얻는다.  video인경우는 i-frame을 찾는다. 
*/
rs_status_t rtpfile_set_session_stream_info_by_seeking(session_stream_info_t *session_stream_info, double duration, UInt32 rap_find_t)
{
    rs_status_t result_code = ISOM_UNKNOWN;
	UInt64      u64duration, rap_dts;	
	UInt32      sample_number = 1;
	UInt32      visual_trackid = 0;

	track_stream_info_t *track_stream_info = NULL;
    track_packetize_info_t *track_packetize_info;
    
	UInt32      *key;
    size_t       len;
    
    u64duration = (UInt64)(duration * 1000);

//	memset(session_stream_info->rtp_packet_send_info->send_buffer, 0x00, session_stream_info->mtu_size);
	session_stream_info->rtp_packet_send_info->rtp_packet_size = 0;
	session_stream_info->rtp_packet_send_info->DTS             = 0;
	session_stream_info->rtp_packet_send_info->has_packet      = 0;
	
    //VISUAL
    track_stream_info = NULL;    
    while (rtpfile_next_track_stream_info(session_stream_info, &track_stream_info, track_stream_info) == ISOM_OK) {
		key = NULL;
		len = 0;

		if (track_stream_info->is_packetizing == 0) {
			continue;
		}
        track_packetize_info = track_stream_info->track_packetize_info;
	
		memset(track_stream_info->au_info->data, 0x00, track_stream_info->track_decode_info->au_max_size);
		track_stream_info->au_info->cts_offset  = 0;
		track_stream_info->au_info->data_length = 0;
		track_stream_info->au_info->DTS         = 0;
		track_stream_info->au_info->is_RAP      = 0;

/*      
        // rtsp module에서 rtp 패킷 전송의 시점이 rtsp 이벤트를 기다리는 시점보다 앞당겨져 아래 로직은 필요 없음. 
		if (session_stream_info->rtp_packet_send_info->track_id == track_stream_info->box_trak->box_tkhd->track_id) {
			track_stream_info->rtp_packet_info.rtp_header_info.sequence_number -= 1;//마지막 저장된 패킷 전송 이전이므로 
		}
*/
		track_stream_info->track_decode_info->rtp_sl_header.access_unit_start_flag = 1; // sample parsing start
		track_stream_info->track_decode_info->rtp_sl_header.access_unit_end_flag   = 0; // sample parsing end
		track_stream_info->track_packetize_info->bytes_in_rtp_payload              = 0; // payload의 byte
		track_stream_info->track_packetize_info->readbytes_leftsize_in_au          = 0; // sample에서 읽어들이고 남은 byte
		track_stream_info->track_decode_info->rtp_timestamp	                       = 0; // timestamp 
		track_stream_info->track_packetize_info->au_count_in_packet                = 0; 
		track_stream_info->current_au_number                                       = 0;
		track_stream_info->rtp_packet_info.rtp_payload_size                        = 0;
		memset(track_stream_info->rtp_packet_info.rtp_payload_data, 0x00, session_stream_info->mtu_size-50);
		
		if (track_stream_info->track_packetize_info->bo_payload_data) {
			isom_delete_bit_operation(track_stream_info->track_packetize_info->bo_payload_data);
			track_stream_info->track_packetize_info->bo_payload_data = NULL;
		}
		
		if (track_stream_info->track_packetize_info->bo_payload_header) {
			isom_delete_bit_operation(track_stream_info->track_packetize_info->bo_payload_header);
			track_stream_info->track_packetize_info->bo_payload_header = NULL;
		}
        
		if (ISOM_TRACK_VISUAL == track_stream_info->box_trak->box_mdia->box_hdlr->handler_type) {
			visual_trackid = track_stream_info->box_trak->box_tkhd->track_id;

			u64duration = (u64duration * track_stream_info->box_trak->box_mdia->box_mdhd->timescale) / 1000;
			result_code = rtpfile_get_computed_dts(track_stream_info, track_stream_info->box_trak, u64duration, rap_find_t, &rap_dts, &sample_number);
			
			track_stream_info->track_decode_info->rtp_sl_header.composition_timestamp = (UInt64)((SInt64)rap_dts * track_stream_info->ts_scale);
            
            if (sample_number == 0) {
				session_stream_info->rtp_packet_send_info->DTS = (u64duration * 1000) / 
                                        track_stream_info->box_trak->box_mdia->box_mdhd->timescale;
            }
            else {
				session_stream_info->rtp_packet_send_info->DTS = (rap_dts * 1000) / 
                                        track_stream_info->box_trak->box_mdia->box_mdhd->timescale;
            }
			
			track_stream_info->current_au_number = sample_number - 1;
		}
	}

    //AUDIO
    track_stream_info = NULL;    
    while (rtpfile_next_track_stream_info(session_stream_info, &track_stream_info, track_stream_info) == ISOM_OK) {
		key = NULL;
		len = 0;
		
        if (track_stream_info->is_packetizing == 0) {
            continue;
		}

		if (visual_trackid != track_stream_info->box_trak->box_tkhd->track_id) {
            if (visual_trackid) {
				u64duration = (session_stream_info->rtp_packet_send_info->DTS * track_stream_info->box_trak->box_mdia->box_mdhd->timescale) / 1000;
            }
            else { 
				u64duration = (u64duration * track_stream_info->box_trak->box_mdia->box_mdhd->timescale) / 1000;
            }
		
			result_code = rtpfile_get_computed_dts(track_stream_info, track_stream_info->box_trak, u64duration, 0, &rap_dts, &sample_number);	

			track_stream_info->track_decode_info->rtp_sl_header.composition_timestamp = (UInt64)((SInt64)rap_dts * track_stream_info->ts_scale);
            
            if (sample_number == 0) {
				u64duration = (u64duration * 1000) / track_stream_info->box_trak->box_mdia->box_mdhd->timescale;
            }
            else {
				u64duration = (rap_dts * 1000) / track_stream_info->box_trak->box_mdia->box_mdhd->timescale;
            }

            if (visual_trackid == 0) {
				session_stream_info->rtp_packet_send_info->DTS = u64duration;
            }

			track_stream_info->current_au_number = sample_number - 1;
		}
	}

    session_stream_info->result_code = ISOM_OK;
	return ISOM_OK;
}

/*
* Desc : 
*/
double rtpfile_get_first_packet_transmit_time(session_stream_info_t *session_stream_info)
{
	double dts = 0;

    if (!session_stream_info) {
        session_stream_info->result_code = ISOM_BAD_PARAM;
        return 0;
    }

    
    dts = (double)((UInt32)session_stream_info->rtp_packet_send_info->DTS)/1000;
    session_stream_info->result_code = ISOM_OK;


	return dts; 
}

/*
* Desc : 
*/
UInt32 rtpfile_get_timestamp(session_stream_info_t *session_stream_info, UInt32 track_id)
{
    rs_status_t            result_code = ISOM_UNKNOWN;
    UInt32 timestamp = 0;
    track_stream_info_t *track_stream_info   = NULL;

    if (!session_stream_info) {
        session_stream_info->result_code = ISOM_BAD_PARAM;
        return 0;
    }

    if ((result_code = rtpfile_find_track_stream_info(session_stream_info, track_id, &track_stream_info)) == ISOM_OK) {
        timestamp = (UInt32)track_stream_info->track_decode_info->rtp_sl_header.composition_timestamp;
        if (timestamp > 0) {
            session_stream_info->result_code = ISOM_OK;
            if (track_stream_info->timestamp_random_offset != 0) {
                timestamp += (UInt32)track_stream_info->timestamp_random_offset;
            }
            return timestamp;
        }
        else {
            session_stream_info->result_code = result_code;
        }
    }
    return 0;
}

/*
* Desc : 
*/
UInt16 rtpfile_get_next_sequence_number(session_stream_info_t *session_stream_info, UInt32 track_id)
{
    rs_status_t            result_code = ISOM_UNKNOWN;
    UInt16 next_sequence_number = 0;
    track_stream_info_t *track_stream_info   = NULL;

    if (!session_stream_info) {
        session_stream_info->result_code = ISOM_BAD_PARAM;
        return 0;
    }

    if ((result_code = rtpfile_find_track_stream_info(session_stream_info, track_id, &track_stream_info)) == ISOM_OK) {
        next_sequence_number = track_stream_info->rtp_packet_info.rtp_header_info.sequence_number + 1;
        if (next_sequence_number > 0) {
            session_stream_info->result_code = ISOM_OK;
            return next_sequence_number;
        }
        else {
            session_stream_info->result_code = result_code;
        }
    }
    return 0;
}

/*
* Desc : 
*/
UInt32 rtpfile_get_timescale(session_stream_info_t *session_stream_info, UInt32 track_id)
{
    rs_status_t            result_code = ISOM_UNKNOWN;
    UInt32 timestamp_resolution = 0;
    track_stream_info_t *track_stream_info   = NULL;

    if (!session_stream_info) {
        session_stream_info->result_code = ISOM_BAD_PARAM;
        return -1;
    }

    if ((result_code = rtpfile_find_track_stream_info(session_stream_info, track_id, &track_stream_info)) == ISOM_OK) {
        timestamp_resolution = track_stream_info->track_decode_info->sl_config_descriptor.timestamp_resolution;
        if (timestamp_resolution > 0) {
            session_stream_info->result_code = ISOM_OK;
            return timestamp_resolution;
        }
        else {
            session_stream_info->result_code = result_code;
        }
    }
    return 0;
}

/*
* Desc : 
*/
rs_status_t rtpfile_get_rtp_packet(session_stream_info_t *session_stream_info)
{
    rs_status_t           result_code = ISOM_UNKNOWN;
    track_stream_info_t   *track_stream_info, *send_track_steam_info;
    UInt64      	  min_ts;
    UInt32           *key;
    size_t            len;
    
    /*browse all sessions and locate most mature stream*/
    send_track_steam_info = NULL;
    min_ts	   = (UInt64) -1;	

    track_stream_info = NULL;
    while (rtpfile_next_track_stream_info(session_stream_info, &track_stream_info, track_stream_info) == ISOM_OK) {
        key = NULL;
        len = 0;
        if (!track_stream_info->is_packetizing) {
            continue;
        }

        if (track_stream_info->au_info->data_length == 0) {// /*load next AU*/
            if (track_stream_info->current_au_number >= track_stream_info->track_decode_info->total_au_count) {//track_stream_info->nb_aus total packet count
                continue;
            }
            
            if ((result_code = rtpfile_get_access_unit_info(session_stream_info, track_stream_info, track_stream_info->current_au_number + 1, track_stream_info->au_info, track_stream_info->track_decode_info->au_max_size)) != ISOM_OK) {
                session_stream_info->result_code =	result_code;
                return result_code;
            }
            
            track_stream_info->current_au_number ++;// current unit
            if (track_stream_info->au_info->data_length) {					
                UInt64 ts;
//              track_stream_info->sample_duration = rtpfile_get_sample_duration(track_stream_info->track, track_stream_info->current_au_number); // sample 하나의 duration
//              track_stream_info->sample_duration = (UInt32)(track_stream_info->sample_duration*track_stream_info->timescale);//timescale = track_stream_info->packetizer->sl_config_descriptor.timestampResolution / media_timescale 
                
                ts = (UInt64)(track_stream_info->ts_scale * (SInt64)(track_stream_info->au_info->DTS)); 
                track_stream_info->track_decode_info->rtp_sl_header.decoding_timestamp = ts;

                track_stream_info->microsec_dts = ((UInt64)(track_stream_info->microsec_ts_scale*1000) * track_stream_info->au_info->DTS);
                
                ts = (UInt64)(track_stream_info->ts_scale * (SInt64) (track_stream_info->au_info->DTS + track_stream_info->au_info->cts_offset));
                track_stream_info->track_decode_info->rtp_sl_header.composition_timestamp = ts + track_stream_info->timestamp_random_offset;// timestamp_random_offset random timestamp 적용 
                
                track_stream_info->track_decode_info->rtp_sl_header.is_RAP = track_stream_info->au_info->is_RAP; // i-frame
            }
        }
        
        /*check timing*/ 
        if (track_stream_info->au_info->data_length) {
            if (min_ts > track_stream_info->microsec_dts) {
                min_ts = track_stream_info->microsec_dts;
                send_track_steam_info = track_stream_info;
            }
        }
    } // while (track_stream_info)
    
    if (!send_track_steam_info) {	/*no input data ...*/
        UInt32 au_over_check = 0;
        track_stream_info = NULL;
        while (rtpfile_next_track_stream_info(session_stream_info, &track_stream_info, track_stream_info) == ISOM_OK) {
            if (track_stream_info->current_au_number >= track_stream_info->track_decode_info->total_au_count) {//track_stream_info->nb_aus total packet count
                au_over_check++;
            }
        }

        if (au_over_check == session_stream_info->has_stream_info_track_count) {
            session_stream_info->result_code = ISOM_EOS;
            return ISOM_EOS;
        }
        session_stream_info->result_code = ISOM_ERROR;
		return ISOM_ERROR;
    }

    /*packetizer*/
    if ((result_code = rtpfile_process_rtp_packetizing(session_stream_info, 
                                                       send_track_steam_info, 
                                                       send_track_steam_info->au_info->data, 
                                                       send_track_steam_info->au_info->data_length,
                                                       (UInt8) 1,
                                                       send_track_steam_info->au_info->data_length, 
                                                       send_track_steam_info->au_duration)) != ISOM_OK) {

        session_stream_info->result_code = result_code;
		return result_code;
    }
    
    /*delete sample*/
    if (send_track_steam_info->track_packetize_info->readbytes_leftsize_in_au == 0) {
        send_track_steam_info->au_info->cts_offset = 0;
        send_track_steam_info->au_info->data_length = 0;
        send_track_steam_info->au_info->DTS = 0;
        send_track_steam_info->au_info->is_RAP = 0;
    }

    session_stream_info->result_code = ISOM_OK;
    return ISOM_OK;
}

/*
* Desc : 
*/
rs_status_t rtpfile_set_drop_repeat_packets(session_stream_info_t *session_stream_info, UInt16 allow_repeat_packets)
{
    rs_status_t result_code = ISOM_UNKNOWN;
    if (!session_stream_info) {
        session_stream_info->result_code = ISOM_BAD_PARAM;
        return ISOM_ERROR;
    }

	if (allow_repeat_packets) {
		do {
			if ((result_code = rtpfile_get_rtp_packet(session_stream_info)) != ISOM_OK) {	
                session_stream_info->result_code = result_code;
				return result_code;
			}
		} while (session_stream_info->rtp_packet_send_info->has_packet != 1);
	}
    session_stream_info->result_code = ISOM_OK;
	return ISOM_OK;
}

/*
* Desc : 
*/
double rtpfile_get_next_packet(session_stream_info_t *session_stream_info, char **packet, UInt32 *packet_length)
{
    rs_status_t result_code = ISOM_UNKNOWN;
    if (!session_stream_info || !session_stream_info->rtp_packet_send_info) {
        *packet = NULL;
        *packet_length = 0;
        session_stream_info->result_code = ISOM_BAD_PARAM;
        return 0;
    }
    
    do {
        if ((result_code = rtpfile_get_rtp_packet(session_stream_info)) != ISOM_OK) {
            *packet = NULL;
            *packet_length = 0;		
            session_stream_info->rtp_packet_send_info->has_packet = 0;
            session_stream_info->result_code = result_code;
            return (double)((UInt32)session_stream_info->rtp_packet_send_info->DTS) / 1000;
        }
        
    } while(session_stream_info->rtp_packet_send_info->has_packet != 1);  
    
    /* out track_stream_info packet */
    *packet		   = session_stream_info->rtp_packet_send_info->rtp_packet_data;
    *packet_length = session_stream_info->rtp_packet_send_info->rtp_packet_size;
    session_stream_info->rtp_packet_send_info->has_packet = 0;
    session_stream_info->result_code = ISOM_OK;

    return (double)((UInt32)session_stream_info->rtp_packet_send_info->DTS)/1000;
}

/*
* Desc : 가장 최근 생성한 rtp packet의 track id
*/
UInt32 rtpfile_get_last_packet_track_id(session_stream_info_t *session_stream_info)
{
    if (!session_stream_info) {
        session_stream_info->result_code = ISOM_BAD_PARAM;
        return 0;
    }

    if (session_stream_info->rtp_packet_send_info->track_id > 0) {
        session_stream_info->result_code = ISOM_OK;
        return session_stream_info->rtp_packet_send_info->track_id;
    }
    else {
        session_stream_info->result_code = ISOM_ERROR;
    }
    return 0;
}

/*
* Desc : millisecond (mvhd duration / mvhd timescal)
*/
UInt32 rtpfile_get_session_mili_duration(session_stream_info_t *session_stream_info)
{
    if (!session_stream_info) {
        session_stream_info->result_code = ISOM_BAD_PARAM;
        return 0;
    }

    if (session_stream_info->content_msec_duration > 0) {
        session_stream_info->result_code = ISOM_OK;
        return session_stream_info->content_msec_duration;
    }
    else {
        session_stream_info->result_code = ISOM_ERROR;
    }
    return 0;
}

/*
* Desc :
*/
UInt32 rtpfile_get_track_count(session_stream_info_t *session_stream_info)
{
    if (!session_stream_info) {
        session_stream_info->result_code = ISOM_BAD_PARAM;
        return 0;
    }

    if(session_stream_info->has_stream_info_track_count > 0) {
        session_stream_info->result_code = ISOM_OK;
        return session_stream_info->has_stream_info_track_count;
    }
    else {
        session_stream_info->result_code = ISOM_ERROR;
    }

    return 0;
}

/*
* Desc :
*/

// ?????????????????????????????????????????????????????????????
track_stream_info_t *rtpfile_get_track_stream_info(session_stream_info_t *session_stream_info, UInt32 track_id)
{
    rs_status_t               result_code = ISOM_UNKNOWN;
    track_stream_info_t *track_stream_info = NULL;

    if (!session_stream_info) {
        session_stream_info->result_code = ISOM_BAD_PARAM;
        return NULL;
    }

    result_code = rtpfile_find_track_stream_info(session_stream_info, track_id, &track_stream_info);
    session_stream_info->result_code = result_code;
    return track_stream_info;
}

/*
* Desc :
*/
UInt32 rtpfile_get_payload_type(track_stream_info_t *track_stream_info)
{
    return track_stream_info->rtp_packet_info.rtp_header_info.payload_type;
}

/*
* Desc :
*/
rs_status_t rtpfile_get_media_payload_name(track_stream_info_t *track_stream_info, char *sz_payload_name, char *sz_media_name)
{
    return rtpfile_get_payload_name(track_stream_info->track_decode_info, sz_payload_name, sz_media_name);
}

/*
* Desc :
*/
UInt32 rtpfile_get_iv_length(track_stream_info_t *track_stream_info)
{
    return track_stream_info->track_decode_info->rtp_mp4_es_generic_info.iv_length;
}

/*
* Desc :
*/
UInt32 rtpfile_get_bandwidth(track_stream_info_t *track_stream_info)
{
    double max_bitrate = 0;

    if (track_stream_info->track_decode_info->track_bitrate_info) {
        if (track_stream_info->track_decode_info->track_bitrate_info->max_bitrate) {
            max_bitrate = track_stream_info->track_decode_info->track_bitrate_info->max_bitrate / 1000;
            max_bitrate += 0.5;
            return (UInt32)max_bitrate;
        }
    }

    return track_stream_info->track_decode_info->bandwidth;
}

/*
* Desc :
*/
UInt32 rtpfile_get_timestamp_resolution(track_stream_info_t *track_stream_info)
{
    return track_stream_info->track_decode_info->sl_config_descriptor.timestamp_resolution;
}

/*
* Desc :
*/
UInt32 rtpfile_get_trackid(track_stream_info_t *track_stream_info)
{
    return track_stream_info->box_trak->box_tkhd->track_id;
}

/*
* Desc :
*/
UInt32 rtpfile_get_media_handler_type(track_stream_info_t *track_stream_info)
{
    return track_stream_info->box_trak->box_mdia->box_hdlr->handler_type;
}

/*
* Desc :
*/
SInt32 rtpfile_get_visual_info_w_h(track_stream_info_t *track_stream_info, UInt32 *width, UInt32 *height)
{
    return rtpfile_get_visual_info(track_stream_info->box_trak, 1, width, height);
}

/*
* Desc :
*/
UInt32 rtpfile_get_rfc_codec_type(track_stream_info_t *track_stream_info)
{
    return track_stream_info->track_decode_info->rfc_codec_type;
}

/*
* Desc :
*/
UInt32 rtpfile_get_auh_size(track_stream_info_t *track_stream_info)
{
    return track_stream_info->track_decode_info->auh_size;
}

/*
* Desc :
*/
isom_avc_config_t* rtpfile_get_avc_config_default(track_stream_info_t *track_stream_info)
{
    return rtpfile_get_avc_config(track_stream_info->box_trak, LIST_AVCC_INDEX);
}

/*
* Desc :
*/
SInt32 rtpfile_get_avc_profile_level_id(isom_avc_config_t *avcc, char *sdp_plid)
{
    sprintf(sdp_plid, "%02X%02X%02X", avcc->avc_profile_indication, avcc->profile_compatibility, avcc->avc_level_indication);
    return ISOM_OK;
}

/*
* Desc :
*/
rs_status_t rtpfile_get_avc_sps_rfc3984(isom_avc_config_t *avcc, char *sdp_sps)
{
    UInt32 i, count, b64s;
    char b64[200];
    count = isom_get_list_count(avcc->sequence_parameter_sets);
    memset(sdp_sps, 0x00, sizeof(sdp_sps));
    if (count) {
        for (i=0; i<count; i++) {
            isom_avc_config_slot_t *sl = (isom_avc_config_slot_t *)isom_get_list(avcc->sequence_parameter_sets, i);
            b64s = ut_encode_base64(sl->data, sl->size, b64, 200);
            b64[b64s]=0;
            strcat(sdp_sps, b64);
            if (i + 1 < count) {
                strcat(sdp_sps, ",");
            }
        }
    }
    else {
        return ISOM_ERROR;
    }
    
    return ISOM_OK;
}

/*
* Desc :
*/
rs_status_t rtpfile_get_avc_pps_rfc3984(isom_avc_config_t *avcc, char *sdp_pps) {
    UInt32 i, count, b64s;
    char b64[200];
    count = isom_get_list_count(avcc->picture_parameter_sets);
    memset(sdp_pps, 0x00, sizeof(sdp_pps));
    if (count) {
        for (i=0; i<count; i++) {
            isom_avc_config_slot_t *sl = (isom_avc_config_slot_t *)isom_get_list(avcc->picture_parameter_sets, i);
            b64s = ut_encode_base64(sl->data, sl->size, b64, 200);
            b64[b64s]=0;
            strcat(sdp_pps, b64);
            if (i + 1 < count) {
                strcat(sdp_pps, ",");
            }
        }
    }
    else {
        return ISOM_ERROR;
    }
    
    return ISOM_OK;
}

/*
* Desc :
*/
rs_status_t rtpfile_get_avc_sps_custom_draft(isom_avc_config_t *avcc, char *sdp_sps)
{
    UInt16 i;
    UInt32 count, b64s;
    char tmp_ps[200], b64[200];
    count = isom_get_list_count(avcc->sequence_parameter_sets);
    memset(sdp_sps, 0x00, sizeof(sdp_sps));
    if (count) {
        for (i=0; i<count; i++) {
            isom_avc_config_slot_t *sl = (isom_avc_config_slot_t *)isom_get_list(avcc->sequence_parameter_sets, i);
            sprintf(tmp_ps, "%02X%04X", (i+1), sl->size);
            tmp_ps[14] = 0;
            strcat(sdp_sps, tmp_ps);

            b64s = ut_encode_base16(sl->data, sl->size, b64, 200);
            b64[b64s] = 0;
            strcat(sdp_sps, b64);
        }
    }
    else {
        return ISOM_ERROR;
    }
    
    return ISOM_OK;
}

/*
* Desc :
*/
rs_status_t rtpfile_get_avc_pps_custom_draft(isom_avc_config_t *avcc, char *sdp_pps)
{
    UInt16 i;
    UInt32 count, b64s;
    char tmp_ps[200], b64[200];
    count = isom_get_list_count(avcc->picture_parameter_sets);
    memset(sdp_pps, 0x00, sizeof(sdp_pps));
    if (count) {
        for (i=0; i<count; i++) {
            isom_avc_config_slot_t *sl = (isom_avc_config_slot_t *)isom_get_list(avcc->picture_parameter_sets, i);
            sprintf(tmp_ps, "%02X%04X", (i+1), sl->size);
            tmp_ps[14] = 0;
            strcat(sdp_pps, tmp_ps);
            b64s = ut_encode_base16(sl->data, sl->size, b64, 200);
            b64[b64s] = 0;
            strcat(sdp_pps, b64);
        }
    }
    else {
        return ISOM_ERROR;
    }
    
    return ISOM_OK;
}

/*
* Desc :
*/
UInt32 rtpfile_get_buffer_sizedb(track_stream_info_t *track_stream_info)
{
    if (track_stream_info->track_decode_info->track_bitrate_info) {
        return track_stream_info->track_decode_info->track_bitrate_info->buffer_sizedb;
    }
    return 0;
}

/*
* Desc :
*/
UInt32 rtpfile_get_avg_bitrate(track_stream_info_t *track_stream_info) {
    if (track_stream_info->track_decode_info->track_bitrate_info) {
        return track_stream_info->track_decode_info->track_bitrate_info->avg_bitrate;
    }
    return 0;
}

/*
* Desc :
*/
UInt32 rtpfile_get_max_bitrate(track_stream_info_t *track_stream_info)
{
    if (track_stream_info->track_decode_info->track_bitrate_info) {
        return track_stream_info->track_decode_info->track_bitrate_info->max_bitrate;
    }
    return 0;
}

/*
* Desc :
*/
rs_status_t rtpfile_delete_avc_config(isom_avc_config_t *avcc)
{
    isom_delete_odf_avc_config(avcc);
    return ISOM_OK;
}

/*
* Desc :
*/
UInt32 rtpfile_get_mp4_profile_level_id(track_stream_info_t *track_stream_info)
{
    if (track_stream_info->track_decode_info->rtp_mp4_es_generic_info.pl_id) {
        return track_stream_info->track_decode_info->rtp_mp4_es_generic_info.pl_id;
    }
    return 0;
}

/*
* Desc :
*/
rs_status_t rtpfile_get_mp4v_dsi_config(track_stream_info_t *track_stream_info, char *sz_config)
{
    char *dsi;
    UInt32 i, k;
    UInt32 dsi_size;
    isom_decoder_config_t *decoder_config;
    memset(sz_config, 0x00, sizeof(sz_config));

	decoder_config = rtpfile_get_decoder_config(track_stream_info->box_trak, 1);
    if (!decoder_config) {
        return ISOM_ERROR;
    }

    if (decoder_config && decoder_config->decoder_specific_info && decoder_config->decoder_specific_info->data) {
        dsi      = decoder_config->decoder_specific_info->data;
        dsi_size = decoder_config->decoder_specific_info->data_length;
        k = 0;
        for (i=0; i<dsi_size; i++) {
            sprintf(&sz_config[k], "%02x", (unsigned char) dsi[i]);
            k+=2;
        }
        sz_config[k] = 0;
        if (decoder_config) isom_delete_odf_descriptor((isom_descriptor_t *)decoder_config);
    }
    else {
        if (decoder_config) isom_delete_odf_descriptor((isom_descriptor_t *)decoder_config);
        return ISOM_ERROR;
    }

    return ISOM_OK;
}

/*
* Desc :
*/
rs_status_t rtpfile_get_mp4a_dsi_config(track_stream_info_t *track_stream_info, char *sz_config)
{
    char *config_bytes;
    UInt32 config_size; 
    UInt32 i, k;
    isom_decoder_config_t *decoder_config;
    isom_bit_operation_t *bo;
    memset(sz_config, 0x00, sizeof(sz_config));

    /* form config string */ 
    bo = isom_init_bit_operation(NULL, 32, ISOM_BITOPER_WRITE); 
    isom_write_bit_operation_int(bo, 0, 1); /* AudioMuxVersion */ 
    isom_write_bit_operation_int(bo, 1, 1); /* all streams same time */ 
    isom_write_bit_operation_int(bo, 0, 6); /* numSubFrames */ 
    isom_write_bit_operation_int(bo, 0, 4); /* numPrograms */ 
    isom_write_bit_operation_int(bo, 0, 3); /* numLayer */ 
    
    /* audio-specific config */ 
    decoder_config = rtpfile_get_decoder_config(track_stream_info->box_trak, 1); 
    if (decoder_config) { 
        isom_write_bit_operation_data(bo, decoder_config->decoder_specific_info->data, decoder_config->decoder_specific_info->data_length); 
        isom_delete_odf_descriptor((isom_descriptor_t *)decoder_config); 
    }
    else {
        return ISOM_ERROR;
    }
    
    /* other data */ 
    isom_write_bit_operation_int(bo, 0, 3); /* frameLengthType */ 
    isom_write_bit_operation_int(bo, 0xff, 8); /* latmBufferFullness */ 
    isom_write_bit_operation_int(bo, 0, 1); /* otherDataPresent */ 
    isom_write_bit_operation_int(bo, 0, 1); /* crcCheckPresent */ 
    isom_get_bit_operation_content(bo, &config_bytes, &config_size); 
	isom_delete_bit_operation(bo); 

    if (config_bytes && config_size) {
        k = 0;
        for (i=0; i<config_size; i++) {
            sprintf(&sz_config[k], "%02x", (unsigned char) config_bytes[i]);
            k+=2;
        }
        sz_config[k] = 0;
    }
    if (config_bytes) {
        free(config_bytes); 
    }

    return ISOM_OK;
}

/*
* Desc :
*/
UInt32 rtpfile_get_sdp_media_bandwidth(track_stream_info_t *track_stream_info)
{
    return track_stream_info->track_decode_info->bandwidth;
}

/*
* Desc :
*/
rs_status_t rtpfile_set_sdp(session_stream_info_t *session_stream_info, UInt32 avc_ver)
{
    rs_status_t result_code = ISOM_UNKNOWN;
    char        media_name[30], payload_name[30];
	char        sdp_val[512];
	char        sdp_temp[512];
	char        sdp[1024];  

    UInt32      cnt_track, rfc_codec_type;
    UInt32      w=0, h=0;
    UInt32      payload_type = 0;
    UInt32      add_rtp_port    = 0;
    UInt32      channel = 0;
    
    track_stream_info_t *track_stream_info;

    memset(sdp_val , 0x00,  sizeof(sdp_val));
    memset(sdp_temp , 0x00, sizeof(sdp_temp));    
    memset(sdp , 0x00,  sizeof(sdp));
    
    if (!session_stream_info) {
        result_code = ISOM_BAD_PARAM;
        session_stream_info->result_code = result_code;
        return result_code;
    }

    if ((add_rtp_port = rtpfile_get_session_mili_duration(session_stream_info)) > 0) {
        sprintf(sdp, "a=range:npt=0-%10.5f"CRLF_, (double)(add_rtp_port / 1000));
    }
 
    if ((cnt_track = rtpfile_get_track_count(session_stream_info)) == 0) {
        result_code = ISOM_INVALID_TRACK_STREAM_INFO;
        session_stream_info->result_code = result_code;
        return result_code;
    }
    
    add_rtp_port = 0;

    track_stream_info = NULL;
    while (rtpfile_next_track_stream_info(session_stream_info, &track_stream_info, track_stream_info) == ISOM_OK) {
        rfc_codec_type = rtpfile_get_rfc_codec_type(track_stream_info);
        payload_type = rtpfile_get_payload_type(track_stream_info);
        
        if ((result_code = rtpfile_get_media_payload_name(track_stream_info, payload_name, media_name)) == ISOM_OK) {
            sprintf(sdp_val, "m=%s %u RTP/%s %u"CRLF_, 
                              media_name, 
                              session_stream_info->rtp_port ? session_stream_info->rtp_port + add_rtp_port : 0, 
                              rtpfile_get_iv_length(track_stream_info) ? "SAVP" : "AVP", 
                              payload_type);
            add_rtp_port += 2;
            strcat(sdp, sdp_val);
        }
        else {
            session_stream_info->result_code = result_code;
            return result_code;
        }

        channel = track_stream_info->track_decode_info->nb_chan;
        if (channel > 1) {
            sprintf(sdp_val, "a=rtpmap:%u %s/%u/%u"CRLF_, payload_type, payload_name,
                rtpfile_get_timestamp_resolution(track_stream_info), (channel > 2) ? 2 : channel);
        }
        else {
            sprintf(sdp_val, "a=rtpmap:%u %s/%u"CRLF_, payload_type, payload_name,
                rtpfile_get_timestamp_resolution(track_stream_info));
        }
        
        strcat(sdp, sdp_val);
        
        sprintf(sdp_val, "a=control:trackID=%u"CRLF_, rtpfile_get_trackid(track_stream_info));
        strcat(sdp, sdp_val);
        
        if (rtpfile_get_media_handler_type(track_stream_info) == ISOM_TRACK_VISUAL) {
            w = h = 0;
            rtpfile_get_visual_info_w_h(track_stream_info, &w, &h);
            if (rfc_codec_type == RTP_PAYT_H263) {
                sprintf(sdp_val, "a=cliprect:0,0,%u,%u"CRLF_, h, w);
                strcat(sdp, sdp_val);
                /*extensions for some mobile phones*/
                sprintf(sdp_val, "a=framesize:%u %u-%u"CRLF_, payload_type, w, h);
                strcat(sdp, sdp_val);
            }
        }
        if (rfc_codec_type == RTP_PAYT_H264_AVC) {            
            isom_avc_config_t *avcc = rtpfile_get_avc_config_default(track_stream_info);
            rtpfile_get_avc_profile_level_id(avcc, sdp_temp);
            if (avc_ver == AVC_SDP_RFC_3984) {
                sprintf(sdp_val, "a=fmtp:%u profile-level-id=%s;packetization-mode=1", payload_type, sdp_temp);
                strcat(sdp_val, ";sprop-parameter-sets=");
                if (rtpfile_get_avc_sps_rfc3984(avcc, sdp_temp) == ISOM_OK) {
                    strcat(sdp_val, sdp_temp);
                    strcat(sdp_val, ",");
                }
                if (rtpfile_get_avc_pps_rfc3984(avcc, sdp_temp) == ISOM_OK) {
                    strcat(sdp_val, sdp_temp);
                }
            
                sprintf(sdp_temp, "%s"CRLF_, sdp_val);
                strcat(sdp, sdp_temp);

                if (avcc) {
                    rtpfile_delete_avc_config(avcc);
                }
            }
            else {
                sprintf(sdp_val, "a=fmtp:%u profile-level-id=%s;parameter-sets=", payload_type, sdp_temp);
                if (rtpfile_get_avc_sps_custom_draft(avcc, sdp_temp) == ISOM_OK) {
                    strcat(sdp_val, sdp_temp);
                }
                if (rtpfile_get_avc_pps_custom_draft(avcc, sdp_temp) == ISOM_OK) {
                    strcat(sdp_val, sdp_temp);
                }
                sprintf(sdp_temp, ";packetization-mode=1;framesize:%u-%u"CRLF_, w, h);
                strcat(sdp_val, sdp_temp);
                strcat(sdp, sdp_val);
                if (avcc) {
                    rtpfile_delete_avc_config(avcc);
                }
            }
        }
        else if (rfc_codec_type==RTP_PAYT_MPEG4) {
            sprintf(sdp_val, "a=fmtp:%u profile-level-id=%u;framesize=%u-%u;config=", payload_type, rtpfile_get_mp4_profile_level_id(track_stream_info), w, h);
            if ((rtpfile_get_mp4v_dsi_config(track_stream_info, sdp_temp)) == ISOM_OK && sdp_temp) {
                strcat(sdp_val, sdp_temp);
                sprintf(sdp_temp, "%s"CRLF_, sdp_val);
                strcat(sdp, sdp_temp);
            }
            else {
                session_stream_info->result_code = ISOM_INVALID_MP4V_DSI_CONFIG;
                return ISOM_INVALID_MP4V_DSI_CONFIG;
            }
        }
        /*AMR*/
        if ((track_stream_info->track_decode_info->rfc_codec_type == RTP_PAYT_AMR) || (track_stream_info->track_decode_info->rfc_codec_type == RTP_PAYT_AMR_WB)) {
            sprintf(sdp_val, "a=fmtp:%u octet-align"CRLF_, track_stream_info->rtp_packet_info.rtp_header_info.payload_type);
            strcat(sdp, sdp_val);
        }
        /*EVRC/SMV in non header-free mode*/
        else if ((track_stream_info->track_decode_info->rfc_codec_type == RTP_PAYT_EVRC_SMV) && (track_stream_info->track_decode_info->auh_size>1)) {
            sprintf(sdp_val, "a=fmtp:%u maxptime=%u"CRLF_, track_stream_info->rtp_packet_info.rtp_header_info.payload_type, track_stream_info->track_decode_info->auh_size*20);
            strcat(sdp, sdp_val);
		}
        else if (rfc_codec_type == RTP_PAYT_LATM) { 
            sprintf(sdp_val, "a=fmtp:%u profile-level-id=%u;object=2;bitrate=%u;cpresent=0;config=", payload_type, rtpfile_get_mp4_profile_level_id(track_stream_info), rtpfile_get_sdp_media_bandwidth(track_stream_info)*1000);
            if ((rtpfile_get_mp4a_dsi_config(track_stream_info, sdp_temp)) == ISOM_OK && sdp_temp) {
                strcat(sdp_val, sdp_temp);
                sprintf(sdp_temp, "%s"CRLF_, sdp_val);
                strcat(sdp, sdp_temp);
            }
            else {
                session_stream_info->result_code = ISOM_INVALID_MP4A_DSI_CONFIG;
                return ISOM_INVALID_MP4A_DSI_CONFIG;
            }
        }
        sprintf(sdp_val, "b=AS:%u"CRLF_, rtpfile_get_bandwidth(track_stream_info));
        strcat(sdp, sdp_val);
    }
    session_stream_info->output_sdp = strdup(sdp);
    session_stream_info->result_code = ISOM_OK;
    return ISOM_OK;
}

/*
* Desc :
*/
rs_status_t rtpfile_init_packet_send_info(session_stream_info_t *session_stream_info, UInt32 path_mtu)
{
    rs_status_t             result_code = ISOM_UNKNOWN;
    rtp_packet_send_info_t *rtp_packet_send_info = NULL;
    
    if (!session_stream_info || path_mtu == 0){
        result_code = ISOM_BAD_PARAM;
        session_stream_info->result_code = result_code;
        return ISOM_BAD_PARAM;
    }   
    
    rtp_packet_send_info = (rtp_packet_send_info_t *) malloc(sizeof(rtp_packet_send_info_t));
    if (rtp_packet_send_info == NULL) {
        result_code = ISOM_MEMORY_ERROR;
        session_stream_info->result_code = result_code;
        return result_code;
    }
    memset(rtp_packet_send_info, 0x00, sizeof(rtp_packet_send_info_t)); 

    rtp_packet_send_info->rtp_packet_data = (char *) malloc(path_mtu);
    if (rtp_packet_send_info->rtp_packet_data == NULL) {
        result_code = ISOM_MEMORY_ERROR;
        session_stream_info->result_code = result_code;
        return result_code;
    }
     
    rtp_packet_send_info->rtp_packet_size = path_mtu;
    rtp_packet_send_info->DTS             = 0;
    rtp_packet_send_info->has_packet      = 0;
    rtp_packet_send_info->track_id		  = 0;
    
    session_stream_info->rtp_packet_send_info = rtp_packet_send_info;
    session_stream_info->result_code = ISOM_OK;

    return ISOM_OK;
}

/*
* Desc : track의 sequence_number, time_stamp 시작값을 random 으로 사용할지  결정, 
          is_packetizing 를 1로 설정
*/
rs_status_t rtpfile_add_track_stream_info(session_stream_info_t *session_stream_info, UInt32 track_id, UInt32 b_rand)
{
    rs_status_t          result_code = ISOM_UNKNOWN;
    track_stream_info_t *track_stream_info   = NULL;
    UInt16 sq_rand = 0,  ut_rand = 0;
    
    if (!session_stream_info) {
        result_code = ISOM_BAD_PARAM;
        session_stream_info->result_code = result_code;
        return ISOM_BAD_PARAM;
    }

    if ((result_code = rtpfile_find_track_stream_info(session_stream_info, track_id, &track_stream_info)) == ISOM_OK) {
        if (b_rand) {
            sq_rand = rand();
            track_stream_info->rtp_packet_info.rtp_header_info.sequence_number = sq_rand;
            ut_rand = rand();
            track_stream_info->timestamp_random_offset = ut_rand;
        }
        track_stream_info->is_packetizing = 1;
    }
    
    session_stream_info->result_code = result_code;
    return result_code;
}


/*
* Desc : video, audio 트랙에 대해 track_stream_info_t setting
*/
rs_status_t rtpfile_set_track_stream_info(session_stream_info_t *session_stream_info)
{
    rs_status_t result_code = ISOM_UNKNOWN;
    UInt32      decode_option = 0;
    UInt32      usable_mtu_payload_size, track_media_type;
    
    Bool16 mpeg4_video_generic_flag = 0;            // mpeg4_video_generic use: 1, none_use: 0
    Bool16 mpeg4_audio_generic_flag = 0;            // mpeg4_audio_generic use: 1, none_use: 0    
    Bool16 audio_multi_packet_flag  = 0;            // audio_multi_packet use: 1, none_use: 0

    isom_box_trak_t       *box_trak = NULL;

    while (isom_next_trak_box(session_stream_info->mp4file_info, &box_trak, box_trak) == ISOM_OK) {
        track_stream_info_t         *track_stream_info = NULL;
        track_stream_info_entry_t   *track_stream_info_entry = NULL;
        track_sample_control_info_t *track_sample_control_info = NULL;

        switch (box_trak->box_mdia->box_hdlr->handler_type) { 
        case ISOM_TRACK_VISUAL: //video
        case ISOM_TRACK_AUDIO: //sound
            break;
        default:
            continue;
        }
        
        // create(add) track_stream_info of session
        track_stream_info_entry = (track_stream_info_entry_t *)malloc(sizeof(track_stream_info_entry_t));
        if (track_stream_info_entry == NULL) {
            result_code = ISOM_MEMORY_ERROR;
            session_stream_info->result_code = result_code;
            return result_code;
        }

        track_stream_info = (track_stream_info_t *)malloc(sizeof(track_stream_info_t));
        if (track_stream_info == NULL) {
            result_code = ISOM_MEMORY_ERROR;
            session_stream_info->result_code = result_code;
            return result_code;
        }
 
        memset(track_stream_info, 0, sizeof(track_stream_info_t));
        memset(track_stream_info_entry, 0x00, sizeof(track_stream_info_entry_t));

        
        track_stream_info_entry->track_stream_info        = track_stream_info;
        session_stream_info->track_stream_info_list_entry = track_stream_info_entry;
        session_stream_info->has_stream_info_track_count += 1;

        if ((result_code = rtpfile_init_track_sample_control_info(box_trak, &track_sample_control_info)) != ISOM_OK) {
            session_stream_info->result_code = result_code;
            return result_code;
        }
        track_stream_info->track_sample_control_info = track_sample_control_info;

        track_stream_info->box_trak = box_trak;

        track_stream_info->rtp_packet_info.rtp_payload_data = (char *)malloc(session_stream_info->mtu_size-50);
        if (track_stream_info->rtp_packet_info.rtp_payload_data == NULL) {
            result_code = ISOM_MEMORY_ERROR;
            session_stream_info->result_code = result_code;
            return result_code;
        }
        track_stream_info->is_packetizing = 0;  	

        // get decode option
        track_media_type = box_trak->box_mdia->box_hdlr->handler_type;
        if (track_media_type == ISOM_TRACK_VISUAL) {
            if (mpeg4_video_generic_flag) {
                decode_option = RTP_PCK_SIGNAL_RAP | RTP_PCK_FORCE_MPEG4;
            }
        }
        else if (track_media_type == ISOM_TRACK_AUDIO)	{
            if (mpeg4_audio_generic_flag) {
                decode_option = RTP_PCK_SIGNAL_RAP | RTP_PCK_FORCE_MPEG4;
            }
            else {
                decode_option = RTP_PCK_USE_LATM_AAC;

                if (audio_multi_packet_flag)
                    decode_option |= RTP_PCK_USE_MULTI;
            } 
        }    
   
        usable_mtu_payload_size = session_stream_info->mtu_size - 50;

        if ((result_code = rtpfile_set_track_decode_packetize_info(session_stream_info, track_stream_info, box_trak, usable_mtu_payload_size, decode_option)) != ISOM_OK) {
            session_stream_info->result_code = result_code;
            return result_code;
        }
        
        track_stream_info->ts_scale = track_stream_info->track_decode_info->sl_config_descriptor.timestamp_resolution;
        track_stream_info->ts_scale /= box_trak->box_mdia->box_mdhd->timescale;

        track_stream_info->microsec_ts_scale = 1000000;
        track_stream_info->microsec_ts_scale /= box_trak->box_mdia->box_mdhd->timescale;

        session_stream_info->conf_payload_type++;

        if ((result_code = rtpfile_init_access_unit_info(track_stream_info)) != ISOM_OK) {
            session_stream_info->result_code = result_code;
            return result_code;
        }

        track_stream_info_entry->track_stream_info_entry_id = box_trak->box_tkhd->track_id;
        //tmp_list_entry->track_type = tmp_list_entry->track->box_hdlr->handler_type;
        
        track_stream_info_entry->next_track_stream_info_entry = NULL;
        if (session_stream_info->track_stream_info_list_head == NULL) {
            session_stream_info->track_stream_info_list_head = session_stream_info->track_stream_info_list_tail = track_stream_info_entry;
        } 
        else {
            session_stream_info->track_stream_info_list_tail->next_track_stream_info_entry = track_stream_info_entry;
            session_stream_info->track_stream_info_list_tail = track_stream_info_entry;
        }
    }

    session_stream_info->result_code = ISOM_OK;
    return ISOM_OK;    
}


/*
* Desc : track의 ssrc 값 셋팅 
*/
rs_status_t rtpfile_set_track_ssrc(session_stream_info_t *session_stream_info, UInt32 track_id, UInt32 ssrc)
{
    rs_status_t       result_code = ISOM_UNKNOWN;
    track_stream_info_t *track_stream_info   = NULL;
    
    if (!session_stream_info) {
        session_stream_info->result_code = ISOM_BAD_PARAM;
        return ISOM_BAD_PARAM;
    }

    if ((result_code = rtpfile_find_track_stream_info(session_stream_info, track_id, &track_stream_info)) == ISOM_OK) {
        track_stream_info->rtp_packet_info.rtp_header_info.ssrc = ssrc;
    }
    
    session_stream_info->result_code = result_code;
    return result_code;
}

/*
* Desc : 
*/
rs_status_t rtpfile_process_rtp_packetizing(session_stream_info_t *session_stream_info, track_stream_info_t *track_stream_info, char *data, UInt32 data_size, UInt8 is_au_end, UInt32 full_au_size, UInt32 duration)
{
    track_decode_info_t *track_decode_info;
    if (!session_stream_info || !track_stream_info) {
        session_stream_info->result_code = ISOM_BAD_PARAM;
        return ISOM_BAD_PARAM;
    }
    track_decode_info = track_stream_info->track_decode_info;
    
    switch (track_decode_info->rfc_codec_type) {

    case RTP_PAYT_H264_AVC:
        return rtpfile_packetize_avc(session_stream_info, track_stream_info, data, data_size, is_au_end);
    case RTP_PAYT_LATM: 
        return rtpfile_packetize_latm(session_stream_info, track_stream_info, data, data_size, duration);
    case RTP_PAYT_MPEG4:
        return rtpfile_packetize_mpeg4(session_stream_info, track_stream_info, data, data_size, is_au_end, full_au_size);
    case RTP_PAYT_H263:
        return rtpfile_packetize_h263(session_stream_info, track_stream_info, data, data_size, is_au_end, full_au_size);
    case RTP_PAYT_AMR:
    case RTP_PAYT_AMR_WB:
		return rtpfile_packetize_amr(session_stream_info, track_stream_info, data, data_size, is_au_end, full_au_size);
    case RTP_PAYT_QCELP:
        return rtpfile_packetize_qcelp(session_stream_info, track_stream_info, data, data_size, is_au_end, full_au_size);
    case RTP_PAYT_EVRC_SMV:
		return rtpfile_packetize_evrc_smv(session_stream_info, track_stream_info, data, data_size, is_au_end, full_au_size);

    default:
        session_stream_info->result_code = ISOM_UNKNOWN_CODEC_TYPE;
        return ISOM_UNKNOWN_CODEC_TYPE;
    }
}

/*
* Desc :
*/
rs_status_t rtpfile_set_session_output_sdp(session_stream_info_t *session_stream_info, char *sdp)
{
    if (!session_stream_info || !sdp) {
        session_stream_info->result_code = ISOM_BAD_PARAM;
        return ISOM_ERROR;
    }
    session_stream_info->output_sdp = strdup(sdp);
    session_stream_info->result_code = ISOM_OK;
    return ISOM_OK;
}


/*
* Desc :  track_stream_info_t 얻을때 사용 
*/
rs_status_t rtpfile_next_track_stream_info(session_stream_info_t *session_stream_info,
                                           track_stream_info_t  **track_stream_info,
                                           track_stream_info_t   *last_found_track_stream_info)
{
    track_stream_info_entry_t      *list_entry;

    // Return the first track if requested.
    if (last_found_track_stream_info == NULL) {
        if (session_stream_info->track_stream_info_list_head != NULL) {
            *track_stream_info = session_stream_info->track_stream_info_list_head->track_stream_info;
            return ISOM_OK;
        }
        else {
            return ISOM_ERROR;
        }
    }
    
    // Find LastTrack and return the one after it.
    for (list_entry = session_stream_info->track_stream_info_list_head; list_entry != NULL; list_entry = list_entry->next_track_stream_info_entry) {
        // Check for matches.
        if (list_entry->track_stream_info_entry_id == last_found_track_stream_info->box_trak->box_tkhd->track_id) {

            // Is there a next track?
            if( list_entry->next_track_stream_info_entry != NULL ) {
                *track_stream_info = list_entry->next_track_stream_info_entry->track_stream_info;
                return ISOM_OK;
            } else {
                return ISOM_ERROR;
            }
        }
    }

    return ISOM_ERROR;
}

/*
* Desc : track_id로 track_stream_info 얻음  
*/
rs_status_t rtpfile_find_track_stream_info(session_stream_info_t *session_stream_info, UInt32 track_id, track_stream_info_t **track_stream_info)
{
    // General vars
    track_stream_info_entry_t      *list_entry;

    // Find the specified track.
    for(list_entry = session_stream_info->track_stream_info_list_head; list_entry != NULL; list_entry = list_entry->next_track_stream_info_entry ) {
        // Check for matches.
        if (list_entry->track_stream_info_entry_id == track_id) {
            *track_stream_info = list_entry->track_stream_info;
            session_stream_info->result_code = ISOM_OK;
            return ISOM_OK;
        }
    }    

    // The search failed.
    session_stream_info->result_code = ISOM_TRACK_STREAM_INFO_NOT_FOUND;
    return ISOM_TRACK_STREAM_INFO_NOT_FOUND;
}

/*
* Desc : 
*/
void rtpfile_delete_track_decode_packetize_info(track_stream_info_t *track_stream_info)
{
    track_packetize_info_t *track_packetize_info;
    track_decode_info_t *track_decode_info;

    track_packetize_info = track_stream_info->track_packetize_info;
    track_decode_info = track_stream_info->track_decode_info;

    if (track_decode_info) {
        if (track_decode_info->track_bitrate_info) {
            free(track_decode_info->track_bitrate_info);
            track_decode_info->track_bitrate_info = NULL;
        }
        free(track_decode_info);
        track_stream_info->track_decode_info = NULL; 
    }

    if (track_packetize_info) { 
        if (track_packetize_info->bo_payload_data) {
            isom_delete_bit_operation(track_packetize_info->bo_payload_data);
        }

        if (track_packetize_info->bo_payload_header) {
            isom_delete_bit_operation(track_packetize_info->bo_payload_header);
        }
        free(track_packetize_info);
        track_stream_info->track_packetize_info = NULL;
    }

    return;
}

/*
* Desc : session_stream_info 내의 모든 track_stream_info 및 track_stream_info_entry delete
*/
void rtpfile_delete_track_stream_info(session_stream_info_t *session_stream_info)
{
    track_stream_info_t *track_stream_info = NULL;
    track_stream_info_entry_t *track_stream_info_entry = NULL;
    track_stream_info_entry_t *next_track_stream_info_entry = NULL;

    track_stream_info_entry = session_stream_info->track_stream_info_list_head;
    next_track_stream_info_entry = track_stream_info_entry ? track_stream_info_entry->next_track_stream_info_entry : NULL;
    while (track_stream_info_entry) {
        track_stream_info = track_stream_info_entry->track_stream_info;
        
        if (track_stream_info_entry->track_stream_info != NULL) {
            if (track_stream_info->track_decode_info) {
                rtpfile_delete_track_decode_packetize_info(track_stream_info);
            }

            if (track_stream_info->track_sample_control_info) {
                free(track_stream_info->track_sample_control_info);
                track_stream_info->track_sample_control_info = NULL;
            }
        
            if (track_stream_info->rtp_packet_info.rtp_payload_data) {
                free(track_stream_info->rtp_packet_info.rtp_payload_data);	
                track_stream_info->rtp_packet_info.rtp_payload_data = NULL;
            }
            if (track_stream_info->au_info) {
                if (track_stream_info->au_info->data) {
                    free(track_stream_info->au_info->data);
                    track_stream_info->au_info->data = NULL;
                }
                free(track_stream_info->au_info);
                track_stream_info->au_info = NULL;
            }
            
            free(track_stream_info_entry->track_stream_info);
            track_stream_info_entry->track_stream_info = NULL;
        }        
        free(track_stream_info_entry);

        track_stream_info_entry = next_track_stream_info_entry;
        if (track_stream_info_entry != NULL) {
            next_track_stream_info_entry = track_stream_info_entry->next_track_stream_info_entry;
        }
    }
}
