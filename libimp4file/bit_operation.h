#ifndef BIT_OPERATION_H
#define BIT_OPERATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "os_header.h"

enum
{
    ISOM_BITOPER_READ = 0,
    ISOM_BITOPER_WRITE,
};

/*the default size for new streams allocation...*/
#define BO_MEM_BLOCK_ALLOC_SIZE		250

/*private types*/
enum
{
    ISOM_BITOPER_FILE_READ = ISOM_BITOPER_WRITE + 1,
    ISOM_BITOPER_FILE_WRITE,
    /*private mode if we own the buffer*/
    ISOM_BITOPER_WRITE_DYN
};

typedef struct isom_bit_operation_t isom_bit_operation_t;

struct isom_bit_operation_t
{
    /*original stream data*/
    FILE *stream;    
    /*original data*/
    char *original;
    /*the size of our buffer*/
    UInt64 size;
    /*current position in BYTES*/
    UInt64 position;
    /*the byte readen/written */
    UInt32 current;
    /*the number of bits in the current byte*/
    UInt32 nbBits;
    /*the bitstream mode*/
    UInt32 bsmode;
    
    void (*EndOfStream)(void *par);
    void *par;
};

isom_bit_operation_t *isom_init_bit_operation(char *buffer, UInt64 BufferSize, UInt32 mode);

void isom_delete_bit_operation(isom_bit_operation_t *bs);

UInt32 isom_write_bit_operation_data(isom_bit_operation_t *bs, char *data, UInt32 nb_bytes);

void isom_write_bit_operation_long_int(isom_bit_operation_t *bo, SInt64 value, SInt32 nBits);

void isom_write_bit_operation_byte(isom_bit_operation_t *bo, UInt8 val);

void isom_write_bit_operation_bit(isom_bit_operation_t *bo, UInt32 bit);

void isom_write_bit_operation_int(isom_bit_operation_t *bo, SInt32 value, SInt32 nbits);

void isom_write_byte(isom_bit_operation_t *bo, UInt8 val);

void isom_write_bit_operation_u32(isom_bit_operation_t *bo, UInt32 value);

void isom_write_bit_operation_u24(isom_bit_operation_t *bo, UInt32 value);

void isom_write_bit_operation_u16(isom_bit_operation_t *bo, UInt32 value);

void isom_write_bit_operation_u8(isom_bit_operation_t *bo, UInt32 value);

void isom_write_bit_operation_u64(isom_bit_operation_t *bo, UInt64 value);

Bool16 isom_is_align_bit_operation(isom_bit_operation_t *bs);

void isom_get_bit_operation_content(isom_bit_operation_t *bo, char **output, UInt32 *outSize);

UInt32 isom_cut_bit_operation_buffer(isom_bit_operation_t *bs);

UInt8 isom_align_bit_operation(isom_bit_operation_t *bs);

UInt32 isom_get_bit_operation_bit_size(UInt32 max_val);

UInt32 isom_read_bit_operation_int(isom_bit_operation_t *bo, UInt32 nBits);

UInt8 isom_read_bit_operation_bit(isom_bit_operation_t *bo);

UInt32 isom_read_bit_operation_u8(isom_bit_operation_t *bs);

UInt8 isom_read_bit_operation_byte(isom_bit_operation_t *bo);

UInt64 isom_available_bit_operation(isom_bit_operation_t *bo);

UInt64 isom_get_bit_operation_position(isom_bit_operation_t *bo);

UInt32 isom_read_bit_operation_data(isom_bit_operation_t *bo, char *data, UInt32 nbBytes);

Bool16 isom_seek_bit_operation(isom_bit_operation_t *bo, UInt64 offset);

Bool16 isom_seek_bit_operation_intern(isom_bit_operation_t *bo, UInt64 offset);

#ifdef __cplusplus
}
#endif

#endif
