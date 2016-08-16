#ifndef DATA_RESOURCE_H
#define DATA_RESOURCE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "os_header.h"

// -------------------------------------
// Macros
//
#define DLOG

#ifdef DLOG
#define ISOM_DEEP_DEBUG_PRINT(s)
#define ISOM_DEBUG_PRINT(s) printf s
#else
#define ISOM_DEEP_DEBUG_PRINT(s) 
#define ISOM_DEBUG_PRINT(s)
#endif    

#define SEXTENDED_TYPE_SIZE       16
#define ISOM_RESOURCE_FILE_TYPE   1
#define ISOM_RESOURCE_MEM_TYPE    2


typedef struct isom_resource_mem_t  isom_resource_mem_t;
typedef struct isom_resource_file_t isom_resource_file_t;
typedef struct isom_resource_t      isom_resource_t;
typedef struct isom_box_entry_t     isom_box_entry_t;


struct isom_resource_mem_t{
    UInt64	 mem_pos;		
    UInt32	 mem_size;
    char	*mem_data;
};

struct isom_resource_file_t{
    UInt64			file_pos;
    int				file_des;
};

struct isom_resource_t{
    UInt32              type;		// 1: file or 2: memory
    /* file mode */
    isom_resource_file_t *res_file;
	/* memory mode */
    isom_resource_mem_t  *res_mem;
};

struct isom_box_entry_t {
    // Box Entry id (used to compare Box Entrys)
    UInt32           box_entry_id;
    
    // Box information
    OSType           box_type;
	OSType           big_endian_box_type;		// be = Big Endian
    
    UInt64           box_data_pos;
    UInt64           box_data_size;
    UInt32           box_header_size;
    
    // box_entry pointers
    isom_box_entry_t  *next_order_box;			// box entry id 순서상의 next box

    isom_box_entry_t  *prev_sibling_box;
	isom_box_entry_t  *next_sibling_box;			// tree 상에서 sibling 중에서의 next box

    isom_box_entry_t  *parent_box;
	isom_box_entry_t  *first_child_box;
};


SInt64 ut_ntoh64(SInt64 network_ordered);

rs_status_t isom_read_resource(isom_resource_t *data_resource, UInt64 in_position, char * const destination_data, UInt32 length);

rs_status_t isom_init_resource_file(const char *inpath, isom_resource_t **data_resource);

void isom_delete_resource(isom_resource_t *data_resource);

rs_status_t isom_read_resource_mem(isom_resource_t *data_resource, void *destination_memory, UInt32 destination_length);

rs_status_t isom_read_resource_file(isom_resource_t *data_resource, void *destination_memory, UInt32 destination_length, UInt32 *result_length);

rs_status_t isom_init_resource_mem(isom_resource_t **data_resource, char *memory_data, UInt32 memory_size);

void isom_delete_resource_mem(isom_resource_t *data_resource);

rs_status_t isom_read_resource_uint32(isom_resource_t *data_resource, UInt64 offset, UInt32 *datum, isom_box_entry_t *box_entry);

rs_status_t isom_read_resource_uint32_to_uint64(isom_resource_t *data_resource, UInt64 offset, UInt64 *datum, isom_box_entry_t *box_entry);

rs_status_t isom_read_resource_uint16(isom_resource_t *data_resource, UInt64 offset, UInt16 *datum, isom_box_entry_t *box_entry);

rs_status_t isom_read_resource_uint8(isom_resource_t *data_resource, UInt64 offset, UInt8 *datum, isom_box_entry_t *box_entry);

rs_status_t isom_read_resource_uint64(isom_resource_t *data_resource, UInt64 offset, UInt64 *datum, isom_box_entry_t *box_entry);

rs_status_t isom_read_resource_bytes(isom_resource_t *data_resource, UInt64 offset, char *data, UInt32 length, isom_box_entry_t *box_entry);


#ifdef __cplusplus
}
#endif

#endif