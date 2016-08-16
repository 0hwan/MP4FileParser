
#include "data_resource.h"

/*
* Desc : 메모리 할당 초기화(file open)
*/
rs_status_t isom_init_resource_file(const char *inpath, isom_resource_t **data_resource)
{
    isom_resource_file_t *file_resource;
    isom_resource_t *resource;

    resource = (isom_resource_t *) malloc(sizeof(isom_resource_t));
    if (resource == NULL) {
        return ISOM_MEMORY_ERROR;
    }
    memset(resource, 0x00, sizeof(isom_resource_t)); 

    *data_resource = resource;
    
    file_resource = (isom_resource_file_t *) malloc(sizeof(isom_resource_file_t));
    if (file_resource == NULL) {
        return ISOM_MEMORY_ERROR;
    }
    memset(file_resource, 0x00, sizeof(isom_resource_file_t)); 

    resource->res_file = file_resource;
    resource->type = ISOM_RESOURCE_FILE_TYPE;
    
#if WIN32
    file_resource->file_des = open(inpath, O_RDONLY | O_BINARY);
#elif LINUX
    file_resource->file_des = open(inpath, O_RDONLY | O_LARGEFILE);
#else
    file_resource->file_des = open(inpath, O_RDONLY);
#endif
    if (file_resource->file_des == -1) {
        return ISOM_FILE_OPEN_ERROR;
    }   
    
    return ISOM_OK;
}

/*
* Desc : data resource delete
*/
void isom_delete_resource(isom_resource_t *data_resource)
{
    if (data_resource) {
        if (data_resource->type == ISOM_RESOURCE_FILE_TYPE && data_resource->res_file) {
            if (data_resource->res_file->file_des != -1) {
                close(data_resource->res_file->file_des);
            }
            free(data_resource->res_file);
            data_resource->res_file = NULL;
        }
        else if (data_resource->type == ISOM_RESOURCE_MEM_TYPE && data_resource->res_mem) {
            free(data_resource->res_mem);
            data_resource->res_mem = NULL;
        }

        free(data_resource);
        data_resource = NULL;
    }
}

/*
* Desc : ut_ntoh64 Sint64 networkorder->hostorder 
*/
SInt64 ut_ntoh64(SInt64 network_ordered)
{
#if BIGENDIAN
    return network_ordered;
#else
    return (SInt64) (  (UInt64)  (network_ordered << 56) | (UInt64)  (((UInt64) 0x00ff0000 << 32) & (network_ordered << 40))
        | (UInt64)  ( ((UInt64)  0x0000ff00 << 32) & (network_ordered << 24)) | (UInt64)  (((UInt64)  0x000000ff << 32) & (network_ordered << 8))
        | (UInt64)  ( ((UInt64)  0x00ff0000 << 8) & (network_ordered >> 8)) | (UInt64)     ((UInt64)  0x00ff0000 & (network_ordered >> 24))
        | (UInt64)  (  (UInt64)  0x0000ff00 & (network_ordered >> 40)) | (UInt64)  ((UInt64)  0x00ff & (network_ordered >> 56)) );
#endif
}


/*
* Desc : memory source init
*/
rs_status_t isom_init_resource_mem(isom_resource_t **data_resource, char *memory_data, UInt32 memory_size)
{
    isom_resource_t       *resource;
    isom_resource_mem_t   *memory_resource;

    resource = (isom_resource_t *) malloc(sizeof(isom_resource_t));
    if (resource == NULL) {
        return ISOM_MEMORY_ERROR;
    }
    memset(resource, 0x00, sizeof(isom_resource_t));  
    
    *data_resource = resource;

    memory_resource = (isom_resource_mem_t *) malloc(sizeof(isom_resource_mem_t));
    if (memory_resource == NULL) {
        return ISOM_MEMORY_ERROR;
    }
    memset(memory_resource, 0x00, sizeof(isom_resource_mem_t));  
    resource->res_mem = memory_resource;    
    resource->type = ISOM_RESOURCE_MEM_TYPE;

    memory_resource->mem_data = memory_data;
    memory_resource->mem_size = memory_size;

    return ISOM_OK;
}

/*
* Desc : memory source delete
*/
void isom_delete_resource_mem(isom_resource_t *data_resource)
{
    if (data_resource) {
        if (data_resource->type == ISOM_RESOURCE_MEM_TYPE && data_resource->res_mem) {
            free(data_resource->res_mem);
            data_resource->res_mem = NULL;
        }
        free(data_resource);
        data_resource = NULL;
    }
}

/*
* Desc : data seek and read  form memory  
*/
rs_status_t isom_read_resource_mem(isom_resource_t *data_resource, void *destination_memory, UInt32 destination_length)
{
    char    *source_memory;
    source_memory = data_resource->res_mem->mem_data + data_resource->res_mem->mem_pos;
    
    if ((data_resource->res_mem->mem_pos + destination_length) > data_resource->res_mem->mem_size) {
        return ISOM_MEMORY_READ_SIZE_MISMATCH;
    }

    memcpy(destination_memory, source_memory, destination_length);
    data_resource->res_mem->mem_pos += destination_length;

    return ISOM_OK;
}

/*
* Desc : data seek and read  form disk  
*/
rs_status_t isom_read_resource_file(isom_resource_t *data_resource, void *destination_memory, UInt32 destination_length, UInt32 *result_length)
{
    int     rcvlen;
    int     fd = data_resource->res_file->file_des;

#if WIN32
    if (_lseeki64(fd, data_resource->res_file->file_pos, SEEK_SET) == -1) {
        return ISOM_FILE_IO_ERROR;
    }
#else
    if (lseek(fd, data_resource->res_file->file_pos, SEEK_SET) == -1) {
		return ISOM_FILE_IO_ERROR;
    }
#endif
        
    rcvlen = read(fd, (char*)destination_memory, destination_length);

    if (rcvlen == -1) {
        return ISOM_FILE_IO_ERROR;
    }

    if (result_length != NULL) {
        *result_length = rcvlen;
    }

    data_resource->res_file->file_pos += rcvlen;
    
    return ISOM_OK;
}

/*
* Desc : data_source read
*/
rs_status_t isom_read_resource(isom_resource_t *data_resource, UInt64 in_position, char * const destination_data, UInt32 length)
{
    UInt32      result_length = 0;
    rs_status_t   result_code   = ISOM_UNKNOWN;

    if (data_resource->type == ISOM_RESOURCE_FILE_TYPE) {
        data_resource->res_file->file_pos = in_position;
        if ((result_code = isom_read_resource_file(data_resource, destination_data, length, &result_length)) != ISOM_OK) {
            return result_code;
        }
        if (result_length != length) {
            return ISOM_FILE_READ_SIZE_MISMATCH;
        }
    }
    else if (data_resource->type == ISOM_RESOURCE_MEM_TYPE) {
        data_resource->res_mem->mem_pos = in_position;
        if ((result_code = isom_read_resource_mem(data_resource, destination_data, length)) != ISOM_OK) {
            return result_code;
        }
    }
    else {
        return ISOM_DATA_RESOURCE_BAD_PARAM;
    }
    return ISOM_OK;
}

/*
* Desc : length 길이만큼 read  
*/
rs_status_t isom_read_resource_bytes(isom_resource_t *data_resource, UInt64 offset, char *data, UInt32 length, isom_box_entry_t *box_entry)
{
    //
    // Validate the arguments.
    if (box_entry == NULL) {
        return isom_read_resource(data_resource, offset, data, length);
    }
    else{
        if ((offset + length) > box_entry->box_data_size) {
            return ISOM_FILE_BOX_SIZE_MISMATCH;
        }
    }
    
    //
    // Read and return this data.
    return isom_read_resource(data_resource, box_entry->box_data_pos + offset, data, length);
}

/*
* Desc : 4bytes read
*/
rs_status_t isom_read_resource_uint32(isom_resource_t *data_source, UInt64 offset, UInt32 *datum, isom_box_entry_t *box_entry)
{
    // General vars
    UInt32      tempdatum;
    rs_status_t   result_code = ISOM_UNKNOWN;
    //
    // Read and flip.
    if ((result_code = isom_read_resource_bytes(data_source, offset, (char *)&tempdatum, 4, box_entry)) != ISOM_OK) {
        return result_code;    
    }

    *datum = ntohl(tempdatum);
    return ISOM_OK;
}

/*
* Desc : 4bytes read 후 UInt64에  값 저장 
*/
rs_status_t isom_read_resource_uint32_to_uint64(isom_resource_t *data_source, UInt64 offset, UInt64 *datum, isom_box_entry_t *box_entry)
{
    UInt32      tempdatum;
    rs_status_t   result_code = ISOM_UNKNOWN;
    //
    // Read and flip.
    if ((result_code = isom_read_resource_bytes(data_source, offset, (char *)&tempdatum, 4, box_entry)) != ISOM_OK) {
        return result_code;
    }
    
    tempdatum =  ntohl(tempdatum);
    *datum = (UInt64) tempdatum;
    return ISOM_OK;
}

/*
* Desc : 2bytes read
*/
rs_status_t isom_read_resource_uint16(isom_resource_t *data_source, UInt64 offset, UInt16 *datum, isom_box_entry_t *box_entry)
{
    // General vars
    UInt16      tempdatum;
    rs_status_t   result_code = ISOM_UNKNOWN;
    //
    // Read and flip.
    if ((result_code = isom_read_resource_bytes(data_source, offset, (char *)&tempdatum, 2, box_entry)) != ISOM_OK)
        return result_code;
    
    *datum = ntohs(tempdatum);
    return ISOM_OK;
}

/*
* Desc : 1bytes read
*/
rs_status_t isom_read_resource_uint8(isom_resource_t *data_source, UInt64 offset, UInt8 *datum, isom_box_entry_t *box_entry)
{
    rs_status_t   result_code = ISOM_UNKNOWN;
    //
    // Read and flip.
    if ((result_code = isom_read_resource_bytes(data_source, offset, (char *)datum, 1, box_entry)) != ISOM_OK){
        return result_code;
    }
    
    return ISOM_OK;
}

/*
* Desc : 8bytes read
*/
rs_status_t isom_read_resource_uint64(isom_resource_t *data_source, UInt64 offset, UInt64 *datum, isom_box_entry_t *box_entry)
{
    // General vars
    UInt64      tempdatum;
    rs_status_t   result_code = ISOM_UNKNOWN;
    //
    // Read and flip.
    if ((result_code = isom_read_resource_bytes(data_source, offset, (char *)&tempdatum, 8, box_entry)) != ISOM_OK) {
        return result_code;
    }
    
    *datum = (UInt64) ut_ntoh64(tempdatum);
    return ISOM_OK;
}