//
// Door routines

#include "3d_def.h"


int8_t    pint_xy[8][2] =    // TODO: according to comments, this is y by x?
{
    {-1,-1},{-1, 0},{-1, 1},
    { 0,-1},        { 0, 1},
    { 1,-1},{ 1, 0},{ 1, 1},
};


unsigned  pwallstate;
unsigned  pwallpos;       // amount a pushable wall has been moved (0-63)
unsigned  pwallx,pwally;
int       pwalldir,pwalldist;

int       dirinc[4][2] = { {0,-1},{1,0},{0,1},{-1,0}, };

statobj_t statobjlist[MAXSTATS],*laststatobj;

stattype statinfo[] =
{
    {SPR_STAT_0,bo_water_puddle},   // Water Puddle     SPR1V
    {SPR_STAT_1,block},             // Containment Canister
    {SPR_STAT_2,block},             // Lunch Table
    {SPR_STAT_3,block},             // Floor Lamp
    {SPR_STAT_4,block},             // Lab Table
    {SPR_STAT_5,block},             // Pillar
    {SPR_STAT_6},                   // Blood Puddle
    {SPR_STAT_7},                   // Piss Puddle

    {SPR_STAT_8,block},             // Ficus Tree      SPR2V
    {SPR_STAT_9},                   // Half-Eaten Corpse
    {SPR_STAT_10,block},            // Water Fountain
    {SPR_STAT_11,block},            // Plant 1
    {SPR_STAT_12,block},            // Vase
    {SPR_STAT_13,block},            // General Table
    {SPR_STAT_14},                  // Ceiling Light
    {SPR_STAT_15,block},            // General Chair

    {SPR_STAT_16,block},            // Kitchen Trash      SPR3V
    {SPR_STAT_17},                  // Office Trash
    {SPR_STAT_18,block},            // Plant 2
    {SPR_STAT_19,block},            // Gurney No-Blood
    {SPR_STAT_20},                  // Indirect Half-Sphere
    {SPR_STAT_21},                  // Exit Sign
    {SPR_STAT_22},                  // Transporter
    {SPR_STAT_23,block},            // Body Can

    {SPR_STAT_24,bo_pistol},        // PISTOL       SPR4V
    {SPR_STAT_25,block},            // Statue

    {SPR_STAT_31,bo_clip},          // Charge Unit

    {SPR_STAT_27,bo_burst_rifle},   // Auto-Burst Rifle
    {SPR_STAT_28,bo_ion_cannon},    // Particle Charged ION
    {SPR_STAT_29,bo_firstaid},      // First Aid
    {SPR_VSPIKE8,block},            // Static VSPIKE

    {SPR_STAT_26,bo_clip2},         // Big Charge pack/clip

    {SPR_STAT_32,bo_red_key},       // Red Key        SPR5V
    {SPR_STAT_33,bo_yellow_key},    // Yellow Key
    {SPR_STAT_34,bo_bfg_cannon},    // BFG Cannon
    {SPR_STAT_35,bo_blue_key},      // Blue Key
    {SPR_STAT_36},                  // OPEN
    {SPR_STAT_37,block},            // Office Desk
    {SPR_STAT_38,block},            // Office Chair
    {SPR_STAT_39,block},            // Security Desk

    {SPR_STAT_40,bo_water},         // Full Water Bowl    SPR7V
    {SPR_STAT_41},                  // Empty Water Bowl
    {SPR_STAT_42,bo_chicken},       // Chicken Leg
    {SPR_STAT_43},                  // Chicken Bone
    {SPR_STAT_44,bo_ham},           // Ham
    {SPR_STAT_45},                  // Ham Bone
    {SPR_STAT_46,bo_grenade},       // Grande Launcher
    {SPR_STAT_47},                  // Video Game Machine

    {SPR_VPOST8,block},             // Static VPOST

//    -- VARIOUS --

    {SPR_GURNEY_MUT_READY,block},   // 49 Gurney Mutant
    {SPR_LCAN_ALIEN_READY,block},   // 50 Large Alien Canister
    {SPR_SCAN_ALIEN_READY,block},   // 51 Small Alien Canister

    {SPR_GURNEY_MUT_EMPTY,block},   // 52 Gurney Mutant
    {SPR_LCAN_ALIEN_EMPTY,block},   // 53 Large Alien Canister
    {SPR_SCAN_ALIEN_EMPTY,block},   // 54 Small Alien Canister

    {SPR_OFC_DEAD},                 // 55 Dead Gen Sci.

    {0},                            // 56 Spacer

    {SPR_AIR_VENT,bo_plainvent},    // 57 Plain air vent
    {SPR_AIR_VENT,bo_bloodvent},    // 58 Blood air vent
    {SPR_AIR_VENT,bo_watervent},    // 59 Water air vent
    {SPR_GRATE},                    // 60 Floor Grate
    {SPR_STEAM_PIPE},               // 61 Steam Pipe

    {SPR_STAT_48,bo_money_bag},     // 62 money bag
    {SPR_STAT_49,bo_loot},          // 63 loot
    {SPR_STAT_50,bo_gold},          // 64 gold
    {SPR_STAT_51,bo_bonus},         // 65 bonus

    {SPR_STAT_52, block},           // 66 Greek Post
    {SPR_STAT_53, block},           // 67 Red/Blue post
    {SPR_STAT_54, block},           // 68 Red HiTech Post
    {SPR_STAT_55},                  // 69 Ceiling Lamp #2
    {SPR_STAT_56},                  // 70 Ceiling Lamp #3
    {SPR_STAT_57},                  // 71 Body Parts
    {SPR_STAT_58},                  // 72 OR Lamp
    {SPR_STAT_59,block},            // 73 Office Sink
    {SPR_STAT_57,0},                // EMPTY - Copy of 71 - Body Parts...
    {SPR_CANDY_BAR,bo_candybar},    // 75 candy bar
    {SPR_SANDWICH,bo_sandwich},     // 76 sandwich
    {SPR_CRATE_1,block},            // 77 Crate #1
    {SPR_CRATE_2,block},            // 78 Crate #2
    {SPR_CRATE_3,block},            // 79 Crate #3
    {SPR_STAT_61,block},            // 80 Table
    {SPR_STAT_62,block},            // 81 Chair
    {SPR_STAT_63,block},            // 82 Stool
    {SPR_STAT_64},                  // 83 Gore

    {SPR_STAT_65,bo_gold3},         // Gold 3
    {SPR_STAT_66,bo_gold2},         // Gold 2
    {SPR_STAT_67,bo_gold1},         // Gold 1

    {SPR_STAT_68,block},
    {SPR_STAT_69,block},
    {SPR_STAT_70,block},
    {SPR_STAT_71,block},
    {SPR_STAT_72,block},
    {SPR_STAT_73},
    {SPR_STAT_74},
    {SPR_STAT_75},
    {SPR_STAT_76},

    {SPR_RENT_DEAD},
    {SPR_PRO_DEAD},
    {SPR_SWAT_DEAD},
    {SPR_GSCOUT_DEAD},
    {SPR_FSCOUT_DEAD},
    {SPR_MUTHUM1_DEAD},
    {SPR_MUTHUM2_DEAD},
    {SPR_LCAN_ALIEN_DEAD},
    {SPR_SCAN_ALIEN_DEAD},
    {SPR_GURNEY_MUT_DEAD},
    {SPR_TERROT_DEAD},
    {SPR_POD_DIE3},
    {SPR_STAT_77,bo_coin},          // Concession Machine Money
    {SPR_STAT_78,bo_coin5},         // Concession Machine Money
    {SPR_STAT_79},                  // Auto-Charge Pistol

    {SPR_DOORBOMB,bo_plasma_detonator},   // Plasma Detonator
    {SPR_RUBBLE},                   // Door Rubble
    {SPR_AUTOMAPPER,bo_automapper1},      // Auto Mapper Bonus #1
    {SPR_BONZI_TREE,block},         // BonziTree
    {SPR_POT_PLANT,block},          // Yellow Potted Plant
    {SPR_TUBE_PLANT,block},         // Tube Plant
    {SPR_HITECH_CHAIR,block},       // Hi Tech table and chair
    {SPR_DEAD_RENT},                // Dead AOG: Rent A Cop
    {SPR_DEAD_PRO},                 // Dead AOG: Pro Guard
    {SPR_DEAD_SWAT},                // Dead AOG: Swat Guad

    {-1}                            // terminator
};


/*
===============
=
= InitStatObjList
=
===============
*/

void InitStatObjList (void)
{
    memset (statobjlist,0,sizeof(statobjlist));

	laststatobj = &statobjlist[0];
}


/*
================
=
= FindStatic
=
= Searches statobjlist and returns ptr to a statobj
= at the given tile
=
= Returns pointer to static obj, or NULL if no static found
=
================
*/

statobj_t *FindStatic (int tilex, int tiley)
{
    statobj_t *spot;

    for (spot = &statobjlist[0]; spot != laststatobj; spot++)
    {
        if (spot->shapenum != -1 && spot->tilex == tilex && spot->tiley == tiley)
            return spot;
    }

    return NULL;
}


/*
================
=
= FindEmptyStatic
=
= Searches statobjlist and returns ptr to an empty statobj
=
= Returns pointer to empty static obj, or NULL if statobjlist full
=
================
*/

statobj_t *FindEmptyStatic (void)
{
    statobj_t *spot;

    for (spot = &statobjlist[0]; ; spot++)
    {
        if (spot == laststatobj)
        {
            if (spot == &statobjlist[MAXSTATS])    // TODO: this is a BO, but make sure fixing it doesn't break anything
                return NULL;

            laststatobj++;              // space at end
            break;
        }

        if (spot->shapenum == -1)       // -1 is a free spot
            break;
    }

    return spot;
}


/*
===============
=
= SpawnStatic
=
===============
*/

void SpawnStatic (int tilex, int tiley, int type)
{
    statobj_t *spot;

    spot = FindEmptyStatic();

    if (!spot)
        return;

    spot->shapenum = statinfo[type].picnum;
    spot->tilex = tilex;
    spot->tiley = tiley;
    spot->flags = 0;

#if GAME_VERSION == SHAREWARE_VERSION
    if (!spot->shapenum)
        Quit ("Invalid static: %dx%d",tilex,tiley);
#endif
    switch (spot->shapenum)
    {
#if GAME_VERSION != SHAREWARE_VERSION
        case SPR_STAT_3:    // floor lamp
#endif
        case SPR_STAT_14:   // ceiling light
#if GAME_VERSION != SHAREWARE_VERSION
        case SPR_STAT_20:
#endif
        case SPR_STAT_47:
        case SPR_STAT_51:
        case SPR_STAT_55:
        case SPR_STAT_56:
            spot->lighting = LAMP_ON_SHADING;
            break;

        default:
            spot->lighting = 0;
            break;
    }

    switch (statinfo[type].type)
    {
        case block:
            actorat[tilex][tiley] = (objtype *)BLOCKTILE;  // consider it a blocking tile
            break;

        case bo_red_key:
        case bo_yellow_key:
        case bo_blue_key:
        case bo_plasma_detonator:
            TravelTable[tilex][tiley] |= TT_KEYS;

        case bo_gold1:
        case bo_gold2:
        case bo_gold3:
        case bo_gold:
        case bo_bonus:
        case bo_money_bag:
        case bo_loot:

        case bo_fullheal:
        case bo_firstaid:
        case bo_clip:
        case bo_clip2:
        case bo_burst_rifle:
        case bo_ion_cannon:
        case bo_grenade:
        case bo_bfg_cannon:
        case bo_pistol:
        case bo_chicken:
        case bo_ham:
        case bo_water:
        case bo_water_puddle:
        case bo_sandwich:
        case bo_candybar:
        case bo_coin:
        case bo_coin5:
        case bo_automapper1:
            spot->flags = FL_BONUS;
            spot->itemnumber = statinfo[type].type;
            break;
    }

    spot->areanumber = GetAreaNumber(spot->tilex,spot->tiley);

    spot++;

    if (spot == &statobjlist[MAXSTATS])    // TODO: again BO, make sure fixing doesn't break it
        Quit ("Too many static objects!");
}


/*
===============
=
= ReserveStatic
=
= Reserves a static object at location 0,0 (unseen). This function is
= used to guarantee that a static will be available.
=
===============
*/

statobj_t *ReserveStatic (void)
{
    statobj_t *spot;

    spot = FindEmptyStatic();

    if (!spot)
        Quit ("Too many static objects!");

    //
    // mark as used
    //
    spot->shapenum = 1;
    spot->tilex = 0;
    spot->tiley = 0;

    return spot;
}


/*
===============
=
= FindReservedStatic
=
= Finds a reserved static object at location 0,0 (unseen). This function is
= used to guarantee that a static will be available.
=
===============
*/

statobj_t *FindReservedStatic (void)
{
    statobj_t *spot;

    for (spot = &statobjlist[0]; spot < &statobjlist[MAXSTATS]; spot++)  // TODO: BO cond, fixit but don't break
    {
        if (spot->shapenum == 1 && !spot->tilex && !spot->tiley)    // -1 is a free spot
            return spot;
    }

    return NULL;
}


/*
===============
=
= UseReservedStatic
=
= Finds a reserved static object and moves it to a new location with new
= attributes.
=
= This function acts like PlaceItemType - but REQUIRES a reserved
= static. Before using this function, make sure that you have already
= reserved a static to be used using ReserveStatic
=
===============
*/

statobj_t *UseReservedStatic (int itemtype, int tilex, int tiley)
{
    statobj_t *spot;
    int       type;

    spot = FindReservedStatic();

    if (!spot)
        Quit ("Could not find a reserved static at location 0,0 with shapenum == 1");

    //
    // find the item number
    //

    for (type = 0; ; type++)
    {
        if (statinfo[type].picnum == -1)        // end of list
            Quit ("Couldn't find type!");

        if (statinfo[type].type == itemtype)    // bingo, found it!
            break;
    }

    //
    // place it
    //
    switch (type)
    {
        case bo_red_key:
        case bo_yellow_key:
        case bo_blue_key:
            TravelTable[tilex][tiley] |= TT_KEYS;
            break;
    }

    spot->shapenum = statinfo[type].picnum;
    spot->tilex = tilex;
    spot->tiley = tiley;
    spot->flags = FL_BONUS;
    spot->itemnumber = statinfo[type].type;
    spot->areanumber = GetAreaNumber(spot->tilex,spot->tiley);

    return spot;
}


/*
===============
=
= PlaceReservedItemNearTile
=
===============
*/

void PlaceReservedItemNearTile (int itemtype, int tilex, int tiley)
{
    int     i;
    int     x,y;
    objtype *check;

    for (i = 0; i < 8; i++)
    {
        x = tilex + pint_xy[i][1];
        y = tiley + pint_xy[i][0];

        if (!tilemap[x][y])
        {
            check = actorat[x][y];

            if (!ISPOINTER(check) && (uintptr_t)check == BLOCKTILE)    // check for a SOLID static
                continue;

            UseReservedStatic (itemtype,x,y);
            return;
        }
    }

    UseReservedStatic (itemtype,tilex,tiley);
}



/*
===============
=
= PlaceItemType
=
= Called during game play to drop actors' items. It finds the proper
= item number based on the item type (bo_???). If there are no free item
= spots, nothing is done.
=
===============
*/

void PlaceItemType (int itemtype, int tilex, int tiley)
{
    int       type;
    statobj_t *spot;

    //
    // find the item number
    //
    for (type = 0; ; type++)
    {
        if (statinfo[type].picnum == -1)        // end of list
            Quit ("Couldn't find type!");

        if (statinfo[type].type == itemtype)
            break;
    }

    //
    // find a spot in statobjlist to put it in
    //
    spot = FindEmptyStatic();

    if (!spot)
        return;

    //
    // place it
    //
    spot->shapenum = statinfo[type].picnum;
    spot->tilex = tilex;
    spot->tiley = tiley;
    spot->flags = FL_BONUS;
    spot->itemnumber = statinfo[type].type;
    spot->areanumber = GetAreaNumber(spot->tilex,spot->tiley);
}


/*
===============
=
= PlaceItemNearTile
=
===============
*/

void PlaceItemNearTile (int itemtype, int tilex, int tiley)
{
// [0] is the y offset
// [1] is the x offset
//
    int     i;
    int     x,y;
    objtype *check;

    for (i = 0; i < 8; i++)
    {
        x = tilex + pint_xy[i][1];
        y = tiley + pint_xy[i][0];

        if (!tilemap[x][y])
        {
            check = actorat[x][y];

            if (!ISPOINTER(check) && (uintptr_t)check == BLOCKTILE)      // check for a SOLID static
                continue;

            PlaceItemType (itemtype,x,y);
            return;
        }
    }

    PlaceItemType (itemtype,tilex,tiley);
}


/*
===============
=
= ExplodeStatics
=
= Explodes statics in a one tile radius from a given tile x and tile y
=
===============
*/

void ExplodeStatics (int tilex, int tiley)
{
#ifdef NOTYET
    statobj_t *statobj,*spot;
    objtype   *obj;
    int       ydiff,xdiff;
    fixed     x,y;
    bool      remove;

    for (spot = &statobjlist[0]; spot != laststatobj; spot++)
    {
        if (spot->shapenum != -1)
        {
            ydiff = abs(spot->tiley - tiley);
            xdiff = abs(spot->tilex - tilex);

            if (xdiff < 2 && ydiff < 2)
            {
                remove = false;

                //
                // test for specific statics
                //
                switch (spot->itemnumber)
                {
                    //
                    // check for clips
                    //
                    case bo_clip:
                    case bo_clip2:
                        remove = true;
                        x = ((fixed)spot->tilex << TILESHIFT) + (TILEGLOBAL - 1);
                        y = ((fixed)spot->tiley << TILESHIFT) + (TILEGLOBAL - 1);
                        SpawnCusExplosion (x,y,SPR_CLIP_EXP1,7,3 + (US_RndT() & 3),explosionobj);
                        break;
                }

                //
                // check to see if we remove it
                //
                if (remove)
                {
                    spot->shapenum = -1;
                    spot->itemnumber = bo_nothing;
                }
            }
        }
    }
#endif
}


/*
=============================================================================

                            DOORS

=============================================================================
=
= doorobjlist[] holds most of the information for the doors
=
= door->position holds the amount the door is open, ranging from 0 to (TILEGLOBAL - 1)
=
= The number of doors is limited to 64 because a spot in tilemap holds the
= door number in the low 6 bits, with the high bit meaning a door center
= and bit 6 meaning a door side tile
=
= Open doors conect two areas, so sounds will travel between them and sight
= will be checked when the player is in a connected area.
=
= Areaconnect is incremented/decremented by each door. If > 0 they connect
=
= Every time a door opens or closes the areabyplayer matrix gets recalculated.
= An area is true if it connects with the player's current spor.
=
=============================================================================
*/

#define OPENTICS 300

doorobj_t doorobjlist[MAXDOORS],*lastdoorobj;
int       doornum;

byte      areaconnect[NUMAREAS][NUMAREAS];

bool      areabyplayer[NUMAREAS];


/*
==============
=
= InitAreas
=
==============
*/

void InitAreas (void)
{
    memset (areabyplayer,0,sizeof(areabyplayer));
    areabyplayer[player->areanumber] = true;
}


/*
==============
=
= ConnectAreas
=
= Scans outward from playerarea, marking all connected areas
=
==============
*/

void RecursiveConnect (int areanumber)
{
    int i;

    for (i = 0; i < NUMAREAS; i++)
    {
        if (areaconnect[areanumber][i] && !areabyplayer[i])
        {
            areabyplayer[i] = true;
            RecursiveConnect (i);
        }
    }
}


void ConnectAreas (void)
{
    InitAreas ();
    RecursiveConnect (player->areanumber);
}


/*
===============
=
= InitDoorObjList
=
===============
*/

void InitDoorObjList (void)
{
    memset (areabyplayer,0,sizeof(areabyplayer));
    memset (areaconnect,0,sizeof(areaconnect));
    memset (doorobjlist,0,sizeof(doorobjlist));

    lastdoorobj = &doorobjlist[0];
    doornum = 0;
}


/*
===============
=
= SpawnDoor
=
===============
*/

void SpawnDoor (int tilex, int tiley, bool vertical, int lock, int type)
{
    word *map;

    if (doornum == MAXDOORS)
        Quit ("64+ doors on level!");

    lastdoorobj->position = 0;      // doors start out fully closed
    lastdoorobj->tilex = tilex;
    lastdoorobj->tiley = tiley;
    lastdoorobj->vertical = vertical;
    lastdoorobj->lock = lock;
    lastdoorobj->type = type;
    lastdoorobj->action = dr_closed;
    //lastdoorobj->flags = DR_BLASTABLE; // JIM - Do something with this! jdebug

    if (vertical)
    {
        lastdoorobj->areanumber[0] = GetAreaNumber(tilex + 1,tiley);
        lastdoorobj->areanumber[1] = GetAreaNumber(tilex - 1,tiley);
    }
    else
    {
        lastdoorobj->areanumber[0] = GetAreaNumber(tilex,tiley - 1);
        lastdoorobj->areanumber[1] = GetAreaNumber(tilex,tiley + 1);
    }

    actorat[tilex][tiley] = (objtype *)(doornum | 0x80);    // consider it a solid wall

//
// make the door tile a special tile, and mark the adjacent tiles
// for door sides
//
    tilemap[tilex][tiley] = doornum | 0x80;

    map = &MAPSPOT(tilex,tiley,0);

    if (vertical)
    {
        if (*(map - mapwidth - 1) == TRANSPORTERTILE)
            *map = GetAreaNumber(tilex + 1,tiley);
        else
            *map = GetAreaNumber(tilex - 1,tiley);

        tilemap[tilex][tiley - 1] |= 0x40;
        tilemap[tilex][tiley + 1] |= 0x40;
    }
    else
    {
        *map = GetAreaNumber(tilex,tiley - 1);

        tilemap[tilex - 1][tiley] |= 0x40;
        tilemap[tilex + 1][tiley] |= 0x40;
    }

    doornum++;
    lastdoorobj++;
}


/*
===============
=
= CheckLinkedDoors
=
= TODO: this was a confusing mess, make sure it's working properly!
=
===============
*/

void CheckLinkedDoors (int door, int dir)
{
    static int linkcheck = 0;
    static int basetilex;
    static int basetiley;
    int        tilex,tiley;
    int        nexttilex,nexttiley;
    unsigned   value;

    tilex = doorobjlist[door].tilex,
    tiley = doorobjlist[door].tiley,
    nexttilex = 0,
    nexttiley = 0;

    //
    // find next door in link.
    //
    value = MAPSPOT(tilex,tiley,1);

    if (value)
    {
        //
        // define the next door in the link
        //
        nexttilex = (value & 0xff00) >> 8;
        nexttiley = value & 0xff;

        //
        // is this the head of the link?
        //
        if (!linkcheck)
        {
            basetilex = tilex;
            basetiley = tiley;
        }
    }

    linkcheck++;

    //
    // recursively open/close linked doors
    //
    if (nexttilex && nexttiley && (nexttilex != basetilex || nexttiley != basetiley))
    {
        door = tilemap[nexttilex][nexttiley] & ~0x80;

        switch (dir)
        {
            case dr_opening:
                doorobjlist[door].lock = kt_none;
                OpenDoor (door);
                break;

            case dr_closing:
                doorobjlist[door].lock = kt_none;
                CloseDoor (door);
                break;
        }
    }

    linkcheck--;
}


/*
=====================
=
= OpenDoor
=
=====================
*/

void OpenDoor (int door)
{
    if (doorobjlist[door].action == dr_jammed)
        return;

    if (doorobjlist[door].action == dr_open)
        doorobjlist[door].ticcount = 0;   // reset open time
    else
        doorobjlist[door].action = dr_opening; // start it opening

    CheckLinkedDoors (door,dr_opening);
}


/*
=====================
=
= CloseDoor
=
=====================
*/

void CloseDoor (int door)
{
    int     tilex,tiley;
    objtype *check;

    if (doorobjlist[door].action == dr_jammed)
        return;

//
// don't close on anything solid
//
    tilex = doorobjlist[door].tilex;
    tiley = doorobjlist[door].tiley;

    if (actorat[tilex][tiley])
        return;

    if (player->tilex == tilex && player->tiley == tiley)
        return;

    if (doorobjlist[door].vertical)
    {
        if (player->tiley == tiley)
        {
            if (((player->x + MINDIST) >> TILESHIFT) == tilex)
                return;
            if (((player->x - MINDIST) >> TILESHIFT) == tilex)
                return;
        }

        check = actorat[tilex - 1][tiley];

        if (ISPOINTER(check) && ((check->x + MINDIST) >> TILESHIFT) == tilex)
            return;

        check = actorat[tilex + 1][tiley];

        if (ISPOINTER(check) && ((check->x - MINDIST) >> TILESHIFT) == tilex)
            return;
    }
    else
    {
        if (player->tilex == tilex)
        {
            if (((player->y + MINDIST) >> TILESHIFT) == tiley)
                return;

            if (((player->y - MINDIST) >> TILESHIFT) == tiley)
                return;
        }

        check = actorat[tilex][tiley - 1];

        if (ISPOINTER(check) && ((check->y + MINDIST) >> TILESHIFT) == tiley)
            return;

        check = actorat[tilex][tiley + 1];

        if (ISPOINTER(check) && ((check->y - MINDIST) >> TILESHIFT) == tiley)
            return;
    }

//
// play door sound if in a connected area
//
    if (areabyplayer[GetAreaNumber(tilex,tiley)])
    {
        switch (doorobjlist[door].type)
        {
            case dr_bio:
            case dr_office:
            case dr_space:
            case dr_normal:
                PlaySoundLocTile (HTECHDOORCLOSESND,tilex,tiley);
                break;

            default:
                PlaySoundLocTile (CLOSEDOORSND,tilex,tiley);
                break;
        }
    }

    doorobjlist[door].action = dr_closing;

    //
    // make the door space solid
    //
    actorat[tilex][tiley] = (objtype *)(door | 0x80);

    CheckLinkedDoors (door,dr_closing);
}


/*
=====================
=
= OperateDoor
=
= The player wants to change the door's direction
=
=====================
*/

char od_oneway[] = "\r\r   DOOR LOCKED FROM\r      THIS SIDE.\r^XX";
char od_locked[] = "\r\r   DOOR PERMANENTLY\r        LOCKED.\r^XX";
char od_reddenied[] = "\r\r      RED LEVEL\r    ACCESS DENIED!\r^XX";
char od_yellowdenied[] = "\r\r     YELLOW LEVEL\r    ACCESS DENIED!\r^XX";
char od_bluedenied[] = "\r\r      BLUE LEVEL\r    ACCESS DENIED!\r^XX";
char od_granted[] = "\r\r    ACCESS GRANTED\r    DOOR UNLOCKED.\r^XX";
char od_operating[] = "\r\r    OPERATING DOOR.\r^XX";

void OperateDoor (int door)
{
    int  lock;
    bool oneway = false;

    //
    // check for wrong way on a ONEWAY door
    //
    switch (doorobjlist[door].type)
    {
        case dr_oneway_left:
            if (player->tilex < doorobjlist[door].tilex)
                oneway = true;
            break;

        case dr_oneway_right:
            if (player->tilex > doorobjlist[door].tilex)
                oneway = true;
            break;

        case dr_oneway_up:
            if (player->tiley < doorobjlist[door].tiley)
                oneway = true;
            break;

        case dr_oneway_down:
            if (player->tiley > doorobjlist[door].tiley)
                oneway = true;
            break;
    }

    if (oneway)
    {
        if (doorobjlist[door].action == dr_closed)
        {
            DISPLAY_TIMED_MSG (od_oneway,MP_DOOR_OPERATE,MT_GENERAL);
            SD_PlaySound (NOWAYSND);
        }

        return;
    }

    //
    // check for possibly being locked
    //
    lock = doorobjlist[door].lock;

    if (lock != kt_none)
    {
        if (!(gamestate.numkeys[lock - kt_red]))
        {
            SD_PlaySound (NOWAYSND);

            switch (lock)
            {
                case kt_red:
                    DISPLAY_TIMED_MSG (od_reddenied,MP_DOOR_OPERATE,MT_GENERAL);
                    break;

                case kt_yellow:
                    DISPLAY_TIMED_MSG (od_yellowdenied,MP_DOOR_OPERATE,MT_GENERAL);
                    break;

                case kt_blue:
                    DISPLAY_TIMED_MSG (od_bluedenied,MP_DOOR_OPERATE,MT_GENERAL);
                    break;

                default:
                    DISPLAY_TIMED_MSG (od_locked,MP_DOOR_OPERATE,MT_GENERAL);
                    break;
            }

            return;
        }
        else
        {
            //
            // unLock door
            //
            TakeKey (lock - kt_red);

            DISPLAY_TIMED_MSG (od_granted,MP_DOOR_OPERATE,MT_GENERAL);

            doorobjlist[door].lock = kt_none;
        }
    }
    else
        DISPLAY_TIMED_MSG (od_operating,MP_DOOR_OPERATE,MT_GENERAL);

    switch (doorobjlist[door].action)
    {
        case dr_closed:
        case dr_closing:
            OpenDoor (door);
            break;

        case dr_open:
        case dr_opening:
            CloseDoor (door);
            break;
    }
}


/*
=====================
=
= BlockDoorOpen
=
=====================
*/

void BlockDoorOpen (int door)
{
    doorobjlist[door].action = dr_jammed;
    doorobjlist[door].ticcount = 0;
    doorobjlist[door].position = TILEGLOBAL - 1;
    doorobjlist[door].lock = kt_none;
    doorobjlist[door].flags &= ~DR_BLASTABLE;

    actorat[doorobjlist[door].tilex][doorobjlist[door].tiley] = NULL;
    //tilemap[doorobjlist[door].tilex][doorobjlist[door].tiley] = 0;

    TransformAreas (doorobjlist[door].tilex,doorobjlist[door].tiley,1);
    //SpawnStatic (doorobjlist[door].tilex,doorobjlist[door].tiley,DOOR_RUBBLE);

}

#ifdef NOTYET
/*
=====================
=
= TryBlastDoor
=
=====================
*/

void TryBlastDoor (int door)
{
    fixed doorx,doory;

    switch (doorobjlist[door].type)
    {
        case dr_oneway_left:
        case dr_oneway_up:
        case dr_oneway_right:
        case dr_oneway_down:
            break;

        default:
            if (doorobjlist[door].position < (TILECENTER - 1)
             && doorobjlist[door].action != dr_jammed
             && doorobjlist[door].lock == kt_none)
            {
                BlockDoorOpen (door);

                doorx = ((fixed)doorobjlist[door].tilex << TILESHIFT) + (TILECENTER - 1);
                doory = ((fixed)doorobjlist[door].tiley << TILESHIFT) + (TILECENTER - 1);

                SpawnCusExplosion (doorx,doory,SPR_EXPLOSION_1,4,3,doorexplodeobj);
            }
            break;
    }
}


/*
=====================
=
= BlastNearDoors
=
= TODO: This looks dangerous
=
=====================
*/

void BlastNearDoors (int tilex, int tiley)
{
    byte door;
    char *doorptr;
    int  x,y;

    doorptr = (char *)&tilemap[tilex][tiley];

    for (x = -1; x < 2; x++)
    {
        for (y = -64; y < 128; y += 64)
        {
            door = *(doorptr + x + y)

            if (door & 0x80)
            {
                door &= ~0x80;

                TryBlastDoor (door);
            }
        }
    }
}
#endif

/*
=====================
=
= DropPlasmaDetonator
=
= Move a chaff from reserve to the player location
=
=====================
*/
#ifdef NOTYET
void DropPlasmaDetonator(void)
{
    objtype *obj;

    obj = MoveHiddenOfs(plasma_detonator_reserveobj,plasma_detonatorobj,player->x,player->y)

    if (obj)
    {
        obj->flags |= FL_SHOOTABLE;

        DISPLAY_TIMED_MSG (pd_dropped,MP_DOOR_OPERATE,MT_GENERAL);
        SD_PlaySound (ROBOT_SERVOSND);  // jdebug-This sound will probly change.
        TakePlasmaDetonator (1);

        return;
    }

    Quit ("Could not find Fission/Plasma Detonator reserve object!");
}


/*
=====================
=
= TryDropPlasmaDetonator
=
= Check if player is close enough to drop a detonator
=
=====================
*/

void TryDropPlasmaDetonator (void)
{
    #define MAX_RANGE_DIST    2

    objtype *obj;
    int     dx,dy,dist;

    if (!gamestuff.level[gamestate.mapon + 1].locked)
    {
        DISPLAY_TIMED_MSG (pd_floornotlocked,MP_DETONATOR,MT_GENERAL);
        return;
    }

    if (gamestate.mapon > GOLD_MORPH_LEVEL)
    {
        DISPLAY_TIMED_MSG (pd_no_computer,MP_DETONATOR,MT_GENERAL);
        return;
    }

    if (!gamestate.plasma_detonators)
    {
        DISPLAY_TIMED_MSG (pd_donthaveany,MP_DETONATOR,MT_GENERAL);
        return;
    }

    obj = FindObj(rotating_cubeobj,-1,-1);

    if (!obj)
        Quit ("Counl not find level computer - Need to have one pal!");

    if (obj->areanumber != player->areanumber)
    {
        DISPLAY_TIMED_MSG (pd_notnear,MP_DETONATOR,MT_GENERAL);
        return;
    }

    dx = abs(player->tilex - obj->tilex);
    dy = abs(player->tiley - obj->tiley);
    dist = dx > dy ? dx : dy;

    if (dist > MAX_RANGE_DIST)
    {
        DISPLAY_TIMED_MSG (pd_getcloser,MP_DETONATOR,MT_GENERAL);
        return;
    }
    else
        DropPlasmaDetonator ();
}
#endif


/*
===============
=
= DoorOpen
=
= Close the door after three seconds
=
===============
*/

void DoorOpen (int door)
{
    doorobjlist[door].ticcount += tics;

    if (doorobjlist[door].ticcount >= OPENTICS)
        CloseDoor (door);
}


/*
===============
=
= TransformAreas
=
===============
*/

unsigned TransformAreas (int tilex, int tiley, int xform)
{
    int      xofs,yofs;
    unsigned area1,area2;

//
// Is this walkway:  Horizontal?   Vertical?   Error?
//
    if (tilemap[tilex][tiley + 1] && tilemap[tilex][tiley - 1])
    {
        xofs = 1;
        yofs = 0;
    }
    else if (tilemap[tilex + 1][tiley] && tilemap[tilex - 1][tiley])
    {
        xofs = 0;
        yofs = 1;
    }
    else
        Quit ("Invalid linkable area!");

//
// define the two areas
//
    area1 = GetAreaNumber(tilex + xofs,tiley + yofs);

    if (area1 >= NUMAREAS)
        Quit ("Area1 out of range!");

    area2 = GetAreaNumber(tilex - xofs,tiley - yofs);

    if (area2 >= NUMAREAS)
        Quit ("Area2 out of range!");

//
// connect these two areas
//
    areaconnect[area1][area2] += xform;
    areaconnect[area2][area1] += xform;

    ConnectAreas ();

    return area1;
}


/*
===============
=
= DoorOpening
=
===============
*/

void DoorOpening (int door)
{
    int     tilex,tiley;
    int32_t position;

    tilex = doorobjlist[door].tilex;
    tiley = doorobjlist[door].tiley;
    position = doorobjlist[door].position;

    if (!position)
    {
        if (areabyplayer[TransformAreas(tilex,tiley,1)])
        {
            switch (doorobjlist[door].type)
            {
                case dr_bio:
                case dr_office:
                case dr_space:
                case dr_normal:
                    PlaySoundLocTile (HTECHDOOROPENSND,tilex,tiley);
                    break;

                default:
                    PlaySoundLocTile (OPENDOORSND,tilex,tiley);
                    break;
            }
        }
    }

    //
    // slide the door by an adaptive amount
    //
    position += tics << 10;

    if (position >= TILEGLOBAL - 1)
    {
        //
        // door is all the way open
        //
        position = TILEGLOBAL - 1;
        doorobjlist[door].ticcount = 0;
        doorobjlist[door].action = dr_open;

        actorat[tilex][tiley] = NULL;
    }

    doorobjlist[door].position = position;
}


/*
===============
=
= DoorClosing
=
===============
*/

void DoorClosing (int door)
{
    int     tilex,tiley;
    int32_t position;

    tilex = doorobjlist[door].tilex;
    tiley = doorobjlist[door].tiley;
    position = doorobjlist[door].position;

    if (((uintptr_t)actorat[tilex][tiley] != (door | 0x80)) || (player->tilex == tilex && player->tiley == tiley) )
    {
        //
        // something got inside the door
        //
        OpenDoor (door);

        return;
    }

    //
    // slide the door by an adaptive amount
    //
    position -= tics << 10;

    if (position <= 0)
    {
        //
        // door is closed all the way, so disconnect the areas
        //
        position = 0;
        doorobjlist[door].action = dr_closed;

        TransformAreas (tilex,tiley,-1);
    }

    doorobjlist[door].position = position;
}


/*
=====================
=
= MoveDoors
=
= Called from PlayLoop
=
=====================
*/

void MoveDoors (void)
{
    int door;

    for (door = 0; door < doornum; door++)
    {
        switch (doorobjlist[door].action)
        {
            case dr_open:
                DoorOpen (door);
                break;

            case dr_opening:
                DoorOpening (door);
                break;

            case dr_closing:
                DoorClosing (door);
                break;
        }
    }
}


/*
=============================================================================

                        PUSHABLE WALLS

=============================================================================
*/


/*
===============
=
= PushWall
=
===============
*/

void PushWall (int checkx, int checky, int dir)
{
    int oldtile;
    int dx,dy;

    if (pwallstate)
        return;

    TransformAreas (checkx,checky,1);

    oldtile = tilemap[checkx][checky];

    if (!oldtile)
        return;

    dx = dirinc[dir][0];
    dy = dirinc[dir][1];

    if (actorat[checkx + dx][checky + dy])
        return;

    actorat[checkx + dx][checky + dy] = (objtype *)oldtile;
    tilemap[checkx + dx][checky + dy] = oldtile;

    pwalldist = 2;
    pwallx = checkx;
    pwally = checky;
    pwalldir = dir;
    pwallstate = 1;
    pwallpos = 0;
    tilemap[pwallx][pwally] |= 0xc0;
    MAPSPOT(pwallx,pwally,1) = 0;               // remove P tile info

    SD_PlaySound (PUSHWALLSND);
}


/*
=================
=
= MovePWalls
=
=================
*/

void MovePWalls (void)
{
    int      oldblock,oldtile;
    int      dx,dy;
    unsigned areanumber;

    if (!pwallstate)
        return;

    oldblock = pwallstate >> 7;

    pwallstate += tics << 2;

    if (pwallstate >> 7 != oldblock)
    {
        pwalldist--;

        //
        // block crossed into a new block
        //
        oldtile = tilemap[pwallx][pwally] & 63;

        //
        // the tile can now be walked into
        //
        tilemap[pwallx][pwally] = 0;
        actorat[pwallx][pwally] = NULL;

        areanumber = GetAreaNumber(player->tilex,player->tiley);

        if (GAN_HiddenArea)
            areanumber += HIDDENAREATILE;
        else
            areanumber += AREATILE;

        MAPSPOT(pwallx,pwally,0) = areanumber;

        //
        // see if it should be pushed farther
        //
        if (!pwalldist)  // TODO: heh, looks like they fixed the 3 move bug their own way
        {
            //
            // the block has been pushed two tiles
            //
            pwallstate = 0;
            return;
        }
        else
        {
            dx = dirinc[pwalldir][0];
            dy = dirinc[pwalldir][1];

            pwallx += dx;
            pwally += dy;

            if (actorat[pwallx + dx][pwally + dy])
            {
                pwallstate = 0;
                return;
            }

            actorat[pwallx + dx][pwally + dy] = (objtype *)oldtile;
            tilemap[pwallx + dx][pwally + dy] = oldtile;

            tilemap[pwallx][pwally] = oldtile | 0xc0;
        }
    }

    pwallpos = (pwallstate >> 1) & 63;
}
