//
// basic game stuff

#include "3d_def.h"


#define ATABLEMAX 15

unsigned    TopColor,BottomColor;

int         viewsize = 20;
bool        loadedgame;
fargametype gamestuff;
gametype    gamestate;

int         mapwidth,mapheight;
int         leftchannel,rightchannel;

byte        righttable[ATABLEMAX][ATABLEMAX * 2] =
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

byte        lefttable[ATABLEMAX][ATABLEMAX * 2] =
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
= TODO: just basic stuff for now
=
=====================
*/

void NewGame (int difficulty, int episode)
{
	memset (&gamestate,0,sizeof(gamestate));

	gamestate.difficulty = difficulty;

	gamestate.weapons = (1 << wp_autocharge) | (1 << wp_pistol);
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
	gamestate.flags |= GS_LIGHTING;
	gamestate.key_floor = gamestate.mapon + 1;

	//startgame = true;

	ExtraRadarFlags = 0;
}


void ScanInfoPlane (void)
{
    int  x,y;
    int  tile;
    word *start;

    start = mapsegs[1];

    for (y = 0; y < mapheight; y++)
    {
        for (x = 0; x < mapwidth; x++)
        {
            tile = *start++;

            switch (tile)
            {
                case 19:
                case 20:
                case 21:
                case 22:
                    SpawnPlayer (x,y,NORTH + tile - 19);
                    break;

                case 85:     // Money bag
                case 86:     // Loot
                case 87:     // Gold
                case 88:     // Bonus
                case 23:
                case 24:
                case 25:
                case 26:
                case 27:
                case 28:
                case 29:
#if GAME_VERSION != SHAREWARE_VERSION
                case 30:         // Yellow Puddle
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
#ifdef NOTYET
                case 486:    // Plasma Detonator
                    SpawnHiddenOfs (en_plasma_detonator_reserve,x,y);   // Spawn a reserve
                    SpawnStatic (x,y,486 - 375);
                    break;
#endif
                case 487:    // Door rubble
                case 488:    // AutoMapper Bonus #1
                case 489:    // BonziTree
                case 490:    // Yellow Potted plant
                case 491:    // Tube Plant
                case 492:    // HiTech Chair
                case 493:    // AOG: Rent A Cop - Dead.
                case 494:    // AOG: Pro Guard - Dead.
                case 495:    // AOG: Swat Guard - Dead.
                    SpawnStatic (x,y,tile - 375);
                    break;

                case 392:    // crate 1
                    SpawnStatic (x,y,tile - 315);
                    break;
            }
        }
    }
}


void SetupGameLevel (void)
{
    int  x,y;
    int  lock;
    word *map,*map1;
    int  tile;

//
// load the level
//
    CA_CacheMap (gamestate.mapon + (MAPS_PER_EPISODE * gamestate.episode));

    mapwidth = mapheaderseg[gamestate.mapon]->width;
    mapheight = mapheaderseg[gamestate.mapon]->height;

    if (mapwidth != MAPSIZE || mapheight != MAPSIZE)
        Quit ("Map %d not 64x64!",gamestate.mapon + 1);

    memset (tilemap,0,sizeof(tilemap));
    memset (actorat,0,sizeof(actorat));

    map = mapsegs[0];

    for (y = 0; y < mapheight; y++)
    {
        for (x = 0; x < mapwidth; x++)
        {
            tile = *map++;

            if (tile < AMBUSHTILE)
            {
                tilemap[x][y] = tile;
                actorat[x][y] = (objtype *)tile;
            }
            else
            {
                tilemap[x][y] = 0;
                actorat[x][y] = NULL;
            }
        }
    }

    InitObjList ();
    InitDoorObjList ();
    InitStatObjList ();

    map = mapsegs[0];
    map1 = mapsegs[1];

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
#ifdef NOTYET
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
                            eaList[NumEAWalls].delay = 60 * 8 + Random(60 * 22);

                        if (NumEAWalls++ == MAXEAWALLS)
                            GAME_ERROR (SETUPGAME_MAX_EA_WALLS);
                        break;

                    case ON_SWITCH:
                        switchon = true;
                    case OFF_SWITCH:
                        *map1 = 0xf800 | UpdateBarrierTable(*map1 >> 8,*map1 & 0xff,switchon);

                        switchon = false;    // init for next time
                        break;
                }
#endif
            }

            map1++;
        }
    }

    ScanInfoPlane ();
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
= DrawPlayBorder
=
===================
*/

void DrawPlayBorder (void)
{
    int xl,yl;

    if (viewwidth == screen.width)
    {
        VW_Bar (0,TOP_STRIP_HEIGHT,320,200 - STATUSLINES - TOP_STRIP_HEIGHT,0);
        return;
    }

    xl = 160 - centerx;
    yl = (200 - STATUSLINES + TOP_STRIP_HEIGHT - viewheight) / 2;

    BevelBox (0,TOP_STRIP_HEIGHT,320,200 - STATUSLINES - TOP_STRIP_HEIGHT,BORDER_HI_COLOR,BORDER_MED_COLOR,BORDER_LO_COLOR);
    BevelBox (xl - 1,yl - 1,viewwidth + 2,viewheight + 2,BORDER_LO_COLOR,0,BORDER_HI_COLOR);
}


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
= DrawPlayScreen
=
===================
*/

void DrawPlayScreen (bool InitInfoMsg)
{
    if (loadedgame)
        return;
#ifdef NOTYET
    if (playstate != ex_transported)
        VW_FadeOut ();
#endif
    WindowW = 253;
    WindowH = 8;
    fontnumber = 2;

    DrawPlayBorder ();

    VW_DrawPic (0,200 - STATUSLINES,STATUSBARPIC);
    VW_DrawPic (0,0,TOP_STATUSBARPIC);
#ifdef NOTYET
    ShadowPrintLocationText (sp_normal);
#endif
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
==========================
=
= SetViewSize
=
==========================
*/

void SetViewSize (int size)
{
    int width,height;

    width = ((size << 4) * screen.width) / 320;
    height = (((size << 4) * HEIGHTRATIO) * screen.height) / 200;

    viewwidth = width & ~15;                  // must be divisible by 16
    viewheight = height & ~1;                 // must be even
    centerx = viewwidth / 2;
    centery = viewheight / 2;
    shootdelta = viewwidth / 10;
    normalshade = (((viewwidth * 9) >> 4) - 3) / normalshadediv;

//
// calculate trace angles and projection constants
//
    CalcProjection (FOCALLENGTH);
}
