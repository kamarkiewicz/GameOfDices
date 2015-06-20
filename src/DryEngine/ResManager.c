#include "ResManager.h"

typedef struct drResData {
	void* data;
	void (*destructor)(void* data);
} drResData;


///////////////////////////////////////////////////////////
/// Private
void list_dstr (void* data){
	drResData *ptr = data;
	if (ptr->destructor)
		ptr->destructor (ptr->data);
	free (ptr);
}


//////////////////////////////////////////////////////////
/// Public
drResManager*
drResManager_create (
	void
){
	drResManager *self = malloc (sizeof (drResManager));
	self->list = drList_create (list_dstr);
	return self;
}


void drResManager_destroy (
	drResManager *self
){
	drList_destroy (self->list);
}


/// Create new resource and keep it
void*
drResManager_insert (
	drResManager *self,
	drResType    type, ///< type of resource
	void         *item ///< pointer to resource
){
	assert (item && "Should not be empty");
	drResData *data = malloc (sizeof (drResData));
	assert (data);
	data->data = item;
	switch (type){
	case drResTexture:
		data->destructor = DESTRUCTOR(sfTexture_destroy); break;
	case drResFont:
		data->destructor = DESTRUCTOR(sfFont_destroy); break;
	case drResMusic:
		data->destructor = DESTRUCTOR(sfMusic_destroy); break;
	case drResSprite:
		data->destructor = DESTRUCTOR(sfSprite_destroy); break;
	case drResText:
		data->destructor = DESTRUCTOR(sfText_destroy); break;
	case drResConvexShape:
		data->destructor = DESTRUCTOR(sfConvexShape_destroy); break;
	}
	drList_insert (self->list, data, 1);
	return item;
}


void*
drResManager_myInsert (
	drResManager *self,
	void         *item, ///< pointer to resource
	void         (*destructor)(void*)
){
	drResData *data = malloc (sizeof (drResData));
	assert (data);
	data->data = item;
	data->destructor = destructor;
	drList_insert (self->list, data, 1);
	return item;
}
