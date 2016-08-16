
#include "misc_list.h"


/*
* Desc : 
*/
UInt32 isom_get_list_count(ut_list_t *ptr)
{
    if (!ptr) {
        return 0;
    }
    return ptr->entry_count;
}

/*
* Desc : list new 
*/
ut_list_t *isom_create_list()
{
    ut_list_t *nlist = (ut_list_t *) malloc(sizeof(ut_list_t));
    if (! nlist) {
        return NULL;
    }

    nlist->item_slot_head = nlist->item_slot_found = NULL;
    nlist->item_slot_tail = NULL;
    nlist->found_entry_number = -1;
    nlist->entry_count = 0;
    return nlist;
}

/*
* Desc : remove
*/
Bool16 isom_clear_list(ut_list_t *ptr, UInt32 item_number)
{
    ut_item_slot_t *tmp, *tmp2;
    UInt32 i;
    
    /* !! if item_slot_head is null (empty list)*/
    if ((! ptr) || (! ptr->item_slot_head) || (ptr->item_slot_head && !ptr->entry_count) || (item_number >= ptr->entry_count)) {
        return false;
    }
    
    /*we delete the item_slot_head*/
    if (!item_number) {
        tmp = ptr->item_slot_head;
        ptr->item_slot_head = ptr->item_slot_head->item_slot_next;
        ptr->entry_count --;
        ptr->item_slot_found = ptr->item_slot_head;
        ptr->found_entry_number = 0;
        free(tmp);

        /*that was the last entry, reset the item_slot_tail*/
        if (!ptr->entry_count) {
            ptr->item_slot_tail = ptr->item_slot_head = ptr->item_slot_found = NULL;
            ptr->found_entry_number = -1;
        }
        return true;
    }
    
    tmp = ptr->item_slot_head;
    i = 0;
    while (i < item_number - 1) {
        tmp = tmp->item_slot_next;
        i++;
    }
    tmp2 = tmp->item_slot_next;
    tmp->item_slot_next = tmp2->item_slot_next;

    /*if we deleted the last entry, update the item_slot_tail !!!*/
    if (!tmp->item_slot_next || (ptr->item_slot_tail == tmp2)) {
        ptr->item_slot_tail = tmp;
        tmp->item_slot_next = NULL;
    }
    
    free(tmp2);
    ptr->entry_count --;
    ptr->item_slot_found = ptr->item_slot_head;
    ptr->found_entry_number = 0;
    
    return true;
}

/*
* Desc : 
*/
void isom_delete_list(ut_list_t *ptr)
{
    if (!ptr) return;
    while (ptr->entry_count) {
        isom_clear_list(ptr, 0);
    }

    free(ptr);
}

/*
* Desc : 
*/
void isom_clear_all_list(ut_list_t *ptr)
{
    while (ptr && ptr->entry_count) {
        isom_clear_list(ptr, 0);
    }
}

/*
* Desc : 
*/
void *isom_get_list(ut_list_t *ptr, UInt32 item_number)
{
    ut_item_slot_t *entry;
    UInt32 i;
    
    if ((! ptr) || (item_number >= ptr->entry_count)) {
        return NULL;
    }
    
    if ( item_number < (UInt32) ptr->found_entry_number ) {
        ptr->found_entry_number = 0;
        ptr->item_slot_found = ptr->item_slot_head;
    }
    entry = ptr->item_slot_found;
    for (i = ptr->found_entry_number; i < item_number; i++ ) {
        entry = entry->item_slot_next;
    }
    ptr->found_entry_number = item_number;
    ptr->item_slot_found = entry;
    return (void *) entry->data;
}

/*
* Desc : 
*/
void *isom_get_list_enum(ut_list_t *ptr, UInt32 *pos)
{
    void *res = isom_get_list(ptr, *pos);
    (*pos)++;
    return res;
}

/*
* Desc : 
*/
Bool16 isom_add_list(ut_list_t *ptr, void* item)
{
    ut_item_slot_t *entry;

    if (!ptr) {
        return false;
    }

    entry = (ut_item_slot_t *) malloc(sizeof(ut_item_slot_t));

    if (!entry) {
        return false;
    }

    entry->data = item;
    entry->item_slot_next = NULL;

    if (!ptr->item_slot_head) {
        ptr->item_slot_head = entry;
        ptr->entry_count = 1;
    }
    else {
        ptr->entry_count += 1;
        ptr->item_slot_tail->item_slot_next = entry;
    }

    ptr->item_slot_tail = entry;
    ptr->found_entry_number = ptr->entry_count - 1;
    ptr->item_slot_found = entry;
    return true;
}

