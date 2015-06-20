#include "Stack.h"

drStack*
drStack_create (
	void (*destructor)(void*)
){
	return drList_create (destructor);
}


void
drStack_destroy (
	drStack* self
){
	drList_destroy (self);
}
///////////////////////////////////////////////////////////


void
drStack_push (
	drStack* self,
	void* data
){
	drList_insert (self, data, true);
}


void
drStack_pop (
	drStack* self
){
	assert (!drList_empty (self));
	drList_delete (self, true);
}


void*
drStack_top (
	drStack* self
){
	assert (!drList_empty (self) /* TADAM */);
	return drListIter_get (drList_rbegin (self));
}


bool
drStack_empty (
	drStack* self
){
	return drList_empty (self);
}
