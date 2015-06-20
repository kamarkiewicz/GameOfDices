#include "TheGameState.h"
#include "MainMenuState.h"
#include "PLAY.h"

static drResManager *RM;
/// Drawable
static sfSprite *bggame;


///////////////////////////////////////////////////////////
/// DiceWar
static struct DiceWar_layer_t {
	bool      visible;
	bool      started;
	sfTexture *texture;
	sfClock   *clocker;
	sfSprite  *dice [12];
	sfText    *results [2];
	sfConvexShape   *darker;
} DiceWar_layer;


void
DiceWar_layer_init (void)
{
	static struct DiceWar_layer_t *dwl = &DiceWar_layer;
	dwl->visible = false;
	dwl->started = false;

	dwl->clocker = drResManager_myInsert (RM, sfClock_create (), DESTRUCTOR (sfClock_destroy));

	dwl->texture = drResManager_insert(RM, drResTexture,
		sfTexture_createFromFile ("res/warDices.png", NULL));
	
	for (int i = 0; i < 12; ++i){
		dwl->dice [i] = drResManager_insert(RM, drResSprite,
			sfSprite_create ());
		sfSprite_setTexture (dwl->dice [i], dwl->texture, sfTrue);
		sfSprite_setTextureRect (dwl->dice [i], (sfIntRect){0, 0, 64, 64});/// tmp
	}

	dwl->results [0] = load_text ();
	sfText_setColor (dwl->results [0], sfWhite);
	dwl->results [1] = load_text ();
	sfText_setColor (dwl->results [1], sfWhite);

	dwl->darker = drResManager_insert(RM, drResConvexShape,
		sfConvexShape_create ());
	sfConvexShape_setPointCount (dwl->darker, 4);
	sfConvexShape_setPoint (dwl->darker, 0, (sfVector2f){0.0, 0.0});
	sfConvexShape_setPoint (dwl->darker, 1, (sfVector2f){960.0, 0.0});
	sfConvexShape_setPoint (dwl->darker, 2, (sfVector2f){960.0, 540.0});
	sfConvexShape_setPoint (dwl->darker, 3, (sfVector2f){0.0, 540.0});
	sfConvexShape_setFillColor (dwl->darker, sfColor_fromRGBA (0,0,0,200));




	/// Positioning
	sfVector2f tmp = {150, 100};
	sfText_setPosition (dwl->results[0], tmp);
	tmp.x += 440;
	sfText_setPosition (dwl->results[1], tmp);

	sfVector2f dicePos = {100, 200};
	sfVector2f diceOffset [6] = {{0,0}, {100,0}, {200, 0}, {0, 100}, {100, 100}, {200, 100}};
	for (int i = 0; i < 12; ++i){
		if (i == 6) dicePos.x = 550;
		tmp.x = dicePos.x + diceOffset [i % 6].x;
		tmp.y = dicePos.y + diceOffset [i % 6].y;
		sfSprite_setPosition (dwl->dice [i], tmp);
	}
}

void Dices_updateFromPLAY (void);

void
DiceWar_layer_update (void)
{
	static struct DiceWar_layer_t *dwl = &DiceWar_layer;
	if (PLAY.state == PLAY_ATTACK_ANIMATION){
		if (dwl->started == false)
		{
			dwl->started = true;
			dwl->visible = true;
			sfClock_restart (dwl->clocker);
		}
	} else return;

	///TODO: Dices rolling animation here
	char res[7] = "";
	sprintf (res, "ATK %d", PLAY.atk);
	sfText_setString (dwl->results[0], res);
	sprintf (res, "DEF %d", PLAY.def);
	sfText_setString (dwl->results[1], res);

	for (int i = 0; i < 6; ++i)
		if (PLAY.atk_points [i])
			sfSprite_setTextureRect (dwl->dice [i],
				(sfIntRect){64 * (PLAY.atk_points [i] - 1), 0, 64, 64}
			);
		else
			sfSprite_setTextureRect (dwl->dice [i],
				(sfIntRect){0, 0, 0, 0}
			);
	for (int i = 0; i < 6; ++i)
		if (PLAY.def_points [i])
			sfSprite_setTextureRect (dwl->dice [i + 6],
				(sfIntRect){64 * (PLAY.def_points [i] - 1), 0, 64, 64}
			);
		else
			sfSprite_setTextureRect (dwl->dice [i + 6],
				(sfIntRect){0, 0, 0, 0}
			);


	sfTime timer = sfClock_getElapsedTime (dwl->clocker);
	if (timer.microseconds > 2000000){
		dwl->visible = false;
		dwl->started = false;
		PLAY.state = PLAY_END;
	    Dices_updateFromPLAY ();
	}
}


bool
DiceWar_layer_handleEvents (void)
{
	static struct DiceWar_layer_t *dwl = &DiceWar_layer;
	if (! dwl->visible) return false;

	return true;
}


void
DiceWar_layer_draw (void)
{
	static struct DiceWar_layer_t *dwl = &DiceWar_layer;
	if (! dwl->visible) return;

	sfRenderWindow_drawConvexShape (drWindow, dwl->darker, NULL);
	for (int i = 0; i < 12; ++i)
		sfRenderWindow_drawSprite (drWindow, dwl->dice [i], NULL);
	sfRenderWindow_drawText (drWindow, dwl->results [0], NULL);
	sfRenderWindow_drawText (drWindow, dwl->results [1], NULL);
}


///////////////////////////////////////////////////////////
/// HUD
static struct HUD_layer_t {
	bool visible;
	#define BALLS_AMOUNT 8
	sfSprite *ball [BALLS_AMOUNT];
	sfSprite *selector;
} HUD_layer;

void
HUD_layer_init (void)
{
	static struct HUD_layer_t *hl = &HUD_layer;
	hl->visible = true;

	/// Load selector
	hl->selector = load_sprite ("res/shadow.png");

	/// Creating sprites from balls
	sfTexture *texture = drResManager_insert (RM, drResTexture,
    	sfTexture_createFromFile ("res/balls.png", NULL));
	sfIntRect rect = {0.0, 0.0, 64.0, 64.0};
	for (int i = 0; i < BALLS_AMOUNT; ++i){
		hl->ball[i] = drResManager_insert (RM, drResSprite, sfSprite_create ());
		sfSprite_setTexture (hl->ball[i], texture, sfTrue);
		sfSprite_setTextureRect (hl->ball[i], rect);
		rect.left += 64.0;
	}

	/// Positioning
	sfVector2f pos = {4.0, 	15.0};
	for (int i = 0; i < BALLS_AMOUNT; ++i){
		if (i == 1)
			sfSprite_setPosition (hl->selector, pos);
		sfSprite_setPosition (hl->ball[i], pos);
		pos.y += 64.0 + 1.0;
	}
}


bool
HUD_layer_handleEvents (void)
{
	static struct HUD_layer_t *hl = &HUD_layer;
	if (! hl->visible) return false;

	sfFloatRect bounds;
	if (drEvent.type == sfEvtMouseButtonReleased)
	for (int i = 0; i < BALLS_AMOUNT; ++i){
		bounds = sfSprite_getGlobalBounds (hl->ball [i]);
		if (sfFloatRect_contains (&bounds,
			drEvent.mouseButton.x,
			drEvent.mouseButton.y)
		)
		switch (i){
		case 0: /// Pause button clicked
			drPopState ();
			return true;
		case 7: /// End turn clicked
			PLAY_endTurn ();
			return true;
		default:
			return true;
		}
	}
	return false;
}



void
HUD_layer_draw (void)
{
	static struct HUD_layer_t *hl = &HUD_layer;
	if (! hl->visible) return;
	for (int i = 0; i < BALLS_AMOUNT; ++i){
		if (i == 2 && PLAY.players < 2) continue;
		if (i == 3 && PLAY.players < 3) continue;
		if (i == 4 && PLAY.players < 4) continue;
		if (i == 5 && PLAY.players < 5) continue;
		if (i == 6 && PLAY.players < 6) continue;
		sfRenderWindow_drawSprite (drWindow, hl->ball[i], NULL);
	}

	if (PLAY.current_player){
		sfVector2f pos = {-20.0, 15.0};
		pos.y += 65.0 * PLAY.current_player;
		sfSprite_setPosition (hl->selector, pos);
		sfRenderWindow_drawSprite (drWindow, hl->selector, NULL);
	}
}



///////////////////////////////////////////////////////////
/// Hexmap_layer
typedef struct dices_i {
	drHexmap_Chunk *chunk;
	sfSprite       *sprite;
	int            amount;
} dices_i;

static struct Hexmap_layer_t {
	bool		    visible;
	drHexmap 	    *map;
	sfTexture       *dicesTexture;
	drList          *dices; /// of dices_i
} Hexmap_layer;


void Dice_spriteUpdate (dices_i* self)
{
	assert (self);
	sfVector2i pos = *(sfVector2i*) drList_back (self->chunk->tiles);
	/// in pos is axial position of first hex in chunk
	sfVector2f pix = drHexmap_Tile_getPixelOffset (self->chunk->hexmap, pos);
	sfSprite_setPosition (self->sprite, pix);
	assert (self->amount > 0);
	sfIntRect rect = {
		(self->amount - 1) * 40.0, // left
		0.0,  // top
		40.0, // width
		40.0  // height
	};
	/// self->amount should be settled
	assert (self->amount);
	assert (self->sprite);
	sfSprite_setTextureRect (self->sprite, rect);
	sfSprite_setOrigin (self->sprite, (sfVector2f){20.0, 30.0});
}


void
Dices_updateFromPLAY (void)
{
	struct Hexmap_layer_t *hml = &Hexmap_layer;
	/// Here we have PLAY.chunks fullfiled and need to transport its data to hml->dices
	assert (!drList_empty (hml->dices));
	assert (!drList_empty (PLAY.chunks));
	drListIter dicesIt = drList_begin (hml->dices);	
	for (; !drListIter_isEnd (&dicesIt); drListIter_next (&dicesIt))
	{
		dices_i *dices = drListIter_get (dicesIt);
		drListIter pchunksIt = drList_begin (PLAY.chunks);
		for (; !drListIter_isEnd (&pchunksIt); drListIter_next (&pchunksIt))
		{
			PLAY_Chunk *pchunks = drListIter_get (pchunksIt);
			if (dices->chunk == pchunks->chunk)
			{
				dices->amount = pchunks->dices;
				// printf ("TheGameState.c:156 %d %d\n", pchunks->owner, pchunks->dices);
				/// Update sprite
				Dice_spriteUpdate (dices);
			}
		}
	}
}


void
Hexmap_layer_init (void)
{
	struct Hexmap_layer_t *hml = &Hexmap_layer;
	hml->visible = true;

	/// Initialize the map
	hml->map = drResManager_myInsert (RM, 
		drHexmap_create (), DESTRUCTOR (drHexmap_destroy));
    drHexmap_setHexSizes (hml->map, 28.75, M_PI / 6.0);
    hml->map->bounds = (sfFloatRect){75.0, 8.5, 890.0, 523.0};
    drHexmapLoader_loadFromFile (hml->map, "res/map_generator.lua");

    /// Load dices
    hml->dicesTexture = drResManager_insert(RM, drResTexture,
    	sfTexture_createFromFile ("res/smallDices.png", NULL));

    hml->dices = drResManager_myInsert(RM,
    	drList_create (free), DESTRUCTOR (drList_destroy));

    dices_i *dice;
    drListIter mapIt = drList_begin (hml->map->chunks);
    for (; !drListIter_isEnd (&mapIt); drListIter_next (&mapIt)){
    	dice = malloc (sizeof (dices_i));
    	assert (dice);
    	drList_insert (hml->dices, dice, true);
    	dice->chunk = drListIter_get (mapIt);
    	dice->sprite = drResManager_insert (RM, drResSprite,
    		sfSprite_create ());
    	sfSprite_setTexture (dice->sprite, hml->dicesTexture, sfTrue);
    	dice->amount = 0;
    }
}


bool
Hexmap_layer_handleEvents (void)
{
	struct Hexmap_layer_t *hml = &Hexmap_layer;
	if (! hml->visible) return false;

	if (drEvent.type == sfEvtMouseButtonReleased){
		sfVector2i selected;
		selected.x = drEvent.mouseButton.x;
		selected.y = drEvent.mouseButton.y;
		/// Pick field by mouse
		drHexmap_Chunk* chunk;
		chunk = drHexmap_Chunk_getByPoint (hml->map, selected);
		if (chunk == NULL) return false;
		
		PLAY_performChunkSelect (chunk);


		return true;
	}

	return false;
}


void
Hexmap_layer_update ()
{
	struct Hexmap_layer_t *hml = &Hexmap_layer;
	if (! hml->visible) return;

	if (PLAY.state == PLAY_UPDATE_DICES){
		Dices_updateFromPLAY ();
		PLAY.state = PLAY_PICK_ATTACKER;
	}
}


void
Hexmap_layer_draw (void)
{
	struct Hexmap_layer_t *hml = &Hexmap_layer;
	if (! hml->visible) return;
	drRenderWindow_drawHexmap (drWindow, hml->map);

	drListIter dicesIt = drList_begin (hml->dices);
	for (; !drListIter_isEnd (&dicesIt); drListIter_next (&dicesIt))
	{
		dices_i *dice = drListIter_get (dicesIt);
		sfRenderWindow_drawSprite (drWindow, dice->sprite, NULL);
	}
}






///////////////////////////////////////////////////////////
/// TheGameState
void
TheGameState_init (void)
{
	RM = drResManager_create ();
    // Background
    bggame = load_sprite ("res/bggame.png");

    HUD_layer_init ();
    Hexmap_layer_init ();
    DiceWar_layer_init ();

    PLAY.hexmap = Hexmap_layer.map;
    PLAY_init ();
    drResManager_myInsert (RM, PLAY.chunks, DESTRUCTOR(drList_destroy));

    Dices_updateFromPLAY ();
}


void
TheGameState_cleanup (void)
{
	drResManager_destroy (RM);
}



void
TheGameState_pause (void)
{
}


void
TheGameState_resume (void)
{
}


void
TheGameState_handleEvents (void)
{
	/* Process events */
	while (sfRenderWindow_pollEvent (drWindow, &drEvent))
	{
		/* Close window : exit */
		if (drEvent.type == sfEvtClosed)
			sfRenderWindow_close (drWindow);

		if (DiceWar_layer_handleEvents ()) continue;
		/* HUD */
		if (HUD_layer_handleEvents ()) continue;
		/* Map */
		if (Hexmap_layer_handleEvents ()) continue;
	}
}


void
TheGameState_update (void)
{
	PLAY_performer ();
	DiceWar_layer_update ();
	if (PLAY.state == PLAY_WINNER)
		drChangeState (&WinnerState);
	Hexmap_layer_update ();
}


void
TheGameState_draw (void)
{
	// draw background
	sfRenderWindow_drawSprite (drWindow, bggame, NULL);
	// draw map and dices
	Hexmap_layer_draw ();
	// draw HUD
	HUD_layer_draw ();
	// draw war!
	DiceWar_layer_draw ();
}


drState TheGameState = {
	TheGameState_init,
	TheGameState_cleanup,
	TheGameState_pause,
	TheGameState_resume,
	TheGameState_handleEvents,
	TheGameState_update,
	TheGameState_draw
};
