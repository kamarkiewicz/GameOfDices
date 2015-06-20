#include "MainMenuState.h"

static drResManager *RM;
static sfSprite *bgmenu, *mainlogo; /// Background



//////////////////////////////////////////////////////////
/// NewGame_layer
static struct NewGame_layer_t {
	bool 	visible;
	int 	playersNumber;
	int 	seed;
	sfText 	*text [6];
} NewGame_layer;


void
NewGame_layer_init (void)
{
	struct NewGame_layer_t *ngl = &NewGame_layer;
	ngl->visible = false;
	ngl->playersNumber = 0;
	ngl->seed = 1234;
	for (int i = 0; i < 6; ++i) ngl->text[i] = load_text ();
	/// positioning
	sfVector2f pos[2] = {{380.0, 200.0}, {390.0, 280.0}};
	sfText_setPosition (ngl->text[0], pos[0]);
	for (int i = 1; i < 6; ++i){ // for every text (sfText)
		sfText_setPosition (ngl->text[i], pos[1]);
		pos[1].x += 90.0;
	}
	/// labeling
	sfUSTR labels[6] = {
		SFUSTR(L_NUM_OF_PLAYERS),
		SFUSTR(L"2"),
		SFUSTR(L"3"),
		SFUSTR(L"4"),
		SFUSTR(L"5"),
		SFUSTR(L"6")};
	for (int i = 0; i < 6; ++i)
		sfText_setUnicodeString (ngl->text[i], labels[i]);
}


bool
NewGame_layer_handleEvents (void)
{
	struct NewGame_layer_t *ngl = &NewGame_layer;
	if (! ngl->visible) return false;

	bool handled = false;

	sfFloatRect bounds;
	if (drEvent.type == sfEvtMouseButtonReleased)
	for (int i=0; i < 6; ++i){
		bounds = sfText_getGlobalBounds (ngl->text[i]);
		if (! sfFloatRect_contains (&bounds, drEvent.mouseButton.x, drEvent.mouseButton.y))
			continue;
		if (1 <= i && i <= 5)
			ngl->playersNumber = i + 1;
		handled = true;
	}

	if (ngl->playersNumber != 0){
		PLAY.players = ngl->playersNumber;
		drPushState (&TheGameState);
	}

	return handled;
}


void
NewGame_layer_reasume ()
{
	struct NewGame_layer_t *ngl = &NewGame_layer;
	ngl->visible = false;
	ngl->playersNumber = 0;
	ngl->seed = 1234;
}


void
NewGame_layer_draw (void)
{
	struct NewGame_layer_t *ngl = &NewGame_layer;
	if (! ngl->visible) return;

	for (int i = 0; i < 6; ++i)
		sfRenderWindow_drawText (drWindow, ngl->text[i], NULL);
}




//////////////////////////////////////////////////////////
/// MainMenu_layer
static struct MainMenu_layer_t {
	bool visible;
	sfText   *text [5];
} MainMenu_layer;


void
MainMenu_layer_init (void)
{
	struct MainMenu_layer_t *mml = &MainMenu_layer;
	mml->visible = true;
	for (int i = 0; i < 6; ++i) mml->text[i] = load_text ();
	/// positioning
    sfVector2f pos = {70.0, 200.0};
    for (int i = 0; i < 5; ++i){
    	if (i == 2) sfText_setPosition (mml->text[0], pos); /// DEMO
		sfText_setPosition (mml->text[i], pos);
		pos.y += 55.0;
    }
	/// labeling
	sfUSTR labels[5] = {
		SFUSTR(L_NEW_GAME),
		SFUSTR(L_HELP),
		SFUSTR(L_DEMOS),
		SFUSTR(L_SETTINGS),
		SFUSTR(L_QUIT)};
	for (int i = 0; i < 5; ++i)
		sfText_setUnicodeString (mml->text[i], labels[i]);
}


void
MainMenu_layer_resume (void)
{
	struct MainMenu_layer_t *mml = &MainMenu_layer;
	mml->visible = true;	
}


bool
MainMenu_layer_handleEvents (void)
{
	struct MainMenu_layer_t *mml = &MainMenu_layer;
	if (! mml->visible) return false;
	/* Main Menu */
	bool handled = false;
	sfFloatRect bounds;
	if (drEvent.type == sfEvtMouseButtonReleased)
	for (int i = 0; i < 5; ++i){
		bounds = sfText_getGlobalBounds (mml->text[i]);
		if (! sfFloatRect_contains (&bounds, drEvent.mouseButton.x, drEvent.mouseButton.y))
			continue;
		switch (i){
		case 0:	NewGame_layer.visible = true; break; // new game
		case 1: break; // HELP
		case 2: break; // demos
		case 3: break; // settings
		case 4: sfRenderWindow_close (drWindow); // quit
		}
		handled = true;
	}
	return handled;
}


void
MainMenu_layer_draw (void)
{
	struct MainMenu_layer_t *mml = &MainMenu_layer;
	if (! mml->visible) return;
	for (int i = 0; i < 5; ++i)
		if (i == 0 || i == 4)
			sfRenderWindow_drawText (drWindow, mml->text[i], NULL);
}




//////////////////////////////////////////////////////////
/// MainMenuState
void
MainMenuState_init (void)
{
	RM = drResManager_create ();
	/* Load a sprite to display */
    bgmenu = load_sprite ("res/bgmenu.png");
    mainlogo = load_sprite ("res/mainlogo.png");
	sfSprite_move (mainlogo, (sfVector2f){35.0, 22.0});

	MainMenu_layer_init ();
	NewGame_layer_init ();
}


void
MainMenuState_cleanup ()
{
	drResManager_destroy (RM);
}


void
MainMenuState_pause ()
{
}


void
MainMenuState_resume ()
{
	sfSprite_setPosition (bgmenu, (sfVector2f){0.0, 0.0});
	NewGame_layer_reasume ();
	MainMenu_layer_resume ();
}


void
MainMenuState_handleEvents ()
{
	/* Process events */
	while (sfRenderWindow_pollEvent (drWindow, &drEvent))
	{
		/* Close window : exit */
		if (drEvent.type == sfEvtClosed)
			sfRenderWindow_close (drWindow);
		/* NewGame_layer */
		if (NewGame_layer_handleEvents ()) continue;
		/* MainMenu_layer */
		if (MainMenu_layer_handleEvents ()) continue;
	}
}


void
MainMenuState_update ()
{
	/// Animation of background
	/*
		This trick needs specially prepared map.
		So if image has size 1600 x 800:
			1. Add 960 to width and 540 to height
			2. Fill it with the same pattern.
	 */
	sfFloatRect rect = sfSprite_getGlobalBounds (bgmenu);
	sfVector2u wnd = sfRenderWindow_getSize (drWindow);

	if (wnd.x - rect.left > rect.width)
		sfSprite_setPosition (bgmenu, (sfVector2f){0.0, 0.0});

	sfSprite_move (bgmenu, (sfVector2f){-1.0, -0.5});
}


void
MainMenuState_draw ()
{
	sfRenderWindow_drawSprite (drWindow, bgmenu, NULL);
	sfRenderWindow_drawSprite (drWindow, mainlogo, NULL);
	MainMenu_layer_draw ();
	NewGame_layer_draw ();
}


drState MainMenuState = {
	MainMenuState_init,
	MainMenuState_cleanup,
	MainMenuState_pause,
	MainMenuState_resume,
	MainMenuState_handleEvents,
	MainMenuState_update,
	MainMenuState_draw
};
