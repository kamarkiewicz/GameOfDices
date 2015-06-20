#include <SFML/Audio.h>
#include <SFML/Graphics.h>
#include <stdlib.h>
#include <stdio.h>

#include "../src/DryEngine/HexMap.h"
#include "../src/DryEngine/Snippets.h"

sfRenderWindow* window;
char str[100];
sfText* text;


int main()
{
    sfContextSettings settings = {16, 16, 8, 2, 0};

    sfVideoMode mode = {960, 540, 32};
    sfFont *font;
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

    // Generuj mapkę
    drHexMap *hexmap = drHexMap_create ();
    hexmap->edge = 28.75;
    hexmap->angle = M_PI / 6.0;
    hexmap->board = (sfFloatRect){20.0, 8.5, 920.0, 523.0};
    drHexMap_generate (hexmap);

    // ramy mapki
    sfRectangleShape *rect = sfRectangleShape_create ();
    sfVector2f vec1 = {hexmap->board.width, hexmap->board.height};
    sfRectangleShape_setSize (rect, vec1);
    sfRectangleShape_setOutlineColor (rect, sfRed);
    sfVector2f vec2 = {hexmap->board.left, hexmap->board.top};
    sfRectangleShape_setPosition (rect, vec2);
    sfRectangleShape_setOutlineThickness (rect, 5.0);
    sfRectangleShape_setFillColor (rect, sfTransparent);

    sprintf (str, "Hello SFML: %d %d", hexmap->n, hexmap->m);
    sfText_setString (text, str);

    /* Start the game loop */
    sfVector2i pos = {-1, -1};
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
                pos = drHexMap_getHexPosition (hexmap, pos);
                sprintf (str, "Clicked: %d %d", pos.x, pos.y);
                sfText_setString (text, str);
                if (pos.x >= 0){
                    sfConvexShape_setFillColor(hexmap->hexes[pos.x][pos.y], sfYellow);
                }
            }

            if (event.type == sfEvtMouseButtonReleased){
                if (pos.x >= 0){
                    sfConvexShape_setFillColor(hexmap->hexes[pos.x][pos.y], sfWhite);
                }
            }
        }
        /* Clear the screen */
        sfRenderWindow_clear(window, sfGreen);

        /* draw the map */
        sfRenderWindow_drawRectangleShape (window, rect, NULL);
        drHexMap_draw (hexmap, window);

        /* Draw the text */
        sfRenderWindow_drawText(window, text, NULL);
        /* Update the window */
        sfRenderWindow_display(window);
        sfSleep ((sfTime){30000});
    }

    sfRectangleShape_destroy (rect);
    drHexMap_destroy (hexmap);

    /* Cleanup resources */
    sfFont_destroy (font);
    sfText_destroy(text);
    sfRenderWindow_destroy(window);
    return EXIT_SUCCESS;
}