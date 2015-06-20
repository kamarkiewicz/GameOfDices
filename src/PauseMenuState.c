#include "PauseMenuState.h"

drResManager *RM;
sfTexture *texture;
sfSprite  *sprite;


void
PauseMenuState_init ()
{
	RM = drResManager_create ();

	/* Load stuff to display */
    texture = drResManager_insert (RM, drResTexture,
    	sfTexture_createFromFile ("res/pause.png", NULL));
    sprite = drResManager_insert (RM, drResSprite,
    	sfSprite_create ());
    sfSprite_setTexture (sprite, texture, sfTrue);
}

void
PauseMenuState_cleanup ()
{
	// free resources
	drResManager_destroy (RM);
}


void
PauseMenuState_pause ()
{
}

void
PauseMenuState_resume ()
{
}

void
PauseMenuState_handleEvents ()
{
	/* Process events */
	while (sfRenderWindow_pollEvent (drWindow, &drEvent))
	{
		/* Close window : exit */
		if (drEvent.type == sfEvtClosed)
			sfRenderWindow_close (drWindow);
	}
}

void
PauseMenuState_update ()
{

}

void
PauseMenuState_draw ()
{
	sfRenderWindow_drawSprite(drWindow, sprite, NULL);
}


drState PauseMenuState = {
	PauseMenuState_init,
	PauseMenuState_cleanup,
	PauseMenuState_pause,
	PauseMenuState_resume,
	PauseMenuState_handleEvents,
	PauseMenuState_update,
	PauseMenuState_draw
};
