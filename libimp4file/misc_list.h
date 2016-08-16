#ifndef MISC_LIST_H
#define MISC_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "os_header.h"

typedef struct ut_item_slot_t ut_item_slot_t;
typedef struct ut_list_t ut_list_t;

struct ut_item_slot_t{
    ut_item_slot_t   *item_slot_next;
    void           *data;
};

struct ut_list_t{
    ut_item_slot_t      *item_slot_head;
    ut_item_slot_t      *item_slot_tail;
    UInt32             entry_count;
    SInt32             found_entry_number;
    ut_item_slot_t      *item_slot_found;
};


ut_list_t *isom_create_list();

UInt32 isom_get_list_count(ut_list_t *ptr);

Bool16 isom_add_list(ut_list_t *ptr, void* item);

void *isom_get_list_enum(ut_list_t *ptr, UInt32 *pos);

void *isom_get_list(ut_list_t *ptr, UInt32 item_number);

void isom_clear_all_list(ut_list_t *ptr);

void isom_delete_list(ut_list_t *ptr);

Bool16 isom_clear_list(ut_list_t *ptr, UInt32 item_number);


#ifdef __cplusplus
}
#endif

#endif