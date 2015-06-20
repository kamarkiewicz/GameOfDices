#include "WinnerState.h"

static drResManager *RM;
static sfSprite  *bg;
static sfSprite  *reward;
static sfText    *text;
//static sfClock   *watch;


void
WinnerState_init ()
{
	RM = drResManager_create ();

	/* Load stuff to display */
    bg = load_sprite ("res/bggame.png");
    reward = load_sprite ("res/reward.png");

    text = load_text ();
    char buff [128] = "";
    sprintf (buff, L_PLAYER_WON, PLAY.current_player);
    sfText_setString (text, buff);


    sfSprite_setPosition (reward, (sfVector2f){100, 100});
    sfText_setPosition (text, (sfVector2f){300, 300});

//    watch = drResManager_myInsert (RM, 
//    	sfClock_create (), DESTRUCTOR (sfClock_destroy));
}

void
WinnerState_cleanup ()
{
	// free resources
	drResManager_destroy (RM);
}


void
WinnerState_pause ()
{
}

void
WinnerState_resume ()
{
}

void
WinnerState_handleEvents (void)
{
	/* Process events */
	while (sfRenderWindow_pollEvent (drWindow, &drEvent))
	{
		/* Close window : exit */
		if (drEvent.type == sfEvtClosed)
			sfRenderWindow_close (drWindow);

		if (drEvent.type == sfEvtMouseButtonReleased)
			drPopState ();
	}
}

void
WinnerState_update (void)
{

}

void
WinnerState_draw (void)
{
	sfRenderWindow_drawSprite (drWindow, bg, NULL);
	sfRenderWindow_drawSprite (drWindow, reward, NULL);
	sfRenderWindow_drawText (drWindow, text, NULL);
}


drState WinnerState = {
	WinnerState_init,
	WinnerState_cleanup,
	WinnerState_pause,
	WinnerState_resume,
	WinnerState_handleEvents,
	WinnerState_update,
	WinnerState_draw
};
