#include "bit_operation.h"


/*
*/
isom_bit_operation_t *isom_init_bit_operation(char *buffer, UInt64 BufferSize, UInt32 mode)
{
    isom_bit_operation_t *tmp;
    if ( (buffer && ! BufferSize)) return NULL;
    
    tmp = (isom_bit_operation_t *)malloc(sizeof(isom_bit_operation_t));
    if (!tmp) return NULL;
    memset(tmp, 0, sizeof(isom_bit_operation_t));
    
    tmp->original = (char*)buffer;
    tmp->size = BufferSize;
    
    tmp->position = 0;
    tmp->current = 0;
    tmp->bsmode = mode;
    tmp->stream = NULL;
    
    switch (tmp->bsmode) {
    case ISOM_BITOPER_READ:
        tmp->nbBits = 8;
        tmp->current = 0;
        break;
    case ISOM_BITOPER_WRITE:
        tmp->nbBits = 0;
        if (! buffer) {
             /* if BufferSize is specified, use it. This is typically used when AvgSize of
              *  some buffers is known, but some exceed it.
              */
            if (BufferSize) {
                tmp->size = BufferSize;
            } else {
                tmp->size = BO_MEM_BLOCK_ALLOC_SIZE;
            }
            tmp->original = (char *) malloc(sizeof(char) * ((UInt32) tmp->size));
            if (! tmp->original) {
                free(tmp);
                return NULL;
            }
            tmp->bsmode = ISOM_BITOPER_WRITE_DYN;
        } else {
            tmp->original = (char*)buffer;
            tmp->size = BufferSize;
        }
        break;
    default:
        /*the stream constructor is not the same...*/
        free(tmp);
        return NULL;
    }
    return tmp;
}

/*
*/
void isom_delete_bit_operation(isom_bit_operation_t *bo)
{
    if (!bo) {
        return;
    }
    /*if we are in dynamic mode (alloc done by the bitstream), free the buffer if still present*/
    if ((bo->bsmode == ISOM_BITOPER_WRITE_DYN) && bo->original) {
        free(bo->original);
        bo->original = NULL;
    }
    free(bo);
    bo = NULL;
}

/*
* Desc : 
*/
void isom_write_bit_operation_int(isom_bit_operation_t *bo, SInt32 value, SInt32 nbits)
{
    value <<= sizeof (SInt32) * 8 - nbits;

    while (--nbits >= 0) {
        isom_write_bit_operation_bit (bo, value < 0);
        value <<= 1;
    }
}

/*
* Desc : 
*/
void isom_write_bit_operation_u8(isom_bit_operation_t *bo, UInt32 value)
{
    isom_write_byte(bo, (UInt8) value);
}

/*
* Desc : 
*/
void isom_write_bit_operation_u16(isom_bit_operation_t *bo, UInt32 value)
{
    isom_write_byte(bo, (UInt8) ((value>>8)&0xff));
    isom_write_byte(bo, (UInt8) ((value)&0xff));
}

/*
* Desc : 
*/
void isom_write_bit_operation_u24(isom_bit_operation_t *bo, UInt32 value)
{
    isom_write_byte(bo, (UInt8) ((value>>16)&0xff));
    isom_write_byte(bo, (UInt8) ((value>>8)&0xff));
    isom_write_byte(bo, (UInt8) ((value)&0xff));
}

/*
* Desc : 
*/
void isom_write_bit_operation_u32(isom_bit_operation_t *bo, UInt32 value)
{
    isom_write_byte(bo, (UInt8) ((value>>24)&0xff));
    isom_write_byte(bo, (UInt8) ((value>>16)&0xff));
    isom_write_byte(bo, (UInt8) ((value>>8)&0xff));
    isom_write_byte(bo, (UInt8) ((value)&0xff));
}

/*
* Desc : 
*/
void isom_write_bit_operation_u64(isom_bit_operation_t *bo, UInt64 value)
{
    isom_write_bit_operation_u32(bo, (UInt32) ((value>>32)&0xffffffff));
    isom_write_bit_operation_u32(bo, (UInt32) (value&0xffffffff));
}

/*
* Desc : 
*/
void isom_write_byte(isom_bit_operation_t *bo, UInt8 val)
{
    /*we don't allow write on READ buffers*/
    if ((bo->bsmode == ISOM_BITOPER_READ) || (bo->bsmode == ISOM_BITOPER_FILE_READ)) {
        return;
    }

    if (!bo->original && !bo->stream) {
        return;
    }
    
    /*we are in MEM mode*/
    if ( (bo->bsmode == ISOM_BITOPER_WRITE) || (bo->bsmode == ISOM_BITOPER_WRITE_DYN) ) {
        if (bo->position == bo->size) {
            /*no more space...*/
            if (bo->bsmode != ISOM_BITOPER_WRITE_DYN) {
                return;
            }

            /*realloc if enough space */
            if (bo->size > 0xFFFFFFFF) {
                return;
            }

            bo->original = (char*)realloc(bo->original, (UInt32) (bo->size + BO_MEM_BLOCK_ALLOC_SIZE));
            if (!bo->original) {
                return;
            }
            bo->size += BO_MEM_BLOCK_ALLOC_SIZE;
        }
        bo->original[bo->position] = val;
        bo->position++;
        return;
    }
    /*we are in FILE mode, no pb for any realloc...*/
    fputc(val, bo->stream);
    /*check we didn't rewind the stream*/
    if (bo->size == bo->position) {
        bo->size++;
    }

    bo->position += 1;
}

/*
*/
Bool16 isom_is_align_bit_operation(isom_bit_operation_t *bo)
{
    switch (bo->bsmode) {
    case ISOM_BITOPER_READ:
    case ISOM_BITOPER_FILE_READ:
        return ( (8 == bo->nbBits) ? 1 : 0);
    default:
        return !bo->nbBits;
    }
}

/*
* Desc : 
*/
UInt32 isom_write_bit_operation_data(isom_bit_operation_t *bo, char *data, UInt32 nb_bytes)
{
    /*we need some feedback for this guy...*/
    UInt64 begin = bo->position;
    if (!nb_bytes) {
        return 0;
    }
    
    if (isom_is_align_bit_operation(bo)) {
        switch (bo->bsmode) {
        case ISOM_BITOPER_WRITE:
            if (bo->position+nb_bytes > bo->size) {
                return 0;
            }

            memcpy(bo->original + bo->position, data, nb_bytes);
            bo->position += nb_bytes;
            return nb_bytes;
        case ISOM_BITOPER_WRITE_DYN:
            /*need to realloc ...*/
            if (bo->position+nb_bytes > bo->size) {
                if (bo->size + nb_bytes > 0xFFFFFFFF) {
                    return 0;
                }

                bo->original = (char*)realloc(bo->original, sizeof(UInt32)*((UInt32) bo->size + nb_bytes));
                if (!bo->original) {
                    return 0;
                }

                bo->size += nb_bytes;
            }
            memcpy(bo->original + bo->position, data, nb_bytes);
            bo->position += nb_bytes;
            return nb_bytes;
        case ISOM_BITOPER_FILE_READ:
        case ISOM_BITOPER_FILE_WRITE:
            if (fwrite(data, nb_bytes, 1, bo->stream) != 1) {
                return 0;
            }

            if (bo->size == bo->position) {
                bo->size += nb_bytes;
            }

            bo->position += nb_bytes;
            return nb_bytes;
        default:
            return 0;
        }
    }
    
    while (nb_bytes) {
        isom_write_bit_operation_int(bo, (SInt32) *data, 8);
        data++;
        nb_bytes--;
    }
    return (UInt32) (bo->position - begin);
}

/*
* Desc : 
*/
void isom_get_bit_operation_content(isom_bit_operation_t *bo, char **output, UInt32 *outSize)
{
    /*only in WRITE MEM mode*/
    if (bo->bsmode != ISOM_BITOPER_WRITE_DYN) return;
    if (!bo->position && !bo->nbBits) {
        *output = NULL;
        *outSize = 0;
        free(bo->original);
    } else {
        isom_cut_bit_operation_buffer(bo);
        *output = bo->original;
        *outSize = (UInt32) bo->size;
    }
    bo->original = NULL;
    bo->size = 0;
    bo->position = 0;
}

/*
* Desc : 
*/
void isom_write_bit_operation_long_int(isom_bit_operation_t *bo, SInt64 value, SInt32 nBits)
{
    value <<= sizeof (SInt64) * 8 - nBits;
    while (--nBits >= 0) {
        isom_write_bit_operation_bit (bo, value < 0);
        value <<= 1;
    }
}

/*
* Desc : 
*/
void isom_write_bit_operation_bit(isom_bit_operation_t *bo, UInt32 bit)
{
    bo->current <<= 1;
    bo->current |= bit;
    if (++ bo->nbBits == 8) {
        bo->nbBits = 0;
        isom_write_bit_operation_byte(bo, (UInt8) bo->current);
        bo->current = 0;
    }
}

/*
* Desc : 
*/
void isom_write_bit_operation_byte(isom_bit_operation_t *bo, UInt8 val)
{
    /*we don't allow write on READ buffers*/
    if ( (bo->bsmode == ISOM_BITOPER_READ) || (bo->bsmode == ISOM_BITOPER_FILE_READ) ) {
        return;
    }
    if (!bo->original && !bo->stream) {
        return;
    }
    
    /*we are in MEM mode*/
    if ( (bo->bsmode == ISOM_BITOPER_WRITE) || (bo->bsmode == ISOM_BITOPER_WRITE_DYN) ) {
        if (bo->position == bo->size) {
            /*no more space...*/
            if (bo->bsmode != ISOM_BITOPER_WRITE_DYN) {
                return;
            }
            /*realloc if enough space...*/
            if (bo->size > 0xFFFFFFFF) {
                return;
            }
            bo->original = (char*)realloc(bo->original, (UInt32) (bo->size + BO_MEM_BLOCK_ALLOC_SIZE));
            if (!bo->original) {
                return;
            }
            bo->size += BO_MEM_BLOCK_ALLOC_SIZE;
        }
        bo->original[bo->position] = val;
        bo->position++;
        return;
    }
    /*we are in FILE mode, no pb for any realloc...*/
    fputc(val, bo->stream);
    /*check we didn't rewind the stream*/
    if (bo->size == bo->position) {
        bo->size++;
    }
    bo->position += 1;
}

/*
*/
UInt32 isom_cut_bit_operation_buffer(isom_bit_operation_t *bo)
{	
    UInt32 nbBytes;
    if ( (bo->bsmode != ISOM_BITOPER_WRITE_DYN) && (bo->bsmode != ISOM_BITOPER_WRITE)) {
        return (UInt32) -1;
    }
    /*Align our buffer or we're dead!*/
    isom_align_bit_operation(bo);
    
    nbBytes = (UInt32) (bo->size - bo->position);
    if (!nbBytes || (nbBytes == 0xFFFFFFFF) || (bo->position >= 0xFFFFFFFF)) {
        return 0;
    }
    bo->original = (char*)realloc(bo->original, (UInt32) bo->position);
    if (! bo->original) {
        return (UInt32) -1;
    }
    /*just in case, re-adjust..*/
    bo->size = bo->position;
    return nbBytes;
}

/*
*/
UInt8 isom_align_bit_operation(isom_bit_operation_t *bo)
{
    UInt8 res = (UInt8)(8 - bo->nbBits);
    
    if ( (bo->bsmode == ISOM_BITOPER_READ) || (bo->bsmode == ISOM_BITOPER_FILE_READ)) {
        if (res > 0) {
            isom_read_bit_operation_int(bo, res);
        }
        return res;
    }
    
    if (bo->nbBits > 0) {
        isom_write_bit_operation_int (bo, 0, res);
        return res;
    }
    return 0;
}

UInt32 isom_read_bit_operation_u8(isom_bit_operation_t *bo)
{
    return (UInt32) isom_read_bit_operation_byte(bo);
}

/*
*/
UInt32 isom_read_bit_operation_int(isom_bit_operation_t *bo, UInt32 nBits)
{
    UInt32 ret;
    
    ret = 0;
    while (nBits-- > 0) {
        ret <<= 1;
        ret |= isom_read_bit_operation_bit(bo);
    }
    return ret;
}

/*
* Desc : 
*/
UInt8 isom_read_bit_operation_bit(isom_bit_operation_t *bo)
{
    SInt32 ret;

    if (bo->nbBits == 8) {	
        bo->current = isom_read_bit_operation_byte(bo);
        bo->nbBits = 0;
    }
    
    bo->current <<= 1;
    bo->nbBits++;
    ret = (bo->current & 0x100) >> 8;
    return (UInt8) ret;
 
}

/*
*/
UInt32 isom_get_bit_operation_bit_size(UInt32 max_val)
{
    UInt32 k=0;
    while ((SInt32) max_val > ((1<<k)-1) ) {
        k+=1;
    }
    return k;
}

/*
* Desc : 
*/
UInt8 isom_read_bit_operation_byte(isom_bit_operation_t *bo)
{
    if (bo->bsmode == ISOM_BITOPER_READ) {
        if (bo->position == bo->size) {
            if (bo->EndOfStream) bo->EndOfStream(bo->par);
            return 0;
        }
        return (UInt8) bo->original[bo->position++];
    }
    /*we are in FILE mode, test for end of file*/
    if (!feof(bo->stream)) {
        bo->position++;
        return (UInt8) fgetc(bo->stream);
    }
    if (bo->EndOfStream) {
        bo->EndOfStream(bo->par);
    }
    return 0;
}

/*
*/
UInt64 isom_available_bit_operation(isom_bit_operation_t *bo)
{    
    /*in WRITE mode only, this should not be called, but return something big in case ...*/
    if ( (bo->bsmode == ISOM_BITOPER_WRITE) 
        || (bo->bsmode == ISOM_BITOPER_WRITE_DYN) 
        ) {
        return (UInt64) -1;
    }
    
    /*we are in MEM mode*/
    if (bo->bsmode == ISOM_BITOPER_READ) {
        return (bo->size - bo->position);
    }
    /*FILE READ: assume size hasn't changed, otherwise the user shall call isom_bo_get_refreshed_size*/
    if (bo->bsmode==ISOM_BITOPER_FILE_READ) {
        return (bo->size - bo->position);
    }
    return 0;
}

/*
* Desc : 
*/
UInt64 isom_get_bit_operation_position(isom_bit_operation_t *bo)
{
    return bo->position; 
}

/*
* Desc : 
*/
UInt32 isom_read_bit_operation_data(isom_bit_operation_t *bo, char *data, UInt32 nbBytes)
{
    UInt64 orig = bo->position;
    
    if (bo->position+nbBytes > bo->size) return 0;
    
    if (isom_is_align_bit_operation(bo)) {
        switch (bo->bsmode) {
        case ISOM_BITOPER_READ:
            memcpy(data, bo->original + bo->position, nbBytes);
            bo->position += nbBytes;
            return nbBytes;
        default:
            return 0;
        }
    }
    
    while (nbBytes-- > 0) {
        *data++ = (UInt8)isom_read_bit_operation_int(bo, 8);
    }
    return (UInt32) (bo->position - orig);
    
}

/*
* Desc : 
*/
Bool16 isom_seek_bit_operation(isom_bit_operation_t *bo, UInt64 offset)
{
    /*warning: we allow offset = bo->size for WRITE buffers*/
    if (offset > bo->size) {
        return false;
    }
    
    isom_align_bit_operation(bo);
    return isom_seek_bit_operation_intern(bo, offset);
}

/*
* Desc : 
*/
Bool16 isom_seek_bit_operation_intern(isom_bit_operation_t *bo, UInt64 offset)
{
    UInt32 i;
    /*if mem, do it */
    if ((bo->bsmode == ISOM_BITOPER_READ) || (bo->bsmode == ISOM_BITOPER_WRITE) || (bo->bsmode == ISOM_BITOPER_WRITE_DYN)) {
        if (offset > 0xFFFFFFFF) {
            return false;
        }
        /*0 for write, read will be done automatically*/
        if (offset >= bo->size) {
            if ( (bo->bsmode == ISOM_BITOPER_READ) || (bo->bsmode == ISOM_BITOPER_WRITE) ) {
                return false;
            }
            /*in DYN, realloc ...*/
            bo->original = (char*)realloc(bo->original, (UInt32) (offset + 1));
            for (i = 0; i < (UInt32) (offset + 1 - bo->size); i++) {
                bo->original[bo->size + i] = 0;
            }
            bo->size = offset + 1;
        }
        bo->current = bo->original[offset];
        bo->position = offset;
        bo->nbBits = (bo->bsmode == ISOM_BITOPER_READ) ? 8 : 0;
        return true;
    }
    return false;
}