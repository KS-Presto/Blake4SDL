//
// basic game stuff

#include "3d_def.h"


#define ATABLEMAX 15

unsigned    TopColor,BottomColor;

int         viewsize = 20;
int         viewscreenx,viewscreeny;
int         fizzlewidth,fizzleheight;
int         fizzlex,fizzley;
unsigned    screenofs;
bool        startgame,ingame,loadedgame;
fargametype gamestuff;
gametype    gamestate;

int         LS_current,LS_total;

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

    LoadLocationText (gamestate.mapon + (MAPS_PER_EPISODE * gamestate.episode));

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
#ifdef NOTYET
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
#endif
                }
            }

            map1++;
        }
    }

    ScanInfoPlane ();
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

	temp = strstr(LocationText,"^XX");

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
= BMAmsg
=
= These messages are displayed by the Text Presenter
=
===================
*/

void BMAmsg (const char *msg)
{
    #define BMAx1 0             // outer bevel
    #define BMAy1 152
    #define BMAw1 320
    #define BMAh1 48

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

    VW_DrawPic (0,200 - STATUSLINES,STATUSBARPIC);
    VW_DrawPic (0,0,TOP_STATUSBARPIC);

    ShadowPrintLocationText (sp_normal);
    UpdateStatusBar ();
    VW_UpdateScreen (screen.buffer);    // TODO: temp fix until GameLoop redraws it

    SD_PlaySound (WARPINSND);

    SetupFizzlein (viewscreenx,viewscreeny,viewwidth,viewheight);
    ThreeDRefresh ();               // TODO: this call might not be necessary
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

    VW_Bar (viewscreenx,viewscreeny,viewwidth,viewheight,0);
    VW_FizzleFade (viewscreenx,viewscreeny,viewwidth,viewheight,70,false);

    IN_UserInput (100);
    SD_WaitSoundDone ();
}


/*
===================
=
= ShPrint
=
===================
*/

void ShPrint (const char *text, int shadowcolor, bool singlechar)
{
    size_t len;
    int    oldcolor,oldx,oldy;
    char   *str,buf[2] = {'\0','\0'};

    oldcolor = fontcolor;
    oldx = px;
    oldy = py;

    if (singlechar)
    {
        str = buf;
        buf[0] = *text;
    }
    else
    {
        len = strlen(text) + 1;
        str = SafeMalloc(len);

        snprintf (str,len,text);
    }

    fontcolor = shadowcolor;
    px++;
    py++;
    VW_DrawString (str);

    fontcolor = oldcolor;
    px = oldx;
    py = oldy;
    VW_DrawString (str);

    if (!singlechar)
        free (str);
}


/*
==========================
=
= ShadowPrintLocationText
=
==========================
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

    px = 160 - (w / 2);

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

    if (viewheight == screen.height)
        viewscreenx = viewscreeny = screenofs = 0;
    else
    {
        viewscreenx = (screen.width - viewwidth) >> 1;
        viewscreeny = ((screen.height - (STATUSLINES - TOP_STRIP_HEIGHT)) - viewheight) >> 1;
        screenofs = ylookup[viewscreeny] + viewscreenx;
    }

//
// calculate trace angles and projection constants
//
    CalcProjection (FOCALLENGTH);
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


const char *prep_msg = "^ST1^CEGet Ready, Blake!\r^XX";


void DisplayPrepingMsg (const char *text)
{
    fontnumber = 1;

    BMAmsg (text);

//
// set thermometer boundaries
//
    WindowX = 36;
    WindowY = 188;
    WindowW = 260;
    WindowH = 32;

    VW_Bar (WindowX,WindowY - 7,WindowW - 10,2,BORDER_LO_COLOR);
    VW_Bar (WindowX,WindowY - 7,WindowW - 11,1,BORDER_TEXT_COLOR - 15);
    VW_Bar (WindowX,WindowY,WindowW - 10,2,BORDER_LO_COLOR);
    VW_Bar (WindowX,WindowY,WindowW - 11,1,BORDER_TEXT_COLOR - 15);

    if (screen.flags & SC_FADED)
        VW_FadeIn ();
    else
        VW_UpdateScreen (screen.buffer);
}


void PreloadUpdate (int current, int total)
{
    int w = WindowW - 10;

    if (current > total)
        current = total;

    w = ((int32_t)w * current) / total;

    if (w)
        VW_Bar (WindowX,WindowY,w - 1,1,BORDER_TEXT_COLOR);

    VW_UpdateScreen (screen.buffer);
}


void PreloadGraphics (void)
{
    WindowY = 188;

    if (!(gamestate.flags & GS_QUICKRUN))
        VW_FadeIn ();

    PreloadUpdate (10,10);
    IN_UserInput (70);

    if (playstate != ex_transported)
        VW_FadeOut ();

    DrawPlayBorder ();
    VW_UpdateScreen (screen.buffer);
}


/*
==========================
=
= GameLoop
=
==========================
*/

void GameLoop (void)
{
    SD_StopDigitized ();
    SetFontColor (0,15);
    DrawPlayScreen (true);

    while (playstate != ex_abort)
    {
        ingame = true;

        //if (!(loadedgame || LevelInPlaytemp(gamestate.mapon)))
        if (!loadedgame)
        {
            gamestate.tic_score = gamestate.score = gamestate.oldscore;
            memcpy (gamestate.numkeys,gamestate.old_numkeys,sizeof(gamestate.numkeys));
            memcpy (gamestate.barrier_table,gamestate.old_barrier_table,sizeof(gamestate.barrier_table));
            gamestate.rpower = gamestate.old_rpower;
            gamestate.tokens = gamestate.old_tokens;
            gamestate.weapons = gamestate.old_weapons[0];
            gamestate.weapon = gamestate.old_weapons[1];
            gamestate.chosenweapon = gamestate.old_weapons[2];
            gamestate.ammo = gamestate.old_ammo;
            gamestate.plasma_detonators = gamestate.old_plasma_detonators;
            gamestate.boss_key_dropped=gamestate.old_boss_key_dropped;
            memcpy (&gamestuff.level[0],gamestuff.old_levelinfo,sizeof(gamestuff.old_levelinfo));
            DrawKeys ();
            DrawScore ();
        }

        startgame = false;

        if (!loadedgame)
        {
#ifdef NOTYET
            if (LS_current == -1)
            {
                DrawTopInfo (sp_loading);
                DisplayPrepingMsg (prep_msg);
                LS_current = 1;
                LS_total = 20;
            }

            LoadLevel (gamestate.mapon);
#else
            SetupGameLevel ();
#endif
        }
        else
            loadedgame = false;

        LS_current = LS_total = -1;

        LoadPlanes (126,126);
#ifdef NOTYET
        PreloadGraphics ();
#endif
        if (playstate == ex_transported)
            DrawWarpIn ();
        else
            DrawPlayScreen (false);

        //if (!sqActive)
            StartMusic ();

        PlayLoop ();

        LS_current = LS_total = -1;

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
#ifdef NOTYET
                DrawTopInfo (sp_loading);
                DisplayPrepingMsg (prep_msg);
#endif
                WindowY = 181;
                LS_current = 1;
                LS_total = 38;
                StartMusic ();
#ifdef NOTYET
                SaveLevel (gamestate.lastmapon);
#endif
                gamestate.old_rpower = gamestate.rpower;
                gamestate.oldscore = gamestate.score;
                memcpy (gamestate.old_numkeys,gamestate.numkeys,sizeof(gamestate.old_numkeys));
                gamestate.old_tokens = gamestate.tokens;
                memcpy (gamestate.old_barrier_table,gamestate.barrier_table,sizeof(gamestate.old_barrier_table));
                gamestate.old_weapons[0] = gamestate.weapons;
                gamestate.old_weapons[1] = gamestate.weapon;
                gamestate.old_weapons[2] = gamestate.chosenweapon;
                gamestate.old_ammo = gamestate.ammo;
                gamestate.old_boss_key_dropped = gamestate.boss_key_dropped;
                memcpy (gamestuff.old_levelinfo,&gamestuff.level[0],sizeof(gamestuff.old_levelinfo));
                break;

            default:
                SD_StopDigitized ();
                break;
        }
    }
}
