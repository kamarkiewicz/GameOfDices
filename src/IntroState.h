#ifndef INTROSTATE_H
#define INTROSTATE_H

#include "DryEngine/Engine.h"
#include "MainMenuState.h"

extern drState IntroState;

void IntroState_init ();
void IntroState_cleanup ();
void IntroState_pause ();
void IntroState_resume ();
void IntroState_handleEvents ();
void IntroState_update ();
void IntroState_draw ();

#endif