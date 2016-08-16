#include "sample_description.h"


/*
* Desc : case ignore comparing 대소문자 무시하고 비교
*/
UInt32 ut_strnicmp(char *str, char *needle, UInt32 len)
{ 
    UInt32 count=0; 
    while (count!=len) {         
        if (tolower(*(str+count))==tolower(*(needle+count))) {             
            ++count;             
        }
        else{            
            return 1;             
        }         
    } 
    
    return 0;     
}

#if 0
/*
* Desc : 
*/
Bool16 ts_initialize_iso_sample(track_stream_info_t *track_stream_info)
{
	access_unit_info_t *tmp_au = NULL;

    if (track_stream_info->track_decode_info->au_max_size == 0) {
		return false;
    }
    
    tmp_au = (access_unit_info_t *) malloc(sizeof(access_unit_info_t));
    if (tmp_au == NULL) {
        return false;
    }
    memset(tmp_au, 0, sizeof(access_unit_info_t));


	tmp_au->au_data = (char *) malloc(track_stream_info->track_decode_info->au_max_size);

	track_stream_info->au_info = tmp_au;

	return true;
}  

#endif

rs_status_t rtpfile_set_track_decode_packetize_info(session_stream_info_t *session_stream_info,
                                                    track_stream_info_t *track_stream_info,
                                                    isom_box_trak_t *box_trak,
                                                    UInt32 usable_mtu_payload_size,
                                                    UInt32 decode_option)

{
    rs_status_t result_code = ISOM_UNKNOWN;
    UInt32 min_size, avg_ts, const_dur, max_dts_delta;
    UInt32 stream_type, oti, nb_ch;	
    UInt32 track_media_subtype, track_media_type, rfc_codec_type, required_rate, force_dts_delta, pl_id, iv_length, ki_length;
    UInt32 default_rtp_rate;
    UInt32 bandwidth = 0;
    UInt32 nalunit_size_info = 0;
    UInt32 au_max_size;
    Bool16 is_crypted, has_mpeg4_mapping;
    UInt8 static_payload_type;
    UInt8 payload_type;
    char *mpeg4mode;

    isom_esd_t *esd = NULL;
    isom_sl_config_descriptor_t sl_config_descriptor;

    track_decode_info_t    *track_decode_info = NULL;
    track_packetize_info_t *track_packetize_info = NULL;
    track_bitrate_info_t   *track_bitrate_info = NULL;

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
    rfc_codec_type = RTP_PAYT_UNKNOWN;
    
    payload_type = session_stream_info->conf_payload_type;
	track_media_type = box_trak->box_mdia->box_hdlr->handler_type;
	
	/*for max compatibility with QT*/
	default_rtp_rate = DEFAULT_VIDEO_RTP_RATE;

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
	
    if (isom_get_list_count(box_trak->box_mdia->box_minf->box_stbl->box_stsd->box_list) > 1) {
        result_code = ISOM_CODEC_COUNT_OVER;
        session_stream_info->result_code = result_code;
        return result_code;
    }
	track_media_subtype = rtpfile_get_media_subtype(box_trak, 1);

	switch (track_media_subtype) {
	case ISOM_SUBTYPE_MPEG4_CRYP: 
		is_crypted = 1;
	case ISOM_SUBTYPE_MPEG4:
        if (!rtpfile_get_esd_from_trak(box_trak, 1, &esd)) {
			break;
        }
		rfc_codec_type = RTP_PAYT_MPEG4;

		if (esd) {
            track_bitrate_info->avg_bitrate   = esd->decoder_config->avg_bitrate;
            track_bitrate_info->max_bitrate   = esd->decoder_config->max_bitrate;
            track_bitrate_info->buffer_sizedb = esd->decoder_config->buffer_sizedb;

			stream_type = esd->decoder_config->stream_type; //sdp streamtype=
			oti = esd->decoder_config->object_type_indication;//sdp objectType= //
            if (esd->url_string) {
                rfc_codec_type = 0;
            }

			//AAC oti(Object Type Indication) : 0x40(64)
			if ((stream_type== ISOM_STREAM_AUDIO) && esd->decoder_config->decoder_specific_info //(nb: we use mpeg4 for MPEG-2 AAC)			
			     && ((oti==0x40) || (oti==0x66) || (oti==0x67) || (oti==0x68)) ) {
				UInt32 sample_rate;
				m4a_decspec_info_t a_cfg;
				rtpfile_get_m4a_config(esd->decoder_config->decoder_specific_info->data, 
                                       esd->decoder_config->decoder_specific_info->data_length,
                                       &a_cfg);
				nb_ch = a_cfg.nb_chan;
				sample_rate = a_cfg.base_sr;//sampling rate: Stsd Box의 Dec-Specific Info Description Sample_freq_index
				pl_id = a_cfg.audioPL;
				switch (a_cfg.base_object_type) { // sdp mode 설정 
				case M4A_AAC_MAIN:
				case M4A_AAC_LC:
					if (decode_option & RTP_PCK_USE_LATM_AAC) {//LATM 결정 
						rfc_codec_type = RTP_PAYT_LATM;
						break;
					}
				case M4A_AAC_SBR:
				case M4A_AAC_LTP:
				case M4A_AAC_SCALABLE:
				case M4A_ER_AAC_LC:
				case M4A_ER_AAC_LTP:
				case M4A_ER_AAC_SCALABLE:
					mpeg4mode = "AAC";
					break;
				case M4A_CELP:
				case M4A_ER_CELP:
					mpeg4mode = "CELP";
					break;
				}
				required_rate = sample_rate;
			}
            /*QCELP audio*/
            else if ((stream_type==ISOM_STREAM_AUDIO) && (oti==0xE1)) {
                rfc_codec_type = RTP_PAYT_QCELP;
                static_payload_type = 12;
                payload_type = 12;
                required_rate = DEFAULT_VOICE_RTP_RATE;
                stream_type = ISOM_STREAM_AUDIO;
                nb_ch = 1;
            }
            /*EVRC/SVM audio*/
            else if ((stream_type==ISOM_STREAM_AUDIO) && ((oti==0xA0) || (oti==0xA1)) ) {
                rfc_codec_type = RTP_PAYT_EVRC_SMV;
                required_rate = DEFAULT_VOICE_RTP_RATE;
                stream_type = ISOM_STREAM_AUDIO;
                nb_ch = 1;
            }	
			//visual streams
			else if (stream_type==ISOM_STREAM_VISUAL) {
				if (oti==0x20) {
					m4v_decspecinfo_t dsi;
					rtpfile_get_m4v_config(esd->decoder_config->decoder_specific_info->data, 
                        esd->decoder_config->decoder_specific_info->data_length, &dsi);
					pl_id = dsi.VideoPL;
				}
				//MPEG1/2 video
				if ( ((oti>=0x60) && (oti<=0x65)) || (oti==0x6A)) {
					if (!is_crypted) {
						rfc_codec_type = RTP_PAYT_MPEG12_VIDEO;
						static_payload_type = 32;
					}
				}
				//for ISMA
				if (is_crypted) {
					//that's another pain with ISMACryp, even if no B-frames the dts is signaled...
					if (oti==0x20) force_dts_delta = 22;
					decode_option |= RTP_PCK_SIGNAL_RAP | RTP_PCK_SIGNAL_TS;
				}

				required_rate = default_rtp_rate;
			}
			
			isom_delete_odf_descriptor((isom_descriptor_t *)esd);
		}
		break;
    case ISOM_SUBTYPE_3GP_H263:
        rfc_codec_type = RTP_PAYT_H263;
        required_rate = DEFAULT_VIDEO_RTP_RATE;
        stream_type = ISOM_STREAM_VISUAL;
        static_payload_type = 34;
        /*not 100% compliant (short rtp_header_info is missing) but should still work*/
        oti = 0x20;
        pl_id = 0x01;
        break;
    case ISOM_SUBTYPE_3GP_AMR:
        required_rate = DEFAULT_VIDEO_RTP_RATE;
        rfc_codec_type = RTP_PAYT_AMR;
        stream_type = ISOM_STREAM_AUDIO;
        has_mpeg4_mapping = 0;
        nb_ch = 1;
        break;
    case ISOM_SUBTYPE_3GP_AMR_WB:
        required_rate = 16000;
        rfc_codec_type = RTP_PAYT_AMR_WB;
        stream_type = ISOM_STREAM_AUDIO;
        has_mpeg4_mapping = 0;
        nb_ch = 1;
			break;
    case ISOM_SUBTYPE_AVC_H264:
        {
            isom_avc_config_t *avcc = rtpfile_get_avc_config(box_trak, LIST_AVCC_INDEX);
            required_rate = DEFAULT_VIDEO_RTP_RATE;	// "90 kHz clock rate MUST be used"
            rfc_codec_type = RTP_PAYT_H264_AVC;
            stream_type = ISOM_STREAM_VISUAL;
            nalunit_size_info = avcc->nal_unit_size;
            oti = 0x21;
            pl_id = 0x0F;
            rtpfile_set_avc_bitrate_info(box_trak, 1, track_bitrate_info);
            isom_delete_odf_avc_config(avcc);
        }
		break;
    case ISOM_SUBTYPE_3GP_QCELP:
        required_rate = DEFAULT_VOICE_RTP_RATE;
        rfc_codec_type = RTP_PAYT_QCELP;
        stream_type = ISOM_STREAM_AUDIO;
        oti = 0xE1;
        static_payload_type = 12;
        payload_type = 12;
        nb_ch = 1;
        break;
    case ISOM_SUBTYPE_3GP_EVRC:
    case ISOM_SUBTYPE_K3G_EVRC:
    case ISOM_SUBTYPE_3GP_SMV:
        required_rate = DEFAULT_VOICE_RTP_RATE;
        rfc_codec_type = RTP_PAYT_EVRC_SMV;
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
        result_code = ISOM_UNKNOWN_CODEC_TYPE;
        session_stream_info->result_code = result_code;
        return result_code;
    }
	
	//override hinter type if requested and possible
	if (has_mpeg4_mapping && (decode_option & RTP_PCK_FORCE_MPEG4)) {
		rfc_codec_type = RTP_PAYT_MPEG4;
		nalunit_size_info = 0;
	}
	//use static payload type if enabled
	else if (static_payload_type && (decode_option & RTP_PCK_USE_STATIC_ID) ) {
		payload_type = static_payload_type;
	}
    
    if ((result_code = rtpfile_get_avg_info(track_stream_info, box_trak, &min_size, &au_max_size, &avg_ts, &max_dts_delta, &const_dur, &bandwidth)) != ISOM_OK) {
        session_stream_info->result_code = result_code;
        return result_code;
    }

    //systems carousel: we need at least IDX and RAP signaling
	if (decode_option & RTP_PCK_AUTO_CAROUSEL) {
		decode_option |= RTP_PCK_SIGNAL_RAP | RTP_PCK_SIGNAL_AU_IDX;
	}   

	//update decode_option in MultiSL
	if (decode_option & RTP_PCK_USE_MULTI) {
		if (min_size != au_max_size) decode_option |= RTP_PCK_SIGNAL_SIZE;
		if (!const_dur) decode_option |= RTP_PCK_SIGNAL_TS;
	}
    
    //default SL for RTP rtpfile_init_sl(&sl_config_descriptor);
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

	sl_config_descriptor.au_seqnum_length = (UInt8)isom_get_bit_operation_bit_size(box_trak->box_mdia->box_minf->box_stbl->box_stsz->numentries);
	sl_config_descriptor.cu_duration = (UInt16)const_dur;

	if (rtpfile_has_sync_points(box_trak)) {
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
	rtpfile_set_decode_option_use_mp4_generic_info(track_stream_info, usable_mtu_payload_size,
					   stream_type, oti, pl_id, min_size, au_max_size, avg_ts, max_dts_delta, iv_length, ki_length, mpeg4mode);
	
    session_stream_info->result_code = ISOM_OK;
    return result_code;
}



UInt32 rtpfile_get_media_subtype(isom_box_trak_t *box_trak, UInt32 description_index)
{
    isom_box_t *entry;
    
    if (!box_trak || !description_index) {
        return 0;
    }
    entry = (isom_box_t*)isom_get_list(box_trak->box_mdia->box_minf->box_stbl->box_stsd->box_list, description_index-1);
    if (!entry) {
        return 0;
    }
    
    //filter MPEG sub-types
    if (rtpfile_is_mp4_description(entry->type)) {
        if (rtpfile_is_mp4_encrypted_description(entry->type)) {
            return ISOM_SUBTYPE_MPEG4_CRYP;
        }
        else {
            return ISOM_SUBTYPE_MPEG4;
        }          
    }
    
    return entry->type;
}


Bool16 rtpfile_is_mp4_encrypted_description(UInt32 entry_type)
{
    switch (entry_type) {
    case ISOM_BOX_TYPE_ENCA:
    case ISOM_BOX_TYPE_ENCV:
    case ISOM_BOX_TYPE_ENCS:
        return 1;
    default:
        return 0;
    }
}


Bool16 rtpfile_is_mp4_description(UInt32 entry_type)
{
    switch (entry_type) {
    case ISOM_BOX_TYPE_MP4S:
    case ISOM_BOX_TYPE_MP4A:
    case ISOM_BOX_TYPE_MP4V:
    case ISOM_BOX_TYPE_ENCA:
    case ISOM_BOX_TYPE_ENCV:
    case ISOM_BOX_TYPE_ENCS:
        return true;
    default:
        return false;
    }
}


Bool16 rtpfile_track_find_ref(isom_box_trak_t *box_trak, isom_track_referencetypebox_t **dpnd)
{
    if (!box_trak) {
        return false;
    }
    
    *dpnd = NULL;
    return true;
    
}

Bool16 rtpfile_get_esd_from_trak(isom_box_trak_t *box_trak, UInt32 stream_description_index, isom_esd_t **out_esd)
{
    isom_esd_t *esd;
    isom_sl_config_descriptor_t *slc;
    isom_box_trak_t *ocr_box_trak;
    isom_track_referencetypebox_t *dpnd;
    isom_mpeg_sample_entrybox_t *entry;
    
    dpnd = NULL;
	*out_esd = NULL;
    
    if (!rtpfile_get_esd_from_stsd(box_trak->box_mdia->box_minf->box_stbl->box_stsd, stream_description_index, &esd, 0)) {
        return false;
    }

    if (!rtpfile_get_sample_description(box_trak->box_mdia->box_minf->box_stbl->box_stsd, stream_description_index,
                                    (isom_sample_entry_box_t **) &entry, NULL)) {
        return false;
    }
    
    //set the ID
	esd->esid = (UInt16)box_trak->box_tkhd->track_id;
	
    // TODO : ?????????????????????????????????????????????
	//find stream dependencies
    if (!rtpfile_track_find_ref(box_trak, &dpnd)) {
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
    if (!rtpfile_track_find_ref(box_trak, &dpnd)) {
        return false;
    }
    // TODO : ?????????????????????????????????????????????????????????

	
	/*normally all files shall be stored with predefined=SLPredef_MP4, but of course some are broken (philips)
	so we just check the ESD_URL. If set, use the given cfg, otherwise always rewrite it*/
	if (esd->url_string != NULL) {
		*out_esd = esd;
		return true;
	}

	//if we are in publishing mode and we have an SLConfig specified, use it as is
	switch (entry->type) {
	case ISOM_BOX_TYPE_MP4V:
		slc = ((isom_mpeg_visual_sample_entry_box_t *)entry)->slc;
		break;
	case ISOM_BOX_TYPE_MP4A:
		slc = ((isom_mpeg_audio_sample_entry_box_t *)entry)->slc;
		break;
	case ISOM_BOX_TYPE_MP4S:
		slc = entry->slc;
		break;
	default:
		slc = NULL;
		break;
	}

	if (slc) {
		isom_delete_odf_descriptor((isom_descriptor_t *)esd->sl_config_descriptor);
		isom_copy_odf_desc((isom_descriptor_t *)slc, (isom_descriptor_t **)&esd->sl_config_descriptor);
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


Bool16 rtpfile_get_esd_from_stsd(isom_box_stsd_t *stsd, UInt32 stream_description_index, isom_esd_t **out_esd, Bool16 true_desc_only)
{
    isom_esd_t *esd;
    isom_mpeg_sample_entrybox_t *entry = NULL;
    isom_esd_box_t *esd_a;
    
    *out_esd = NULL;
    if (!stsd || !stsd->box_list || !stream_description_index || 
        (stream_description_index > isom_get_list_count(stsd->box_list)) ){
        return false;
    }
    
    esd = NULL;
    entry = (isom_mpeg_sample_entrybox_t *)isom_get_list(stsd->box_list, stream_description_index - 1);
    if (!entry) {
        return false;
    }

    *out_esd = NULL;
    esd_a = NULL;

    switch (entry->type) {
    case ISOM_BOX_TYPE_MP4V:
    case ISOM_BOX_TYPE_ENCV:
        esd_a = ((isom_mpeg_visual_sample_entry_box_t *)entry)->esd;
        if (esd_a) {
            esd = (isom_esd_t *) esd_a->desc;
        }
        break;
    case ISOM_BOX_TYPE_AVC1:
        break;
        
    case ISOM_BOX_TYPE_MP4A:
    case ISOM_BOX_TYPE_ENCA:
        esd_a = ((isom_mpeg_audio_sample_entry_box_t *)entry)->esd;
        if (esd_a) {
            esd = (isom_esd_t *) esd_a->desc;
        }
        break;
    case ISOM_BOX_TYPE_MP4S:
    case ISOM_BOX_TYPE_ENCS:
        esd_a = entry->esd;
        if (esd_a) {
            esd = (isom_esd_t *) esd_a->desc;
        }
        break;
    default: 
        return false;
    }
    
    if (true_desc_only) {
        if (!esd) {
            return false;
        }
        *out_esd = esd;
        return true;
    } else {
        if (!esd && !*out_esd) {
            return false;
        }
        if (*out_esd == NULL) {
            isom_copy_odf_desc((isom_descriptor_t *)esd, (isom_descriptor_t **)out_esd);
        }
    }
    return true;
}


Bool16 rtpfile_get_sample_description(isom_box_stsd_t *stsd, UInt32 stream_description_index, isom_sample_entry_box_t **out_entry, UInt32 *data_reference_index)
{
    isom_sample_entry_box_t *entry = NULL;
    
    if (!stsd) {
        return false;
    }
    if (!stream_description_index || (stream_description_index > isom_get_list_count(stsd->box_list)) ) {
        return false;
    }
    
    entry = (isom_sample_entry_box_t*)isom_get_list(stsd->box_list, stream_description_index - 1);
    if (!entry) {
        return false;
    }
    
    if (out_entry) {
        *out_entry = entry;
    }
    if (data_reference_index) {
        *data_reference_index = entry->data_reference_index;
    }
    return true;
}


Bool16 rtpfile_set_avc_bitrate_info(isom_box_trak_t *box_trak, UInt32 description_index, track_bitrate_info_t *track_bitrate_info)
{
    isom_mpeg_visual_sample_entry_box_t *entry;
    
    if (!box_trak || !description_index) {
        return false;
    }

    entry = (isom_mpeg_visual_sample_entry_box_t*)isom_get_list(box_trak->box_mdia->box_minf->box_stbl->box_stsd->box_list, description_index-1);
    if (!entry) {
        return false;
    }
    //if (entry->type != TS_ISOM_BOX_TYPE_AVC1) return NULL;
    if (!entry->bitrate) {
        return false;
    }

    track_bitrate_info->avg_bitrate  = entry->bitrate->avg_bitrate;
    track_bitrate_info->max_bitrate  = entry->bitrate->max_bitrate;
    track_bitrate_info->buffer_sizedb= entry->bitrate->buffer_sizedb;

    return true;
}


rs_status_t rtpfile_get_avg_info(track_stream_info_t *track_stream_info, isom_box_trak_t *box_trak, UInt32 *avg_size, UInt32 *max_size, UInt32 *time_delta, UInt32 *max_cts_delta, UInt32 *const_duration, UInt32 *bandwidth)
{
    rs_status_t result_code = ISOM_UNKNOWN;
    UInt32 i, count, ts_diff, dataLength;
    UInt32 cts_offset = 0;
    UInt64 prev_ts, dts, tdelta;
    double bw;
   
    *avg_size = *max_size = 0;
    *time_delta = 0;
    *max_cts_delta = 0;
    bw = 0;
    prev_ts = 0;
    dts = 0;
    tdelta = 0;
    
    if (!track_stream_info || !box_trak) {
        result_code = ISOM_BAD_PARAM;
        return result_code;
    }

    count = box_trak->box_mdia->box_minf->box_stbl->box_stsz->numentries;
    *const_duration = 0;
    
    for (i=0; i<count; i++) {
        result_code = rtpfile_get_sample_size_from_stsz(track_stream_info, box_trak->box_mdia->box_minf->box_stbl->box_stsz, i+1, &dataLength);
        if (result_code != ISOM_OK) {
            return result_code;
        }
        // ctts box is option.
        if (box_trak->box_mdia->box_minf->box_stbl->box_ctts) {
            if ((result_code = rtpfile_get_sample_cts_from_ctts(track_stream_info, box_trak->box_mdia->box_minf->box_stbl->box_ctts, i+1, &cts_offset)) != ISOM_OK) {
                return result_code;
            }
        }
        result_code = rtpfile_get_sample_dts_from_stts(track_stream_info, box_trak->box_mdia->box_minf->box_stbl->box_stts, i+1, &dts);
        if (result_code != ISOM_OK) {
            return result_code;
        }
        //get the size
        *avg_size += dataLength;
        if (*max_size < dataLength) {
            *max_size = dataLength;
        }
        ts_diff = (UInt32) (dts+cts_offset - prev_ts);
        //get the time
        tdelta += ts_diff;
        
        if (i==1) {
            *const_duration = ts_diff;
        } 
        else if ( (i < count - 1) && (*const_duration != ts_diff) ) {
            *const_duration = 0;
        }
        
        prev_ts = dts+cts_offset;
        bw += 8*dataLength;
        
        //get the CTS delta
        if (cts_offset > *max_cts_delta) {
            *max_cts_delta = cts_offset;
        }
    }
    if (count>1) {
        *time_delta = (UInt32) (tdelta / (count - 1) );
    }
    else {
        *time_delta = (UInt32) tdelta;
    }

    *avg_size /= count;
    bw *= box_trak->box_mdia->box_mdhd->timescale;
    bw /= (SInt64)box_trak->box_mdia->box_mdhd->duration;
    bw /= 1000;
    (*bandwidth) = (UInt32) (bw+0.5);//b=AS:bandwith

    result_code = ISOM_OK;
    return result_code;
}


isom_avc_config_t *rtpfile_duplicate_avc_config(isom_avc_config_t *cfg)
{
    UInt32 i, count;
    isom_avc_config_slot_t *p1, *p2;
    isom_avc_config_t *cfg_new;

    isom_create_odf_avc_config(&cfg_new);

    cfg_new->avc_level_indication = cfg->avc_level_indication;
    cfg_new->avc_profile_indication = cfg->avc_profile_indication;
    cfg_new->configuration_version = cfg->configuration_version;
    cfg_new->nal_unit_size = cfg->nal_unit_size;
    cfg_new->profile_compatibility = cfg->profile_compatibility;
    
    count = isom_get_list_count(cfg->sequence_parameter_sets);
    for (i=0; i<count; i++) {
        p1 = (isom_avc_config_slot_t*)isom_get_list(cfg->sequence_parameter_sets, i);
        p2 = (isom_avc_config_slot_t*)malloc(sizeof(isom_avc_config_slot_t));
        p2->size = p1->size;
        p2->data = (char *)malloc(sizeof(char)*p1->size);
        memcpy(p2->data, p1->data, sizeof(char)*p1->size);
        isom_add_list(cfg_new->sequence_parameter_sets, p2);
    }
    
    count = isom_get_list_count(cfg->picture_parameter_sets);
    for (i=0; i<count; i++) {
        p1 = (isom_avc_config_slot_t*)isom_get_list(cfg->picture_parameter_sets, i);
        p2 = (isom_avc_config_slot_t*)malloc(sizeof(isom_avc_config_slot_t));
        p2->size = p1->size;
        p2->data = (char*)malloc(sizeof(char)*p1->size);
        memcpy(p2->data, p1->data, sizeof(char)*p1->size);
        isom_add_list(cfg_new->picture_parameter_sets, p2);
    }
    return cfg_new;	
}

/*
* Desc : 
*/
UInt8 rtpfile_has_sync_points(isom_box_trak_t *box_trak)
{
    if (!box_trak) {
        return 0;
    }
    if (box_trak->box_mdia->box_minf->box_stbl->box_stss) {
        if (!box_trak->box_mdia->box_minf->box_stbl->box_stss->numentries) {
            return 2;	
        }
        return 1;
    }
    return 0;
}




/*
* Desc : decode_option을 ts_rtp_mp4_es_generic_info settting
*/
void rtpfile_set_decode_option_use_mp4_generic_info(track_stream_info_t *track_stream_info,
                                                    UInt32 usable_mtu_payload_size,
                                                    UInt32 stream_type, UInt32 oti, UInt32 pl_id,
                                                    UInt32 avg_size, UInt32 max_size, 
                                                    UInt32 abg_ts, UInt32 max_dts,
                                                    UInt32 iv_length, UInt32 ki_length,
                                                    char *pref_mode) 
{
	UInt32 k, tot_delta, ismacrypt_flags;
    track_decode_info_t *track_decode_info = track_stream_info->track_decode_info;
    track_packetize_info_t *track_packetize_info = track_stream_info->track_packetize_info;

	memset(&track_decode_info->rtp_mp4_es_generic_info, 0, sizeof(rtp_mp4_es_generic_info_t));
	
	track_decode_info->rtp_mp4_es_generic_info.stream_type = (UInt8)stream_type;
	track_decode_info->rtp_mp4_es_generic_info.object_type_indication = (UInt8)oti;
	track_decode_info->rtp_mp4_es_generic_info.pl_id = pl_id;

    if (pref_mode) {
        strcpy(track_decode_info->rtp_mp4_es_generic_info.mode, pref_mode);
    }
	
	/*no AUX data*/
	track_decode_info->rtp_mp4_es_generic_info.auxiliary_data_size_length= 0;

	/*just compute max aggregation size*/
	switch (track_decode_info->rfc_codec_type) {   
    case RTP_PAYT_QCELP:
    case RTP_PAYT_EVRC_SMV:
    case RTP_PAYT_AMR:
    case RTP_PAYT_AMR_WB:
    {
            UInt32 nb_pck = 1;
            UInt32 block_size = 160;
            /*compute max frames per packet - if no avg size, use max size per codec*/
            if (track_decode_info->decode_option & RTP_PCK_USE_MULTI) {
                if (track_decode_info->rfc_codec_type == RTP_PAYT_QCELP) {
                    if (!avg_size) {
                        avg_size = 35;
                    }
                    nb_pck = (usable_mtu_payload_size-1) / avg_size;	/*one-byte rtp_header_info*/
                    if (nb_pck>10) {
                        nb_pck=10;	/*cf RFC2658*/
                    }

                } 
                else if (track_decode_info->rfc_codec_type == RTP_PAYT_EVRC_SMV) {
                    if (!avg_size) {
                        avg_size = 23;
                    }
                    nb_pck = (usable_mtu_payload_size) / avg_size;
                    if (nb_pck>32) {
                        nb_pck=32;	/*cf RFC3558*/
                    }
                } 
                else if (track_decode_info->rfc_codec_type == RTP_PAYT_AMR_WB) {
                    if (!avg_size) {
                        avg_size = 61;
                    }
                    nb_pck = (usable_mtu_payload_size-1) / avg_size;
                    block_size = 320;
                } else {
                    if (!avg_size) avg_size = 32;
                    nb_pck = (usable_mtu_payload_size-1) / avg_size;
                }
                /*
                if (max_ptime) {
                    UInt32 max_pck = max_ptime / block_size;
                    if (nb_pck > max_pck) nb_pck = max_pck;
                }
                */
            }
            if (nb_pck <= 1) {
                track_decode_info->decode_option &= ~(RTP_PCK_USE_MULTI | RTP_PCK_USE_INTERLEAVING);
                track_decode_info->auh_size = 1; 
            } 
			else {
                track_decode_info->auh_size = nb_pck;//GP_RTPPacketizer auh_size setting
            }
            /*remove all MPEG-4 and ISMA decode_option */
            track_decode_info->decode_option &= 0x07;
    }
		return;
	case RTP_PAYT_LATM: 
	case RTP_PAYT_MPEG4:
		break;
	default:
		/*remove all MPEG-4 and ISMA decode_option */
		track_decode_info->decode_option &= 0x07;
		/*disable aggregation for visual streams*/
        if (stream_type == ISOM_STREAM_VISUAL) {
            track_decode_info->decode_option &= ~RTP_PCK_USE_MULTI;
        }
        else if (avg_size && (usable_mtu_payload_size <= avg_size) ) {
            track_decode_info->decode_option &= ~RTP_PCK_USE_MULTI;
        }
		return;
	} 

	track_decode_info->rtp_mp4_es_generic_info.iv_length = (UInt8)iv_length;
	track_decode_info->rtp_mp4_es_generic_info.ki_length = (UInt8)ki_length;

	ismacrypt_flags = 0;
	if (track_decode_info->decode_option & RTP_PCK_SELECTIVE_ENCRYPTION) ismacrypt_flags |= RTP_PCK_SELECTIVE_ENCRYPTION;
	if (track_decode_info->decode_option & RTP_PCK_KEY_IDX_PER_AU) ismacrypt_flags |= RTP_PCK_KEY_IDX_PER_AU;

	/*mode setup*/
	if (!ut_strnicmp(track_decode_info->rtp_mp4_es_generic_info.mode, "AAC", 3)) {
		track_decode_info->decode_option = RTP_PCK_USE_MULTI | RTP_PCK_SIGNAL_SIZE | RTP_PCK_SIGNAL_AU_IDX | ismacrypt_flags;
		if (track_decode_info->decode_option & RTP_PCK_USE_INTERLEAVING) track_decode_info->rtp_mp4_es_generic_info.constant_duration = abg_ts;

		/*AAC LBR*/
		if (max_size < 63) {
			strcpy(track_decode_info->rtp_mp4_es_generic_info.mode, "AAC-lbr");
			track_decode_info->rtp_mp4_es_generic_info.index_length = track_decode_info->rtp_mp4_es_generic_info.index_delta_length = 2;
			track_decode_info->rtp_mp4_es_generic_info.size_length = 6;
		}
		/*AAC HBR*/
		else {
			strcpy(track_decode_info->rtp_mp4_es_generic_info.mode, "AAC-hbr");
			track_decode_info->rtp_mp4_es_generic_info.index_length = track_decode_info->rtp_mp4_es_generic_info.index_delta_length = 3;
			track_decode_info->rtp_mp4_es_generic_info.size_length = 13;
		}
		goto check_header;
	}
	if (!ut_strnicmp(track_decode_info->rtp_mp4_es_generic_info.mode, "CELP", 4)) {
		/*CELP-cbr*/
		if (max_size == avg_size) {
			/*reset decode_option (interleaving forbidden)*/
			track_decode_info->decode_option = RTP_PCK_USE_MULTI | ismacrypt_flags;
			strcpy(track_decode_info->rtp_mp4_es_generic_info.mode, "CELP-cbr");
			track_decode_info->rtp_mp4_es_generic_info.constant_size = avg_size;
			track_decode_info->rtp_mp4_es_generic_info.constant_duration = abg_ts;
		}
		/*CELP VBR*/
		else {
			strcpy(track_decode_info->rtp_mp4_es_generic_info.mode, "CELP-vbr");
			track_decode_info->rtp_mp4_es_generic_info.index_length = track_decode_info->rtp_mp4_es_generic_info.index_delta_length = 2;
			track_decode_info->rtp_mp4_es_generic_info.size_length = 6;
			if (track_decode_info->decode_option & RTP_PCK_USE_INTERLEAVING) track_decode_info->rtp_mp4_es_generic_info.constant_duration = abg_ts;
			track_decode_info->decode_option = RTP_PCK_USE_MULTI | RTP_PCK_SIGNAL_SIZE | RTP_PCK_SIGNAL_AU_IDX | ismacrypt_flags;
		}
		goto check_header;
	}

	/*generic setup by decode_option*/
	
	/*size*/
	if (track_decode_info->decode_option & RTP_PCK_SIGNAL_SIZE) {
		if (avg_size==max_size) {
			track_decode_info->rtp_mp4_es_generic_info.size_length = 0;
			track_decode_info->rtp_mp4_es_generic_info.constant_size = max_size;
		} else {
			track_decode_info->rtp_mp4_es_generic_info.size_length = isom_get_bit_operation_bit_size(max_size ? max_size : usable_mtu_payload_size);
			track_decode_info->rtp_mp4_es_generic_info.constant_size = 0;
		}
	} else {
		track_decode_info->rtp_mp4_es_generic_info.size_length = 0;
		if (track_decode_info->decode_option & RTP_PCK_USE_MULTI)
			track_decode_info->rtp_mp4_es_generic_info.constant_size = (avg_size==max_size) ? max_size : 0;
		else
			track_decode_info->rtp_mp4_es_generic_info.constant_size = 0;
	}

	/*single SL per RTP*/
	if (!(track_decode_info->decode_option & RTP_PCK_USE_MULTI)) {
		if ( track_decode_info->sl_config_descriptor.au_seqnum_length && (track_decode_info->decode_option & RTP_PCK_SIGNAL_AU_IDX)) {
			track_decode_info->rtp_mp4_es_generic_info.index_length = track_decode_info->sl_config_descriptor.au_seqnum_length;
		} else {
			track_decode_info->rtp_mp4_es_generic_info.index_length = 0;
		}
		/*one packet per RTP so no delta*/
		track_decode_info->rtp_mp4_es_generic_info.index_delta_length = 0;
		track_decode_info->rtp_mp4_es_generic_info.iv_delta_length = 0;

		/*CTS Delta is always 0 since we have one SL packet per RTP*/
		track_decode_info->rtp_mp4_es_generic_info.cts_delta_length = 0;

		/*DTS Delta depends on the video type*/
		if ((track_decode_info->decode_option & RTP_PCK_SIGNAL_TS) && max_dts ) 
			track_decode_info->rtp_mp4_es_generic_info.dts_delta_length = isom_get_bit_operation_bit_size(max_dts);
		else
			track_decode_info->rtp_mp4_es_generic_info.dts_delta_length = 0;

		/*RAP*/
		if (track_decode_info->sl_config_descriptor.use_random_access_point_flag && (track_decode_info->decode_option & RTP_PCK_SIGNAL_RAP)) {
			track_decode_info->rtp_mp4_es_generic_info.random_access_indication = 1;
		} else {
			track_decode_info->rtp_mp4_es_generic_info.random_access_indication = 0;
		}
		/*TODO: stream state*/
		goto check_header;
	}

	/*this is the avg samples we can store per RTP packet*/
	k = usable_mtu_payload_size / avg_size;
	if (k<=1) {
		track_decode_info->decode_option &= ~RTP_PCK_USE_MULTI;
		/*keep TS signaling for B-frames (eg never default to M4V-ES when B-frames are present)*/
		//track_decode_info->decode_option &= ~RTP_PCK_SIGNAL_TS;
		track_decode_info->decode_option &= ~RTP_PCK_SIGNAL_SIZE;
		track_decode_info->decode_option &= ~RTP_PCK_SIGNAL_AU_IDX;
		track_decode_info->decode_option &= ~RTP_PCK_USE_INTERLEAVING;
		track_decode_info->decode_option &= ~RTP_PCK_KEY_IDX_PER_AU;
		rtpfile_set_decode_option_use_mp4_generic_info(track_stream_info, usable_mtu_payload_size, stream_type, oti, pl_id, avg_size, max_size, abg_ts, max_dts, iv_length, ki_length, pref_mode);
		return;
	}

	/*multiple SL per RTP - check if we have to send TS*/
	track_decode_info->rtp_mp4_es_generic_info.constant_duration = track_decode_info->sl_config_descriptor.cu_duration;
	if (!track_decode_info->rtp_mp4_es_generic_info.constant_duration) {
		track_decode_info->decode_option |= RTP_PCK_SIGNAL_TS;
	}
	/*if we have a constant duration and are not writting TSs, make sure we write AU IDX when interleaving*/
	else if (! (track_decode_info->decode_option & RTP_PCK_SIGNAL_TS) && (track_decode_info->decode_option & RTP_PCK_USE_INTERLEAVING)) {
		track_decode_info->decode_option |= RTP_PCK_SIGNAL_AU_IDX;
	}

	if (track_decode_info->decode_option & RTP_PCK_SIGNAL_TS) {
		/*compute CTS delta*/
		tot_delta = k*abg_ts;
		track_decode_info->rtp_mp4_es_generic_info.cts_delta_length = isom_get_bit_operation_bit_size(k*abg_ts);

		/*compute DTS delta. Delta is ALWAYS from the CTS of the same sample*/ 
        if (max_dts) {
			track_decode_info->rtp_mp4_es_generic_info.dts_delta_length = isom_get_bit_operation_bit_size(max_dts);
        }
        else {
			track_decode_info->rtp_mp4_es_generic_info.dts_delta_length = 0;
        }
	}

	if ((track_decode_info->decode_option & RTP_PCK_SIGNAL_AU_IDX) && track_decode_info->sl_config_descriptor.au_seqnum_length) {
		track_decode_info->rtp_mp4_es_generic_info.index_length = track_decode_info->sl_config_descriptor.au_seqnum_length;
		/*and k-1 AUs in Delta*/
		track_decode_info->rtp_mp4_es_generic_info.index_delta_length = (track_decode_info->decode_option & RTP_PCK_USE_INTERLEAVING) ? isom_get_bit_operation_bit_size(k-1) : 0;
	}

	/*RAP*/
	if (track_decode_info->sl_config_descriptor.use_random_access_point_flag && (track_decode_info->decode_option & RTP_PCK_SIGNAL_RAP)) {
		track_decode_info->rtp_mp4_es_generic_info.random_access_indication = 1;
	} else {
		track_decode_info->rtp_mp4_es_generic_info.random_access_indication = 0;
	}

check_header:

	/*IV delta only if interleaving (otherwise reconstruction from IV is trivial)*/
	if (iv_length && (track_decode_info->decode_option & RTP_PCK_USE_INTERLEAVING)) {
		track_decode_info->rtp_mp4_es_generic_info.iv_delta_length = (UInt8)isom_get_bit_operation_bit_size(max_size);
	}
	/*ISMACryp video mode*/
	if ((track_decode_info->rtp_mp4_es_generic_info.stream_type==ISOM_STREAM_VISUAL) && (track_decode_info->rtp_mp4_es_generic_info.object_type_indication==0x20)
		&& (track_decode_info->decode_option & RTP_PCK_SIGNAL_RAP) && track_decode_info->rtp_mp4_es_generic_info.iv_length 
		&& !(track_decode_info->decode_option & RTP_PCK_SIGNAL_AU_IDX) && !(track_decode_info->decode_option & RTP_PCK_SIGNAL_SIZE)
		/*shall have SignalTS*/
		&& (track_decode_info->decode_option & RTP_PCK_SIGNAL_TS) && !(track_decode_info->decode_option & RTP_PCK_USE_MULTI)
	) {
		strcpy(track_decode_info->rtp_mp4_es_generic_info.mode, "mpeg4-video");
	}
	/*ISMACryp AVC video mode*/
	else if ((track_decode_info->rtp_mp4_es_generic_info.stream_type==ISOM_STREAM_VISUAL) && (track_decode_info->rtp_mp4_es_generic_info.object_type_indication==0x21)
		&& (track_decode_info->decode_option & RTP_PCK_SIGNAL_RAP) && track_decode_info->rtp_mp4_es_generic_info.iv_length 
		&& !(track_decode_info->decode_option & RTP_PCK_SIGNAL_AU_IDX) && !(track_decode_info->decode_option & RTP_PCK_SIGNAL_SIZE)
		/*shall have SignalTS*/
		&& (track_decode_info->decode_option & RTP_PCK_SIGNAL_TS) && !(track_decode_info->decode_option & RTP_PCK_USE_MULTI)
	) {
		strcpy(track_decode_info->rtp_mp4_es_generic_info.mode, "avc-video");
	}

	/*check if we use AU rtp_header_info or not*/
	if (!track_decode_info->rtp_mp4_es_generic_info.size_length 
		&& !track_decode_info->rtp_mp4_es_generic_info.index_length 
		&& !track_decode_info->rtp_mp4_es_generic_info.index_delta_length
		&& !track_decode_info->rtp_mp4_es_generic_info.dts_delta_length  
		&& !track_decode_info->rtp_mp4_es_generic_info.cts_delta_length
		&& !track_decode_info->rtp_mp4_es_generic_info.random_access_indication
		&& !track_decode_info->rtp_mp4_es_generic_info.iv_length
		&& !track_decode_info->rtp_mp4_es_generic_info.ki_length
	) {
		track_packetize_info->has_au_header= 0;
	} else {
		track_packetize_info->has_au_header = 1;
	}
}

isom_decoder_config_t *rtpfile_get_decoder_config(isom_box_trak_t *box_trak, UInt32 stream_description_index)
{
    isom_esd_t *esd;
    isom_descriptor_t *decInfo;

    if (!box_trak) {
        return NULL;
    }

    //get the ESD (possibly emulated)
    rtpfile_get_esd_from_stsd(box_trak->box_mdia->box_minf->box_stbl->box_stsd, stream_description_index, &esd, 0);
    if (!esd) {
        return NULL;
    }
    decInfo = (isom_descriptor_t *) esd->decoder_config;
    esd->decoder_config = NULL;
    isom_delete_odf_descriptor((isom_descriptor_t *) esd);
    return (isom_decoder_config_t *)decInfo;
}


/*
* Desc : 
*/
UInt32 ut_encode_base64(char *_in, UInt32 in_size, char *_out, UInt32 out_size)
{
    SInt32 padding;
    UInt32 i = 0, j = 0;
    unsigned char *in = (unsigned char *)_in;
    unsigned char *out = (unsigned char *)_out;
    char base_64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    if (out_size < (in_size * 4 / 3)) return 0;
    
    while (i < in_size) {
        padding = 3 - (in_size - i);
        if (padding == 2) {
            out[j] = base_64[in[i]>>2];
            out[j+1] = base_64[(in[i] & 0x03) << 4];
            out[j+2] = '=';
            out[j+3] = '=';
        } else if (padding == 1) {
            out[j] = base_64[in[i]>>2];
            out[j+1] = base_64[((in[i] & 0x03) << 4) | ((in[i+1] & 0xf0) >> 4)];
            out[j+2] = base_64[(in[i+1] & 0x0f) << 2];
            out[j+3] = '=';
        } else{
            out[j] = base_64[in[i]>>2];
            out[j+1] = base_64[((in[i] & 0x03) << 4) | ((in[i+1] & 0xf0) >> 4)];
            out[j+2] = base_64[((in[i+1] & 0x0f) << 2) | ((in[i+2] & 0xc0) >> 6)];
            out[j+3] = base_64[in[i+2] & 0x3f];
        }
        i += 3;
        j += 4;
    }
    return j;
}

UInt32 ut_encode_base16(char *_in, UInt32 in_size, char *_out, UInt32 out_size)
{
    UInt32 i = 0;
    char base_16[] = "0123456789abcdef";
    unsigned char *in = (unsigned char *)_in;
    unsigned char *out = (unsigned char *)_out;
    
    if (out_size < (in_size * 2)+1) {
        return 0;
    }
    
    for (i=0;i<in_size;i++) {
        out[2*i] = base_16[((in[i] & 0xf0) >> 4)];
        out[2*i+1] = base_16[(in[i] & 0x0f)];	
    }
    out[(in_size * 2)] = 0;
    
    return i * 2;
}



Bool16 rtpfile_get_visual_info(isom_box_trak_t *box_trak, UInt32 stream_description_index, UInt32 *width, UInt32 *height)
{
    isom_sample_entry_box_t *entry;
    if (!box_trak) {
        return false;
    }
    
    if (!box_trak->box_mdia->box_minf->box_stbl->box_stsd) {
        return false;
    }
    if (!stream_description_index || 
            stream_description_index > isom_get_list_count(box_trak->box_mdia->box_minf->box_stbl->box_stsd->box_list)) {
        return false;
    }
    
    entry = (isom_sample_entry_box_t *)isom_get_list(box_trak->box_mdia->box_minf->box_stbl->box_stsd->box_list, stream_description_index - 1);
    //no support for generic sample entries (eg, no MPEG4 descriptor)
    if (entry == NULL) {
        return false;
    }
    
    //valid for MPEG visual, JPG and 3GPP H263
    switch (entry->type) {
    case ISOM_BOX_TYPE_ENCV:
    case ISOM_SUBTYPE_3GP_H263:
    case ISOM_BOX_TYPE_MP4V:
    case ISOM_BOX_TYPE_AVC1:
        *width = ((isom_visual_sample_entry_box_t *)entry)->Width;
        *height = ((isom_visual_sample_entry_box_t *)entry)->Height;
        return true;
    default:
        return false;
    }
}


rs_status_t rtpfile_get_payload_name(track_decode_info_t *rtpb, char *sz_payload_name, char *sz_media_name)
{
    UInt32 decode_option = rtpb->decode_option;
    
    switch (rtpb->rfc_codec_type) {
    case RTP_PAYT_MPEG4:
        if ((rtpb->rtp_mp4_es_generic_info.stream_type==ISOM_STREAM_VISUAL) && (rtpb->rtp_mp4_es_generic_info.object_type_indication==0x20)) {
            strcpy(sz_media_name, "video");
            /*ISMACryp video*/
            if ( (decode_option & RTP_PCK_SIGNAL_RAP) && rtpb->rtp_mp4_es_generic_info.iv_length
                && !(decode_option & RTP_PCK_SIGNAL_AU_IDX) && !(decode_option & RTP_PCK_SIGNAL_SIZE)
                && (decode_option & RTP_PCK_SIGNAL_TS) && !(decode_option & RTP_PCK_USE_MULTI)
                ) 
            {
                strcpy(sz_payload_name, "enc-mpeg4-generic");
                return ISOM_OK;
            }
            /*mpeg4-generic*/
            if ( (decode_option & RTP_PCK_SIGNAL_RAP) || (decode_option & RTP_PCK_SIGNAL_AU_IDX) || (decode_option & RTP_PCK_SIGNAL_SIZE) 
                || (decode_option & RTP_PCK_SIGNAL_TS) || (decode_option & RTP_PCK_USE_MULTI) ) {
                strcpy(sz_payload_name, "mpeg4-generic");
                return ISOM_OK;
            } else {
                strcpy(sz_payload_name, "MP4V-ES");
                return ISOM_OK;
            } 
        }
        /*for all other types*/
        if (rtpb->rtp_mp4_es_generic_info.stream_type==ISOM_STREAM_AUDIO) strcpy(sz_media_name, "audio");
        else if (rtpb->rtp_mp4_es_generic_info.stream_type==ISOM_STREAM_MPEGJ) strcpy(sz_media_name, "application");
        else strcpy(sz_media_name, "video");
        strcpy(sz_payload_name, rtpb->rtp_mp4_es_generic_info.iv_length ? "enc-mpeg4-generic" : "mpeg4-generic");
        return ISOM_OK;
    case RTP_PAYT_MPEG12_VIDEO:
        strcpy(sz_media_name, "video");
        strcpy(sz_payload_name, "MPV");
        return ISOM_OK;
    case RTP_PAYT_MPEG12_AUDIO:
        strcpy(sz_media_name, "audio");
        strcpy(sz_payload_name, "MPA");
        return ISOM_OK;
    case RTP_PAYT_H263:
        strcpy(sz_media_name, "video");
        strcpy(sz_payload_name, "H263-1998");
        return ISOM_OK;
    case RTP_PAYT_AMR:
        strcpy(sz_media_name, "audio");
        strcpy(sz_payload_name, "AMR");
        return ISOM_OK;
    case RTP_PAYT_AMR_WB:
        strcpy(sz_media_name, "audio");
        strcpy(sz_payload_name, "AMR-WB");
        return ISOM_OK;
    case RTP_PAYT_3GPP_TEXT:
        strcpy(sz_media_name, "text");
        strcpy(sz_payload_name, "3gpp-tt");
        return ISOM_OK;
    case RTP_PAYT_H264_AVC:
        strcpy(sz_media_name, "video");
        strcpy(sz_payload_name, "H264");
        return ISOM_OK;
    case RTP_PAYT_QCELP:
        strcpy(sz_media_name, "audio");
        strcpy(sz_payload_name, "QCELP");
        return ISOM_OK;
    case RTP_PAYT_EVRC_SMV:
        strcpy(sz_media_name, "audio");
        strcpy(sz_payload_name, (rtpb->rtp_mp4_es_generic_info.object_type_indication==0xA0) ? "EVRC" : "SMV");
        /*rtp_header_info-free version*/
        //if (rtpb->auh_size<=1) strcat(sz_payload_name, "0");
        return ISOM_OK;
    case RTP_PAYT_LATM: 
        strcpy(sz_media_name, "audio"); 
        strcpy(sz_payload_name, "MP4A-LATM"); 
        return ISOM_OK; 
    default:
        strcpy(sz_media_name, "");
        strcpy(sz_payload_name, "");
        return ISOM_UNKNOWN_CODEC_TYPE;
    }
    return ISOM_ERROR;
}


//rs_status_t ts_initialize_track_sample_control_info(isom_box_trak_t *box_trak,  track_sample_control_info_t **track_sample_control_info)
rs_status_t rtpfile_init_track_sample_control_info(isom_box_trak_t *box_trak,  track_sample_control_info_t **track_sample_control_info)
{
    rs_status_t  result_code = ISOM_UNKNOWN;
    track_sample_control_info_t *temp_track_sample_control_info = NULL;
    isom_box_stbl_t             *box_stbl = box_trak->box_mdia->box_minf->box_stbl;

    temp_track_sample_control_info = (track_sample_control_info_t *)malloc(sizeof(track_sample_control_info_t));
    if (temp_track_sample_control_info == NULL) {
        result_code = ISOM_MEMORY_ERROR;
        return result_code;
    }
    memset(temp_track_sample_control_info, 0x00, sizeof(track_sample_control_info_t));
    *track_sample_control_info = temp_track_sample_control_info;

    /* Sample Table Sample To Chunk */
    if (box_stbl->box_stsc) {
        temp_track_sample_control_info->stsc_current_chunk                  = box_stbl->box_stsc->current_chunk;
        temp_track_sample_control_info->stsc_current_index                  = box_stbl->box_stsc->current_index;
        temp_track_sample_control_info->stsc_first_sample_in_current_chunk  = box_stbl->box_stsc->first_sample_in_current_chunk;
        temp_track_sample_control_info->stsc_ghost_number                   = box_stbl->box_stsc->ghost_number;
    }
    
    /* Sample Table Sync Samples */
    if (box_stbl->box_stss) {
        temp_track_sample_control_info->stss_last_sample_index   = box_stbl->box_stss->last_sample_index;
        temp_track_sample_control_info->stss_last_sync_sample    = box_stbl->box_stss->last_sync_sample;
    }

    /* Sample Table Time to Sample */
    if (box_stbl->box_stts) {
        temp_track_sample_control_info->stts_current_dts         = box_stbl->box_stts->current_dts;
        temp_track_sample_control_info->stts_current_entry_index = box_stbl->box_stts->current_entry_index;
        temp_track_sample_control_info->stts_first_samp_in_entry = box_stbl->box_stts->first_samp_in_entry;
    }

    /* Composition Time To Sample*/
    if (box_stbl->box_ctts) {
        temp_track_sample_control_info->ctts_current_entry_index = box_stbl->box_ctts->current_entry_index;
        temp_track_sample_control_info->ctts_first_samp_in_entry = box_stbl->box_ctts->first_samp_in_entry;
    }

    return ISOM_OK;    
}


Bool16 rtpfile_get_m4a_config(char *dsi, UInt32 dsi_size, m4a_decspec_info_t *cfg)
{
    isom_bit_operation_t *bo;
    const UInt32 m4a_sample_rates[] = 
    {
        96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 
            16000, 12000, 11025, 8000, 7350, 0, 0, 0
    };

    memset(cfg, 0, sizeof(m4a_decspec_info_t));
    if (!dsi || !dsi_size || (dsi_size<2) ) {
        return false;
    }
    bo = isom_init_bit_operation(dsi, dsi_size, ISOM_BITOPER_READ);
    
    cfg->base_object_type = isom_read_bit_operation_int(bo, 5); //sdp mode 설정 sdi 5bit 
    /*extended object type*/
    if (cfg->base_object_type==31) {
        cfg->base_object_type = 32 + isom_read_bit_operation_int(bo, 6);
    }
    cfg->base_sr_index = isom_read_bit_operation_int(bo, 4);
    if (cfg->base_sr_index == 0x0F) {
        cfg->base_sr = isom_read_bit_operation_int(bo, 24);
    } else {
        cfg->base_sr = m4a_sample_rates[cfg->base_sr_index];
    }
    cfg->nb_chan = isom_read_bit_operation_int(bo, 4);
    /*this is 7+1 channels*/
    if (cfg->nb_chan==7) cfg->nb_chan=8;
    
    if (cfg->base_object_type==5) {
        cfg->has_sbr = 1;
        cfg->sbr_sr_index = isom_read_bit_operation_int(bo, 4);
        if (cfg->sbr_sr_index == 0x0F) {
            cfg->sbr_sr = isom_read_bit_operation_int(bo, 24);
        } else {
            cfg->sbr_sr = m4a_sample_rates[cfg->sbr_sr_index];
        }
        cfg->sbr_object_type = isom_read_bit_operation_int(bo, 5);
    }
    
    isom_is_align_bit_operation(bo);
    if (isom_available_bit_operation(bo)>=2) {
        UInt32 sync = isom_read_bit_operation_int(bo, 11);
        if (sync==0x2b7) {
            cfg->sbr_object_type = isom_read_bit_operation_int(bo, 5);
            cfg->has_sbr = (Bool16)isom_read_bit_operation_int(bo, 1);
            if (cfg->has_sbr) {
                cfg->sbr_sr_index = isom_read_bit_operation_int(bo, 4);
                if (cfg->sbr_sr_index == 0x0F) {
                    cfg->sbr_sr = isom_read_bit_operation_int(bo, 24);
                } else {
                    cfg->sbr_sr = m4a_sample_rates[cfg->sbr_sr_index];
                }
            }
        }
    }
    cfg->audioPL = (UInt8)rtpfile_get_m4a_profile_level_id(cfg); // mpeg-4 audio profile-level-id
    isom_delete_bit_operation(bo);
    return true;
}


Bool16 rtpfile_get_m4v_config(char *rawdsi, UInt32 rawdsi_size, m4v_decspecinfo_t *dsi)
{
    m4v_parser_t *vparse;
    if (!rawdsi || !rawdsi_size) {
        return false;
    }
    vparse = rtpfile_create_m4v_parser(rawdsi, rawdsi_size, 0);
    if (!rtpfile_parse_m4v_config(vparse, dsi)){
        rtpfile_delete_m4v_parser(vparse);
        return false;
    }
    rtpfile_delete_m4v_parser(vparse);
    return true;
}

isom_avc_config_t *rtpfile_get_avc_config(isom_box_trak_t *box_trak, UInt32 description_index)
{
    isom_mpeg_visual_sample_entry_box_t *entry;
    
    if (!box_trak || !description_index) {
        return NULL;
    }

    entry = (isom_mpeg_visual_sample_entry_box_t *)isom_get_list(box_trak->box_mdia->box_minf->box_stbl->box_stsd->box_list, description_index-1);
    if (!entry) {
        return NULL;
    }
    //if (entry->type != TS_ISOM_BOX_TYPE_AVC1) return NULL;
    if (!entry->avc_config) {
        return NULL;
    }
    return rtpfile_duplicate_avc_config(entry->avc_config->config);
}

UInt32 rtpfile_get_m4a_profile_level_id(m4a_decspec_info_t *cfg)
{
    switch (cfg->base_object_type) {
    case 2: /*AAC LC*/
        if (cfg->nb_chan<=2) {
            return (cfg->base_sr<=24000) ? 0x28 : 0x29; /*LC@L1 or LC@L2*/
        }

        return (cfg->base_sr<=48000) ? 0x2A : 0x2B; /*LC@L4 or LC@L5*/
    case 5: /*HE-AAC*/
        if (cfg->nb_chan<=2) {
            return (cfg->base_sr<=24000) ? 0x2C : 0x2D; /*HE@L2 or HE@L3*/
        }

        return (cfg->base_sr<=48000) ? 0x2E : 0x2F; /*HE@L4 or HE@L5*/
        /*default to HQ*/
    default:
        if (cfg->nb_chan<=2) {
            return (cfg->base_sr<24000) ? 0x0E : 0x0F; /*HQ@L1 or HQ@L2*/
        }

        return 0x10; /*HQ@L3*/
    }
}


m4v_parser_t *rtpfile_create_m4v_parser(char *data, UInt32 data_size, Bool16 mpeg12video)
{
    m4v_parser_t *tmp;
    if (!data || !data_size) {
        return NULL;
    }
    tmp = (m4v_parser_t *) malloc(sizeof(m4v_parser_t));
    if (tmp) {
        memset(tmp, 0, sizeof(m4v_parser_t)); 
    }

    tmp->bs = isom_init_bit_operation(data, data_size, ISOM_BITOPER_READ);
    tmp->mpeg12 = mpeg12video;
    return tmp;
}


void rtpfile_delete_m4v_parser(m4v_parser_t *m4v)
{
    isom_delete_bit_operation(m4v->bs);
    free(m4v);
}

Bool16 rtpfile_parse_m4v_config(m4v_parser_t *m4v, m4v_decspecinfo_t *dsi)
{
    if (m4v->mpeg12) {
        return false;
    } 
    else {
        return rtpfile_parse_m4v_config_mpeg4(m4v, dsi);
    }
}


Bool16 rtpfile_parse_m4v_config_mpeg4(m4v_parser_t *m4v, m4v_decspecinfo_t *dsi)
{
    SInt32 o_type;
    UInt8 go, verid, par;
    SInt32 clock_rate;
    const struct { UInt32 w, h; } m4v_sar[6] = { { 0,   0 }, { 1,   1 }, { 12, 11 }, { 10, 11 }, { 16, 11 }, { 40, 33 } };

    if (!m4v || !dsi) {
        return false;
    }
    
    memset(dsi, 0, sizeof(m4v_decspecinfo_t));
    
    go = 1;
    while (go) {
        o_type = rtpfile_read_m4v_object(m4v);
        switch (o_type) {
            /*vosh*/
        case M4V_VOS_START_CODE:
            dsi->VideoPL = (UInt8) isom_read_bit_operation_u8(m4v->bs); //profile-level-id
            break;
            
        case M4V_VOL_START_CODE:
            verid = 0;
            dsi->RAP_stream = (UInt8)isom_read_bit_operation_int(m4v->bs, 1);
            dsi->objectType = (UInt8)isom_read_bit_operation_int(m4v->bs, 8);
            if (isom_read_bit_operation_int(m4v->bs, 1)) {
                verid = (UInt8)isom_read_bit_operation_int(m4v->bs, 4);
                isom_read_bit_operation_int(m4v->bs, 3);
            }
            par = (UInt8)isom_read_bit_operation_int(m4v->bs, 4);
            if (par == 0xF) {
                dsi->par_num = (UInt8)isom_read_bit_operation_int(m4v->bs, 8);
                dsi->par_den = (UInt8)isom_read_bit_operation_int(m4v->bs, 8);
            } else if (par<6) {
                dsi->par_num = (UInt8)m4v_sar[par].w;
                dsi->par_den = (UInt8)m4v_sar[par].h;
            }
            if (isom_read_bit_operation_int(m4v->bs, 1)) {
                isom_read_bit_operation_int(m4v->bs, 3);
                if (isom_read_bit_operation_int(m4v->bs, 1)) isom_read_bit_operation_int(m4v->bs, 79);
            }
            dsi->has_shape = (UInt8)isom_read_bit_operation_int(m4v->bs, 2);
            if (dsi->has_shape && (verid!=1) ) isom_read_bit_operation_int(m4v->bs, 4);
            isom_read_bit_operation_int(m4v->bs, 1);
            /*clock rate*/
            dsi->clock_rate = (UInt16)isom_read_bit_operation_int(m4v->bs, 16);
            /*marker*/
            isom_read_bit_operation_int(m4v->bs, 1);
            
            clock_rate = dsi->clock_rate-1;
            if (clock_rate >= 65536) clock_rate = 65535;
            if (clock_rate > 0) {
                for (dsi->NumBitsTimeIncrement = 1; dsi->NumBitsTimeIncrement < 16; dsi->NumBitsTimeIncrement++)	{	
                    if (clock_rate == 1) break;
                    clock_rate = (clock_rate >> 1);
                }
            } else {
                /*fix from vivien for divX*/
                dsi->NumBitsTimeIncrement = 1;
            }			
            /*fixed FPS stream*/
            dsi->time_increment = 0;
            if (isom_read_bit_operation_int(m4v->bs, 1)) {
                dsi->time_increment = isom_read_bit_operation_int(m4v->bs, dsi->NumBitsTimeIncrement);
            }
            if (!dsi->has_shape) {
                isom_read_bit_operation_int(m4v->bs, 1);
                dsi->width = (UInt16)isom_read_bit_operation_int(m4v->bs, 13);
                isom_read_bit_operation_int(m4v->bs, 1);
                dsi->height = (UInt16)isom_read_bit_operation_int(m4v->bs, 13);
            } else {
                dsi->width = dsi->height = 0;
            }
            /*shape will be done later*/
            isom_is_align_bit_operation(m4v->bs);
            break;
            
        case M4V_VOP_START_CODE:
        case M4V_GOV_START_CODE:
            go = 0;
            break;
            /*EOS*/
        case -1:
            go = 0;
            m4v->current_object_start = (UInt32) isom_get_bit_operation_position(m4v->bs);
            break;
            /*don't interest us*/
        case M4V_UDTA_START_CODE:
        default:
            break;
        }
    }
    return true;
}


void rtpfile_init_sl(isom_sl_config_descriptor_t *slc)
{
    memset(slc, 0, sizeof(isom_sl_config_descriptor_t));
    slc->tag = ISOM_ODF_SLC_TAG;
    slc->use_timestamps_flag = 1;
    slc->timestamp_length = 32;
}



#define M4V_CACHE_SIZE		4096
SInt32 rtpfile_read_m4v_object(m4v_parser_t *m4v)
{
    UInt32 v, bpos, found;
    char m4v_cache[M4V_CACHE_SIZE];
    UInt64 end, cache_start, load_size;
    if (!m4v) {
        return 0;
    }
    bpos = 0;
    found = 0;
    load_size = 0;
    end = 0;
    cache_start = 0;
    v = 0xffffffff;
    while (!end) {
        /*refill cache*/
        if (bpos == (UInt32) load_size) {
            if (!isom_available_bit_operation(m4v->bs)) {
                break;
            }
            load_size = isom_available_bit_operation(m4v->bs);
            if (load_size>M4V_CACHE_SIZE) load_size=M4V_CACHE_SIZE;
            bpos = 0;
            cache_start = isom_get_bit_operation_position(m4v->bs);
            isom_read_bit_operation_data(m4v->bs, m4v_cache, (UInt32) load_size);
        }
        v = ( (v<<8) & 0xFFFFFF00) | ((UInt8) m4v_cache[bpos]);
        bpos++;
        if ((v & 0xFFFFFF00) == 0x00000100) {
            end = cache_start+bpos-4;
            found = 1;
            break;
        }
    }
    if (!found) return -1;
    m4v->current_object_start = (UInt32) end;
    isom_seek_bit_operation(m4v->bs, end+3);
    m4v->current_object_type = isom_read_bit_operation_u8(m4v->bs);
    return (SInt32) m4v->current_object_type;
}
