/**
 * @file
 * @section DESCRIPTION
 * The main engine file.
 */
#include "Engine.h"

const sfTime TIME_STEP = {20000}, // framerate
             MAX_ACCUMULATED_TIME = {1000000};

drStack *drGameStates;


void
drInit (void)
{
	drGameStates = drStack_create (NULL);
    sfContextSettings settings = {16, 16, 8, 2, 0};
	sfVideoMode mode = {960, 540, 32};
	drWindow = sfRenderWindow_create (mode, "Game Of Dices", sfClose, &settings);
	assert (drWindow /* cannot init the window */);
	// default font
	drDefaultFont = sfFont_createFromFile ("res/drDefaultFont.ttf");
	assert (drDefaultFont);
}


void
drCleanup (void)
{
	// cleanup all inited states
	while (! drStack_empty (drGameStates)){
		DRSTATE (drStack_top (drGameStates)) -> cleanup ();
		drStack_pop (drGameStates);
	}
	drStack_destroy (drGameStates);
	// delete the default font from memory
	sfFont_destroy (drDefaultFont);
	sfRenderWindow_destroy (drWindow);
}

void
drChangeState (drState* state)
{
	if (! drStack_empty (drGameStates)){
		DRSTATE (drStack_top (drGameStates)) -> cleanup ();
		drStack_pop (drGameStates);
	}
	drStack_push (drGameStates, state);
	state -> init ();
}


void
drPushState (drState* state)
{
	if (drStack_empty (drGameStates))
		DRSTATE (drStack_top (drGameStates)) -> pause ();
	drStack_push (drGameStates, state);
	state -> init ();
}


void
drPopState (void)
{
	DRSTATE (drStack_top (drGameStates)) -> cleanup ();
	drStack_pop (drGameStates);
	if (! drStack_empty (drGameStates))
		DRSTATE (drStack_top (drGameStates)) -> resume ();
}


void
drHandleEvents (void)
{
	DRSTATE (drStack_top (drGameStates)) -> handleEvents ();
}


void
drUpdate (void){
	DRSTATE (drStack_top (drGameStates)) -> update ();
}


void
drDraw (void){
	sfRenderWindow_clear (drWindow, sfWhite);
	DRSTATE (drStack_top (drGameStates)) -> draw ();
	sfRenderWindow_display (drWindow);
}


int
drMainLoop (void){
	sfClock *lastUpdateTime = sfClock_create ();
	sfTime accumulator = {0};

	while (sfRenderWindow_isOpen (drWindow))
	{
		accumulator.microseconds += sfTime_asMicroseconds (sfClock_restart (lastUpdateTime)); // dt >= 0
		// zapobiegamy zbyt duzej ilosci aktualizacji w danym obiegu petli glownej
		accumulator.microseconds = clamp (accumulator.microseconds, sfTime_Zero.microseconds, MAX_ACCUMULATED_TIME.microseconds);

		drHandleEvents ();
		while (accumulator.microseconds > TIME_STEP.microseconds){
			drUpdate (); // aktualizacja fizyki i logiki gry
			accumulator.microseconds -= TIME_STEP.microseconds;
		}

		drDraw ();
		sfSleep (TIME_STEP); /// experimental
	}

	sfClock_destroy (lastUpdateTime);
	return 0;
}

// void drLog (char* text)