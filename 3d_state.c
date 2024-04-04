// 3D_STATE.C

#include "3d_def.h"


#define MINSIGHT    (TILEGLOBAL + TILECENTER)


bool  nevermark;

const int dirtoangle[NUMOBJDIRS] = {0,ANG45,ANG90,ANG135,ANG180,ANG225,ANG270,ANG315,0};
const int oppdirinc[NUMOBJDIRS][2] = { {-1,0},{-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{0,0} };
const int opposite[NUMOBJDIRS] = {west,southwest,south,southeast,east,northeast,north,northwest,nodir};
const int diagonal[NUMOBJDIRS][NUMOBJDIRS] =
{
    {nodir,nodir,northeast,nodir,nodir,nodir,southeast,nodir,nodir},
    {nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir},
    {northeast,nodir,nodir,nodir,northwest,nodir,nodir,nodir,nodir},
    {nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir},
    {nodir,nodir,northwest,nodir,nodir,nodir,southwest,nodir,nodir},
    {nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir},
    {southeast,nodir,nodir,nodir,southwest,nodir,nodir,nodir,nodir},
    {nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir},
    {nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir,nodir},
};

const int adjangletable[2][8] =
{
    {ANG225,ANG270,ANG315,ANG360,ANG45,ANG90,ANG135,ANG180},      // upper bound
    {ANG180,ANG225,ANG270,ANG315,0,ANG45,ANG90,ANG135},           // lower bound
};


bool    ElevatorFloor (int tilex, int tiley);


/*
===================
=
= SpawnNewObj
=
= Spawns a new actor at the given TILE coordinates with the given state
=
= Returns a pointer to an initialized new actor
=
===================
*/

objtype *SpawnNewObj (int tilex, int tiley, statetype *state)
{
    objtype *newobj = NULL;

    newobj = GetNewObj();

    newobj->state = state;
    newobj->ticcount = Random(state->tictime) + 1;

    newobj->tilex = tilex;
    newobj->tiley = tiley;
    newobj->x = ((fixed)tilex << TILESHIFT) + TILECENTER;
    newobj->y = ((fixed)tiley << TILESHIFT) + TILECENTER;
    newobj->dir = newobj->trydir = nodir;

    if (!nevermark)
    {
        if (!actorat[tilex][tiley])
            actorat[tilex][tiley] = newobj;
    }

    newobj->areanumber = GetAreaNumber(newobj->tilex,newobj->tiley);

#ifdef IN_DEVELOPMENT
    if (newobj->areanumber >= NUMAREAS && !nevermark)
        Quit ("Actor spawned on wall at %dx%d",newobj->tilex,newobj->tiley);
#endif
    return newobj;
}


/*
===================
=
= NewState
=
= Changes obj to a new state, setting ticcount to the max for that state
=
===================
*/

void NewState (objtype *obj, statetype *state)
{
    obj->state = state;
    obj->ticcount = state->tictime;
}


/*
=============================================================================

                    ENEMY TILE WORLD MOVEMENT CODE

=============================================================================
*/


/*
==================================
=
= TryWalk
=
= Attempts to move obj in its current (obj->dir) direction.
=
= If blocked by either a wall or an actor returns FALSE
=
= If move is either clear or blocked only by a door, returns TRUE and sets
=
= obj->tilex        = new destination
= obj->tiley
= obj->areanumber   = the floor tile number (0 - (NUMAREAS - 1)) of destination
= obj->distance     = TILEGLOBAl, or ~doornumber if a door is blocking the way
=
= If a door is in the way, an OpenDoor call is made to start it opening.
= The actor code should wait until doorobjlist[~obj->distance].action = dr_open,
= meaning the door has been fully opened
=
==================================
*/

#define CHECKDIAG(x,y)        \
{                                                   \
    temp = (uintptr_t)actorat[x][y];                   \
                                                \
    if (temp)                                       \
    {                                               \
        if (temp < 256)               \
            return false;                           \
        if (actorat[x][y]->flags & FL_SOLID)  \
            return false;                           \
    }                                               \
                                                    \
    if (ElevatorFloor(x,y))       \
        return false;                         \
}

#define CHECKSIDE(x,y)        \
{                                                   \
    temp = (uintptr_t)actorat[x][y];                   \
                                        \
    if (temp)                                       \
    {                                               \
        if (temp < 128)                               \
            return false;                           \
                                            \
        if (temp < 256)                               \
        {                                         \
            doornum = temp & 63;                     \
                                                    \
            if (doorobjlist[doornum].lock != kt_none)   \
                return false;                      \
        }                                         \
        else if (actorat[x][y]->flags & FL_SOLID) \
            return false;                           \
    }                                               \
}


bool TryWalk (objtype *obj, bool moveit)
{
    int      doornum;
    unsigned temp;
    int      oldtilex,oldtiley;

    oldtilex = obj->tilex;
    oldtiley = obj->tiley;

    if (ElevatorFloor(obj->tilex,obj->tiley))
        return false;

    doornum = -1;

    switch (obj->dir)
    {
        case north:
            CHECKSIDE (obj->tilex,obj->tiley - 1);

            if (ElevatorFloor(obj->tilex,obj->tiley - 1))
                return false;

            if (!moveit)
                return true;

            obj->tiley--;
            break;

        case northeast:
            CHECKDIAG (obj->tilex + 1,obj->tiley - 1);
            CHECKDIAG (obj->tilex + 1,obj->tiley);
            CHECKDIAG (obj->tilex,obj->tiley - 1);

            if (!moveit)
                return true;

            obj->tilex++;
            obj->tiley--;
            break;

        case east:
            CHECKSIDE (obj->tilex + 1,obj->tiley);

            if (ElevatorFloor(obj->tilex + 1,obj->tiley))
            {
                if (doornum != -1 && obj->obclass != electrosphereobj)
                    OpenDoor (doornum);

                return false;
            }

            if (!moveit)
                return true;

            obj->tilex++;
            break;

        case southeast:
            CHECKDIAG (obj->tilex + 1,obj->tiley + 1);
            CHECKDIAG (obj->tilex + 1,obj->tiley);
            CHECKDIAG (obj->tilex,obj->tiley + 1);

            if (!moveit)
                return true;

            obj->tilex++;
            obj->tiley++;
            break;

        case south:
            CHECKSIDE (obj->tilex,obj->tiley + 1);

            if (ElevatorFloor(obj->tilex,obj->tiley + 1))
                return false;

            if (!moveit)
                return true;

            obj->tiley++;
            break;

        case southwest:
            CHECKDIAG (obj->tilex - 1,obj->tiley + 1);
            CHECKDIAG (obj->tilex - 1,obj->tiley);
            CHECKDIAG (obj->tilex,obj->tiley + 1);

            if (!moveit)
                return true;

            obj->tilex--;
            obj->tiley++;
            break;

        case west:
            CHECKSIDE (obj->tilex - 1,obj->tiley);

            if (ElevatorFloor(obj->tilex - 1,obj->tiley))
            {
                if (doornum != -1 && obj->obclass != electrosphereobj)
                    OpenDoor (doornum);

                return false;
            }

            if (!moveit)
                return true;

            obj->tilex--;
            break;

        case northwest:
            CHECKDIAG (obj->tilex - 1,obj->tiley - 1);
            CHECKDIAG (obj->tilex - 1,obj->tiley);
            CHECKDIAG (obj->tilex,obj->tiley - 1);

            if (!moveit)
                return true;

            obj->tilex--;
            obj->tiley--;
            break;

        default:
            return false;
    }

    //
    // should actor open this door?
    //
    if (doornum != -1)
    {
        switch (obj->obclass)
        {
            //
            // actors that don't open doors
            //
            case liquidobj:
            case electrosphereobj:
                obj->tilex = oldtilex;
                obj->tiley = oldtiley;
                return false;

            //
            // all other actors open doors
            //
            default:
                //
                // TODO: obj should attempt to call OpenDoor from its
                // movement function, otherwise it'll jump into the door tile
                // before it's open and make it non-solid
                //
                // This bug exists in original Wolf3D, but is fixed in Wolf4SDL
                //
                OpenDoor (doornum);
                obj->distance = ~doornum;
                return true;
        }
    }

    obj->areanumber = GetAreaNumber(obj->tilex,obj->tiley);

#ifdef IN_DEVELOPMENT
    if (obj->areanumber >= NUMAREAS)
        Quit ("Actor walked on wall at %dx%d",obj->tilex,obj->tiley);
#endif

    obj->distance = TILEGLOBAL;

    return true;
}


/*
===================
=
= ElevatorFloor
=
===================
*/

bool ElevatorFloor (int tilex, int tiley)
{
    unsigned tile;

    tile = MAPSPOT(tilex,tiley,0);

    if (tile >= HIDDENAREATILE)
        tile -= HIDDENAREATILE;
    else
        tile -= AREATILE;

    return !tile;
}


/*
==================================
=
= SelectDodgeDir
=
= Attempts to choose and initiate a movement for obj that sends it towards
= the player while dodging
=
= If there is no possible move (obj is totally surrounded)
=
= obj->dir          = nodir
=
= Otherwise
=
= obj->dir          = new direction to follow
= obj->distance     = TILEGLOBAL or ~doornumber
= obj->tilex        = new destination
= obj->tiley
= obj->areanumber   = the floor tile number (0 - (NUMAREAS - 1)) of destination
=
==================================
*/

void SelectDodgeDir (objtype *obj)
{
    int   i;
    fixed deltax,deltay;
    int   dirtry[5];
    int   turnaround,tdir;

    if (obj->flags & FL_FIRSTATTACK)
    {
        //
        // turning around is only ok the very first time after noticing the
        // player
        //
        turnaround = nodir;
        obj->flags &= ~FL_FIRSTATTACK;
    }
    else
        turnaround = opposite[obj->dir];

    SeekPlayerOrStatic (obj,&deltax,&deltay);

//
// arange 5 direction choices in order of preference
// the four cardinal directions plus the diagonal straight towards
// the player
//
    if (deltax > 0)
    {
        dirtry[1] = east;
        dirtry[3] = west;
    }
    else if (deltax <= 0)
    {
        dirtry[1] = west;
        dirtry[3] = east;
    }

    if (deltay > 0)
    {
        dirtry[2] = south;
        dirtry[4] = north;
    }
    else if (deltay <= 0)
    {
        dirtry[2] = north;
        dirtry[4] = south;
    }

//
// randomize a bit for dodging
//
    if (labs(deltax) > labs(deltay))
    {
        tdir = dirtry[1];
        dirtry[1] = dirtry[2];
        dirtry[2] = tdir;
        tdir = dirtry[3];
        dirtry[3] = dirtry[4];
        dirtry[4] = tdir;
    }

    if (US_RndT() < 128)
    {
        tdir = dirtry[1];
        dirtry[1] = dirtry[2];
        dirtry[2] = tdir;
        tdir = dirtry[3];
        dirtry[3] = dirtry[4];
        dirtry[4] = tdir;
    }

    dirtry[0] = diagonal[dirtry[1]][dirtry[2]];

//
// try the directions util one works
//
    for (i = 0; i < lengthof(dirtry); i++)
    {
        if (dirtry[i] == nodir || dirtry[i] == turnaround)
            continue;

        obj->dir = dirtry[i];

        if (TryWalk(obj,true))
            return;
    }

//
// turn around only as a last resort
//
    if (turnaround != nodir)
    {
        obj->dir = turnaround;

        if (TryWalk(obj,true))
            return;
    }

    obj->dir = nodir;

    if (obj->obclass == electrosphereobj)
        obj->s_tilex = 0;
}


/*
============================
=
= SelectChaseDir
=
= As SelectDodgeDir, but doesn't try to dodge
=
============================
*/

void SelectChaseDir (objtype *obj)
{
    fixed deltax,deltay;
    int   dirtry[3];
    int   tdir,olddir,turnaround;

    olddir = obj->dir;
    turnaround = opposite[olddir];

    SeekPlayerOrStatic (obj,&deltax,&deltay);

    dirtry[1] = nodir;
    dirtry[2] = nodir;

    if (deltax > 0)
        dirtry[1] = east;
    else if (deltax < 0)
        dirtry[1] = west;

    if (deltay > 0)
        dirtry[2] = south;
    else if (deltay < 0)
        dirtry[2] = north;

    if (labs(deltay) > labs(deltax))
    {
        tdir = dirtry[1];
        dirtry[1] = dirtry[2];
        dirtry[2] = tdir;
    }

    if (dirtry[1] == turnaround)
        dirtry[1] = nodir;
    if (dirtry[2] == turnaround)
        dirtry[2] = nodir;

    if (dirtry[1] != nodir)
    {
        obj->dir = dirtry[1];

        if (TryWalk(obj,true))
            return;     // either moved forward or attacked
    }

    if (dirtry[2] != nodir)
    {
        obj->dir = dirtry[2];

        if (TryWalk(obj,true))
            return;
    }

//
// there is no direct path to the player, so pick another direction
//
    if (olddir != nodir)
    {
        obj->dir = olddir;

        if (TryWalk(obj,true))
            return;
    }

//
// randomly determine direction of search
//
    if (US_RndT() > 128)
    {
        for (tdir = north; tdir <= west; tdir++)
        {
            if (tdir != turnaround)
            {
                obj->dir = tdir;

                if (TryWalk(obj,true))
                    return;
            }
        }
    }
    else
    {
        for (tdir = west; tdir >= north; tdir--)
        {
            if (tdir != turnaround)
            {
                obj->dir = tdir;

                if (TryWalk(obj,true))
                    return;
            }
        }
    }

    if (turnaround != nodir)
    {
        obj->dir = turnaround;

        if (obj->dir != nodir)
        {
            if (TryWalk(obj,true))
                return;
        }
    }

    obj->dir = nodir;  // can't move

    if (obj->obclass == electrosphereobj)
        obj->s_tilex = 0;
}


/*
===================
=
= GetCornerSeek
=
===================
*/

void GetCornerSeek (objtype *obj)
{
    int SeekPointX[] = {MAPSIZE / 2,MAPSIZE - 1,MAPSIZE / 2,1};  // s_tilex can't seek to 0!
    int SeekPointY[] = {1,MAPSIZE - 1,MAPSIZE / 2,1};
    int seektile = US_RndT() & 3;

    obj->flags &= ~FL_RUNTOSTATIC;
    obj->s_tilex = SeekPointX[seektile];
    obj->s_tiley = SeekPointY[seektile];
}


/*
=================
=
= MoveObj
=
= Moves obj by move global units in obj->dir direction
= Actors are not allowed to move inside the player
= Does NOT check to see if the move is tile map valid
=
= obj->x   = adjusted for new position
= obj->y
=
=================
*/

void MoveObj (objtype *obj, fixed move)
{
    fixed deltax,deltay;
    fixed newx,newy;

    newx = obj->x;
    newy = obj->y;

    switch (obj->dir)
    {
        case north:
            newy -= move;
            break;

        case northeast:
            newx += move;
            newy -= move;
            break;

        case east:
            newx += move;
            break;

        case southeast:
            newx += move;
            newy += move;
            break;

        case south:
            newy += move;
            break;

        case southwest:
            newx -= move;
            newy += move;
            break;

        case west:
            newx -= move;
            break;

        case northwest:
            newx -= move;
            newy -= move;
            break;

        case nodir:
            return;

        default:
            Quit ("Bad dir!");
    }

//
// check to make sure it's not on top of player
//
    if (obj->obclass != electrosphereobj)
    {
        if (areabyplayer[obj->areanumber])
        {
            deltax = labs(newx - player->x);
            deltay = labs(newy - player->y);

            if (deltax <= MINACTORDIST && deltay <= MINACTORDIST)
            {
                PlayerIsBlocking (obj);

                return;
            }
        }
    }

    obj->x = newx;
    obj->y = newy;
    obj->distance -= move;
}


/*
===============
=
= CheckAndReserve
=
= Checks for room in objlist and returns a pointer
= to the new object or NULL
=
===============
*/

objtype *CheckAndReserve (void)
{
    objtype *newobj;

    usedummy = nevermark = true;

    newobj = SpawnNewObj(0,0,&s_hold);

    usedummy = nevermark = false;

    if (newobj == &dummyobj)
        return NULL;

    return newobj;
}


/*
===============
=
= KillActor
=
===============
*/

unsigned actor_points[] =
{
    1025,       // rent-a-cop
    1050,       // turret
    500,        // general scientist
    5075,       // pod alien
    5150,       // electric alien
    2055,       // electro-sphere
    5000,       // pro guard
    10000,      // genetic guard
    5055,       // mutant human1
    6055,       // mutant human2
    0,          // large canister wait
    6050,       // large canister alien
    0,          // small canister wait
    3750,       // small canister alien
    0,          // gurney wait
    3750,       // gurney
    12000,      // liquid
    7025,       // swat
    5000,       // goldtern
    5000,       // goldstern Morphed
    2025,       // volatile transport
    2025,       // floating bomb
    0,          // rotating cube

    5000,       // spider_mutant
    6000,       // breather_beast
    7000,       // cyborg_warror
    8000,       // reptilian_warrior
    9000,       // acid_dragon
    9000,       // mech_guardian
    30000,      // final boss #1
    40000,      // final_boss #2
    50000,      // final_boss #3
    60000,      // final_boss #4

    0,0,0,0,0,  // blake,crate1/2/3, oozes
    0,          // pod egg

    5000,       // morphing_spider_mutant
    8000,       // morphing_reptilian_warrior
    6055,       // morphing_mutant human2
};


//
// TODO: this function uses the variable "clas" to hold obj->obclass
// and sometimes it changes the value clas holds in the switch, but
// other times it changes obj->obclass directly. Check if this is
// intentional or a bug
//
void KillActor (objtype *obj)
{
    int  tilex,tiley;
    bool keepsolid = false,givepoints = true,deadguy = true;
    int  clas;

    tilex = obj->x >> TILESHIFT;  // drop item on center
    tiley = obj->y >> TILESHIFT;

    obj->flags &= ~(FL_FRIENDLY | FL_SHOOTABLE);
    clas = obj->obclass;

    switch (clas)
    {
        case podeggobj:
            PlaySoundLocActor (PODHATCHSND,obj);
            InitSmartSpeedAnim (obj,SPR_POD_HATCH1,0,2,at_ONCE,ad_FWD,7);
            keepsolid = true;
            deadguy = givepoints = false;
            break;

        case morphing_spider_mutantobj:
        case morphing_reptilian_warriorobj:
        case morphing_mutanthuman2obj:
            obj->flags &= ~FL_SHOOTABLE;
            InitSmartSpeedAnim (obj,obj->temp1,0,8,at_ONCE,ad_FWD,2);
            keepsolid = true;
            deadguy = givepoints = false;
            break;

        case crate1obj:
        case crate2obj:
        case crate3obj:
#ifdef IN_DEVELOPMENT
            if (obj->temp3 >= MAXSTATS)
                Quit ("exp crate->temp3 out of range!");
#endif
            statobjlist[obj->temp3].shapenum = -1;      // release reserve static

            SpawnStatic (tilex,tiley,obj->temp2);
            obj->obclass = deadobj;
            obj->lighting = NO_SHADING;
            InitSmartSpeedAnim (obj,SPR_GRENADE_EXPLODE2,0,3,at_ONCE,ad_FWD,3 + (US_RndT() & 7));
            A_DeathScream (obj);
            MakeAlertNoise (obj);
            break;

        case floatingbombobj:
            obj->lighting = EXPLOSION_SHADING;
            A_DeathScream (obj);
            InitSmartSpeedAnim (obj,SPR_FSCOUT_DIE1,0,7,at_ONCE,ad_FWD,5);
            break;

        case volatiletransportobj:
            obj->lighting = EXPLOSION_SHADING;
            A_DeathScream (obj);
            InitSmartSpeedAnim (obj,SPR_GSCOUT_DIE1,0,8,at_ONCE,ad_FWD,5);
            break;

        case goldsternobj:
            NewState (obj,&s_goldwarp_it);
            GoldsternInfo.flags = GS_NEEDCOORD;
            GoldsternInfo.GoldSpawned = false;

            //
            // init timer & search for a location out of all possible locations
            //
            GoldsternInfo.WaitTime = MIN_GOLDIE_WAIT + Random(MAX_GOLDIE_WAIT - MIN_GOLDIE_WAIT);
            clas = goldsternobj;
            break;

        case gold_morphobj:
            GoldsternInfo.flags = GS_NO_MORE;
            PlaySoundLocActor (PODDEATHSND,obj);
            obj->flags |= FL_OFFSET_STATES;
            InitAnim (obj,SPR_GOLD_DEATH1,0,4,at_ONCE,ad_FWD,25,9);
            break;

        case gen_scientistobj:
            if (obj->flags & FL_INFORMANT)
            {
                givepoints = false;
                clas = nothing;
                gamestuff.level[gamestate.mapon].stats.accum_inf--;

                if (!(gamestate.flags & GS_KILL_INF_WARN) || US_RndT() < 25)
                {
                    DisplayInfoMsg (dki_msg,MP_INTERROGATE - 1,DISPLAY_MSG_STD_TIME * 3,MT_GENERAL);
                    gamestate.flags |= GS_KILL_INF_WARN;
                }
            }

            NewState (obj,&s_biodie1);

            if (obj->ammo && !(obj->flags & FL_INFORMANT))
            {
                if (US_RndT() < 65)
                    PlaceItemType (bo_coin,tilex,tiley);
                else
                    PlaceItemType (bo_clip2,tilex,tiley);
            }
            break;

        case rentacopobj:
            NewState (obj,&s_rent_die1);

            if (!(gamestate.weapons & (1 << wp_pistol)))
                PlaceItemType (bo_pistol,tilex,tiley);
            else if (US_RndT() < 65 || !obj->ammo)
                PlaceItemType (bo_coin,tilex,tiley);
            else if (obj->ammo)
                PlaceItemType (bo_clip2,tilex,tiley);
            break;

        case swatobj:
            NewState (obj,&s_swatdie1);

            if (!(gamestate.weapons & (1 << wp_burst_rifle)))
                PlaceItemType(bo_burst_rifle,tilex,tiley);
            else if (US_RndT() < 65 || !obj->ammo)
                PlaceItemType (bo_coin,tilex,tiley);
            else if (obj->ammo)
                PlaceItemType (bo_clip2,tilex,tiley);
            break;

        case proguardobj:
            NewState (obj,&s_prodie1);

            if (!(gamestate.weapons & (1 << wp_burst_rifle)))
                PlaceItemType (bo_burst_rifle,tilex,tiley);
            else if (US_RndT() < 65 || !obj->ammo)
                PlaceItemType (bo_coin,tilex,tiley);
            else if (obj->ammo)
                PlaceItemType (bo_clip2,tilex,tiley);
            break;

        case electroobj:
            NewState (obj,&s_electro_die1);

            eaList[obj->temp2].aliens_out--;
            obj->obclass = nothing;  // TODO: not clas = nothing?
            actorat[obj->tilex][obj->tiley] = NULL;
            break;

        case liquidobj:
            NewState (obj,&s_liquid_die1);

            obj->obclass = nothing;
            actorat[obj->tilex][obj->tiley] = NULL;
            break;

        case podobj:
            obj->temp1 = SPR_POD_DIE1;

            NewState (obj,&s_ofs_pod_death1);
            A_DeathScream (obj);
            break;

        case electrosphereobj:
            obj->obclass = nothing;
            obj->temp1 = SPR_ELECTRO_SPHERE_DIE1;

            NewState (obj,&s_ofs_esphere_death1);

            actorat[obj->tilex][obj->tiley] = NULL;
            break;

        case cyborg_warriorobj:
        case mech_guardianobj:
        case reptilian_warriorobj:
        case mutant_human1obj:
            PlaceItemNearTile (bo_clip2,tilex,tiley);
        case spider_mutantobj:
        case breather_beastobj:
        case acid_dragonobj:
        case final_boss3obj:
        case final_boss4obj:
        case mutant_human2obj:
        case scan_alienobj:
        case lcan_alienobj:
            NewState (obj,&s_ofs_die1);
            break;

        case final_boss2obj:
            PlaySoundLocActor (PODDEATHSND,obj);
            InitAnim (obj,SPR_BOSS8_DIE1,0,4,at_ONCE,ad_FWD,25,9);
            break;

        case genetic_guardobj:
        case final_boss1obj:
        case gurneyobj:
            if (!(gamestate.weapons & (1 << wp_pistol)))
                PlaceItemNearTile (bo_pistol,tilex,tiley);
            else
                PlaceItemNearTile (bo_clip2,tilex,tiley);

            NewState (obj,&s_ofs_die1);
            break;

        case gurney_waitobj:        // mutant asleep on gurney
            InitSmartAnim (obj,SPR_GURNEY_MUT_B1,0,3,at_ONCE,ad_FWD);
            keepsolid = true;
            givepoints = false;
            break;

        case scan_wait_alienobj:   // Actual Canister - destroyed
            InitSmartAnim (obj,SPR_SCAN_ALIEN_B1,0,3,at_ONCE,ad_FWD);
            keepsolid = true;
            givepoints = false;
            break;

        case lcan_wait_alienobj:     // Actual Canister - destroyed
            InitSmartAnim (obj,SPR_LCAN_ALIEN_B1,0,3,at_ONCE,ad_FWD);
            keepsolid = true;
            givepoints = false;
            break;

        case hang_terrotobj:
            NewState (obj,&s_terrot_die1);
            obj->lighting = EXPLOSION_SHADING;
            break;
    }
#ifdef LOOK_FOR_DEAD_GUYS
    switch (clas)
    {
        case SMART_ACTORS:
            DeadGuys[NumDeadGuys++] = obj;
            break;
    }
#endif
    if (keepsolid)
    {
        obj->flags &= ~(FL_SHOOTABLE);
        obj->flags2 &= ~FL2_BFG_SHOOTABLE;

        if (deadguy)
            obj->flags |= FL_DEADGUY;
    }
    else
    {
        if (deadguy)
            obj->flags |= (FL_NONMARK | FL_DEADGUY);

        if (clas >= rentacopobj && clas < crate1obj && clas != electroobj && clas != goldsternobj)
        {
            gamestuff.level[gamestate.mapon].stats.accum_enemy++;
#ifdef TRACK_ENEMY_COUNT
            numenemy[clas]--;
#endif
        }

        if (givepoints)
        {
            if (clas == electroobj || clas == goldsternobj)
                GivePoints (actor_points[clas - rentacopobj],false);
            else
                GivePoints (actor_points[clas - rentacopobj],true);
        }

        obj->flags &= ~(FL_SHOOTABLE | FL_SOLID | FL_FAKE_STATIC);
        obj->flags2 &= ~FL2_BFGSHOT_SOLID;

        if (actorat[obj->tilex][obj->tiley] == obj)
        {
            //
            // clear obj from where it was going in actorat
            //
            if (!(tilemap[obj->tilex][obj->tiley] & 0x80))
                actorat[obj->tilex][obj->tiley] = NULL;

            //
            // set actor where it died in actorat if there's a door,
            // otherwise just leave it removed
            //
            if (tilemap[tilex][tiley] & 0x80)
                actorat[tilex][tiley] = obj;
            else
                obj->flags |= FL_NEVERMARK;
        }
    }

    DropCargo (obj);

    obj->tilex = tilex;
    obj->tiley = tiley;

    if (LastMsgPri == MP_TAKE_DAMAGE && LastInfoAttacker == clas)
        MsgTicsRemain = 1;

    switch (clas)
    {
        case electroobj:
        case liquidobj:
        case electrosphereobj:
            obj->obclass = clas;
            obj->flags |= FL_NEVERMARK;
            break;
    }
}


/*
===================
=
= DamageActor
=
= Called when the player successfully hits an enemy.
=
= Does damage points to enemy obj, either putting it into a stun frame or
= killing it.
=
===================
*/

void DamageActor (objtype *obj, int damage, objtype *attacker)
{
    int oldhp;
    int woundmod,modbefore,modafter;

    oldhp = obj->hitpoints;
    modbefore = 0;
    modafter = 1;

    if (!(obj->flags & FL_SHOOTABLE))
        return;

    //
    // TODO: don't we already call this from GunAttack?
    //
    if (gamestate.weapon != wp_autocharge)
        MakeAlertNoise (player);

    if (obj->flags & FL_FREEZE)
        return;

    switch (obj->obclass)
    {
        case hang_terrotobj:
            if (gamestate.weapon < wp_burst_rifle)
                return;
            break;

        case gurney_waitobj:
            if (obj->temp3)
                return;
            break;

        case arc_barrierobj:
            if (attacker->obclass == bfg_shotobj)
            {
                if (BARRIER_STATE(obj) != bt_DISABLING)
                {
                    BARRIER_STATE(obj) = bt_DISABLING;
                    obj->hitpoints = 15;
                    obj->temp3 = 0;
                    obj->temp2 = US_RndT() & 0xf;

                    NewState (obj,&s_barrier_shutdown);
                }
            }
            return;

        case post_barrierobj:
        case rotating_cubeobj:
            return;

        case plasma_detonatorobj:
            //
            // detonate 'em!
            //
            if (attacker == player)
                obj->temp3 = 1;
            else
                obj->temp3 = damage;
            return;
    }

//
// do double damage if shooting a non attack mode actor
//
    if (!(obj->flags & FL_ATTACKMODE))
        damage <<= 1;

    obj->hitpoints -= damage;
    obj->flags2 |= FL2_DAMAGE_CLOAK;

    if (obj->hitpoints <= 0)
    {
        switch (obj->obclass)
        {
#ifdef OBJ_RESERV
            case scan_wait_alienobj:
            case lcan_wait_alienobj:
            case gurney_waitobj:
                obj->tempobj = CheckAndReserve();

                if (!obj->tempobj)
                {
                    obj->hitpoints += damage;
                    return;
                }
                break;
#endif
            case goldsternobj:
                if (gamestate.mapon == GOLD_MORPH_LEVEL)
                {
                    morphwaittime = 60;
                    noShots = true;

                    NewState (obj,&s_goldmorphwait1);
                    obj->obclass = gold_morphingobj;
                    obj->flags &= ~FL_SHOOTABLE;
                    return;
                }
                break;
        }

        obj->tempobj = attacker;
        KillActor (obj);
        return;
    }
    else
    {
        switch (obj->obclass)
        {
            case swatobj:
                //
                // don't get wounded if it's an arc!
                //
                if (attacker->obclass == arc_barrierobj || attacker->obclass == post_barrierobj)
                    break;

                //
                // calculate wound boundary (based on NUM_WOUND_STAGES).
                //
                woundmod = starthitpoints[gamestate.difficulty][en_swat] / (obj->temp1 + 1) + 1;
                modbefore = oldhp / woundmod;
                modafter = obj->hitpoints / woundmod;

                //
                // if mod before and after are different, we've crossed
                // a wound boundary!
                //
                if (modbefore != modafter)
                {
                    PlaySoundLocActor (SWATDEATH2SND,obj);
                    NewState (obj,&s_swatwounded1);
                    obj->flags &= ~(FL_SHOOTABLE | FL_SOLID);
                    obj->temp2 = (5 * 60) + ((US_RndT() % 20) * 60);
                    return;
                }
                break;
        }

        if (obj->flags & FL_LOCKED_STATE)
            return;

        if (!(obj->flags & FL_ATTACKMODE))
        {
            if (obj->obclass == gen_scientistobj && (obj->flags & FL_INFORMANT))
                return;

            FirstSighting (obj);  // put into combat mode
        }

        switch (obj->obclass)
        {
#if GAME_VERSION != SHAREWARE_VERSION
            case volatiletransportobj:
            case floatingbombobj:
                T_PainThink (obj);
                break;
#endif
            case goldsternobj:
                NewState (obj,&s_goldpain);
                break;

            case gold_morphobj:
                NewState (obj,&s_mgold_pain);
                break;

            case liquidobj:
                NewState (obj,&s_liquid_ouch);
                break;

            case rentacopobj:
                NewState (obj,&s_rent_pain);
                break;

            case podobj:
                NewState (obj,&s_ofs_pod_ouch);
                break;

            case spider_mutantobj:
            case breather_beastobj:
            case cyborg_warriorobj:
            case reptilian_warriorobj:
            case acid_dragonobj:
            case mech_guardianobj:
            case final_boss1obj:
            case final_boss2obj:
            case final_boss3obj:
            case final_boss4obj:

            case genetic_guardobj:
            case mutant_human1obj:
            case mutant_human2obj:
            case scan_alienobj:
            case lcan_alienobj:
            case gurneyobj:
                NewState (obj,&s_ofs_pain);
                break;

            case electrosphereobj:
                NewState (obj,&s_ofs_ouch);
                obj->temp1 = SPR_ELECTRO_SPHERE_OUCH;
                break;

            case electroobj:
                NewState (obj,&s_electro_ouch);
                break;

            case gen_scientistobj:
                NewState (obj,&s_biopain);
                break;

            case swatobj:
                NewState (obj,&s_swatpain);
                break;

            case proguardobj:
                NewState (obj,&s_propain);
                break;
        }
    }

    //
    // make sure actors aren't sitting ducks!
    //
    if (US_RndT() < 192 && !(obj->flags & (FL_LOCKED_STATE | FL_BARRIER_DAMAGE)))
    {
        ChangeShootMode (obj);
        DoAttack (obj);
    }

    obj->flags |= FL_LOCKED_STATE;
}


/*
=============================================================================

                        CHECKSIGHT

=============================================================================
*/


/*
=====================
=
= CalcAngle
=
= Calculates angle from 1st object to 2nd object
=
=====================
*/

int CalcAngle (objtype *from_obj, objtype *to_obj)
{
    fixed deltax,deltay;
    float angle;
    int   iangle;

    deltax = to_obj->x - from_obj->x;
    deltay = from_obj->y - to_obj->y;

    if (!(deltax | deltay))
        return 1;

    angle = atan2(deltay,deltax);

    if (angle < 0)
        angle = (M_PI * 2) + angle;

    iangle = (angle / (M_PI * 2)) * ANG360;

    return iangle;
}


/*
=====================
=
= CheckLine
=
= Returns true if a straight line between two actors is unobstructed
=
=====================
*/

bool CheckLine (objtype *from_obj, objtype *to_obj)
{
    int      x1,y1,xt1,yt1,x2,y2,xt2,yt2;
    int      x,y;
    int      xstep,ystep;
    int      partial,delta;
    int32_t  ltemp;
    int      xfrac,yfrac,deltafrac;
    unsigned value,intercept;

    x1 = from_obj->x >> UNSIGNEDSHIFT;      // 1/256 tile precision
    y1 = from_obj->y >> UNSIGNEDSHIFT;
    xt1 = x1 >> UNSIGNEDSHIFT;
    yt1 = y1 >> UNSIGNEDSHIFT;

    x2 = to_obj->x >> UNSIGNEDSHIFT;
    y2 = to_obj->y >> UNSIGNEDSHIFT;

    //
    // KS: to_obj->tilex/y can be completely different to
    // to_obj->x/y >> UNSIGNEDSHIFT because an actor's TILE
    // coordinates jump around depending on the actor's destination.
    // Checking x/y >> UNSIGNEDSHIFT should be a lot more accurate,
    // test this
    //
    xt2 = to_obj->tilex;
    yt2 = to_obj->tiley;

    if (abs(xt2 - xt1) > 0)
    {
        if (xt2 > xt1)
        {
            partial = 256 - (x1 & 0xff);
            xstep = 1;
        }
        else
        {
            partial = x1 & 0xff;
            xstep = -1;
        }

        deltafrac = abs(x2 - x1);

        if (!deltafrac)
            deltafrac = 1;

        delta = y2 - y1;

        ltemp = ((int32_t)delta << UNSIGNEDSHIFT) / deltafrac;

        if (ltemp > 0x7fffl)
            ystep = 0x7fff;
        else if (ltemp < -0x7fffl)
            ystep = -0x7fff;
        else
            ystep = ltemp;

        yfrac = y1 + (((int32_t)ystep * partial) >> UNSIGNEDSHIFT);

        x = xt1 + xstep;
        xt2 += xstep;

        do
        {
            y = yfrac >> UNSIGNEDSHIFT;
            yfrac += ystep;

            value = tilemap[x][y];
            x += xstep;

            if (!value)
                continue;

            if (value < 128 || value > 256)
                return false;

            //
            // see if the door is open enough
            //
            // TODO: this never worked properly in Wolf, and it
            // doesn't even take into account Blake's trek doors
            //
            value &= ~0x80;
            intercept = yfrac - (ystep / 2);

            if (intercept > doorobjlist[value].position)
                return false;

        } while (x != xt2);
    }

    if (abs(yt2 - yt1) > 0)
    {
        if (yt2 > yt1)
        {
            partial = 256 - (y1 & 0xff);
            ystep = 1;
        }
        else
        {
            partial = y1 & 0xff;
            ystep = -1;
        }

        deltafrac = abs(y2 - y1);

        if (!deltafrac)
            deltafrac = 1;

        delta = x2 - x1;
        ltemp = ((int32_t)delta << UNSIGNEDSHIFT) / deltafrac;

        if (ltemp > 0x7fffl)
            xstep = 0x7fff;
        else if (ltemp < -0x7fffl)
            xstep = -0x7fff;
        else
            xstep = ltemp;

        xfrac = x1 + (((int32_t)xstep * partial) >> UNSIGNEDSHIFT);

        y = yt1 + ystep;
        yt2 += ystep;

        do
        {
            x = xfrac >> UNSIGNEDSHIFT;
            xfrac += xstep;

            value = tilemap[x][y];
            y += ystep;

            if (!value)
                continue;

            if (value < 128 || value > 256)
                return false;

            //
            // see if the door is open enough
            //
            value &= ~0x80;
            intercept = xfrac - (xstep / 2);

            if (intercept > doorobjlist[value].position)
                return false;

        } while (y != yt2);
    }

    return true;
}


/*
================
=
= CheckSight
=
= Checks a straight line between two actors
=
= If the sight is ok, check alertness and angle to see if they notice
=
= Returns true if to_obj has been spotted
=
================
*/

bool CheckSight (objtype *from_obj, objtype *to_obj)
{
    fixed deltax,deltay;

//
// don't bother tracing a line if the area isn't connected to the player's
//
    if (!areabyplayer[from_obj->areanumber])
        return false;

//
// if to_obj is real close, sight is automatic
//
    deltax = to_obj->x - from_obj->x;
    deltay = to_obj->y - from_obj->y;

    if (labs(deltax) < MINSIGHT && labs(deltay) < MINSIGHT)
        return true;

//
// see if they are looking in the right direction
//
// TODO: this doesn't take diagonals into account
//
    switch (from_obj->dir)
    {
        case north:
            if (deltay > 0)
                return false;
            break;

        case east:
            if (deltax < 0)
                return false;
            break;

        case south:
            if (deltay < 0)
                return false;
            break;

        case west:
            if (deltax > 0)
                return false;
            break;
    }

//
// trace a line to check for blocking tiles (corners)
//
    return CheckLine(from_obj,to_obj);
}


/*
===============
=
= CheckView
=
= Checks a straight line between two actors
=
= If the sight is ok, check angle to see if they notice
=
= Returns true if to_obj is in from_obj's field of view
=
===============
*/

bool CheckView (objtype *from_obj, objtype *to_obj)
{
    fixed deltax,deltay;
    int   iangle;
    float fangle;

    //
    // don't bother tracing a line if the area isn't connected to the player's
    //
    if (!areabyplayer[from_obj->areanumber])
        return false;

    deltax = from_obj->x - to_obj->x;
    deltay = to_obj->y - from_obj->y;

    fangle = atan2(deltay,deltax);   // returns -pi to pi

    if (fangle < 0)
        fangle = (M_PI * 2) + fangle;

    iangle = ((fangle / (M_PI * 2)) * ANG360) + (ANG45 / 2);

    if (iangle > ANG360)
        iangle = ANG360;

    if (iangle <= adjangletable[1][from_obj->dir] || iangle >= adjangletable[0][from_obj->dir])
        return false;

    //
    // trace a line to check for blocking tiles (corners)
    //
    return CheckLine(from_obj,to_obj);
}


/*
===============
=
= FirstSighting
=
= Puts an actor into attack mode and possibly reverses the direction
= if the player is behind it
=
===============
*/

void FirstSighting (objtype *obj)
{
    if (PlayerInvisible)
        return;

//
// react to the player
//
    switch (obj->obclass)
    {
        case floatingbombobj:
            if (obj->flags & FL_STATIONARY)
                return;

            PlaySoundLocActor (SCOUT_ALERTSND,obj);
            NewState (obj,&s_scout_run);
            obj->speed *= 3;   // haul ass
            break;

        case goldsternobj:
            PlaySoundLocActor (GOLDSTERNHALTSND,obj);
            NewState (obj,&s_goldchase1);
            obj->speed *= 3;
            break;

        case rentacopobj:
            PlaySoundLocActor (HALTSND,obj);
            NewState (obj,&s_rent_chase1);
            obj->speed *= 3;
            break;

        case gen_scientistobj:
            PlaySoundLocActor (SCIENTISTHALTSND,obj);
            NewState (obj,&s_biochase1);
            obj->speed *= 3;
            break;

        case swatobj:
            PlaySoundLocActor (SWATHALTSND,obj);
            NewState (obj,&s_swatchase1);
            obj->speed *= 3;
            break;

        case breather_beastobj:
        case reptilian_warriorobj:
        case genetic_guardobj:
        case final_boss4obj:
        case final_boss2obj:
            PlaySoundLocActor (GGUARDHALTSND,obj);
            NewState (obj,&s_ofs_chase1);
            obj->speed *= 3;
            break;

        case cyborg_warriorobj:
        case mech_guardianobj:
        case mutant_human1obj:
        case final_boss3obj:
        case final_boss1obj:
            PlaySoundLocActor (BLUEBOYHALTSND,obj);
            NewState (obj,&s_ofs_chase1);
            obj->speed *= 2;
            break;

        case mutant_human2obj:
            PlaySoundLocActor (DOGBOYHALTSND,obj);
            NewState (obj,&s_ofs_chase1);
            obj->speed *= 2;
            break;

        case liquidobj:
            NewState (obj,&s_liquid_move);
            break;

        case spider_mutantobj:
        case scan_alienobj:
            PlaySoundLocActor (SCANHALTSND,obj);
            NewState (obj,&s_ofs_chase1);
            obj->speed *= 3;
            break;

        case lcan_alienobj:
            PlaySoundLocActor (LCANHALTSND,obj);
            NewState (obj,&s_ofs_chase1);
            obj->speed *= 3;
            break;

        case gurneyobj:
            PlaySoundLocActor (GURNEYSND,obj);
            NewState (obj,&s_ofs_chase1);
            obj->speed *= 3;
            break;

        case acid_dragonobj:
        case podobj:
            PlaySoundLocActor (PODHALTSND,obj);
            NewState (obj,&s_ofs_chase1);
            obj->speed *= 2;
            break;

        case gurney_waitobj:
            if (obj->temp3)
            {
                obj->tempobj = CheckAndReserve();

                if (obj->tempobj)
                {
                    obj->flags &= ~(FL_SHOOTABLE);
                    InitSmartAnim (obj,SPR_GURNEY_MUT_B1,0,3,at_ONCE,ad_FWD);
                }
                else
                    return;
            }
            break;

        case proguardobj:
            PlaySoundLocActor (PROHALTSND,obj);
            NewState (obj,&s_prochase1);
            obj->speed *= 4;
            break;

        case hang_terrotobj:
            PlaySoundLocActor (TURRETSND,obj);
            NewState (obj,&s_terrot_seek1);
            break;
    }

    obj->flags |= FL_ATTACKMODE | FL_FIRSTATTACK;
}


/*
===============
=
= SightPlayer
=
= Called by actors that ARE NOT chasing the player. If the player
= is detected (by sight, noise, or proximity), the actor is put into
= its combat frame and true is returned.
=
= Incorporates a random reaction delay
=
===============
*/

bool SightPlayer (objtype *obj)
{
    bool sighted;

    if (obj->obclass == gen_scientistobj)
    {
        if (obj->flags & FL_INFORMANT)
            return false;
    }

    if (PlayerInvisible)
        return false;

    if (obj->flags & FL_ATTACKMODE)
        return true;

    if (obj->temp2)
    {
        //
        // count down reaction time
        //
        obj->temp2 -= tics;

        if (obj->temp2 > 0)
            return false;

        obj->temp2 = 0;     // time to react
    }
    else
    {
        if (!areabyplayer[obj->areanumber])
            return false;

        if (obj->flags & FL_AMBUSH)
        {
            if (!CheckSight(obj,player))
                return false;

            obj->flags &= ~FL_AMBUSH;
        }
        else
        {
            sighted = false;

            if (madenoise || CheckSight(obj,player))
                sighted = true;

            switch (obj->obclass)
            {
                //
                // actors that look fine while just standing around go here
                //
                case rentacopobj:
                case proguardobj:
                case swatobj:
                case goldsternobj:
                case gen_scientistobj:
                case floatingbombobj:
                case volatiletransportobj:
                    break;

                //
                // actors that look funny when just standing around go here
                //
                default:
                    if (obj->flags & FL_VISIBLE)
                        sighted = true;
                    break;
            }

            if (!sighted)
                return false;
        }

        switch (obj->obclass)
        {
            case goldsternobj:
            case rentacopobj:
            case hang_terrotobj:
            case floatingbombobj:
                obj->temp2 = 1 + (US_RndT() / 4);
                break;

            case gen_scientistobj:
                obj->temp2 = 2;
                break;

            case proguardobj:
            case swatobj:
            case liquidobj:
                obj->temp2 = 1 + (US_RndT() / 6);
                break;

            case gurney_waitobj:
                obj->temp2 = obj->temp3;
                break;

            default:
                obj->temp2 = 1;
                break;
        }

        obj->flags &= ~FL_FRIENDLY;

        return false;
    }

    FirstSighting (obj);

    return true;
}

#ifdef LOOK_FOR_DEAD_GUYS
/*
===============
=
= LookForDeadGuys
=
===============
*/

bool LookForDeadGuys (objtype *obj)
{
    int  i;

    if (obj->obclass == gen_scientistobj && (obj->flags & FL_INFORMANT))
        return false;

    for (i = 0; i < numdeadguys; i++)
    {
        if (CheckSight(obj,deadguys[i]))
        {
            FirstSighting (obj);

            return true;
        }
    }

    return false;
}
#endif

/*
===============
=
= LookForGoodies
=
===============
*/

bool LookForGoodies (objtype *obj, unsigned RunReason)
{
    #define DOORCHOICES    8

    doorobj_t *door,*doorlist[DOORCHOICES];
    statobj_t *statptr;
    int       hp;
    int       shapenum,doorsfound,doornum;
    unsigned  area1,area2;
    bool      finddoor,getit;

    hp = starthitpoints[gamestate.difficulty][obj->obclass - rentacopobj];
    finddoor = false;

//
// don't look for goodies if this actor is simply a non-informant that
// was interrogated
//
// these actors back away, then attack
//
    if (RunReason == RR_INTERROGATED)
    {
        finddoor = true;

        if (US_RndT() < 128)
            obj->flags &= ~FL_INTERROGATED;  // no longer runs, now attacks!
    }

//
// let the computer-controlled actors cheat in some circumstances
//
    if (player->areanumber != obj->areanumber && !(obj->flags & FL_VISIBLE))
    {
        if (!obj->ammo)
            obj->ammo += 8;

        if (obj->hitpoints <= hp >> 1)
            obj->hitpoints += 10;

        return true;
    }

//
// let's REALLY look for some goodies!
//
    for (statptr = &statobjlist[0]; statptr != laststatobj; statptr++)
    {
        if (finddoor)
            break;

        if (obj->areanumber != statptr->areanumber || statptr->shapenum != -1)
            continue;

        switch (statptr->itemnumber)
        {
            case bo_chicken:
            case bo_ham:
            case bo_water:
            case bo_clip:
            case bo_clip2:
            case bo_candybar:
            case bo_sandwich:
                //
                // if actor is on top of this object, get it
                //
                if (statptr->tilex == obj->tilex && statptr->tiley == obj->tiley)
                {
                    shapenum = -1;

                    switch (statptr->itemnumber)
                    {
                        case bo_clip:
                        case bo_clip2:
                            if (obj->ammo)
                                continue;   // obj has plenty of ammo!

                            obj->ammo += 8;
                            break;

                        case bo_candybar:
                        case bo_sandwich:
                        case bo_chicken:
                            if (obj->hitpoints > hp >> 1)
                                continue;   // obj has plenty of health!

                            obj->hitpoints += 8;
                            shapenum = statptr->shapenum + 1;
                            break;

                        case bo_ham:
                            if (obj->hitpoints > hp >> 1)
                                continue;

                            obj->hitpoints += 12;
                            shapenum = statptr->shapenum + 1;
                            break;

                        case bo_water:
                            if (obj->hitpoints > hp >> 1)
                                continue;

                            obj->hitpoints += 2;
                            shapenum = statptr->shapenum + 1;
                            break;
                    }

                    obj->s_tilex = 0;               // reset for next search
                    statptr->shapenum = shapenum;   // remove from list if necessary
                    statptr->itemnumber = bo_nothing;
                    statptr->flags &= ~FL_BONUS;

                    return true;
                }

//
// stop searching when (s_tilex,s_tiley) object is found; even though obj
// may be standing on another needed object (depends on where objects appear
// in statobjlist)
//
#ifdef ONE_TRACK_MIND
                //
                // is obj looking for current object?
                //
                if (statptr->tilex == obj->s_tilex && statptr->tiley == obj->s_tiley)
                    return false;
#endif
                //
                // give obj a chance to run towards this object
                //
                if (!(obj->flags & FL_RUNTOSTATIC))
                {
                    getit = false;

                    switch (statptr->itemnumber)
                    {
                        case bo_clip:
                        case bo_clip2:
                            if (RunReason & RR_AMMO)
                                getit = true;
                            break;

                        case bo_firstaid:
                        case bo_water:
                        case bo_chicken:
                        case bo_candybar:
                        case bo_sandwich:
                        case bo_ham:
                            if (RunReason & RR_HEALTH)
                                getit = true;
                            break;

                    }

                    if (getit)
                    {
                        obj->flags |= FL_RUNTOSTATIC;
                        obj->s_tilex = statptr->tilex;
                        obj->s_tiley = statptr->tiley;

                        return false;
                    }
                }
                break;
        }
    }

//
// should obj run for a door? (quick escape!)
//
    if (areabyplayer[obj->areanumber])
    {
        //
        // if obj is running for a goody or a door, leave it alone
        //
        if (obj->flags & FL_RUNTOSTATIC)
            return false;

        //
        // search for all doors in obj's current area
        //
        doorsfound = 0;

        for (door = &doorobjlist[0]; door != lastdoorobj; door++)
        {
            //
            // is this an elevator door or a locked door?
            //
            // TODO: why can't we just check the door's areanumbers?
            //
            area1 = MAPSPOT(door->tilex - 1,door->tiley,0) - AREATILE;
            area2 = MAPSPOT(door->tilex + 1,door->tiley,0) - AREATILE;

            if (!area1 || !area2 || door->lock != kt_none)
                continue;

            //
            // does this door connect the area obj is in with another area?
            //
            if (door->areanumber[0] == obj->areanumber || door->areanumber[1] == obj->areanumber)
            {
                doorlist[doorsfound] = door;       // add to list

                if (++doorsfound >= DOORCHOICES)   // check for max
                    break;
            }
        }

        //
        // randomly choose a door if any were found
        //
        if (doorsfound)
        {
            //
            // only choose the last door used if it's the only door in this area!
            //
            doornum = Random(doorsfound);
            door = doorlist[doornum];

            if (DOORNUM(door) == obj->temp3 && doorsfound > 1)
            {
                if (++doornum >= doorsfound)
                    doornum = 0;

                door = doorlist[doornum];
            }

            obj->temp3 = DOORNUM(door);

            obj->s_tilex = door->tilex;
            obj->s_tiley = door->tiley;

            obj->flags |= FL_RUNTOSTATIC;
        }
    }
    else
    {
        //
        // obj is running to corner (leave it alone) or
        // chasing an object already removed by another
        // actor (make obj run to corner)
        //
        if (obj->flags & FL_RUNTOSTATIC)
            obj->s_tilex = 0;
    }

    return false;
}


/*
===============
=
= CheckRunChase
=
===============
*/

unsigned CheckRunChase (objtype *obj)
{
    #define RUNAWAY_SPEED 1000

    unsigned RunReason = 0;

//
// mark the reason for running
//
    if (!obj->ammo)                     // out of ammo!
        RunReason |= RR_AMMO;

    if (obj->hitpoints <= starthitpoints[gamestate.difficulty][obj->obclass - rentacopobj] >> 1)
        RunReason |= RR_HEALTH;         // feeling sickly!

    if ((obj->flags & (FL_FRIENDLY | FL_INTERROGATED)) == FL_INTERROGATED)
        RunReason |= RR_INTERROGATED;   // non-informant was interrogated!

//
// time to RUN or CHASE?
//
    if (RunReason)          // run, Run, RUN!
    {
        if (!(obj->flags & FL_RUNAWAY))
        {
            obj->temp3 = 0;
            obj->flags |= FL_RUNAWAY;
            obj->speed += RUNAWAY_SPEED;
        }
    }
    else                    // chase, Chase, CHASE!
    {
        if (obj->flags & FL_RUNAWAY)
        {
            obj->flags &= ~FL_RUNAWAY;
            obj->speed -= RUNAWAY_SPEED;
        }
    }

    return RunReason;
}


/*
===============
=
= SeekPlayerOrStatic
=
===============
*/

void SeekPlayerOrStatic (objtype *obj, fixed *deltax, fixed *deltay)
{
    unsigned whyrun = 0;
    bool     smart = false;

//
// is this a "smart" actor?
//
    switch (obj->obclass)
    {
        case SMART_ACTORS:
            smart = true;
            break;

        case electrosphereobj:
            if (!obj->s_tilex)
                GetCornerSeek (obj);

            *deltax = obj->s_tilex - obj->tilex;
            *deltay = obj->s_tiley - obj->tiley;
            return;
    }

//
// should obj run away (chase static) or chase player?
//
    whyrun = CheckRunChase(obj);

    if (smart && whyrun)
    {
        //
        // initilize seek tile?
        //
        if (!obj->s_tilex)
            GetCornerSeek (obj);

        //
        // are there any goodies available?
        //
        if (!LookForGoodies(obj,whyrun))
        {
            //
            // change seek tile when obj reaches it
            //
            if (obj->tilex == obj->s_tilex && obj->tiley == obj->s_tiley)
            {
                GetCornerSeek (obj);
                obj->flags &= ~FL_INTERROGATED;
            }

            *deltax = obj->s_tilex - obj->tilex;
            *deltay = obj->s_tiley - obj->tiley;
        }
        else
            whyrun = CheckRunChase(obj);
    }

//
// make obj chase player if it's not running
//
    if (!whyrun)
    {
        *deltax = player->tilex - obj->tilex;
        *deltay = player->tiley - obj->tiley;
    }
}


/*
===============
=
= PlayerIsBlocking
=
===============
*/

void PlayerIsBlocking (objtype *obj)
{
    fixed distance = obj->distance;

    obj->dir = opposite[obj->dir];

    if (!TryWalk(obj,true))
    {
        obj->dir = nodir;
        return;
    }

    obj->distance = TILEGLOBAL - distance;
}


/*
===============
=
= MakeAlertNoise
=
===============
*/

void MakeAlertNoise (objtype *obj)
{
    madenoise = true;
    alerted = 2;
    alertedarea = obj->areanumber;
}
