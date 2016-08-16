#include "isom_file.h"

/*
* Desc : isom_file_info_t 메모리 할당 및 셋팅  
*/
rs_status_t isom_init_file_info(const char *content_path, isom_file_info_t **in_mp4file_info, isom_resource_t *data_resource)
{
    isom_file_info_t  *mp4file_info = NULL;
    isom_box_entry_t  *box_entry = NULL;
    isom_box_moov_t   *box_moov = NULL;
    rs_status_t        result_code = ISOM_UNKNOWN;

    if (content_path == NULL || data_resource == NULL) {
        return ISOM_BAD_PARAM;
    }

    mp4file_info = (isom_file_info_t *) malloc(sizeof(isom_file_info_t));
    if (mp4file_info == NULL) {
        return ISOM_MEMORY_ERROR;
    }
    memset(mp4file_info, 0x00, sizeof(isom_file_info_t));    
    
    *in_mp4file_info = mp4file_info;

    mp4file_info->content_path = strdup(content_path);
    mp4file_info->data_resource = data_resource; // 주소만 복사해서 사용, delete의 경우 외부에서 한다. 
       
    if ((result_code=isom_generate_box_entry(mp4file_info)) != ISOM_OK) {
        return result_code;
    }
    /////////////////////////////////////////////box_offset_info complete/////////////////////////////////////////

    if ((result_code = isom_find_box_entry(mp4file_info, "moov", &box_entry, NULL)) != ISOM_OK) {
        return ISOM_FILE_NOT_FOUND_MOOV_BOX;
    }
    if ((result_code = isom_init_moov_box(mp4file_info, box_entry, &mp4file_info->box_moov)) != ISOM_OK) {
        return result_code;
    }

    if ((result_code = isom_find_box_entry(mp4file_info, "moov:mvhd", &box_entry, NULL)) != ISOM_OK) {
        return ISOM_FILE_NOT_FOUND_MVHD_BOX;
    }
    if ((result_code = isom_init_mvhd_box(mp4file_info, box_entry, &mp4file_info->box_moov->box_mvhd)) != ISOM_OK) {
        return result_code;
    } 

    box_moov = mp4file_info->box_moov;
    box_entry = NULL;

    while (isom_find_box_entry(mp4file_info, "moov:trak", &box_entry, box_entry) == ISOM_OK) {
        isom_box_trak_entry_t  *temp_box_trak_entry;

        temp_box_trak_entry = (isom_box_trak_entry_t *) malloc(sizeof(isom_box_trak_entry_t));
        if (temp_box_trak_entry == NULL) {
            return ISOM_MEMORY_ERROR;
        }
        memset(temp_box_trak_entry, 0x00, sizeof(isom_box_trak_entry_t));

        box_moov->trak_list_entry = temp_box_trak_entry;

        if ((result_code = isom_init_trak_box(mp4file_info, box_entry, &(temp_box_trak_entry->box_trak))) != ISOM_OK) {
            ISOM_DEEP_DEBUG_PRINT(("ts_init_trak_box  failed.\n"));
            return result_code; 
        }        
        else {
            if (temp_box_trak_entry->box_trak == NULL) {
                return ISOM_FILE_INVALID_TRAK_BOX;
            }
        }
        temp_box_trak_entry->box_trak_entry_id = temp_box_trak_entry->box_trak->box_tkhd->track_id;

        temp_box_trak_entry->next_box_trak_entry = NULL;
        if (box_moov->trak_list_head == NULL) {
            box_moov->trak_list_head = box_moov->trak_list_tail = temp_box_trak_entry;
        } 
        else {
            box_moov->trak_list_tail->next_box_trak_entry = temp_box_trak_entry;
            box_moov->trak_list_tail = temp_box_trak_entry;
        }
        
        mp4file_info->track_count++;
        box_moov->trak_list_entry = NULL;
    }

    return ISOM_OK;
}


/*
* Desc : isom_file_info_t 메모리 해제 (하위 모든 구조체의 모든 메모리 해제)  
*/

Bool16 isom_delete_file_info(isom_file_info_t *mp4file_info)
{
    isom_box_entry_t     *box_entry;
    isom_box_entry_t     *next_box_entry;

    if (mp4file_info) {

        if (mp4file_info->content_path) {
            free(mp4file_info->content_path);
            mp4file_info->content_path = NULL;
        }
        //외부에서 delete를 해준다. 
        //isom_delete_resource(mp4file_info->data_resource);
        if (mp4file_info->box_moov) {
            if (mp4file_info->box_moov->box_mvhd) {
                free(mp4file_info->box_moov->box_mvhd);
                mp4file_info->box_moov->box_mvhd = NULL;
            }
        }

        box_entry = mp4file_info->box_list_head;
        next_box_entry = box_entry ? box_entry->next_order_box : NULL;

        while (box_entry != NULL) {
            //
            // Delete this box entry and move to the next one.
            free(box_entry);
            box_entry = NULL;
            
            box_entry = next_box_entry;
            if (box_entry != NULL) {
                next_box_entry = box_entry->next_order_box;
            }
        }

        isom_delete_trak_box(mp4file_info);
        
        if (mp4file_info->box_moov) {
            free(mp4file_info->box_moov);
            mp4file_info->box_moov = NULL;
        }
        
        free(mp4file_info);
        mp4file_info = NULL;
        return true;
    }
    return false;
}


/*
* Desc : moov box initialize
*/
rs_status_t isom_init_moov_box(isom_file_info_t *mp4file_info, isom_box_entry_t *org_box_entry, isom_box_moov_t **box_moov)
{
    isom_box_moov_t     *temp_box_moov;

    temp_box_moov = (isom_box_moov_t *) malloc(sizeof(isom_box_moov_t));
    if (temp_box_moov == NULL) {
        return ISOM_MEMORY_ERROR;
    }
    memset(temp_box_moov, 0x00, sizeof(isom_box_moov_t));

    *box_moov = temp_box_moov;

    temp_box_moov->size = org_box_entry->box_data_size + org_box_entry->box_header_size;
    temp_box_moov->type = org_box_entry->box_type;

    // This box has been successfully read in.
    return ISOM_OK;
}

/*
* Desc : mvhd box 의 속성값  isom_box_mvhd_t 구조체에 셋팅
*/
rs_status_t isom_init_mvhd_box(isom_file_info_t *mp4file_info, isom_box_entry_t *org_box_entry, isom_box_mvhd_t **box_mvhd)
{
    UInt32           temp_uint32;
    isom_box_entry_t     box_entry;
    isom_box_mvhd_t      *temp_box_mvhd;
    rs_status_t        result_code = ISOM_UNKNOWN;

    temp_box_mvhd = (isom_box_mvhd_t *) malloc(sizeof(isom_box_mvhd_t));
    if (temp_box_mvhd == NULL) {
        return ISOM_MEMORY_ERROR;
    }
    memset(temp_box_mvhd, 0x00, sizeof(isom_box_mvhd_t));
    
    *box_mvhd = temp_box_mvhd;
    temp_box_mvhd->size = org_box_entry->box_data_size + org_box_entry->box_header_size;
    temp_box_mvhd->type = org_box_entry->box_type;
    
    memcpy(&box_entry, org_box_entry, sizeof(isom_box_entry_t));
    // Parse this box's fields.
    isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOS_VERSIONFLAGS, &temp_uint32, &box_entry);   
    
    temp_box_mvhd->version = (UInt8)((temp_uint32 >> 24) & 0x000000ff);
    temp_box_mvhd->flags = temp_uint32 & 0x00ffffff;

    if (0 == temp_box_mvhd->version) {
        // Verify that this box is the correct length.
        if( box_entry.box_data_size != 100 ) {
            ISOM_DEEP_DEBUG_PRINT(("ts_init_mvhd_box failed. Expected BoxDataLength == 100 version: %d BoxDataLength: %"_64BITARG_"u\n",temp_box_mvhd->version, box_entry.box_data_size));
            return ISOM_FILE_INVALID_MVHD_BOX;
        }
        isom_read_resource_uint32_to_uint64(mp4file_info->data_resource, MVHDPOS_CREATIONTIME, &(temp_box_mvhd->creation_time), &box_entry);
        isom_read_resource_uint32_to_uint64(mp4file_info->data_resource, MVHDPOS_MODIFICATIONTIME, &(temp_box_mvhd->modification_time), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOS_TIMESCALE, &(temp_box_mvhd->timescale), &box_entry);
        isom_read_resource_uint32_to_uint64(mp4file_info->data_resource, MVHDPOS_DURATION, &(temp_box_mvhd->duration), &box_entry);

        /* non use field */
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOS_PREFERREDRATE, &(temp_box_mvhd->preferred_rate), &box_entry);
        isom_read_resource_uint16(mp4file_info->data_resource, MVHDPOS_PREFERREDVOLUME, &(temp_box_mvhd->preferred_volume), &box_entry);

        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOS_A, &(temp_box_mvhd->fa), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOS_B, &(temp_box_mvhd->fb), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOS_U, &(temp_box_mvhd->fu), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOS_C, &(temp_box_mvhd->fc), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOS_D, &(temp_box_mvhd->fd), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOS_V, &(temp_box_mvhd->fv), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOS_X, &(temp_box_mvhd->fx), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOS_Y, &(temp_box_mvhd->fy), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOS_W, &(temp_box_mvhd->fw), &box_entry);

        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOS_PREVIEWTIME, &(temp_box_mvhd->preview_time), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOS_PREVIEWDURATION, &(temp_box_mvhd->preview_duration), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOS_POSTERTIME, &(temp_box_mvhd->poster_time), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOS_SELECTIONTIME, &(temp_box_mvhd->selection_time), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOS_SELECTIONDURATION, &(temp_box_mvhd->selection_duration), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOS_CURRENTTIME, &(temp_box_mvhd->current_time), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOS_NEXTTRACKID, &(temp_box_mvhd->next_track_id), &box_entry);
        /* non use field end*/
    }
    else if (1 == temp_box_mvhd->version) {
        // Verify that this box is the correct length.
        if( box_entry.box_data_size != 112 ) {
            ISOM_DEEP_DEBUG_PRINT(("ts_init_mvhd_box failed. Expected boxDataLength = 112 version: %d boxDataLength: %"_64BITARG_"u\n",temp_box_mvhd->version, box_entry.box_data_size));
            return ISOM_FILE_INVALID_MVHD_BOX;
        }
        
        isom_read_resource_uint64(mp4file_info->data_resource, MVHDPOSV1_CREATIONTIME, &(temp_box_mvhd->creation_time), &box_entry);
        isom_read_resource_uint64(mp4file_info->data_resource, MVHDPOSV1_MODIFICATIONTIME, &(temp_box_mvhd->modification_time), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOSV1_TIMESCALE, &(temp_box_mvhd->timescale), &box_entry);
        isom_read_resource_uint64(mp4file_info->data_resource, MVHDPOSV1_DURATION, &(temp_box_mvhd->duration), &box_entry);
        /* non use field */
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOSV1_PREFERREDRATE, &(temp_box_mvhd->preferred_rate), &box_entry);
        isom_read_resource_uint16(mp4file_info->data_resource, MVHDPOSV1_PREFERREDVOLUME, &(temp_box_mvhd->preferred_volume), &box_entry);

        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOSV1_A, &(temp_box_mvhd->fa), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOSV1_B, &(temp_box_mvhd->fb), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOSV1_U, &(temp_box_mvhd->fu), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOSV1_C, &(temp_box_mvhd->fc), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOSV1_D, &(temp_box_mvhd->fd), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOSV1_V, &(temp_box_mvhd->fv), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOSV1_X, &(temp_box_mvhd->fx), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOSV1_Y, &(temp_box_mvhd->fy), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOSV1_W, &(temp_box_mvhd->fw), &box_entry);
        
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOSV1_PREVIEWTIME, &(temp_box_mvhd->preview_time), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOSV1_PREVIEWDURATION, &(temp_box_mvhd->preview_duration), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOSV1_POSTERTIME, &(temp_box_mvhd->poster_time), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOSV1_SELECTIONTIME, &(temp_box_mvhd->selection_time), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOSV1_SELECTIONDURATION, &(temp_box_mvhd->selection_duration), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOSV1_CURRENTTIME, &(temp_box_mvhd->current_time), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MVHDPOSV1_NEXTTRACKID, &(temp_box_mvhd->next_track_id), &box_entry);
        /* non use field end*/
    }
    else {
        ISOM_DEEP_DEBUG_PRINT(("ts_init_mvhd_box failed. Version unsupported: %d\n",temp_box_mvhd->version));
        return ISOM_FILE_INVALID_MVHD_BOX;
    }

    // This box has been successfully read in.
    return ISOM_OK;
}


/*
* Desc : tkhd box 의 속성값  isom_box_tkhd_t 구조체에 셋팅
*/
rs_status_t isom_init_tkhd_box(isom_file_info_t *mp4file_info, isom_box_entry_t *org_box_entry, isom_box_tkhd_t **box_tkhd)
{
    UInt32           temp_uint32;
    isom_box_entry_t     box_entry;
    isom_box_tkhd_t      *temp_box_tkhd;

    temp_box_tkhd = (isom_box_tkhd_t *) malloc(sizeof(isom_box_tkhd_t));
    if (temp_box_tkhd == NULL) {
        return ISOM_MEMORY_ERROR;
    }
    memset(temp_box_tkhd, 0x00, sizeof(isom_box_tkhd_t));

    *box_tkhd = temp_box_tkhd;
    temp_box_tkhd->size = org_box_entry->box_data_size + org_box_entry->box_header_size;
    temp_box_tkhd->type = org_box_entry->box_type;

    memcpy(&box_entry, org_box_entry, sizeof(isom_box_entry_t));
    //
    // Parse this box's fields.
    isom_read_resource_uint32(mp4file_info->data_resource, TKHDPOS_VERSIONFLAGS, &temp_uint32, &box_entry);
    temp_box_tkhd->version = (UInt8)((temp_uint32 >> 24) & 0x000000ff);
    temp_box_tkhd->flags = temp_uint32 & 0x00ffffff;
    
    if (0 == temp_box_tkhd->version) {
        // Verify that this box is the correct length.
        if( box_entry.box_data_size != 84 ) {
            ISOM_DEEP_DEBUG_PRINT(("ts_init_tkhd_box faild. Expected BoxDataLength == 84 version: %d BoxDataLength: %"_64BITARG_"u\n",temp_box_tkhd->version, box_entry.box_data_size));
            return ISOM_FILE_INVALID_TKHD_BOX;
        }
        
        isom_read_resource_uint32_to_uint64(mp4file_info->data_resource, TKHDPOS_CREATIONTIME, &(temp_box_tkhd->creation_time), &box_entry);
        isom_read_resource_uint32_to_uint64(mp4file_info->data_resource, TKHDPOS_MODIFICATIONTIME, &(temp_box_tkhd->modification_time), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, TKHDPOS_TRACKID, &(temp_box_tkhd->track_id), &box_entry);
        isom_read_resource_uint32_to_uint64(mp4file_info->data_resource, TKHDPOS_DURATION, &(temp_box_tkhd->duration), &box_entry);
        /* non use field */
        isom_read_resource_uint16(mp4file_info->data_resource, TKHDPOS_ALTERNATEGROUP, &(temp_box_tkhd->alternate_group), &box_entry);
        isom_read_resource_uint16(mp4file_info->data_resource, TKHDPOS_VOLUME, &(temp_box_tkhd->volume), &box_entry);
        
        isom_read_resource_uint32(mp4file_info->data_resource, TKHDPOS_A, &(temp_box_tkhd->fa), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, TKHDPOS_B, &(temp_box_tkhd->fb), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, TKHDPOS_U, &(temp_box_tkhd->fu), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, TKHDPOS_C, &(temp_box_tkhd->fc), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, TKHDPOS_D, &(temp_box_tkhd->fd), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, TKHDPOS_V, &(temp_box_tkhd->fv), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, TKHDPOS_X, &(temp_box_tkhd->fx), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, TKHDPOS_Y, &(temp_box_tkhd->fy), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, TKHDPOS_W, &(temp_box_tkhd->fw), &box_entry);
        /* non use field end*/
        isom_read_resource_uint32(mp4file_info->data_resource, TKHDPOS_TRACKWIDTH, &(temp_box_tkhd->track_width), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, TKHDPOS_TRACKHEIGHT, &(temp_box_tkhd->track_height), &box_entry);
    }
    else if (1 == temp_box_tkhd->version) {   
        // Verify that this box is the correct length.
        if (box_entry.box_data_size != 96) {
            ISOM_DEEP_DEBUG_PRINT(("ts_init_tkhd_box failed. Expected BoxDataLength == 96 version: %d BoxDataLength: %"_64BITARG_"u\n",temp_box_tkhd->version, box_entry.box_data_size));
            return ISOM_FILE_INVALID_TKHD_BOX;
        }
        
        isom_read_resource_uint64(mp4file_info->data_resource,TKHDPOSV1_CREATIONTIME, &(temp_box_tkhd->creation_time), &box_entry);
        isom_read_resource_uint64(mp4file_info->data_resource,TKHDPOSV1_MODIFICATIONTIME, &(temp_box_tkhd->modification_time), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, TKHDPOSV1_TRACKID, &(temp_box_tkhd->track_id), &box_entry);
        isom_read_resource_uint64(mp4file_info->data_resource, TKHDPOSV1_DURATION, &(temp_box_tkhd->duration), &box_entry);
        /* non use field */
        isom_read_resource_uint16(mp4file_info->data_resource, TKHDPOSV1_ALTERNATEGROUP, &(temp_box_tkhd->alternate_group), &box_entry);
        isom_read_resource_uint16(mp4file_info->data_resource, TKHDPOSV1_VOLUME, &(temp_box_tkhd->volume), &box_entry);
        
        isom_read_resource_uint32(mp4file_info->data_resource, TKHDPOSV1_A, &(temp_box_tkhd->fa), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, TKHDPOSV1_B, &(temp_box_tkhd->fb), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, TKHDPOSV1_U, &(temp_box_tkhd->fu), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, TKHDPOSV1_C, &(temp_box_tkhd->fc), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, TKHDPOSV1_D, &(temp_box_tkhd->fd), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, TKHDPOSV1_V, &(temp_box_tkhd->fv), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, TKHDPOSV1_X, &(temp_box_tkhd->fx), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, TKHDPOSV1_Y, &(temp_box_tkhd->fy), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, TKHDPOSV1_W, &(temp_box_tkhd->fw), &box_entry);
        /* non use field end*/
        isom_read_resource_uint32(mp4file_info->data_resource, TKHDPOSV1_TRACKWIDTH, &(temp_box_tkhd->track_width), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, TKHDPOSV1_TRACKHEIGHT, &(temp_box_tkhd->track_height), &box_entry);
    }
    else {
        ISOM_DEEP_DEBUG_PRINT(("ts_init_tkhd_box failed. Version unsupported: %d",temp_box_tkhd->version));
        return ISOM_FILE_INVALID_TKHD_BOX;
    }
    //
    // This box has been successfully read in.
    return ISOM_OK;
}

/*
* Desc : mdia box 의 속성값  isom_box_mdia_t 구조체에 셋팅
*/
rs_status_t isom_init_mdia_box(isom_file_info_t *mp4file_info, isom_box_entry_t *org_box_entry, isom_box_mdia_t **box_mdia)
{
    isom_box_mdia_t      *temp_box_mdia;

    temp_box_mdia = (isom_box_mdia_t *) malloc(sizeof(isom_box_mdia_t));
    if (temp_box_mdia == NULL) {
        return ISOM_MEMORY_ERROR;
    }
    memset(temp_box_mdia, 0x00, sizeof(isom_box_mdia_t));

    *box_mdia = temp_box_mdia;
    temp_box_mdia->size = org_box_entry->box_data_size + org_box_entry->box_header_size;
    temp_box_mdia->type = org_box_entry->box_type;

    return ISOM_OK;
}


/*
* Desc : mdhd box 의 속성값  isom_box_mdhd_t 구조체에 셋팅
*/
rs_status_t isom_init_mdhd_box(isom_file_info_t *mp4file_info, isom_box_entry_t *org_box_entry, isom_box_mdhd_t **box_mdhd)
{
    UInt32            temp_uint32;
    isom_box_entry_t  box_entry;
    isom_box_mdhd_t  *temp_box_mdhd;

    temp_box_mdhd = (isom_box_mdhd_t *) malloc(sizeof(isom_box_mdhd_t));
    if (temp_box_mdhd == NULL) {
        return ISOM_MEMORY_ERROR;
    }
    memset(temp_box_mdhd, 0x00, sizeof(isom_box_mdhd_t));   
    
    *box_mdhd = temp_box_mdhd;
    temp_box_mdhd->size = org_box_entry->box_data_size + org_box_entry->box_header_size;
    temp_box_mdhd->type = org_box_entry->box_type;

    memcpy(&box_entry, org_box_entry, sizeof(isom_box_entry_t));
    //
    // Parse this box's fields.
    isom_read_resource_uint32(mp4file_info->data_resource, MDHDPOS_VERSIONFLAGS, &temp_uint32, &box_entry);
    temp_box_mdhd->version = (UInt8)((temp_uint32 >> 24) & 0x000000ff);
    temp_box_mdhd->flags = temp_uint32 & 0x00ffffff;
    
    if (0 == temp_box_mdhd->version) {
        // Verify that this box is the correct length.
        if ( box_entry.box_data_size != 24 ) {
            ISOM_DEEP_DEBUG_PRINT(("ts_init_mdhd_box failed. Expected boxDataLength == 24 version: %d BoxDataLength: %"_64BITARG_"u\n",temp_box_mdhd->version, box_entry.box_data_size));
            return ISOM_FILE_INVALID_MDHD_BOX;
        }
        
        isom_read_resource_uint32_to_uint64(mp4file_info->data_resource, MDHDPOS_CREATIONTIME, &(temp_box_mdhd->creation_time), &box_entry);
        isom_read_resource_uint32_to_uint64(mp4file_info->data_resource, MDHDPOS_MODIFICATIONTIME, &(temp_box_mdhd->modification_time), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MDHDPOS_TIMESCALE, &(temp_box_mdhd->timescale), &box_entry);
        isom_read_resource_uint32_to_uint64(mp4file_info->data_resource, MDHDPOS_DURATION, &(temp_box_mdhd->duration), &box_entry);
        /* non use field */ 
        isom_read_resource_uint16(mp4file_info->data_resource, MDHDPOS_LANGUAGE, &(temp_box_mdhd->language), &box_entry);
        isom_read_resource_uint16(mp4file_info->data_resource, MDHDPOS_QUALITY, &(temp_box_mdhd->quality), &box_entry);
        /* non use field end*/ 
    }
    else if (1 == temp_box_mdhd->version) {
        // Verify that this box is the correct length.
        if( box_entry.box_data_size != 36 ) {
            ISOM_DEEP_DEBUG_PRINT(("ts_init_mdhd_box failed. Expected BoxDataLength == 36 version: %d BoxDataLength: %"_64BITARG_"u\n",temp_box_mdhd->version, box_entry.box_data_size));
            return ISOM_FILE_INVALID_MDHD_BOX;
        }
        
        isom_read_resource_uint64(mp4file_info->data_resource, MDHDPOSV1_CREATIONTIME, &(temp_box_mdhd->creation_time), &box_entry);
        isom_read_resource_uint64(mp4file_info->data_resource, MDHDPOSV1_MODIFICATIONTIME, &(temp_box_mdhd->modification_time), &box_entry);
        isom_read_resource_uint32(mp4file_info->data_resource, MDHDPOSV1_TIMESCALE, &(temp_box_mdhd->timescale), &box_entry);
        isom_read_resource_uint64(mp4file_info->data_resource, MDHDPOSV1_DURATION, &(temp_box_mdhd->duration), &box_entry);
        /* non use field */
        isom_read_resource_uint16(mp4file_info->data_resource, MDHDPOSV1_LANGUAGE, &(temp_box_mdhd->language), &box_entry);
        isom_read_resource_uint16(mp4file_info->data_resource, MDHDPOSV1_QUALITY, &(temp_box_mdhd->quality), &box_entry);
        /* non use field end*/
    }
    else {
        ISOM_DEEP_DEBUG_PRINT(("ts_init_mdhd_box  failed. Version unsupported: %d\n",temp_box_mdhd->version));
        return ISOM_FILE_INVALID_MDHD_BOX;
    }
    
    //
    // Compute the reciprocal of the timescale.
    temp_box_mdhd->timescale_recip = 1 / (Float64)temp_box_mdhd->timescale;
    //
    // This box has been successfully read in.
    return ISOM_OK;
}

/*
* Desc : hdlr box 의 속성값  isom_box_hdlr_t 구조체에 셋팅
*/
rs_status_t isom_init_hdlr_box(isom_file_info_t *mp4file_info, isom_box_entry_t *org_box_entry, isom_box_hdlr_t **box_hdlr)
{
    UInt32            temp_uint32;
    UInt32            name_utf8_length = 0;
    isom_box_entry_t  box_entry;
    isom_box_hdlr_t  *temp_box_hdlr;

    temp_box_hdlr = (isom_box_hdlr_t *) malloc(sizeof(isom_box_hdlr_t));
    if (temp_box_hdlr == NULL) {
        return ISOM_MEMORY_ERROR;
    }
    memset(temp_box_hdlr, 0x00, sizeof(isom_box_hdlr_t));   
    
    *box_hdlr = temp_box_hdlr;
    temp_box_hdlr->size = org_box_entry->box_data_size + org_box_entry->box_header_size;
    temp_box_hdlr->type = org_box_entry->box_type;

    memcpy(&box_entry, org_box_entry, sizeof(isom_box_entry_t));
    //
    // Parse this box's fields.
    isom_read_resource_uint32(mp4file_info->data_resource, HDLRPOS_VERSIONFLAGS, &temp_uint32, &box_entry);
    temp_box_hdlr->version = (UInt8)((temp_uint32 >> 24) & 0x000000ff);
    temp_box_hdlr->flags = temp_uint32 & 0x00ffffff;

    isom_read_resource_uint32(mp4file_info->data_resource, HDLRPOS_RESERVED1, &(temp_box_hdlr->reserved1), &box_entry);
    isom_read_resource_uint32(mp4file_info->data_resource, HDLRPOS_HANDLERTYPE, &(temp_box_hdlr->handler_type), &box_entry);

    isom_read_resource_bytes(mp4file_info->data_resource, HDLRPOS_RESERVED2, temp_box_hdlr->reserved2, 12, &box_entry);

    name_utf8_length = (UInt32)(box_entry.box_data_size - HDLRPOS_NAMEUTF8);

    if (name_utf8_length) {
        temp_box_hdlr->name_utf8 = (char*)malloc((UInt32) name_utf8_length);
        if (temp_box_hdlr->name_utf8 == NULL) {
            return ISOM_FILE_INVALID_HDLR_BOX;
        }
        isom_read_resource_bytes(mp4file_info->data_resource, HDLRPOS_NAMEUTF8, temp_box_hdlr->name_utf8, name_utf8_length, &box_entry);

        //safety check in case the string is not null-terminated
        if (temp_box_hdlr->name_utf8[name_utf8_length-1]) {
            char *str = (char*)malloc((UInt32) name_utf8_length + 1);
            if (str == NULL) {
                return ISOM_FILE_INVALID_HDLR_BOX;
            }
            memcpy(str, temp_box_hdlr->name_utf8, (UInt32) name_utf8_length);
            str[name_utf8_length] = 0;
            free(temp_box_hdlr->name_utf8);
            temp_box_hdlr->name_utf8 = str;
		}
    }
    /* non use field end*/   
    return ISOM_OK;
}

/*
* Desc : minf box 의 속성값  isom_box_minf_t 구조체에 셋팅
*/
rs_status_t isom_init_minf_box(isom_file_info_t *mp4file_info, isom_box_entry_t *org_box_entry, isom_box_minf_t **box_minf)
{
    isom_box_minf_t      *temp_box_minf;

    temp_box_minf = (isom_box_minf_t *) malloc(sizeof(isom_box_minf_t));
    if (temp_box_minf == NULL) {
        return ISOM_MEMORY_ERROR;
    }
    memset(temp_box_minf, 0x00, sizeof(isom_box_minf_t));

    *box_minf = temp_box_minf;
    temp_box_minf->size = org_box_entry->box_data_size + org_box_entry->box_header_size;
    temp_box_minf->type = org_box_entry->box_type;

    return ISOM_OK;
}

/*
* Desc : dinf box 의 속성값  isom_box_dinf_t 구조체에 셋팅
*/
rs_status_t isom_init_dinf_box(isom_file_info_t *mp4file_info, isom_box_entry_t *org_box_entry, isom_box_dinf_t **box_dinf)
{
    isom_box_dinf_t      *temp_box_dinf;

    temp_box_dinf = (isom_box_dinf_t *) malloc(sizeof(isom_box_dinf_t));
    if (temp_box_dinf == NULL) {
        return ISOM_MEMORY_ERROR;
    }
    memset(temp_box_dinf, 0x00, sizeof(isom_box_minf_t));

    *box_dinf = temp_box_dinf;
    temp_box_dinf->size = org_box_entry->box_data_size + org_box_entry->box_header_size;
    temp_box_dinf->type = org_box_entry->box_type;

    return ISOM_OK;
}

/*
* Desc : dref box 의 속성값  isom_box_dref_t 구조체에 셋팅
*/
rs_status_t isom_init_dref_box(isom_file_info_t *mp4file_info, isom_box_entry_t *org_box_entry, isom_box_dref_t **box_dref)
{
    isom_box_dref_t  *temp_box_dref;
    UInt32            temp_uint32;
    isom_box_entry_t  box_entry;

    temp_box_dref = (isom_box_dref_t *) malloc(sizeof(isom_box_dref_t));
    if (temp_box_dref == NULL) {
        return ISOM_MEMORY_ERROR;
    }
    memset(temp_box_dref, 0x00, sizeof(isom_box_dref_t));
  
    *box_dref = temp_box_dref;
    temp_box_dref->size = org_box_entry->box_data_size + org_box_entry->box_header_size;
    temp_box_dref->type = org_box_entry->box_type;

    memcpy(&box_entry, org_box_entry, sizeof(isom_box_entry_t));
    //
    // Parse this box's fields.
    isom_read_resource_uint32(mp4file_info->data_resource, DREFPOS_VERSIONFLAGS, &temp_uint32, &box_entry);
    temp_box_dref->version = (UInt8)((temp_uint32 >> 24) & 0x000000ff);
    temp_box_dref->flags = temp_uint32 & 0x00ffffff;

    isom_read_resource_uint32(mp4file_info->data_resource, DREFPOS_NUMREFS, &(temp_box_dref->num_refs), &box_entry);

    return ISOM_OK;
}

/*
* Desc : stbl box 의 속성값  isom_box_stbl_t 구조체에 셋팅
*/
rs_status_t isom_init_stbl_box(isom_file_info_t *mp4file_info, isom_box_entry_t *org_box_entry, isom_box_stbl_t **box_stbl)
{
    isom_box_stbl_t      *temp_box_stbl;

    temp_box_stbl = (isom_box_stbl_t *) malloc(sizeof(isom_box_stbl_t));
    if (temp_box_stbl == NULL) {
        return ISOM_MEMORY_ERROR;
    }
    memset(temp_box_stbl, 0x00, sizeof(isom_box_stbl_t));

    *box_stbl = temp_box_stbl;
    temp_box_stbl->size = org_box_entry->box_data_size + org_box_entry->box_header_size;
    temp_box_stbl->type = org_box_entry->box_type;

    return ISOM_OK;
}

/*
* Desc : stts box 의 속성값  isom_box_stts_t 구조체에 셋팅
*/
rs_status_t isom_init_stts_box(isom_file_info_t *mp4file_info, isom_box_entry_t *org_box_entry, isom_box_stts_t **box_stts)
{
    UInt32           temp_uint32;
    isom_box_entry_t     box_entry;
    isom_box_stts_t      *temp_box_stts;

    temp_box_stts = (isom_box_stts_t *) malloc(sizeof(isom_box_stts_t));
    if (temp_box_stts == NULL) {
        return ISOM_MEMORY_ERROR;
    }
    memset(temp_box_stts, 0x00, sizeof(isom_box_stts_t));   
    
    *box_stts = temp_box_stts;

    memcpy(&box_entry, org_box_entry, sizeof(isom_box_entry_t));
    //
    // Parse this box's fields.
    isom_read_resource_uint32(mp4file_info->data_resource, STTSPOS_VERSIONFLAGS, &temp_uint32, &box_entry);
    temp_box_stts->version = (UInt8)((temp_uint32 >> 24) & 0x000000ff);
    temp_box_stts->flags = temp_uint32 & 0x00ffffff;
    
    isom_read_resource_uint32(mp4file_info->data_resource, STTSPOS_NUMENTRIES, &(temp_box_stts->numentries), &box_entry);
    
    //
    // Validate the size of the sample table.
    if ((unsigned long)(temp_box_stts->numentries * 8) != (box_entry.box_data_size - 8)) {
        return ISOM_FILE_INVALID_STISOM_BOX;
    }
    //
    // Read in the time-to-sample table.
    temp_box_stts->time_to_sample_table = (char *) malloc(temp_box_stts->numentries * 8);
    if (temp_box_stts->time_to_sample_table == NULL) {
        return ISOM_FILE_INVALID_STISOM_BOX;
    }
    
    isom_read_resource_bytes(mp4file_info->data_resource, STTSPOS_SAMPLETABLE, (char *)temp_box_stts->time_to_sample_table, temp_box_stts->numentries * 8, &box_entry);

    //
    // This box has been successfully read in.
    return ISOM_OK;
}

/*
* Desc : ctts box 의 속성값  isom_box_ctts_t 구조체에 셋팅
*/
rs_status_t isom_init_ctts_box(isom_file_info_t *mp4file_info, isom_box_entry_t *org_box_entry, isom_box_ctts_t **box_ctts)
{
    UInt32           temp_uint32;
    isom_box_entry_t     box_entry;
    isom_box_ctts_t      *temp_box_ctts;

    temp_box_ctts = (isom_box_ctts_t *) malloc(sizeof(isom_box_ctts_t));
    if (temp_box_ctts == NULL) {
        return ISOM_MEMORY_ERROR;
    }
    memset(temp_box_ctts, 0x00, sizeof(isom_box_ctts_t));   
    
    *box_ctts = temp_box_ctts;

    memcpy(&box_entry, org_box_entry, sizeof(isom_box_entry_t));
    //
    // Parse this box's fields.
    isom_read_resource_uint32(mp4file_info->data_resource, CTTSPOS_VERSIONFLAGS, &temp_uint32, &box_entry);
    temp_box_ctts->version = (UInt8)((temp_uint32 >> 24) & 0x000000ff);
    temp_box_ctts->flags = temp_uint32 & 0x00ffffff;
    
    isom_read_resource_uint32(mp4file_info->data_resource, CTTSPOS_NUMENTRIES, &(temp_box_ctts->numentries), &box_entry);
    
    //
    // Validate the size of the sample table.
    if ((unsigned long)(temp_box_ctts->numentries * 8) != (box_entry.box_data_size - 8)) {
        return ISOM_FILE_INVALID_CTISOM_BOX;
    }
    //
    // Read in the time-to-sample table.
    temp_box_ctts->time_to_sample_table = (char *) malloc(temp_box_ctts->numentries * 8);
    if (temp_box_ctts->time_to_sample_table == NULL){
        return ISOM_FILE_INVALID_CTISOM_BOX;
    }
    
    isom_read_resource_bytes(mp4file_info->data_resource, CTTSPOS_SAMPLETABLE, (char *)temp_box_ctts->time_to_sample_table, temp_box_ctts->numentries * 8, &box_entry);

    //
    // This box has been successfully read in.
    return ISOM_OK;
}

/*
* Desc : stsc box 의 속성값  isom_box_stsc_t 구조체에 셋팅
*/
rs_status_t isom_init_stsc_box(isom_file_info_t *mp4file_info, isom_box_entry_t *org_box_entry, isom_box_stsc_t **box_stsc)
{
    UInt32            temp_uint32;
    isom_box_entry_t  box_entry;
    isom_box_stsc_t  *temp_box_stsc;

    temp_box_stsc = (isom_box_stsc_t *) malloc(sizeof(isom_box_stsc_t));
    if (temp_box_stsc == NULL) {
        return ISOM_MEMORY_ERROR;
    }
    memset(temp_box_stsc, 0x00, sizeof(isom_box_stsc_t));   
    
    *box_stsc = temp_box_stsc;

    memcpy(&box_entry, org_box_entry, sizeof(isom_box_entry_t));
    //
    // Parse this box's fields.
    isom_read_resource_uint32(mp4file_info->data_resource, STSCPOS_VERSIONFLAGS, &temp_uint32, &box_entry);
    temp_box_stsc->version = (UInt8)((temp_uint32 >> 24) & 0x000000ff);
    temp_box_stsc->flags = temp_uint32 & 0x00ffffff;
    
    isom_read_resource_uint32(mp4file_info->data_resource, STSCPOS_NUMENTRIES, &(temp_box_stsc->numentries), &box_entry);
    
    //
    // Validate the size of the sample table.
    if ((unsigned long)(temp_box_stsc->numentries * 12) != (box_entry.box_data_size - 8)) {
        return ISOM_FILE_INVALID_STSC_BOX;
    }
    //
    // Read in the time-to-sample table.
    temp_box_stsc->sample_to_chunk_table = (char *) malloc(temp_box_stsc->numentries * 12);

    if (temp_box_stsc->sample_to_chunk_table == NULL){
        return ISOM_MEMORY_ERROR;
    }
    
    isom_read_resource_bytes(mp4file_info->data_resource, STSCPOS_SAMPLETABLE, (char *)temp_box_stsc->sample_to_chunk_table, temp_box_stsc->numentries * 12, &box_entry);

    //
    // This box has been successfully read in.
    return ISOM_OK;
}

/*
* Desc : stsd box 의 속성값  isom_box_stsd_t 구조체에 셋팅
*/
rs_status_t isom_init_stsd_box(isom_file_info_t *mp4file_info, isom_box_entry_t *org_box_entry, isom_box_stsd_t **box_stsd)
{
    rs_status_t       result_code = ISOM_UNKNOWN;
    UInt32            temp_uint32;
    UInt32            cur_desc;
    isom_box_entry_t  box_entry;
    isom_box_stsd_t  *temp_box_stsd;
    //char                *p_sampledescriptiontable;
    char             *sample_description_table;
    isom_resource_t  *memory_resource;
    isom_box_t       *tmp_box = NULL;

    temp_box_stsd = (isom_box_stsd_t *) malloc(sizeof(isom_box_stsd_t));
    if (temp_box_stsd == NULL) {
        return ISOM_MEMORY_ERROR;
    }
    memset(temp_box_stsd, 0x00, sizeof(isom_box_stsd_t));   
    
    *box_stsd = temp_box_stsd;

    memcpy(&box_entry, org_box_entry, sizeof(isom_box_entry_t));
    //
    // Parse this box's fields.
    isom_read_resource_uint32(mp4file_info->data_resource, STSDPOS_VERSIONFLAGS, &temp_uint32, &box_entry);
    temp_box_stsd->version = (UInt8)((temp_uint32 >> 24) & 0x000000ff);
    temp_box_stsd->flags = temp_uint32 & 0x00ffffff;
    
    isom_read_resource_uint32(mp4file_info->data_resource, STSDPOS_NUMENTRIES, &(temp_box_stsd->numentries), &box_entry);
    
    //
    // Read in all of the sample descriptions.
    if (temp_box_stsd->numentries > 0) {
        //
        // Allocate our description tables.
        UInt64 table_size = box_entry.box_data_size - 8;
        if (table_size > kSInt32_Max) {
            return ISOM_FILE_INVALID_STSD_BOX;
        }
       
        temp_box_stsd->sample_description_table = (char *) malloc((SInt32)table_size);
        if (temp_box_stsd->sample_description_table == NULL) {
            return ISOM_MEMORY_ERROR;
        }
        
        //
        // Read in the sample description table.
        isom_read_resource_bytes(mp4file_info->data_resource, STSDPOS_SAMPLETABLE, temp_box_stsd->sample_description_table, (UInt32) table_size, &box_entry);
        //
        // Read them all in..
        sample_description_table = temp_box_stsd->sample_description_table;
        
        if ((result_code = isom_init_resource_mem(&memory_resource, sample_description_table, (UInt32)table_size)) != ISOM_OK) {
            return result_code;
        }

        temp_box_stsd->box_list = isom_create_list();
        if (!temp_box_stsd->box_list) {
            return ISOM_MEMORY_ERROR;
	    }
        
        for (cur_desc = 0; cur_desc < temp_box_stsd->numentries; cur_desc++) {
            if ((result_code = isom_parse_box(&tmp_box, memory_resource)) != ISOM_OK) {
                isom_delete_resource_mem(memory_resource);
                return result_code;
            }
            if (tmp_box == NULL) {
                return ISOM_CODEC_BOX_NULL;
            }
            if (!isom_add_stsd_box(temp_box_stsd, tmp_box)) {
                isom_delete_resource_mem(memory_resource);
                return ISOM_ADD_CODEC_BOX_FAIL;
            }
            
        }
        isom_delete_resource_mem(memory_resource);
    }
    //
    // This box has been successfully read in.
    
    return ISOM_OK;
}

/*
* Desc : stco box 의 속성값  isom_box_stss_t 구조체에 셋팅
*/
rs_status_t isom_init_stco_box(isom_file_info_t *mp4file_info, isom_box_entry_t *org_box_entry, isom_box_stco_t **box_stco, UInt16 offset_size)
{
    UInt32           temp_uint32;
    isom_box_entry_t     box_entry;
    isom_box_stco_t      *temp_box_stco;
    
    temp_box_stco = (isom_box_stco_t *) malloc(sizeof(isom_box_stco_t));
    if (temp_box_stco == NULL) {
        return ISOM_MEMORY_ERROR;
    }
    memset(temp_box_stco, 0x00, sizeof(isom_box_stco_t));  
    
    *box_stco = temp_box_stco;
    
    memcpy(&box_entry, org_box_entry, sizeof(isom_box_entry_t));
    temp_box_stco->offset_size = offset_size;

    //
    // Parse this box's fields.
    isom_read_resource_uint32(mp4file_info->data_resource, STCOPOS_VERSIONFLAGS, &temp_uint32, &box_entry);
    temp_box_stco->version = (UInt8)((temp_uint32 >> 24) & 0x000000ff);
    temp_box_stco->flags = temp_uint32 & 0x00ffffff;
    
    isom_read_resource_uint32(mp4file_info->data_resource, STCOPOS_NUMENTRIES, &(temp_box_stco->numentries), &box_entry);
    
    //
    // Validate the size of the sample table.
    if ((unsigned long)(temp_box_stco->numentries * temp_box_stco->offset_size) != (box_entry.box_data_size - 8)) {
        return ISOM_FILE_INVALID_STCO_BOX;
    }
    //
    // Read in the time-to-sample table.
    temp_box_stco->chunk_offset_table = (char *) malloc((temp_box_stco->numentries * temp_box_stco->offset_size) + 1);
    
    if (temp_box_stco->chunk_offset_table == NULL){
        return ISOM_MEMORY_ERROR;
    }

    temp_box_stco->table = (void *)temp_box_stco->chunk_offset_table;
    
    isom_read_resource_bytes(mp4file_info->data_resource, STCOPOS_SAMPLETABLE, (char *)temp_box_stco->table, temp_box_stco->numentries * temp_box_stco->offset_size, &box_entry);
    
    return ISOM_OK;    
}

/*
* Desc : stsz box 의 속성값  isom_box_stsd_t 구조체에 셋팅
*/
rs_status_t isom_init_stsz_box(isom_file_info_t *mp4file_info, isom_box_entry_t *org_box_entry, isom_box_stsz_t **box_stsz)
{
    UInt32           temp_uint32;
    isom_box_entry_t     box_entry;
    isom_box_stsz_t      *temp_box_stsz;

    temp_box_stsz = (isom_box_stsz_t *) malloc(sizeof(isom_box_stsz_t));
    if (temp_box_stsz == NULL) {
        return ISOM_MEMORY_ERROR;
    }
    memset(temp_box_stsz, 0x00, sizeof(isom_box_stsz_t));   
    
    *box_stsz = temp_box_stsz;

    memcpy(&box_entry, org_box_entry, sizeof(isom_box_entry_t));
    //
    // Parse this box's fields.
    isom_read_resource_uint32(mp4file_info->data_resource, STSZPOS_VERSIONFLAGS, &temp_uint32, &box_entry);
    temp_box_stsz->version = (UInt8)((temp_uint32 >> 24) & 0x000000ff);
    temp_box_stsz->flags = temp_uint32 & 0x00ffffff;

    isom_read_resource_uint32(mp4file_info->data_resource, STSZPOS_SAMPLESIZE, &(temp_box_stsz->common_sample_size), &box_entry);
    
    isom_read_resource_uint32(mp4file_info->data_resource, STSZPOS_NUMENTRIES, &(temp_box_stsz->numentries), &box_entry);
    
    if (temp_box_stsz->common_sample_size != 0) {
        temp_box_stsz->sample_size_table = NULL;
        temp_box_stsz->table = NULL;
    }
    else {
        //
        // Validate the size of the sample table.
        if ((unsigned long)(temp_box_stsz->numentries * 4) != (box_entry.box_data_size - 12)) {
            return ISOM_FILE_INVALID_STSZ_BOX;
        }
        //
        // Read in the time-to-sample table.
        temp_box_stsz->sample_size_table = (char *) malloc((temp_box_stsz->numentries * 4) + 1);
        
        if (temp_box_stsz->sample_size_table == NULL){
            return ISOM_MEMORY_ERROR;
        }
        temp_box_stsz->table = (UInt32 *)temp_box_stsz->sample_size_table;
        
        isom_read_resource_bytes(mp4file_info->data_resource, STSZPOS_SAMPLETABLE, (char *)temp_box_stsz->table, temp_box_stsz->numentries * 4, &box_entry);
    }
    //
    // This box has been successfully read in.
    return ISOM_OK;
}

/*
* Desc : stss box 의 속성값  isom_box_stss_t 구조체에 셋팅
*/
rs_status_t isom_init_stss_box(isom_file_info_t *mp4file_info, isom_box_entry_t *org_box_entry, isom_box_stss_t **box_stss)
{
    UInt32           temp_uint32;
    isom_box_entry_t     box_entry;
    isom_box_stss_t      *temp_box_stss;
    Bool16           init_succeeds = false;

    temp_box_stss = (isom_box_stss_t *) malloc(sizeof(isom_box_stss_t));
    if (temp_box_stss == NULL) {
        return ISOM_MEMORY_ERROR;
    }
    memset(temp_box_stss, 0x00, sizeof(isom_box_stss_t));   
    
    *box_stss = temp_box_stss;

    memcpy(&box_entry, org_box_entry, sizeof(isom_box_entry_t));
    //
    // Parse this box's fields.
    isom_read_resource_uint32(mp4file_info->data_resource, STSSPOS_VERSIONFLAGS, &temp_uint32, &box_entry);
    temp_box_stss->version = (UInt8)((temp_uint32 >> 24) & 0x000000ff);
    temp_box_stss->flags = temp_uint32 & 0x00ffffff;

    isom_read_resource_uint32(mp4file_info->data_resource, STSSPOS_NUMENTRIES, &(temp_box_stss->numentries), &box_entry);

    //
    // Validate the size of the sample table.
    if( (unsigned long)(temp_box_stss->numentries * 4) != (box_entry.box_data_size - 8) ) {
        return ISOM_FILE_INVALID_STSS_BOX;
    }

    temp_box_stss->sync_sample_table = (char *) malloc( (temp_box_stss->numentries * 4) + 1 );
    if( temp_box_stss->sync_sample_table == NULL ) {
        return ISOM_MEMORY_ERROR;
    }

    temp_box_stss->table = (UInt32 *)temp_box_stss->sync_sample_table;
    
    if (isom_read_resource_bytes(mp4file_info->data_resource,STSSPOS_SAMPLETABLE, (char *)temp_box_stss->table, temp_box_stss->numentries * 4, &box_entry) == ISOM_OK) {
        // This box has been successfully read in.
        // sample offsets are in network byte order on disk, convert them to host order
        UInt32      sample_index = 0;
        
        // convert each sample to host order
        // NOTE - most other Boxs handle byte order conversions in
        // the accessor function.  For efficiency reasons it's converted
        // to host order here for sync samples.
        
        for ( sample_index = 0; sample_index < temp_box_stss->numentries; sample_index++ ) {
            temp_box_stss->table[sample_index] = ntohl( temp_box_stss->table[sample_index] );
            
        }
    }

    return ISOM_OK;
}

/*
* Desc : list del
*/
Bool16 isom_delete_box_list(ut_list_t *box_list)
{
    UInt32 count, i;
    isom_box_t *a;
    if (!box_list) {
        return false;
    }

    count = isom_get_list_count(box_list); 
    for (i = 0; i < count; i++) {
        a = (isom_box_t *)isom_get_list(box_list, i);
        if (a) {
            isom_delete_box(a);
        }
    }
    isom_delete_list(box_list);
    return true;
}

/*
* Desc : stsd box delete
*/
Bool16 isom_delete_stsd_box(isom_box_stsd_t *box_stsd)
{
    if (box_stsd == NULL) {
        return false;
    }
    isom_delete_box_list(box_stsd->box_list);
    free(box_stsd);
    return true;
}

/*
* Desc : 
*/
Bool16 isom_create_default_box(isom_box_t **box)
{
    isom_unknown_box_t *tmp = (isom_unknown_box_t *) malloc(sizeof(isom_unknown_box_t));
    memset(tmp, 0, sizeof(isom_unknown_box_t));
    *box = (isom_box_t *) tmp;
    return true;
}




/*
* Desc :
*/
Bool16 isom_set_default_box(isom_box_t *newbox, isom_resource_t *data_resource)
{
    UInt32 bytes_to_read;
    isom_unknown_box_t *ptr = (isom_unknown_box_t *)newbox;
    if (ptr->size > 0xFFFFFFFF) {
        return false;
    }

    bytes_to_read = (UInt32) (ptr->size);
    
    if (bytes_to_read) {
        ptr->data = (char*)malloc(bytes_to_read);
        if (ptr->data == NULL ) {
            return false;
        }
        ptr->dataSize = bytes_to_read;
        isom_read_resource_bytes(data_resource, data_resource->res_mem->mem_pos, ptr->data, ptr->dataSize, NULL);
    }
    return true;
}

/*
* Desc :  stsd 하위box 저장 
*/
Bool16 isom_add_stsd_box(isom_box_stsd_t *stsd, isom_box_t *box)
{
    if (!box) {
        return false;
    }

	switch (box->type) {
	case ISOM_BOX_TYPE_MP4A:
	case ISOM_BOX_TYPE_MP4V:
	case ISOM_BOX_TYPE_AVC1:
        /*3GPP boxes*/
    case ISOM_SUBTYPE_3GP_AMR:
    case ISOM_SUBTYPE_3GP_AMR_WB:
    case ISOM_SUBTYPE_3GP_EVRC:
    case ISOM_SUBTYPE_K3G_EVRC:
    case ISOM_SUBTYPE_3GP_QCELP:
    case ISOM_SUBTYPE_3GP_SMV:
    case ISOM_SUBTYPE_3GP_H263: 
		return isom_add_list(stsd->box_list, box);
	default:
		return false;
	}
    return true;
}


/*
* Desc : debugging function isom_box_entry_t 값 확인  
*/
void isom_dump_box_entry(isom_file_info_t *mp4file_info)
{
    // General vars
    isom_box_entry_t     *box;
    isom_box_entry_t     *box_entry = mp4file_info->box_entry;
    char                Indent[128] = { '\0' };
    
    // Display all of the boxs.
    ISOM_DEBUG_PRINT(("ts_dump_box_entry - dumping box entry.\n"));

    for (box = box_entry; box != NULL;) {
        // Print out this box.     
        ISOM_DEBUG_PRINT(("%s[%03lu] box_type=%c%c%c%c; box_data_pos=%"_64BITARG_"u; box_data_size=%"_64BITARG_"u\n",
            Indent,
            box->box_entry_id,
            (char)((box->box_type & 0xff000000) >> 24),
            (char)((box->box_type & 0x00ff0000) >> 16),
            (char)((box->box_type & 0x0000ff00) >> 8),
            (char)((box->box_type & 0x000000ff)),
            box->box_data_pos, box->box_data_size));
        
        // Descend into this box's children if it has any.
        if( box->first_child_box != NULL ) {
            box = box->first_child_box;
            strcat(Indent, "  ");
            continue;
        }
        
        // Are we at the end of a sibling list?  If so, move up a level.  Keep
        // moving up until we find a non-NULL box.
        while (box && (box->next_sibling_box == NULL)) {
            box = box->parent_box;
            Indent[strlen(Indent) - 1] = '\0';
        }
        
        // Next box..
        if ( box != NULL ) { // could be NULL if we just moved up to a NULL parent
            box = box->next_sibling_box;
        }
    }
}


/*
* Desc : box parsing  
*/
rs_status_t isom_generate_box_entry(isom_file_info_t *mp4file_info)
{
    OSType            box_type;
    UInt32            box_length;
    UInt32            next_box_entry_id = 1;
    UInt64            big_box_length;
    UInt64            cur_pos;
    unsigned long     cur_box_header_size;
    Bool16            has_moov_box  = false;
    Bool16            has_big_box   = false;
    isom_resource_t  *data_resource   = mp4file_info->data_resource;
    isom_box_entry_t *new_box_entry = NULL,
                     *cur_parent    = NULL,
                     *last_box_enrty = NULL;
    rs_status_t       result_code    = ISOM_UNKNOWN;
    //
    // Scan through all of the boxs in this movie, generating a box_entry
    // for each one.
    cur_pos = 0;
    while (isom_read_resource(data_resource, cur_pos, (char *)&box_length, 4) == ISOM_OK) {
        //
        // Swap the BoxLength for little-endian machines.
        cur_pos += 4;
        box_length = ntohl(box_length);
        big_box_length = (UInt64) box_length;
        has_big_box = false;

        //
        // Is BoxLength zero?  If so, and we're in a 'udta' box, then all
        // is well (this is the end of a 'udta' box).  Leave this level of
        // siblings as the 'udta' box is obviously over.
        if ((0 == big_box_length) && cur_parent && (cur_parent->box_type == FOUR_CHARS_TO_INT('u', 'd', 't', 'a'))) {
            //
            // Do no harm..
            ISOM_DEEP_DEBUG_PRINT(("isom_generate_box_entry - Found End-of-udta marker.\n"));
            last_box_enrty = cur_parent;
            cur_parent = cur_parent->parent_box;

            //
            // Keep moving up.
            goto SKIP_BOX;

        //
        // Is the BoxLength zero?  If so, this is a "QT box" which needs
        // some additional work before it can be processed.
        } 
        else if (0 == big_box_length) { 
            //
            // This is a ts box; skip the (rest of the) reserved field and
            // the lock count field.
            cur_pos += 22;

            //
            // Read the size and the type of this box.
            if ((result_code = isom_read_resource(data_resource, cur_pos, (char *)&box_length, 4)) != ISOM_OK) {
                return result_code;
            }

            cur_pos += 4;
            big_box_length =  (UInt64) ntohl(box_length);
            
            if ((result_code = isom_read_resource(data_resource, cur_pos, (char *)&box_type, 4)) != ISOM_OK) {
                return result_code;
            }

            cur_pos += 4;
            box_type = ntohl(box_type);

            //
            // Skip over the rest of the fields.
            cur_pos += 12;
            
            //
            // Set the header size to that of a QT box.
            cur_box_header_size = 10 + 16 + 4 + 4 + 4 + 2 + 2 + 4;

        //
        // This is a normal box; get the box type.
        } 
        else {  // This is a normal box; get the box type.

            if ((result_code = isom_read_resource(data_resource, cur_pos, (char *)&box_type, 4)) != ISOM_OK) {
                break;
            }

            cur_pos += 4;
            cur_box_header_size = 4 + 4; // BoxLength + box_type
            box_type = ntohl(box_type);

            if (1 == box_length) { //large size box
                if ((result_code = isom_read_resource(data_resource, cur_pos, (char *)&big_box_length, 8)) != ISOM_OK) {
                    break;
                }

                big_box_length = ut_ntoh64(big_box_length);
                cur_pos += 8;
                cur_box_header_size += 8; // BoxLength + box_type + big box length
                has_big_box = true;
            }
             
            if (box_type == FOUR_CHARS_TO_INT('u', 'u', 'i', 'd')) {
                UInt8 usertype[SEXTENDED_TYPE_SIZE + 1]; //sExtendedTypeSize for the type + 1 for 0 terminator.
                usertype[SEXTENDED_TYPE_SIZE] = 0;

                if ((result_code = isom_read_resource(data_resource, cur_pos, (char *)usertype, 16)) != ISOM_OK) { // read and just throw it away we don't need to store
                    return result_code;
                }
    
                ISOM_DEEP_DEBUG_PRINT(("isom_generate_box_entry - Found 'uuid' extended type name= %s.\n",usertype));
                cur_pos += SEXTENDED_TYPE_SIZE;
                cur_box_header_size += SEXTENDED_TYPE_SIZE;
            }

        }


        if (0 == big_box_length) {
            ISOM_DEEP_DEBUG_PRINT(("isom_generate_box_entry - Fail box is bad. type= '%c%c%c%c'; pos=%"_64BITARG_"u; length=%"_64BITARG_"u; header=%lu.\n",
            (char)((box_type & 0xff000000) >> 24),
            (char)((box_type & 0x00ff0000) >> 16),
            (char)((box_type & 0x0000ff00) >> 8),
            (char)((box_type & 0x000000ff)),
            cur_pos - cur_box_header_size, big_box_length, cur_box_header_size)); 
            return ISOM_FILE_INVALID_BOX_SIZE;
        }

        if (has_big_box) {   
            ISOM_DEEP_DEBUG_PRINT(("isom_generate_box_entry - Found 64 bit box '%c%c%c%c'; pos=%"_64BITARG_"u; length=%"_64BITARG_"u; header=%lu.\n",
            (char)((box_type & 0xff000000) >> 24),
            (char)((box_type & 0x00ff0000) >> 16),
            (char)((box_type & 0x0000ff00) >> 8),
            (char)((box_type & 0x000000ff)),
            cur_pos - cur_box_header_size, big_box_length, cur_box_header_size));
        }
        else {
            ISOM_DEEP_DEBUG_PRINT(("isom_generate_box_entry - Found 32 bit box '%c%c%c%c'; pos=%"_64BITARG_"u; length=%"_64BITARG_"u; header=%lu.\n",
            (char)((box_type & 0xff000000) >> 24),
            (char)((box_type & 0x00ff0000) >> 16),
            (char)((box_type & 0x0000ff00) >> 8),
            (char)((box_type & 0x000000ff)),
            cur_pos - cur_box_header_size, big_box_length, cur_box_header_size));
        }

        if ((box_type == FOUR_CHARS_TO_INT('m', 'o', 'o', 'v')) && (has_moov_box)) {
            //
            // Skip over any additional 'moov' boxs once we find one.
            cur_pos += big_box_length - cur_box_header_size;
            continue;
        }
        else if (box_type == FOUR_CHARS_TO_INT('m', 'o', 'o', 'v')) {
           has_moov_box = true;
        }
        else if (!has_moov_box) {
            cur_pos += big_box_length - cur_box_header_size; 
            continue;
        }

        //
        // Create a box_entry for this box.
        new_box_entry = (isom_box_entry_t *) malloc(sizeof(isom_box_entry_t));

        if (new_box_entry == NULL) {
            return ISOM_MEMORY_ERROR;
        }
        memset(new_box_entry, 0x00, sizeof(isom_box_entry_t));
        new_box_entry->box_entry_id = next_box_entry_id++;
        new_box_entry->box_type = box_type;
        new_box_entry->big_endian_box_type = htonl(box_type);

        new_box_entry->box_data_pos = cur_pos;
        new_box_entry->box_data_size = big_box_length - cur_box_header_size;
        new_box_entry->box_header_size = cur_box_header_size;

        new_box_entry->next_order_box = NULL;

        new_box_entry->prev_sibling_box = last_box_enrty;
        new_box_entry->next_sibling_box = NULL;

        if (new_box_entry->prev_sibling_box) {
            new_box_entry->prev_sibling_box->next_sibling_box = new_box_entry;
        }

        new_box_entry->parent_box = cur_parent;
        new_box_entry->first_child_box = NULL;

        last_box_enrty = new_box_entry;

        //
        // Make this entry the head of the box_entry list if necessary.
        if (mp4file_info->box_entry == NULL) {
            ISOM_DEEP_DEBUG_PRINT(("isom_generate_box_entry - Placing this box at the head of the box_entry.\n"));
            mp4file_info->box_entry = new_box_entry;
            mp4file_info->box_list_head = mp4file_info->box_list_tail = new_box_entry;
        } 
        else {
            mp4file_info->box_list_tail->next_order_box = new_box_entry;
            mp4file_info->box_list_tail = new_box_entry;
        }

        //
        // Make this the first child if we have one.
        if (cur_parent && (cur_parent->first_child_box == NULL)) {
            ISOM_DEEP_DEBUG_PRINT(("isom_generate_box_entry - ..This box is the first child of our new parent.\n"));
            cur_parent->first_child_box = new_box_entry;
        }


        //
        // Figure out if we have to descend into this entry and do so.
        switch (new_box_entry->box_type) {
            case FOUR_CHARS_TO_INT('m', 'o', 'o', 'v'): //moov
            case FOUR_CHARS_TO_INT('c', 'l', 'i', 'p'): //clip
            case FOUR_CHARS_TO_INT('t', 'r', 'a', 'k'): //trak
            case FOUR_CHARS_TO_INT('m', 'a', 't', 't'): //matt
            case FOUR_CHARS_TO_INT('e', 'd', 't', 's'): //edts
            case FOUR_CHARS_TO_INT('t', 'r', 'e', 'f'): //tref
            case FOUR_CHARS_TO_INT('m', 'd', 'i', 'a'): //mdia
            case FOUR_CHARS_TO_INT('m', 'i', 'n', 'f'): //minf
            case FOUR_CHARS_TO_INT('d', 'i', 'n', 'f'): //dinf
            case FOUR_CHARS_TO_INT('s', 't', 'b', 'l'): //stbl
            case FOUR_CHARS_TO_INT('u', 'd', 't', 'a'): /* can appear anywhere */ //udta
            case FOUR_CHARS_TO_INT('h', 'n', 't', 'i'): //hnti
            case FOUR_CHARS_TO_INT('h', 'i', 'n', 'f'): //hinf
            {
                //
                // All of the above boxs need to be descended into.  Set up
                // our variables to descend into this box.

                if (new_box_entry->box_data_size > 0) { // maybe it should be greater than some number such as header size?
                   ISOM_DEEP_DEBUG_PRINT(("isom_generate_box_entry - ..Creating a new parent.\n"));
                    cur_parent = new_box_entry;
                    last_box_enrty = NULL;
                    continue; // skip the level checks below
                }
                else {
                    ISOM_DEEP_DEBUG_PRINT(("isom_generate_box_entry - Empty box.\n"));
                }
            
            }
            break;
        }
        //
        // Skip over this box's data.
        cur_pos += new_box_entry->box_data_size;

        //
        // Would continuing to the next box cause us to leave this level?
        // If so, move up a level and move on.  Keep doing this until we find
        // a level we can continue on.
SKIP_BOX:
        while (cur_parent && 
                (
                    (last_box_enrty->box_data_pos - last_box_enrty->box_header_size) + 
                    (last_box_enrty->box_data_size + last_box_enrty->box_header_size)
                ) >= 
                (
                    (cur_parent->box_data_pos - cur_parent->box_header_size) +
                    (cur_parent->box_data_size + cur_parent->box_header_size)
                )
              ) {
            ISOM_DEEP_DEBUG_PRINT(("isom_generate_box_entry - End of this parent's ('%c%c%c%c') children.\n",
                (char)((cur_parent->box_type & 0xff000000) >> 24),
                (char)((cur_parent->box_type & 0x00ff0000) >> 16),
                (char)((cur_parent->box_type & 0x0000ff00) >> 8),
                (char)((cur_parent->box_type & 0x000000ff))));
        
            last_box_enrty = cur_parent;
            cur_parent = cur_parent->parent_box;
        }
    }

    //
    // The box_entry has been successfully read in.
    return ISOM_OK;
}


/*
* Desc : box의 offset 위치를 찾는다.  
*/
rs_status_t isom_find_box_entry(isom_file_info_t *mp4file_info, const char *box_path, isom_box_entry_t **box_entry, isom_box_entry_t *last_found_box_entry)
{
    // General vars
    isom_box_entry_t     *box, *cur_parent;
    const char         *pcur_box_type = box_path;
    UInt32              root_box_id = 0;

    ISOM_DEEP_DEBUG_PRINT(("isom_find_box_entry - Searching for \"%s\".\n", box_path));

    //
    // If we were given a last_found_box_entry to start from, then we need to
    // find that before we can do the real search.
    if (last_found_box_entry != NULL) {
        for (box = mp4file_info->box_list_head; ; box = box->next_order_box) {
            //
            // If it's NULL, then something is seriously wrong.
            if (box == NULL) {
                return ISOM_BAD_PARAM;
            }

            //
            // Check for matches.
            if (box->box_entry_id == last_found_box_entry->box_entry_id) {
                break;
            }
        }

        //
        // Is this a root search or a rooted search?
        if (*pcur_box_type == ':') {
            ISOM_DEEP_DEBUG_PRINT(("isom_find_box_entry - ..Rooting search at [%03lu].\n", last_found_box_entry->box_entry_id));

            root_box_id = last_found_box_entry->box_entry_id;
            pcur_box_type++;
            box = box->first_child_box;
        } 
        else {
            //
            // "Wind up" the list to get our new search path.
            for (cur_parent = box->parent_box; cur_parent != NULL; cur_parent = cur_parent->parent_box) {
                pcur_box_type += (4 + 1);
            }

            //
            // Move to the next box.
            box = box->next_sibling_box;

            if (box != NULL) {
                ISOM_DEEP_DEBUG_PRINT(("isom_find_box_entry - ..Starting search at [%03lu] '%c%c%c%c'.  Search path is \"%s\"\n",
                    box->box_entry_id,
                    (char)((box->box_type & 0xff000000) >> 24),
                    (char)((box->box_type & 0x00ff0000) >> 16),
                    (char)((box->box_type & 0x0000ff00) >> 8),
                    (char)((box->box_type & 0x000000ff)),
                    pcur_box_type));
            }
        }
    } 
    else {
        //
        // Start at the head..
        box = mp4file_info->box_entry;
    }

    //
    // Recurse through our table of contents until we find this path.
    while (box != NULL) { // already initialized by the above
        ISOM_DEEP_DEBUG_PRINT(("isom_find_box_entry - ..Comparing against [%03lu] '%c%c%c%c'\n",
            box->box_entry_id,
            (char)((box->box_type & 0xff000000) >> 24),
            (char)((box->box_type & 0x00ff0000) >> 16),
            (char)((box->box_type & 0x0000ff00) >> 8),
            (char)((box->box_type & 0x000000ff))));

        //
        // Is this a match?
        if (memcmp(&box->big_endian_box_type, pcur_box_type, 4) == 0) {
            //
            // Skip to the delimiter.
            pcur_box_type += 4;

            ISOM_DEEP_DEBUG_PRINT(("isom_find_box_entry - ....Found match for '%c%c%c%c'; search path is \"%s\"\n",
                (char)((box->box_type & 0xff000000) >> 24),
                (char)((box->box_type & 0x00ff0000) >> 16),
                (char)((box->box_type & 0x0000ff00) >> 8),
                (char)((box->box_type & 0x000000ff)),
                pcur_box_type));

            //
            // Did we finish matching?
            if (*pcur_box_type == '\0') {
                //
                // Here's the box.
                ISOM_DEEP_DEBUG_PRINT(("isom_find_box_entry - ..Matched box path.\n"));
                if( box_entry != NULL )
                    *box_entry = box;

                return ISOM_OK;
            }

            //
            // Not done yet; descend.
            pcur_box_type++;
            box = box->first_child_box;
            continue;
        }

        //
        // If there is no next box, but we have a parent, then move up and
        // continue the search. (this is necessary if A) the file is wacky,
        // or B) we were given a last_found_box_entry)  Do not, however, leave
        // the realm of the root_box_id (if we have one).
        while ((box->next_sibling_box == NULL) && (box->parent_box != NULL)) {
            //
            // Do not leave the realm of the root_box_id (if we have one).
            if (root_box_id && (root_box_id == box->parent_box->box_entry_id)) {
                ISOM_DEEP_DEBUG_PRINT(("isom_find_box_entry - ....Hit root_box_id; aborting ascension.\n"));
                break;
            }

            //
            // Move up.
            pcur_box_type -= (4 + 1);
            box = box->parent_box;

            ISOM_DEEP_DEBUG_PRINT(("isom_find_box_entry - ....Failed match; ascending to parent.  Search path is \"%s\".\n", pcur_box_type));
        }

        //
        // No match; keep going.
        box = box->next_sibling_box;
    }

    //
    // Couldn't find a match..
    ISOM_DEEP_DEBUG_PRINT(("isom_find_box_entry - ..Match failed.\n"));
    return ISOM_ERROR;
}

/*
* Desc : track 에서 box의 속성값 저장 
*/
rs_status_t isom_init_trak_box(isom_file_info_t *mp4file_info, isom_box_entry_t *box_entry, isom_box_trak_t **box_trak)
{
    isom_box_trak_t  *temp_box_trak;
    isom_box_entry_t *dest_box_entry;
    isom_box_entry_t  source_box_entry; 
    UInt16            offset_size = 0;    //stco
    Bool16            stco_found = false; //stco
    rs_status_t       result_code = ISOM_UNKNOWN;

    temp_box_trak = (isom_box_trak_t *) malloc(sizeof(isom_box_trak_t));
    if (temp_box_trak == NULL) {
        return ISOM_MEMORY_ERROR;
    }
    memset(temp_box_trak, 0x00, sizeof(isom_box_trak_t));
    *box_trak = temp_box_trak;

    memcpy(&source_box_entry, box_entry, sizeof(isom_box_entry_t));  // 외부에서 box_entry값을 계속 사용, 함수에서는 copy 해서 사용 

    if ((result_code = isom_find_box_entry(mp4file_info, ":tkhd", &dest_box_entry, &source_box_entry)) != ISOM_OK) {
        return ISOM_FILE_NOT_FOUND_TKHD_BOX;
    }
    if ((result_code = isom_init_tkhd_box(mp4file_info, dest_box_entry, &(temp_box_trak->box_tkhd))) != ISOM_OK) {
        return result_code;
    }

    if ((result_code = isom_find_box_entry(mp4file_info, ":mdia", &dest_box_entry, &source_box_entry)) != ISOM_OK) {
        return ISOM_FILE_NOT_FOUND_MDIA_BOX;
    }
    if ((result_code = isom_init_mdia_box(mp4file_info, dest_box_entry, &(temp_box_trak->box_mdia))) != ISOM_OK) {
        return result_code;
    }
    
    if ((result_code = isom_find_box_entry(mp4file_info,":mdia:mdhd", &dest_box_entry, &source_box_entry)) != ISOM_OK) {
        return ISOM_FILE_NOT_FOUND_MDHD_BOX;
    }
    if ((result_code = isom_init_mdhd_box(mp4file_info, dest_box_entry, &(temp_box_trak->box_mdia->box_mdhd)))!= ISOM_OK) {
        return result_code;
    }
    
    if ((result_code = isom_find_box_entry(mp4file_info,":mdia:hdlr", &dest_box_entry, &source_box_entry)) != ISOM_OK) {
        return ISOM_FILE_NOT_FOUND_HDLR_BOX;
    }
    if ((result_code = isom_init_hdlr_box(mp4file_info, dest_box_entry, &(temp_box_trak->box_mdia->box_hdlr))) != ISOM_OK) {
        return result_code;
    }

    switch (temp_box_trak->box_mdia->box_hdlr->handler_type) { 
        case ISOM_TRACK_VISUAL: //video
        case ISOM_TRACK_AUDIO: //sound
            break;
        default:
        {
            return ISOM_OK;
        }
    }
    
    if ((result_code = isom_find_box_entry(mp4file_info, ":mdia:minf", &dest_box_entry, &source_box_entry)) != ISOM_OK) {
        return ISOM_FILE_NOT_FOUND_MINF_BOX;
    }
    if ((result_code = isom_init_minf_box(mp4file_info, dest_box_entry, &(temp_box_trak->box_mdia->box_minf))) != ISOM_OK) {
        return result_code;
    }

    if (isom_find_box_entry(mp4file_info, ":mdia:minf:dinf", &dest_box_entry, &source_box_entry) == ISOM_OK) {
        if ((result_code = isom_init_dinf_box(mp4file_info, dest_box_entry, &(temp_box_trak->box_mdia->box_minf->box_dinf))) == ISOM_OK) {

            if ((result_code = isom_find_box_entry(mp4file_info, ":mdia:minf:dinf:dref", &dest_box_entry, &source_box_entry)) == ISOM_OK) {
                if ((result_code = isom_init_dref_box(mp4file_info, dest_box_entry, &(temp_box_trak->box_mdia->box_minf->box_dinf->box_dref))) != ISOM_OK) {
                    return result_code;
                }
            }
        }
        else {
            return result_code;
        }
    }

    if ((result_code = isom_find_box_entry(mp4file_info, ":mdia:minf:stbl", &dest_box_entry, &source_box_entry)) != ISOM_OK) {
        return ISOM_FILE_NOT_FOUND_STBL_BOX;
    }
    if ((result_code = isom_init_stbl_box(mp4file_info, dest_box_entry, &(temp_box_trak->box_mdia->box_minf->box_stbl))) != ISOM_OK) {
        return result_code;
    }

    //find stts
    if ((result_code = isom_find_box_entry(mp4file_info,":mdia:minf:stbl:stts", &dest_box_entry, &source_box_entry)) != ISOM_OK) {
        return ISOM_FILE_NOT_FOUND_STISOM_BOX;
    }

    if ((result_code = isom_init_stts_box(mp4file_info, dest_box_entry, &(temp_box_trak->box_mdia->box_minf->box_stbl->box_stts))) != ISOM_OK) {
        return result_code;
    }
    else {
        if (temp_box_trak->box_mdia->box_minf->box_stbl->box_stts == NULL) {
            return ISOM_FILE_INVALID_STISOM_BOX;
        }
    }
   
    //find ctts
    if ((result_code = isom_find_box_entry(mp4file_info,":mdia:minf:stbl:ctts", &dest_box_entry, &source_box_entry)) == ISOM_OK) {
        
        if ((result_code = isom_init_ctts_box(mp4file_info, dest_box_entry, &(temp_box_trak->box_mdia->box_minf->box_stbl->box_ctts))) != ISOM_OK) { 
            return result_code;
        }
        else {
            if (temp_box_trak->box_mdia->box_minf->box_stbl->box_ctts == NULL) {
                return ISOM_FILE_INVALID_CTISOM_BOX;
            }
        }
    }

    //find stsc
    if ((result_code = isom_find_box_entry(mp4file_info, ":mdia:minf:stbl:stsc", &dest_box_entry, &source_box_entry)) != ISOM_OK) {
        return ISOM_FILE_NOT_FOUND_STSC_BOX;
    }
    if ((result_code = isom_init_stsc_box(mp4file_info, dest_box_entry, &(temp_box_trak->box_mdia->box_minf->box_stbl->box_stsc))) != ISOM_OK) {
        return result_code;
    } 
    else {
        if (temp_box_trak->box_mdia->box_minf->box_stbl->box_stsc == NULL) {
            return ISOM_FILE_INVALID_STSC_BOX;
        }
    }

     //find stco
    if (isom_find_box_entry(mp4file_info,":mdia:minf:stbl:stco", &dest_box_entry, &source_box_entry) == ISOM_OK) {
        stco_found = true;
        offset_size = 4;
    }
    else if (isom_find_box_entry(mp4file_info,":mdia:minf:stbl:co64", &dest_box_entry, &source_box_entry) == ISOM_OK) {
        stco_found = true;
        offset_size = 8;
    }
    if (!stco_found) {
        return ISOM_FILE_NOT_FOUND_STCO_BOX;
    }

    if ((result_code = isom_init_stco_box(mp4file_info, dest_box_entry, &(temp_box_trak->box_mdia->box_minf->box_stbl->box_stco), offset_size)) != ISOM_OK) {
        return result_code;                         
    }
    else {
        if (temp_box_trak->box_mdia->box_minf->box_stbl->box_stco == NULL) {
            return ISOM_FILE_INVALID_STCO_BOX;
        }
    }

    //find stsz
    if ((result_code = isom_find_box_entry(mp4file_info,":mdia:minf:stbl:stsz", &dest_box_entry, &source_box_entry)) != ISOM_OK) {
        return ISOM_FILE_NOT_FOUND_STSZ_BOX;
    }
    if ((result_code = isom_init_stsz_box(mp4file_info, dest_box_entry, &(temp_box_trak->box_mdia->box_minf->box_stbl->box_stsz))) != ISOM_OK) {
        return result_code;
    }
    else {
        if (temp_box_trak->box_mdia->box_minf->box_stbl->box_stsz == NULL) {
            return ISOM_FILE_INVALID_STSZ_BOX;
        }
    }

    //find stss
    if ((result_code = isom_find_box_entry(mp4file_info,":mdia:minf:stbl:stss", &dest_box_entry, &source_box_entry)) == ISOM_OK) {

        if ((result_code = isom_init_stss_box(mp4file_info, dest_box_entry, &(temp_box_trak->box_mdia->box_minf->box_stbl->box_stss))) != ISOM_OK) {
            return result_code;
        }
        else {
            if (temp_box_trak->box_mdia->box_minf->box_stbl->box_stss == NULL) {
                return ISOM_FILE_INVALID_STSS_BOX;
            }
        }
    }
    else {
        temp_box_trak->box_mdia->box_minf->box_stbl->box_stss = NULL;
    }

    //find stsd
    if ((result_code = isom_find_box_entry(mp4file_info,":mdia:minf:stbl:stsd", &dest_box_entry, &source_box_entry)) != ISOM_OK) {
        return ISOM_FILE_NOT_FOUND_STSD_BOX;
    }
    if ((result_code = isom_init_stsd_box(mp4file_info, dest_box_entry, &(temp_box_trak->box_mdia->box_minf->box_stbl->box_stsd))) != ISOM_OK) {
        return result_code;
    }
    else {
        if (temp_box_trak->box_mdia->box_minf->box_stbl->box_stsd == NULL) {
            return ISOM_FILE_INVALID_STSD_BOX;
        }
    }
   
    return ISOM_OK;
}

/*
* Desc : isom_box_trak_entry_t 메모리 해제  
*/
Bool16 isom_delete_trak_box(isom_file_info_t *mp4file_info)
{
    isom_box_trak_entry_t    *box_trak_entry;
    isom_box_trak_entry_t    *next_box_trak_entry;

    box_trak_entry      = mp4file_info->box_moov->trak_list_head;
    next_box_trak_entry = box_trak_entry ? box_trak_entry->next_box_trak_entry : NULL;
    while (box_trak_entry != NULL) {

        if (box_trak_entry->box_trak != NULL) {
            isom_box_trak_t *temp_box_trak = box_trak_entry->box_trak;
            
            if (temp_box_trak->box_tkhd) {
                free(temp_box_trak->box_tkhd);
            }
            
            if (temp_box_trak->box_mdia) {
                if (temp_box_trak->box_mdia->box_hdlr) {
                    if (temp_box_trak->box_mdia->box_hdlr->name_utf8) {
                        free(temp_box_trak->box_mdia->box_hdlr->name_utf8);
                    }
                    free(temp_box_trak->box_mdia->box_hdlr);
                }
                
                if (temp_box_trak->box_mdia->box_mdhd) {
                    free(temp_box_trak->box_mdia->box_mdhd);
                }   

                if (temp_box_trak->box_mdia->box_minf) {
                    if (temp_box_trak->box_mdia->box_minf->box_dinf) {
                        if (temp_box_trak->box_mdia->box_minf->box_dinf->box_dref) {
                            free(temp_box_trak->box_mdia->box_minf->box_dinf->box_dref);
                        }
                        free(temp_box_trak->box_mdia->box_minf->box_dinf);
                    }

                    if (temp_box_trak->box_mdia->box_minf->box_stbl) {
                        if (temp_box_trak->box_mdia->box_minf->box_stbl->box_stts) {
                            if (temp_box_trak->box_mdia->box_minf->box_stbl->box_stts->time_to_sample_table) {
                                free(temp_box_trak->box_mdia->box_minf->box_stbl->box_stts->time_to_sample_table);
                            }
                            
                            free(temp_box_trak->box_mdia->box_minf->box_stbl->box_stts);
                        }

                        if (temp_box_trak->box_mdia->box_minf->box_stbl->box_ctts) {
                            if (temp_box_trak->box_mdia->box_minf->box_stbl->box_ctts->time_to_sample_table) {
                                free(temp_box_trak->box_mdia->box_minf->box_stbl->box_ctts->time_to_sample_table);
                            }
                            
                            free(temp_box_trak->box_mdia->box_minf->box_stbl->box_ctts);
                        }
                        
                        if (temp_box_trak->box_mdia->box_minf->box_stbl->box_stsc) {
                            if (temp_box_trak->box_mdia->box_minf->box_stbl->box_stsc->sample_to_chunk_table) {
                                free(temp_box_trak->box_mdia->box_minf->box_stbl->box_stsc->sample_to_chunk_table);
                            }
                            
                            free(temp_box_trak->box_mdia->box_minf->box_stbl->box_stsc);
                        }
                        
                        if (temp_box_trak->box_mdia->box_minf->box_stbl->box_stsd) {
                            if (temp_box_trak->box_mdia->box_minf->box_stbl->box_stsd->sample_description_table) {
                                free(temp_box_trak->box_mdia->box_minf->box_stbl->box_stsd->sample_description_table);
                            }
                            
                            isom_delete_stsd_box(temp_box_trak->box_mdia->box_minf->box_stbl->box_stsd);
                        }
                        
                        if (temp_box_trak->box_mdia->box_minf->box_stbl->box_stco) {
                            if (temp_box_trak->box_mdia->box_minf->box_stbl->box_stco->chunk_offset_table) {
                                free(temp_box_trak->box_mdia->box_minf->box_stbl->box_stco->chunk_offset_table);
                            }
                            
                            free(temp_box_trak->box_mdia->box_minf->box_stbl->box_stco);
                        }
                        
                        if (temp_box_trak->box_mdia->box_minf->box_stbl->box_stsz) {
                            if (temp_box_trak->box_mdia->box_minf->box_stbl->box_stsz->sample_size_table) {
                                free(temp_box_trak->box_mdia->box_minf->box_stbl->box_stsz->sample_size_table);
                            }
                            
                            free(temp_box_trak->box_mdia->box_minf->box_stbl->box_stsz);
                        }
                        
                        if (temp_box_trak->box_mdia->box_minf->box_stbl->box_stss) {
                            if (temp_box_trak->box_mdia->box_minf->box_stbl->box_stss->sync_sample_table) {
                                free(temp_box_trak->box_mdia->box_minf->box_stbl->box_stss->sync_sample_table);
                            }
                            
                            free(temp_box_trak->box_mdia->box_minf->box_stbl->box_stss);
                        }

                        free(temp_box_trak->box_mdia->box_minf->box_stbl);
                    }
                    
                    free(temp_box_trak->box_mdia->box_minf);
                }

                free(temp_box_trak->box_mdia);
            }
            
            free(box_trak_entry->box_trak);
        }

        free(box_trak_entry);
        box_trak_entry = next_box_trak_entry;

        if (box_trak_entry != NULL) {
            next_box_trak_entry = box_trak_entry->next_box_trak_entry;
        }
    }
    if (mp4file_info->box_moov->trak_list_entry) {
        free(mp4file_info->box_moov->trak_list_entry);
    }
    return true;
}

/*
* Desc : isom_track_entry에서  track 얻을때 사용 
*/
rs_status_t isom_next_trak_box(isom_file_info_t *mp4file_info, isom_box_trak_t **box_trak, isom_box_trak_t *lastfound_box_trak)
{
    isom_box_trak_entry_t      *box_trak_entry;

    //
    // Return the first track if requested.
    if (lastfound_box_trak == NULL) {
        if (mp4file_info->box_moov->trak_list_head != NULL) {
            *box_trak = mp4file_info->box_moov->trak_list_head->box_trak;
            return ISOM_OK;
        }
        else {
            return ISOM_DECLINE;
        }
    }
    
    //
    // Find LastTrack and return the one after it.
    for (box_trak_entry = mp4file_info->box_moov->trak_list_head; box_trak_entry != NULL; box_trak_entry = box_trak_entry->next_box_trak_entry) {
        //
        // Check for matches.
        if (box_trak_entry->box_trak_entry_id == lastfound_box_trak->box_tkhd->track_id) {
            //
            // Is there a next track?
            if( box_trak_entry->next_box_trak_entry != NULL ) {
                *box_trak = box_trak_entry->next_box_trak_entry->box_trak;
                return ISOM_OK;
            } else {
                return ISOM_DECLINE;
            }
        }
    }
    
    return ISOM_DECLINE;
}

/*
* Desc : track_id로 track을 얻음  
*/
Bool16 isom_find_track(isom_file_info_t *mp4file_info, UInt32 track_id, isom_box_trak_t **box_trak)
{
    isom_box_trak_entry_t      *box_trak_entry;    
    
    //
    // Find the specified track.
    for( box_trak_entry = mp4file_info->box_moov->trak_list_head; box_trak_entry != NULL; box_trak_entry = box_trak_entry->next_box_trak_entry ) {
        //
        // Check for matches.
        if (box_trak_entry->box_trak_entry_id == track_id) {
            *box_trak = box_trak_entry->box_trak;
            return true;
        }
    }
    
    //
    // The search failed.
    return false;
}