/**
 * This class is designed to create, handle and draw crazy stuff
 * using hexagonal map with flat top.
 */


#ifndef DRY_HEXMAP_H
#define DRY_HEXMAP_H

#include <SFML/Graphics.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "List.h"
#include "Snippets.h"


/**
 * After you initialize drHexmap, you can get
 * values directly. There's no accessor.
 */
typedef struct drHexmap {
	sfFloatRect  bounds;
	sfVector2f   grid; // x == width, h == height,
	                   // where width = edge/2, height = cos (angle) * edge
	drList       *chunks; /// of drHexmap_Chunk
	/// drSet        *bunches; /// some of chunks
} drHexmap;


typedef struct drHexmap_Chunk {
	int           id;
	drList        *tiles; /// of sfVector2i
	sfConvexShape *outline;
	sfColor       color;
	drHexmap      *hexmap; /// on which hexmap it is?
	drList        *neighbours;
} drHexmap_Chunk;


///////////////////////////////////////////////////////////
/// Constructor / Destructor
drHexmap* drHexmap_create (void);
void drHexmap_setHexSizes (drHexmap *self, float edge, float angle);
void drHexmap_destroy (drHexmap *self);
drHexmap_Chunk* drHexmap_Chunk_create (void);
void drHexmap_Chunk_destroy (drHexmap_Chunk *self);

void drHexmap_pushChunk (drHexmap *self, drHexmap_Chunk *chunk);
void drHexmap_Chunk_pushTile (drHexmap_Chunk *self, sfVector2i tile);

// sfVector2i drHexmap_Tile_getLocalOffset (sfVector2i  coords);
sfVector2f drHexmap_Tile_getPixelOffset (drHexmap *hexmap, sfVector2i coords);

sfVector2i drHexmap_Tile_getByPoint (drHexmap *map, sfVector2i dot);
drHexmap_Chunk* drHexmap_Chunk_getByTile (drHexmap *map, sfVector2i axial);
drHexmap_Chunk* drHexmap_Chunk_getByPoint (drHexmap *map, sfVector2i dot);

bool drHexmap_areChunksNeighbours (drHexmap_Chunk *a, drHexmap_Chunk *b);

void drRenderWindow_drawHexmap (sfRenderWindow *window, drHexmap *hexmap);
void drRenderWindow_drawHexmap_Tile (sfRenderWindow *window, drHexmap *hexmap, sfVector2i coords, sfColor color);
void drRenderWindow_drawHexmap_Chunk (sfRenderWindow *window, drHexmap_Chunk *chunk);
void drRenderWindow_drawHexmapGrid (sfRenderWindow *window, drHexmap *hexmap);


#ifndef NDEBUG
void
drRenderWindow_drawHexmap_Tile_labelIt (
	sfRenderWindow *window,
	drHexmap *hexmap,
	sfVector2i coords,
	sfFont *font
);
#endif

#endif // DRY_HEXMAP_H