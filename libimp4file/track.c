#include "osheader.h"
#include "rtp_streamer.h"

/*
* Desc : 
*/
Bool16 get_esd_from_trak(ts_box_trak *box_trak, UInt32 stream_description_index, ts_esd **out_esd)
{
    isom_esd_t              *esd;
    ts_sl_config_descriptor *slc;
    ts_box_trak *ocr_box_trak;
    ts_track_referencetypebox *dpnd;
    ts_mpeg_sample_entrybox *entry;
    
    dpnd = NULL;
	*out_esd = NULL;
    
    if (!ts_get_esd_from_stsd(box_trak->box_mdia->box_minf->box_stbl->box_stsd, stream_description_index, &esd, 0)) {
        return false;
    }

    if (!ts_get_sample_description(box_trak->box_mdia->box_minf->box_stbl->box_stsd, stream_description_index,
                                    (ts_sample_entry_box **) &entry, NULL)) {
        return false;
    }
    
    //set the ID
	esd->esid = (UInt16)box_trak->box_tkhd->track_id;
	
	//find stream dependencies
    if (!ts_track_find_ref(box_trak, &dpnd)) {
        return false;
    }

    if (dpnd) {
		//ONLY ONE STREAM DEPENDENCY IS ALLOWED
        if (dpnd->track_idcount != 1) {
            return false;
        }
		//fix the spec: where is the index located ??
		esd->depends_onesid = (UInt16)dpnd->track_ids[0];
	} else {
		esd->depends_onesid = 0;
	}

	//OK, get the OCR (in a REAL MP4File, OCR is 0 in ESD and is specified through track reference
	dpnd = NULL;
	ocr_box_trak = NULL;
	//find OCR dependencies
    if (!ts_track_find_ref(box_trak, &dpnd)) {
        return false;
    }
    // TODO : 분석 후 제거 - end

	
	/*normally all files shall be stored with predefined=SLPredef_MP4, but of course some are broken (philips)
	so we just check the ESD_URL. If set, use the given cfg, otherwise always rewrite it*/
	if (esd->url_string != NULL) {
		*out_esd = esd;
		return true;
	}

	//if we are in publishing mode and we have an SLConfig specified, use it as is
	switch (entry->type) {
	case TS_ISOM_BOX_TYPE_MP4V:
		slc = ((ts_mpeg_visual_sample_entry_box *)entry)->slc;
		break;
	case TS_ISOM_BOX_TYPE_MP4A:
		slc = ((ts_mpeg_audio_sample_entry_box *)entry)->slc;
		break;
	case TS_ISOM_BOX_TYPE_MP4S:
		slc = entry->slc;
		break;
	default:
		slc = NULL;
		break;
	}
	if (slc) {
		ts_delete_odf_descriptor((ts_descriptor *)esd->sl_config_descriptor);
		ts_copy_odf_desc((ts_descriptor *)slc, (ts_descriptor **)&esd->sl_config_descriptor);
		*out_esd = esd;
		return true;
	}
	//otherwise use the regular mapping
	
	//this is a desc for a track_stream_info in the file, let's rewrite some param
	esd->sl_config_descriptor->timestamp_length = 32;
	esd->sl_config_descriptor->timestamp_resolution = box_trak->box_mdia->box_mdhd->timescale;
	//NO OCR from MP4File streams (eg, constant OC Res one)
	esd->sl_config_descriptor->ocr_length = 0;
	if (ocr_box_trak) {
		esd->sl_config_descriptor->ocr_resolution = ocr_box_trak->box_mdia->box_mdhd->timescale;
	} else {
		esd->sl_config_descriptor->ocr_resolution = 0;
	}
	
	// a little optimization here: if all our samples are sync, 
	//set the RAPOnly to true... for external users...
	if (!box_trak->box_mdia->box_minf->box_stbl->box_stss) {
		esd->sl_config_descriptor->has_random_access_units_only_flag = 1;
		esd->sl_config_descriptor->use_random_access_point_flag = 0;
	} else {
		esd->sl_config_descriptor->has_random_access_units_only_flag = 0;
		//signal we are NOT using sync points if no info is present in the table
		esd->sl_config_descriptor->use_random_access_point_flag = box_trak->box_mdia->box_minf->box_stbl->box_stss->numentries ? 1 : 0;
	}
	
	
	//change to support reflecting OD streams
	esd->sl_config_descriptor->use_access_unit_end_flag = 1;
	esd->sl_config_descriptor->use_access_unit_start_flag = 1;

	//signal we do have padding flag (since we only use logical SL packet
	//the user can decide whether to use the info or not
	esd->sl_config_descriptor->use_padding_flag = 0;

	//same with degradation priority
	esd->sl_config_descriptor->degradation_priority_length = 0;

	//this new SL will be OUT OF THE FILE. Let's set its predefined to 0
	esd->sl_config_descriptor->predefined = 0;
	
	*out_esd = esd;

    return true;
}


rs_status_t set_track_decode_packetize_info(session_stream_info_t *session_stream_info, 
                                          track_stream_info_t *track_stream_info,
                                          isom_box_trak_t *box_trak,
                                          UInt32 usable_mtu_payload_size,
                                          UInt32 decode_option)
{
    rs_status_t result_code = ISOM_UNKNOWN;
    UInt32      min_size, avg_ts, const_dur, max_dts_delta;
    UInt32      stream_type, oti, nb_ch;	
    UInt32      track_media_subtype, track_media_type, rfc_codec_type, required_rate, force_dts_delta, pl_id, iv_length, ki_length;
    UInt32      default_rtp_rate;
    UInt32      bandwidth = 0;
    UInt32      nalunit_size_info = 0;
    UInt32      au_max_size;
    Bool16      is_crypted, has_mpeg4_mapping;
    UInt8       static_payload_type;
    UInt8       payload_type;
    char       *mpeg4mode;	
    isom_esd_t *esd = NULL;

    isom_sl_config_descriptor_t  sl_config_descriptor;
    track_decode_info_t         *track_decode_info = NULL;
    track_packetize_info_t      *track_packetize_info = NULL;
    track_bitrate_info_t        *track_bitrate_info = NULL;

    pl_id = 0;
    static_payload_type = 0;
    force_dts_delta = 0;
    stream_type = oti = 0;
    mpeg4mode = NULL;
    required_rate = 0;
    is_crypted = 0;
    iv_length = ki_length = 0;
    oti = 0;
    nb_ch = 0;
    has_mpeg4_mapping = 1;
    rfc_codec_type = ISOM_RTP_PAYT_UNKNOWN;
    
    payload_type = session_stream_info->conf_payload_type;
	track_media_type = box_trak->box_mdia->box_hdlr->handler_type;
	
	/*for max compatibility with QT*/
	default_rtp_rate = ISOM_VIDEO_DEFAULT_RTP_RATE;

    track_stream_info->track_decode_info = (track_decode_info_t *) malloc(sizeof(track_decode_info_t));
    if (track_stream_info->track_decode_info == NULL) {
        result_code = ISOM_MEMORY_ERROR;
        session_stream_info->result_code = result_code;
        return result_code;
    }
    track_stream_info->track_packetize_info = (track_packetize_info_t *) malloc(sizeof(track_packetize_info_t));
    if (track_stream_info->track_packetize_info == NULL) {
        result_code = ISOM_MEMORY_ERROR;
        session_stream_info->result_code = result_code;
        return result_code;
    }
    memset(track_stream_info->track_decode_info, 0x00, sizeof(track_decode_info_t));
    memset(track_stream_info->track_packetize_info, 0x00, sizeof(track_packetize_info_t));

    track_stream_info->track_decode_info->track_bitrate_info = (track_bitrate_info_t *) malloc(sizeof(track_bitrate_info_t));
    if (track_stream_info->track_decode_info->track_bitrate_info == NULL) {
        result_code = ISOM_MEMORY_ERROR;
        session_stream_info->result_code = result_code;
        return result_code;
    }
    memset(track_stream_info->track_decode_info->track_bitrate_info, 0x00, sizeof(track_bitrate_info_t)); 

    track_decode_info = track_stream_info->track_decode_info;
    track_packetize_info = track_stream_info->track_packetize_info;
    track_bitrate_info = track_stream_info->track_decode_info->track_bitrate_info;
	
    if (ts_get_list_count(box_trak->box_mdia->box_minf->box_stbl->box_stsd->box_list) > 1) {
        result_code = ISOM_CODEC_COUNT_OVER;
        session_stream_info->result_code = result_code;
        return result_code;
    }
	track_media_subtype = ts_get_media_subtype(box_trak, 1);

	switch (track_media_subtype) {
	case ISOM_SUBTYPE_MPEG4_CRYP: 
		is_crypted = 1;
	case ISOM_SUBTYPE_MPEG4:
        if (!ts_get_esd_from_trak(box_trak, 1, &esd)) {
			break;
        }
		rfc_codec_type = ISOM_RTP_PAYT_MPEG4;

		if (esd) {
            track_bitrate_info->avg_bitrate = esd->decoder_config->avg_bitrate;
            track_bitrate_info->max_bitrate = esd->decoder_config->max_bitrate;
            track_bitrate_info->buffer_sizedb = esd->decoder_config->buffer_sizedb;

			stream_type = esd->decoder_config->stream_type; //sdp streamtype=
			oti = esd->decoder_config->object_type_indication;//sdp objectType= //
            if (esd->url_string) {
                rfc_codec_type = 0;
            }

			//AAC
			if ((stream_type==ISOM_STREAM_AUDIO) && esd->decoder_config->decoder_specific_info //(nb: we use mpeg4 for MPEG-2 AAC)			
			&& ((oti==0x40) || (oti==0x66) || (oti==0x67) || (oti==0x68)) ) {
				UInt32 sample_rate;
				ts_m4a_decspec_info a_cfg;
				ts_get_m4a_config(esd->decoder_config->decoder_specific_info->data, 
                    esd->decoder_config->decoder_specific_info->data_length, &a_cfg);
				nb_ch = a_cfg.nb_chan;
				sample_rate = a_cfg.base_sr;//sampling rate: Stsd Box의 Dec-Specific Info Description Sample_freq_index
				pl_id = a_cfg.audioPL;
				switch (a_cfg.base_object_type) { // sdp mode 설정 
				case ISOM_M4A_AAC_MAIN:
				case ISOM_M4A_AAC_LC:
					if (decode_option & ISOM_RTP_PCK_USE_LATM_AAC) {//LATM 결정 
						rfc_codec_type = ISOM_RTP_PAYT_LATM;
						break;
					}
				case ISOM_M4A_AAC_SBR:
				case ISOM_M4A_AAC_LTP:
				case ISOM_M4A_AAC_SCALABLE:
				case ISOM_M4A_ER_AAC_LC:
				case ISOM_M4A_ER_AAC_LTP:
				case ISOM_M4A_ER_AAC_SCALABLE:
					mpeg4mode = "AAC";
					break;
				case ISOM_M4A_CELP:
				case ISOM_M4A_ER_CELP:
					mpeg4mode = "CELP";
					break;
				}
				required_rate = sample_rate;
			}
            /*QCELP audio*/
            else if ((stream_type==ISOM_STREAM_AUDIO) && (oti==0xE1)) {
                rfc_codec_type = ISOM_RTP_PAYT_QCELP;
                static_payload_type = 12;
                payload_type = 12;
                required_rate = ISOM_VOICE_DEFAULT_RTP_RATE;
                stream_type = ISOM_STREAM_AUDIO;
                nb_ch = 1;
            }
            /*EVRC/SVM audio*/
            else if ((stream_type==ISOM_STREAM_AUDIO) && ((oti==0xA0) || (oti==0xA1)) ) {
                rfc_codec_type = ISOM_RTP_PAYT_EVRC_SMV;
                required_rate = ISOM_VOICE_DEFAULT_RTP_RATE;
                stream_type = ISOM_STREAM_AUDIO;
                nb_ch = 1;
            }	
			//visual streams
			else if (stream_type==ISOM_STREAM_VISUAL) {
				if (oti==0x20) {
					ts_m4v_decspecinfo dsi;
					ts_get_m4v_config(esd->decoder_config->decoder_specific_info->data, 
                        esd->decoder_config->decoder_specific_info->data_length, &dsi);
					pl_id = dsi.VideoPL;
				}
				//MPEG1/2 video
				if ( ((oti>=0x60) && (oti<=0x65)) || (oti==0x6A)) {
					if (!is_crypted) {
						rfc_codec_type = ISOM_RTP_PAYT_MPEG12_VIDEO;
						static_payload_type = 32;
					}
				}
				//for ISMA
				if (is_crypted) {
					//that's another pain with ISMACryp, even if no B-frames the dts is signaled...
					if (oti==0x20) force_dts_delta = 22;
					decode_option |= ISOM_RTP_PCK_SIGNAL_RAP | ISOM_RTP_PCK_SIGNAL_TS;
				}

				required_rate = default_rtp_rate;
			}
			
			ts_delete_odf_descriptor((ts_descriptor*)esd);
		}
		break;
    case ISOM_SUBTYPE_3GP_H263:
        rfc_codec_type = ISOM_RTP_PAYT_H263;
        required_rate = ISOM_VIDEO_DEFAULT_RTP_RATE;
        stream_type = ISOM_STREAM_VISUAL;
        static_payload_type = 34;
        /*not 100% compliant (short rtp_header_info is missing) but should still work*/
        oti = 0x20;
        pl_id = 0x01;
        break;
    case ISOM_SUBTYPE_3GP_AMR:
        required_rate = ISOM_VOICE_DEFAULT_RTP_RATE;
        rfc_codec_type = ISOM_RTP_PAYT_AMR;
        stream_type = ISOM_STREAM_AUDIO;
        has_mpeg4_mapping = 0;
        nb_ch = 1;
        break;
    case ISOM_SUBTYPE_3GP_AMR_WB:
        required_rate = 16000;
        rfc_codec_type = ISOM_RTP_PAYT_AMR_WB;
        stream_type = ISOM_STREAM_AUDIO;
        has_mpeg4_mapping = 0;
        nb_ch = 1;
			break;
    case ISOM_SUBTYPE_AVC_H264:
        {
            ts_avc_config *avcc = ts_get_avc_config(box_trak, ISOM_LIST_AVCC_INDEX);
            required_rate = ISOM_VIDEO_DEFAULT_RTP_RATE;	// "90 kHz clock rate MUST be used"
            rfc_codec_type = ISOM_RTP_PAYT_H264_AVC;
            stream_type = ISOM_STREAM_VISUAL;
            nalunit_size_info = avcc->nal_unit_size;
            oti = 0x21;
            pl_id = 0x0F;
            ts_set_avc_bitrate_info(box_trak, 1, track_bitrate_info);
            ts_delete_odf_avc_config(avcc);
        }
		break;
    case ISOM_SUBTYPE_3GP_QCELP:
        required_rate = ISOM_VOICE_DEFAULT_RTP_RATE;
        rfc_codec_type = ISOM_RTP_PAYT_QCELP;
        stream_type = ISOM_STREAM_AUDIO;
        oti = 0xE1;
        static_payload_type = 12;
        payload_type = 12;
        nb_ch = 1;
        break;
    case ISOM_SUBTYPE_3GP_EVRC:
    case ISOM_SUBTYPE_K3G_EVRC:
    case ISOM_SUBTYPE_3GP_SMV:
        required_rate = ISOM_VOICE_DEFAULT_RTP_RATE;
        rfc_codec_type = ISOM_RTP_PAYT_EVRC_SMV;
        stream_type = ISOM_STREAM_AUDIO;
        if (track_media_subtype == ISOM_SUBTYPE_3GP_EVRC || track_media_subtype == ISOM_SUBTYPE_K3G_EVRC) {
            oti = 0xA0;
        }
        else {
            oti = 0xA1;
        }
        nb_ch = 1;
		break;
	default:
		//ERROR
		rfc_codec_type = 0;
		break;
	}

	//not codec type
    if (!rfc_codec_type) {
        result_code = TS_UNKNOWN_CODEC_TYPE;
        session_stream_info->result_code = result_code;
        return result_code;
    }
	
	//override hinter type if requested and possible
	if (has_mpeg4_mapping && (decode_option & ISOM_RTP_PCK_FORCE_MPEG4)) {
		rfc_codec_type = TS_RTP_PAYT_MPEG4;
		nalunit_size_info = 0;
	}
	//use static payload type if enabled
	else if (static_payload_type && (decode_option & ISOM_RTP_PCK_USE_STATIC_ID) ) {
		payload_type = static_payload_type;
	}
    
    if ((result_code = ts_get_avg_info(track_stream_info, box_trak, &min_size, &au_max_size, &avg_ts, &max_dts_delta, &const_dur, &bandwidth)) != ISOM_OK) {
        session_stream_info->result_code = result_code;
        return result_code;
    }

    //systems carousel: we need at least IDX and RAP signaling
	if (decode_option & ISOM_RTP_PCK_AUTO_CAROUSEL) {
		decode_option |= ISOM_RTP_PCK_SIGNAL_RAP | ISOM_RTP_PCK_SIGNAL_AU_IDX;
	}   

	//update decode_option in MultiSL
	if (decode_option & ISOM_RTP_PCK_USE_MULTI) {
		if (min_size != au_max_size) decode_option |= ISOM_RTP_PCK_SIGNAL_SIZE;
		if (!const_dur) decode_option |= ISOM_RTP_PCK_SIGNAL_TS;
	}
    
    //default SL for RTP ts_initialize_sl(&sl_config_descriptor);
    memset(&sl_config_descriptor, 0, sizeof(isom_sl_config_descriptor_t));
    sl_config_descriptor.tag = ISOM_ODF_SLC_TAG;
    sl_config_descriptor.use_timestamps_flag = 1;
    sl_config_descriptor.timestamp_length = 32;

	sl_config_descriptor.timestamp_resolution = box_trak->box_mdia->box_mdhd->timescale;
	//override clockrate if set
	if (required_rate) {
		double sc = required_rate;
		sc /= sl_config_descriptor.timestamp_resolution;
		max_dts_delta = (UInt32) (max_dts_delta*sc);
		sl_config_descriptor.timestamp_resolution = required_rate;
	}

	sl_config_descriptor.au_seqnum_length = (UInt8)ts_get_bit_operation_bit_size(box_trak->box_mdia->box_minf->box_stbl->box_stsz->numentries);
	sl_config_descriptor.cu_duration = (UInt16)const_dur;

	if (ts_has_sync_points(box_trak)) {
		sl_config_descriptor.use_random_access_point_flag = 1;
	} else {
		sl_config_descriptor.use_random_access_point_flag = 0;
		sl_config_descriptor.has_random_access_units_only_flag = 1;
	}
  
    track_decode_info->nb_chan = nb_ch;

    track_decode_info->au_max_size = au_max_size;
    track_decode_info->bandwidth   = bandwidth;
    track_decode_info->total_au_count = box_trak->box_mdia->box_minf->box_stbl->box_stsz->numentries;
    track_packetize_info->nalunit_size_info = nalunit_size_info;
    track_packetize_info->usable_mtu_payload_size = usable_mtu_payload_size;

    //rtp header
    track_stream_info->rtp_packet_info.rtp_header_info.version = 2;
	track_stream_info->rtp_packet_info.rtp_header_info.payload_type = payload_type;

    if (&sl_config_descriptor) {
        memcpy(&track_decode_info->sl_config_descriptor, &sl_config_descriptor, sizeof(isom_sl_config_descriptor_t));
    } else {
        memset(&track_decode_info->sl_config_descriptor, 0, sizeof(isom_sl_config_descriptor_t));
        track_decode_info->sl_config_descriptor.use_timestamps_flag = 1;
        track_decode_info->sl_config_descriptor.timestamp_length = 32;
    }
    
    track_decode_info->rfc_codec_type = rfc_codec_type;
    track_decode_info->decode_option = decode_option;
    //default init
    track_decode_info->rtp_sl_header.au_sequence_number = 1;
    track_decode_info->rtp_sl_header.packet_sequence_number = 1;
    
    //we assume we start on a new AU
    track_decode_info->rtp_sl_header.access_unit_start_flag = 1;
    /*our max config is with 1 packet only (SingleSL)*/
	track_packetize_info->first_sl_in_rtp = 1;
	
    //ts_rtp_mp4_es_generic_info set
	ts_set_decode_option_use_mp4_generic_info(track_stream_info, usable_mtu_payload_size,
					   stream_type, oti, pl_id, min_size, au_max_size, avg_ts, max_dts_delta, iv_length, ki_length, mpeg4mode);
	
    session_stream_info->result_code = ISOM_OK;
    return result_code;
}