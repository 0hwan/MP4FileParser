#include "isom_stsd_box.h"



/*
* Desc :
*/
rs_status_t isom_init_box(isom_box_t **newbox, UInt32 type)
{
    switch (type)
    {
    case ISOM_BOX_TYPE_MP4V:
        if (!isom_create_mp4v(newbox, type)) {
            return ISOM_MEMORY_ERROR;
        }
        break; 
    case ISOM_BOX_TYPE_MP4A:
        if (!isom_create_mp4a(newbox, type)) {
            return ISOM_MEMORY_ERROR;
        }
        break;
    case ISOM_BOX_TYPE_ESDS:
        if (!isom_create_esds(newbox)) {
            return ISOM_MEMORY_ERROR;
        }
        break; 
        /*AVC boxes*/
    case ISOM_BOX_TYPE_AVC1:
        if (!isom_create_avc1(newbox, type)) {
            return ISOM_MEMORY_ERROR;
        }
        break; 
    case ISOM_BOX_TYPE_AVCC: 
        if (!isom_create_avcc(newbox)) {
            return ISOM_MEMORY_ERROR;
        }
        break;
    case ISOM_BOX_TYPE_BTRT: 
        if (! isom_create_btrt(newbox)) {
            return ISOM_MEMORY_ERROR;
        }
        break;
    case ISOM_BOX_TYPE_M4DS: 
        if (! isom_create_m4ds(newbox)) {
            return ISOM_MEMORY_ERROR;
        }
        break;

    /*3GPP boxes*/
    case ISOM_SUBTYPE_3GP_AMR:
    case ISOM_SUBTYPE_3GP_AMR_WB:
    case ISOM_SUBTYPE_3GP_EVRC:
    case ISOM_SUBTYPE_K3G_EVRC:
    case ISOM_SUBTYPE_3GP_QCELP:
    case ISOM_SUBTYPE_3GP_SMV:
        if (! isom_create_3gpa(newbox, type)) {
            return ISOM_MEMORY_ERROR;
        }
        break;
    case ISOM_SUBTYPE_3GP_H263: 
        if (!isom_create_3gpv(newbox, type)) {
            return ISOM_MEMORY_ERROR;
        }
		break; 
    case ISOM_BOX_TYPE_DAMR: 
    case ISOM_BOX_TYPE_DEVC: 
    case ISOM_BOX_TYPE_DQCP:
    case ISOM_BOX_TYPE_DSMV:
    case ISOM_BOX_TYPE_D263:
        if (!isom_create_3gpc(newbox, type)) {
            return ISOM_MEMORY_ERROR;
        }
		break; 

    default:
		//return isom_create_default_box(newbox);
        return ISOM_UNKNOWN_CODEC_BOX;
    }
    return ISOM_OK;
}



/*
* Desc :
*/
rs_status_t isom_set_box(isom_box_t *newbox, isom_resource_t *data_resource, UInt32 type)
{
    switch (type)
    {
    case ISOM_BOX_TYPE_MP4V:
        if (!isom_set_mp4v(newbox, data_resource)) {
            return ISOM_MP4V_BOX_SET_FAIL;
        }
        break;
    case ISOM_BOX_TYPE_MP4A:
        if (!isom_set_mp4a(newbox, data_resource)) {
            return ISOM_MP4A_BOX_SET_FAIL;
        }
        break;
    case ISOM_BOX_TYPE_ESDS:
        if (!isom_set_esds(newbox, data_resource)) {
            return ISOM_ESDS_BOX_SET_FAIL;
        }
        break; 
    case ISOM_BOX_TYPE_AVC1:
        if (!isom_set_avc1(newbox, data_resource)) {
            return ISOM_AVC1_BOX_SET_FAIL;
        }
        break; 
    case ISOM_BOX_TYPE_AVCC:
        if (!isom_set_avcc(newbox, data_resource)) {
            return ISOM_AVCC_BOX_SET_FAIL;
        }
        break; 
    case ISOM_BOX_TYPE_BTRT:
        if (!isom_set_btrt(newbox, data_resource)) {
            return ISOM_BTRT_BOX_SET_FAIL;
        }
        break; 
    case ISOM_BOX_TYPE_M4DS:
        if (!isom_set_m4ds(newbox, data_resource)) {
            return ISOM_M4DS_BOX_SET_FAIL;
        }
        break; 

    /*3GPP boxes*/
    case ISOM_SUBTYPE_3GP_AMR:
    case ISOM_SUBTYPE_3GP_AMR_WB:
    case ISOM_SUBTYPE_3GP_EVRC:
    case ISOM_SUBTYPE_K3G_EVRC:
    case ISOM_SUBTYPE_3GP_QCELP:
    case ISOM_SUBTYPE_3GP_SMV:
        if (!isom_set_3gpa(newbox, data_resource)) {
            return ISOM_3GPA_BOX_SET_FAIL;
        }
        break;
    case ISOM_SUBTYPE_3GP_H263: 
        if (!isom_set_3gpv(newbox, data_resource)) {
            return ISOM_3GPV_BOX_SET_FAIL;
        }
        break; 
    case ISOM_BOX_TYPE_DAMR:
    case ISOM_BOX_TYPE_DEVC: 
    case ISOM_BOX_TYPE_DQCP:
    case ISOM_BOX_TYPE_DSMV:
    case ISOM_BOX_TYPE_D263:
        if (!isom_set_3gpc(newbox, data_resource)) {
            return ISOM_3GPC_BOX_SET_FAIL;
        }
		break; 

    default:
		//return isom_set_default_box(newbox, data_resource);
        return ISOM_UNKNOWN_CODEC_BOX;
    }     
    return ISOM_OK;
}

/*
* Desc : stsd box 하위 box delete
*/
void isom_delete_box(isom_box_t *box)
{
    switch (box->type)
    {
    case ISOM_BOX_TYPE_MP4V:
        isom_delete_mp4v(box); 
		break;
    case ISOM_BOX_TYPE_MP4A:
        isom_delete_mp4a(box);
        break;
    case ISOM_BOX_TYPE_AVCC:
        isom_delete_avcc(box);
        break;
    case ISOM_BOX_TYPE_AVC1:
        isom_delete_mp4v(box);
        break;
    case ISOM_BOX_TYPE_M4DS:
        isom_delete_m4ds(box);
        break;
    case ISOM_BOX_TYPE_ESDS:
        isom_delete_esds(box);
        break;
    case ISOM_BOX_TYPE_BTRT:
        isom_delete_btrt(box);
        break;
    case ISOM_SUBTYPE_3GP_AMR:
    case ISOM_SUBTYPE_3GP_AMR_WB:
    case ISOM_SUBTYPE_3GP_EVRC:
    case ISOM_SUBTYPE_K3G_EVRC:
    case ISOM_SUBTYPE_3GP_QCELP:
    case ISOM_SUBTYPE_3GP_SMV:
        isom_delete_3gpa(box);
        return;
    case ISOM_SUBTYPE_3GP_H263: 
        isom_delete_3gpv(box);
        return;
    case ISOM_BOX_TYPE_DAMR: 
    case ISOM_BOX_TYPE_DEVC: 
    case ISOM_BOX_TYPE_DQCP:
    case ISOM_BOX_TYPE_DSMV:
    case ISOM_BOX_TYPE_D263: 
		isom_delete_3gpc(box);
        return;
    default:
		isom_delete_default(box);
        break;
    }
    return;
}

/*
* Desc : 
*/
void isom_init_full_box(isom_box_t *a)
{
    isom_full_box_t *ptr = (isom_full_box_t *)a;
    if (! ptr) {
        return;
    }

    ptr->flags = 0;
    ptr->version = 0;
}

/*
* Desc : version, flag
*/
Bool16 isom_set_full_box(isom_box_t *box, isom_resource_t *data_resource)
{
    UInt32 temp_uint32;
    UInt64 pos;
    isom_full_box_t *self = (isom_full_box_t *) box;
    
    if (box->size < 4) {
        return false;
    }
    pos = data_resource->res_mem->mem_size - box->size;
    isom_read_resource_uint32(data_resource, pos, &temp_uint32, NULL);
    self->version = (UInt8)((temp_uint32 >> 24) & 0x000000ff);
    self->flags = temp_uint32 & 0x00ffffff;
    
    box->size -= 4;
    return true;
}


/*
* Desc : visual 공통 속성 read  
*/
Bool16 isom_set_visual_sample(isom_visual_sample_entry_box_t *ent, isom_resource_t *data_resource)
{
    if (ent == NULL || data_resource == NULL) {
        return false;
    }

    if (ent->size < 78) {
        return false;
    }
	ent->size -= 78;
    
    isom_read_resource_bytes(data_resource, STSDDESCPOS_HEADER, (char *)ent->reserved, 6, NULL);
    isom_read_resource_uint16(data_resource, STSDDESCPOS_HEADER + 6, &ent->data_reference_index, NULL);
    isom_read_resource_uint16(data_resource, STSDDESCPOS_HEADER + 8, &ent->version, NULL);
    isom_read_resource_uint16(data_resource, STSDDESCPOS_HEADER + 10, &ent->revision, NULL);
    isom_read_resource_uint32(data_resource, STSDDESCPOS_HEADER + 12, &ent->vendor, NULL);
    isom_read_resource_uint32(data_resource, STSDDESCPOS_HEADER + 16, &ent->temporal_quality, NULL);
    isom_read_resource_uint32(data_resource, STSDDESCPOS_HEADER + 20, &ent->spacial_quality, NULL);
    isom_read_resource_uint16(data_resource, STSDDESCPOS_HEADER + 24, &ent->Width, NULL);
    isom_read_resource_uint16(data_resource, STSDDESCPOS_HEADER + 26, &ent->Height, NULL);
    isom_read_resource_uint32(data_resource, STSDDESCPOS_HEADER + 28, &ent->horiz_res, NULL);
    isom_read_resource_uint32(data_resource, STSDDESCPOS_HEADER + 32, &ent->vert_res, NULL);
    isom_read_resource_uint32(data_resource, STSDDESCPOS_HEADER + 36, &ent->entry_data_size, NULL);
    isom_read_resource_uint16(data_resource, STSDDESCPOS_HEADER + 40, &ent->frames_per_sample, NULL);
    isom_read_resource_bytes (data_resource, STSDDESCPOS_HEADER + 42, (char *)ent->compressor_name, 32, NULL);
    ent->compressor_name[32] = 0;
    
    isom_read_resource_uint16(data_resource, STSDDESCPOS_HEADER + 74, &ent->bit_depth, NULL);
    isom_read_resource_uint16(data_resource, STSDDESCPOS_HEADER + 76, &ent->color_table_index, NULL);

    return true;
}

/*
* Desc : audio 공통 속성 read  
*/
Bool16 isom_set_audio_sample_entry(isom_audio_sample_entry_box_t *ent, isom_resource_t *data_resource)
{
    if (ent == NULL || data_resource == NULL) {
        return false;
    }
    if (data_resource->res_mem->mem_size - STSDDESCPOS_HEADER < 28) {
        return false;
    }

	isom_read_resource_bytes(data_resource, STSDDESCPOS_HEADER, (char *)ent->reserved, 6, NULL);
    isom_read_resource_uint16(data_resource, STSDDESCPOS_HEADER + 6, &ent->data_reference_index, NULL);
    isom_read_resource_uint16(data_resource, STSDDESCPOS_HEADER + 8, &ent->version, NULL);
    isom_read_resource_uint16(data_resource, STSDDESCPOS_HEADER + 10, &ent->revision, NULL);
    isom_read_resource_uint32(data_resource, STSDDESCPOS_HEADER + 12, &ent->vendor, NULL);
    isom_read_resource_uint16(data_resource, STSDDESCPOS_HEADER + 16, &ent->channel_count, NULL);

    isom_read_resource_uint16(data_resource, STSDDESCPOS_HEADER + 18, &ent->bitspersample, NULL);
    isom_read_resource_uint16(data_resource, STSDDESCPOS_HEADER + 20, &ent->compression_id, NULL);
    isom_read_resource_uint16(data_resource, STSDDESCPOS_HEADER + 22, &ent->packet_size, NULL);
    isom_read_resource_uint16(data_resource, STSDDESCPOS_HEADER + 24, &ent->samplerate_hi, NULL);
    isom_read_resource_uint16(data_resource, STSDDESCPOS_HEADER + 26, &ent->samplerate_lo, NULL);

    ent->size -= 28;

	if (ent->version == 1) {
        if (ent->size < 16) {
            return false;
        }
		//skip_ 16bytes	
        data_resource->res_mem->mem_pos += 16;
        ent->size -= 16;
	} else if (ent->version == 2) {
        if (ent->size < 72) {
            return false;
        }
		//skip 72bytes
        data_resource->res_mem->mem_pos += 72;
        ent->size -= 72;
	}

    return true;
}

/*
* Desc : box parsing 후 add box 
*/
Bool16 isom_set_box_list(isom_box_t *parent,  isom_resource_t *data_resource, Bool16 (*add_box)(isom_box_t *par, isom_box_t *b))
{
	isom_box_t *a;
	while (parent->size) {
        if (isom_parse_box(&a, data_resource) != ISOM_OK) {
            if (a) {
                isom_delete_box(a);
            }
			return false;
		}
		if (parent->size < a->size) {
            if (a) {
                isom_delete_box(a);
            }
			return false;
		}
		parent->size -= a->size;
        if (!add_box(parent, a)) {
            isom_delete_box(a);
            return false;
        }
	}
    return true;
}


/*
* Desc :
*/
rs_status_t isom_parse_box(isom_box_t **outbox, isom_resource_t *data_resource)
{
    rs_status_t   result_code = ISOM_UNKNOWN;
    UInt32      type, hdr_size;
    UInt64      size, start, end;
    char        uuid[16];
    isom_box_t     *newbox;

    if (outbox == NULL || data_resource == NULL) {
        return ISOM_BAD_PARAM;
    }

    *outbox = NULL;

    start = data_resource->res_mem->mem_pos;

    isom_read_resource_uint32_to_uint64(data_resource, start, &size, NULL);
    hdr_size = 4;

    isom_read_resource_uint32(data_resource, start + hdr_size, &type, NULL);
    hdr_size += 4;

    // uuid
    memset(uuid, 0, 16);
    if (type == ISOM_BOX_TYPE_UUID ) {
        isom_read_resource_bytes(data_resource, start + hdr_size, uuid, 16, NULL);
        hdr_size += 16;
	}

    // large box
    if (size == 1) {
        isom_read_resource_uint64(data_resource, start + hdr_size, &size, NULL);
        hdr_size += 8;
	}
    
    if ( size < hdr_size ) {
        return ISOM_CODEC_BOX_SIZE_MISMATCH;
	}

    if ((result_code = isom_init_box(&newbox, type)) != ISOM_OK) {
        return result_code;
    }

    if (type==ISOM_BOX_TYPE_UUID) {
        memcpy(((isom_uuid_box_t *)newbox)->uuid, uuid, 16);
    }

    if (!newbox->type) {
        newbox->type = type;
    }

    end = data_resource->res_mem->mem_size;
    if (size - hdr_size > end ) {
        newbox->size = size - hdr_size - end;
        *outbox = newbox;
        return ISOM_CODEC_BOX_SIZE_MISMATCH;
	}

    newbox->size = size - hdr_size;

    if ((result_code = isom_set_box( newbox, data_resource, type)) != ISOM_OK) {
        return result_code;
    }

    newbox->size = size;
    end = data_resource->res_mem->mem_pos;
    *outbox = newbox;
    return ISOM_OK;

}


/*
* Desc :
*/
Bool16 isom_set_odf_descriptor(char *raw_desc, UInt32 desc_size, isom_descriptor_t **out_desc)
{
    UInt32 size;
	isom_resource_t *tmp_source;

    if (!raw_desc || !desc_size) {
        return false;
    }
    if (isom_init_resource_mem(&tmp_source, raw_desc, desc_size) != ISOM_OK) {
        return false;
    }

    size = 0;
    if (!isom_parse_odf_descriptor(tmp_source, out_desc, &size)) {
        isom_delete_resource_mem(tmp_source);
        return false;
    }
    size += isom_get_odf_field_size(size);

    isom_delete_resource_mem(tmp_source);

    return true;
}


/*
* Desc : esds 하위 delete
*/
Bool16 isom_init_odf_descriptor(isom_descriptor_t **desc, UInt8 tag)
{
    isom_descriptor_t *newdesc;
    
    if (!isom_create_odf_descriptor(tag, &newdesc)) {
        return false;
    }
    newdesc->tag = tag;
    *desc = newdesc;
    
    return true;
}

Bool16 isom_set_odf_slc_predef(isom_sl_config_descriptor_t *sl)
{
    if (!sl) {
        return false;
    }
    
    switch (sl->predefined) {
    case SLPredef_MP4:
        sl->use_access_unit_start_flag = 0;
        sl->use_access_unit_end_flag = 0;
        //each packet is an AU, and we need RAP signaling
        sl->use_random_access_point_flag = 1;
        sl->has_random_access_units_only_flag = 0;
        sl->use_padding_flag = 0;
        //in MP4 file, we use TimeStamps
        sl->use_timestamps_flag = 1;
        sl->use_idle_flag = 0;
        sl->duration_flag = 0;
        sl->timestamp_length = 0;
        sl->ocr_length = 0;
        sl->au_length = 0;
        sl->instant_bitrate_length = 0;
        sl->degradation_priority_length = 0;
        sl->au_seqnum_length = 0;
        sl->packet_seqnum_length = 0;
        break;
        
    case SLPredef_Null:
        sl->use_access_unit_start_flag = 0;
        sl->use_access_unit_end_flag = 0;
        sl->use_random_access_point_flag = 0;
        sl->has_random_access_units_only_flag = 0;
        sl->use_padding_flag = 0;
        sl->use_timestamps_flag = 0;
        sl->use_idle_flag = 0;
        sl->au_length = 0;
        sl->degradation_priority_length = 0;
        sl->au_seqnum_length = 0;
        sl->packet_seqnum_length = 0;
        
        //for MPEG4 IP
        sl->timestamp_resolution = 1000;
        sl->timestamp_length = 32;
        break;
    case SLPredef_SkipSL:
        sl->predefined = SLPredef_SkipSL;
        break;
        /*handle all unknown predefined as predef-null*/
    default:
        sl->use_access_unit_start_flag = 0;
        sl->use_access_unit_end_flag = 0;
        sl->use_random_access_point_flag = 0;
        sl->has_random_access_units_only_flag = 0;
        sl->use_padding_flag = 0;
        sl->use_timestamps_flag = 1;
        sl->use_idle_flag = 0;
        sl->au_length = 0;
        sl->degradation_priority_length = 0;
        sl->au_seqnum_length = 0;
        sl->packet_seqnum_length = 0;
        
        sl->timestamp_resolution = 1000;
        sl->timestamp_length = 32;
        break;
    }
    
    return true;
}


/*
* Desc :
*/
void isom_delete_odf_avc_config(isom_avc_config_t *cfg)
{
    if (!cfg) {
        return;
    }
    while (isom_get_list_count(cfg->sequence_parameter_sets)) {
        isom_avc_config_slot_t *sl = (isom_avc_config_slot_t *)isom_get_list(cfg->sequence_parameter_sets, 0);
        isom_clear_list(cfg->sequence_parameter_sets, 0);
        if (sl->data) free(sl->data);
        free(sl);
    }

    isom_delete_list(cfg->sequence_parameter_sets);
    while (isom_get_list_count(cfg->picture_parameter_sets)) {
        isom_avc_config_slot_t *sl = (isom_avc_config_slot_t *)isom_get_list(cfg->picture_parameter_sets, 0);
        isom_clear_list(cfg->picture_parameter_sets, 0);
        if (sl->data) free(sl->data);
        free(sl);
    }
    isom_delete_list(cfg->picture_parameter_sets);
    free(cfg);
}


/*
* Desc :
*/
Bool16 isom_create_odf_avc_config(isom_avc_config_t **cfg)
{
    isom_avc_config_t *tmp = (isom_avc_config_t *) malloc(sizeof(isom_avc_config_t));
    if (tmp == NULL) {
        return false;
    }
    memset(tmp, 0, sizeof(isom_avc_config_t));

    tmp->sequence_parameter_sets = isom_create_list();
    tmp->picture_parameter_sets = isom_create_list();

    *cfg = tmp;
    return true;
}


/*
* Desc : 
*/
Bool16 isom_set_odf_descriptor_list(char *raw_list, UInt32 raw_size, ut_list_t *desc_list)
{
    isom_resource_t *data_resource;
    UInt32 size, desc_size;
    isom_descriptor_t *desc;
    Bool16 rlt = true;
    
    if (!desc_list || !raw_list || !raw_size) {
        return false;
    }
    
    if (isom_init_resource_mem(&data_resource, raw_list, raw_size) != ISOM_OK){
        return false;
    }

    size = 0;
    while (size < raw_size) {
        if ((rlt = isom_parse_odf_descriptor(data_resource, &desc, &desc_size)) == false){
            goto exit;
        }
        isom_add_list(desc_list, desc);
        size += desc_size + isom_get_odf_field_size(desc_size);
    }
    
exit:
    //then delete our bitstream
    isom_delete_resource_mem(data_resource);
    if (size != raw_size) {
        return false;
    }
    return rlt;
}

/*
* Desc : 
*/
Bool16 isom_delete_odf_descriptor_list(ut_list_t *desc_list)
{
    isom_descriptor_t *tmp;
    
    if (! desc_list) {
        return false;
    }
    
    while (isom_get_list_count(desc_list)) {
        tmp = (isom_descriptor_t*)isom_get_list(desc_list, 0);
        isom_clear_list(desc_list, 0);
        if (!isom_delete_odf_descriptor(tmp)) {
            return false;
        }
    }
    return true;
}

/*
* Desc : 
*/
Bool16 isom_create_odf_esd(isom_descriptor_t **desc)
{
    isom_esd_t *new_desc = (isom_esd_t *) malloc(sizeof(isom_esd_t));
    if (!new_desc) {
        return false;
    }
    memset(new_desc, 0, sizeof(isom_esd_t));
    new_desc->ipi_dataset = isom_create_list();
    new_desc->ipmp_descriptor_pointers = isom_create_list();
    new_desc->extension_descriptors = isom_create_list();
    new_desc->tag = ISOM_ODF_ESD_TAG;
	*desc = (isom_descriptor_t *) new_desc;

    return true;
} //ts_delete_odf_esd(isom_esd_t *esd)


/*
* Desc : esds 하위 delete
*/
Bool16 isom_delete_odf_descriptor(isom_descriptor_t *desc)
{
    switch (desc->tag) {
    
        case ISOM_ODF_ESD_TAG :
            return isom_delete_odf_esd((isom_esd_t *)desc);
        case ISOM_ODF_DCD_TAG :
            return isom_delete_odf_dcd((isom_decoder_config_t *)desc);
        case ISOM_ODF_SLC_TAG:
            return isom_delete_odf_slc((isom_sl_config_descriptor_t *)desc);
        default:
            return isom_delete_odf_default((isom_default_descriptor_t *)desc);
    }

    return false;
}

/*
* Desc : esds descriptor parsing 
*/
Bool16 isom_parse_odf_descriptor(isom_resource_t *data_resource, isom_descriptor_t **desc, UInt32 *desc_size)
{
    UInt64 pos;
    UInt32 size, size_header;
	UInt8 val, tag;
    UInt64 bytes = 0;

    isom_descriptor_t *new_desc;
    if (!data_resource) {
        return false;
    }
    pos = data_resource->res_mem->mem_pos;
    *desc_size = 0;
    //tag
    isom_read_resource_uint8(data_resource, pos, &tag, NULL);
    bytes += 1;
    size_header = 1;
    
    //size
	size = 0;

    do {
        isom_read_resource_uint8(data_resource, pos + bytes, &val, NULL);
        bytes++;
        size_header++;
        size <<= 7;
        size |= val & 0x7F;
    } while (val & 0x80);

	*desc_size = size;
    if (!isom_create_odf_descriptor(tag, &new_desc)) {
        return false;
    }
    if (!new_desc) {
        return false;
	}

    new_desc->tag = tag;
    isom_set_descriptor_read_odf(data_resource, new_desc, *desc_size);

    /*FFMPEG fix*/
    if ((tag==ISOM_ODF_SLC_TAG) && (((isom_sl_config_descriptor_t*)new_desc)->predefined==2)) {
        if (*desc_size==3) {
            *desc_size = 1;
        }
	}

    *desc_size += size_header - isom_get_odf_field_size(*desc_size);
	*desc = new_desc;

    return true;
}


/*
* Desc : 
*/
SInt32 isom_get_odf_field_size(UInt32 size_desc)
{
    if (size_desc < 0x00000080) {
        return 1 + 1;
    } else if (size_desc < 0x00004000) {
        return 2 + 1;
    } else if (size_desc < 0x00200000) {
        return 3 + 1;
    } else if (size_desc < 0x10000000) {
        return 4 + 1;
    } else {
        return -1;
    }
    
}

/*
* Desc : esds 초기화
*/
Bool16 isom_create_odf_descriptor(UInt8 tag, isom_descriptor_t **desc)
{
    switch (tag) {
        case ISOM_ODF_ESD_TAG:
		    return isom_create_odf_esd(desc);
        case ISOM_ODF_DCD_TAG:
            return isom_create_odf_dcd(desc);

        //default. The DecSpecInfo is handled as default
	    //the appropriate track_decode_info (audio, video, bifs...) has to decode the DecSpecInfo alone !
	    case ISOM_ODF_DSI_TAG:
            if (!isom_create_odf_default(desc)) {
                return false;
            }
            if (!*desc) {
                return false;
            }
		    (*desc)->tag = ISOM_ODF_DSI_TAG;
		    return true;
        case ISOM_ODF_SLC_TAG:
            //default : we create it without any predefinition...
		    return isom_init_odf_slc(0, desc);

        case 0:
        case 0xFF:
            return false;
        default:
            //ISO Reserved
            if ( (tag >= ISOM_ODF_ISO_RES_BEGIN_TAG) &&
                (tag <= ISOM_ODF_ISO_RES_END_TAG) ) {
                return false;
            }
            if (!isom_create_odf_default(desc)) {
                return false;
            }
            (*desc)->tag = tag;
            return true;
	}
    return false;
}


/*
* Desc : 
*/
Bool16 isom_delete_odf_esd(isom_esd_t *esd)
{
    if (!esd) {
        return false;
    }

    if (esd->url_string) {
        free(esd->url_string);
    }
    
    if (esd->decoder_config) {
        if (!isom_delete_odf_descriptor((isom_descriptor_t *) esd->decoder_config)) {
            return false;
        }
    }
    if (esd->sl_config_descriptor) {
        if (!isom_delete_odf_descriptor((isom_descriptor_t *) esd->sl_config_descriptor)) {
            return false;
        }
    }
    if (esd->ipi_ptr) {
        if (!isom_delete_odf_descriptor((isom_descriptor_t *) esd->ipi_ptr)) {
            return false;
        }
    }
    if (esd->qos) {
        if (!isom_delete_odf_descriptor((isom_descriptor_t *) esd->qos)) {
            return false;
        }
    }
    if (esd->reg_descriptor) {
        if (!isom_delete_odf_descriptor((isom_descriptor_t *) esd->reg_descriptor)) {
            return false;
        }
    }
    if (esd->lang_desc)	{
        if (!isom_delete_odf_descriptor((isom_descriptor_t *) esd->lang_desc)) {
            return false;
        }
    }
    
    if (!isom_delete_odf_all_descriptor_list(esd->ipi_dataset)) {
        return false;
    }
    if (!isom_delete_odf_all_descriptor_list(esd->ipmp_descriptor_pointers)) {
        return false;
    }
    if (!isom_delete_odf_all_descriptor_list(esd->extension_descriptors)) {
        return false;
    }
    free(esd);
    return true;
}

/*
* Desc : 
*/
Bool16 isom_delete_odf_dcd(isom_decoder_config_t *dcd)
{
    if (!dcd) {
        return false;
    }
    
    if (dcd->decoder_specific_info) {
        if (!isom_delete_odf_descriptor((isom_descriptor_t *) dcd->decoder_specific_info)) {
            return false;
        }
    }
    if (!isom_delete_odf_all_descriptor_list(dcd->profile_level_indication_index_descriptor)) {
        return false;
    }

    free(dcd);
    return true;
}

/*
* Desc : 
*/
Bool16 isom_delete_odf_slc(isom_sl_config_descriptor_t *sl)
{
    if (!sl) {
        return false;
    }
    free(sl);
	
    return true;
}

/*
* Desc : 
*/
Bool16 isom_delete_odf_default(isom_default_descriptor_t *dd)
{
    if (!dd) {
        return false;
    }
    
    if (dd->data) {
        free(dd->data);
    }
    free(dd);
    return true;
}
/*
* Desc : esds 속성 read
*/
Bool16 isom_set_descriptor_read_odf(isom_resource_t *data_resource, isom_descriptor_t *desc, UInt64 pos)
{
    switch (desc->tag) {
        case ISOM_ODF_ESD_TAG:
            return isom_set_esd_read_odf(data_resource, (isom_esd_t *)desc, pos);
        case ISOM_ODF_DCD_TAG:
            return isom_set_dcd_read_odf(data_resource, (isom_decoder_config_t *)desc, pos);
        case ISOM_ODF_SLC_TAG :
		    return isom_set_slc_read_odf(data_resource, (isom_sl_config_descriptor_t *)desc, pos);
            //default:
        case ISOM_ODF_DSI_TAG:
        default:
            return isom_set_default_read_odf(data_resource, (isom_default_descriptor_t *)desc, pos);//sdp get dsi parsing

            
    }
    return false;
}


/*
* Desc : 
*/
Bool16 isom_create_odf_dcd(isom_descriptor_t **desc)
{
    isom_decoder_config_t *new_desc = (isom_decoder_config_t *) malloc(sizeof(isom_decoder_config_t));
    if (!new_desc) {
        return false;
    }
    memset(new_desc, 0, sizeof(isom_decoder_config_t));
    
    new_desc->avg_bitrate = 0;
    new_desc->buffer_sizedb = 0;
    new_desc->max_bitrate = 0;
    new_desc->object_type_indication = 0;
    new_desc->stream_type = 0;
    new_desc->up_stream = 0;
    new_desc->decoder_specific_info = NULL;
    new_desc->profile_level_indication_index_descriptor = isom_create_list();
    new_desc->tag = ISOM_ODF_DCD_TAG;
    
    *desc = (isom_descriptor_t *) new_desc;    
    return true;
} //ts_delete_odf_dcd(isom_esd_t *esd)

/*
* Desc : 
*/
isom_esd_t *isom_init_odf_desc_esd(UInt32 sl_predefined)
{
    isom_esd_t *esd;
    isom_init_odf_descriptor((isom_descriptor_t **)&esd, ISOM_ODF_ESD_TAG);
    isom_init_odf_descriptor((isom_descriptor_t **)&esd->decoder_config, ISOM_ODF_DCD_TAG);
    isom_init_odf_descriptor((isom_descriptor_t **)&esd->decoder_config->decoder_specific_info, ISOM_ODF_DSI_TAG);
    isom_init_odf_slc((UInt8)sl_predefined, (isom_descriptor_t **)&esd->sl_config_descriptor);
    return esd;
}

/*
* Desc : descriptor default  셋팅 
*/
Bool16 isom_create_odf_default(isom_descriptor_t **desc)
{
    isom_default_descriptor_t *new_desc = (isom_default_descriptor_t *) malloc(sizeof(isom_default_descriptor_t));
    if (!new_desc) {
        return false;
    }
    
    new_desc->data_length = 0;
    new_desc->data = NULL;
    //set it to the Max allowed
    new_desc->tag = ISOM_ODF_USER_END_TAG;

    *desc = (isom_descriptor_t *) new_desc;  
	return true;
}

/*
* Desc : 
*/
Bool16 isom_init_odf_slc(UInt8 predef, isom_descriptor_t **desc)
{
    isom_sl_config_descriptor_t *new_desc = (isom_sl_config_descriptor_t *) malloc(sizeof(isom_sl_config_descriptor_t));

    if (!new_desc) {
        return false;
    }
    memset(new_desc, 0, sizeof(isom_sl_config_descriptor_t));
    new_desc->tag = ISOM_ODF_SLC_TAG;
    new_desc->predefined = predef;

    if (predef) {
        isom_set_odf_slc_predef(new_desc);
    }
    new_desc->use_timestamps_flag = 1;
    
    *desc = (isom_descriptor_t *)new_desc;
    return true;
}


/*
* Desc : element stream decriptor 셋팅 
*/
Bool16 isom_set_esd_read_odf(isom_resource_t *data_resource, isom_esd_t *esd, UInt64 desc_size)
{
    UInt8 temp_uint8;
	UInt32 ocrflag, urlflag, streamdependflag, tmp_size;//, read;
    UInt64 bytes, pos;

    if (!esd) {
        return false;
    }
    
    pos = data_resource->res_mem->mem_pos;
    bytes = 0;

    isom_read_resource_uint16(data_resource, pos + bytes, &esd->esid, NULL);
    bytes += 2;

    isom_read_resource_uint8(data_resource, pos + bytes, &temp_uint8, NULL);
    bytes += 1;

    streamdependflag = (UInt32)((temp_uint8 >> 7) & 0x00000001);
    urlflag = (UInt32)((temp_uint8 >> 6) & 0x00000001);
    ocrflag = (UInt32)((temp_uint8 >> 5) & 0x00000001);
    esd->stream_priority = (UInt8)(temp_uint8 & 0x0000001f);    

    if (streamdependflag) {
        isom_read_resource_uint16(data_resource, pos + bytes, &esd->depends_onesid, NULL);
        bytes += 2;

	}
    /*
    if (urlflag) {
        e = isom_odf_read_url_string(data_resource, pos + nbBytes, &esd->URLString, &read);
        if (e) {
            return e;
        }
        bytes += read;
	}
    */
    if (ocrflag) {
        isom_read_resource_uint16(data_resource, pos + bytes, &esd->ocr_esid, NULL);
        bytes += 2;
	}

    while (bytes < desc_size) {
        isom_descriptor_t *tmp = NULL;

        if (!isom_parse_odf_descriptor(data_resource, &tmp, &tmp_size)) {
            return false;
        }

        if (!tmp) {
            return false;
        }
        if (!isom_add_descriptor_to_esd(esd, tmp)) {
            return false;
        }

        bytes += tmp_size + isom_get_odf_field_size(tmp_size);
        
        //apple fix
        if (!tmp_size) {
            bytes = desc_size;
        }
	}
    return true;
}

/*
* Desc : 
*/
Bool16 isom_delete_odf_all_descriptor_list(ut_list_t *descList)
{
    isom_descriptor_t*tmp;
    UInt32 i;
    //no error if NULL chain...
    if (!descList) {
        return true;
    }
    i=0;
    while ((tmp = (isom_descriptor_t*)isom_get_list_enum(descList, &i))) {
        if (!isom_delete_odf_descriptor(tmp)) {
            return false;
        }
    }
    isom_delete_list(descList);
    return true;
}

/*
* Desc : 
*/
Bool16 isom_set_slc_read_odf(isom_resource_t *data_resource, isom_sl_config_descriptor_t *sl, UInt64 desc_size)
{
    UInt8   temp_uint8;
    UInt16  temp_uint16;
    UInt32  reserved, bytes = 0;
    UInt64  pos = data_resource->res_mem->mem_pos;

    if (!sl) {
        return false;
    }
    
    //APPLE fix
    if (!desc_size) {
        sl->predefined = SLPredef_MP4;
        return isom_set_odf_slc_predef(sl);
    }
    
    isom_read_resource_uint8(data_resource, pos + bytes, &sl->predefined, NULL);
    bytes += 1;
    
    //MPEG4 IP fix
    if (!sl->predefined && bytes==desc_size) {
        sl->predefined = SLPredef_Null;
        isom_set_odf_slc_predef(sl);
        return true;
    }
    
    if (sl->predefined) {
        //predefined configuration
        if (!isom_set_odf_slc_predef(sl)) {
            return false;
        }
    }
    else {
        isom_read_resource_uint8(data_resource, pos + bytes, &temp_uint8, NULL);
        bytes += 1;
        
        sl->use_access_unit_start_flag = (temp_uint8 >> 7) & 0x00000001;
        sl->use_access_unit_end_flag = (temp_uint8 >> 6) & 0x00000001;
        sl->use_random_access_point_flag = (temp_uint8 >> 5) & 0x00000001;
        sl->has_random_access_units_only_flag = (temp_uint8 >> 4) & 0x00000001;
        sl->use_padding_flag = (temp_uint8 >> 3) & 0x00000001;
        sl->use_timestamps_flag = (temp_uint8 >> 2) & 0x00000001;
        sl->use_idle_flag = (temp_uint8 >> 1) & 0x00000001;
        sl->duration_flag = (temp_uint8 & 0x00000001);

        isom_read_resource_uint32(data_resource, pos + bytes, &sl->timestamp_resolution, NULL);
        bytes += 4;

        isom_read_resource_uint32(data_resource, pos + bytes, &sl->ocr_resolution, NULL);
        bytes += 4;
        
        isom_read_resource_uint8(data_resource, pos + bytes, &sl->timestamp_length, NULL);
        bytes += 1;
        if (sl->timestamp_length > 64) {
            return false;
        }
        
        isom_read_resource_uint8(data_resource, pos + bytes, &sl->ocr_length, NULL);
        bytes += 1;
        if (sl->ocr_length > 64) {
            return false;
        }
        
        isom_read_resource_uint8(data_resource, pos + bytes, &sl->au_length, NULL);
        bytes += 1;
        if (sl->au_length > 32) {
            return false;
        }

        isom_read_resource_uint8(data_resource, pos + bytes, &sl->instant_bitrate_length, NULL);
        bytes += 1;
        
        isom_read_resource_uint16(data_resource, pos + bytes, &temp_uint16, NULL);
        bytes += 2;
        
        sl->degradation_priority_length = (UInt8)((temp_uint16 >> 12) & 0x0000000F); //4bit

        sl->au_seqnum_length = (UInt8)((temp_uint16 >> 7) & 0x0000001F);//5bit
        if (sl->au_seqnum_length > 16) {
            return false;
        }

        sl->packet_seqnum_length = (UInt8)((temp_uint16 >> 2) & 0x0000001F);//5bit
        if (sl->packet_seqnum_length > 16) {
            return false;
        }
        
        reserved = (UInt32)((temp_uint16 >> 2) & 0x00000003);//2bit
    }
    
    if (sl->duration_flag) {
        isom_read_resource_uint32(data_resource, pos + bytes, &sl->timescale, NULL);
        bytes += 4;

        isom_read_resource_uint16(data_resource, pos + bytes, &sl->au_duration, NULL);
        bytes += 2;

        isom_read_resource_uint16(data_resource, pos + bytes, &sl->cu_duration, NULL);
        bytes += 2;
    }
    if (!sl->use_timestamps_flag) {
        isom_read_resource_uint64(data_resource, pos + bytes, &sl->startdts, NULL);
        isom_read_resource_uint64(data_resource, pos + bytes + 8, &sl->startcts, NULL);
        
        bytes += isom_get_sl_time_stamp_bytes_len(sl);
    }
    
    if (desc_size != bytes) {
        return false;
    }
    return true;
}

/*
* Desc : 
*/
Bool16 isom_set_default_read_odf(isom_resource_t *data_resource, isom_default_descriptor_t *dd, UInt64 desc_size)
{
	UInt32 bytes = 0;
    if (! dd) {
        return false;
    }

	dd->data_length = (UInt32)desc_size;
	dd->data = NULL;
	if (desc_size) {
		dd->data = (char*)malloc(dd->data_length);
        if (!dd->data) {
            return false;
        }
        isom_read_resource_bytes(data_resource, data_resource->res_mem->mem_pos, dd->data, dd->data_length, NULL);
		bytes += dd->data_length;
	}
    if (bytes != desc_size) {
        return false;
    }
    return true;
}

/*
* Desc : 
*/
Bool16 isom_add_descriptor_to_esd(isom_esd_t *esd, isom_descriptor_t *desc)
{
    if (!esd || !desc) {
        return false;
    }
    
    switch (desc->tag) {
    case ISOM_ODF_DCD_TAG: // Decoder Config Descriptor : tag(0x04)
        if (esd->decoder_config) {
            return false;
        }
        esd->decoder_config = (isom_decoder_config_t *) desc;
        break;
    case ISOM_ODF_SLC_TAG: // SL(Sync Layer) Config Descriptor : tag(0x06)
        if (esd->sl_config_descriptor) {
            return false;
        }
        esd->sl_config_descriptor = (isom_sl_config_descriptor_t *) desc;
        break;
        
        //the GF_ODF_ISOM_IPI_PTR_TAG is only used in the file format and replaces GF_ODF_IPI_PTR_TAG...
    case ISOM_ODF_ISOM_IPI_PTR_TAG:
    case ISOM_ODF_IPI_PTR_TAG:
        if (esd->ipi_ptr) {
            return false;
        }
        esd->ipi_ptr = (isom_ipi_ptr_t *) desc;
        break;
        
    case ISOM_ODF_QOS_TAG:
        if (esd->qos) {
            return false;
        }
        esd->qos  =(isom_qos_descriptor_t *) desc;
        break;
        
    case ISOM_ODF_LANG_TAG:
        if (esd->lang_desc) {
            return false;
        }
        esd->lang_desc = (isom_language_t *) desc;
        break;
        
    case ISOM_ODF_CI_TAG:
    case ISOM_ODF_SCI_TAG:
        return isom_add_list(esd->ipi_dataset, desc);
        
        //we use the same struct for v1 and v2 IPMP DPs
    case ISOM_ODF_IPMP_PTR_TAG:
        return isom_add_list(esd->ipmp_descriptor_pointers, desc);
        
    case ISOM_ODF_REG_TAG:
        if (esd->reg_descriptor) {
            return false;
        }
        esd->reg_descriptor =(isom_registration_t *) desc;
        break;
        
    case ISOM_ODF_MUXINFO_TAG:
        isom_add_list(esd->extension_descriptors, desc);
        break;
        
    default:
        if ( (desc->tag >= ISOM_ODF_EXT_BEGIN_TAG) &&
            (desc->tag <= ISOM_ODF_EXT_END_TAG) ) {
            return isom_add_list(esd->extension_descriptors, desc);
        }
        isom_delete_odf_descriptor(desc);
        return true;
    }
    
    return true;
}

/*
* Desc : this function gets the real amount of bytes needed to store the timeStamp
*/
UInt32 isom_get_sl_time_stamp_bytes_len(isom_sl_config_descriptor_t *sl)
{
    UInt32 isom_len, isom_bytes;

    if (!sl){
        return 0;
    }
    
    isom_len = sl->timestamp_length * 2;
    isom_bytes = isom_len / 8;
    isom_len = isom_len % 8;
    if (isom_len) {
        isom_bytes += 1;
    }
    return isom_bytes;
}



/*
* Desc : 
*/
Bool16 isom_set_dcd_read_odf(isom_resource_t *data_resource, isom_decoder_config_t *dcd, UInt64 desc_size)
{
    
    UInt32      reserved, tmp_size, bytes = 0;
    UInt8       temp_uint8;
    UInt32      temp_uint32;
    UInt64      pos = data_resource->res_mem->mem_pos;

    if (!dcd) {
        return false;
    }

    bytes = 0;
    isom_read_resource_uint8(data_resource, pos + bytes, &dcd->object_type_indication, NULL);
    bytes += 1;
    isom_read_resource_uint8(data_resource, pos + bytes, &temp_uint8, NULL);

    dcd->stream_type = (temp_uint8 >> 2) & 0x0000003F;
    dcd->up_stream    = (temp_uint8 >> 1) & 0x00000001;
    reserved         = (UInt32)(temp_uint8 & 0x00000001);
    bytes += 1;

    isom_read_resource_uint32(data_resource, pos + bytes, &temp_uint32, NULL);
    dcd->buffer_sizedb = (UInt32)((temp_uint32 >> 8) & 0x00ffffff);
    bytes += 3;

    isom_read_resource_uint32(data_resource, pos + bytes, &dcd->max_bitrate, NULL);
    bytes += 4;
    isom_read_resource_uint32(data_resource, pos + bytes, &dcd->avg_bitrate, NULL);
	bytes += 4;

    while (bytes < desc_size) {
        isom_descriptor_t *tmp = NULL;
        if (!isom_parse_odf_descriptor(data_resource, &tmp, &tmp_size)) {
            return false;
        }
        if (!tmp) {
            return false;
        }
        switch (tmp->tag) {
        case ISOM_ODF_DSI_TAG:
            if (dcd->decoder_specific_info) {
                isom_delete_odf_descriptor(tmp);
                return false;
            }
            dcd->decoder_specific_info = (isom_default_descriptor_t *) tmp;
            break;
            
        case ISOM_ODF_EXT_PL_TAG:
            if (!isom_add_list(dcd->profile_level_indication_index_descriptor, tmp)) {
                isom_delete_odf_descriptor(tmp);
                return false;
            }
            break;
            
            /*iPod fix: delete and aborts, this will create an InvalidDescriptor at the ESD level with a loaded DSI,
            laoding will abort with a partially valid ESD which is all the matters*/
        case ISOM_ODF_SLC_TAG:
            isom_delete_odf_descriptor(tmp);
            return true;
            
            //what the hell is this descriptor ?? Don't know, so delete it !
        default:
            isom_delete_odf_descriptor(tmp);
            break;
		}
        bytes += tmp_size + isom_get_odf_field_size(tmp_size);        
	}
    if (desc_size != bytes) {
        return false;
    }

    return true;
}

/*
* Desc : 
*/
Bool16 isom_copy_odf_desc(isom_descriptor_t *in_desc, isom_descriptor_t **out_desc)
{
    Bool16 e;
    char *desc;
    UInt32 size;
    
    //warning: here we get some data allocated
    e = isom_set_odf_write_descriptor(in_desc, &desc, &size);
    if (e == false) {
        return e;
    }
    e = isom_set_odf_descriptor(desc, size, out_desc);
    free(desc);
    return e;
}

/*
* Desc : 
*/
Bool16 isom_set_odf_write_descriptor(isom_descriptor_t *desc, char **out_encdesc, UInt32 *out_size)
{
    
    isom_bit_operation_t *bo;
    if (!desc || !out_encdesc || !out_size) {
        return false;
    }
    
    *out_encdesc = NULL;
    *out_size = 0;
    
    bo = isom_init_bit_operation(NULL, 0, ISOM_BITOPER_WRITE);

    if (!bo) {
        return false;
    }

    //then encode our desc...
    
    if (!isom_set_high_odf_write_descriptor(bo, desc)) {
        isom_delete_bit_operation(bo);
        return false;
    }
    //then get the content from our bitstream
    isom_get_bit_operation_content(bo, out_encdesc, out_size);
    isom_delete_bit_operation(bo);
    
    return true;
    
}

/*
* Desc : 
*/
Bool16 isom_set_high_odf_write_descriptor(isom_bit_operation_t *bo, isom_descriptor_t *desc)
{
    switch(desc->tag) {
    case ISOM_ODF_ESD_TAG : 
        return isom_set_esd_write_odf(bo, (isom_esd_t *)desc);
    case ISOM_ODF_DCD_TAG : 
        return isom_set_dcd_write_odf(bo, (isom_decoder_config_t *)desc);
    case ISOM_ODF_SLC_TAG : 
        return isom_set_slc_write_odf(bo, (isom_sl_config_descriptor_t *)desc);
    
    default:
        return isom_set_default_write_odf(bo, (isom_default_descriptor_t *)desc);
    }
    return true;
}


/*
* Desc : 
*/
Bool16 isom_set_esd_write_odf(isom_bit_operation_t *bo, isom_esd_t *esd)
{
	UInt32 size;
    if (!esd) {
        return false;
    }

    if (!isom_get_odf_size_descriptor((isom_descriptor_t *)esd, &size)) {
        return false;
    }

    if (!isom_set_base_descriptor_write_odf(bo, esd->tag, size)) {
	    return false;
    }

	isom_write_bit_operation_int(bo, esd->esid, 16);
	isom_write_bit_operation_int(bo, esd->depends_onesid ? 1 : 0, 1);
	isom_write_bit_operation_int(bo, esd->url_string != NULL ? 1 : 0, 1);
	isom_write_bit_operation_int(bo, esd->ocr_esid ? 1 : 0, 1);
	isom_write_bit_operation_int(bo, esd->stream_priority, 5);

	if (esd->depends_onesid) {
		isom_write_bit_operation_int(bo, esd->depends_onesid, 16);
	}
	if (esd->url_string) {
        if (!isom_set_url_string_write_odf(bo, esd->url_string)) {
            return false;
        }
	}


	if (esd->ocr_esid) {
		isom_write_bit_operation_int(bo, esd->ocr_esid, 16);
	}
	if (esd->decoder_config) {
        if (!isom_set_high_odf_write_descriptor(bo, (isom_descriptor_t *) esd->decoder_config)) {
            return false;
        }
		
	}
	if (esd->sl_config_descriptor) {
        if (!isom_set_high_odf_write_descriptor(bo, (isom_descriptor_t *) esd->sl_config_descriptor)) {
            return false;
        }
	}
	if (esd->ipi_ptr) {
        if (!isom_set_high_odf_write_descriptor(bo, (isom_descriptor_t *) esd->ipi_ptr)) {
            return false;
        }
	}
	if (esd->lang_desc) {
        if (!isom_set_high_odf_write_descriptor(bo, (isom_descriptor_t *) esd->lang_desc)) {
            return false;
        }
	}

    if (!isom_set_descriptor_list_write_odf(bo, esd->ipi_dataset)) {
        return false;
    }
	
    if (!isom_set_descriptor_list_write_odf(bo, esd->ipmp_descriptor_pointers)) {
        return false;
    }
	
	if (esd->qos) {
        if (!isom_set_high_odf_write_descriptor(bo, (isom_descriptor_t *) esd->qos)) {
            return false;
        }
	}
	if (esd->reg_descriptor) {
        if (!isom_set_high_odf_write_descriptor(bo, (isom_descriptor_t *) esd->reg_descriptor)) {
            return false;
        }
	}
	return isom_set_descriptor_list_write_odf(bo, esd->extension_descriptors);
}

/*
* Desc : 
*/
Bool16 isom_get_odf_size_dcd(isom_decoder_config_t *dcd, UInt32 *outSize)
{
	UInt32 tmpSize;
    if (! dcd) {
        return false;
    }

	*outSize = 0;
	*outSize += 13;

	if (dcd->decoder_specific_info) {
		//warning: we don't know anything about the structure of a generic DecSpecInfo
		//we check the tag and size of the descriptor, but we most ofthe time can't parse it
		//the decSpecInfo is handle as a defaultDescriptor (opaque data, but same structure....)
        if (!isom_get_odf_size_descriptor((isom_descriptor_t *) dcd->decoder_specific_info , &tmpSize)){
            return false;
        }
		
		*outSize += tmpSize + isom_get_odf_field_size(tmpSize);
	}
    if (!isom_get_odf_size_descriptor_list(dcd->profile_level_indication_index_descriptor, outSize)) {
        return false;
    }

	return true;

}

/*
* Desc : 
*/
Bool16 isom_get_odf_size_slc(isom_sl_config_descriptor_t *sl, UInt32 *outSize)
{
    if (! sl) {
        return false;
    }
    
    *outSize = 1;
    if (! sl->predefined) {
        *outSize += 15;
    }
    if (sl->duration_flag) {
        *outSize += 8;
    }
    if (! sl->use_timestamps_flag) {
        *outSize += isom_get_sl_time_stamp_bytes_len(sl);
    }
    return true;
}

/*
* Desc : 
*/
Bool16 isom_get_odf_size_default(isom_default_descriptor_t *dd, UInt32 *out_size)
{
    if (! dd) {
        return false;
    }
    *out_size  = dd->data_length;
    return true;
}


/*
* Desc : 
*/
Bool16 isom_set_slc_write_odf(isom_bit_operation_t *bo, isom_sl_config_descriptor_t *sl)
{
    UInt32 size;
    if (! sl) {
        return false;
    }
    
    isom_get_odf_size_descriptor((isom_descriptor_t *)sl, &size);
    isom_set_base_descriptor_write_odf(bo, sl->tag, size);
    
    isom_write_bit_operation_int(bo, sl->predefined, 8);
    if (! sl->predefined) {
        isom_write_bit_operation_int(bo, sl->use_access_unit_start_flag, 1);
        isom_write_bit_operation_int(bo, sl->use_access_unit_end_flag, 1);
        isom_write_bit_operation_int(bo, sl->use_random_access_point_flag, 1);
        isom_write_bit_operation_int(bo, sl->has_random_access_units_only_flag, 1);
        isom_write_bit_operation_int(bo, sl->use_padding_flag, 1);
        isom_write_bit_operation_int(bo, sl->use_timestamps_flag, 1);
        isom_write_bit_operation_int(bo, sl->use_idle_flag, 1);
        isom_write_bit_operation_int(bo, sl->duration_flag, 1);
        isom_write_bit_operation_int(bo, sl->timestamp_resolution, 32);
        isom_write_bit_operation_int(bo, sl->ocr_resolution, 32);
        isom_write_bit_operation_int(bo, sl->timestamp_length, 8);
        isom_write_bit_operation_int(bo, sl->ocr_length, 8);
        isom_write_bit_operation_int(bo, sl->au_length, 8);
        isom_write_bit_operation_int(bo, sl->instant_bitrate_length, 8);
        isom_write_bit_operation_int(bo, sl->degradation_priority_length, 4);
        isom_write_bit_operation_int(bo, sl->au_seqnum_length, 5);
        isom_write_bit_operation_int(bo, sl->packet_seqnum_length, 5);
        isom_write_bit_operation_int(bo, 3, 2);	//reserved: 0b11 == 3
    }
    if (sl->duration_flag) {
        isom_write_bit_operation_int(bo, sl->timescale, 32);
        isom_write_bit_operation_int(bo, sl->au_duration, 16);
        isom_write_bit_operation_int(bo, sl->cu_duration, 16);
    }
    if (! sl->use_timestamps_flag) {
        isom_write_bit_operation_long_int(bo, sl->startdts, sl->timestamp_length);
        isom_write_bit_operation_long_int(bo, sl->startcts, sl->timestamp_length);
    }
    
    return true;
}

/*
* Desc : 
*/
Bool16 isom_set_default_write_odf(isom_bit_operation_t *bo, isom_default_descriptor_t *dd)
{
    UInt32 size;
    if (! dd) {
        return false;
    }
    
    if (!isom_get_odf_size_descriptor((isom_descriptor_t *)dd, &size)) {
        return false;
    }

    if (!isom_set_base_descriptor_write_odf(bo, dd->tag, size)) {
        return false;
    }
    
    if (dd->data) {
        isom_write_bit_operation_data(bo, dd->data, dd->data_length);
    }
    return true;
}

/*
* Desc : 
*/
Bool16 isom_set_dcd_write_odf(isom_bit_operation_t *bo, isom_decoder_config_t *dcd)
{
    UInt32 size;
    if (! dcd) {
        return false;
    }
    
    if (!isom_get_odf_size_descriptor((isom_descriptor_t *)dcd, &size)) {
        return false;
    }

    if (!isom_set_base_descriptor_write_odf(bo, dcd->tag, size)) {
        return false;
    }
    
    isom_write_bit_operation_int(bo, dcd->object_type_indication, 8);
    isom_write_bit_operation_int(bo, dcd->stream_type, 6);
    isom_write_bit_operation_int(bo, dcd->up_stream, 1);
    isom_write_bit_operation_int(bo, 1, 1);	//reserved field...
    isom_write_bit_operation_int(bo, dcd->buffer_sizedb, 24);
    isom_write_bit_operation_int(bo, dcd->max_bitrate, 32);
    isom_write_bit_operation_int(bo, dcd->avg_bitrate, 32);
    
    if (dcd->decoder_specific_info) {
        if (!isom_set_high_odf_write_descriptor(bo, (isom_descriptor_t *) dcd->decoder_specific_info)) {
            return false;
        }
    }
    return isom_set_descriptor_list_write_odf(bo, dcd->profile_level_indication_index_descriptor);
    
}

/*
* Desc : 
*/
Bool16 isom_get_odf_size_descriptor(isom_descriptor_t *desc, UInt32 *outSize)
{
    switch(desc->tag) {
    case ISOM_ODF_ESD_TAG : 
        return isom_get_odf_size_esd((isom_esd_t *)desc, outSize);
    case ISOM_ODF_DCD_TAG : 
        return isom_get_odf_size_dcd((isom_decoder_config_t *)desc, outSize);
    case ISOM_ODF_SLC_TAG : 
		return isom_get_odf_size_slc((isom_sl_config_descriptor_t *)desc, outSize);
    default:
        return isom_get_odf_size_default((isom_default_descriptor_t *)desc, outSize);
    }
    return true;
}

/*
* Desc : 
*/
Bool16 isom_set_base_descriptor_write_odf(isom_bit_operation_t *bo, UInt8 tag, UInt32 size)
{
    UInt32 length;
    unsigned char vals[4];
    
    if (!tag ) {
        return false;
    }
    
    length = size;
    vals[3] = (unsigned char) (length & 0x7f);
    length >>= 7;
    vals[2] = (unsigned char) ((length & 0x7f) | 0x80); 
    length >>= 7;
    vals[1] = (unsigned char) ((length & 0x7f) | 0x80); 
    length >>= 7;
    vals[0] = (unsigned char) ((length & 0x7f) | 0x80);
    
    isom_write_bit_operation_int(bo, tag, 8);
    if (size < 0x00000080) {
        isom_write_bit_operation_int(bo, vals[3], 8);
    } else if (size < 0x00004000) {
        isom_write_bit_operation_int(bo, vals[2], 8);
        isom_write_bit_operation_int(bo, vals[3], 8);
    } else if (size < 0x00200000) {
        isom_write_bit_operation_int(bo, vals[1], 8);
        isom_write_bit_operation_int(bo, vals[2], 8);
        isom_write_bit_operation_int(bo, vals[3], 8);
    } else if (size < 0x10000000) {
        isom_write_bit_operation_int(bo, vals[0], 8);
        isom_write_bit_operation_int(bo, vals[1], 8);
        isom_write_bit_operation_int(bo, vals[2], 8);
        isom_write_bit_operation_int(bo, vals[3], 8);
    } else {
        return false;
    }
    return true;
}

/*
* Desc : 
*/
Bool16 isom_set_url_string_write_odf(isom_bit_operation_t *bo, char *str)
{
    UInt32 len;
    /*we accept NULL strings now*/
    if (!str) {
        isom_write_bit_operation_int(bo, 0, 8);
        return true;
    }		
    len = strlen(str);
    if (len > 255) {
        isom_write_bit_operation_int(bo, 0, 8);
        isom_write_bit_operation_int(bo, len, 32);
    } else {
        isom_write_bit_operation_int(bo, len, 8);
    }
    isom_write_bit_operation_data(bo, str, len);
    return true;
}

/*
* Desc : 
*/
Bool16 isom_set_descriptor_list_write_odf(isom_bit_operation_t *bo, ut_list_t *desc_list)
{
    UInt32 count, i;
    isom_descriptor_t *tmp;
    
    if (! desc_list) {
        return true;
    }
    count = isom_get_list_count(desc_list);
    for ( i = 0; i < count; i++ ) {
        tmp = (isom_descriptor_t*)isom_get_list(desc_list, i);
        if (tmp) {
            if (!isom_set_high_odf_write_descriptor(bo, tmp)) {
                return false;
            }
        } 
    }
    return true;
}

/*
* Desc : 
*/
Bool16 isom_get_odf_size_descriptor_list(ut_list_t *descList, UInt32 *outSize)
{
    isom_descriptor_t *tmp;
    UInt32 tmpSize, count, i;
    if (!descList) {
        return true;
    }
    
    count = isom_get_list_count(descList);
    for ( i = 0; i < count; i++ ) {
        tmp = (isom_descriptor_t*)isom_get_list(descList, i);
        if (tmp) {
            if (!isom_get_odf_size_descriptor(tmp, &tmpSize)) {
                return false;
            }

            if (tmpSize) *outSize += tmpSize + isom_get_odf_field_size(tmpSize);
        }
    }
    return true;
}


/*
* Desc : 
*/
Bool16 isom_get_odf_size_esd(isom_esd_t *esd, UInt32 *outSize)
{
    UInt32 tmpSize;

    if (! esd) {
        return false;
    }
    
    *outSize = 0;
    *outSize += 3;
    
    /*this helps keeping proper sync: some people argue that OCR_ES_ID == ES_ID is a circular reference
    of streams. Since this is equivalent to no OCR_ES_ID, keep it that way*/
    //	if (esd->OCRESID == esd->ESID) esd->OCRESID = 0;
    
    if (esd->depends_onesid) {
        *outSize += 2;
    }
    if (esd->url_string) {
        *outSize += isom_get_odf_size_url_string(esd->url_string);
    }
    if (esd->ocr_esid) {
        *outSize += 2;
    }
    
    if (esd->decoder_config) {
        if (!isom_get_odf_size_descriptor((isom_descriptor_t *) esd->decoder_config, &tmpSize)) {
            return false;
        }

        *outSize += tmpSize + isom_get_odf_field_size(tmpSize);
    }
    if (esd->sl_config_descriptor) {
        if (!isom_get_odf_size_descriptor((isom_descriptor_t *) esd->sl_config_descriptor, &tmpSize)) {
            return false;
        }

        *outSize += tmpSize + isom_get_odf_field_size(tmpSize);
    }
    if (esd->ipi_ptr) {
        if (!isom_get_odf_size_descriptor((isom_descriptor_t *) esd->ipi_ptr, &tmpSize)) {
            return false;
        }

        *outSize += tmpSize + isom_get_odf_field_size(tmpSize);
    }
    if (esd->lang_desc) {
        if (!isom_get_odf_size_descriptor((isom_descriptor_t *) esd->lang_desc, &tmpSize)) {
            return false;
        }

        *outSize += tmpSize + isom_get_odf_field_size(tmpSize);
    }
    
    if (!isom_get_odf_size_descriptor_list(esd->ipi_dataset, outSize)) {
        return false;
    }
    
    if (!isom_get_odf_size_descriptor_list(esd->ipmp_descriptor_pointers, outSize)) {
        return false;
    }

    if (esd->qos) {
        if (!isom_get_odf_size_descriptor((isom_descriptor_t *) esd->qos, &tmpSize)) {
            return false;
        }

        *outSize += tmpSize + isom_get_odf_field_size(tmpSize);
    }

    if (esd->reg_descriptor) {
        if (!isom_get_odf_size_descriptor((isom_descriptor_t *) esd->reg_descriptor, &tmpSize)) {
            return false;
        }

        *outSize += tmpSize + isom_get_odf_field_size(tmpSize);
    }
    return isom_get_odf_size_descriptor_list(esd->extension_descriptors, outSize);
}

/*
* Desc : 
*/
UInt32 isom_get_odf_size_url_string(char *_string)
{
	UInt32 len = strlen(_string);
	if (len>255) return len+5;
	return len+1;
}

/*
* Desc : 3gpa new
*/
Bool16 isom_create_3gpa(isom_box_t **box, UInt32 type)
{
    isom_3gpp_audio_sample_entry_box *tmp;
    tmp = (isom_3gpp_audio_sample_entry_box *) malloc(sizeof(isom_3gpp_audio_sample_entry_box));
    if (tmp == NULL) {
        return false;
    }
    memset(tmp, 0x00, sizeof(isom_3gpp_audio_sample_entry_box));
    
    isom_init_audio_sample_entry((isom_audio_sample_entry_box_t*)tmp);

    *box = (isom_box_t *)tmp;
    tmp->type = type;

    return true;
}

/*
* Desc : 3gpv new
*/
Bool16 isom_create_3gpv(isom_box_t **box, UInt32 type)
{
    isom_3gpp_visual_sample_entry_box *tmp;
    tmp = (isom_3gpp_visual_sample_entry_box *) malloc(sizeof(isom_3gpp_visual_sample_entry_box));
    if (tmp == NULL) {
        return false;
    }
    memset(tmp, 0x00, sizeof(isom_3gpp_visual_sample_entry_box));
    
    isom_init_video_sample_entry((isom_visual_sample_entry_box_t*)tmp);

    *box = (isom_box_t *)tmp;
    tmp->type = type;

    return true;
}

/*
* Desc : 3gpc new
*/
Bool16 isom_create_3gpc(isom_box_t **box, UInt32 type)
{
    
    isom_3gpp_config_box *tmp;
    tmp = (isom_3gpp_config_box *) malloc(sizeof(isom_3gpp_config_box));
    if (tmp == NULL) {
        return false;
    }
    memset(tmp, 0x00, sizeof(isom_3gpp_config_box));
    
    *box = (isom_box_t *)tmp;
    tmp->type = type;

    return true;
}

/*
* Desc : mp4v new
*/
Bool16 isom_create_mp4v(isom_box_t **box, UInt32 type)
{
    return isom_create_mp4v_avc1(box, type);
}

/*
* Desc : mp4v avc1 new
*/
Bool16 isom_create_mp4v_avc1(isom_box_t **box, UInt32 type)
{
    isom_mpeg_visual_sample_entry_box_t *tmp;
    
    tmp = (isom_mpeg_visual_sample_entry_box_t *) malloc(sizeof(isom_mpeg_visual_sample_entry_box_t));
    if (tmp == NULL) {
        return false;
    }
    memset(tmp, 0x00, sizeof(isom_mpeg_visual_sample_entry_box_t));
    
    *box = (isom_box_t *)tmp;
    
    isom_init_video_sample_entry((isom_visual_sample_entry_box_t*) tmp);
    tmp->type = type;
    return true;
}

/*
* Desc : mp4a new
*/
Bool16 isom_create_mp4a(isom_box_t **box, UInt32 type)
{
    isom_mpeg_audio_sample_entry_box_t *tmp;
    
    tmp = (isom_mpeg_audio_sample_entry_box_t *) malloc(sizeof(isom_mpeg_audio_sample_entry_box_t));
    if (tmp == NULL) {
        return false;
    }
    memset(tmp, 0x00, sizeof(isom_mpeg_audio_sample_entry_box_t));
    tmp->type = ISOM_BOX_TYPE_MP4A;

    *box = (isom_box_t *)tmp;
    
    isom_init_audio_sample_entry((isom_audio_sample_entry_box_t*) tmp);
    tmp->type = type;
    return true;
}

/*
* Desc : 
*/
Bool16 isom_create_esds(isom_box_t **newbox)
{
    isom_esd_box_t *tmp = (isom_esd_box_t *) malloc(sizeof(isom_esd_box_t));
    if (tmp == NULL) {
        return false;
    }
    memset(tmp, 0, sizeof(isom_esd_box_t));
    
    isom_init_full_box((isom_box_t *) tmp);
    *newbox = (isom_box_t *)tmp;
    
    tmp->type = ISOM_BOX_TYPE_ESDS;
    return true;
}

/*
* Desc :
*/
Bool16 isom_create_avc1(isom_box_t **box, UInt32 type)
{
    return isom_create_mp4v_avc1(box, type);
}


/*
* Desc :
*/
Bool16 isom_create_avcc(isom_box_t **box)
{
    isom_avcconfiguration_box_t *tmp = (isom_avcconfiguration_box_t *) malloc(sizeof(isom_avcconfiguration_box_t));
    if (tmp == NULL) {
        return false;
    }
    memset(tmp, 0, sizeof(isom_avcconfiguration_box_t));
    tmp->type = ISOM_BOX_TYPE_AVCC;

    *box = (isom_box_t *)tmp;
    return true;
}

/*
* Desc :
*/
Bool16 isom_create_btrt(isom_box_t **box)
{
    isom_mpeg4_bitrate_box_t *tmp = (isom_mpeg4_bitrate_box_t *) malloc(sizeof(isom_mpeg4_bitrate_box_t));
    if (tmp == NULL) {
        return false;
    }

    memset(tmp, 0, sizeof(isom_mpeg4_bitrate_box_t));
    tmp->type = ISOM_BOX_TYPE_BTRT;
    *box = (isom_box_t *)tmp;
    return true;
}

/*
* Desc :
*/
Bool16 isom_create_m4ds(isom_box_t **box)
{
    isom_mpeg4_extension_descriptors_box_t *tmp = (isom_mpeg4_extension_descriptors_box_t *) malloc(sizeof(isom_mpeg4_extension_descriptors_box_t));
    if (tmp == NULL) {
        return false;
    }

    memset(tmp, 0, sizeof(isom_mpeg4_extension_descriptors_box_t));
    tmp->type = ISOM_BOX_TYPE_M4DS;
    tmp->descriptors = isom_create_list();
    *box = (isom_box_t *)tmp;
    return true;
}

/*
* Desc :
*/
Bool16 isom_set_3gpa(isom_box_t *box, isom_resource_t *data_resource)
{
    UInt64 pos;
    UInt32 size;
    isom_3gpp_audio_sample_entry_box *ptr = NULL;

    if (!isom_set_audio_sample_entry((isom_audio_sample_entry_box_t*)box, data_resource)) {
        return false;
    }
    pos = data_resource->res_mem->mem_pos;
	size = (UInt32) box->size;
    ptr = (isom_3gpp_audio_sample_entry_box *)box;

    if (isom_parse_box((isom_box_t **)&ptr->info, data_resource) != ISOM_OK) {
        return false;
    }

    ptr->info->cfg.type = ptr->type;
    return true;
}

/*
* Desc :
*/
Bool16 isom_set_3gpv(isom_box_t *box, isom_resource_t *data_resource)
{
    UInt64 pos;
    UInt32 size;
	isom_3gpp_visual_sample_entry_box *ptr = (isom_3gpp_visual_sample_entry_box *)box;
    if (!isom_set_visual_sample((isom_visual_sample_entry_box_t*) box, data_resource)) {
        return false;
    }
    
    pos = data_resource->res_mem->mem_pos;
    size = (UInt32) box->size;
    ptr = (isom_3gpp_visual_sample_entry_box *)box;

    if (isom_parse_box((isom_box_t **)&ptr->info, data_resource) != ISOM_OK) {
        return false;
    }
	return true;
}
  
/*
* Desc :
*/
Bool16 isom_set_3gpc(isom_box_t *box, isom_resource_t *data_resource)
{
    UInt64 pos = 0;
    isom_3gpp_config_box *ptr = (isom_3gpp_config_box *)box;
    if (ptr == NULL) {
        return false;
    }

    memset(&ptr->cfg, 0x00, sizeof(isom_3gp_config));
    
    pos = data_resource->res_mem->mem_pos;

    isom_read_resource_uint32(data_resource, pos, &ptr->cfg.vendor, NULL);
    isom_read_resource_uint8(data_resource, pos + 4, &ptr->cfg.decoder_version, NULL);
        
    switch (ptr->type) {
    case ISOM_BOX_TYPE_D263:
        isom_read_resource_uint8(data_resource, pos + 5, &ptr->cfg.h263_level, NULL);
        isom_read_resource_uint8(data_resource, pos + 6, &ptr->cfg.h263_profile, NULL);
        break;
    case ISOM_BOX_TYPE_DAMR:
        isom_read_resource_uint16(data_resource, pos + 5, &ptr->cfg.amr_mode_set, NULL);
        isom_read_resource_uint8(data_resource, pos + 7, &ptr->cfg.amr_mode_change_period, NULL);
        isom_read_resource_uint8(data_resource, pos + 8, &ptr->cfg.frames_per_sample, NULL);
        break;
    case ISOM_BOX_TYPE_DEVC: 
    case ISOM_BOX_TYPE_DQCP:
    case ISOM_BOX_TYPE_DSMV:
        isom_read_resource_uint8(data_resource, pos + 5, &ptr->cfg.frames_per_sample, NULL);
        break;
    }

    return true;
}

/*
* Desc : mp4a속성값 read
*/
Bool16 isom_set_mp4a(isom_box_t *box, isom_resource_t *data_resource)
{
    UInt64 pos;
    UInt32 size;

    if (!isom_set_audio_sample_entry((isom_audio_sample_entry_box_t*)box, data_resource)) {
        return false;
    }

	pos = data_resource->res_mem->mem_pos;
	size = (UInt32) box->size;

    if (!isom_set_box_list(box, data_resource, isom_add_mp4a_box)) {
        return false;
    }
    
    return true;
}

/*
* Desc : mp4v avc1 read
*/
Bool16 isom_set_mp4v_avc1(isom_box_t *box, isom_resource_t *data_resource)
{
    if (!isom_set_visual_sample((isom_visual_sample_entry_box_t*) box, data_resource)) {
        return false;
    }
    
    if (!isom_set_box_list(box, data_resource, isom_add_mp4v_box)) {
        return false;
    }
    
    return true;
}

/*
* Desc : mp4v속성값 read
*/
Bool16 isom_set_mp4v(isom_box_t *box, isom_resource_t *data_resource)
{
    return isom_set_mp4v_avc1(box, data_resource);
}


/*
* Desc : 
*/
Bool16 isom_add_mp4v_box(isom_box_t *s, isom_box_t *a)
{
    isom_mpeg_visual_sample_entry_box_t *ptr = (isom_mpeg_visual_sample_entry_box_t *)s;

    switch (a->type) {
    case ISOM_BOX_TYPE_ESDS:
        if (ptr->esd) {
            return false;
        }

        ptr->esd = (isom_esd_box_t *)a;
        break;        
    case ISOM_BOX_TYPE_AVCC:
        if (ptr->avc_config) {
            return false;
        }

        ptr->avc_config = (isom_avcconfiguration_box_t *)a;
        break;
    case ISOM_BOX_TYPE_BTRT:
        if (ptr->bitrate) {
            return false;
        }

        ptr->bitrate = (isom_mpeg4_bitrate_box_t *)a;
        break;
    case ISOM_BOX_TYPE_M4DS:
        if (ptr->descr) {
            return false;
        }

        ptr->descr = (isom_mpeg4_extension_descriptors_box_t *)a;
        break;
    case ISOM_BOX_TYPE_UUID:
        if (ptr->ipod_ext) {
            return false;
        }

        ptr->ipod_ext = (isom_unknown_uuid_box_t *)a;
        break;
    default:
        isom_delete_box(a);
        break;
    }
    return true;
}




/*
* Desc : 
*/
Bool16 isom_add_mp4a_box(isom_box_t *s, isom_box_t *a)
{
    isom_mpeg_audio_sample_entry_box_t *ptr = (isom_mpeg_audio_sample_entry_box_t *)s;
    switch (a->type) {
    case ISOM_BOX_TYPE_ESDS:
        if (ptr->esd) {
            return false;
        }
        ptr->esd = (isom_esd_box_t *)a;
        break;
    default:
        isom_delete_box(a);
        break;
    }
    return true;
}

/*
* Desc : 
*/
Bool16 isom_set_esds(isom_box_t *box, isom_resource_t *data_resource)
{
    UInt32       desc_size;
    UInt64       pos;
    char        *enc_desc;
	isom_esd_box_t  *ptr = (isom_esd_box_t *)box;

    if (!isom_set_full_box(box, data_resource)){
        return false;
    }

    desc_size = (UInt32) (box->size);
    pos = data_resource->res_mem->mem_pos;

    if (desc_size) {
        enc_desc = (char*)malloc(sizeof(char) * desc_size);
        if (!enc_desc) {
            return false;
        }
        //get the payload
        isom_read_resource_bytes(data_resource, pos, enc_desc, desc_size, NULL);
        //send it to the OD Codec
        if (!isom_set_odf_descriptor(enc_desc, desc_size, (isom_descriptor_t **) &ptr->desc)) {
            return false;
        }
        //OK, free our desc
        free(enc_desc);

        /*fix broken files*/
        if (!ptr->desc->url_string) {
            if (!ptr->desc->sl_config_descriptor) {
                isom_init_odf_descriptor((isom_descriptor_t **)&ptr->desc->sl_config_descriptor, ISOM_ODF_SLC_TAG);
                ptr->desc->sl_config_descriptor->predefined = SLPredef_MP4;
            } else if (ptr->desc->sl_config_descriptor->predefined != SLPredef_MP4) {
                ptr->desc->sl_config_descriptor->predefined = SLPredef_MP4;
                isom_set_odf_slc_predef(ptr->desc->sl_config_descriptor);
            }
		}
        
	}

    return true;
}


/*
* Desc :
*/
Bool16 isom_set_avc1(isom_box_t *box, isom_resource_t *data_resource)
{
    return isom_set_mp4v_avc1(box, data_resource);
}


/*
* Desc :
*/
Bool16 isom_set_avcc(isom_box_t *box, isom_resource_t *data_resource)
{
    UInt8 temp_uint8;
    UInt32 i, count;
    UInt64 pos, nbytes = 0;
	isom_avcconfiguration_box_t *ptr = (isom_avcconfiguration_box_t *)box;

    if (ptr->config) {
        isom_delete_odf_avc_config(ptr->config);
    }
	isom_create_odf_avc_config(&ptr->config);

    pos = data_resource->res_mem->mem_pos;

    isom_read_resource_uint8(data_resource, pos, &ptr->config->configuration_version, NULL);
    isom_read_resource_uint8(data_resource, pos + 1, &ptr->config->avc_profile_indication, NULL);
    isom_read_resource_uint8(data_resource, pos + 2, &ptr->config->profile_compatibility, NULL);
    isom_read_resource_uint8(data_resource, pos + 3, &ptr->config->avc_level_indication, NULL);

    isom_read_resource_uint8(data_resource, pos + 4, &temp_uint8, NULL);    
    ptr->config->nal_unit_size = 1 + (temp_uint8 & 0x00000003);

    isom_read_resource_uint8(data_resource, pos + 5, &temp_uint8, NULL);
	count = temp_uint8 & 0x0000001f;
    nbytes += 6;

    for (i=0; i<count; i++) { //sps
		isom_avc_config_slot_t *sl = (isom_avc_config_slot_t *) malloc(sizeof(isom_avc_config_slot_t));

        isom_read_resource_uint16(data_resource, pos + nbytes, &sl->size, NULL);
		nbytes += 2;
		sl->data = (char *)malloc(sizeof(char) * sl->size);

        isom_read_resource_bytes(data_resource, pos + nbytes, sl->data, sl->size, NULL);
        nbytes += sl->size;
		isom_add_list(ptr->config->sequence_parameter_sets, sl);
	}
    isom_read_resource_uint8(data_resource, pos + nbytes, &temp_uint8, NULL);
    count = (UInt32) temp_uint8;
	nbytes += 1;
    for (i=0; i<count; i++) { //pps
		isom_avc_config_slot_t *sl = (isom_avc_config_slot_t *)malloc(sizeof(isom_avc_config_slot_t));
        isom_read_resource_uint16(data_resource, pos + nbytes, &sl->size, NULL);
		nbytes += 2;

		sl->data = (char *)malloc(sizeof(char) * sl->size);
        isom_read_resource_bytes(data_resource, pos + nbytes, sl->data, sl->size, NULL);
        nbytes += sl->size;		
		isom_add_list(ptr->config->picture_parameter_sets, sl);
	}
	return true;
}

/*
* Desc :
*/
void isom_delete_avcc(isom_box_t *s)
{
    isom_avcconfiguration_box_t *ptr = (isom_avcconfiguration_box_t *)s;
    if (ptr->config) {
        isom_delete_odf_avc_config(ptr->config);
    }
    free(ptr);
}


/*
* Desc :
*/
Bool16 isom_set_btrt(isom_box_t *box, isom_resource_t *data_resource)
{
    UInt64  pos = data_resource->res_mem->mem_pos;
    isom_mpeg4_bitrate_box_t *ptr = (isom_mpeg4_bitrate_box_t *)box;

    isom_read_resource_uint32(data_resource, pos, &ptr->buffer_sizedb, NULL);
    isom_read_resource_uint32(data_resource, pos + 4, &ptr->max_bitrate, NULL);
    isom_read_resource_uint32(data_resource, pos + 8, &ptr->avg_bitrate, NULL);

    return true;
}


/*
* Desc :
*/
Bool16 isom_set_m4ds(isom_box_t *box, isom_resource_t *data_resource)
{
	char *enc_od;
	isom_mpeg4_extension_descriptors_box_t *ptr = (isom_mpeg4_extension_descriptors_box_t *)box;
	UInt32 od_size = (UInt32) ptr->size;
    if (!od_size) {
        return true;
    }
	enc_od = (char *)malloc(sizeof(char) * od_size);
	isom_read_resource_bytes(data_resource, data_resource->res_mem->mem_pos, enc_od, od_size, NULL);
    if (!isom_set_odf_descriptor_list((char *)enc_od, od_size, ptr->descriptors)){
        free(enc_od);
        return false;
    }
	free(enc_od);
	return true;
}



/*
* Desc :
*/
void isom_delete_btrt(isom_box_t *s)
{
    isom_mpeg4_bitrate_box_t *ptr = (isom_mpeg4_bitrate_box_t *)s;
    if (ptr) {
        free(ptr);
    }
}


/*
* Desc :
*/
void isom_delete_m4ds(isom_box_t *box)
{
	isom_mpeg4_extension_descriptors_box_t *ptr = (isom_mpeg4_extension_descriptors_box_t *)box;
	isom_delete_odf_descriptor_list(ptr->descriptors);
	isom_delete_list(ptr->descriptors);
	free(ptr);
}

/*
* Desc : 3gpa delete
*/
void isom_delete_3gpa(isom_box_t *box)
{
    isom_3gpp_audio_sample_entry_box *ptr = (isom_3gpp_audio_sample_entry_box *)box;
    if (ptr == NULL) {
        return;
    }

    if (ptr->info) {
        isom_delete_box((isom_box_t *)ptr->info);
    }
    free(ptr);
}

/*
* Desc : 3gpc delete
*/
void isom_delete_3gpc(isom_box_t *box)
{
    isom_3gpp_config_box *ptr = (isom_3gpp_config_box *)box;
    if (ptr == NULL) {
        return;
    }

    free(ptr);
}

/*
* Desc : 3gpv delete
*/
void isom_delete_3gpv(isom_box_t *box)
{
    isom_3gpp_visual_sample_entry_box *ptr = (isom_3gpp_visual_sample_entry_box *)box;
    if (ptr == NULL) {
        return;
    }

    if (ptr->info) {
        isom_delete_box((isom_box_t *)ptr->info);
    }
    free(ptr);
}

/*
* Desc : mp4v delete
*/
void isom_delete_mp4v(isom_box_t *box)
{
    isom_mpeg_visual_sample_entry_box_t *ptr = (isom_mpeg_visual_sample_entry_box_t *)box;
    if (ptr == NULL) {
        return;
    }

    if (ptr->esd) {
        isom_delete_box((isom_box_t *)ptr->esd);
    }

    if (ptr->slc) {
        isom_delete_odf_descriptor((isom_descriptor_t *)ptr->slc);
    }

    if (ptr->avc_config) {
        isom_delete_box((isom_box_t *) ptr->avc_config);
    }
    if (ptr->bitrate) {
        isom_delete_box((isom_box_t *) ptr->bitrate);
    }
    if (ptr->descr) {
        isom_delete_box((isom_box_t *) ptr->descr);
    }
    if (ptr->ipod_ext) {
        isom_delete_box((isom_box_t *)ptr->ipod_ext);
    }  
    if (ptr->emul_esd) {
        isom_delete_odf_descriptor((isom_descriptor_t *)ptr->emul_esd);
    }

    free(ptr);
}

/*
* Desc : mp4a delete
*/
void isom_delete_mp4a(isom_box_t *s)
{
    isom_mpeg_audio_sample_entry_box_t *ptr = (isom_mpeg_audio_sample_entry_box_t *)s;
    if (ptr == NULL) {
        return;
    }
    if (ptr->esd) {
        isom_delete_box((isom_box_t *)ptr->esd);
    }
    if (ptr->slc) {
        isom_delete_odf_descriptor((isom_descriptor_t *)ptr->slc);
    }
    
    free(ptr);
}

/*
* Desc : esds 구조체  delete
*/
void isom_delete_esds(isom_box_t *s)
{
    isom_esd_box_t *ptr = (isom_esd_box_t *)s;

    if (ptr == NULL) {
        return;
    }

    if (ptr->desc) {
        isom_delete_odf_descriptor((isom_descriptor_t *)ptr->desc);
    }
    free(ptr);
}


/*
* Desc : 
*/
void isom_delete_default(isom_box_t *s)
{
    isom_unknown_box_t *ptr = (isom_unknown_box_t *) s;
    if (!s) {
        return;
    }
    if (ptr->data) {
        free(ptr->data);
    }
    if (ptr) {
        free(ptr);
    }
}

/*
* Desc : visual 공통 속성 초기화 
*/
void isom_init_video_sample_entry(isom_visual_sample_entry_box_t *ent)
{
    ent->horiz_res = ent->vert_res = 0x00480000;
    ent->frames_per_sample = 1;
    ent->bit_depth = 0x18;
    ent->color_table_index = -1;
}

/*
* Desc : audio 공통 속성 초기화 
*/
void isom_init_audio_sample_entry(isom_audio_sample_entry_box_t *ptr)
{
    ptr->channel_count = 2;
    ptr->bitspersample = 16;
}
