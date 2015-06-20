/**
 * @brief Stack structure
 * Implemented on drList.
 */

#ifndef DRY_STACK_H
#define DRY_STACK_H

#include <assert.h>
#include "List.h"

typedef drList drStack;

///////////////////////////////////////////////////////////
/// Constructor / Destructor
drStack* drStack_create (void (*destructor)(void*));
void drStack_destroy (drStack* self);
///////////////////////////////////////////////////////////

/// Edit
void drStack_push (drStack* self, void* data);
void drStack_pop (drStack* self);

/// Access
void* drStack_top (drStack* self);
bool drStack_empty (drStack* self);

#endif // DRY_STACK_H