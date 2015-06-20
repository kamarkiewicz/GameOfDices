/**
 * @file List.h
 * @brief The List
 * @author Kamil Markiewicz <k.a.markiewicz@gmail.com>
 * @details Very useful implementation of doubly-linked list.
 * Uses XOR to keep two pointers on a small size of memory.
 */

#ifndef DRY_LIST_H
#define DRY_LIST_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include "Snippets.h"
#include "Types.h"


///////////////////////////////////////////////////////////
/// Structures
typedef struct drListNode drListNode;

typedef struct drList drList;

typedef struct drListIter {
	drListNode  *prev,
	            *curr,
	            *next;
	drList 		*list;
} drListIter;

///////////////////////////////////////////////////////////
/// Constructors / Destructors
drList* drList_create (void (*destructor)(void* data));
void drList_destroy (drList *self);

///////////////////////////////////////////////////////////
/// Functions
void drList_insert (drList *self, void *data, bool at_tail);
void drList_delete (drList *self, bool from_tail);
void* drList_front (drList *self); // gets data from tail
void* drList_back (drList *self);

///////////////////////////////////////////////////////////
/// Iterators
drListIter drList_begin (drList *self);
drListIter drList_end (drList *self);
drListIter drList_rbegin (drList *self);
drListIter drList_rend (drList *self);

void* drListIter_get (drListIter self);
void drListIter_next (drListIter *self);
void drListIter_prev (drListIter *self);
bool drListIter_equal (drListIter *self, drListIter iter);
bool drListIter_isEnd (drListIter *self);

drListIter drListIter_insert (drListIter self, void *data);
drListIter drListIter_erase (drListIter self);
void drListIter_swap (drListIter *x, drListIter *y);
void drList_sort (drList *self, int (*data_cmp)(void *x, void *y));


///////////////////////////////////////////////////////////
/// Capacity
bool drList_empty (drList *self);
size_t drList_size (drList *self);

#endif // DRY_LIST_H