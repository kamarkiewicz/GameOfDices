#include "PLAY.h"

#include <stdio.h>

///////////////////////////////////////////////////////////
/// Private
void
PLAY_colorize (void)
{
	drListIter pchunkIt = drList_begin (PLAY.chunks);
	for (; !drListIter_isEnd (&pchunkIt); drListIter_next (&pchunkIt)){
		PLAY_Chunk *pchunk = drListIter_get (pchunkIt);
		if (pchunk->chunk == PLAY.attacker){
			pchunk->chunk->color = sfBlack;
		} else
		if (pchunk->chunk == PLAY.victim){
			pchunk->chunk->color = sfWhite;
		} else
			pchunk->chunk->color = PLAY.colors [pchunk->owner - 1];
	}
}


void
PLAY_divChunksBetweenPlayers (void)
{
	assert (PLAY.hexmap->chunks);
	assert (drList_empty (PLAY.chunks));
	drListIter chunkIt = drList_begin (PLAY.hexmap->chunks);
	for (; !drListIter_isEnd (&chunkIt); drListIter_next (&chunkIt)){
		PLAY_Chunk *pchunk = malloc (sizeof (PLAY_Chunk));
		drList_insert (PLAY.chunks, pchunk, 1);
		pchunk->owner = rand () % PLAY.players + 1;
		pchunk->chunk = drListIter_get (chunkIt);
	}
	PLAY_colorize ();
}


void
PLAY_divDicesBetweenPlayers (void)
{
	/// Need to transport for every chunk in PLAY.chunks
	/// some amount of dices. <1..6>
	drListIter pchunkIt = drList_begin (PLAY.chunks);	
	for (; !drListIter_isEnd (&pchunkIt); drListIter_next (&pchunkIt)){
		PLAY_Chunk *pchunk = drListIter_get (pchunkIt);
		pchunk->dices = rand () % 6 + 1;
		// printf ("PLAY.c:46 %d\n", pchunk->dices);
	}
}



int
PLAY_recalcTheBiggestOwnersChunk (int owner)
{
	///TODO: There should be maximum tree...
	///TODO: need to be faster?
	int q = drList_size (PLAY.chunks);
	int *checked = calloc (q + 1, sizeof (int)), n_checked = 0;

	while (q--)
	{

		drListIter pchunkIt = drList_begin (PLAY.chunks);	
		for (; !drListIter_isEnd (&pchunkIt); drListIter_next (&pchunkIt)){
			PLAY_Chunk *A = drListIter_get (pchunkIt);
			if(A->owner != owner) continue;

			if (! checked [A->chunk->id])
				checked [A->chunk->id] = A->chunk->id;

			drListIter nbIt = drList_begin (PLAY.chunks);
			for (; !drListIter_isEnd (&nbIt); drListIter_next (&nbIt))
			{
				PLAY_Chunk *B = drListIter_get (nbIt);
				if(B->owner != owner) continue;
				if (A == B) continue;

				if (! checked [B->chunk->id])
					checked [B->chunk->id] = B->chunk->id;

				int *a = & checked [A->chunk->id];
				int *b = & checked [B->chunk->id];
				if (drHexmap_areChunksNeighbours (A->chunk, B->chunk)){
					*a = *b = min (*a, *b);
					n_checked = max (A->chunk->id, B->chunk->id);
				}
			}
		}

	}


	/// Count
	int *counter = calloc (n_checked + 1, sizeof (int)), m_counter = 0, result = 0;
	for (int i = 0; i <= n_checked; ++i){
		if (checked [i]){
			counter [checked [i]]++;
			m_counter = max(m_counter, checked [i]);
		}
	}

	for (int i = 0; i <= m_counter; ++i){
		result = max(result, counter[i]);
	}


	free (checked);
	free (counter);
	return result;
}


void
PLAY_redistDices (void)
{
	/// Redist dices between current_player' fields
	int dc = PLAY_recalcTheBiggestOwnersChunk (PLAY.current_player);
	int dt = 0;

	bool working = true;
	while (dc > 0 && working){
		working = false;
		drListIter pchunkIt = drList_begin (PLAY.chunks);	
		for (; dc && !drListIter_isEnd (&pchunkIt); drListIter_next (&pchunkIt)){
			PLAY_Chunk *pchunk = drListIter_get (pchunkIt);
			if (pchunk->owner != PLAY.current_player) continue;
			if (pchunk->dices == 6) continue;

			dt = clamp (rand () % (6 - pchunk->dices + 1), 0, dc);
			if (dt != 0) {
				working = true;
				dc -= dt;
				pchunk->dices += dt;
			}
		}
	}
}


PLAY_Chunk*
PLAY_getByChunk (drHexmap_Chunk *chunk){
	/// Find chunk in PLAY.chunks
	PLAY_Chunk *pchunk;
	drListIter pchunkIt = drList_begin (PLAY.chunks);	
	for (; !drListIter_isEnd (&pchunkIt); drListIter_next (&pchunkIt)){
		pchunk = drListIter_get (pchunkIt);
		if (pchunk->chunk == chunk)
			return pchunk;
	}
	assert (!drListIter_isEnd (&pchunkIt));
	return NULL;
}


/**
 * Checks if attacker selected own field. If not, PLAY.attacker = NULL
 */
void PLAY_checkAttacker (void)
{
	PLAY_Chunk *pchunk = PLAY_getByChunk (PLAY.attacker);
	/// check if owner == current_player
	if (pchunk->owner != PLAY.current_player)
		PLAY.attacker = NULL;
	/// efford to attack?
	if (pchunk->dices == 1)
		PLAY.attacker = NULL;
	PLAY_colorize ();
}



void PLAY_checkVictim (void){
	PLAY_Chunk *pchunk = PLAY_getByChunk (PLAY.victim);

	/// If PLAY.victim belongs to current_player
	/// Change it to attacker
	if (pchunk->owner == PLAY.current_player){
		drHexmap_Chunk *old = PLAY.attacker;
		PLAY.attacker = PLAY.victim;
		PLAY_checkAttacker ();
		if (PLAY.attacker == NULL)
			PLAY.attacker = old;
		PLAY.victim = NULL;
		PLAY_colorize ();
		return;
	}

	/// Check if PLAY.victim is in PLAY.attacker's neighborhood
	if (!drHexmap_areChunksNeighbours (PLAY.attacker, PLAY.victim)){
		PLAY.victim = NULL;
		PLAY_colorize ();
		return;
	}
}


/// @return sum of dices
int
PLAY_roll (int dices, int *table, int n)
{
	int sum = 0;
	for (int i = 0; i < n; ++i)
		if (i < dices)
			sum += (table [i] = rand () % 6 + 1);
		else
			table [i] = 0;
	return sum;
}


void
PLAY_performAttack (void)
{
	/// At this moment all variables are set right.
	/// We need just loss some dices
	/// and store results in PLAY.
	PLAY_Chunk *pattacker = PLAY_getByChunk (PLAY.attacker);
	PLAY_Chunk *pvictim = PLAY_getByChunk (PLAY.victim);

	PLAY.atk = PLAY_roll (pattacker->dices, PLAY.atk_points, 6);
	PLAY.def = PLAY_roll (pvictim->dices, PLAY.def_points, 6);

	//  PLAY.atk += 50; /// ALWAYS WIN

	if (PLAY.atk > PLAY.def){ /// attacker wins!
		pvictim->owner = PLAY.current_player;
		pvictim->dices = pattacker->dices - 1;
	}
	pattacker->dices = 1;

	PLAY_colorize ();
}


/**
 * Checks if someone won
 * @return winner id. If no: 0.
 */
int
PLAY_TheChosenOne (void)
{
	int player = 0;
	PLAY_Chunk *pchunk;
	drListIter pchunkIt = drList_begin (PLAY.chunks);	
	for (; !drListIter_isEnd (&pchunkIt); drListIter_next (&pchunkIt)){
		pchunk = drListIter_get (pchunkIt);
		if (player == 0){
			player = pchunk->owner;
			continue;
		}
		if (player != pchunk->owner)
			return 0;
	}
	return player;
}

///////////////////////////////////////////////////////////
/// Public
void PLAY_init (void){
	PLAY.state = PLAY_BEGIN;

	sfColor colors [6] = { /// Need to be same as balls
		sfColor_fromRGBA (214, 158,  20, 255),    /// BALL 1
		sfColor_fromRGBA ( 64, 139, 193, 240),    /// BALL 2
		sfColor_fromRGBA (210,  35,  28, 240),    /// BALL 3
		sfColor_fromRGBA ( 79,  13, 100, 240),    /// BALL 4
		sfColor_fromRGBA (235,  86,   6, 240),    /// BALL 5
		sfColor_fromRGBA (  8,  91,  26, 240)     /// BALL 6
	};
	for (int i = 0; i < 6; ++i)
		PLAY.colors [i] = colors[i];
	PLAY.current_player = 0;
	PLAY.attacker = NULL;
	PLAY.victim = NULL;
	PLAY.chunks = drList_create (free); /// Will be destroyed at TheGameState.c:257
	PLAY.seed = time (NULL);
	/// PLAY.hexmap is filled by outside
#ifndef NDEBUG
	PLAY.seed = 42;
#endif
	srand (PLAY.seed);
	/// Divide chunks between players
	PLAY_divChunksBetweenPlayers ();
	/// and dices
	PLAY_divDicesBetweenPlayers ();
}




void
PLAY_performer (void)
{
	int winner;

	switch (PLAY.state)
	{
	case PLAY_BEGIN:
		PLAY.current_player = 1;
		PLAY.state = PLAY_PICK_ATTACKER;
		break;
	case PLAY_UPDATE_DICES: /// need to be changed by TheGameState
		break; 
	case PLAY_PICK_ATTACKER:
		/// Check if selected chunk belongs to current_playier
		if (PLAY.attacker == NULL) break;
		PLAY_checkAttacker ();
		if (PLAY.attacker == NULL) break;
		/// if all good
		PLAY.state = PLAY_PICK_VICTIM;
		break;
	case PLAY_PICK_VICTIM:
		/// Check if selected chunk belongs to someone in neighborhood
		if (PLAY.victim == NULL) break;
		PLAY_checkVictim ();
		if (PLAY.victim == NULL) break;
		/// if all good
		PLAY.state = PLAY_ATTACK;
		break;
	case PLAY_ATTACK:
		/// In this step we have checked the attacker and the victim.
		/// Roll the dices!
		PLAY_performAttack ();
		PLAY.state = PLAY_ATTACK_ANIMATION;
		break;
	case PLAY_ATTACK_ANIMATION:
		break;
	case PLAY_CHANGE_PLAYER:
		PLAY.attacker = PLAY.victim = NULL;
		PLAY_redistDices ();
		PLAY.current_player = PLAY.current_player % PLAY.players + 1;
		PLAY_colorize ();
		PLAY.state = PLAY_UPDATE_DICES;
		break;
	case PLAY_END: /// someone wins, someone loses...
		/// Check if one player has all the chunks
		/// Only then he wins!
		PLAY.attacker = NULL;
		PLAY.victim = NULL;
		PLAY_colorize ();

		winner = PLAY_TheChosenOne ();
		if (! winner){
			PLAY.state = PLAY_UPDATE_DICES;
		} else {
			PLAY.state = PLAY_WINNER;
			PLAY.current_player = winner;
		}
	case PLAY_WINNER:
		/// Player <winner> won!
		break;
	}
}


void
PLAY_performChunkSelect (
	drHexmap_Chunk *chunk
){
	assert (chunk != NULL);

	if (PLAY.state != PLAY_PICK_ATTACKER
	 && PLAY.state != PLAY_PICK_VICTIM)
		return;

	/// deselect attacker
	if (PLAY.attacker == chunk){
		PLAY.attacker = NULL;
		PLAY.state = PLAY_PICK_ATTACKER;
		PLAY_colorize ();
		return;
	}

	if (PLAY.state == PLAY_PICK_ATTACKER)
		PLAY.attacker = chunk;
	else
		PLAY.victim = chunk;
}


void
PLAY_endTurn (void)
{
	/// Share the dices
	/// Change player
	PLAY.state = PLAY_CHANGE_PLAYER;
}
