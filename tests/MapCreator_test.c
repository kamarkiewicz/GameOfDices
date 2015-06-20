#include <SFML/Audio.h>
#include <SFML/Graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdio.h>

#include "../src/DryEngine/Hexmap.h"
#include "../src/DryEngine/HexmapLoader.h"

sfRenderWindow* window;
char str[100];
sfFont *font;
sfText* text;


/// typedef sfVector2i drHexmap_Tile;


int main ()
{
	sfContextSettings settings = {16, 16, 8, 2, 0};

	sfVideoMode mode = {960, 540, 32};
	sfEvent event;
	/* Create the main window */
	window = sfRenderWindow_create(mode, "SFML window",
		sfResize | sfClose, &settings);
	if (!window)
		return EXIT_FAILURE;
	font = sfFont_createFromFile ("/usr/share/fonts/truetype/freefont/FreeSerif.ttf");
	if (!font)
		return EXIT_FAILURE;
	text = sfText_create ();
	sfText_setString (text, "Hello SFML!");
	sfText_setFont(text, font);
	sfText_setColor (text, sfRed);
	sfText_setCharacterSize(text, 50);
	sfVector2i pos;
	/*
	// ramy mapki
	sfRectangleShape *rect = sfRectangleShape_create ();
	sfVector2f vec1 = {hexmap->board.width, hexmap->board.height};
	sfRectangleShape_setSize (rect, vec1);
	sfRectangleShape_setOutlineColor (rect, sfRed);
	sfVector2f vec2 = {hexmap->board.left, hexmap->board.top};
	sfRectangleShape_setPosition (rect, vec2);
	sfRectangleShape_setOutlineThickness (rect, 5.0);
	sfRectangleShape_setFillColor (rect, sfTransparent);
	*/   

	/// Init hexmap
	drHexmap *hexmap = drHexmap_create ();
	hexmap->edge = 28.75;
	hexmap->bounds = (sfFloatRect){
		20.0, 10.0,
		920.0, 520.0
	};
	if (1){
	drHexmapLoader_loadFromFile (hexmap, "map_generator.lua");


	/// Make it all sfTransparent
	drListIter chunkIt = drList_begin (hexmap->chunks);
	for (; !drListIter_isEnd (&chunkIt); drListIter_next (&chunkIt)){
		drHexmap_Chunk *chunk = drListIter_get (chunkIt);
		chunk->color = sfTransparent;
	}


	printf ("chunks = %d\n", (int)drList_size (hexmap->chunks));
	}

	/* Start the game loop */
	while (sfRenderWindow_isOpen(window))
	{
		/* Process events */
		while (sfRenderWindow_pollEvent(window, &event))
		{
			/* Close window : exit */
			if (event.type == sfEvtClosed)
				sfRenderWindow_close(window);

			if (event.type == sfEvtMouseButtonPressed){
				pos.x = event.mouseButton.x;
				pos.y = event.mouseButton.y;
				pos = drHexmap_Tile_getByPoint (hexmap, pos);
				sprintf (str, "Clicked: %d %d", pos.x, pos.y);
				drHexmap_Chunk *chunk = drHexmap_Chunk_getByTile (hexmap, pos);
				if (chunk != NULL)
					sfConvexShape_setFillColor (chunk->outline, sfTransparent);
				sfText_setString (text, str);
			}
		}
		/* Clear the screen */
		sfRenderWindow_clear(window, sfGreen);

		/* Hexmap grid */
		sfVector2i hex;
		for (int i = 0; i < 20; ++i)
			for (int j = -i / 2; j < 10 - i/2; ++j){
				hex.x = i;
				hex.y = j;
				drRenderWindow_drawHexmap_Tile (window, hexmap, hex, sfWhite);
				drRenderWindow_drawHexmap_Tile_labelIt (
					window, hexmap, hex, font);				
			}


		drRenderWindow_drawHexmap (window, hexmap);

		drRenderWindow_drawHexmapGrid (window, hexmap);

		sfRenderWindow_drawText (window, text, NULL);
		/* Update the window */
		sfRenderWindow_display(window);
		sfSleep ((sfTime){30000});
	}

	drHexmap_destroy (hexmap);

	sfFont_destroy (font);
	sfText_destroy (text);
	sfRenderWindow_destroy (window);
	return EXIT_SUCCESS;
}