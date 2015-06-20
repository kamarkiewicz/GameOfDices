#include "HexmapLoader.h"

static lua_State* L;

///////////////////////////////////////////////////////////
/// Private



///////////////////////////////////////////////////////////
/// Public
void
drHexmapLoader_loadFromFile (
	drHexmap    *map,
	const char  *filename
){
	L = lua_open ();
	luaL_openlibs (L);
	(void) luaL_dofile (L, filename);

	lua_getglobal (L, "chunkedHexmap");
	assert (lua_istable (L, -1));
	/// stack now contains -1 => chunks(table)

	drHexmap_Chunk* chunk;
	sfVector2i tile;


	for (int id = 1;; id++)
	{
	    lua_rawgeti (L, -1, id);
	    if (lua_isnil(L, -1))
	    {
	        lua_pop(L, 1);
	        break;
	    }
		assert (lua_istable (L, -1));

		chunk = drHexmap_Chunk_create ();
		chunk->id = id;
		
	    for (int i = 1;; i++)
	    {
	        lua_rawgeti(L, -1, i);

	        if (lua_isnil(L, -1))
	        {
	            lua_pop(L, 1);
	            break;
	        }

	        assert(lua_istable(L, -1));

	        lua_rawgeti(L, -1, 1);
	        tile.x = luaL_checkint(L, -1);
	        lua_pop(L, 1);

	        lua_rawgeti(L, -1, 2);
	        tile.y = luaL_checkint(L, -1);
	        lua_pop(L, 2);

			drHexmap_Chunk_pushTile (chunk, tile);
	    }
		/// fill the chunk
		drHexmap_pushChunk (map, chunk);
		lua_pop (L, 1);
	}

	lua_close (L);
}



