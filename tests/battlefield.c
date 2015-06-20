/* file.lua contains:
data = {
	{{0, 0}, {0,1}, {1, 0}, {2,1}},
	{{2,-1}, {2,0}, {3,-1}}
}
 */

#include <stdio.h>
#include <assert.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"


int data [3][10][2];
int N[3];

int main (void)
{
	lua_State* L = lua_open ();
	luaL_openlibs (L);
	(void) luaL_dofile (L, "file.lua");

	lua_getglobal (L, "data");
	assert (lua_istable (L, -1));

	int id;
	for (id = 1;; id++)
	{
	    lua_rawgeti (L, -1, id);

	    if (lua_isnil(L, -1))
	    {
	        lua_pop(L, 1);
	        break;
	    }

	    assert(lua_istable(L, -1));

	    for (int n = 1;; n++)
	    {
	        lua_rawgeti(L, -1, n);

	        if (lua_isnil(L, -1))
	        {
	            lua_pop(L, 1);
	            break;
	        }

	        puts(lua_typename(L, lua_type(L, -1)));
	        assert(lua_istable(L, -1));

	        lua_rawgeti(L, -1, 1);
	        data[id-1][n-1][0] = luaL_checkint(L, -1);
	        lua_pop(L, 1);

	        lua_rawgeti(L, -1, 2);
	        data[id-1][n-1][1] = luaL_checkint(L, -1);
	        lua_pop(L, 2);

	        N[id-1] = n;
	    }

	    lua_pop(L, 1);
	}




	for (int i = 0; i < id-1; i++){
		for (int j = 0; j < N[i]; j++)
			printf ("%2d @ %2d  ", data[i][j][0], data[i][j][1]);
		printf ("\n");
	}


	lua_close (L);
}