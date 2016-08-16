#include "packetizer_3gpp.h"
#include "sample_pck_func.h"

/*
* Desc : h263
*/
rs_status_t rtpfile_packetize_h263(session_stream_info_t *session_stream_info, track_stream_info_t *track_stream_info, char *data, UInt32 data_size, UInt8 is_au_end, UInt32 full_au_size)                         
{
    char hdr[2];
    Bool16 Pbit;
    UInt32 offset, size, max_size;

    isom_bit_operation_t   *bo;
    track_decode_info_t    *track_decode_info;
    track_packetize_info_t *track_packetize_info;
    
    if (!track_stream_info) {
        return ISOM_BAD_PARAM;
    }
    track_decode_info = track_stream_info->track_decode_info;
    track_packetize_info = track_stream_info->track_packetize_info;

    track_stream_info->rtp_packet_info.rtp_header_info.timestamp = (UInt32) track_decode_info->rtp_sl_header.composition_timestamp;
    max_size = track_packetize_info->usable_mtu_payload_size - 2;
    
    /*the H263 hinter doesn't perform inter-sample concatenation*/
    if (!data) {
        return ISOM_BAD_PARAM;
    }
    
    if (track_packetize_info->readbytes_leftsize_in_au == 0) {
        track_packetize_info->readbytes_leftsize_in_au = data_size;
    }
    
    if (track_decode_info->rtp_sl_header.access_unit_start_flag) {
        Pbit = 1;
        track_decode_info->iv += 2;
        /*skip 16 0'ed bits of start code*/
        offset = 2;
        track_packetize_info->readbytes_leftsize_in_au -= 2;
        
    }
    else {
        offset = data_size - track_packetize_info->readbytes_leftsize_in_au;
        Pbit = 0;
    }
    
    if(track_packetize_info->readbytes_leftsize_in_au > max_size) {
        size = max_size;
        track_stream_info->rtp_packet_info.rtp_header_info.marker = 0;
    }
    else {
        track_decode_info->rtp_sl_header.access_unit_end_flag = 1;
        size = track_packetize_info->readbytes_leftsize_in_au;
        track_stream_info->rtp_packet_info.rtp_header_info.marker = 1;
    }
    
    track_packetize_info->readbytes_leftsize_in_au -= size;
    track_decode_info->rtp_sl_header.access_unit_start_flag = 0;
    
    /*create new RTP Packet */
    track_stream_info->rtp_packet_info.rtp_header_info.sequence_number += 1;
    //track_decode_info->OnNewPacket(track_decode_info->cbk_obj, &track_stream_info->rtp_packet_info.rtp_header_info);
    
    bo = isom_init_bit_operation(hdr, 2, ISOM_BITOPER_WRITE);
    isom_write_bit_operation_int(bo, 0, 5);
    isom_write_bit_operation_int(bo, Pbit, 1);
    isom_write_bit_operation_int(bo, 0, 10);
    isom_delete_bit_operation(bo);
    
    /*add header*/
    rtpfile_set_rtp_payload_data(session_stream_info, track_stream_info, (char*) hdr, 2, 1);
    /*add payload*/
    rtpfile_set_rtp_payload_data(session_stream_info, track_stream_info, data + offset, size, 0);
    
    track_packetize_info->bytes_in_rtp_payload += size;
    track_decode_info->iv += size;
    
    rtpfile_set_rtp_packet_send_info(session_stream_info, track_stream_info, &track_stream_info->rtp_packet_info.rtp_header_info);
    
    track_packetize_info->bytes_in_rtp_payload = 0;
    
    if (track_packetize_info->readbytes_leftsize_in_au == 0 || track_packetize_info->readbytes_leftsize_in_au > data_size) {
        track_decode_info->rtp_sl_header.access_unit_start_flag = 1;
        track_decode_info->rtp_sl_header.access_unit_end_flag   = 0;
        track_packetize_info->readbytes_leftsize_in_au		   = 0;
        track_decode_info->rtp_timestamp = 0;
    }
    
    session_stream_info->result_code = ISOM_OK;
    return ISOM_OK;
}

/*
* Desc : amr flush
*/
static void rtp_amr_flush(session_stream_info_t *session_stream_info,  track_stream_info_t *track_stream_info)
{
    char *hdr;
    UInt32 hdr_size;
    track_decode_info_t *track_decode_info;
    track_packetize_info_t *track_packetize_info;
    
    track_packetize_info = track_stream_info->track_packetize_info;
    track_decode_info = track_stream_info->track_decode_info;

    if (!track_packetize_info->bytes_in_rtp_payload) {
        track_packetize_info->bytes_in_rtp_payload         = 0;
        track_packetize_info->au_count_in_packet			   = 0;
        track_packetize_info->readbytes_leftsize_in_au = 0;
        isom_delete_bit_operation(track_packetize_info->bo_payload_header);
        return;
    }
    isom_get_bit_operation_content(track_packetize_info->bo_payload_header, &hdr, &hdr_size);
    isom_delete_bit_operation(track_packetize_info->bo_payload_header);
    track_packetize_info->bo_payload_header = NULL;
    /*overwrite last frame F bit*/
    hdr[track_packetize_info->au_count_in_packet] &= 0x7F;
    rtpfile_set_rtp_payload_data(session_stream_info, track_stream_info, hdr, hdr_size, 1);
    if (hdr) {
        free(hdr);
    }
    rtpfile_set_rtp_packet_send_info(session_stream_info, track_stream_info, &track_stream_info->rtp_packet_info.rtp_header_info);
    track_packetize_info->bytes_in_rtp_payload = 0;
    track_packetize_info->au_count_in_packet = 0;
}

/*
* Desc : amr
*/
rs_status_t rtpfile_packetize_amr(session_stream_info_t *session_stream_info,  track_stream_info_t *track_stream_info, char *data, UInt32 data_size, UInt8 is_au_end, UInt32 full_au_size)                         
{
    UInt32 offset, block_size, size;
    UInt8 ft;
    track_decode_info_t *track_decode_info;
    track_packetize_info_t *track_packetize_info;
    
    if (!track_stream_info) {
        return ISOM_BAD_PARAM;
    }

    track_packetize_info = track_stream_info->track_packetize_info;
    track_decode_info = track_stream_info->track_decode_info;

    if (track_decode_info->rtp_sl_header.access_unit_start_flag == 1) {
        if (!data) {
            rtp_amr_flush(session_stream_info, track_stream_info);
            return ISOM_OK;
        }
        
        track_decode_info->rtp_timestamp = (UInt32) track_decode_info->rtp_sl_header.composition_timestamp;
        track_packetize_info->readbytes_leftsize_in_au = data_size;
        offset = 0;
    }
    else {
        offset = data_size - track_packetize_info->readbytes_leftsize_in_au;
    }
    
    //	while (data_size>offset) {
    ft = (data[offset] & 0x78) >> 3;
    
    
    if (track_decode_info->rfc_codec_type == RTP_PAYT_AMR_WB) {
        size = AMR_WB_FRAME_SIZE[ft];
        block_size = 320;
    } else {
        size = AMR_FRAME_SIZE[ft];
        block_size = 160;
    }
    track_decode_info->rtp_sl_header.access_unit_start_flag = 0;
    
    /*packet full or too long*/
    if (track_packetize_info->bytes_in_rtp_payload + 1 + size > track_packetize_info->usable_mtu_payload_size) {
        rtp_amr_flush(session_stream_info, track_stream_info);
    }
    
    /*need new*/
    if (!track_packetize_info->bytes_in_rtp_payload) {
        track_stream_info->rtp_packet_info.rtp_header_info.timestamp = track_decode_info->rtp_timestamp;
        track_stream_info->rtp_packet_info.rtp_header_info.marker = 0;	/*never set*/
        track_stream_info->rtp_packet_info.rtp_header_info.sequence_number += 1;
        //		assert(track_decode_info->bo_payload_header==NULL);
        
        /*always have header and Box Entry*/
        track_packetize_info->bo_payload_header = isom_init_bit_operation(NULL, 0, ISOM_BITOPER_WRITE);
        /*CMR + res (all 0, no interleaving)*/
        isom_write_bit_operation_int(track_packetize_info->bo_payload_header, ft, 4);
        isom_write_bit_operation_int(track_packetize_info->bo_payload_header, 0, 4);
        track_packetize_info->bytes_in_rtp_payload = 1;
        /*no interleaving*/
    }
    
    /*F always to 1*/
    isom_write_bit_operation_int(track_packetize_info->bo_payload_header, 1, 1);
    isom_write_bit_operation_int(track_packetize_info->bo_payload_header, ft, 4);
    /*Q*/
    isom_write_bit_operation_int(track_packetize_info->bo_payload_header, (data[offset] & 0x4) ? 1 : 0, 1);
    isom_write_bit_operation_int(track_packetize_info->bo_payload_header, 0, 2);
    track_packetize_info->bytes_in_rtp_payload ++;
    
    /*remove frame type byte*/
    offset++;
    
    /*add frame data without rate_type byte header*/
    rtpfile_set_rtp_payload_data(session_stream_info, track_stream_info, data+offset, size, 0);

    track_packetize_info->au_count_in_packet++;
    track_packetize_info->bytes_in_rtp_payload += size;
    offset += size;
    track_packetize_info->readbytes_leftsize_in_au -= size+1;
    track_decode_info->rtp_timestamp += block_size;
    //	assert(track_decode_info->bytes_in_rtp_payload<=track_decode_info->Path_MTU);
    /*take care of aggregation, flush if needed*/
    if (track_packetize_info->au_count_in_packet==track_decode_info->auh_size) {
        rtp_amr_flush(session_stream_info, track_stream_info);
    }
    
    if (track_packetize_info->readbytes_leftsize_in_au == 0 || track_packetize_info->readbytes_leftsize_in_au > data_size) {
        track_decode_info->rtp_sl_header.access_unit_start_flag = 1;
        track_decode_info->rtp_sl_header.access_unit_end_flag   = 0;
        track_packetize_info->readbytes_leftsize_in_au		   = 0;
        track_decode_info->rtp_timestamp = 0;
    }
    
    session_stream_info->result_code = ISOM_OK;
    return ISOM_OK;
}

/*
* Desc : qcelp rate size
*/
#define TSINLINE	inline
static UInt32 rtpfile_qes_get_rate_size(UInt32 idx, UInt32 *rates, UInt32 nb_rates)
{
    UInt32 i;
    for (i=0; i<nb_rates; i++) {
        if (rates[2*i]==idx) {
            return rates[2*i+1];
        }
    }
    return 0;
}

/*
* Desc : qcelp
*/
rs_status_t rtpfile_packetize_qcelp(session_stream_info_t *session_stream_info,  track_stream_info_t *track_stream_info, char *data, UInt32 data_size, UInt8 is_au_end, UInt32 full_au_size)
{
    UInt32 offset, size;
    UInt8 hdr, frame_type;
    track_decode_info_t *track_decode_info;
    track_packetize_info_t *track_packetize_info;
    
    if (!track_stream_info) {
        return ISOM_BAD_PARAM;
    }

    track_packetize_info = track_stream_info->track_packetize_info;
    track_decode_info = track_stream_info->track_decode_info;

    if (track_decode_info->rtp_sl_header.access_unit_start_flag == 1) {
        if (!data) {
            if (track_packetize_info->bytes_in_rtp_payload) rtpfile_set_rtp_packet_send_info(session_stream_info, track_stream_info, &track_stream_info->rtp_packet_info.rtp_header_info);
            track_packetize_info->bytes_in_rtp_payload					= 0;
            track_packetize_info->au_count_in_packet						= 0;
            track_decode_info->rtp_sl_header.access_unit_start_flag  = 1;
            track_packetize_info->readbytes_leftsize_in_au			= 0;
            track_decode_info->rtp_timestamp							= 0;
            
            return ISOM_OK;
        }
        
        track_decode_info->rtp_timestamp = (UInt32) track_decode_info->rtp_sl_header.composition_timestamp;
        track_packetize_info->readbytes_leftsize_in_au = data_size;
        offset = 0;
    }
    else {
        offset = data_size - track_packetize_info->readbytes_leftsize_in_au;
    }
    
    
    frame_type = data[offset];
    size = rtpfile_qes_get_rate_size(frame_type, (UInt32 *)QCELP_RATE_TO_SIZE, QCELP_RATE_TO_SIZE_NB);
    track_decode_info->rtp_sl_header.access_unit_start_flag = 0;
    
    /*reserved, not sent*/
    if (frame_type>=5) {
        offset += size;
        track_packetize_info->readbytes_leftsize_in_au -= size;
    }
    else {
        //packet full or too long
        if (track_packetize_info->bytes_in_rtp_payload + size > track_packetize_info->usable_mtu_payload_size) {
            rtpfile_set_rtp_packet_send_info(session_stream_info, track_stream_info, &track_stream_info->rtp_packet_info.rtp_header_info);
            track_packetize_info->bytes_in_rtp_payload = 0;
            track_packetize_info->au_count_in_packet = 0;
        }
        
        //need new
        if (!track_packetize_info->bytes_in_rtp_payload) {
            track_stream_info->rtp_packet_info.rtp_header_info.timestamp = track_decode_info->rtp_timestamp;
            track_stream_info->rtp_packet_info.rtp_header_info.marker = 0;	//never set
            track_stream_info->rtp_packet_info.rtp_header_info.sequence_number += 1;
            hdr = 0;//no interleaving
            rtpfile_set_rtp_payload_data(session_stream_info, track_stream_info, (char*)&hdr, 1, 0);
            track_packetize_info->bytes_in_rtp_payload = 1;
        }

        rtpfile_set_rtp_payload_data(session_stream_info, track_stream_info, data+offset, size, 0);

        track_packetize_info->readbytes_leftsize_in_au -= size;
        track_packetize_info->bytes_in_rtp_payload += size;
        offset += size;
        track_decode_info->rtp_timestamp += 160;
        //		assert(track_decode_info->bytes_in_rtp_payload<=track_decode_info->Path_MTU);
        
        //take care of aggregation, flush if needed
        track_packetize_info->au_count_in_packet++;
        if (track_packetize_info->au_count_in_packet==track_decode_info->auh_size) {
            rtpfile_set_rtp_packet_send_info(session_stream_info, track_stream_info, &track_stream_info->rtp_packet_info.rtp_header_info);
            track_packetize_info->bytes_in_rtp_payload = 0;
            track_packetize_info->au_count_in_packet = 0;
        }
    }
    
    if (track_packetize_info->readbytes_leftsize_in_au == 0 || track_packetize_info->readbytes_leftsize_in_au > data_size) {
        track_decode_info->rtp_sl_header.access_unit_start_flag = 1;
        track_decode_info->rtp_sl_header.access_unit_end_flag   = 0;
        track_packetize_info->readbytes_leftsize_in_au		   = 0;
        track_decode_info->rtp_timestamp = 0;
    }
    
    session_stream_info->result_code = ISOM_OK;
    return ISOM_OK;
}

/*
* Desc : evrc flush
*/
static void rtp_evrc_smv_flush(session_stream_info_t *session_stream_info,  track_stream_info_t *track_stream_info)
{
    track_decode_info_t *track_decode_info;
    track_packetize_info_t *track_packetize_info;
    
    track_packetize_info = track_stream_info->track_packetize_info;
    track_decode_info = track_stream_info->track_decode_info;

	if (!track_packetize_info->bytes_in_rtp_payload) {
		track_packetize_info->bytes_in_rtp_payload = 0;
		track_packetize_info->au_count_in_packet = 0;
		track_packetize_info->readbytes_leftsize_in_au = 0;
		isom_delete_bit_operation(track_packetize_info->bo_payload_header);
		return;
	}
	if (track_decode_info->auh_size>0) {
		char *hdr;
		UInt32 hdr_size;
		/*padding*/
		if (track_packetize_info->au_count_in_packet % 2) isom_write_bit_operation_int(track_packetize_info->bo_payload_header, 0, 4);
		isom_get_bit_operation_content(track_packetize_info->bo_payload_header, &hdr, &hdr_size);
		isom_delete_bit_operation(track_packetize_info->bo_payload_header);
		track_packetize_info->bo_payload_header = NULL;
		/*overwrite count*/
		hdr[0] = 0;
		hdr[1] = (char)track_packetize_info->au_count_in_packet-1;/*MMM + frameCount-1*/
		rtpfile_set_rtp_payload_data(session_stream_info, track_stream_info, hdr, hdr_size, 1);
        if (hdr) {
            free(hdr);
        }
	}
	rtpfile_set_rtp_packet_send_info(session_stream_info, track_stream_info, &track_stream_info->rtp_packet_info.rtp_header_info);
	track_packetize_info->bytes_in_rtp_payload = 0;
	track_packetize_info->au_count_in_packet = 0;
}

/*
* Desc : evrc
*/
rs_status_t rtpfile_packetize_evrc_smv(session_stream_info_t *session_stream_info,  track_stream_info_t *track_stream_info, char *data, UInt32 data_size, UInt8 is_au_end, UInt32 full_au_size)
{
	UInt32 offset, size;
	UInt8 frame_type;
    track_decode_info_t *track_decode_info;
    track_packetize_info_t *track_packetize_info;
    
    if (!track_stream_info) {
        return ISOM_BAD_PARAM;
    }

    track_packetize_info = track_stream_info->track_packetize_info;
    track_decode_info = track_stream_info->track_decode_info;

    if ( track_decode_info->rtp_sl_header.access_unit_start_flag == 1 ) {
		if (!data) {
			rtp_evrc_smv_flush(session_stream_info, track_stream_info);
			track_packetize_info->bytes_in_rtp_payload					= 0;
			track_packetize_info->au_count_in_packet						= 0;
			track_decode_info->rtp_sl_header.access_unit_start_flag  = 1;
			track_packetize_info->readbytes_leftsize_in_au			= 0;
			track_decode_info->rtp_timestamp							= 0;
			return ISOM_OK;
		}

		track_decode_info->rtp_timestamp = (UInt32) track_decode_info->rtp_sl_header.composition_timestamp;
		track_packetize_info->readbytes_leftsize_in_au = data_size;
		offset = 0;
	}
	else {
		offset = data_size - track_packetize_info->readbytes_leftsize_in_au;
	}
//	while (data_size>offset) {
	frame_type = data[offset];
	size = rtpfile_qes_get_rate_size(frame_type, (UInt32 *)SMV_EVRC_RATE_TO_SIZE, SMV_EVRC_RATE_TO_SIZE_NB);
	track_decode_info->rtp_sl_header.access_unit_start_flag = 0;

	/*reserved, not sent)*/
	if (frame_type>=5) {
		offset += size;
		track_packetize_info->readbytes_leftsize_in_au -= size;
	}
	else {
		/*packet full or too long*/
		if (track_packetize_info->bytes_in_rtp_payload + size > track_packetize_info->usable_mtu_payload_size) 
		{
			rtp_evrc_smv_flush(session_stream_info, track_stream_info);
		}

		/*need new*/
		if (!track_packetize_info->bytes_in_rtp_payload) {
			track_stream_info->rtp_packet_info.rtp_header_info.timestamp = track_decode_info->rtp_timestamp;
			track_stream_info->rtp_packet_info.rtp_header_info.marker = 0;	/*never set*/
			track_stream_info->rtp_packet_info.rtp_header_info.sequence_number += 1;
			//track_decode_info->OnNewPacket(track_decode_info->cbk_obj, &track_stream_info->rtp_packet_info.rtp_header_info);
//			assert(track_decode_info->bo_payload_header==NULL);

			if (track_decode_info->auh_size>0) {
				track_packetize_info->bo_payload_header = isom_init_bit_operation(NULL, 0, ISOM_BITOPER_WRITE);
				/*RRLLLNNN (all 0, no interleaving)*/
				isom_write_bit_operation_u8(track_packetize_info->bo_payload_header, 0);
				/*MMM + count-1 : overriden when flushing*/
				isom_write_bit_operation_u8(track_packetize_info->bo_payload_header, 0);
				track_packetize_info->bytes_in_rtp_payload = 2;
			}
		}

		/*bundle mode: cat rate byte to Box Entry, on 4 bits*/
		if (track_decode_info->auh_size > 0) {
			isom_write_bit_operation_int(track_packetize_info->bo_payload_header, data[offset], 4);
			if (!(track_packetize_info->au_count_in_packet % 2)) track_packetize_info->bytes_in_rtp_payload += 1;
		}
		/*note that EVEN in header-free format the rate_type byte is removed*/
		offset += 1;
		size -= 1;
		track_packetize_info->readbytes_leftsize_in_au -= 1;
		//size = track_decode_info->readbytes_leftsize_in_au;
		/*add frame data without rate_type byte header*/
		rtpfile_set_rtp_payload_data(session_stream_info, track_stream_info, data+offset, size, 0);
		
		track_packetize_info->readbytes_leftsize_in_au -= size;
		track_packetize_info->au_count_in_packet++;
		track_packetize_info->bytes_in_rtp_payload += size;
		offset += size;
		track_decode_info->rtp_timestamp += 160;
//		assert(track_decode_info->bytes_in_rtp_payload<=track_decode_info->Path_MTU);
		/*take care of aggregation, flush if needed*/
        if (track_packetize_info->au_count_in_packet == track_decode_info->auh_size) {
            rtp_evrc_smv_flush(session_stream_info, track_stream_info);
        }
	}
	
	if (track_packetize_info->readbytes_leftsize_in_au == 0 || track_packetize_info->readbytes_leftsize_in_au > data_size) {
		track_decode_info->rtp_sl_header.access_unit_start_flag = 1;
		track_decode_info->rtp_sl_header.access_unit_end_flag   = 0;
		track_packetize_info->readbytes_leftsize_in_au		   = 0;
		track_decode_info->rtp_timestamp = 0;
	}

//	}
    session_stream_info->result_code = ISOM_OK;
	return ISOM_OK;
}


