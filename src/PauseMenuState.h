#ifndef PAUSEMENUSTATE_H
#define PAUSEMENUSTATE_H

#include "DryEngine/Engine.h"

extern drState PauseMenuState;

void PauseMenuState_init ();
void PauseMenuState_cleanup ();
void PauseMenuState_pause ();
void PauseMenuState_resume ();
void PauseMenuState_handleEvents ();
void PauseMenuState_update ();
void PauseMenuState_draw ();

#endif // PAUSEMENUSTATE_H