#include "IntroState.h"

drResManager *RM;
sfTexture *texture;
sfSprite  *sprite;


void
IntroState_init ()
{
	RM = drResManager_create ();
	/* Load a sprite to display */
    texture = drResManager_insert (RM, drResTexture,
    	sfTexture_createFromFile ("res/splash.png", NULL));
    sprite = drResManager_insert (RM, drResSprite,
    	sfSprite_create ());
    sfSprite_setTexture (sprite, texture, sfTrue);
}


void
IntroState_cleanup ()
{
	// free resources
	drResManager_destroy (RM);
}


void
IntroState_pause ()
{
}


void
IntroState_resume ()
{
}


void
IntroState_handleEvents ()
{
	/* Process events */
	while (sfRenderWindow_pollEvent (drWindow, &drEvent))
	{
		if ( // drEvent.type == sfEvtKeyPressed ||
			drEvent.type == sfEvtMouseButtonReleased)
			drChangeState (&MainMenuState);
		/* Close window : exit */
		if (drEvent.type == sfEvtClosed)
			sfRenderWindow_close (drWindow);
	}
}


void
IntroState_update ()
{

}


void
IntroState_draw ()
{
	sfRenderWindow_drawSprite (drWindow, sprite, NULL);
}


drState IntroState = {
	IntroState_init,
	IntroState_cleanup,
	IntroState_pause,
	IntroState_resume,
	IntroState_handleEvents,
	IntroState_update,
	IntroState_draw
};
