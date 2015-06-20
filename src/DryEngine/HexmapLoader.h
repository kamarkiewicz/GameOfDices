#ifndef DRY_HEXMAPLOADER_H
#define DRY_HEXMAPLOADER_H

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "Hexmap.h"

void drHexmapLoader_loadFromFile (drHexmap *map, const char* filename);

#endif