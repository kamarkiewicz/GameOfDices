#include "List.h"


typedef struct drListNode {
	void    *data;
	struct  drListNode *np; ///< pointer to (begin XOR end) node
} drListNode;

typedef struct drList {
	drListNode  *head,
	            *tail;
	size_t      size;
	void (*destructor)(void* data); // optional destructor, may be free
} drList;

typedef struct drListIter drListIter;


///////////////////////////////////////////////////////////
/// Private
drListNode*
xor (drListNode *a, drListNode *b){
	return (drListNode*) ((uintptr_t) a ^ (uintptr_t) b);
}


///////////////////////////////////////////////////////////
/// Public
drList*
drList_create (
	void (*destructor)(void* data) // optional destructor, can be free/NULL/own function.
){
	drList *ptr = malloc (sizeof (drList));
	ptr->head = ptr->tail = NULL;
	ptr->size = 0;
	ptr->destructor = destructor;
	return ptr;
}


void
drList_destroy (
	drList *self
){
	while (! drList_empty (self)){
		if (self->destructor != NULL)
			self->destructor (drList_front (self));
		drList_delete (self, 1);
	}
	free (self);
}


void
drList_insert (
	drList *self,
	void *data,
	bool at_tail
){
	if (at_tail)
		drListIter_insert (drList_end (self), data);
	else
		drListIter_insert (drList_begin (self), data);
}


void
drList_delete (
	drList *self,
	bool from_tail
){
	if (from_tail)
		drListIter_erase (drList_rbegin (self));
	else
		drListIter_erase (drList_begin (self));
}


void*
drList_front (drList *self){
	return drListIter_get (drList_rbegin (self));
}


void*
drList_back (drList *self){
	return drListIter_get (drList_begin (self));
}

///////////////////////////////////////////////////////////
drListIter
drList_begin (
	drList *self
){
	drListIter it = {NULL, self->head, NULL, self};
	if (self->head)
		it.next = xor (NULL, self->head->np);
	return it;
}


drListIter
drList_end (
	drList *self
){
	return (drListIter){self->tail, NULL, NULL, self};
}


drListIter
drList_rbegin (
	drList *self
){
	drListIter it = {NULL, self->tail, NULL, self};
	if (self->tail)
		it.prev = xor (self->tail->np, NULL);
	return it;
}


drListIter
drList_rend (
	drList *self
){
	return (drListIter){NULL, NULL, self->head, self};
}


void*
drListIter_get (
	drListIter self
){
	assert (self.curr);
	return self.curr->data;
}


void
drListIter_next (
	drListIter *self
){
	assert (self && (self->curr || self->next));
	drListIter it = {self->curr, self->next, NULL, self->list};
	if (it.curr)
		it.next = xor (it.curr->np, it.prev);
	*self = it;
}


void
drListIter_prev (
	drListIter *self
){
	assert (self && (self->curr || self->prev));
	drListIter it = {NULL, self->prev, self->curr, self->list};
	if (it.curr)
		it.prev = xor (it.curr->np, it.next);
	*self = it;
}


bool
drListIter_equal (
	drListIter *self,
	drListIter iter
){
	return (self->curr == iter.curr);
}


bool
drListIter_isEnd (
	drListIter *self
){
	return (self->curr == NULL);
}


/**
 * Is able to insert new element BEFORE 'self' pointer.
 * @param  self Pointer
 * @param  data Data
 * @return      Iterator on the brand new element
 */
drListIter
drListIter_insert (
	drListIter self,
	void *data
){
	drListNode *ptr = malloc (sizeof (drListNode));
	assert (ptr);
	ptr->data = data;
	
	// list is empty
	if (self.list->head == NULL){
		assert (!self.list->size);
		self.curr = self.list->head = self.list->tail = ptr;
		ptr->np = xor (NULL, NULL);
		self.list->size = 1;
		return self;
	}
	assert (self.list->size > 0); // list must have at least one element
	if (self.list->head == self.curr){ // at beginning
		assert (self.prev == NULL);
		self.list->head = ptr;
	}
	if (self.list->tail == self.prev){ // at the end
		assert (self.curr == NULL);
		self.list->tail = ptr;
	}
	drListIter before = self;
	drListIter_prev (&before);
	ptr->np = xor (before.curr, self.curr);
	if (self.curr)
		self.curr->np = xor (ptr, self.next);
	if (before.curr)
		before.curr->np = xor (before.prev, ptr);
	self.next = self.curr;
	self.curr = ptr;
	self.prev = before.curr;

	self.list->size++;
	assert (! drList_empty (self.list));

	return self;
}

/**
 * Delete the element pointed by self and return iterator on next element.
 * @param  self Element to delete
 * @return      Iterator on the next element
 */
drListIter
drListIter_erase (
	drListIter self
){
	assert (self.list->head != NULL && self.list->size > 0);
	assert (self.curr);
	drListIter before = self, after = self;
	drListIter_prev (&before);
	drListIter_next (&after);
	if (self.list->head == self.curr)
		self.list->head = self.next;
	if (self.list->tail == self.curr)
		self.list->tail = self.prev;
	if (before.curr){
		before.curr->np = xor (before.prev, self.next);
		before.next = self.next;
	}
	if (after.curr){
		after.curr->np = xor (self.prev, after.next);
		after.prev = self.prev;
	}
	free (self.curr);

	assert (after.list->size > 0);
	after.list->size--;

	return after;
}


/**
 * Swaps values of two iterators
 * @param x
 * @param y
 */
void
drListIter_swap (
	drListIter *x,
	drListIter *y
){
	swap (x->curr->data, y->curr->data);
}



void
drList_sort (
	drList *self,
	int (*data_cmp)(void *x, void *y)
){
	drListIter i = drList_begin (self), j;
	for (; !drListIter_equal (&i, drList_end (self)); drListIter_next (&i)){
		j = i;
		drListIter_next (&j);
		for (; !drListIter_equal (&j, drList_end (self)); drListIter_next (&j))
			if (data_cmp (i.curr->data, j.curr->data) == 1)
				drListIter_swap (&i, &j);
	}
}


bool
drList_empty (
	drList *self
){
	return !self->size;
}


/** 
 * @return Counter of elements in list. O(1)
 */
size_t
drList_size (
	drList *self
){
	return self->size;
}