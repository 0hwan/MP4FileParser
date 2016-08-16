#include "packetizer_mp4_avc.h"
#include "sample_description.h"

/*
* Desc : 
*/
rs_status_t rtpfile_packetize_mpeg4(session_stream_info_t *session_stream_info, track_stream_info_t *track_stream_info, char *data, UInt32 data_size, UInt8 is_au_end, UInt32 full_au_size)
{
	char *sl_buffer, *payl_buffer;
	UInt32 sl_buffer_size, payl_buffer_size; 
	UInt32 auh_size_tmp, info_size, pck_size, pos;//pck_size 전송할 data size
	UInt8 flush_pck, no_split;

    track_decode_info_t    *track_decode_info;
    track_packetize_info_t *track_packetize_info;
    
    if (!track_stream_info) {
        return ISOM_BAD_PARAM;
    }

    track_packetize_info = track_stream_info->track_packetize_info;
    track_decode_info = track_stream_info->track_decode_info;

	flush_pck = 0;

    if (track_packetize_info->readbytes_leftsize_in_au == 0) {
		track_packetize_info->readbytes_leftsize_in_au = data_size;
    }
	/*flush everything*/
	if (!data) {
        /*
        if (track_decode_info->bo_payload_data) {
            goto flush_packet;
        }
		return true;
        */
        return ISOM_BAD_PARAM;
	}

    if (track_packetize_info->bo_payload_data){//generic audio의 경우 모아서 보내기 위해
		track_decode_info->rtp_sl_header.access_unit_start_flag = 1;
    }

	no_split = 0;

	if (track_decode_info->rtp_sl_header.access_unit_start_flag) {
		//init SL
		if (track_decode_info->rtp_sl_header.composition_timestamp != track_decode_info->rtp_sl_header.decoding_timestamp) {
			track_decode_info->rtp_sl_header.decoding_timestamp_flag = 1;
		}
		track_decode_info->rtp_sl_header.composition_timestamp_flag = 1;
		track_decode_info->rtp_sl_header.accessunit_length = (UInt16)full_au_size;

		//init some vars - based on the available size and the TS
		//we decide if we go with the same RTP TS serie or not
		if (track_packetize_info->bo_payload_data) {
			//don't store more than what we can (that is 2^rtp_mp4_es_generic_info->CTSDelta - 1)
			if ( (track_decode_info->decode_option & RTP_PCK_SIGNAL_TS) 
				&& (track_decode_info->rtp_sl_header.composition_timestamp - track_stream_info->rtp_packet_info.rtp_header_info.timestamp 
                    >= (UInt32) ( 1 << track_decode_info->rtp_mp4_es_generic_info.cts_delta_length) ) ) {
				goto flush_packet;
			}
			//don't split AU if # TS , start a new RTP pck 
            if (track_decode_info->rtp_sl_header.composition_timestamp != track_stream_info->rtp_packet_info.rtp_header_info.timestamp) {
				no_split = 1;
            }
		}
	}

	/*new RTP Packet*/
	if (!track_packetize_info->bo_payload_data) {
		/*first SL in RTP*/
		track_packetize_info->first_sl_in_rtp = 1;

		/*if this is the end of an AU we will set it to 0 as soon as an AU is splited*/
		track_stream_info->rtp_packet_info.rtp_header_info.marker = 1;
		track_stream_info->rtp_packet_info.rtp_header_info.sequence_number += 1;

		track_stream_info->rtp_packet_info.rtp_header_info.timestamp = (UInt32) track_decode_info->rtp_sl_header.composition_timestamp;
		/*prepare the mapped headers*/
		track_packetize_info->bo_payload_header = isom_init_bit_operation(NULL, 0, ISOM_BITOPER_WRITE);
		track_packetize_info->bo_payload_data = isom_init_bit_operation(NULL, 0, ISOM_BITOPER_WRITE);
		pck_size = info_size = 0;
		track_packetize_info->bytes_in_rtp_payload = 0;

		/*in multiSL there is a MSLHSize structure on 2 bytes*/
		track_packetize_info->mp4_packetize_au_header_size = 0;
		if (track_packetize_info->has_au_header) {
			track_packetize_info->mp4_packetize_au_header_size = 16;
			isom_write_bit_operation_int(track_packetize_info->bo_payload_header, 0, 16);
		}			
		flush_pck = 0;
		/*and create packet*/
//		track_decode_info->on_new_packet(track_decode_info->cbk_obj, &track_stream_info->rtp_packet_info.rtp_header_info);
	}

	//make sure we are not interleaving too much - this should never happen actually
	if (track_decode_info->rtp_mp4_es_generic_info.index_delta_length 
		&& !track_packetize_info->first_sl_in_rtp 
		&& (track_decode_info->rtp_sl_header.au_sequence_number - track_packetize_info->au_count_in_packet >= (UInt32) 1<<track_decode_info->rtp_mp4_es_generic_info.index_delta_length)) {
		//we cannot write this packet here
		goto flush_packet;
	} 
	/*check max ptime*/
    /*
    if (track_decode_info->max_ptime && ( (UInt32) track_decode_info->rtp_sl_header.composition_timestamp >= track_stream_info->rtp_packet_info.rtp_header_info.timestamp + track_decode_info->max_ptime)) {
		goto flush_packet;
    }
    */

	auh_size_tmp = rtpfile_get_rtp_au_header_size(session_stream_info, track_stream_info, &track_decode_info->rtp_sl_header);

	info_size = auh_size_tmp + track_packetize_info->mp4_packetize_au_header_size;
	info_size /= 8;
	/*align*/
    if ( (track_packetize_info->mp4_packetize_au_header_size + auh_size_tmp) % 8) {
        info_size += 1;
    }

	if (track_packetize_info->readbytes_leftsize_in_au + info_size + track_packetize_info->bytes_in_rtp_payload <= track_packetize_info->usable_mtu_payload_size) {
		//End of our data chunk
		pck_size = track_packetize_info->readbytes_leftsize_in_au;
		track_decode_info->rtp_sl_header.access_unit_end_flag = is_au_end;

		track_packetize_info->mp4_packetize_au_header_size += auh_size_tmp;
		
		track_decode_info->rtp_sl_header.padding_flag = track_decode_info->rtp_sl_header.padding_bits ? 1 : 0;
	} else {

		//AU cannot fit in packet. If no split, start a new packet
        if (no_split) {
            goto flush_packet;
        }

		track_packetize_info->mp4_packetize_au_header_size += auh_size_tmp;

		pck_size = track_packetize_info->usable_mtu_payload_size - (info_size + track_packetize_info->bytes_in_rtp_payload);
		//that's the end of the rtp packet
		flush_pck = 1;
		//but not of the AU -> marker is 0
		track_stream_info->rtp_packet_info.rtp_header_info.marker = 0;
	}

	rtpfile_write_rtp_au_header(session_stream_info, track_stream_info, pck_size, track_stream_info->rtp_packet_info.rtp_header_info.timestamp);
	
    isom_write_bit_operation_data(track_packetize_info->bo_payload_data, data + (data_size - track_packetize_info->readbytes_leftsize_in_au), pck_size);

	track_packetize_info->readbytes_leftsize_in_au -= pck_size;
	track_packetize_info->bytes_in_rtp_payload += pck_size;
	//update IV
	track_decode_info->iv += pck_size;		// IV 전송한 packsize 총합
	track_decode_info->rtp_sl_header.padding_flag = 0;
	track_decode_info->rtp_sl_header.access_unit_start_flag = 0;

	if ( (track_packetize_info->readbytes_leftsize_in_au == 0) && !(track_decode_info->decode_option & RTP_PCK_USE_MULTI) ) {//last packet 전송 
		track_stream_info->rtp_packet_info.rtp_header_info.marker = 1;
		flush_pck = 1;
	}

	//first SL in RTP is done
	track_packetize_info->first_sl_in_rtp = 0;

	//store current sl
	track_packetize_info->au_count_in_packet = track_decode_info->rtp_sl_header.au_sequence_number;

    if (!flush_pck) {
        return ISOM_PACKETIZE_MPEG4_ERROR;
    }

	//done with the packet ############################### flush_packet ################################
flush_packet:

	isom_align_bit_operation(track_packetize_info->bo_payload_header);

	//no aux data yet
	if (track_decode_info->rtp_mp4_es_generic_info.auxiliary_data_size_length)	{
		//write RSLH after the MSLH
		isom_write_bit_operation_int(track_packetize_info->bo_payload_header, 0, track_decode_info->rtp_mp4_es_generic_info.auxiliary_data_size_length);			
	}				
	//rewrite the size header
	if (track_packetize_info->has_au_header) {
		pos = (UInt32) isom_get_bit_operation_position(track_packetize_info->bo_payload_header);		
		isom_seek_bit_operation(track_packetize_info->bo_payload_header, 0);
		track_packetize_info->mp4_packetize_au_header_size -= 16;
		isom_write_bit_operation_int(track_packetize_info->bo_payload_header, track_packetize_info->mp4_packetize_au_header_size, 16);
		isom_seek_bit_operation(track_packetize_info->bo_payload_header, pos);
	}

	sl_buffer = NULL;
	isom_get_bit_operation_content(track_packetize_info->bo_payload_header, &sl_buffer, &sl_buffer_size);
	//delete our bitstream
	isom_delete_bit_operation(track_packetize_info->bo_payload_header);
	track_packetize_info->bo_payload_header = NULL;

	payl_buffer = NULL;
	payl_buffer_size = 0;

	isom_get_bit_operation_content(track_packetize_info->bo_payload_data, &payl_buffer, &payl_buffer_size);

	isom_delete_bit_operation(track_packetize_info->bo_payload_data);
	track_packetize_info->bo_payload_data = NULL;

	//notify header
	rtpfile_set_rtp_payload_data(session_stream_info, track_stream_info, sl_buffer, sl_buffer_size, 1);
	//notify payload
	if (payl_buffer) {
		rtpfile_set_rtp_payload_data(session_stream_info, track_stream_info, payl_buffer, payl_buffer_size, 0);
		free(payl_buffer);
	}
	//flush packet
	rtpfile_set_rtp_packet_send_info(session_stream_info, track_stream_info, &track_stream_info->rtp_packet_info.rtp_header_info);
	free(sl_buffer);

	if (track_packetize_info->readbytes_leftsize_in_au == 0 || track_packetize_info->readbytes_leftsize_in_au > data_size) {
		track_decode_info->rtp_sl_header.access_unit_start_flag = 1;
		track_decode_info->rtp_sl_header.access_unit_end_flag   = 0;
		track_packetize_info->readbytes_leftsize_in_au		   = 0;
	}

    session_stream_info->result_code = ISOM_OK;
	return ISOM_OK;
}

/*
* Desc : get the size of the RSLH section given the TS_SLHeader and the SLMap
*/
UInt32 rtpfile_get_rtp_au_header_size(session_stream_info_t *session_stream_info, track_stream_info_t *track_stream_info, rtp_sl_header_t *slh)
{
    UInt32 nb_bits = 0;
    track_decode_info_t *track_decode_info;
    track_packetize_info_t *track_packetize_info;
    
    if (!track_stream_info) {
        return false;
    }

    track_packetize_info = track_stream_info->track_packetize_info;
    track_decode_info = track_stream_info->track_decode_info;

    /*sel enc*/
    if (track_decode_info->decode_option & RTP_PCK_SELECTIVE_ENCRYPTION) {
        nb_bits += 8;
    }
    /*Note: ISMACryp ALWAYS indicates IV (BSO) and KEYIDX, even when sample is not encrypted. This is
    quite a waste when using selective encryption....*/
    
    /*IV*/
    nb_bits += track_packetize_info->first_sl_in_rtp ? 8*track_decode_info->rtp_mp4_es_generic_info.iv_length : 8 * track_decode_info->rtp_mp4_es_generic_info.iv_delta_length;
    /*keyIndicator*/
    if (track_packetize_info->first_sl_in_rtp || (track_decode_info->decode_option & RTP_PCK_KEY_IDX_PER_AU)) {
        nb_bits += 8*track_decode_info->rtp_mp4_es_generic_info.ki_length;
    }
    
    /*no input header specified, compute the MAX size*/
    if (!slh) {
        /*size length*/
        if (!track_decode_info->rtp_mp4_es_generic_info.constant_size) {
            nb_bits += track_decode_info->rtp_mp4_es_generic_info.size_length;
        }
        /*AU index length*/
        nb_bits += track_packetize_info->first_sl_in_rtp ? track_decode_info->rtp_mp4_es_generic_info.index_length : track_decode_info->rtp_mp4_es_generic_info.index_delta_length;
        /*CTS flag*/
        if (track_decode_info->rtp_mp4_es_generic_info.cts_delta_length) {
            nb_bits += 1;
            /*all non-first packets have the CTS written if asked*/
            if (!track_packetize_info->first_sl_in_rtp) {
                nb_bits += track_decode_info->rtp_mp4_es_generic_info.cts_delta_length;
            }
        }
        if (track_decode_info->rtp_mp4_es_generic_info.dts_delta_length) {
            nb_bits += 1 + track_decode_info->rtp_mp4_es_generic_info.dts_delta_length;
        }
        if (track_decode_info->decode_option & RTP_PCK_SELECTIVE_ENCRYPTION) {
            nb_bits += 8;
        }
        return nb_bits;
    }
    
    /*size length*/
    if (!track_decode_info->rtp_mp4_es_generic_info.constant_size) {
        nb_bits += track_decode_info->rtp_mp4_es_generic_info.size_length;
    }
    
    /*AU index*/
    if (track_packetize_info->first_sl_in_rtp) {
        if (track_decode_info->rtp_mp4_es_generic_info.index_length) {
            nb_bits += track_decode_info->rtp_mp4_es_generic_info.index_length;
        }
    } 
    else {
        if (track_decode_info->rtp_mp4_es_generic_info.index_delta_length) {
            nb_bits += track_decode_info->rtp_mp4_es_generic_info.index_delta_length;
        }
    }
    
    /*CTS Flag*/
    if (track_decode_info->rtp_mp4_es_generic_info.cts_delta_length) {
        /*CTS not written if first SL*/
        if (track_packetize_info->first_sl_in_rtp) {
            slh->composition_timestamp_flag = 0;
        }
        /*but CTS flag is always written*/
        nb_bits += 1;
    } 
    else {
        slh->composition_timestamp_flag = 0;
    }
    /*CTS*/
    if (slh->composition_timestamp_flag) {
        nb_bits += track_decode_info->rtp_mp4_es_generic_info.cts_delta_length;
    }
    
    /*DTS Flag*/
    if (track_decode_info->rtp_mp4_es_generic_info.dts_delta_length) {
        nb_bits += 1;
    } 
    else {
        slh->decoding_timestamp_flag = 0;
    }
    /*DTS*/
    if (slh->decoding_timestamp_flag) {
        nb_bits += track_decode_info->rtp_mp4_es_generic_info.dts_delta_length;
    }
    /*RAP indication*/
    if (track_decode_info->rtp_mp4_es_generic_info.random_access_indication) {
        nb_bits ++;
    }
    /*streamState indication*/
    nb_bits += track_decode_info->rtp_mp4_es_generic_info.stream_state_indication;
    
    return nb_bits;
}

/*
* Desc : write the AU header section - return the nb of BITS written for AU header
*/
UInt32 rtpfile_write_rtp_au_header(session_stream_info_t *session_stream_info, track_stream_info_t *track_stream_info, UInt32 payloadsize, UInt32 rtp_timestamp)
{
    UInt32 nb_bits = 0;
    SInt32 delta;
    track_decode_info_t *track_decode_info;
    track_packetize_info_t *track_packetize_info;
    
    if (!track_stream_info) {
        return false;
    }

    track_packetize_info = track_stream_info->track_packetize_info;
    track_decode_info = track_stream_info->track_decode_info;
    
    /*selective encryption*/
    if (track_decode_info->decode_option & RTP_PCK_SELECTIVE_ENCRYPTION) {
        UInt32 is_encrypted = 0;
        isom_write_bit_operation_int(track_packetize_info->bo_payload_header, is_encrypted, 1);
        isom_write_bit_operation_int(track_packetize_info->bo_payload_header, 0, 7);
        nb_bits = 8;
    }
    /*IV*/
    if (track_packetize_info->first_sl_in_rtp) {
        if (track_decode_info->rtp_mp4_es_generic_info.iv_length) {
            isom_write_bit_operation_long_int(track_packetize_info->bo_payload_header, track_decode_info->iv, 8*track_decode_info->rtp_mp4_es_generic_info.iv_length);
            nb_bits += 8*track_decode_info->rtp_mp4_es_generic_info.iv_length;
        }
    } 
    else if (track_decode_info->rtp_mp4_es_generic_info.iv_delta_length) {
        /*NOT SUPPORTED!!! this only applies to interleaving*/
    }
    /*key*/
    if (track_decode_info->rtp_mp4_es_generic_info.ki_length) {
        if (track_packetize_info->first_sl_in_rtp || (track_decode_info->decode_option & RTP_PCK_KEY_IDX_PER_AU)) {
            isom_write_bit_operation_int(track_packetize_info->bo_payload_header, 0, 8*track_decode_info->rtp_mp4_es_generic_info.ki_length);
            nb_bits += 8*track_decode_info->rtp_mp4_es_generic_info.ki_length;
        }
    }
    
    
    /*size length*/
    if (track_decode_info->rtp_mp4_es_generic_info.constant_size) {
        if (payloadsize != track_decode_info->rtp_mp4_es_generic_info.constant_size) {
            return nb_bits;
        }
    } 
    else if (track_decode_info->rtp_mp4_es_generic_info.size_length) {
        /*write the AU size - if not enough bytes (real-time cases) set size to 0*/
        if (track_decode_info->rtp_sl_header.accessunit_length >= (1<<track_decode_info->rtp_mp4_es_generic_info.size_length)) {
            isom_write_bit_operation_int(track_packetize_info->bo_payload_header, 0, track_decode_info->rtp_mp4_es_generic_info.size_length);
        } 
        else {
            isom_write_bit_operation_int(track_packetize_info->bo_payload_header, track_decode_info->rtp_sl_header.accessunit_length, track_decode_info->rtp_mp4_es_generic_info.size_length);
        }
        nb_bits += track_decode_info->rtp_mp4_es_generic_info.size_length;
    }
    /*AU index*/
    if (track_packetize_info->first_sl_in_rtp) {
        if (track_decode_info->rtp_mp4_es_generic_info.index_length) {
            isom_write_bit_operation_int(track_packetize_info->bo_payload_header, track_decode_info->rtp_sl_header.au_sequence_number, track_decode_info->rtp_mp4_es_generic_info.index_length);
            nb_bits += track_decode_info->rtp_mp4_es_generic_info.index_length;
        }
    } else {
        if (track_decode_info->rtp_mp4_es_generic_info.index_delta_length) {
            //check interleaving, otherwise force default (which is currently always the case)
            delta = track_decode_info->rtp_sl_header.au_sequence_number - track_packetize_info->au_count_in_packet;
            delta -= 1;
            isom_write_bit_operation_int(track_packetize_info->bo_payload_header, delta, track_decode_info->rtp_mp4_es_generic_info.index_delta_length);
            nb_bits += track_decode_info->rtp_mp4_es_generic_info.index_delta_length;
        }
    }
    
    /*CTS Flag*/
    if (track_decode_info->rtp_mp4_es_generic_info.cts_delta_length) {
        if (track_packetize_info->first_sl_in_rtp) {
            track_decode_info->rtp_sl_header.composition_timestamp_flag = 0;
            track_decode_info->rtp_sl_header.composition_timestamp = rtp_timestamp;
        }
        isom_write_bit_operation_int(track_packetize_info->bo_payload_header, track_decode_info->rtp_sl_header.composition_timestamp_flag, 1);
        nb_bits += 1;
    }
    /*CTS*/
    if (track_decode_info->rtp_sl_header.composition_timestamp_flag) {
        delta = (UInt32) track_decode_info->rtp_sl_header.composition_timestamp - rtp_timestamp;
        isom_write_bit_operation_int(track_packetize_info->bo_payload_header, delta, track_decode_info->rtp_mp4_es_generic_info.cts_delta_length);
        nb_bits += track_decode_info->rtp_mp4_es_generic_info.cts_delta_length;
    }
    /*DTS Flag*/
    if (track_decode_info->rtp_mp4_es_generic_info.dts_delta_length) {
        isom_write_bit_operation_int(track_packetize_info->bo_payload_header, track_decode_info->rtp_sl_header.decoding_timestamp_flag, 1);
        nb_bits += 1;
    }
    /*DTS*/
    if (track_decode_info->rtp_sl_header.decoding_timestamp_flag) {
        delta = (UInt32) (track_decode_info->rtp_sl_header.composition_timestamp - track_decode_info->rtp_sl_header.decoding_timestamp);
        isom_write_bit_operation_int(track_packetize_info->bo_payload_header, delta, track_decode_info->rtp_mp4_es_generic_info.dts_delta_length);
        nb_bits += track_decode_info->rtp_mp4_es_generic_info.dts_delta_length;
    }
    /*RAP indication*/
    if (track_decode_info->rtp_mp4_es_generic_info.random_access_indication) {
        isom_write_bit_operation_int(track_packetize_info->bo_payload_header, track_decode_info->rtp_sl_header.is_RAP, 1);
        nb_bits ++;
    }
    return nb_bits;
}

/*
* Desc : aac latm packetizer
*/
rs_status_t rtpfile_packetize_latm(session_stream_info_t *session_stream_info, track_stream_info_t *track_stream_info, char *data, UInt32 data_size, UInt32 duration) 
{
	UInt32 size, latm_hdr_size, i, data_offset = 0; 
	Bool16 fragmented; 
	unsigned char latm_hdr[2048];
    track_decode_info_t    *track_decode_info;
    track_packetize_info_t *track_packetize_info;
    
    if (!track_stream_info) {
        return ISOM_BAD_PARAM;
    }

    track_packetize_info = track_stream_info->track_packetize_info;
    track_decode_info = track_stream_info->track_decode_info;

	if (track_decode_info->rtp_sl_header.access_unit_start_flag == 1) {
		if (!data) {
            /*
			rtpfile_flush_latm(track_decode_info);
			track_decode_info->rtp_sl_header.access_unit_start_flag = 0;
			return true; 
            */
            return ISOM_BAD_PARAM;
		}
	    /*
		if ((track_decode_info->decode_option & RTP_PCK_USE_MULTI) && track_decode_info->max_ptime) {
            if ((UInt32) track_decode_info->rtp_sl_header.composition_timestamp + duration >= track_stream_info->rtp_packet_info.rtp_header_info.timestamp + track_decode_info->max_ptime) {
				rtpfile_flush_latm(track_decode_info);
            }
		}
        */
		/*compute max size for frame, flush current if this doesn't fit*/
		latm_hdr_size = (data_size / 255) + 1; 
		if (latm_hdr_size + data_size + track_packetize_info->bytes_in_rtp_payload > track_packetize_info->usable_mtu_payload_size) {
			rtpfile_flush_latm(session_stream_info, track_stream_info);
		}
		
		track_packetize_info->readbytes_leftsize_in_au = data_size;
		data_offset = 0;
		fragmented = 0;
	}
    else {
        data_offset = data_size - track_packetize_info->readbytes_leftsize_in_au;
    }
    
	track_decode_info->rtp_sl_header.access_unit_start_flag = 0;
	latm_hdr_size = (track_packetize_info->readbytes_leftsize_in_au / 255) + 1; 

    /*fragmenting*/
	if (latm_hdr_size + track_packetize_info->readbytes_leftsize_in_au > track_packetize_info->usable_mtu_payload_size) {
		fragmented = 1;
		latm_hdr_size = (track_packetize_info->usable_mtu_payload_size / 255) + 1; 
		size = track_packetize_info->usable_mtu_payload_size - latm_hdr_size; 
		track_stream_info->rtp_packet_info.rtp_header_info.marker = 0; 
	}
	/*last fragment or full AU*/
	else { 
		fragmented = 0;
		size = track_packetize_info->readbytes_leftsize_in_au; 
		track_stream_info->rtp_packet_info.rtp_header_info.marker = 1; 
	} 
	track_packetize_info->readbytes_leftsize_in_au -= size; 
	
	/*create new RTP Packet if needed*/ 
	if (!track_packetize_info->bytes_in_rtp_payload) {
		track_stream_info->rtp_packet_info.rtp_header_info.sequence_number += 1; 
		track_stream_info->rtp_packet_info.rtp_header_info.timestamp = (UInt32) track_decode_info->rtp_sl_header.composition_timestamp; 
//		track_decode_info->on_new_packet(track_decode_info->cbk_obj, &track_stream_info->rtp_packet_info.rtp_header_info); 
	}

	/* compute AudioMuxUnit header */ 
	latm_hdr_size = (size / 255) + 1; 
	//latm_hdr = (unsigned char *)malloc( sizeof(char) * latm_hdr_size); 
	memset(latm_hdr, 0x00, sizeof(latm_hdr));
    for (i=0; i<latm_hdr_size-1; i++) {
        latm_hdr[i] = 255; 
    }

	latm_hdr[latm_hdr_size-1] = (unsigned char)(size % 255); 
	
	/*add LATM header IN ORDER in case we aggregate audioMuxElements in RTP*/ 
	rtpfile_set_rtp_payload_data(session_stream_info, track_stream_info, (char*) latm_hdr, latm_hdr_size, 0);
	track_packetize_info->bytes_in_rtp_payload += latm_hdr_size;
	//free(latm_hdr);
	

	rtpfile_set_rtp_payload_data(session_stream_info, track_stream_info, data+data_offset, size, 0); 

	track_packetize_info->bytes_in_rtp_payload += size;

	/*fragmented AU, always flush packet*/
    if (!track_stream_info->rtp_packet_info.rtp_header_info.marker) {
        rtpfile_flush_latm(session_stream_info, track_stream_info);
    }

	
	/*if the AU has been fragmented or we don't use RTP aggregation, flush*/
	if (track_packetize_info->readbytes_leftsize_in_au == 0 || track_packetize_info->readbytes_leftsize_in_au > data_size) {
        if (!(track_decode_info->decode_option & RTP_PCK_USE_MULTI)) {
            fragmented = 1;
        }
        if (fragmented) {
            rtpfile_flush_latm(session_stream_info, track_stream_info);
        }
		track_decode_info->rtp_sl_header.access_unit_start_flag = 1;
		track_decode_info->rtp_sl_header.access_unit_end_flag   = 0;
		track_packetize_info->readbytes_leftsize_in_au		   = 0;
	}
	
    session_stream_info->result_code = ISOM_OK;
	return ISOM_OK; 
}

/*
* Desc : aac latm packet done
*/
void rtpfile_flush_latm(session_stream_info_t *session_stream_info, track_stream_info_t *track_stream_info)
{
    track_decode_info_t *track_decode_info;
    track_packetize_info_t *track_packetize_info;
    
    track_packetize_info = track_stream_info->track_packetize_info;
    track_decode_info = track_stream_info->track_decode_info;

    if (track_packetize_info->bytes_in_rtp_payload) {
        rtpfile_set_rtp_packet_send_info(session_stream_info, track_stream_info, &track_stream_info->rtp_packet_info.rtp_header_info); 
        track_packetize_info->bytes_in_rtp_payload = 0;
    }
}

/*
* Desc : avc packetizer
*/
rs_status_t rtpfile_packetize_avc(session_stream_info_t *session_stream_info, track_stream_info_t *track_stream_info, char *nalu, UInt32 nalu_size, UInt8 is_au_end)
{
    UInt32 do_flush, size, nal_type, offset;
	char shdr[2];
	char stap_hdr;
    
    track_packetize_info_t *track_packetize_info;
    track_decode_info_t    *track_decode_info;

    if (!track_stream_info) {
        return ISOM_BAD_PARAM;
    }
    
    track_packetize_info = track_stream_info->track_packetize_info;
    track_decode_info = track_stream_info->track_decode_info;

	/* nal size check(mp4box) */
	if (track_packetize_info->nalunit_size_info) {
        nalu = nalu + track_packetize_info->nalunit_size_info;
        nalu_size -= track_packetize_info->nalunit_size_info;
	}

	if ( track_decode_info->rtp_sl_header.access_unit_start_flag == 1 ) {
        //2009.09.03 현재 FU-A 방식만 지원한다. bytes_in_rtp_payload 변수는 항상 0 이며 패킷을 모아서 전송안한다.
		do_flush = 0;
        if (!nalu) {
			do_flush = 1;
        }
        /*we only do STAP or SINGLE modes*/
        else if (track_decode_info->rtp_sl_header.access_unit_start_flag) {
			do_flush = 1;
        }
        /*we must NOT fragment a NALU*/
        else if ( track_packetize_info->bytes_in_rtp_payload + nalu_size >= track_packetize_info->usable_mtu_payload_size ) {
			do_flush = 2;
        }

        //FU-A 방식만 지원.  bytes_in_rtp_payload == 0 아래 if문 내용은 사용 안함.
        if (track_packetize_info->bytes_in_rtp_payload && do_flush) {
			track_stream_info->rtp_packet_info.rtp_header_info.marker = (do_flush==1) ? 1 : 0;
			rtpfile_set_rtp_packet_send_info(session_stream_info, track_stream_info, &track_stream_info->rtp_packet_info.rtp_header_info);
			track_packetize_info->bytes_in_rtp_payload = 0;
		}

		if (!nalu) {
			track_packetize_info->readbytes_leftsize_in_au = 0;
			track_decode_info->rtp_sl_header.access_unit_start_flag = 1;
			track_decode_info->rtp_sl_header.access_unit_end_flag = 0;
			return ISOM_OK;
		}

		/*need a new RTP packet*/
		if (!track_packetize_info->bytes_in_rtp_payload) {
			track_stream_info->rtp_packet_info.rtp_header_info.timestamp = (UInt32) track_decode_info->rtp_sl_header.composition_timestamp;
			track_stream_info->rtp_packet_info.rtp_header_info.sequence_number += 1;
//			track_decode_info->on_new_packet(track_decode_info->cbk_obj, &track_stream_info->rtp_packet_info.rtp_header_info);
			track_packetize_info->avc_non_idr = 1;
		}
		/*check NAL type to see if disposable or not*/
		nal_type = nalu[0] & 0x1F;
		switch (nal_type) {
		case AVC_NALU_NON_IDR_SLICE:
		case AVC_NALU_ACCESS_UNIT:
		case AVC_NALU_END_OF_SEQ:
		case AVC_NALU_END_OF_STREAM:
		case AVC_NALU_FILLER_DATA:
			break;
		default:
			track_packetize_info->avc_non_idr = 0;
			break;
		}

		/*FU payload doesn't have the NAL hdr*/
		track_packetize_info->readbytes_leftsize_in_au = nalu_size - 1;
		offset = 1;
	}
	else
	{
		offset = nalu_size - track_packetize_info->readbytes_leftsize_in_au;		
	}

	/*at this point we're sure the NALU fits in current packet OR must be splitted*/

	/*pb: we don't know if next NALU from this AU will be small enough to fit in the packet, so we always
	go for stap...*/
	if (track_packetize_info->bytes_in_rtp_payload+nalu_size < track_packetize_info->usable_mtu_payload_size) {
		Bool16 use_stap = 1;
		track_decode_info->rtp_sl_header.access_unit_start_flag = 0;
		/*if this is the AU end and no NALU in packet, go for single mode*/
        //FU-A 방식만 지원.  bytes_in_rtp_payload == 0 아래  use_stap 사용 안함. 
        if (is_au_end && !track_packetize_info->bytes_in_rtp_payload) {
            use_stap = 0;
        }
		
		if (use_stap) {
			/*declare STAP-A NAL*/
			if (!track_packetize_info->bytes_in_rtp_payload) {
				/*copy over F and NRI from first nal in packet and assign type*/
				stap_hdr = (nalu[0] & 0xE0) | 24;
				rtpfile_set_rtp_payload_data(session_stream_info, track_stream_info, (char *) &stap_hdr, 1, 0);
				track_packetize_info->bytes_in_rtp_payload = 1;
			}
			/*add NALU size*/
			shdr[0] = (char)nalu_size>>8;
			shdr[1] = (char)nalu_size&0x00ff;
			rtpfile_set_rtp_payload_data(session_stream_info, track_stream_info, (char *)shdr, 2, 0);
			track_packetize_info->bytes_in_rtp_payload += 2;
		}
		/*add data*/
		rtpfile_set_rtp_payload_data(session_stream_info, track_stream_info, nalu, nalu_size, 0);

		track_packetize_info->bytes_in_rtp_payload += nalu_size;

		if (is_au_end) {
			track_stream_info->rtp_packet_info.rtp_header_info.marker = 1;
			rtpfile_set_rtp_packet_send_info(session_stream_info, track_stream_info, &track_stream_info->rtp_packet_info.rtp_header_info);
			track_packetize_info->bytes_in_rtp_payload = 0;
			track_packetize_info->readbytes_leftsize_in_au = 0;
		}
	}
	/*fragmentation unit*/
	else {
		track_decode_info->rtp_sl_header.access_unit_start_flag = 0;
		if (2 + track_packetize_info->readbytes_leftsize_in_au > track_packetize_info->usable_mtu_payload_size) {
			size = track_packetize_info->usable_mtu_payload_size - 2;
		} else {
			size = track_packetize_info->readbytes_leftsize_in_au;
			track_decode_info->rtp_sl_header.access_unit_end_flag = 1;
		}
		
		/*copy over F and NRI from nal in packet and assign type*/
		shdr[0] = (nalu[0] & 0xE0) | 28; //FU-A RFC3984
		/*copy over NAL type from nal and set s//tart bit and end bit*/
		shdr[1] = (nalu[0] & 0x1F);
		/*start bit*/
        if (offset==1) {
            shdr[1] |= 0x80;
        }
		/*end bit*/
        else if (size == track_packetize_info->readbytes_leftsize_in_au) {
            shdr[1] |= 0x40;
        }

		rtpfile_set_rtp_payload_data(session_stream_info, track_stream_info, (char *)shdr, 2, 0);

		rtpfile_set_rtp_payload_data(session_stream_info, track_stream_info, nalu+offset, size, 0);

		offset += size;
		track_packetize_info->readbytes_leftsize_in_au -= size;

		/*flush no matter what (FUs cannot be agreggated)*/
		track_stream_info->rtp_packet_info.rtp_header_info.marker = track_packetize_info->readbytes_leftsize_in_au ? 0 : 1;
		rtpfile_set_rtp_packet_send_info(session_stream_info, track_stream_info, &track_stream_info->rtp_packet_info.rtp_header_info);
		track_packetize_info->bytes_in_rtp_payload = 0;

		if (track_packetize_info->readbytes_leftsize_in_au) {
			track_stream_info->rtp_packet_info.rtp_header_info.timestamp = (UInt32) track_decode_info->rtp_sl_header.composition_timestamp;
			track_stream_info->rtp_packet_info.rtp_header_info.sequence_number += 1;
//			track_decode_info->on_new_packet(track_decode_info->cbk_obj, &track_stream_info->rtp_packet_info.rtp_header_info);
		}
	}

	if (track_packetize_info->readbytes_leftsize_in_au == 0 || track_packetize_info->readbytes_leftsize_in_au > nalu_size) {
		track_decode_info->rtp_sl_header.access_unit_start_flag = 1;
		track_decode_info->rtp_sl_header.access_unit_end_flag   = 0;
		track_packetize_info->readbytes_leftsize_in_au		   = 0;
	}
    session_stream_info->result_code = ISOM_OK;
    return ISOM_OK;
}

