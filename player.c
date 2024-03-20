//
// basic player movement

#include "3d_def.h"

#define MAXMOUSETURN    10


#define MOVESCALE       150l
#define BACKMOVESCALE   100l
#define ANGLESCALE      20

#define BASEMOVE        35
#define RUNMOVE         70
#define BASETURN        35
#define RUNTURN         70

#define JOYSCALE        2

#define GRENADE_ENERGY_USE		4
#define BFG_ENERGY_USE			(GRENADE_ENERGY_USE<<1)

int             playstate;

int             shootdelta;
bool            GAN_HiddenArea;
int             mapoffset[8] = {1,-1,MAPSIZE,-MAPSIZE,-(MAPSIZE + 1),-(MAPSIZE - 1),MAPSIZE - 1,MAPSIZE + 1};

int             lastmusicchunk;
int             lastmusicoffset;

int             music_num;

int             DebugOk = 0;

bool            godmode;

objtype         objlist[MAXACTORS],*lastobj;
objtype         *player,*objfreelist,*lastattacker;

objtype         *actorat[MAPSIZE][MAPSIZE];

int32_t         thrustspeed;

int             anglefrac;
int             tics,realtics;

unsigned        ExtraRadarFlags;

int  dirscan[4] = { sc_UpArrow, sc_RightArrow, sc_DownArrow, sc_LeftArrow };
int  buttonscan[NUMBUTTONS] = { sc_Control, sc_Alt, sc_LShift, sc_Space, sc_1, sc_2, sc_3, sc_4 };
bool buttonheld[NUMBUTTONS];

//
// current user input
//
int  controlx,controly;         // range from -100 to 100 per tic
bool buttonstate[NUMBUTTONS];

bool demorecord,demoplayback;

char LocationText[MAX_LOCATION_DESC_LEN];

bool    TryMove (void);
void    Thrust (int angle, int32_t speed);
void    ClipMove (fixed xmove, fixed ymove);
void    T_Player (objtype *obj);
void    T_Attack (objtype *obj);

statetype s_player = {0,0,0,T_Player,NULL,NULL};
statetype s_attack = {0,0,0,T_Attack,NULL,NULL};

void    InitObjList (void);
objtype *GetNewObj (void);
void    RemoveObj (objtype *gone);
void    DoActor (objtype *obj);


atkinf_t attackinfo[NUMWEAPONS][4] =

{
{ {6,0,1},{6,2,2},{6,0,3},{6,-1,4} },     // Auto charge
{ {6,0,1},{6,1,2},{6,0,3},{6,-1,4} },     // Pistol
{ {6,0,1},{6,1,2},{5,3,3},{5,-1,4} },		// Pulse
{ {6,0,1},{6,1,2},{3,4,3},{3,-1,4} },		// ION
{ {6,0,1},{6,5,2},{6,6,3},{6,-1,4} },
{ {6,0,1},{6,9,2},{6,10,3},{6,-1,4} },
{ {5,7,0},{5,8,0},{2,-2,0},{0,0,0} },
};


/*
=============================================================================

						CONTROL STUFF

=============================================================================
*/


/*
======================
=
= CheckWeaponChange
=
= Keys 1-6 change weapons
=
======================
*/

void CheckWeaponChange (void)
{
	int	i;

	for (i = wp_autocharge; i <= wp_bfg_cannon; i++)
	{
		if (buttonstate[bt_ready_autocharge + i - wp_autocharge])
		{
			if (gamestate.useable_weapons & (1 << i))
			{
				gamestate.weapon = gamestate.chosenweapon = i;

				DISPLAY_TIMED_MSG (WeaponAvailMsg,MP_WEAPON_AVAIL,MT_GENERAL);

				DrawWeapon ();

				return;
			}
			else
				DISPLAY_TIMED_MSG (WeaponNotAvailMsg,MP_WEAPON_AVAIL,MT_GENERAL);
		}
	}
}


/*
=======================
=
= ControlMovement
=
= Takes controlx,controly, and buttonstate[bt_strafe]
=
= Changes the player's angle and position
=
= There is an angle hack because when going 70 fps, the roundoff becomes
= significant
=
=======================
*/

void ControlMovement (void)
{
    int angle;
    int angleunits;

    thrustspeed = 0;

    if (buttonstate[bt_strafeleft])
    {
        angle = player->angle + ANG90;

        if (angle >= ANG360)
            angle -= ANG360;

        if (buttonstate[bt_run])
            Thrust (angle,RUNMOVE * MOVESCALE * tics);
        else
            Thrust (angle,BASEMOVE * MOVESCALE * tics);
    }

    if (buttonstate[bt_straferight])
    {
        angle = player->angle - ANG90;

        if (angle < 0)
            angle += ANG360;

        if (buttonstate[bt_run])
            Thrust (angle,RUNMOVE * MOVESCALE * tics);
        else
            Thrust (angle,BASEMOVE * MOVESCALE * tics);
    }

    //
    // side to side move
    //
    if (buttonstate[bt_strafe])
    {
        //
        // strafing
        //
        if (controlx > 0)
        {
            angle = player->angle - ANG90;

            if (angle < 0)
                angle += ANG360;

            Thrust (angle,controlx * MOVESCALE);      // move to left
        }
        else if (controlx < 0)
        {
            angle = player->angle + ANG90;

            if (angle >= ANG360)
                angle -= ANG360;

            Thrust (angle,-controlx * MOVESCALE);     // move to right
        }
    }
    else
    {
        //
        // not strafing
        //
        anglefrac += controlx;
        angleunits = anglefrac / ANGLESCALE;
        anglefrac -= angleunits * ANGLESCALE;
        player->angle -= angleunits;

        if (player->angle >= ANG360)
            player->angle -= ANG360;
        if (player->angle < 0)
            player->angle += ANG360;

    }

    //
    // forward/backwards move
    //
    if (controly < 0)
        Thrust (player->angle,-controly * MOVESCALE);    // move forwards
    else if (controly > 0)
    {
        angle = player->angle + ANG180;

        if (angle >= ANG360)
            angle -= ANG360;

        Thrust (angle,controly * BACKMOVESCALE);         // move backwards
    }
}


/*
=============================================================================

                            MUSIC STUFF

=============================================================================
*/

//
// list of songs for each level
//
int songs[] =
{
    MAJMIN_MUS,
    STICKS_MUS,
    MOURNING_MUS,
    LURKING_MUS,
    CIRCLES_MUS,
    TIME_MUS,
    TOHELL_MUS,
    FORTRESS_MUS,
    GIVING_MUS,
    HARTBEAT_MUS,
    MOURNING_MUS,
    MAJMIN_MUS,
    VACCINAP_MUS,
    LURKING_MUS,
    MONASTRY_MUS,
    TOMBP_MUS,
    DARKNESS_MUS,
    MOURNING_MUS,
    SERPENT_MUS,
    TIME_MUS,
    CATACOMB_MUS,
    PLOT_MUS,
    GIVING_MUS,
    VACCINAP_MUS,
};


/*
=================
=
= StopMusic
=
=================
*/

int StopMusic (void)
{
    int lastoffs = SD_MusicOff();

    CA_UncacheAudioChunk (STARTMUSIC + lastmusicchunk);

    return lastoffs;
}


/*
=================
=
= StartMusic
=
=================
*/

void StartMusic (void)
{
    SD_MusicOff ();

    lastmusicchunk = songs[currentsong];

    SD_StartMusic (STARTMUSIC + lastmusicchunk);
}


/*
=================
=
= ContinueMusic
=
=================
*/

void ContinueMusic (int offs)
{
    SD_MusicOff ();

    lastmusicchunk = songs[currentsong];

    SD_ContinueMusic (STARTMUSIC + lastmusicchunk,offs);
}


/*
===================
=
= PollKeyboardButtons
=
===================
*/

void PollKeyboardButtons (void)
{
    int i;

    for (i = 0; i < NUMBUTTONS; i++)
    {
        if (Keyboard[buttonscan[i]])
            buttonstate[i] = true;
    }
}


/*
===================
=
= PollKeyboardMove
=
===================
*/

void PollKeyboardMove (void)
{
    int delta = buttonstate[bt_run] ? RUNMOVE * tics : BASEMOVE * tics;

    if (Keyboard[dirscan[di_north]])
        controly -= delta;
    if (Keyboard[dirscan[di_south]])
        controly += delta;
    if (Keyboard[dirscan[di_west]])
        controlx -= delta;
    if (Keyboard[dirscan[di_east]])
        controlx += delta;
}


/*
===================
=
= PollControls
=
= Gets user or demo input, call once each frame
=
= controlx      set between -100 and 100 per tic
= controly
= buttonheld[]  the state of the buttons LAST frame
= buttonstate[] the state of the buttons THIS frame
=
===================
*/

void PollControls (void)
{
    int max,min;

    IN_ProcessEvents ();

//
// get timing info for last frame
//
    CalcTics ();

    controlx = 0;
    controly = 0;
    memcpy (buttonheld,buttonstate,sizeof(buttonstate));
    memset (buttonstate,0,sizeof(buttonstate));

//
// get button states
//
    PollKeyboardButtons ();

//
// get movements
//
    PollKeyboardMove ();

//
// bound movement to a maximum
//
    max = 100 * tics;
    min = -max;
    if (controlx > max)
        controlx = max;
    else if (controlx < min)
        controlx = min;

    if (controly > max)
        controly = max;
    else if (controly < min)
        controly = min;
}


/*
===================
=
= TryMove
=
===================
*/

bool TryMove (void)
{
    int     xl,yl,xh,yh,x,y;
    objtype *check;

    xl = (player->x - MINDIST) >> TILESHIFT;
    yl = (player->y - MINDIST) >> TILESHIFT;

    xh = (player->x + MINDIST) >> TILESHIFT;
    yh = (player->y + MINDIST) >> TILESHIFT;

    //
    // check for solid walls
    //
    for (y = yl; y <= yh; y++)
    {
        for (x = xl; x <= xh; x++)
        {
            check = actorat[x][y];

            if (check && !ISPOINTER(check))
                return false;
        }
    }

    return true;
}


/*
===================
=
= ClipMove
=
===================
*/

void ClipMove (fixed xmove, fixed ymove)
{
    fixed basex,basey;

    basex = player->x;
    basey = player->y;

    player->x = basex + xmove;
    player->y = basey + ymove;

    if (TryMove())
        return;

    player->x = basex + xmove;
    player->y = basey;

    if (TryMove())
        return;

    player->x = basex;
    player->y = basey + ymove;

    if (TryMove())
        return;

    player->x = basex;
    player->y = basey;
}


/*
===================
=
= Thrust
=
===================
*/

void Thrust (int angle, int32_t speed)
{
    fixed xmove,ymove;

    thrustspeed += speed;

    //
    // moving bounds speed
    //
    if (speed >= MINDIST * 2)
        speed = MINDIST * 2 - 1;

    xmove = FixedMul(speed,costable[angle]);
    ymove = -FixedMul(speed,sintable[angle]);

    ClipMove (xmove,ymove);

    player->tilex = player->x >> TILESHIFT;
    player->tiley = player->y >> TILESHIFT;

	player->areanumber = GetAreaNumber(player->tilex,player->tiley);

	areabyplayer[player->areanumber] = true;
}


/*
===================
=
= GetAreaNumber
=
===================
*/

unsigned GetAreaNumber (int tilex, int tiley)
{
    int      i;
    word     *map;
    unsigned areanumber;

    GAN_HiddenArea = false;

//
// are we on a wall?
//
    if (tilemap[tilex][tiley] && !(tilemap[tilex][tiley] & 0xc0))
        return 127;

//
// get initial areanumber from map
//
    map = &MAPSPOT(tilex,tiley,0);

//
// special tile areas must use a valid areanumber tile around them
//
    areanumber = ValidAreaTile(*map);

    if (!areanumber)
    {
        for (i = 0; i < 8; i++)
        {
            areanumber = ValidAreaTile(map[mapoffset[i]]);

            if (areanumber)
                break;
        }

        if (i == 8)
            areanumber = AREATILE;
    }

//
// merge hidden areanumbers into non-hidden areanumbers & pull all
// values down to an indexable range
//
    if (areanumber >= HIDDENAREATILE)
    {
        GAN_HiddenArea = true;
        areanumber -= HIDDENAREATILE;
    }
    else
        areanumber -= AREATILE;

    return areanumber;
}


/*
===================
=
= ValidAreaTile
=
===================
*/

unsigned ValidAreaTile (unsigned area)
{
    switch (area)
    {
        case AREATILE:
        case HIDDENAREATILE:
        case DOORTRIGGERTILE:
        case WINTIGGERTILE:
        case SMART_ON_TRIGGER:
        case SMART_OFF_TRIGGER:
        case AMBUSHTILE:
        case LINC_TILE:
        case CLOAK_AMBUSH_TILE:
            break;

        default:
            if (area > AREATILE)
                return area;
    }

    return 0;
}


/*
===============
=
= Cmd_Fire
=
===============
*/

void Cmd_Fire (void)
{
#ifdef NOTYET
	if (noShots)
		return;
#endif
	if (gamestate.weapon == wp_autocharge && gamestate.weapon_wait)
		return;

	buttonheld[bt_attack] = true;

	player->state = &s_attack;

	gamestate.attackframe = 0;
	gamestate.attackcount = attackinfo[gamestate.weapon][gamestate.attackframe].tics;
	gamestate.weaponframe =	attackinfo[gamestate.weapon][gamestate.attackframe].frame;
}


/*
===============
=
= Cmd_Use
=
===============
*/

void Cmd_Use (void)
{
    int checkx,checky,doornum,dir;

    //
    // find which cardinal direction the player is facing
    //
    if (player->angle < ANG45 || player->angle > ANG315)
    {
        checkx = player->tilex + 1;
        checky = player->tiley;
        dir = di_east;
    }
    else if (player->angle < ANG135)
    {
        checkx = player->tilex;
        checky = player->tiley - 1;
        dir = di_north;
    }
    else if (player->angle < ANG225)
    {
        checkx = player->tilex - 1;
        checky = player->tiley;
        dir = di_west;
    }
    else
    {
        checkx = player->tilex;
        checky = player->tiley + 1;
        dir = di_south;
    }

    doornum = tilemap[checkx][checky];

    if (MAPSPOT(checkx,checky,1) == PUSHABLETILE)
    {
        //
        // pushable wall
        //

        PushWall (checkx,checky,dir);
        return;
    }

    if (!buttonheld[bt_use] && doornum & 0x80)
    {
        buttonheld[bt_use] = true;
        OperateDoor (doornum & ~0x80);
    }
    //else
    //    SD_PlaySound (DONOTHINGSND);
}


/*
===============
=
= SpawnPlayer
=
===============
*/

void SpawnPlayer (int tilex, int tiley, int dir)
{
    player->obclass = playerobj;
    player->active = ac_yes;
    player->tilex = tilex;
    player->tiley = tiley;
    player->areanumber = GetAreaNumber(player->tilex,player->tiley);
    player->x = ((fixed)tilex << TILESHIFT) + TILECENTER;
    player->y = ((fixed)tiley << TILESHIFT) + TILECENTER;
    player->flags = FL_NEVERMARK;
    player->state = &s_player;
    player->angle = (1 - dir) * ANG90;

    if (player->angle < 0)
        player->angle += ANG360;

    Thrust (0,0);                           // set some variables

    InitAreas ();
}


/*
===============
=
= GunAttack
=
===============
*/

void GunAttack (objtype *obj)
{
    objtype *check,*closest,*oldclosest;
//    int     damage;
//    int     dx,dy,dist;
    int32_t viewdist;
    bool    skip = false;
#ifdef NOTYET
    if (gamestate.weapon != wp_autocharge)
        MakeAlertNoise (obj);
#endif
    switch (gamestate.weapon)
    {
        case wp_autocharge:
            SD_PlaySound (ATKAUTOCHARGESND);
            skip = true;
            break;

        case wp_pistol:
            SD_PlaySound (ATKCHARGEDSND);
            skip = true;
            break;

        case wp_burst_rifle:
            SD_PlaySound (ATKBURSTRIFLESND);
            break;

        case wp_ion_cannon:
            SD_PlaySound (ATKIONCANNONSND);
            break;
    }

    //
    // find potential targets
    //
    viewdist = 0x7fffffff;
    closest = NULL;

    while (1)
    {
        oldclosest = closest;

        for (check = obj->next; check; check = check->next)
        {
            if ((check->flags & FL_SHOOTABLE) && (check->flags & FL_VISIBLE))
            {
                if (abs(check->viewx - centerx) < shootdelta && check->transx < viewdist)
                {
                    if ((skip && check->obclass == hang_terrotobj) || (check->flags2 & FL2_NOTGUNSHOOTABLE))
                        continue;

                    viewdist = check->transx;
                    closest = check;
                }
            }
        }

        if (closest == oldclosest)
            return;                 // no more targets, all missed

        //
        // trace a line from player to enemy
        //
        if (CheckLine(closest,player))
            break;
    }
#ifdef NOTYET
    //
    // hit something
    //
    dx = abs(closest->tilex - player->tilex);
    dy = abs(closest->tiley - player->tiley);
    dist = dx > dy ? dx : dy;

    if (dist < 2)
        damage = US_RndT() / 2;   // 4
    else if (dist < 4)
        damage = US_RndT() / 4;   // 6
    else
    {
        if ((US_RndT() / 12) < dist)        // missed
            return;

        damage = US_RndT() / 4;   // 6
    }

    DamageActor (closest,damage,player);
#endif
}


/*
===============
=
= T_Player
=
===============
*/

void T_Player (objtype *obj)
{
    CheckWeaponChange ();

	if (gamestate.weapon == wp_autocharge)
		UpdateAmmoMsg ();

    if (buttonstate[bt_use])
        Cmd_Use ();

    if (buttonstate[bt_attack] && !buttonheld[bt_attack])
        Cmd_Fire ();

    ControlMovement ();

    player->tilex = player->x >> TILESHIFT;
    player->tiley = player->y >> TILESHIFT;
}


/*
===============
=
= T_Attack
=
===============
*/

void T_Attack (objtype *obj)
{
    atkinf_t *cur;
    int      i;
#ifdef NOTYET
    if (noShots)  // TODO: something related to goldfire morphing
    {
        obj->state = &s_player;
        gamestate.attackframe = gamestate.weaponframe = 0;
        return;
    }
#endif
    if (gamestate.weapon == wp_autocharge)
        UpdateAmmoMsg ();

    if (buttonstate[bt_use] && !buttonheld[bt_use])
        buttonstate[bt_use] = false;

    if (buttonstate[bt_attack] && !buttonheld[bt_attack])
        buttonstate[bt_attack] = false;

    ControlMovement ();

    player->tilex = player->x >> TILESHIFT;
    player->tiley = player->y >> TILESHIFT;

//
// change frame and fire
//
    gamestate.attackcount -= tics;

    if (gamestate.attackcount <= 0)
    {
        cur = &attackinfo[gamestate.weapon][gamestate.attackframe];

        switch (cur->attack)
        {
            case -1:
                obj->state = &s_player;

                if (!gamestate.ammo)
                {
                    if (gamestate.weapon != wp_autocharge)
                    {
                        gamestate.weapon = wp_autocharge;

                        DrawWeapon ();
                        DisplayInfoMsg (EnergyPackDepleted,MP_NO_MORE_AMMO,DISPLAY_MSG_STD_TIME << 1,MT_OUT_OF_AMMO);
                    }
                }
                else
                {
                    if (!(gamestate.useable_weapons & (1 << gamestate.weapon)))
                    {
                        gamestate.weapon = wp_autocharge;

                        DrawWeapon ();
                        DisplayInfoMsg (NotEnoughEnergyForWeapon,MP_NO_MORE_AMMO,DISPLAY_MSG_STD_TIME << 1,MT_OUT_OF_AMMO);
                    }
                }

                gamestate.attackframe = gamestate.weaponframe = 0;
                return;

            case -2:
                obj->state = &s_player;

                if (!gamestate.plasma_detonators)
                {
                    //
                    // check to see what weapons are possible
                    //
                    for (i = wp_bfg_cannon; i >= wp_autocharge; i--)
                    {
                        if (gamestate.useable_weapons & (1 << i))
                        {
                            gamestate.weapon = i;
                            break;
                        }
                    }

                    DrawWeapon ();
                    /*DisplayInfoMsg (pd_switching,MP_NO_MORE_AMMO,DISPLAY_MSG_STD_TIME << 1,MT_OUT_OF_AMMO);*/
                }

                gamestate.attackframe = gamestate.weaponframe = 0;
                return;

            case 4:
                if (!gamestate.ammo)
                    break;

                if (buttonstate[bt_attack])
                    gamestate.attackframe -= 2;

            case 0:
                if (gamestate.weapon == wp_grenade)
                {
                    if (!objfreelist)
                    {
                        DISPLAY_TIMED_MSG (WeaponMalfunction,MP_WEAPON_MALFUNCTION,MT_MALFUNCTION);

                        gamestate.attackframe++;
                    }
                }
                break;

            case 1:
                if (!gamestate.ammo)
                {
                    gamestate.attackframe++;
                    break;
                }

                GunAttack (obj);

                if (!godmode)
                    gamestate.ammo--;

                DrawAmmo (false);
                break;

            case 2:
                if (gamestate.weapon_wait)
                    break;

                GunAttack (obj);
                gamestate.weapon_wait = AUTOCHARGE_WAIT;

                DrawAmmo (false);
                break;

            case 3:
                if (gamestate.ammo && buttonstate[bt_attack])
                    gamestate.attackframe -= 2;
                break;

            case 6:
                if (gamestate.ammo && buttonstate[bt_attack])
                {
                    if (objfreelist)
                        gamestate.attackframe -= 2;
                    else
                        DISPLAY_TIMED_MSG (WeaponMalfunction,MP_WEAPON_MALFUNCTION,MT_MALFUNCTION);
                }
                break;

            case 5:
                if (!objfreelist)
                {
                    DISPLAY_TIMED_MSG (WeaponMalfunction,MP_WEAPON_MALFUNCTION,MT_MALFUNCTION);
                    gamestate.attackframe++;
                }
                else
                {
                    if (LastMsgType == MT_MALFUNCTION)
                        MsgTicsRemain = 1;      // Clear Malfuction Msg before anim

                    if (!godmode)
                    {
                        if (gamestate.ammo >= GRENADE_ENERGY_USE)
                        {
                            gamestate.ammo -= GRENADE_ENERGY_USE;

                            DrawAmmo (false);
                        }
                        else
                            gamestate.attackframe++;
                    }

                    SD_PlaySound (ATKGRENADESND);
#ifdef NOTYET
                    SpawnProjectile (obj,grenadeobj);
                    MakeAlertNoise (obj);
#endif
                }
                break;

            case 7:
#ifdef NOTYET
                TryDropPlasmaDetonator ();
#endif
                DrawAmmo (false);
                break;

            case 8:
                if (gamestate.plasma_detonators && buttonstate[bt_attack])
                    gamestate.attackframe -= 2;
                break;

            case 9:
                if (!objfreelist)
                {
                    DISPLAY_TIMED_MSG (WeaponMalfunction,MP_WEAPON_MALFUNCTION,MT_MALFUNCTION);
                    gamestate.attackframe++;
                }
                else
                {
                    if (LastMsgType == MT_MALFUNCTION)
                        MsgTicsRemain = 1;      // Clear Malfuction Msg before anim

                    if (!godmode)
                    {
                        if (gamestate.ammo >= BFG_ENERGY_USE)
                        {
                            gamestate.ammo -= BFG_ENERGY_USE;

                            DrawAmmo (false);
                        }
                        else
                            gamestate.attackframe++;
                    }

                    SD_PlaySound (ATKIONCANNONSND);    // JTR - this needs to change
#ifdef NOTYET
                    SpawnProjectile (obj,bfg_shotobj);
                    MakeAlertNoise (obj);
#endif
                }
                break;

            case 10:
                if (gamestate.ammo && buttonstate[bt_attack])
                {
                    if (objfreelist)
                        gamestate.attackframe -= 2;
                    else
                        DISPLAY_TIMED_MSG (WeaponMalfunction,MP_WEAPON_MALFUNCTION,MT_MALFUNCTION);
                }
                break;
        }

        gamestate.attackcount += cur->tics;
        gamestate.attackframe++;
        gamestate.weaponframe = attackinfo[gamestate.weapon][gamestate.attackframe].frame;
    }
}


/*
=============================================================================

                            OBJLIST DATA STRUCTURE

=============================================================================
=
= The objlist contains structures for every actor currently playing. The structure
= is accessed as a linked list starting at *player, ending when obj->next == NULL.
= GetNewObj inserts a new object at the end of the list, meaning that if an actor
= spawns another actor, the new one WILL get to think and react the same frame.
= RemoveObj unlinks the given object and returns it to the free list, but does
= not damage the object's ->next pointer, so if the current object removes itself,
= a linked list following loop can still safely get to the next element.
=
= <backwardly linked free list>
=
=============================================================================
*/


/*
=========================
=
= InitObjList
=
= Call to clear out the actor object lists returning them all to the free
= list. Allocates a special spot for the player.
=
=========================
*/

void InitObjList (void)
{
    int i;

    //
    // init the actor lists and nullify global pointers
    //
    // TODO: make sure fixing the overflow doesn't break Blake
    //
    for (i = 0; i < MAXACTORS; i++)
    {
        objlist[i].prev = &objlist[i + 1];
        objlist[i].next = NULL;
    }

    objlist[MAXACTORS - 1].prev = NULL;

    objfreelist = &objlist[0];
    lastobj = lastattacker = NULL;

    //
    // give the player the first free spots
    //
    player = GetNewObj();
}


/*
=========================
=
= GetNewObj
=
= Returns a pointer to a free spot in objlist.
= The free spot is inserted at the end of the liked list
=
= When the object list is full, the caller can either have it bomb out ot
= return a dummy object pointer that will never get used
=
=========================
*/

objtype *GetNewObj (void)
{
    objtype *newobj = NULL;

    if (!objfreelist)
        Quit ("No free spots in objlist!");

    newobj = objfreelist;
    objfreelist = newobj->prev;
    memset (newobj,0,sizeof(*newobj));

    if (lastobj)
        lastobj->next = newobj;

    newobj->prev = lastobj;     // newobj->next is already NULL from memset
    newobj->active = ac_no;
    lastobj = newobj;

    return newobj;
}


/*
=========================
=
= RemoveObj
=
= Add the given object back into the free list, and unlink it from it's
= neighbors
=
=========================
*/

void RemoveObj (objtype *gone)
{
    if (gone == player)
        Quit ("Tried to remove the player!");

    gone->state = NULL;

    //
    // fix the next object's back link
    //
    if (gone == lastobj)
        lastobj = gone->prev;
    else
        gone->next->prev = gone->prev;

    //
    // fix the previous object's forward link
    //
    gone->prev->next = gone->next;

    //
    // add it back in to the free list
    //
    gone->prev = objfreelist;
    objfreelist = gone;
}


/*
=====================
=
= DoActor
=
=====================
*/

void DoActor (objtype *obj)
{
    void (*think)(objtype *);

    if ((obj->flags & FL_FREEZE) || (!obj->active && !areabyplayer[obj->areanumber]))
		return;

    if (!(obj->flags & (FL_NONMARK | FL_NEVERMARK)))
        actorat[obj->tilex][obj->tiley] = NULL;

//
// non transitional object
//
    if (!obj->ticcount)
    {
        think = obj->state->think;

        if (think)
        {
            think (obj);

            if (!obj->state)
            {
                RemoveObj (obj);
                return;
            }
        }

        if (obj->flags & FL_NEVERMARK)
            return;

        if ((obj->flags & FL_NONMARK) && actorat[obj->tilex][obj->tiley])
            return;

        actorat[obj->tilex][obj->tiley] = obj;
        return;
    }

//
// transitional object
//
    obj->ticcount -= tics;

    while (obj->ticcount <= 0)
    {
        think = obj->state->action;        // end of state action

        if (think)
        {
            think (obj);

            if (!obj->state)
            {
                RemoveObj (obj);
                return;
            }
        }

        //
        // TODO: action (obj) could have changed obj->state; we should test
        // if obj->ticcount is still <= 0 before changing the state here
        //
        obj->state = obj->state->next;

        if (!obj->state)
        {
            RemoveObj (obj);
            return;
        }

        if (!obj->state->tictime)
        {
            obj->ticcount = 0;
            break;
        }

        obj->ticcount += obj->state->tictime;
    }

    //
    // think
    //
    think = obj->state->think;

    if (think)
    {
        think (obj);

        if (!obj->state)
        {
            RemoveObj (obj);
            return;
        }
    }

    if (obj->flags & FL_NEVERMARK)
        return;

    if ((obj->flags & FL_NONMARK) && actorat[obj->tilex][obj->tiley])
        return;

    actorat[obj->tilex][obj->tiley] = obj;
}


/*
===================
=
= PlayLoop
=
===================
*/
#ifndef NOTYET
void PlayLoop (void)
{
    objtype *obj;

    playstate = ex_stillplaying;
    lasttimecount = framecount = frameon = 0;
    pwallstate = anglefrac = 0;
    memset (buttonstate,0,sizeof(buttonstate));
    ForceUpdateStatusBar ();

    if (MousePresent)
        IN_CenterMouse ();

    do
    {
        PollControls ();

        //
        // actor thinking
        //
        MoveDoors ();
        MovePWalls ();

        for (obj = player; obj; obj = obj->next)
            DoActor (obj);

        ThreeDRefresh ();

        gamestate.timecount += tics;

        UpdateSoundLoc ();

    } while (playstate == ex_stillplaying);

    gamestate.flags &= ~GS_VIRGIN_LEVEL;
}
#else
void PlayLoop (void)
{
    bool    reset_areas = false;
    int     give;
    objtype *obj;

    playstate = ex_stillplaying;
    lasttimecount = framecount = frameon = 0;
    pwallstate = anglefrac = 0;
    memset (buttonstate,0,sizeof(buttonstate));
    ClearPaletteShifts ();
    ForceUpdateStatusBar ();

    if (MousePresent)
        IN_CenterMouse ();

    tics = 1;   // for first time through

    if (demoplayback)
        IN_StartAck ();

    do
    {
        PollControls ();

        //
        // actor thinking
        //
        madenoise = false;

        if (alerted)
            alerted--;

        MoveDoors ();
        MovePWalls ();

        for (obj = player; obj; obj = obj->next)
        {
            if (obj != player && (Keyboard[sc_6] || Keyboard[sc_7]) && Keyboard[sc_8] && DebugOk)
            {
                if (!reset_areas)
                    memset (areabyplayer,1,sizeof(areabyplayer));

                reset_areas = true;

                if (((!(obj->flags & FL_INFORMANT) && (obj->flags & FL_SHOOTABLE))
                 || (obj->obclass == liquidobj && !(obj->flags & FL_DEADGUY)))
                    DamageActor (obj,1000,player);
            }
            else if (reset_areas)
            {
                ConnectAreas ();
                reset_areas = false;
            }

            DoActor (obj);
        }

        if (NumEAWalls)
            CheckSpawnEA ();

        if (!GoldsternInfo.GoldSpawned && GoldsternInfo.SpawnCnt)
            CheckSpawnGoldstern ();

        UpdatePaletteShifts ();

        ThreeDRefresh ();

        gamestate.timecount += tics;

        UpdateSoundLoc ();

        //
        // display first-time instructions
        //
        if (ShowQuickMsg)
            ShowQuickInstructions ();

        CheckKeys ();

        if (demoplayback && demoptr == lastdemoptr)
            playstate = ex_title;

        //
        // debug aids
        //
        if (singlestep)
        {
            VW_WaitVBL (14);
            lasttimecount = GetTimeCount();
        }

        if (extravbls)
            VW_WaitVBL (extravbls);

        if (demoplayback && IN_CheckAck())
        {
            IN_ClearKeysDown ();
            playstate = ex_abort;
        }

    } while (!playstate && !startgame);

    if (playstate != ex_died)
        FinishPaletteShifts ();

    gamestate.flags &= ~GS_VIRGIN_LEVEL;
}
#endif
