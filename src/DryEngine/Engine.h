/**
 * @
 *
 */
#ifndef DRY_ENGINE_H
#define DRY_ENGINE_H

#include <assert.h>
#include <SFML/Audio.h>
#include <SFML/Graphics.h>
#include <SFML/System/Clock.h>
#include "Types.h"
#include "Stack.h"
#include "State.h"
#include "Hexmap.h"
#include "HexmapLoader.h"
#include "ResManager.h"
#include "Snippets.h"

extern const sfTime TIME_STEP, // framerate
                    MAX_ACCUMULATED_TIME;
sfRenderWindow*	drWindow;
sfEvent			drEvent;
sfFont*			drDefaultFont;

void drInit (void);
void drCleanup (void);

void drChangeState (drState* state);
void drPushState (drState* state);
void drPopState (void);

void drHandleEvents (void);
void drUpdate (void);
void drDraw (void);

int drMainLoop (void);

#endif // DRY_ENGINE_H