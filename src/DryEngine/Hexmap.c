#include "Hexmap.h"
#ifndef NDEBUG
#include <stdio.h>
#endif


///////////////////////////////////////////////////////////
/// Private
static sfVector2i const hexoffset [6][2] = {
	{{1, 1}, {2, 0}}, // right lower corner
	{{-1, 1}, {1, 1}}, // bottom
	{{-2, 0}, {-1, 1}}, // left lower
	{{-2, 0}, {-1, -1}}, // left upper
	{{-1, -1}, {1, -1}}, // top
	{{1, -1}, {2, 0}}  // right upper
};


static sfVector2i const tileNeighbourhood [6] = { /// in axial
	{ 1, 0}, { 1,-1}, { 0,-1},
	{-1, 0}, {-1, 1}, { 0, 1}
};

/**
 * Draw all tiles in the chunk and the outline.
 */
typedef struct drEdge {
	int counter;
	sfVector2i p, q; ///// (p) ---- (q)   cause every (stick) has two ends.
} drEdge;

int drEdge_cmp (drEdge *a, drEdge *b){
	if (a->p.x < b->p.x) return -1;
	if (a->p.x > b->p.x) return 1;
	if (a->p.y < b->p.y) return -1;
	if (a->p.y > b->p.y) return 1;
	/// p are equal
	if (a->q.x < b->q.x) return -1;
	if (a->q.x > b->q.x) return 1;
	if (a->q.y < b->q.y) return -1;
	if (a->q.y > b->q.y) return 1;
	// p and q are equal
	return 0;
}

bool drEdge_commonPoint (drEdge *a, drEdge *b, sfVector2i *common){
	if (((a->p.x == b->p.x) && (a->p.y == b->p.y))
	 || ((a->p.x == b->q.x) && (a->p.y == b->q.y))){
		if (common != NULL){
			common->x = a->p.x;
			common->y = a->p.y;
		}
		return true;
	}
	if (((a->q.x == b->p.x) && (a->q.y == b->p.y))
	 || ((a->q.x == b->q.x) && (a->q.y == b->q.y))){
		if (common != NULL){
			common->x = a->q.x;
			common->y = a->q.y;
		}
		return true;
	}
	// p and q are equal
	return false;
}


///////////////////////////////////////////////////////////
/// Public
 
drHexmap*
drHexmap_create (void)
{
	drHexmap *self = malloc (sizeof (drHexmap));
	self->bounds = (sfFloatRect){0.0, 0.0, 0.0, 0.0};
	drHexmap_setHexSizes (self, 20.0, M_PI/6.0);
	self->chunks = drList_create (DESTRUCTOR (drHexmap_Chunk_destroy));
	return self;
}

void
drHexmap_setHexSizes (
	drHexmap *self,
	float    edge,
	float    angle
){
	self->grid.x = edge / 2.0;
	self->grid.y = cos (angle) * edge;
}


void
drHexmap_destroy (drHexmap *self){
	drList_destroy (self->chunks);
	free (self);
}


drHexmap_Chunk*
drHexmap_Chunk_create (void)
{
	drHexmap_Chunk *self = malloc (sizeof (drHexmap_Chunk));
	self->id = 0;
	self->tiles = drList_create (free);
	self->outline = NULL;
	self->color = sfWhite;
	self->hexmap = NULL;
	self->neighbours = drList_create (free); // of sfVector2i
	return self;
}


void
drHexmap_Chunk_destroy (drHexmap_Chunk *self)
{
	drList_destroy (self->tiles);
	drList_destroy (self->neighbours);
	if (self->outline)
		sfConvexShape_destroy (self->outline);
	free (self);
}


/**
 * Adds tile into chunk and his neighbours to list.
 * @param self [description]
 * @param tile [description]
 */
void
drHexmap_Chunk_pushTile (
	drHexmap_Chunk *self,
	sfVector2i     tile
){
	sfVector2i *ptr = malloc (sizeof (sfVector2i));
	*ptr = tile;
	drList_insert (self->tiles, ptr, true);
	if (self->outline)
		sfConvexShape_destroy (self->outline);

	/// insert unique tile's neighbours to self->neighbours
	for (int i = 0; i < 6; ++i)
	{
		ptr = malloc (sizeof (sfVector2i));
		ptr->x = tile.x + tileNeighbourhood [i].x;
		ptr->y = tile.y + tileNeighbourhood [i].y;

		drListIter nbIt = drList_begin (self->neighbours);
		for (; !drListIter_isEnd (&nbIt); drListIter_next (&nbIt))
		{
			sfVector2i *tmp = drListIter_get (nbIt);
			if (tmp->x == ptr->x
			 && tmp->y == ptr->y)
			{
				free (ptr);
				break;
			}
		}

		if (drListIter_isEnd (&nbIt))
			drList_insert (self->neighbours, ptr, 1);
	}
}


void
drHexmap_pushChunk (
	drHexmap *self,
	drHexmap_Chunk *chunk
){
	chunk->hexmap = self;
	drList_insert (self->chunks, chunk, true);
}



/// @return Local odd-q offset (point on the grid)
sfVector2i
drHexmap_Tile_getLocalOffset (
	sfVector2i  coords //< axial cords
){
	/// axial coords -> odd-q offset
	sfVector2i offset = {
		coords.x,
		coords.y + (coords.x - (coords.x&1)) / 2
	};
	/// use offset coords to calculate
	sfVector2i result = {
		3.0 * offset.x + 2.0 , // * w
		2.0 * offset.y + (offset.x&1) + 1 // * h
	};
	return result; // returns location on the grid
}


/// @return Global odd-q offset position of hex
sfVector2f
drHexmap_Tile_getPixelOffset (
	drHexmap    *map,
	sfVector2i  coords //< axial cords
){
	sfVector2f result;
	coords = drHexmap_Tile_getLocalOffset (coords);
	result.x = coords.x * map->grid.x;
	result.y = coords.y * map->grid.y;
	result.x += map->bounds.left;
	result.y += map->bounds.top;
	return result;
}


/**
 * [drHexmap_Tile_getPosByLocalPixel  description]
 * @param  map   [description]
 * @param  pixel [description]
 * @return       [description]
 */
sfVector2i
drHexmap_Tile_getPosByLocalPixel (
	drHexmap    *map,
	sfVector2i  pixel
){
	double blockx = (double) pixel.x / map->grid.x;
	double blocky = (double) pixel.y / map->grid.y;
	double ix, iy, fx, fy; /// integer and fractional parts of block
	fx = modf (blockx, &ix);
	fy = modf (blocky, &iy);

	/// coords in odd-q needed correction
	int kx = ix / 3;
	int ky = (iy - (kx&1)) / 2;

	/// grid method
	if (fmod (ix, 3.0) == 0.0){
		if (fmod (ix, 6.0) == 0.0){
			if (fmod (iy, 2.0) == 0.0){
				if (fx + fy < 1.0){
					kx--;
					ky--;
				}
			} else
				if (fx - fy < 0.0)
					kx--;
		} else {
			if (fmod (iy, 2.0) == 1){
				if (fx + fy < 1.0)
					kx--;
			} else
				if (fx - fy < 0.0){
					kx--;
					ky++;
				}
		}
	}


    /// odd-q -> axial
    pixel.x = kx;
    pixel.y = ky - (kx - (kx&1)) / 2;
    return pixel;
}


/**
 * This locating algorithm uses some observations about the grid
 * of hexagons.
 */
sfVector2i
drHexmap_Tile_getByPoint (
	drHexmap 	*map,
	sfVector2i  dot
){
	dot.x -= map->bounds.left;
	dot.y -= map->bounds.top;
	dot = drHexmap_Tile_getPosByLocalPixel (map, dot);
	return dot;
}


bool
drHexmap_areChunksNeighbours (
	drHexmap_Chunk *a,
	drHexmap_Chunk *b
){
	assert (a && b);
	/// Check in a->neighbours if b->tiles exists
	const sfVector2i *A, *B;
	drListIter aIt = drList_begin (a->neighbours),
	           bIt;
	for (; !drListIter_isEnd (&aIt); drListIter_next (&aIt))
	{
		A = drListIter_get (aIt);
		bIt = drList_begin (b->tiles);
		for (; !drListIter_isEnd (&bIt); drListIter_next (&bIt))
		{
			B = drListIter_get (bIt);
			if (A->x == B->x && A->y == B->y)
				return true;
		}
	}
	return false;
}


sfConvexShape*
drHexmap_Tile_createShape (
	float edge
){
	sfConvexShape *hex = sfConvexShape_create ();
	sfConvexShape_setPointCount (hex, 6);
	float angle;
	sfVector2f coords;
	for (int i = 0; i < 6; ++i){
		angle = 2.0 * M_PI / 6.0 * i;
		coords.x = edge * cos(angle);
		coords.y = edge * sin(angle);
		sfConvexShape_setPoint (hex, i, coords);
	}
	sfConvexShape_setPointCount (hex, 6);
	return hex;
}


drHexmap_Chunk*
drHexmap_Chunk_getByTile (
	drHexmap   *map,
	sfVector2i axial
){
	drHexmap_Chunk *chunk;
	sfVector2i tile;
	drListIter chunkIt, tileIt;
	chunkIt = drList_begin (map->chunks);
	while (! drListIter_isEnd (&chunkIt))
	{
		chunk = drListIter_get (chunkIt);
		tileIt = drList_begin (chunk->tiles);
		while (! drListIter_isEnd (&tileIt))
		{
			tile = *(sfVector2i*) drListIter_get (tileIt);
			if (tile.x == axial.x && tile.y == axial.y)
				return chunk;
			drListIter_next (&tileIt);
		}
		drListIter_next (&chunkIt);
	}
	return NULL;
}


drHexmap_Chunk*
drHexmap_Chunk_getByPoint (
	drHexmap   *map,
	sfVector2i dot
){
	dot = drHexmap_Tile_getByPoint (map, dot);
	return drHexmap_Chunk_getByTile (map, dot);
}


drHexmap_Chunk*
drHexmap_Chunk_getById (
	drHexmap   *map,
	int        id
){
	drHexmap_Chunk *chunk;
	drListIter chunkIt;
	chunkIt = drList_begin (map->chunks);
	while (! drListIter_isEnd (&chunkIt))
	{
		chunk = drListIter_get (chunkIt);
		if (chunk->id == id)
			return chunk;
		drListIter_next (&chunkIt);
	}
	return NULL;
}


sfConvexShape*
drHexmap_Chunk_createShape (
	drHexmap        *map,
	drHexmap_Chunk  *chunk
){
	drList *edgelist = drList_create (free);
	sfVector2i axial;

	drEdge *edge, *tmp;

	drListIter tileIt = drList_begin (chunk->tiles);
	while (! drListIter_equal (&tileIt, drList_end(chunk->tiles))){
		axial = *(sfVector2i*)(drListIter_get (tileIt));
		/// axial coords -> local odd-q offset (point on the grid)
		sfVector2i offset = drHexmap_Tile_getLocalOffset (axial);
		// For every edge:
		for (int i = 0; i < 6; ++i){
			/// create edge
			edge = malloc (sizeof (drEdge));
			assert (edge);
			edge->counter = 1;
			edge->p.x = offset.x + hexoffset[i][0].x;
			edge->p.y = offset.y + hexoffset[i][0].y;
			edge->q.x = offset.x + hexoffset[i][1].x;
			edge->q.y = offset.y + hexoffset[i][1].y;

			/// Check if edge is in List
			/// if yes: raise coutner
			///   else: push
			drListIter edgeIt = drList_begin (edgelist);
			while (! drListIter_equal (&edgeIt, drList_end (edgelist))){
				tmp = (drEdge*)(drListIter_get (edgeIt));
				if (! drEdge_cmp (tmp, edge)){
					tmp->counter++;
					free (edge);
					break;
				}
				drListIter_next (&edgeIt);
			}
			if (drListIter_equal (&edgeIt, drList_end (edgelist))) /// if not egsist in edgelist
				drList_insert (edgelist, edge, true); // push at the end
		}
		drListIter_next (&tileIt);
	}


	/// Here i've got the list of edges!
	/// Just create a beauty shape from it
	assert (! drList_empty (edgelist));
	/// Oh wait... I need to figure out how...


	/// Fist i need to remove edges that counter > 1
	drListIter edgeIt = drList_begin (edgelist);
	while (! drListIter_equal (&edgeIt, drList_end (edgelist))){
		edge = drListIter_get(edgeIt);
		if (edge->counter > 1){
			free (drListIter_get (edgeIt));
			edgeIt = drListIter_erase (edgeIt);
		}
		else
			drListIter_next (&edgeIt);
	}


	/// Now set them in the right order to create a ring
	edgeIt = drList_begin (edgelist);
	drListIter subedgeIt;
	while (! drListIter_equal (&edgeIt, drList_end (edgelist))){
		subedgeIt = edgeIt; /// breakpoint here
		drListIter_next (&subedgeIt);
		while (! drListIter_equal (&subedgeIt, drList_end (edgelist))){
			if (drEdge_commonPoint (drListIter_get (edgeIt), drListIter_get(subedgeIt), NULL)){
				/// move subedgeIt element next to edgeIt element
				drListIter after = edgeIt;
				drListIter_next (&after);
				drListIter_swap (&after, &subedgeIt);
				break;
			}
			drListIter_next (&subedgeIt);
		}
		drListIter_next (&edgeIt);
	}
	/// close the ring...
	drList_insert (edgelist, drList_back (edgelist), true);


	//// Now just draw it...
	float w = map->grid.x;
	float h = map->grid.y;

	sfConvexShape *shape = sfConvexShape_create ();
	sfConvexShape_setPointCount (shape, drList_size (edgelist) - 1);
	edgeIt = drList_begin (edgelist);
	subedgeIt = edgeIt; /// breakpoint here
	drListIter_next (&edgeIt);
	int i = 0;
	while (! drListIter_equal (&edgeIt, drList_end (edgelist))){
		sfVector2i common;
		drEdge_commonPoint (drListIter_get (edgeIt), drListIter_get(subedgeIt), &common);
		sfVector2f pkt = {common.x * w, common.y * h};
		sfConvexShape_setPoint (shape, i++, pkt);
		subedgeIt = edgeIt;
		drListIter_next (&edgeIt);
	}

	/// Remember the last element? It has pointer same as the first element...
	drList_delete (edgelist, 1);

	drList_destroy (edgelist);
	return shape;
}


void ////////////////////////////////////////////////////////////// **
drRenderWindow_drawHexmap_Chunk (
	sfRenderWindow  *window,
	drHexmap_Chunk  *chunk
){
	/// Chunk has one common outline
	drHexmap *map = chunk->hexmap;
	///INFO: If there was a drSet, it should be easier...
	if (chunk->outline == NULL){
		chunk->outline = drHexmap_Chunk_createShape (map, chunk);
		assert (chunk->outline);
		sfConvexShape_move (chunk->outline, (sfVector2f){map->bounds.left, map->bounds.top});
		sfConvexShape_setFillColor (chunk->outline, sfTransparent);
		sfConvexShape_setOutlineColor (chunk->outline, sfBlack);
		sfConvexShape_setOutlineThickness (chunk->outline, 2.0);
	}
	/// Draw all the tiles
	sfVector2i pos;
	drListIter tileIt = drList_begin (chunk->tiles);
	for (; !drListIter_isEnd (&tileIt); drListIter_next (&tileIt)){
		pos = *(sfVector2i*) drListIter_get (tileIt);
		drRenderWindow_drawHexmap_Tile (window, map, pos, chunk->color);
	}

	sfRenderWindow_drawConvexShape (window, chunk->outline, NULL);
}


void
drRenderWindow_drawHexmap (
	sfRenderWindow *window,
	drHexmap *map
){
	drListIter chunkIt = drList_begin (map->chunks);
	for (; !drListIter_isEnd (&chunkIt); drListIter_next (&chunkIt))
		drRenderWindow_drawHexmap_Chunk (window, drListIter_get (chunkIt));
}


#ifndef NDEBUG
void
drRenderWindow_drawHexmap_Tile_labelIt (
	sfRenderWindow *window,
	drHexmap *hexmap,
	sfVector2i coords,
	sfFont *font
){
	////////// Label it!
	sfText *label = sfText_create ();
	char str [8];
	sprintf (str, "%2d,%2d", coords.x, coords.y);
	sfText_setString (label, str);
	sfText_setFont(label, font);
	sfText_setColor (label, sfBlack);
	sfText_setCharacterSize (label, 14);
	sfText_setPosition (label,
		drHexmap_Tile_getPixelOffset (hexmap, coords));
	float w = hexmap->grid.x;
	float h = hexmap->grid.y;
	sfText_move (label, (sfVector2f){-1.3*w, -h/2.0});
	sfRenderWindow_drawText (window, label, NULL);
	sfText_destroy (label);
}
#endif

void
drRenderWindow_drawHexmap_Tile (
	sfRenderWindow  *window,
	drHexmap        *hexmap,
	sfVector2i      coords, ///< axial coords
	sfColor         color
){
	sfConvexShape *shape = drHexmap_Tile_createShape (hexmap->grid.x * 2.0);
	/// draw position
	sfConvexShape_setPosition (shape,
		drHexmap_Tile_getPixelOffset (hexmap, coords)
	);
	sfConvexShape_setFillColor (shape, color);
	sfRenderWindow_drawConvexShape (window, shape, NULL);

	sfConvexShape_destroy (shape);
}


void
drRenderWindow_drawHexmapGrid (
	sfRenderWindow *window,
	drHexmap *hexmap
){
	sfVertexArray *line = sfVertexArray_create ();
	sfVertexArray_setPrimitiveType (line, sfLines);
	float w = hexmap->grid.x;
	float h = hexmap->grid.y;
	sfFloatRect bounds = hexmap->bounds;

	sfVertex v[2] = {
		{{bounds.left, bounds.top}, sfBlack, {0, 0}},
		{{bounds.left, bounds.top + bounds.height}, sfBlack, {0, 0}}
	};
	for (float p = 0.0; p <= bounds.width; p += w){
		sfVertexArray_append (line, v[0]);    
		sfVertexArray_append (line, v[1]);
		v[0].position.x += w;
		v[1].position.x += w;
	}

	sfVertex u[2] = {
		{{bounds.left, bounds.top}, sfBlack, {0, 0}},
		{{bounds.left + bounds.width, bounds.top}, sfBlack, {0, 0}}
	};
	for (float p = 0.0; p <= bounds.height; p += h){
		sfVertexArray_append (line, u[0]);    
		sfVertexArray_append (line, u[1]);
		u[0].position.y += h;
		u[1].position.y += h;
	}

	sfRenderWindow_drawVertexArray (window, line, NULL);
	sfVertexArray_destroy (line);
}
