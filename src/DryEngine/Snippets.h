#ifndef DRY_SNIPPETS_H
#define DRY_SNIPPETS_H

#include <string.h> // memcpy

#define M_PI 3.14159265358979323846

#define max(a,b) \
	({ __typeof__ (a) _a = (a); \
	   __typeof__ (b) _b = (b); \
	   _a > _b ? _a : _b; })

#define min(a,b) \
	({ __typeof__ (a) _a = (a); \
	   __typeof__ (b) _b = (b); \
	   _a < _b ? _a : _b; })

#define clamp(what,a,b) \
	({ min ((b), max ((what), (a))); })

#define swap(x,y) do { \
    unsigned char swap_temp[sizeof(x) == sizeof(y) ? (signed)sizeof(x) : -1]; \
    memcpy (swap_temp, &y, sizeof (x)); \
    memcpy (&y, &x,        sizeof (x)); \
    memcpy (&x, swap_temp, sizeof (x)); \
    } while (0)

#define load_sprite(path) \
 ({ sfTexture *texture = drResManager_insert (RM, drResTexture, \
    	sfTexture_createFromFile (path, NULL)); \
    sfSprite *sprite = drResManager_insert (RM, drResSprite, \
    	sfSprite_create ()); \
    sfSprite_setTexture (sprite, texture, sfTrue); \
    sprite; })

#define load_text() \
 ({ sfText *text = drResManager_insert (RM, drResText, \
    	sfText_create ()); \
    sfText_setFont (text, drDefaultFont); \
    sfText_setCharacterSize (text, 50); \
    sfText_setColor (text, sfBlack); \
    text; })

#endif // DRY_SNIPPETS_H