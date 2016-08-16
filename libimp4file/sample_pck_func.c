#include "os_header.h"
#include "rtp_streamer.h"
#include "sample_pck_func.h"

/*
* Desc : access uint initialize, Sample Object
*/

rs_status_t rtpfile_init_access_unit_info(track_stream_info_t *track_stream_info)
{
    rs_status_t               result_code = ISOM_UNKNOWN;
    access_unit_info_t        *tmp_au = NULL;
    
    if (track_stream_info->track_decode_info->au_max_size == 0) {
        result_code = ISOM_INVALID_TRACK_DECODE_INFO;
        return result_code;
    }
    
    tmp_au = (access_unit_info_t *) malloc(sizeof(access_unit_info_t));
    if (tmp_au == NULL) {
        result_code = ISOM_MEMORY_ERROR;
        return result_code;
    }
    memset(tmp_au, 0x00, sizeof(access_unit_info_t)); 	

    tmp_au->data = (char *) malloc(track_stream_info->track_decode_info->au_max_size);
    if (tmp_au->data == NULL) {
        result_code = ISOM_MEMORY_ERROR;
        return result_code;
    }
    
    track_stream_info->au_info = tmp_au;
    
    result_code = ISOM_OK;
    return result_code;
}



/*
* Desc : sample_number까지의 dts 
*/
rs_status_t rtpfile_get_sample_dts_from_stts(track_stream_info_t *track_stream_info, isom_box_stts_t *stts, UInt32 sample_number, UInt64 *dts)
{
    rs_status_t result_code = ISOM_UNKNOWN;
    UInt32 i, j, count;
    UInt32 sample_count, sample_duration;
    track_sample_control_info_t *track_sample_control_info = track_stream_info->track_sample_control_info;
    
    if (!track_stream_info) {
        result_code = ISOM_BAD_PARAM;
        return result_code;
    }

    if (!stts || !sample_number) {
        result_code = ISOM_BAD_PARAM;
        return result_code;
    }    
    
    (*dts) = 0;

    //use our buffer
    count = stts->numentries;

    if (track_sample_control_info->stts_first_samp_in_entry 
        && (track_sample_control_info->stts_first_samp_in_entry <= sample_number)
        && (track_sample_control_info->stts_current_entry_index < count) ) {        
        i = track_sample_control_info->stts_current_entry_index;

    } else {
        i = track_sample_control_info->stts_current_entry_index = 0;
        track_sample_control_info->stts_first_samp_in_entry = 1;
        track_sample_control_info->stts_current_dts = 0;
    }
    
    for (; i < count; i++) {        
        memcpy(&sample_count, stts->time_to_sample_table + (i * 8), 4);
        sample_count = ntohl(sample_count);
        memcpy(&sample_duration, stts->time_to_sample_table + (i * 8) + 4, 4);
        sample_duration = ntohl(sample_duration);
        //in our entry
        if (sample_count + track_sample_control_info->stts_first_samp_in_entry >= 1 + sample_number) {
            j = sample_number - track_sample_control_info->stts_first_samp_in_entry;
            goto found;
        }
        
        //update our buffer
        track_sample_control_info->stts_current_dts += sample_count * sample_duration;
        track_sample_control_info->stts_current_entry_index += 1;
        track_sample_control_info->stts_first_samp_in_entry += sample_count;
    }
    
    if (i == count) {
        (*dts) = track_sample_control_info->stts_current_dts;
    }
    result_code = ISOM_OK;
    return result_code;
    
found:
    (*dts) = track_sample_control_info->stts_current_dts + j * (UInt64) sample_duration;
    
    if (track_sample_control_info->stts_first_samp_in_entry == 1) {
        track_sample_control_info->stts_first_samp_in_entry = 1;
    }

    result_code = ISOM_OK;
    return result_code;
}

/*
* Desc : sample_number 의 cts 값 
*/
rs_status_t rtpfile_get_sample_cts_from_ctts(track_stream_info_t *track_stream_info, isom_box_ctts_t *ctts, UInt32 sample_number, UInt32 *cts_offset)
{
    rs_status_t result_code = ISOM_UNKNOWN;
    UInt32 i, count;
    UInt32 sample_count, sample_duration;
    track_sample_control_info_t *track_sample_control_info = track_stream_info->track_sample_control_info;
    
    (*cts_offset) = 0;

    //test on sample_number is done before
    if (!ctts || !sample_number) {
        result_code = ISOM_BAD_PARAM;
        return result_code;
    }

    count = ctts->numentries;

    if (track_sample_control_info->ctts_first_samp_in_entry 
        && (track_sample_control_info->ctts_first_samp_in_entry < sample_number) 
        && (track_sample_control_info->ctts_current_entry_index < count)) {
        i = track_sample_control_info->ctts_current_entry_index;
    } else {
        i = track_sample_control_info->ctts_current_entry_index = 0;
        track_sample_control_info->ctts_first_samp_in_entry = 1;
        track_sample_control_info->ctts_current_entry_index = 0;        
	}   
    
    for( ; i < count; i++ ) {
        //
        // Copy this sample count and duration.
        memcpy(&sample_count, ctts->time_to_sample_table + (i * 8), 4);
        sample_count = ntohl(sample_count);
        memcpy(&sample_duration, ctts->time_to_sample_table + (i * 8) + 4, 4);
        sample_duration = ntohl(sample_duration);
        
        if (sample_number < track_sample_control_info->ctts_first_samp_in_entry + sample_count) {
            break;
        }
        //update our buffer
        track_sample_control_info->ctts_current_entry_index += 1;
		track_sample_control_info->ctts_first_samp_in_entry += sample_count;
    }
    if (sample_number >= track_sample_control_info->ctts_first_samp_in_entry + sample_count) {
        return ISOM_OK;
    }

	(*cts_offset) = sample_duration;

    return ISOM_OK;
}

/*
* Desc : sample_number의 size
*/
rs_status_t rtpfile_get_sample_size_from_stsz(track_stream_info_t *track_stream_info, isom_box_stsz_t *stsz, UInt32 sample_number, UInt32 *size)
{
    if (!stsz || !sample_number || sample_number > stsz->numentries) {
        return ISOM_BAD_PARAM;
    }
    if (stsz->common_sample_size != 0) {
        (*size) = stsz->common_sample_size;
    }
    else {
        (*size) = ntohl(stsz->table[sample_number - 1]);
    }
    return ISOM_OK;
}

/*
* Desc : 
*/
rs_status_t rtpfile_get_computed_dts(track_stream_info_t *track_stream_info, isom_box_trak_t *box_trak, UInt64 dts, UInt32 rap_find_t, UInt64 *rap_dts, UInt32 *sample_number)
{
    rs_status_t result_code = ISOM_UNKNOWN;
    UInt32 prev_rap=0, next_rap=0;
    UInt8 is_rap;
    track_sample_control_info_t *track_sample_control_info = track_stream_info->track_sample_control_info;

    (*rap_dts) = dts;
    
    if (!box_trak) {
        result_code = ISOM_COMPUTE_DTS_BAD_PARAM;
        return result_code;
    }
    if ((result_code = rtpfile_get_sample_number_around_dts_from_stts(track_stream_info, box_trak->box_mdia->box_minf->box_stbl->box_stts, sample_number, dts)) != ISOM_OK) {
        *sample_number = 0;
        return ISOM_COMPUTE_DTS_AROUND_STTS_FAIL;
    }
    
    if (rap_find_t > 0) {
	    if (box_trak->box_mdia->box_minf->box_stbl->box_stss) {
            // 3초 이내의 i-frame 찾기
            result_code = rtpfile_get_sample_random_access_point_from_stss(track_stream_info, box_trak->box_mdia->box_minf->box_stbl->box_stss, *sample_number, &is_rap, &prev_rap, &next_rap);
        }
        if(is_rap == 0) {
            result_code = rtpfile_get_sample_dts_from_stts(track_stream_info, box_trak->box_mdia->box_minf->box_stbl->box_stts, prev_rap, rap_dts);
            if (result_code != ISOM_OK) {
                *sample_number = 0;
                return ISOM_COMPUTE_DTS_RAP_FIND_STTS;
            }
            if ((rap_find_t * 1000) >= (dts - (*rap_dts))) {
                *sample_number = prev_rap;
                return ISOM_OK;
            }
        }
    }
    
    result_code = rtpfile_get_sample_dts_from_stts(track_stream_info, box_trak->box_mdia->box_minf->box_stbl->box_stts, *sample_number, rap_dts);
    if (result_code != ISOM_OK) {
        result_code = ISOM_COMPUTE_DTS_STTS;
    }
    return result_code;
}

/*
* Desc :Get the sample of a DTS  요청 dur >= DTS(dur의 근사값) (sample number) 
*/
rs_status_t rtpfile_get_sample_number_around_dts_from_stts(track_stream_info_t *track_stream_info, isom_box_stts_t *stts, UInt32 *sample_number, UInt64 dur)
{
    rs_status_t result_code = ISOM_UNKNOWN;
    UInt32 i, j, count;
    UInt32 sample_count, sample_duration;
    track_sample_control_info_t *track_sample_control_info = track_stream_info->track_sample_control_info;
        
    (*sample_number) = 0;
    
    if (!stts) {
        return ISOM_BAD_PARAM;
    }
    
    //use our buffer
    count = stts->numentries;
    
    i = track_sample_control_info->stts_current_entry_index = 0;
    track_sample_control_info->stts_first_samp_in_entry = 1;
    track_sample_control_info->stts_current_dts = 0;
    
    if( dur == track_sample_control_info->stts_current_dts ) {
        goto found;
    }
    
    for (; i < count; i++) {
        memcpy(&sample_count, stts->time_to_sample_table + (i * 8), 4);
        sample_count = ntohl(sample_count);
        memcpy(&sample_duration, stts->time_to_sample_table + (i * 8) + 4, 4);
        sample_duration = ntohl(sample_duration);

        for(j=0; j < sample_count; j++) {
            track_sample_control_info->stts_current_dts += sample_duration;
            track_sample_control_info->stts_first_samp_in_entry += 1;

            if ( dur <= track_sample_control_info->stts_current_dts ) {
                track_sample_control_info->stts_current_dts -= sample_duration;
                track_sample_control_info->stts_first_samp_in_entry -= 1;
                goto found;
            }
        }
        track_sample_control_info->stts_current_entry_index += 1;
    }
    
    return ISOM_ERROR;
    
found:
    (*sample_number) = track_sample_control_info->stts_first_samp_in_entry;
    
    return ISOM_OK;
}

/*
* Desc :Set the RAP flag of a sample
*/
rs_status_t rtpfile_get_sample_random_access_point_from_stss(track_stream_info_t *track_stream_info, isom_box_stss_t *stss, UInt32 sample_number, UInt8 *is_rap, UInt32 *prev_rap, UInt32 *next_rap) {
    rs_status_t result_code = ISOM_UNKNOWN;
    UInt32 i;
    track_sample_control_info_t *track_sample_control_info = track_stream_info->track_sample_control_info;
    
    if (prev_rap) *prev_rap = 0;
    if (next_rap) *next_rap = 0;
    
    (*is_rap) = 0;
    if (!stss || !sample_number) {
        result_code = ISOM_BAD_PARAM;
        return result_code;
    }
    
    if (track_sample_control_info->stss_last_sync_sample && (track_sample_control_info->stss_last_sync_sample < sample_number) ) {
        i = track_sample_control_info->stss_last_sample_index;
    } else {
        i = 0;
    }
    for (; i < stss->numentries; i++) {
        //get the entry
        if (stss->table[i] == sample_number) {
            //update the buffer
            track_sample_control_info->stss_last_sync_sample = sample_number;
            track_sample_control_info->stss_last_sample_index = i;
            (*is_rap) = 1;
        }
        else if (stss->table[i] > sample_number) {
            if (next_rap) {
                *next_rap = stss->table[i];
            }
            result_code = ISOM_OK;
            return result_code;
        }
        if (prev_rap) {
            *prev_rap = stss->table[i];
        }
    }
    result_code = ISOM_OK;
    return result_code;
}

/*
* Desc : sample number의 chunk num과 offset 값을 구한다.
*/
Bool16 rtpfile_get_chunknumber_and_offset_from_stco(track_stream_info_t *track_stream_info, isom_box_trak_t *box_trak, UInt32 sample_number, UInt64 *offset, UInt32 *chunk_number, UInt32 *desc_index, UInt8 *is_edited)
{
    rs_status_t result_code = ISOM_UNKNOWN;
	UInt32 i, j, k, offset_inchunk, size, count;
    UInt32 first_chunk, samples_perchunk, sample_description_index;
	isom_box_stco_t *stco;
    isom_box_stsc_t *stsc;
    track_sample_control_info_t *track_sample_control_info = track_stream_info->track_sample_control_info;

	(*offset) = 0;
	(*chunk_number) = (*desc_index) = 0;
	(*is_edited) = 0;
    if (!box_trak || !sample_number) {
        return false;
    }

    stco = box_trak->box_mdia->box_minf->box_stbl->box_stco;
    stsc = box_trak->box_mdia->box_minf->box_stbl->box_stsc;
	count = box_trak->box_mdia->box_minf->box_stbl->box_stsc->numentries;

	if (count == box_trak->box_mdia->box_minf->box_stbl->box_stsz->numentries) {
        if (sample_number > stsc->numentries) {
            return false;
        }
        memcpy(&first_chunk, stsc->sample_to_chunk_table + ((sample_number-1) * 12) + 0, 4);
        first_chunk = ntohl(first_chunk);
        memcpy(&samples_perchunk, stsc->sample_to_chunk_table + ((sample_number-1) * 12) + 4, 4);
        samples_perchunk = ntohl(samples_perchunk);
        memcpy(&sample_description_index, stsc->sample_to_chunk_table + ((sample_number-1) * 12) + 8, 4);
        sample_description_index = ntohl(sample_description_index);

		(*desc_index) = sample_description_index;
		(*chunk_number) = sample_number;
		
		if ( 4 == stco->offset_size) {
			(*offset) = (UInt64) ntohl(((UInt32 *) stco->table)[sample_number-1]);
		} else {
			(*offset) = (UInt64) ut_ntoh64(((UInt64 *) stco->table)[sample_number-1]);
		}
		return true;
	}

	//check our buffer
	if (track_sample_control_info->stsc_first_sample_in_current_chunk &&
		(track_sample_control_info->stsc_first_sample_in_current_chunk < sample_number)) {	
        
		i = track_sample_control_info->stsc_current_index;

        memcpy(&first_chunk, stsc->sample_to_chunk_table + (i * 12) + 0, 4);
        first_chunk = ntohl(first_chunk);
        memcpy(&samples_perchunk, stsc->sample_to_chunk_table + (i* 12) + 4, 4);
        samples_perchunk = ntohl(samples_perchunk);
        memcpy(&sample_description_index, stsc->sample_to_chunk_table + (i * 12) + 8, 4);
        sample_description_index = ntohl(sample_description_index);

		rtpfile_get_ghost_num(track_stream_info, stco, stsc, i, count, first_chunk);
		k = track_sample_control_info->stsc_current_chunk;
	} 
    else { 

		i = 0;
		track_sample_control_info->stsc_current_index = 0;
		track_sample_control_info->stsc_current_chunk = 1;
		track_sample_control_info->stsc_first_sample_in_current_chunk = 1;

        memcpy(&first_chunk, stsc->sample_to_chunk_table + 0, 4);
        first_chunk = ntohl(first_chunk);
        memcpy(&samples_perchunk, stsc->sample_to_chunk_table + 4, 4);
        samples_perchunk = ntohl(samples_perchunk);
        memcpy(&sample_description_index, stsc->sample_to_chunk_table + 8, 4);
        sample_description_index = ntohl(sample_description_index);

		rtpfile_get_ghost_num(track_stream_info, stco, stsc, 0, count, first_chunk);
		k = track_sample_control_info->stsc_current_chunk;
	}

	//first get the chunk
	for (; i < count; i++) {
		//browse from the current chunk we're browsing from index 1
		for (; k <= track_sample_control_info->stsc_ghost_number; k++) {
			//browse all the samples in this chunk
			for (j = 0; j < samples_perchunk; j++) {
				//ok, this is our sample
                if (track_sample_control_info->stsc_first_sample_in_current_chunk + j == sample_number ) {
					goto sample_found;
                }
			}
			// next_chunk(ghost) find setting
			track_sample_control_info->stsc_first_sample_in_current_chunk += samples_perchunk;
			track_sample_control_info->stsc_current_chunk++;
		}
		//not in this entry, get the next entry if not the last one
		if (i+1 != count) {
            memcpy(&first_chunk, stsc->sample_to_chunk_table + ((i+1) * 12) + 0, 4);
            first_chunk = ntohl(first_chunk);
            memcpy(&samples_perchunk, stsc->sample_to_chunk_table + ((i+1) * 12) + 4, 4);
            samples_perchunk = ntohl(samples_perchunk);
            memcpy(&sample_description_index, stsc->sample_to_chunk_table + ((i+1) * 12) + 8, 4);
            sample_description_index = ntohl(sample_description_index);

			//update the GhostNumber
			rtpfile_get_ghost_num(track_stream_info, stco, stsc, i+1, count, first_chunk);
			//update the entry in our buffer
			track_sample_control_info->stsc_current_index = i+1;
			track_sample_control_info->stsc_current_chunk = 1;
			k = 1;
		}
	}
	//if we get here, gasp, the sample was not found
	return false;

sample_found:

	(*desc_index) = sample_description_index;
	(*chunk_number) = first_chunk + track_sample_control_info->stsc_current_chunk - 1; //current_chunk 시작값이 1이므로 -1
	(*is_edited) = 0;

	//ok, get the size of all the previous sample
	offset_inchunk = 0;

	//that's the size of our offset in the chunk
	for (i = track_sample_control_info->stsc_first_sample_in_current_chunk; i < sample_number; i++) {
        if ((result_code = rtpfile_get_sample_size_from_stsz(track_stream_info, box_trak->box_mdia->box_minf->box_stbl->box_stsz, i, &size)) != ISOM_OK) {
            return false;
        }
		offset_inchunk += size; 
	}
	
	//now get the chunk

    if ( 4 == stco->offset_size) {
        if (stco->numentries < (*chunk_number) ) {
            return false;
        }
        (*offset) = (UInt64) ntohl(((UInt32 *) stco->table)[(*chunk_number)-1]) + (UInt64) offset_inchunk;
    } else {
        if (stco->numentries < (*chunk_number)) {
            return false;
        }
        (*offset) = (UInt64) ut_ntoh64(((UInt64 *) stco->table)[(*chunk_number)-1]) + (UInt64) offset_inchunk;
	}
    return true;

}

/*
* Desc : 동일한 chunk entry를 사용하는 갯수 
*/
void rtpfile_get_ghost_num(track_stream_info_t *track_stream_info, isom_box_stco_t *stco, isom_box_stsc_t *stsc, UInt32 entry_index, UInt32 stsc_entry_count, UInt32 first_chunk)
{
    UInt32 ghost_num = 1;
    UInt32 next_chunk;
    track_sample_control_info_t *track_sample_control_info = track_stream_info->track_sample_control_info;

    if (entry_index+1 == stsc_entry_count) { // stsc 의 마지막 entry firstchunk 값과 stco numentries 비교로 ghost값 검색  
        ghost_num = (stco->numentries > first_chunk) ? (1 + stco->numentries - first_chunk) : 1; 
        track_sample_control_info->stsc_ghost_number = ghost_num;
    }
    else {
        memcpy(&next_chunk, stsc->sample_to_chunk_table + ((entry_index+1) * 12) + 0, 4);
        next_chunk = ntohl(next_chunk);

        ghost_num = (next_chunk > first_chunk) ? (next_chunk - first_chunk) : 1;
    }
    track_sample_control_info->stsc_ghost_number = ghost_num;
}

/*
* Desc : sample number 에 해당되는 duration
*/
UInt32 rtpfile_get_sample_duration(track_stream_info_t *track_stream_info, isom_box_trak_t *box_trak, UInt32 sample_number)
{
    rs_status_t   result_code = ISOM_UNKNOWN;
    UInt64 dur;
	UInt64 dts;
    track_sample_control_info_t *track_sample_control_info = track_stream_info->track_sample_control_info;
	
    if (!box_trak || !sample_number) {
        return 0;
    }

	rtpfile_get_sample_dts_from_stts(track_stream_info, box_trak->box_mdia->box_minf->box_stbl->box_stts, sample_number, &dur);
	if (sample_number == box_trak->box_mdia->box_minf->box_stbl->box_stsz->numentries) {
		return (UInt32) (box_trak->box_mdia->box_mdhd->duration - dur);
	} 
	
	result_code = rtpfile_get_sample_dts_from_stts(track_stream_info, box_trak->box_mdia->box_minf->box_stbl->box_stts, sample_number + 1, &dts);
    if (result_code != ISOM_OK) {
        return 0;
    }
	return (UInt32) (dts - dur);
}


/*
* Desc : access uint initialize
*/
rs_status_t rtpfile_initialize_access_unit_info(track_stream_info_t *track_stream_info)
{
    rs_status_t               result_code = ISOM_UNKNOWN;
    access_unit_info_t     *tmp_au = NULL;
    
    if (track_stream_info->track_decode_info->au_max_size == 0) {
        result_code = ISOM_INVALID_TRACK_DECODE_INFO;
        return result_code;
    }
    
    tmp_au = (access_unit_info_t *) malloc(sizeof(access_unit_info_t));
    if (tmp_au == NULL) {
        result_code = ISOM_MEMORY_ERROR;
        return result_code;
    }
    memset(tmp_au, 0x00, sizeof(access_unit_info_t)); 	

    tmp_au->data = (char *) malloc(track_stream_info->track_decode_info->au_max_size);
    if (tmp_au->data == NULL) {
        result_code = ISOM_MEMORY_ERROR;
        return result_code;
    }
    
    track_stream_info->au_info = tmp_au;
    
    result_code = ISOM_OK;
    return result_code;
}  

/*
* Desc : rtp packet payload setting
*/
Bool16 rtpfile_set_rtp_payload_data(session_stream_info_t *session_stream_info, track_stream_info_t *track_stream_info, char *data, UInt32 data_size, Bool16 is_head)
{
    if (!data ||!data_size) {
        return false;
    }

    if (!track_stream_info->rtp_packet_info.rtp_payload_size) {
        //		track_stream_info->rtp_packet_info.rtp_payload_data = (char*) malloc(data_size);
        memcpy(track_stream_info->rtp_packet_info.rtp_payload_data, data, data_size);
        track_stream_info->rtp_packet_info.rtp_payload_size = data_size;
    } else {
        //		track_stream_info->rtp_packet_info.rtp_payload_data = (char*) realloc(track_stream_info->rtp_packet_info.rtp_payload_data, track_stream_info->rtp_packet_info.rtp_payload_size + data_size);
        if (!is_head) {
            memcpy(track_stream_info->rtp_packet_info.rtp_payload_data+track_stream_info->rtp_packet_info.rtp_payload_size, data, data_size);
        } else {
            memmove(track_stream_info->rtp_packet_info.rtp_payload_data+data_size, track_stream_info->rtp_packet_info.rtp_payload_data, track_stream_info->rtp_packet_info.rtp_payload_size);
            memcpy(track_stream_info->rtp_packet_info.rtp_payload_data, data, data_size);
        }
        track_stream_info->rtp_packet_info.rtp_payload_size += data_size;
    }
    
    return true;    
}

/*
* Desc : rtp header(12bytes) + rtp payload = rtp packet 생성 
*/
Bool16 rtpfile_set_rtp_packet_send_info(session_stream_info_t *session_stream_info, track_stream_info_t *track_stream_info, rtp_header_info_t *rtp_header_info)
{
    UInt64 au_dts_msec;
    UInt32 start, track_timescale;
    UInt32 rtp_payload_size;
    UInt8  tmp_u8;
    char *rtp_payload_data;
    rtp_packet_send_info_t *rtp_packet_send_info;
    
    rtp_payload_data = track_stream_info->rtp_packet_info.rtp_payload_data;
    rtp_payload_size = track_stream_info->rtp_packet_info.rtp_payload_size;
    rtp_packet_send_info = session_stream_info->rtp_packet_send_info;

    //we don't support multiple CSRC now. Only one source (the server) is allowed
    if (!rtp_packet_send_info || !rtp_header_info 
        || !rtp_packet_send_info->rtp_packet_data || !rtp_payload_data 
        || (rtp_header_info->csrc_count)) {
        return false;
    }
    
    if (12 + rtp_payload_size > session_stream_info->mtu_size) {
        return false; 
    }    
  
    tmp_u8 = 0;
    tmp_u8 |= (rtp_header_info->version << 6) + (rtp_header_info->padding << 5) 
                + (rtp_header_info->extension << 4) + (rtp_header_info->csrc_count);
    ((UInt8 *)rtp_packet_send_info->rtp_packet_data)[0] = tmp_u8;

    tmp_u8 = 0;
    tmp_u8 |= (rtp_header_info->marker << 7) + (rtp_header_info->payload_type);
    ((UInt8 *)rtp_packet_send_info->rtp_packet_data)[1] = tmp_u8;

    ((UInt16 *)rtp_packet_send_info->rtp_packet_data)[1] = htons(rtp_header_info->sequence_number);
    ((UInt32 *)rtp_packet_send_info->rtp_packet_data)[1] = htonl(rtp_header_info->timestamp);
    ((UInt32 *)rtp_packet_send_info->rtp_packet_data)[2] = htonl(rtp_header_info->ssrc);

    start = 12; //RTP Header Size 12bytes

    //rtp_payload_data
    memcpy(rtp_packet_send_info->rtp_packet_data + start, rtp_payload_data, rtp_payload_size);

    //track_stream_info packet out packet    
    track_timescale = track_stream_info->box_trak->box_mdia->box_mdhd->timescale;
    au_dts_msec     = track_stream_info->au_info->DTS * 1000;

    rtp_packet_send_info->DTS             = au_dts_msec / track_timescale; //transmit time에 적용할 dts 저장(millisecond)
    rtp_packet_send_info->rtp_packet_size = start + rtp_payload_size;
    rtp_packet_send_info->track_id        = track_stream_info->box_trak->box_tkhd->track_id;
    rtp_packet_send_info->has_packet      = 1;
    
    track_stream_info->rtp_packet_info.rtp_payload_size = 0;  // payload size 초기화

    return true;
}


/*
* Desc : sample_number의 sample data 전체를 얻는다.
*/
rs_status_t rtpfile_get_access_unit_info(session_stream_info_t *session_stream_info, track_stream_info_t *track_stream_info, UInt32 sample_number, access_unit_info_t *au_info, UInt32 max_size)
{
	rs_status_t result_code = ISOM_UNKNOWN;
    UInt32 desc_index;
    if (!session_stream_info || !sample_number || !au_info) {
        return ISOM_GET_ACCESS_UNIT_INFO_BAD_PARAM;
    }

	memset(au_info->data, 0x00, max_size);
	au_info->cts_offset  = 0;
	au_info->data_length = 0;
	au_info->DTS         = 0;
	au_info->is_RAP      = 0;

    result_code = rtpfile_set_access_unit_info(session_stream_info, track_stream_info, sample_number, &au_info, &desc_index, 0, NULL);
    session_stream_info->result_code = result_code;
	return result_code;
}

/*
* Desc : file을 읽어 access_unit_info_t 구조체에 sample 정보 할당 
*/
rs_status_t rtpfile_set_access_unit_info(session_stream_info_t *session_stream_info, track_stream_info_t *track_stream_info, UInt32 sample_number, access_unit_info_t **samp, UInt32 *s_idx, Bool16 no_data, UInt64 *out_offset)
{
    rs_status_t   result_code = ISOM_UNKNOWN;
	UInt32 chunk_number;
	UInt64 offset;
	UInt8 is_edited;
    isom_box_trak_t *box_trak;

    if (!track_stream_info) {
        return ISOM_SET_ACCESS_UNIT_INFO_BAD_PARAM;
    }

    box_trak = track_stream_info->box_trak;

    if (sample_number > box_trak->box_mdia->box_minf->box_stbl->box_stsz->numentries) {
        result_code = ISOM_STREAM_SAMPLE_COUNT_MISMATCH;
        session_stream_info->result_code = result_code;
        return result_code;
    }
	//get the DTS
    if ((result_code = rtpfile_get_sample_dts_from_stts(track_stream_info,
                                                        box_trak->box_mdia->box_minf->box_stbl->box_stts,
                                                        sample_number,
                                                        &(*samp)->DTS)) != ISOM_OK) {
        result_code = ISOM_SET_ACCESS_UNIT_INFO_STTS_FAIL;
        session_stream_info->result_code = result_code;
        return result_code;
    }

	//the CTS offset
	if (box_trak->box_mdia->box_minf->box_stbl->box_ctts) {
        if ((result_code = rtpfile_get_sample_cts_from_ctts(track_stream_info,
                                                            box_trak->box_mdia->box_minf->box_stbl->box_ctts,
                                                            sample_number,
                                                            &(*samp)->cts_offset)) != ISOM_OK) {
            result_code = ISOM_SET_ACCESS_UNIT_INFO_CTTS_FAIL;
            return result_code;
        }
	} else {
		(*samp)->cts_offset = 0;
	}

	//the size
    if ((result_code = rtpfile_get_sample_size_from_stsz(track_stream_info,
                                                         box_trak->box_mdia->box_minf->box_stbl->box_stsz,
                                                         sample_number,
                                                         &(*samp)->data_length)) != ISOM_OK) {
        result_code = ISOM_SET_ACCESS_UNIT_INFO_STSZ_FAIL;
        session_stream_info->result_code = result_code;
        return result_code;
    }

	//the RAP
	if (box_trak->box_mdia->box_minf->box_stbl->box_stss) {
        if ((result_code = rtpfile_get_sample_random_access_point_from_stss(track_stream_info,
                                                                            box_trak->box_mdia->box_minf->box_stbl->box_stss,
                                                                            sample_number,
                                                                            &(*samp)->is_RAP,
                                                                            NULL,
                                                                            NULL)) != ISOM_OK) {
            result_code = ISOM_SET_ACCESS_UNIT_INFO_STSS_RAP_FAIL;
            return result_code;
        }
	} else {
		//if no SyncSample, all samples are sync (cf spec)
		(*samp)->is_RAP = 1;
	}

	//the data info
    if (!s_idx && !no_data) {
        return ISOM_SET_ACCESS_UNIT_INFO_BAD_PARAM;
    }
    if (!s_idx && !out_offset) {
        return ISOM_SET_ACCESS_UNIT_INFO_BAD_PARAM;
    }

	(*s_idx) = 0;
    if (!rtpfile_get_chunknumber_and_offset_from_stco(track_stream_info, box_trak, sample_number, &offset, &chunk_number, s_idx, &is_edited)) {
        return ISOM_GET_CHUNKNUMBER_FAIL;
    }

    if ((result_code = isom_read_resource(session_stream_info->mp4file_info->data_resource,
                                             offset,
                                             (*samp)->data,
                                             (*samp)->data_length)) != ISOM_OK) {
        result_code = result_code + STREAM_ERROR_CODE;
        session_stream_info->result_code = result_code;
        return result_code;
    }

    if ((session_stream_info->mp4file_info->data_resource->res_file->file_pos - offset) < (*samp)->data_length) {
	//if ((session_stream_info->stream_data_resource->file_resource->file_pos - offset) < (*samp)->data_length) {
		return ISOM_STREAM_FILE_OFFSET_AU_SIZE_MISMATCH;
	}	
	return ISOM_OK;
}


