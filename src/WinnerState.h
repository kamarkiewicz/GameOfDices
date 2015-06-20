#ifndef WINNERSTATE_H
#define WINNERSTATE_H

#include "DryEngine/Engine.h"
#include "PLAY.h"
#include "Language.h"

extern drState WinnerState;

void WinnerState_init ();
void WinnerState_cleanup ();
void WinnerState_pause ();
void WinnerState_resume ();
void WinnerState_handleEvents ();
void WinnerState_update ();
void WinnerState_draw ();

#endif // WINNERSTATE_H