#ifndef MAINMENUSTATE_H
#define MAINMENUSTATE_H

#include "DryEngine/Engine.h"
#include "Language.h"
#include "TheGameState.h"

extern drState MainMenuState;

void MainMenuState_init ();
void MainMenuState_cleanup ();
void MainMenuState_pause ();
void MainMenuState_resume ();
void MainMenuState_handleEvents ();
void MainMenuState_update ();
void MainMenuState_draw ();

#endif // MAINMENUSTATE_H