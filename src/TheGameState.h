#ifndef THEGAMESTATE_H
#define THEGAMESTATE_H

#include "DryEngine/Engine.h"
#include "PauseMenuState.h"
#include "WinnerState.h"
#include "PLAY.h"

extern drState TheGameState;

void TheGameState_init ();
void TheGameState_cleanup ();
void TheGameState_pause ();
void TheGameState_resume ();
void TheGameState_handleEvents ();
void TheGameState_update ();
void TheGameState_draw ();


#endif // THEGAMESTATE_H