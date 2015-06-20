/**
 * @brief Simple resource manager
 * @details Its really simple resource manager.
 * After all just removes the all variables.
 * It uses drList to manage all data.
 * For this case it is sufficient.
 */

#ifndef DRY_RESMANAGER_H
#define DRY_RESMANAGER_H

#include <SFML/Graphics.h>
#include <SFML/Audio.h>
#include "List.h"
#include "Types.h"

typedef enum {
	drResTexture,
	drResFont,
	drResMusic,
	drResSprite,
	drResText,
	drResConvexShape
} drResType;


typedef struct drResManager {
	drList *list;
} drResManager;

//////////////////////////////////////////////////////////
/// Constructor / Destructor
drResManager* drResManager_create (void);
void drResManager_destroy (drResManager *self);
///////////////////////////////////////////////////////////

/// Keep it. Just to remove item from memory when im done with it.
void*
drResManager_insert (
	drResManager *self,
	drResType type, ///< type of resource
	void *item ///< string with relative path to resource
);


void*
drResManager_myInsert (
	drResManager *self,
	void         *item, ///< pointer to resource
	void         (*destructor)(void*)
);

#endif // DRY_RESMANAGER_H