#ifndef PLAY_H
#define PLAY_H

#include "DryEngine/Hexmap.h"
#include "DryEngine/List.h"
#include <stdlib.h> // srand, rand
#include <time.h>
#include "TheGameState.h"

typedef enum PLAY_STATE {
	PLAY_BEGIN,
	PLAY_UPDATE_DICES,
	PLAY_PICK_ATTACKER,
	PLAY_PICK_VICTIM,
	PLAY_ATTACK,
	PLAY_ATTACK_ANIMATION,
	PLAY_CHANGE_PLAYER,
	PLAY_END, /// someone wins, someone loses...
	PLAY_WINNER
} PLAY_STATE;

typedef struct PLAY_Chunk {
	int owner;
	int dices;
	drHexmap_Chunk *chunk;
} PLAY_Chunk;

/* This structure contains the configuration of current game state. */
typedef struct PLAY_t {
	PLAY_STATE     state;
	int            players;
	sfColor        colors [6];
	int            current_player;
	drHexmap_Chunk *attacker;
	drHexmap_Chunk *victim;
	int            atk;
	int            atk_points [6];
	int            def;
	int            def_points [6];
	drHexmap       *hexmap;
	drList         *chunks; // chunk id => PLAY_Chunk (sfVector2i)
	                        // Add it to external ResManager
	int seed;
} PLAY_t;

PLAY_t PLAY;

void PLAY_init (void);

void PLAY_dices_share (void);

void PLAY_performer (void);
void PLAY_performChunkSelect (drHexmap_Chunk *chunk);
void PLAY_endTurn (void);

#endif