// 3D_GAME.C

#include "3d_def.h"


#define LOCATION_TEXT_COLOR 0xaf
#define ATABLEMAX           15

#ifdef IN_DEVELOPMENT
int             db_count;
#ifdef DEBUG_STATICS
byte            debug_bonus[2][800];
#endif
#endif
#ifdef TRACK_ENEMY_COUNT
int16_t         numenemy[gold_morphingobj];
#endif

unsigned        ceilingcolor,floorcolor;

int             viewsize;
int             viewscreenx,viewscreeny;
int             baseviewscreenx,baseviewscreeny;
int             fizzlewidth,fizzleheight;
int             fizzlex,fizzley;
unsigned        viewscreenofs;

fargametype     gamestuff;
gametype        gamestate;

int             LS_current = -1,LS_total = -1;

int             mapwidth,mapheight;

bool            startgame,ingame,loadedgame;

eaWallInfo      eaList[MAXEAWALLS];
int             NumEAWalls;

tilecoord_t     GoldieList[GOLDIE_MAX_SPAWNS];
GoldsternInfo_t GoldsternInfo;

#if GAME_VERSION == SHAREWARE_VERSION
char            demoname[13] = "DEMO?S.";
#else
char            demoname[13] = "DEMO?.";
#endif

int             leftchannel,rightchannel;

byte righttable[ATABLEMAX][ATABLEMAX * 2] =
{
    { 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 6, 0, 0, 0, 0, 0, 1, 3, 5, 8, 8, 8, 8, 8, 8, 8, 8},
    { 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 7, 6, 4, 0, 0, 0, 0, 0, 2, 4, 6, 8, 8, 8, 8, 8, 8, 8, 8},
    { 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 4, 1, 0, 0, 0, 1, 2, 4, 6, 8, 8, 8, 8, 8, 8, 8, 8},
    { 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 6, 5, 4, 2, 1, 0, 1, 2, 3, 5, 7, 8, 8, 8, 8, 8, 8, 8, 8},
    { 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 6, 5, 4, 3, 2, 2, 3, 3, 5, 6, 8, 8, 8, 8, 8, 8, 8, 8, 8},
    { 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 6, 6, 5, 4, 4, 4, 4, 5, 6, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8},
    { 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 6, 6, 5, 5, 5, 6, 6, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
    { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 6, 6, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
    { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
    { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
    { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
    { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
    { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
    { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
    { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
};

byte lefttable[ATABLEMAX][ATABLEMAX * 2] =
{
    { 8, 8, 8, 8, 8, 8, 8, 8, 5, 3, 1, 0, 0, 0, 0, 0, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8},
    { 8, 8, 8, 8, 8, 8, 8, 8, 6, 4, 2, 0, 0, 0, 0, 0, 4, 6, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8},
    { 8, 8, 8, 8, 8, 8, 8, 8, 6, 4, 2, 1, 0, 0, 0, 1, 4, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8},
    { 8, 8, 8, 8, 8, 8, 8, 8, 7, 5, 3, 2, 1, 0, 1, 2, 4, 5, 6, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8},
    { 8, 8, 8, 8, 8, 8, 8, 8, 8, 6, 5, 3, 3, 2, 2, 3, 4, 5, 6, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8},
    { 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 6, 5, 4, 4, 4, 4, 5, 6, 6, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8},
    { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 6, 6, 5, 5, 5, 6, 6, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8},
    { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 6, 6, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
    { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
    { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
    { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
    { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
    { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
    { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
    { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
};


//
// NOTE: This array indexs the "statinfo" array in ACT1.C and is indexed
//     upon tile number/values.
//
int ExpCrateShapes[] =
{
    42,         // Chicken Leg
    44,         // Ham/Steak
    26,         // Clip
    24,         // Pistol
    27,         // Pulse
    28,         // ION
    46,         // Grenade
    62,         // Money Bag
    63,         // Loot
    64,         // Gold
    65,         // Bonus
    71,         // Gore 1
    74,         // Gore 2
    32,         // red key
    33,         // yel key
    52 - 23,    // grn key
    35,         // blu key
    488 - 375,  // gld key
};


void    ScanInfoPlane (void);
void    SetupGameLevel (void);
void    DrawPlayScreen (bool InitInfoMsg);
void    Died (void);
void    GameLoop (void);


/*
==========================
=
= SetSoundLoc
=
= Given the location of an object (in global coordinates), munges
= the values for an approximate distance from the left and right
= ear, and puts those values into leftchannel and rightchannel.
=
==========================
*/

void SetSoundLoc (fixed gx, fixed gy)
{
    fixed gxt,gyt;
    int   nx,ny;

//
// translate point to view centered coordinates
//
    gx -= viewx;
    gy -= viewy;

//
// calculate nx
//
    gxt = FixedMul(gx,viewcos);
    gyt = FixedMul(gy,viewsin);
    nx = (gxt - gyt) >> TILESHIFT;

//
// calculate ny
//
    gxt = FixedMul(gx,viewsin);
    gyt = FixedMul(gy,viewcos);
    ny = (gyt + gxt) >> TILESHIFT;

    if (nx < 0)
        nx = -nx;
    if (nx >= ATABLEMAX)
        nx = ATABLEMAX - 1;

    if (ny >= ATABLEMAX)
        ny = ATABLEMAX - 1;
    else if (ny < -ATABLEMAX)
        ny = -ATABLEMAX;

    leftchannel = lefttable[nx][ny + ATABLEMAX];
    rightchannel = righttable[nx][ny + ATABLEMAX];
}


/*
==========================
=
= PlaySoundLocGlobal
=
==========================
*/

void PlaySoundLocGlobal (int sound, fixed gx, fixed gy)
{
    int channel;

    SetSoundLoc (gx,gy);
    SD_PositionSound (leftchannel,rightchannel);

    channel = SD_PlaySound(sound);

    if (channel)
    {
        //
        // TODO: why - 1?
        //
        channelSoundPos[channel - 1].x = gx;
        channelSoundPos[channel - 1].y = gy;
        channelSoundPos[channel - 1].valid = true;
    }
}


/*
==========================
=
= UpdateSoundLoc
=
= Called from PlayLoop
=
==========================
*/

void UpdateSoundLoc (void)
{
    int i;

    for (i = 0; i < MIX_CHANNELS; i++)
    {
        if (channelSoundPos[i].valid)
        {
            SetSoundLoc (channelSoundPos[i].x,channelSoundPos[i].y);
            SD_SetPosition (i,leftchannel,rightchannel);
        }
    }
}


/*
=====================
=
= NewGame
=
= Set up new game to start from the beginning
=
=====================
*/

void NewGame (int difficulty, int episode)
{
    int      i;
    unsigned oldflags = gamestate.flags;

    InitPlaytemp ();
    playstate = ex_stillplaying;

    ShowQuickMsg = true;

    memset (&gamestuff,0,sizeof(gamestuff));
    memset (&gamestate,0,sizeof(gamestate));
    memset (&gamestate.barrier_table,0xff,sizeof(gamestate.barrier_table));
    memset (&gamestate.old_barrier_table,0xff,sizeof(gamestate.old_barrier_table));

    gamestate.flags = oldflags & ~(GS_KILL_INF_WARN);

    gamestate.difficulty = difficulty;

    gamestate.weapons  = 1 << wp_autocharge;
    gamestate.weapon = gamestate.chosenweapon = wp_autocharge;
    gamestate.old_weapons[0] = gamestate.weapons;
    gamestate.old_weapons[1] = gamestate.weapon;
    gamestate.old_weapons[2] = gamestate.chosenweapon;

    gamestate.health = 100;
    gamestate.old_ammo = gamestate.ammo = STARTAMMO;
    gamestate.lives = 3;
    gamestate.nextextra = EXTRAPOINTS;
    gamestate.episode = episode;
    gamestate.flags |= (GS_CLIP_WALLS | GS_ATTACK_INFOAREA);

#if defined(IN_DEVELOPMENT) || defined(TECH_SUPPORT_VERSION)
    if (gamestate.flags & GS_STARTLEVEL)
    {
        gamestate.mapon = starting_level;
        gamestate.difficulty = starting_difficulty;
        gamestate.episode = starting_episode;
    }
    else
#endif
        gamestate.mapon = 0;

    gamestate.key_floor = gamestate.mapon + 1;
    startgame = true;

    for (i = 0; i < MAPS_WITH_STATS; i++)
    {
        gamestuff.old_levelinfo[i].stats.overall_floor = 100;

        if (i)
            gamestuff.old_levelinfo[i].locked = true;
    }

    ExtraRadarFlags = InstantWin = InstantQuit = 0;

    pickquick = 0;
}


/*
==========================
=
= ScanInfoPlane
=
= Spawn all actors and mark down special places
=
==========================
*/

#define INVALID_ACTOR_ERR Quit("Invalid actor: %d %d",x,y)

void ScanInfoPlane (void)
{
    int            x,y;
    int            obclass;
    unsigned       tile;
    unsigned       tilehi,tilelo,block;
    word           *start;
    sci_mCacheInfo *ci;
    scientist_t    *st;
    objtype        *newobj;
    bool           gottextures = false;
#ifdef CEILING_FLOOR_COLORS
    bool           gotcolors = false;
#endif
#ifdef IN_DEVELOPMENT
    db_count = 0;
#endif
    detonators_spawned = 0;

#ifdef TRACK_ENEMY_COUNT
    memset (numenemy,0,sizeof(numenemy));
#endif
    start = mapsegs[1];

    for (y = 0; y < mapheight; y++)
    {
        for (x = 0; x < mapwidth; x++)
        {
            newobj = NULL;
            tile = *start++;

            //
            // check for tiles/icons to ignore
            //
            switch (MAPSPOT(x,y,0))
            {
                case DOORTRIGGERTILE:
                case SMART_OFF_TRIGGER:
                case SMART_ON_TRIGGER:
                    continue;
            }

            tilehi = tile >> 8;
            tilelo = tile & 0xff;

            if ((*start & 0xff00) == 0xfa00)
                scanvalue = *start & 0x00ff;
            else
                scanvalue = 0xffff;

            switch (tilehi)
            {
                //
                // special background
                //
                //case 0xff:
                    //continue;

#ifdef CEILING_FLOOR_COLORS
                //
                // Top/Bottom colors
                //
                case 0xfe:
                    if (gotcolors)
                        break;

                    x++;
                    tile = *start++;
                    ceilingcolor = tile >> 8;
                    floorcolor = tile & 0xff;
                    gotcolors = true;
                    continue;
#else
                case 0xfe:
                    x++;
                    tile = *start++;
                    continue;
#endif
                //
                // global Ceiling/Floor textures
                //
                case 0xfb:
                    if (gottextures)
                        break;

                    x++;
                    tile = *start++;

                    ceilingpic = START_TEXTURES + (tile >> 8);

                    if (ceilingpic > NUM_TILES - 1)
                        Quit ("Ceiling texture is out of range!");

                    floorpic = START_TEXTURES + (tile & 0xff);

                    if (floorpic > NUM_TILES - 1)
                        Quit ("Floor texture is out of range!");

                    gottextures = true;
                    continue;

                //
                // intralevel warp
                //
                case 0xf5:
                    *(start - 1) = *start;     // move coord right on top
                    *start = 0;
                    continue;
#if 0
                //
                // lighting effects
                //
                case 0xf6:
                    if (gotlight)
                        break;

                    x++;
                    tile = *start++;
                    normalshadediv = tile >> 8;

                    if (normalshadediv > 12)
                        Quit ("Shade divisor too large! Must be 1-12!");

                    shademax = tile & 0xff;

                    if (shademax < 5 || shademax > SHADEMAX)
                        Quit ("Shade max value out of range! Must be 5-63!");

                    normalshade = (((viewwidth * 9) >> 4) - 3) / normalshadediv;
                    gotlight = true;
                    continue;
#endif
                case 0xfa:
                    continue;

                //
                // informant messages
                //
                case 0xf1:
                case 0xf2:  // "Nice" scientist messages
                case 0xf3:  // "Mean" scientist messages
                    switch (tilehi)
                    {
                        case 0xf1:
                            block = INFORMANT_HINTS;
                            st = &InfHintList;
                            break;

                        case 0xf2:
                            block = NICE_SCIE_HINTS;
                            st = &NiceSciList;
                            break;

                        case 0xf3:
                            block = MEAN_SCIE_HINTS;
                            st = &MeanSciList;
                            break;
                    }

                    ci = &st->smInfo[st->NumMsgs];
                    ci->mInfo.local_val = 0xff;
                    ci->mInfo.global_val = tilelo;

                    if (!ReuseMsg((mCacheInfo *)ci,st->NumMsgs,sizeof(sci_mCacheInfo)))
                    {
                        CacheMsg ((mCacheInfo *)ci,block,ci->mInfo.global_val);
                        ci->mInfo.local_val = InfHintList.NumMsgs;
                    }

                    if (++st->NumMsgs > MAX_CACHE_MSGS)
                        Quit ("Too many 'cached msgs' loaded! (INFORMANTS)");

                    ci->areanumber = GetAreaNumber(x,y);

                    if (ci->areanumber >= NUMAREAS)
                        ci->areanumber = 0xff;

                    continue;

                case 0:
                    if (!tilelo)
                        continue;
                    break;
            }

            //
            // special spawn coding for blastable crates
            //
            if (tile >= 432 && tile <= 485)
            {
#if GAME_VERSION != SHAREWARE_VERSION
                if (tile >= 468)
                {
                    newobj = SpawnOffsetObj(en_crate3,x,y);

                    newobj->temp2 = ExpCrateShapes[tile - 468];
                    newobj->temp3 = STATICNUM(ReserveStatic());

                    if (tile >= 475 && tile <= 478)
                        tile = (tile - 475) + bo_money_bag;
                    else
                        tile = 0;
                }
                else if (tile >= 450)
                {
                    newobj = SpawnOffsetObj(en_crate2,x,y);

                    newobj->temp2 = ExpCrateShapes[tile - 450];
                    newobj->temp3 = STATICNUM(ReserveStatic());

                    if (tile >= 457 && tile <= 460)
                        tile = (tile - 457) + bo_money_bag;
                    else
                        tile = 0;
                }
                else
#if GAME_VERSION == SHAREWARE_VERSION
#ifdef IN_DEVELOPMENT
                     if (tile >= 450)
                    INVALID_ACTOR_ERR;
                else
#endif
#endif
                     if (tile >= 432)
                {
                    newobj = SpawnOffsetObj(en_crate1,x,y);

                    newobj->temp2 = ExpCrateShapes[tile - 432];
                    newobj->temp3 = STATICNUM(ReserveStatic());

                    if (tile >= 439 && tile <= 442)
                        tile = (tile - 439) + bo_money_bag;
                    else
                        tile = 0;
                }

                if (tile)
                {
                    if (tile > bo_loot)
                        tile += 3;

                    tile -= bo_money_bag;

                    AddTotalPoints (static_points[tile]);
#ifdef IN_DEVELOPMENT
#ifdef DEBUG_STATICS
                    debug_bonus[0][db_count++] = static_points[tile];
#endif
#endif
                }

                continue;
            }
#endif

            switch (tile)
            {
                case 19:
                case 20:
                case 21:
                case 22:
                    SpawnPlayer (x,y,(NORTH + tile) - 19);
                    break;

                case 85:     // Money bag
                case 86:     // Loot
                case 87:     // Gold
                case 88:     // Bonus
                    AddTotalPoints (static_points[statinfo[tile - 23].type-bo_money_bag]);
#ifdef IN_DEVELOPMENT
#ifdef DEBUG_STATICS
                    debug_bonus[0][db_count++] = static_points[statinfo[tile - 23].type - bo_money_bag];
#endif
#endif
                case 23:
                case 24:
                case 25:
                case 26:
                case 27:
                case 28:
                case 29:
#if GAME_VERSION != SHAREWARE_VERSION
                case 30:     // Yellow Puddle
#endif
                case 31:
                case 32:
                case 33:
                case 34:
                case 35:
                case 36:
                case 37:
                case 38:

                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:

                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:

                case 55:
                case 56:
                case 57:
                case 58:
                case 59:
                case 60:
                case 61:
                case 62:

                case 63:
                case 64:
                case 65:
                case 66:
                case 67:
                case 68:
                case 69:
                case 70:
                case 71:     // BFG Weapon
                case 72:     // Gurney Mutant
                case 73:     // Large Canister
                case 74:     // Small Canister
                case 75:     // Empty Gurney
                case 76:     // Empty Large Canister
                case 77:     // Empty Small Canister
                case 78:     // Dead Gen. Sci.

                case 80:
                case 83:     // Floor Grate
                case 84:     // Floor Pipe
                    SpawnStatic (x,y,tile - 23);
                    break;

                case 399:    // gold 1
                case 400:    // gold 2
                case 401:    // gold 3
                    AddTotalPoints (static_points[statinfo[tile - 315].type - bo_money_bag]);
#ifdef IN_DEVELOPMENT
#ifdef DEBUG_STATICS
                    debug_bonus[0][db_count++] = static_points[statinfo[tile - 315].type - bo_money_bag];
#endif
#endif
                case 381:
                case 382:
                case 383:
                case 384:
                case 385:
                case 386:
                case 387:
                case 388:
                //case 389:
                case 390:    // candy bar
                case 391:    // sandwich

                case 395:    // Table
                case 396:    // Chair
                case 397:    // Stool
                case 398:    // Gore

                case 402:
                case 403:
                case 404:
                case 405:
                case 406:
                case 407:
                case 408:
                case 409:
                case 410:
                case 411:
                case 412:
                case 413:
                case 414:
                case 415:
                case 416:
                case 417:
                case 418:
                case 419:
                case 420:
                case 421:
                case 422:
                case 423:    // bo_coin
                case 424:    // bo_coin5
                    SpawnStatic (x,y,tile - 315);
                    break;

                case 486:    // Plasma Detonator
                    SpawnHiddenOfs (en_plasma_detonator_reserve,x,y);
                    SpawnStatic (x,y,486 - 375);
                    break;

                case 487:    // Door rubble
                case 488:    // AutoMapper Bonus #1
                case 489:    // BonziTree
                case 490:    // Yellow Potted plant
                case 491:    // Tube Plant
                case 492:    // HiTech Chair
                case 493:    // AOG: Rent A Cop - Dead
                case 494:    // AOG: Pro Guard - Dead
                case 495:    // AOG: Swat Guard - Dead
                    SpawnStatic (x,y,tile-375);
                    break;

                case 393:    // crate 2
                case 394:    // crate 3
#if GAME_VERSION == SHAREWARE_VERSION
#ifdef IN_DEVELOPMENT
                    INVALID_ACTOR_ERR;
#endif
#endif
                case 392:    // crate 1
                    SpawnStatic (x,y,tile - 315);
                    break;

                case 81:
                case 82:
                    SpawnOffsetObj (en_bloodvent + tile - 81,x,y);
                    break;

                //
                // Green ooze
                //
                case 208:
                    if (gamestate.difficulty < gd_hard)
                        break;
                case 207:
                    if (gamestate.difficulty < gd_medium)
                        break;
                case 206:
                    SpawnOffsetObj (en_green_ooze,x,y);
                    break;

                //
                // Black ooze
                //
                case 212:
                    if (gamestate.difficulty < gd_hard)
                        break;
                case 211:
                    if (gamestate.difficulty < gd_medium)
                        break;
                case 210:
                    SpawnOffsetObj (en_black_ooze,x,y);
                    break;

                //
                // Flickering light
                //
                case 79:
                    newobj = SpawnOffsetObj(en_flickerlight,x,y);
                    newobj->lighting = LAMP_ON_SHADING;
                    break;

                //
                // Arc barrier
                //
                case 174:    // off
                case 175:    // on
                    SpawnBarrier (en_post_barrier,x,y,tile - 174);
                    break;

                //
                // Plasma post barrier
                //
                case 138:    // off
                case 139:    // on
                    SpawnBarrier (en_arc_barrier,x,y,tile - 138);
                    break;

                //
                // VPOST barriers
                //
                // Switchable
                //
                case 562:    // off
                case 563:    // on
                    SpawnBarrier (en_vpost_barrier,x,y,tile - 562);
                    break;

                //
                // Cycle
                //
                case 567:
                    if (gamestate.difficulty < gd_hard)
                        break;
                case 566:
                    if (gamestate.difficulty < gd_medium)
                        break;
                case 565:
                    SpawnBarrier (en_vpost_barrier,x,y,false);
                    break;

                //
                // VSPIKE barriers
                //
                // Switchable
                //
                case 425: // off
                case 426: // on
                    SpawnBarrier (en_vspike_barrier,x,y,tile - 425);
                    break;

                //
                // Cycle
                //
                case 430:
                    if (gamestate.difficulty < gd_hard)
                        break;
                case 429:
                    if (gamestate.difficulty < gd_medium)
                        break;
                case 428:
                    SpawnBarrier (en_vspike_barrier,x,y,false);
                    break;

                //
                // Steam grate
                //
                case 178:
                    SpawnStand (en_steamgrate,x,y,0);
                    break;

                //
                // Steam pipe
                //
                case 179:
                    SpawnStand (en_steampipe,x,y,0);
                    break;

                //
                // Goldfire spawn sites
                //
                case 124:
                    if (!loadedgame)
                    {
                        if (GoldsternInfo.SpawnCnt >= GOLDIE_MAX_SPAWNS)
                            Quit ("Too many Dr. Goldfire spawn sites in level!");

                        GoldsternInfo.flags = GS_FIRSTTIME;

                        if (gamestate.mapon == 9)    // TODO: should this have been GOLD_MORPH_LEVEL?
                            GoldsternInfo.WaitTime = 60;
                        else
                            GoldsternInfo.WaitTime = MIN_GOLDIE_FIRST_WAIT + Random(MAX_GOLDIE_FIRST_WAIT - MIN_GOLDIE_FIRST_WAIT);

                        GoldieList[GoldsternInfo.SpawnCnt].tilex = x;
                        GoldieList[GoldsternInfo.SpawnCnt].tiley = y;
                        GoldsternInfo.SpawnCnt++;

                        if (gamestate.mapon == GOLD_MORPH_LEVEL)
                        {
                            AddTotalPoints (actor_points[goldsternobj - rentacopobj]);
                            AddTotalEnemy (1);
#ifdef TRACK_ENEMY_COUNT
                            numenemy[goldsternobj]++;
#endif
                        }
                    }
                    break;

                //
                // Goldfire spawn - immediately
                //
                case 141:
                    if (!loadedgame)
                    {
                        if (GoldsternInfo.GoldSpawned)
                            Quit ("Too many fast Goldfire spawn sites in map!");

                        if (GoldsternInfo.SpawnCnt >= GOLDIE_MAX_SPAWNS)
                            Quit ("Too many Dr. Goldfire spawn sites in level!");

                        GoldieList[GoldsternInfo.SpawnCnt].tilex = x;
                        GoldieList[GoldsternInfo.SpawnCnt].tiley = y;

                        GoldsternInfo.LastIndex = GoldsternInfo.SpawnCnt++;
                        GoldsternInfo.flags = GS_COORDFOUND;

                        SpawnStand (en_goldstern,x,y,0);
                        GoldsternInfo.GoldSpawned = true;
                    }
                    break;

                //
                // Security light
                //
                case 160:
                    SpawnOffsetObj (en_security_light,x,y);
                    break;

                //
                // Rotating cube
                //
                case 177:
                    SpawnOffsetObj (en_rotating_cube,x,y);
                    break;

                //
                // Rent-a-cop
                //
                case 180:
                case 181:
                case 182:
                case 183:
                    if (gamestate.difficulty < gd_hard)
                        break;
                    tile -= 36;
                case 144:
                case 145:
                case 146:
                case 147:
                    if (gamestate.difficulty < gd_medium)
                        break;
                    tile -= 36;
                case 108:
                case 109:
                case 110:
                case 111:
                    newobj = SpawnStand(en_rentacop,x,y,tile - 108);
                    break;

                case 184:
                case 185:
                case 186:
                case 187:
                    if (gamestate.difficulty < gd_hard)
                        break;
                    tile -= 36;
                case 148:
                case 149:
                case 150:
                case 151:
                    if (gamestate.difficulty < gd_medium)
                        break;
                    tile -= 36;
                case 112:
                case 113:
                case 114:
                case 115:
                    newobj = SpawnPatrol(en_rentacop,x,y,tile - 112);
                    break;

                //
                // Bio-tech
                //
                case 188:
                case 189:
                case 190:
                case 191:
                    if (gamestate.difficulty < gd_hard)
                        break;
                    tile -= 36;
                case 152:
                case 153:
                case 154:
                case 155:
                    if (gamestate.difficulty < gd_medium)
                        break;
                    tile -= 36;
                case 116:
                case 117:
                case 118:
                case 119:
                    newobj = SpawnStand(en_gen_scientist,x,y,tile - 116);

                    if (newobj->flags & FL_INFORMANT)
                    {
                        AddTotalInformants (1);
                        newobj = NULL;
                    }
                    break;

                case 192:
                case 193:
                case 194:
                case 195:
                    if (gamestate.difficulty < gd_hard)
                        break;
                    tile -= 36;
                case 156:
                case 157:
                case 158:
                case 159:
                    if (gamestate.difficulty < gd_medium)
                        break;
                    tile -= 36;
                case 120:
                case 121:
                case 122:
                case 123:
                    newobj = SpawnPatrol(en_gen_scientist,x,y,tile - 120);

                    if (newobj->flags & FL_INFORMANT)
                    {
                        AddTotalInformants (1);
                        newobj = NULL;
                    }
                    break;

                //
                // PRO Guard
                //
                case 198:
                case 199:
                case 200:
                case 201:
                    if (gamestate.difficulty < gd_hard)
                        break;
                    tile -= 36;
                case 162:
                case 163:
                case 164:
                case 165:
                    if (gamestate.difficulty < gd_medium)
                        break;
                    tile -= 36;
                case 126:
                case 127:
                case 128:
                case 129:
                    newobj = SpawnStand(en_proguard,x,y,tile - 126);
                    break;

                case 202:
                case 203:
                case 204:
                case 205:
                    if (gamestate.difficulty < gd_hard)
                        break;
                    tile -= 36;
                case 166:
                case 167:
                case 168:
                case 169:
                    if (gamestate.difficulty < gd_medium)
                        break;
                    tile -= 36;
                case 130:
                case 131:
                case 132:
                case 133:
                    newobj = SpawnPatrol (en_proguard,x,y,tile - 130);
                    break;

                //
                // Electro-alien
                //
                case 312:
                    if (gamestate.difficulty < gd_hard)
                        break;

                case 311:
                    if (gamestate.difficulty < gd_medium)
                        break;

                case 310:
                    SpawnStand (en_electro_alien,x,y,0);
                    break;

                //
                // Floating bomb - stationary
                //
                case 364:
                case 365:
                case 366:
                case 367:
                    if (gamestate.difficulty < gd_hard)
                        break;
                    tile -= 18;
                case 346:
                case 347:
                case 348:
                case 349:
                    if (gamestate.difficulty < gd_medium)
                        break;
                    tile -= 18;
                case 328:
                case 329:
                case 330:
                case 331:
                    newobj = SpawnStand(en_floatingbomb,x,y,tile - 328);
                    newobj->flags |= FL_STATIONARY;
                    break;

                //
                // Floating bomb - start stationary
                //
                case 296:
                case 297:
                case 298:
                case 299:
                    if (gamestate.difficulty < gd_hard)
                        break;
                    tile -= 18;
                case 278:
                case 279:
                case 280:
                case 281:
                    if (gamestate.difficulty < gd_medium)
                        break;
                    tile -= 18;
                case 260:
                case 261:
                case 262:
                case 263:
                    newobj = SpawnStand(en_floatingbomb,x,y,tile - 260);
                    break;

                //
                // Floating bomb - start moving
                //
                case 300:
                case 301:
                case 302:
                case 303:
                    if (gamestate.difficulty < gd_hard)
                        break;
                    tile -= 18;
                case 282:
                case 283:
                case 284:
                case 285:
                    if (gamestate.difficulty < gd_medium)
                        break;
                    tile -= 18;
                case 264:
                case 265:
                case 266:
                case 267:
                    newobj = SpawnPatrol(en_floatingbomb,x,y,tile - 264);
                    break;

                //
                // Volatile mat. transport - stationary
                //
#if GAME_VERSION != SHAREWARE_VERSION
                case 350:
                case 351:
                case 352:
                case 353:
                    if (gamestate.difficulty < gd_hard)
                        break;
                    tile -= 18;
                case 332:
                case 333:
                case 334:
                case 335:
                    if (gamestate.difficulty < gd_medium)
                        break;
                    tile -= 18;
                case 314:
                case 315:
                case 316:
                case 317:
                    newobj = SpawnStand(en_volatiletransport,x,y,tile - 314);
                    break;
#elif defined(IN_DEVELOPMENT)
                case 350:
                case 351:
                case 352:
                case 353:
                case 332:
                case 333:
                case 334:
                case 335:
                case 314:
                case 315:
                case 316:
                case 317:
                    INVALID_ACTOR_ERR;
                    break;
#endif
                //
                // Volatile mat. transport - moving
                //
#if GAME_VERSION != SHAREWARE_VERSION
                case 354:
                case 355:
                case 356:
                case 357:
                    if (gamestate.difficulty < gd_hard)
                        break;
                    tile -= 18;
                case 336:
                case 337:
                case 338:
                case 339:
                    if (gamestate.difficulty < gd_medium)
                        break;
                    tile -= 18;
                case 318:
                case 319:
                case 320:
                case 321:
                    newobj = SpawnPatrol(en_volatiletransport,x,y,tile - 318);
                    break;
#elif defined(IN_DEVELOPMENT)
                case 354:
                case 355:
                case 356:
                case 357:
                case 336:
                case 337:
                case 338:
                case 339:
                case 318:
                case 319:
                case 320:
                case 321:
                    INVALID_ACTOR_ERR;
                    break;
#endif
                //
                // TODO: not sure why these are in twice
                //
                // Black ooze
                //
                case 313:
                    if (gamestate.difficulty < gd_hard)
                        break;
                    tile -= 18;
                case 295:
                    if (gamestate.difficulty < gd_medium)
                        break;
                    tile -= 18;
                case 277:
                    SpawnOffsetObj (en_black2_ooze,x,y);
                    break;

                //
                // Green ooze
                //
                case 322:
                    if (gamestate.difficulty < gd_hard)
                        break;
                    tile -= 18;
                case 304:
                    if (gamestate.difficulty < gd_medium)
                        break;
                    tile -= 18;
                case 286:
                    SpawnOffsetObj (en_green2_ooze,x,y);
                    break;

                //
                // Genetic Guard
                //
                case 143:
                    if (gamestate.difficulty < gd_hard)
                        break;
                case 142:
                    if (gamestate.difficulty < gd_medium)
                        break;
                case 214:
                    newobj = SpawnOffsetObj(en_genetic_guard,x,y);
                    break;

                //
                // Cyborg Warrior
                //
                case 603:
                    if (gamestate.difficulty < gd_hard)
                        break;
                case 585:
                    if (gamestate.difficulty < gd_medium)
                        break;
                case 250:
                    newobj = SpawnOffsetObj(en_cyborg_warrior,x,y);
                    break;

                //
                // Spider Mutant
                //
                case 601:
                    if (gamestate.difficulty < gd_hard)
                        break;
                case 583:
                    if (gamestate.difficulty < gd_medium)
                        break;
                case 232:
                    newobj = SpawnOffsetObj(en_spider_mutant,x,y);
                    break;

                //
                // Acid Dragon
                //
                case 605:
                    if (gamestate.difficulty < gd_hard)
                    break;
                case 587:
                    if (gamestate.difficulty < gd_medium)
                        break;
                case 268:
                    newobj = SpawnOffsetObj(en_acid_dragon,x,y);
                    break;

                //
                // Breather beast
                //
                case 602:
                    if (gamestate.difficulty < gd_hard)
                        break;
                case 584:
                    if (gamestate.difficulty < gd_medium)
                        break;
                case 233:
                    newobj = SpawnOffsetObj(en_breather_beast,x,y);
                    break;

                //
                // Mech Guardian
                //
                case 606:
                    if (gamestate.difficulty < gd_hard)
                        break;
                case 588:
                    if (gamestate.difficulty < gd_medium)
                        break;
                case 269:
                    newobj = SpawnOffsetObj(en_mech_guardian,x,y);
                    break;

                //
                // Reptilian Warrior
                //
                case 604:
                    if (gamestate.difficulty < gd_hard)
                        break;
                case 586:
                    if (gamestate.difficulty < gd_medium)
                        break;
                case 251:
                    newobj = SpawnOffsetObj(en_reptilian_warrior,x,y);
                    break;

                //
                // Mutant Human type 1
                //
                case 105:
                    if (gamestate.difficulty < gd_hard)
                        break;
                case 104:
                    if (gamestate.difficulty < gd_medium)
                        break;
                case 103:
                    newobj = SpawnOffsetObj(en_mutant_human1,x,y);
                    break;

                //
                // Mutant Human type 2
                //
#if GAME_VERSION != SHAREWARE_VERSION
                case 125:
                    if (gamestate.difficulty < gd_hard)
                        break;
                case 107:
                    if (gamestate.difficulty < gd_medium)
                        break;
                case 106:
                    newobj = SpawnOffsetObj(en_mutant_human2,x,y);
                    break;
#elif defined(IN_DEVELOPMENT)
                case 125:
                case 107:
                case 106:
                    INVALID_ACTOR_ERR;
                    break;
#endif
                //
                // Small Canister Alien - contained
                //
                case 136:
                    if (gamestate.difficulty < gd_hard)
                    {
                        SpawnStatic (x,y,74 - 23);
                        break;
                    }
                case 135:
                    if (gamestate.difficulty < gd_medium)
                    {
                        SpawnStatic (x,y,74 - 23);
                        break;
                    }
                case 134:
                    newobj = SpawnOffsetObj(en_scan_wait_alien,x,y);
                    break;

                //
                // Small Canister Alien - active/walking
                //
                case 288:
                    if (gamestate.difficulty < gd_hard)
                        break;
                case 289:
                    if (gamestate.difficulty < gd_medium)
                        break;
                case 290:
                    newobj = SpawnOffsetObj(en_scan_alien,x,y);
                    break;

                //
                // Large Canister Alien - contained
                //
#if GAME_VERSION != SHAREWARE_VERSION
                case 172:
                    if (gamestate.difficulty < gd_hard)
                    {
                        SpawnStatic (x,y,73 - 23);
                        break;
                    }
                case 171:
                    if (gamestate.difficulty < gd_medium)
                    {
                        SpawnStatic(x,y,73 - 23);
                        break;
                    }
                case 170:
                    newobj = SpawnOffsetObj(en_lcan_wait_alien,x,y);
                    break;
#elif defined(IN_DEVELOPMENT)
                case 172:
                case 171:
                case 170:
                    INVALID_ACTOR_ERR;
                    break;
#endif
                //
                // Large Canister Alien - active/walking
                //
#if GAME_VERSION != SHAREWARE_VERSION
                case 270:
                    if (gamestate.difficulty < gd_hard)
                        break;
                case 271:
                    if (gamestate.difficulty < gd_medium)
                        break;
                case 272:
                    newobj = SpawnOffsetObj(en_lcan_alien,x,y);
                    break;
#elif defined(IN_DEVELOPMENT)
                case 270:
                case 271:
                case 272:
                    INVALID_ACTOR_ERR;
                    break;
#endif
                //
                // Gurney Mutant - asleep
                //
#if GAME_VERSION != SHAREWARE_VERSION
                case 161:
                    if (gamestate.difficulty < gd_hard)
                    {
                        SpawnStatic (x,y,72 - 23);
                        break;
                    }
                case 173:
                    if (gamestate.difficulty < gd_medium)
                    {
                        SpawnStatic (x,y,72 - 23);
                        break;
                    }
                case 137:
                    newobj = SpawnOffsetObj(en_gurney_wait,x,y);
                    break;
#elif defined(IN_DEVELOPMENT)
                case 161:
                case 173:
                case 137:
                    INVALID_ACTOR_ERR;
                    break;
#endif
                //
                // Gurney Mutant - awake
                //
#if GAME_VERSION != SHAREWARE_VERSION
                case 275:
                    if (gamestate.difficulty < gd_hard)
                        break;
                case 274:
                    if (gamestate.difficulty < gd_medium)
                        break;
                case 273:
                    newobj = SpawnOffsetObj(en_gurney,x,y);
                    break;
#elif defined(IN_DEVELOPMENT)
                case 275:
                case 274:
                case 273:
                    INVALID_ACTOR_ERR;
                    break;
#endif
                //
                // Fluid alien
                //
#if GAME_VERSION != SHAREWARE_VERSION
                case 293:
                    if (gamestate.difficulty < gd_hard)
                        break;
                case 292:
                    if (gamestate.difficulty < gd_medium)
                        break;
                case 291:
                    newobj = SpawnStand(en_liquid,x,y,0);
                    break;
#elif defined(IN_DEVELOPMENT)
                case 293:
                case 292:
                case 291:
                    INVALID_ACTOR_ERR;
                    break;
#endif
                //
                // P.O.D. Alien Egg
                //
                case 294:
                    if (gamestate.difficulty < gd_hard)
                        scanvalue = 0xff;
                case 276:
                    if (gamestate.difficulty < gd_medium)
                        scanvalue = 0xff;
                case 306:
                    newobj = SpawnOffsetObj(en_podegg,x,y);

                    if (scanvalue == 0xff)
                        newobj->obclass = deadobj;
                    else
                    {
                        AddTotalPoints (actor_points[podobj - rentacopobj]);
                        AddTotalEnemy (1);
#ifdef TRACK_ENEMY_COUNT
                        numenemy[podobj]++;
#endif
                    }

                    scanvalue = 0xffff;
                    break;

                //
                // P.O.D. Alien
                //
                case 309:
                    if (gamestate.difficulty < gd_hard)
                        break;
                case 308:
                    if (gamestate.difficulty < gd_medium)
                        break;
                case 307:
                    newobj = SpawnOffsetObj(en_pod,x,y);
                    break;

                //
                // Morphing Brown/LBlue Post -> Spider Mutant
                //
                case 610:
                    if (gamestate.difficulty < gd_hard)
                        scanvalue = 0xff;
                case 609:
                    if (gamestate.difficulty < gd_medium)
                        scanvalue = 0xff;
                case 608:
                    if (scanvalue == 0xff)
                        SpawnStatic (x,y,402 - 315);
                    else
                    {
                        AddTotalPoints (actor_points[en_spider_mutant]);
                        AddTotalEnemy (1);
                        newobj = SpawnOffsetObj(en_morphing_spider_mutant,x,y);
#ifdef TRACK_ENEMY_COUNT
                        numenemy[newobj->obclass]++;
#endif
                    }

                    scanvalue = 0xffff;
                    break;

                //
                // Morphing Gray/Green Post -> Reptilian Warrior
                //
                case 592:
                    if (gamestate.difficulty < gd_hard)
                        scanvalue = 0xff;
                case 591:
                    if (gamestate.difficulty < gd_medium)
                        scanvalue = 0xff;
                case 590:
                    if (scanvalue == 0xff)
                        SpawnStatic (x,y,403 - 315);
                    else
                    {
                        AddTotalPoints (actor_points[en_reptilian_warrior]);
                        AddTotalEnemy (1);
                        newobj = SpawnOffsetObj(en_morphing_reptilian_warrior,x,y);
#ifdef TRACK_ENEMY_COUNT
                        numenemy[newobj->obclass]++;
#endif
                    }

                    scanvalue = 0xffff;
                    break;

                //
                // Morphing Statue -> Blue Boy
                //
                case 628:
                    if (gamestate.difficulty < gd_hard)
                        scanvalue = 0xff;
                case 627:
                    if (gamestate.difficulty < gd_medium)
                        scanvalue = 0xff;
                case 626:
                    if (scanvalue == 0xff)
                        SpawnStatic (x,y,48 - 23);
                    else
                    {
                        AddTotalPoints (actor_points[en_mutant_human2]);
                        AddTotalEnemy (1);
                        newobj = SpawnOffsetObj(en_morphing_mutanthuman2,x,y);
#ifdef TRACK_ENEMY_COUNT
                        numenemy[newobj->obclass]++;
#endif
                    }

                    scanvalue = 0xffff;
                    break;

                //
                // Electro-Sphere - Vertical Hover
                //
                case 360:
                    if (gamestate.difficulty < gd_hard)
                        break;
                case 342:
                    if (gamestate.difficulty < gd_medium)
                        break;
                case 324:
                    newobj = SpawnOffsetObj(en_vertsphere,x,y);
                    break;

                //
                // Electro-Sphere - Horizontal Hover
                //
                case 361:
                    if (gamestate.difficulty < gd_hard)
                        break;
                case 343:
                    if (gamestate.difficulty < gd_medium)
                        break;
                case 325:
                    newobj = SpawnOffsetObj(en_horzsphere,x,y);
                    break;

                //
                // Electro-Sphere - Diagonal Hover
                //
                case 362:
                    if (gamestate.difficulty < gd_hard)
                        break;
                case 344:
                    if (gamestate.difficulty < gd_medium)
                        break;
                case 326:
                    newobj = SpawnOffsetObj(en_diagsphere,x,y);
                    break;

                //
                // Stationary SWAT Guards
                //
                case 252:
                case 253:
                case 254:
                case 255:
                    if (gamestate.difficulty < gd_hard)
                        break;
                    tile -= 18;
                case 234:
                case 235:
                case 236:
                case 237:
                    if (gamestate.difficulty < gd_medium)
                        break;
                    tile -= 18;
                case 216:
                case 217:
                case 218:
                case 219:
                    newobj = SpawnStand(en_swat,x,y,tile - 216);
                    break;

                //
                // Roaming SWAT Guards
                //
                case 256:
                case 257:
                case 258:
                case 259:
                    if (gamestate.difficulty < gd_hard)
                        break;
                    tile -= 18;
                case 238:
                case 239:
                case 240:
                case 241:
                    if (gamestate.difficulty < gd_medium)
                        break;
                    tile -= 18;
                case 220:
                case 221:
                case 222:
                case 223:
                    newobj = SpawnPatrol(en_swat,x,y,tile - 220);
                    break;

                //
                // Stationary hanging turrets
                //
                case 368:
                case 369:
                case 370:
                case 371:
                    if (gamestate.difficulty < gd_hard)
                        break;
                    tile -= 126;
                case 242:
                case 243:
                case 244:
                case 245:
                    if (gamestate.difficulty < gd_medium)
                        break;
                    tile -= 18;
                case 224:
                case 225:
                case 226:
                case 227:
                    newobj = SpawnStand(en_hang_terrot,x,y,tile - 224);
                    newobj->flags |= FL_STATIONARY;
                    break;

                //
                // Rotating hanging turrets
                //
                case 372:
                case 373:
                case 374:
                case 375:
                    if (gamestate.difficulty < gd_hard)
                        break;
                    tile -= 126;
                case 246:
                case 247:
                case 248:
                case 249:
                    if (gamestate.difficulty < gd_medium)
                        break;
                    tile -= 18;
                case 228:
                case 229:
                case 230:
                case 231:
                    newobj = SpawnStand(en_hang_terrot,x,y,tile - 228);
                    break;

    //
    // Path objects
    //
                //
                // SWAT Guards
                //
                case 540:
                case 541:
                case 542:
                case 543:
                    if (gamestate.difficulty < gd_hard)
                        break;
                    tile -= 18;
                case 522:
                case 523:
                case 524:
                case 525:
                    if (gamestate.difficulty < gd_medium)
                        break;
                    tile -= 18;
                case 504:
                case 505:
                case 506:
                case 507:
                    newobj = SpawnPatrol(en_swat,x,y,tile - 504);
                    newobj->flags &= ~FL_RANDOM_TURN;
                    break;

                //
                //  Volatile mat. transport
                //
                case 548:
                case 549:
                case 550:
                case 551:
                    if (gamestate.difficulty < gd_hard)
                        break;
                    tile -= 18;
                case 530:
                case 531:
                case 532:
                case 533:
                    if (gamestate.difficulty < gd_medium)
                        break;
                    tile -= 18;
                case 512:
                case 513:
                case 514:
                case 515:
                    newobj = SpawnPatrol(en_volatiletransport,x,y,tile - 512);
                    newobj->flags &= ~FL_RANDOM_TURN;
                    break;

                //
                // Floating bomb
                //
                case 544:
                case 545:
                case 546:
                case 547:
                    if (gamestate.difficulty < gd_hard)
                        break;
                    tile -= 18;
                case 526:
                case 527:
                case 528:
                case 529:
                    if (gamestate.difficulty < gd_medium)
                        break;
                    tile -= 18;
                case 508:
                case 509:
                case 510:
                case 511:
                    newobj = SpawnPatrol(en_floatingbomb,x,y,tile - 508);
                    newobj->flags &= ~FL_RANDOM_TURN;
                    break;

                //
                // PRO Guard
                //
                case 594:
                case 595:
                case 596:
                case 597:
                    if (gamestate.difficulty < gd_hard)
                        break;
                    tile -= 18;
                case 576:
                case 577:
                case 578:
                case 579:
                    if (gamestate.difficulty < gd_medium)
                        break;
                    tile -= 18;
                case 558:
                case 559:
                case 560:
                case 561:
                    newobj = SpawnPatrol(en_proguard,x,y,tile - 558);
                    newobj->flags &= ~FL_RANDOM_TURN;
                    break;

                //
                // Rent-a-cop
                //
                case 552:
                case 553:
                case 554:
                case 555:
                    if (gamestate.difficulty < gd_hard)
                        break;
                    tile -= 18;
                case 534:
                case 535:
                case 536:
                case 537:
                    if (gamestate.difficulty < gd_medium)
                        break;
                    tile -= 18;
                case 516:
                case 517:
                case 518:
                case 519:
                    newobj = SpawnPatrol(en_rentacop,x,y,tile - 516);
                    newobj->flags &= ~FL_RANDOM_TURN;
                    break;

    //
    // Boss actors
    //
                case 630: // Final boss 1
                case 631: // Final boss 2
                case 632: // Final boss 3
                case 633: // Final boss 4
                    newobj = SpawnOffsetObj(en_final_boss1 + tile - 630,x,y);
                    break;
            }

            //
            // if newobj is an object that gives points, add those points to level total
            //
            // newobj is cleared to keep from re-adding points from the previous actor
            //
            if (newobj && newobj->obclass >= rentacopobj && newobj->obclass < crate1obj)
            {
                obclass = newobj->obclass;

                switch (obclass)
                {
                    case lcan_wait_alienobj:
                    case scan_wait_alienobj:
                    case gurney_waitobj:
                        obclass++;
                    break;
                }

                AddTotalPoints (actor_points[obclass - rentacopobj]);
                AddTotalEnemy (1);
#ifdef TRACK_ENEMY_COUNT
                numenemy[newobj->obclass]++;
#endif
            }

            //
            // skip past FA code
            //
            if (scanvalue != 0xffff)
            {
                x++;
                start++;
            }
        }
    }

    if (!loadedgame)
        gamestuff.level[gamestate.mapon].stats.accum_inf = gamestuff.level[gamestate.mapon].stats.total_inf;

#ifdef IN_DEVELOPMENT
    db_count = 0;
#endif
}


/*
==================
=
= AddTotalPoints
=
==================
*/

void AddTotalPoints (int points)
{
    if (loadedgame)
        return;

    gamestuff.level[gamestate.mapon].stats.total_points += points;
}


/*
==================
=
= AddTotalInformants
=
==================
*/

void AddTotalInformants (int informants)
{
    if (loadedgame)
        return;

    gamestuff.level[gamestate.mapon].stats.total_inf += informants;
}


/*
==================
=
= AddTotalEnemy
=
==================
*/

void AddTotalEnemy (int enemies)
{
    if (loadedgame)
        return;

    gamestuff.level[gamestate.mapon].stats.total_enemy += enemies;
}


/*
==================
=
= SetupGameLevel
=
==================
*/

void SetupGameLevel (void)
{
    int      i;
    int      x,y;
    int      lock;
    word     *map,*map1,*map2;
    unsigned tile,icon;
    bool     switchon = false;

    if (!loadedgame)
    {
        gamestate.flags |= GS_CLIP_WALLS;

        InitGoldsternInfo ();
    }

    US_InitRndT (!(demoplayback || demorecord));

//
// load the level
//
    CA_CacheMap (gamestate.mapon + (MAPS_PER_EPISODE * gamestate.episode));

    mapwidth = mapheaderseg[gamestate.mapon]->width;
    mapheight = mapheaderseg[gamestate.mapon]->height;

    if (mapwidth != MAPSIZE || mapheight != MAPSIZE)
        Quit ("Map %d not %dx%d!",gamestate.mapon + 1,MAPSIZE,MAPSIZE);

    LoadLocationText (gamestate.mapon + (MAPS_PER_EPISODE * gamestate.episode));

    memset (TravelTable,0,sizeof(TravelTable));
    memset (gamestate.barrier_table,0xff,sizeof(gamestate.barrier_table));
    memset (gamestate.old_barrier_table,0xff,sizeof(gamestate.old_barrier_table));

//
// copy the wall data to tilemap
//
    memset (tilemap,0,sizeof(tilemap));
    memset (actorat,0,sizeof(actorat));

    map = mapsegs[0];
    map2 = mapsegs[1];

    for (y = 0; y < mapheight; y++)
    {
        for (x = 0; x < mapwidth; x++)
        {
            icon = *map2++;
            tile = *map++;

            if (tile < AMBUSHTILE)
            {
                switch (tile)
                {
                    case RKEY_TILE:
                    case YKEY_TILE:
                    case BKEY_TILE:
                    case BFG_TILE:
                    case ION_TILE:
                    case DETONATOR_TILE:
                    case CLOAK_TILE:
                    case LINC_TILE:
                    case CLOAK_AMBUSH_TILE:
                        break;

                    default:
                        tilemap[x][y] = tile;
                        actorat[x][y] = (objtype *)tile;
                        break;
                }
            }

            if (tile < MAXWALLTILES || icon == PUSHABLETILE)
                TravelTable[x][y] |= TT_TRAVELED;
        }
    }

//
// start spawning things with a clean slate
//
    InitObjList ();
    InitDoorObjList ();
    InitStatObjList ();

    InitMsgCache ((mCacheList *)&ConHintList,sizeof(ConHintList),sizeof(ConHintList.cmInfo[0]));
    InitMsgCache ((mCacheList *)&InfHintList,sizeof(InfHintList),sizeof(InfHintList.smInfo[0]));
    InitMsgCache ((mCacheList *)&NiceSciList,sizeof(NiceSciList),sizeof(InfHintList.smInfo[0]));
    InitMsgCache ((mCacheList *)&MeanSciList,sizeof(MeanSciList),sizeof(InfHintList.smInfo[0]));

    map = mapsegs[0];
    map1 = mapsegs[1];

    LastInfoAttacker = alerted = NumEAWalls = 0;

//
// spawn doors & special walls
//
    for (y = 0; y < mapheight; y++)
    {
        for (x = 0; x < mapwidth; x++)
        {
            tile = *map++;
            lock = *map1;

            if (*map == 30)
            {
                gamestate.wintilex = x + 1;
                gamestate.wintiley = y;
            }

            if (tile >= 88 && tile <= 105)
            {
                //
                // keys
                //
                switch (lock)
                {
                    case 55:
                    case 56:
                        lock = kt_red + lock - 55;
                        *map1 = 0;
                        break;

                    case 58:
                        lock = kt_blue;
                        *map1 = 0;
                        break;

                    default:
                        lock = kt_none;
                }

                //
                // doors
                //
                switch (tile)
                {
                    case 88:
                    case 89:
                        SpawnDoor (x,y,!(tile % 2),lock,dr_bio);
                        break;

                    case 90:
                    case 91:
                        SpawnDoor (x,y,!(tile % 2),lock,dr_normal);
                        break;

                    case 92:
                    case 93:
                        SpawnDoor (x,y,!(tile % 2),lock,dr_prison);
                        break;

                    case 94:
                    case 95:
                        SpawnDoor (x,y,!(tile % 2),lock,dr_elevator);
                        break;

                    case 96:
                    case 97:
                        SpawnDoor (x,y,!(tile % 2),lock,dr_high_security);
                        break;

                    case 98:       // oneway left  - Vert
                    case 99:       // oneway up    - Horz
                    case 100:      // oneway right - Vert
                    case 101:      // oneway down  - Horz
                        SpawnDoor (x,y,!(tile % 2),lock,dr_oneway_left + (tile - 98));
                        break;

                    case 102:
                    case 103:
                        SpawnDoor (x,y,!(tile % 2),lock,dr_office);
                        break;

                    case 104:
                    case 105:
                        SpawnDoor (x,y,!(tile % 2),lock,dr_space);
                        break;
                }
            }
            else
            {
                switch (tile)
                {
                    case SODATILE:
                        if (!loadedgame)
                        {
                            SpawnConcession (x,y,lock,CT_BEVS);
                            *map1 = 0;
                        }
                        break;

                    case FOODTILE:
                        if (!loadedgame)
                        {
                            SpawnConcession (x,y,lock,CT_FOOD);
                            *map1 = 0;
                        }
                        break;

                    case EATILE:
                        eaList[NumEAWalls].tilex = x;
                        eaList[NumEAWalls].tiley = y;
                        eaList[NumEAWalls].aliens_out = 0;
                        if ((lock & 0xff00) == 0xfa00)
                            eaList[NumEAWalls].delay = 60 * (lock & 0xff);
                        else
                            eaList[NumEAWalls].delay = (60 * 8) + Random(60 * 22);

                        if (NumEAWalls++ >= MAXEAWALLS)
                            Quit ("Too many Electro-Alien walls in level!");
                        break;

                    case ON_SWITCH:
                        switchon = true;
                    case OFF_SWITCH:
                        *map1 = 0xf800 | UpdateBarrierTable(*map1 >> 8,*map1 & 0xff,switchon);

                        switchon = false;    // init for next time
                        break;
                }
            }

            map1++;
        }
    }

//
// spawn actors
//
    ScanInfoPlane ();
    ConnectBarriers ();

//
// init informant stuff
//
    LastInfArea = 0xff;
    FirstGenInfMsg = 0;

    for (i = 0; i < InfHintList.NumMsgs; i++)
    {
        if (InfHintList.smInfo[i].areanumber == 0xff)
            break;

        FirstGenInfMsg++;
    }

    TotalGenInfMsgs = InfHintList.NumMsgs - FirstGenInfMsg;

//
// take out the special tiles that were not used
//
    map = mapsegs[0];

    for (y = 0; y < mapheight; y++)
    {
        for (x = 0; x < mapwidth; x++)
        {
            tile = *map++;

            switch (tile)
            {
                case AMBUSHTILE:
                case RKEY_TILE:
                case YKEY_TILE:
                case BKEY_TILE:
                case BFG_TILE:
                case ION_TILE:
                case DETONATOR_TILE:
                case CLOAK_TILE:
                case LINC_TILE:
                case CLOAK_AMBUSH_TILE:
                    *(map - 1) = GetAreaNumber(x,y);
                    break;
            }
        }
    }

//
// check and make sure a detonator is in a 'locked' level
//
    if (gamestate.mapon < 20 && !detonators_spawned && gamestuff.level[gamestate.mapon + 1].locked)
        Quit ("No Fission/Plasma Detonator in level! Go put one in the map PAL!");
}


/*
===================
=
= LoadLocationText
=
===================
*/

void LoadLocationText (int textnum)
{
    char *temp;

    LoadMsg (LocationText,LEVEL_DESCS,textnum + 1,MAX_LOCATION_DESC_LEN);

    temp = strstr(LocationText,int_xx);

    if (temp)
        *temp = '\0';
}


/*
===================
=
= SetFontColor
=
===================
*/

void SetFontColor (int f, int b)
{
    fontcolor = f;
    backcolor = b;
}


/*
===================
=
= SetupFizzlein
=
===================
*/

void SetupFizzlein (int x, int y, int width, int height)
{
    screen.flags |= SC_FIZZLEIN;
    fizzlex = x;
    fizzley = y;
    fizzlewidth = width;
    fizzleheight = height;
}


/*
===================
=
= SetViewSize
=
= Height MUST be even before scaling
=
===================
*/

void SetViewSize (int size)
{
    int width,height;

    if (size < 6 || size > 20)
        Quit ("Invalid view size!");

    viewsize = size;

    width = ((size << 4) * screen.width) / screen.basewidth;
    height = (((int)((size << 4) * HEIGHTRATIO) & ~1) * screen.height) / screen.baseheight;
    height += (screen.heightoffset * 2) * screen.scale;

    viewwidth = width & ~15;                  // must be divisible by 16
    viewheight = height;
    centerx = viewwidth / 2;
    centery = viewheight / 2;
    shootdelta = viewwidth / 10;
    normalshade = (((viewwidth * 9) >> 4) - 3) / normalshadediv;

    if (viewheight == screen.height)
        viewscreenx = viewscreeny = viewscreenofs = 0;
    else
    {
        viewscreenx = (screen.width - viewwidth) >> 1;
        viewscreeny = ((screen.height - ((STATUSLINES - TOP_STRIP_HEIGHT) * screen.scale)) - viewheight) >> 1;
        viewscreenofs = ylookup[viewscreeny] + viewscreenx;
    }

    baseviewwidth = viewwidth / screen.scale;
    baseviewheight = viewheight / screen.scale;
    baseviewscreenx = viewscreenx / screen.scale;
    baseviewscreeny = viewscreeny / screen.scale;

//
// calculate trace angles and projection constants
//
    CalcProjection (FOCALLENGTH);
}


/*
===================
=
= ShowViewSize
=
===================
*/

void ShowViewSize (int size)
{
    int oldwidth,oldheight;

    if (size < 6 || size > 20)
        Quit ("Invalid view size!");

    oldwidth = viewwidth;
    oldheight = viewheight;

    viewwidth = ((size << 4) * screen.width) / screen.basewidth;
    viewheight = (((int)((size << 4) * HEIGHTRATIO) & ~1) * screen.height) / screen.baseheight;
    viewheight += (screen.heightoffset * 2) * screen.scale;
    centerx = viewwidth / 2;

    baseviewwidth = viewwidth / screen.scale;
    baseviewheight = viewheight / screen.scale;

    VW_Bar (0,TOP_STRIP_HEIGHT,screen.basewidth,screen.baseheight - STATUSLINES - TOP_STRIP_HEIGHT,BORDER_MED_COLOR);

    DrawPlayBorder ();

    viewheight = oldheight;
    viewwidth = oldwidth;
    centerx = viewwidth / 2;

    baseviewwidth = viewwidth / screen.scale;
    baseviewheight = viewheight / screen.scale;
}


/*
===================
=
= DrawPlayBorder
=
===================
*/

void DrawPlayBorder (void)
{
    int xl,yl;

    if (viewwidth == screen.width)
    {
        VW_Bar (0,TOP_STRIP_HEIGHT,screen.basewidth,screen.baseheight - STATUSLINES - TOP_STRIP_HEIGHT,0);

        return;
    }

    xl = (screen.basewidth / 2) - (baseviewwidth / 2);
    yl = (screen.baseheight - STATUSLINES + TOP_STRIP_HEIGHT - baseviewheight) / 2;

    BevelBox (0,TOP_STRIP_HEIGHT,screen.basewidth,screen.baseheight - STATUSLINES - TOP_STRIP_HEIGHT,BORDER_HI_COLOR,BORDER_MED_COLOR,BORDER_LO_COLOR);
    BevelBox (xl - 1,yl - 1,baseviewwidth + 2,baseviewheight + 2,BORDER_LO_COLOR,0,BORDER_HI_COLOR);
}


/*
===================
=
= BMAmsg
=
= These messages are displayed by the Text Presenter
=
===================
*/

void BMAmsg (const char *msg)
{
    #define BMAx1 0             // outer bevel
    #define BMAy1 (screen.baseheight - STATUSLINES)
    #define BMAw1 screen.basewidth
    #define BMAh1 STATUSLINES

    #define BMAx2 (BMAx1 + 7)   // inner bevel
    #define BMAy2 (BMAy1 + 4)
    #define BMAw2 (BMAw1 - 14)
    #define BMAh2 (BMAh1 - 8)

    PresenterInfo pi;
    fontstruct    *font;
    char          *string,*s;
    char          numlines;
    int           cheight;
    size_t        len;

    BevelBox (BMAx1,BMAy1,BMAw1,BMAh1,BORDER_HI_COLOR,BORDER_MED_COLOR,BORDER_LO_COLOR);
    BevelBox (BMAx2,BMAy2,BMAw2,BMAh2,BORDER_LO_COLOR,BORDER_MED_COLOR,BORDER_HI_COLOR);

    if (msg)
    {
        font = (fontstruct *)grsegs[STARTFONT + fontnumber];
        numlines = 1;

        len = strlen(msg) + 1;
        string = SafeMalloc(len);

        snprintf (string,len,msg);
        s = string;

        memset (&pi,0,sizeof(pi));
        pi.flags = TPF_CACHE_NO_GFX;
        pi.script[0] = s;

        while (*s)
        {
            if (*s++ == TP_RETURN_CHAR)
                numlines++;
        }

        cheight = ((font->height * numlines) + 1) + (TP_MARGIN * 2);

        pi.xl = BMAx2 + 1;
        pi.yl = BMAy2 + ((BMAh2 - cheight) / 2);
        pi.xh = pi.xl + BMAw2 - 3;
        pi.yh = pi.yl + cheight - 1;
        pi.bgcolor = BORDER_MED_COLOR;
        pi.ltcolor = BORDER_HI_COLOR;
        fontcolor = BORDER_TEXT_COLOR;
        pi.shcolor = pi.dkcolor = BORDER_LO_COLOR;
        pi.fontnumber = fontnumber;

        TP_InitScript (&pi);
        TP_Presenter (&pi);

        free (string);
    }
}


/*
===================
=
= CacheBMAmsg
=
= Caches in a message number and displays it using BMAmsg
=
= TODO: make this safe
=
===================
*/

void CacheBMAmsg (int msgnum)
{
    char *string,*pos;

    string = (char *)grsegs[msgnum];

    pos = strstr(string,int_xx);
    pos[strlen(int_xx)] = '\0';

    BMAmsg (string);
}


/*
===================
=
= BevelBox
=
===================
*/

void BevelBox (int xl, int yl, int w, int h, int hi, int med, int lo)
{
    int xh,yh;
    int hc;

    VW_Bar (xl,yl,w,h,med);     // inside

    hc = med + 1;
    xh = xl + w - 1;
    yh = yl + h - 1;

    VW_Hlin (xl,xh,yl,hi);      // top
    VW_Hlin (xl,xh,yh,lo);      // bottom
    VW_Vlin (yl,yh,xl,hi);      // left
    VW_Vlin (yl,yh,xh,lo);      // right

    VW_Plot (xl,yh,hc);         // lower-left
    VW_Plot (xh,yl,hc);         // upper-right
}


/*
===================
=
= ShadowPrintLocationText
=
===================
*/

void ShadowPrintLocationText (int type)
{
    const char *DemoMsg = "-- DEMO --";
    const char *DebugText = "-- DEBUG MODE ENABLED --";
    const char *s,*ls_text[3] = {"-- LOADING --","-- SAVING --","-- CHANGE VIEW SIZE --"};
    word       w,h;

    py = 5;
    fontcolor = 0xaf;

    //
    // print LOCATION info
    //
    switch (type)
    {
        case sp_normal:
            //
            // print LEVEL info
            //
            px = 13;

            if (gamestate.mapon > GOLD_MORPH_LEVEL)
                ShPrint (" SECRET ",0,false);
            else
            {
                ShPrint (" AREA: ",0,false);

                if (!type)
                    ShPrint (itoa(gamestate.mapon + 1,str,10),0,false);
            }

            //
            // print LIVES info
            //
            px = 267;

            ShPrint ("LIVES: ",0,false);

            if (!type)
                ShPrint (itoa(gamestate.lives,str,10),0,false);

            //
            // print location text
            //
            if (demoplayback || demorecord)
                s = DemoMsg;
            else if (DebugOk || (gamestate.flags & (GS_QUICKRUN | GS_STARTLEVEL | GS_TICS_FOR_SCORE | GS_MUSIC_TEST | GS_SHOW_OVERHEAD)))
                s = DebugText;
            else
                s = LocationText;
            break;

        case sp_changeview:
        case sp_loading:
        case sp_saving:
            s = ls_text[type - sp_loading];
            break;
    }

    VW_MeasureString (s,&w,&h);

    px = (screen.basewidth / 2) - (w / 2);

    ShPrint (s,0,false);
}


/*
==========================
=
= DrawTopInfo
=
==========================
*/

void DrawTopInfo (int type)
{
    int oldfontnumber = fontnumber;

    VW_DrawPic (0,0,TOP_STATUSBARPIC);

    fontnumber = 2;

    ShadowPrintLocationText (type);

    fontnumber = oldfontnumber;
}


/*
===================
=
= DrawPlayScreen
=
===================
*/

void DrawPlayScreen (bool InitInfoMsg)
{
    if (loadedgame)
        return;

    if (playstate != ex_transported)
        VW_FadeOut ();

    WindowW = 253;
    WindowH = 8;
    fontnumber = 2;

    DrawPlayBorder ();

    VW_DrawPic (0,screen.baseheight - STATUSLINES,STATUSBARPIC);
    VW_DrawPic (0,0,TOP_STATUSBARPIC);

    ShadowPrintLocationText (sp_normal);

    DrawHealth ();
    DrawKeys ();
    DrawWeapon ();
    DrawScore ();

    InitInfoArea ();

    if (InitInfoMsg)
        DISPLAY_MSG ("R.E.B.A.\rAGENT: BLAKE STONE\rALL SYSTEMS READY.",MP_max_val,MT_NOTHING);
    else
        DisplayNoMoMsgs ();

    ForceUpdateStatusBar ();
}


/*
===================
=
= DrawWarpIn
=
===================
*/

void DrawWarpIn (void)
{
    InitInfoArea ();
    DisplayInfoMsg ("\r\r    TRANSPORTING...",MP_POWERUP,2 * 60,MT_GENERAL);

    DrawHealth ();
    DrawKeys ();
    DrawWeapon ();
    DrawScore ();
    WindowW = 253;
    WindowH = 8;
    fontnumber = 2;

    VW_DrawPic (0,screen.baseheight - STATUSLINES,STATUSBARPIC);
    VW_DrawPic (0,0,TOP_STATUSBARPIC);

    ShadowPrintLocationText (sp_normal);
    UpdateStatusBar ();

    SD_PlaySound (WARPINSND);

    SetupFizzlein (baseviewscreenx,baseviewscreeny,baseviewwidth,baseviewheight);
}


/*
===================
=
= Warped
=
===================
*/

void Warped (void)
{
    int iangle;

    DisplayInfoMsg ("\r\r\r   TRANSPORTING OUT",MP_POWERUP,7 * 60,MT_GENERAL);

    gamestate.old_weapons[3] = gamestate.weapon;
    gamestate.weapon = -1;      // take away weapon

    if (screen.flags & SC_FADED)
        ThreeDRefresh ();

    iangle = (((player->dir + 4) % 8) >> 1) * ANG90;

    RotateView (iangle,2);

    gamestate.weapon = gamestate.old_weapons[3];
    gamestate.attackframe = gamestate.attackcount = gamestate.weaponframe = 0;

    IN_ClearKeysDown ();
    SD_PlaySound (WARPINSND);

    VW_Bar (baseviewscreenx,baseviewscreeny,baseviewwidth,baseviewheight,0);
    VW_FizzleFade (baseviewscreenx,baseviewscreeny,baseviewwidth,baseviewheight,70,false);

    IN_UserInput (100);
    SD_WaitSoundDone ();

}

#ifdef DEMOS_EXTERN

/*
==================
=
= StartDemoRecord
=
==================
*/

#define MAXDEMOSIZE 16384

void StartDemoRecord (int levelnumber)
{
    demobuffer = SafeMalloc(MAXDEMOSIZE);
    demoptr = demobuffer;
    lastdemoptr = &demoptr[MAXDEMOSIZE];    // TODO: buffer overflow

    *demoptr++ = levelnumber;
    demoptr += sizeof(uint16_t);    // leave space for length
    *demoptr++ = 0;                 // KS: the original demos leave a random byte here, so clear it

    demorecord = true;
}


/*
==================
=
= FinishDemoRecord
=
==================
*/

void FinishDemoRecord (void)
{
    FILE     *demofile;
    char     fname[13];
    int      level;
    uint16_t length;

    demorecord = false;

    length = (uint16_t)(demoptr - demobuffer);

    demoptr = &demobuffer[1];
    demoptr[0] = (int8_t)(length >> 8);
    demoptr[1] = (int8_t)(length & 0xff);

    VW_FadeIn ();

    US_CenterWindow (24,3);
    PrintY += 6;
    fontnumber = 4;
    US_Print (" Demo number (0-9):");
    VW_UpdateScreen (screen.buffer);

    if (US_LineInput(px,py,str,NULL,true,2,0))
    {
        level = atoi(str);

        if (level >= 0 && level <= MAX_WARP_LEVEL)
        {
            snprintf (fname,sizeof(fname),"%s%s",demoname,extension);
            fname[4] = '0' + level;

            demofile = fopen(fname,"wb");

            if (!demofile)
                CA_CannotOpen (fname);

            if (!fwrite(demobuffer,length,1,demofile))
                Quit ("Error writing file %s: %s",fname,strerror(errno));
        }
    }

    VW_FadeOut ();

    free (demobuffer);
    demobuffer = demoptr = lastdemoptr = NULL;
}


/*
==================
=
= RecordDemo
=
= Fades the screen out, then starts a demo. Exits with the screen faded
=
==================
*/

void RecordDemo (void)
{
    int level,esc;

    US_CenterWindow (26,3);
    PrintY += 6;
    fontnumber = 4;
    snprintf (str,sizeof(str),"  Demo which level(0-%d):",MAX_WARP_LEVEL);
    US_Print (str);
    VW_FadeIn ();

    esc = !US_LineInput (px,py,str,NULL,true,2,0);

    if (esc)
        return;

    level = atoi(str);

    if (level > MAX_WARP_LEVEL)
        return;

    SetFontColor (0,15);
    VW_FadeOut ();

    NewGame (gd_easy,level / MAPS_PER_EPISODE);
    gamestate.mapon = level % MAPS_PER_EPISODE;
    StartDemoRecord (level);

    DrawPlayScreen (true);
    DrawTopInfo (sp_loading);
    DisplayPrepingMsg (prep_msg);
    LS_current = 1;
    LS_total = 20;

    VW_FadeIn ();

    startgame = false;
    demorecord = true;

    LoadLevel (gamestate.mapon);

    VW_FadeOut ();
    DrawPlayScreen (true);
    StartMusic ();

    screen.flags |= SC_FIZZLEIN;

    PlayLoop ();

    demoplayback = false;

    StopMusic ();
    VW_FadeOut ();
    SD_StopDigitized ();

    FinishDemoRecord ();
}
#endif

/*
==================
=
= PlayDemo
=
= Fades the screen out, then starts a demo. Exits with the screen faded
=
==================
*/

void PlayDemo (int demonumber)
{
#ifdef DEMOS_EXTERN
    char     fname[13];
#endif
    uint16_t length;

#ifndef DEMOS_EXTERN
#if GAME_VERSION == SHAREWARE_VERSION
    int dems[4] = {T_DEMO0,T_DEMO1,T_DEMO2,T_DEMO3};
#else
    int dems[6] = {T_DEMO0,T_DEMO1,T_DEMO2,T_DEMO3,T_DEMO4,T_DEMO5};
#endif
    demoptr = (int8_t *)grsegs[dems[demonumber]];
#else
    snprintf (fname,sizeof(fname),"%s%s",demoname,extension);
    fname[4] = '0' + demonumber;

    IO_LoadFile (fname,(void *)&demobuffer);

    demoptr = demobuffer;
#endif
    NewGame (1,0);
    gamestate.mapon = *demoptr++;
    gamestate.difficulty = gd_easy;
    length = ReadShort(demoptr);
    demoptr += sizeof(length) + 1;   // KS: move past length and the empty byte
    lastdemoptr = &demoptr[length - 4];

    VW_FadeOut ();

    SetFontColor (0,15);
    DrawPlayScreen (true);
    DrawTopInfo (sp_loading);
    DisplayPrepingMsg (prep_msg);
    LS_current = 1;
    LS_total = 20;
    VW_FadeIn ();

    startgame = false;
    demoplayback = true;

    StartMusic ();
    LoadLevel (gamestate.mapon);

    VW_FadeOut ();

    DrawPlayScreen (true);
    LoadPlanes ();
#ifdef NOTYET
    SetupFizzlein (baseviewscreenx,baseviewscreeny,baseviewwidth,baseviewheight);
    //screen.flags |= SC_FIZZLEIN;
#endif
    PlayLoop ();

    if (gamestate.health <= 0)
        Died ();

#ifdef DEMOS_EXTERN
    free (demobuffer);
#endif
    demoplayback = false;
    LS_current = LS_total = -1;

    StopMusic ();
    VW_FadeOut ();
    SD_StopDigitized ();

    playstate = ex_title;
}


/*
==================
=
= Died
=
==================
*/

#define DEATHROTATE 2

void Died (void)
{
    gamestate.weapon = -1;      // take away weapon
    SD_PlaySound (PLAYERDEATHSND);

    RotateView (CalcAngle(player,lastattacker),DEATHROTATE);

//
// fade to red
//
    IN_ClearKeysDown ();
    FinishPaletteShifts ();

    VW_Bar (baseviewscreenx,baseviewscreeny,baseviewwidth,baseviewheight,0x17);
    VW_FizzleFade (baseviewscreenx,baseviewscreeny,baseviewwidth,baseviewheight,70,false);

    if (demoplayback)
        return;

    IN_UserInput (100);

    SD_WaitSoundDone ();
    StopMusic ();

    gamestate.lives--;

    if (gamestate.lives > -1)
    {
        gamestate.health = 100;
        gamestate.weapons = 1 << wp_autocharge;
        gamestate.weapon = gamestate.chosenweapon = wp_autocharge;

        gamestate.ammo = STARTAMMO;
        gamestate.attackframe = gamestate.attackcount =
        gamestate.weaponframe = 0;

        gamestate.flags |= GS_CLIP_WALLS | GS_ATTACK_INFOAREA;

        DrawHealth ();
        DrawKeys ();
        DrawWeapon ();
        DrawScore ();
        DrawKeys ();
        ForceUpdateStatusBar ();
    }
}


/*
==================
=
= LoseScreen
=
= Displays the Goldstern/DamagedReba message
=
==================
*/

void LoseScreen (void)
{
    PresenterInfo pi;

    VW_FadeOut ();

    memset (&pi,0,sizeof(pi));

    pi.flags = TPF_USE_CURRENT | TPF_SHOW_CURSOR | TPF_SCROLL_REGION | TPF_CONTINUE | TPF_TERM_SOUND | TPF_ABORTABLE;
    pi.xl = 14;
    pi.yl = 141;
    pi.xh = 14 + 293;
    pi.yh = 141 + 32;
    pi.ltcolor = 15;
    pi.bgcolor = 0;
    pi.dkcolor = 1;
    pi.shcolor = 1;
    pi.fontnumber = 2;
    pi.cur_x = -1;
    pi.print_delay = 2;

    SD_StopDigitized ();
    StopMusic ();

    VW_DrawPic (0,0,LOSEPIC);
    VW_UpdateScreen (screen.buffer);

#ifdef ID_CACHE_LOSE
    TP_LoadScript (NULL,&pi,LOSETEXT);
#else
    TP_LoadScript ("LOSE.TXT",&pi,0);
#endif
    //
    // Now Presenting... The Loser Prize.. 1 nice message directly from Dr.
    // ==============    Goldstern himself!  Oooo Ohhhhh <clap> <clap> ...
    //
    VW_FadeIn ();
    TP_Presenter (&pi);
    VW_FadeOut ();

#ifdef ID_CACHE_LOSE
    TP_FreeScript (&pi,LOSETEXT);
#else
    TP_FreeScript (&pi,0);
#endif
    IN_ClearKeysDown ();
}


/*
==========================
=
= RotateView
=
==========================
*/

void RotateView (int destangle, int speed)
{
    int	    curangle,clockwise,counter,change;
    objtype *obj;
    bool    oldgodmode;

    if (player->angle > destangle)
    {
        counter = player->angle - destangle;
        clockwise = ANG360 - player->angle + destangle;
    }
    else
    {
        clockwise = destangle - player->angle;
        counter = player->angle + ANG360 - destangle;
    }

    oldgodmode = godmode;
    godmode = true;             // player is invulnerable while rotating
    curangle = player->angle;

    controly = 0;

    if (clockwise < counter)
    {
        //
        // rotate clockwise
        //
        if (curangle > destangle)
            curangle -= ANG360;

        controlx = -1;

        do
        {
            change = tics * speed;

            if (curangle + change > destangle)
                change = destangle - curangle;

            curangle += change;
            player->angle += change;

            if (player->angle >= ANG360)
                player->angle -= ANG360;

            for (obj = player->next; obj; obj = obj->next)
                DoActor (obj);

            ThreeDRefresh ();
            CalcTics ();

        } while (curangle != destangle);
    }
    else
    {
        //
        // rotate counterclockwise
        //
        if (curangle < destangle)
            curangle += ANG360;

        controlx = 1;

        do
        {
            change = -tics * speed;

            if (curangle + change < destangle)
                change = destangle - curangle;

            curangle += change;
            player->angle += change;

            if (player->angle < 0)
                player->angle += ANG360;

            for (obj = player->next; obj; obj = obj->next)
                DoActor (obj);

            ThreeDRefresh ();
            CalcTics ();

        } while (curangle != destangle);
    }

    controlx = 0;
    player->dir = ((player->angle + (ANG45 / 2)) % ANG360) / ANG45;
    godmode = oldgodmode;
}


/*
===================
=
= GameLoop
=
===================
*/

void GameLoop (void)
{
    const char *string = "  Auto Quick Load?  ";
    char       Score[13];
    bool       died;
    SDL_Color  endingpal[256];

    VW_SetBufferOffset (0);    // draw a full screen while in the 3D renderer

    SD_StopDigitized ();
    SetFontColor (0,15);
    DrawPlayScreen (true);

    died = false;

    while (1)
    {
        ingame = true;

        if (died && pickquick)
        {
            US_ResetWindow (STATUSLINES);

            if (Confirm(string))
            {
                playstate = ex_stillplaying;
                DrawPlayBorder ();
                VW_UpdateScreen (screen.buffer);
                ControlPanel (sc_F9);
            }

            DrawPlayBorder ();
            VW_UpdateScreen (screen.buffer);
        }

        if (!sqActive)
            StartMusic ();

        if (!(loadedgame || LevelInPlaytemp(gamestate.mapon)))
        {
            gamestate.tic_score = gamestate.score = gamestate.oldscore;
            gamestate.rpower = gamestate.old_rpower;
            gamestate.tokens = gamestate.old_tokens;
            gamestate.weapons = gamestate.old_weapons[0];
            gamestate.weapon = gamestate.old_weapons[1];
            gamestate.chosenweapon = gamestate.old_weapons[2];
            gamestate.ammo = gamestate.old_ammo;
            gamestate.plasma_detonators = gamestate.old_plasma_detonators;
            gamestate.boss_key_dropped = gamestate.old_boss_key_dropped;
            memcpy (gamestate.numkeys,gamestate.old_numkeys,sizeof(gamestate.numkeys));
            memcpy (gamestate.barrier_table,gamestate.old_barrier_table,sizeof(gamestate.barrier_table));
            memcpy (&gamestuff.level[0],gamestuff.old_levelinfo,sizeof(gamestuff.old_levelinfo));

            DrawKeys ();
            DrawScore ();
        }

        startgame = false;

        if (!loadedgame)
        {
            if (LS_current == -1)
            {
                DrawTopInfo (sp_loading);
                DisplayPrepingMsg (prep_msg);
                LS_current = 1;
                LS_total = 20;
            }

            LoadLevel (gamestate.mapon);
        }
        else
            loadedgame = false;

        LS_current = LS_total = -1;

        LoadPlanes ();

        if (died)
        {
            WindowY = screen.baseheight - 12;

            LoadLevelUpdate (1,1);

            died = false;
            DrawPlayScreen (true);
        }
        else
        {
            DrawLevelTransition ();

            if (playstate == ex_transported)
                DrawWarpIn ();
            else
                DrawPlayScreen (false);
        }

        if (!sqActive)
            StartMusic ();

        PlayLoop ();

        LS_current = LS_total = -1;
        died = false;

        StopMusic ();
        ingame = false;

#ifdef DEMOS_EXTERN
        if (demorecord && playstate != ex_warped)
            FinishDemoRecord ();
#endif
        if (startgame || loadedgame)
        {
            SD_StopDigitized ();
            SetFontColor (0,15);
            DrawPlayScreen (true);

            died = false;

            continue;
        }

        switch (playstate)
        {
            case ex_transported:    // same as ex_completed
                Warped ();

            case ex_completed:
            case ex_secretlevel:
            case ex_warped:
                SD_StopDigitized ();
                gamestate.mapon++;
                ClearNClose ();
                DrawTopInfo (sp_loading);
                DisplayPrepingMsg (prep_msg);
                WindowY = screen.baseheight - 19;
                LS_current = 1;
                LS_total = 38;
                StartMusic ();
                SaveLevel (gamestate.lastmapon);

                gamestate.old_rpower = gamestate.rpower;
                gamestate.oldscore = gamestate.score;
                gamestate.old_tokens = gamestate.tokens;
                gamestate.old_weapons[0] = gamestate.weapons;
                gamestate.old_weapons[1] = gamestate.weapon;
                gamestate.old_weapons[2] = gamestate.chosenweapon;
                gamestate.old_ammo = gamestate.ammo;
                gamestate.old_boss_key_dropped = gamestate.boss_key_dropped;
                memcpy (gamestate.old_numkeys,gamestate.numkeys,sizeof(gamestate.old_numkeys));
                memcpy (gamestate.old_barrier_table,gamestate.barrier_table,sizeof(gamestate.old_barrier_table));
                memcpy (gamestuff.old_levelinfo,&gamestuff.level[0],sizeof(gamestuff.old_levelinfo));
                break;

            case ex_died:
                if (InstantQuit)
                    InstantQuit = false;
                else
                {
                    Died ();

                    died = true;

                    if (gamestate.lives > -1)
                    {
                        SD_StopDigitized ();
                        break;    // more lives left
                    }

                    VW_SetBufferOffset (screen.heightoffset);

                    LoseScreen ();
                }

            case ex_victorious:
                MainMenu[MM_SAVE_MISSION].active = AT_DISABLED;
                MainMenu[MM_VIEW_SCORES].routine = CP_ViewScores;
                snprintf (MainMenu[MM_VIEW_SCORES].string,sizeof(MainMenu[MM_VIEW_SCORES].string),"HIGH SCORES");

                if (playstate == ex_victorious)
                    ThreeDRefresh ();

                SD_StopDigitized ();

                if (playstate == ex_victorious)
                {
                    fontnumber = 1;

                    CacheBMAmsg (YOUWIN_TEXT);
                    SD_PlaySound (BONUS1SND);
                    SD_WaitSoundDone ();
                    IN_UserInput (5 * 60);
                    SD_StopDigitized ();
                }

                VW_FadeOut ();
                ClearMenuBorders ();

                snprintf (Score,sizeof(Score),"%d",gamestate.score);
                piStringTable[0] = Score;

                if (playstate == ex_victorious)
                {
                    //
                    // TODO: test if this looks like the original
                    //
                    VW_ConvertPalette (grsegs[ENDINGPALETTE],endingpal,lengthof(endingpal));

                    DoMovie (mv_final,endingpal);

                    VW_SetPalette (gamepal);

                    Breifing (BT_WIN,gamestate.episode);
                }

                CheckHighScore (gamestate.score,gamestate.mapon + 1);
                return;

            default:
                SD_StopDigitized ();
                break;
        }
    }
}
