// 3D_AGENT.C
//
// KS: this file is way too big! The message stuff should
// probably go to 3D_MSGS.C and the stats stuff to 3D_INTER.C
//

#include "3d_def.h"


//#define NO_STATUS_BAR

#define MAXMOUSETURN    10

#define MOVESCALE       150l
#define BACKMOVESCALE   100l
#define ANGLESCALE      20
#define MAX_DA          100

#define BAR_W           48
#define BAR_H           5

#define BAR1_COLOR      0xe0
#define BAR2_COLOR      0x30
#define BAR3_COLOR      0x10

#define PERC_W          13
#define PERC_H          5

#define SCORE_ROLL_WAIT         (60 * 10)   // tics

//
// ECG scroll rate (delay)
//
#define HEALTH_SCROLL_RATE      7
#define HEALTH_PULSE            70

//
// text "InfoArea" defines
//
#define INFOAREA_X              3
#define INFOAREA_Y              (screen.baseheight - STATUSLINES + 3)
#define INFOAREA_W              109
#define INFOAREA_H              37

#define INFOAREA_BCOLOR         0x01
#define INFOAREA_CCOLOR         0x1A
#define INFOAREA_TCOLOR         0xA6
#define INFOAREA_TSHAD_COLOR    0x04    // text shadow color

#define GRENADE_ENERGY_USE      4
#define BFG_ENERGY_USE          (GRENADE_ENERGY_USE << 1)
#define NUM_AMMO_SEGS           21

#define AMMO_SMALL_FONT_NUM_WIDTH   5

#define IA_MAX_LINE             30

#define INTERROGATEDIST         MINACTORDIST
#define MDIST                   2
#define INTG_ANGLE              45

#define TOV_X                   16
#define TOV_Y                   132

//
// weapon bounce
//
#define wb_MaxPoint             (((fixed)10 << FRACBITS) * screen.scale)
#define wb_MidPoint             (((fixed)6 << FRACBITS) * screen.scale)
#define wb_MinPoint             (((fixed)2 << FRACBITS) * screen.scale)
#define wb_MaxGoalDist          (wb_MaxPoint - wb_MidPoint)

#define wb_MaxOffset            (wb_MaxPoint + (((fixed)2 << FRACBITS) * screen.scale))
#define wb_MinOffset            (wb_MinPoint - (((fixed)2 << FRACBITS) * screen.scale))


#define GD0 0x55
#define YD0 0x35
#define RD0 0x15

#define GD1 0x53
#define YD1 0x33
#define RD1 0x13

char DimAmmo[2][22] =
{
    {GD0,GD0,GD0,GD0,GD0,GD0,GD0,YD0,YD0,YD0,YD0,YD0,YD0,YD0,RD0,RD0,RD0,RD0,RD0,RD0,RD0,RD0},
    {GD1,GD1,GD1,GD1,GD1,GD1,GD1,YD1,YD1,YD1,YD1,YD1,YD1,YD1,RD1,RD1,RD1,RD1,RD1,RD1,RD1,RD1},
};

#define GL0 0x58
#define YL0 0x38
#define RL0 0x18

#define GL1 0x56
#define YL1 0x36
#define RL1 0x16

char LitAmmo[2][22] =
{
    {GL0,GL0,GL0,GL0,GL0,GL0,GL0,YL0,YL0,YL0,YL0,YL0,YL0,YL0,RL0,RL0,RL0,RL0,RL0,RL0,RL0,RL0},
    {GL1,GL1,GL1,GL1,GL1,GL1,GL1,YL1,YL1,YL1,YL1,YL1,YL1,YL1,RL1,RL1,RL1,RL1,RL1,RL1,RL1,RL1},
};


typedef struct
{
    int16_t     x,y;
    int16_t     text_color;
    int16_t     backgr_color;
    int16_t     left_margin;
    int8_t      delay;
    int8_t      numanims;
    int8_t      framecount;
} InfoArea_t;


fixed       bounceVel,bounceDest;
int         bounceOk;
bool        useBounceOffset;
fixed       bounceOffset;

unsigned    LastMsgPri;
int         MsgTicsRemain;
int         LastInfoAttacker;
int         LastInfoAttacker_Cloaked;
int         LastMsgType;
InfoArea_t  InfoAreaSetup;

int         DrawRadarGauge_COUNT = 3;
int         DrawAmmoNum_COUNT = 3;
int         DrawAmmoPic_COUNT = 3;
//int         DrawPDAmmoPic_COUNT = 3;
int         DrawScoreNum_COUNT = 3;
int         DrawWeaponPic_COUNT = 3;
int         DrawKeyPics_COUNT = 3;
int         DrawHealthNum_COUNT = 3;

int         DrawInfoArea_COUNT = 3;
int         InitInfoArea_COUNT = 3;
int         ClearInfoArea_COUNT = 3;

int         mapoffset[8] = {1,-1,MAPSIZE,-MAPSIZE,-(MAPSIZE + 1),-(MAPSIZE - 1),MAPSIZE - 1,MAPSIZE + 1};
bool        GAN_HiddenArea;

int         tryDetonatorDelay;
int         shootdelta;

//
// player state info
//
int32_t     thrustspeed;
int         anglefrac;

objtype     *lastattacker;

bool        PlayerInvisible;

char        LocationText[MAX_LOCATION_DESC_LEN];

int         player_oldtilex;
int         player_oldtiley;

//
// interrogation variables
//
char        msg[MSG_BUFFER_LEN + 1];

void        *InfAreaMsgs[MAX_INF_AREA_MSGS];
int         NumAreaMsgs,LastInfArea;
int         FirstGenInfMsg,TotalGenInfMsgs;

scientist_t InfHintList;  // Informant messages
scientist_t NiceSciList;  // Non-informant, non-pissed messages
scientist_t MeanSciList;  // Non-informant, pissed messages

const char  int_interrogate[] = "INTERROGATE:";
const char  int_informant[] = " ^FC3aINFORMANT^FCa6";
const char  int_rr[] = "\r\r";
const char  int_haveammo[] = " HEY BLAKE,\r TAKE MY CHARGE PACK!";
const char  int_havetoken[] = " HEY BLAKE,\r TAKE MY FOOD TOKENS!";

//
// elevator variables
//
const char  if_help[] =     "UP/DN MOVES SELECTOR - ENTER ACTIVATES";
const char  if_noImage[] =  "   AREA\n"
                            "  UNMAPPED\n"
                            "\n"
                            "\n"
                            " PRESS ENTER\n"
                            " TO TELEPORT";

statsInfoType   ov_stats;
void            *ov_buffer;
bool            ov_noImage;

//
// mission stats variables
//
bool        show_stats_quick;

//
// pinball bonus
//
void    B_GAliFunc (void);
void    B_RollFunc (void);
void    B_MillFunc (void);
void    B_EManFunc (void);

PinballBonusInfo PinballBonus[] =
{
    //
    // BonusText        Points      Recur?  SpecialFunction
    //-----------------------------------------------------
    {B_GAlienDead,      0,          false,  B_GAliFunc},
    {B_ScoreRolled,     1000000l,   true,   B_RollFunc},
    {B_OneMillion,      1000000l,   false,  B_MillFunc},
    {B_ExtraMan,        0,          true,   B_EManFunc},
    {B_EnemyDestroyed,  50000l,     false,  NULL},
    {B_TotalPoints,     50000l,     false,  NULL},
    {B_InformantsAlive, 50000l,     false,  NULL},
};


char    *HandleControlCodes (char *first_ch);

void    ShowOverheadChunk (void);
void    LoadOverheadChunk (int tpNum);
void    SaveOverheadChunk (int tpNum);
void    DisplayTeleportName (int tpNum, bool locked);

void    ForceUpdateStatusBar (void);

void    UpdateRadarGuage (void);
void    DrawLedStrip (int x, int y, int leds, int max);
void    DisplayPinballBonus (void);
void    CheckPinballBonus (int32_t points);
byte    LevelCompleted (void);

void    DrawWeapon (void);
void    GiveAmmo (int ammo);
void    DrawGAmmoNum (void);
void    DrawMAmmoNum (void);
void    DrawPDAmmoMsg (void);
void    ComputeAvailWeapons (void);
void    SW_HandleActor (objtype *obj);
void    SW_HandleStatic (statobj_t *stat, int tilex, int tiley);

void    InitWeaponBounce (void);
void    HandleWeaponBounce (void);

int     ShowRatio (int bx, int by, int px, int py, int32_t total, int32_t perc, int type);

void    SpawnPlayer (int tilex, int tiley, int dir);
void    Thrust (int angle, int32_t speed);
bool    TryMove (objtype *obj, unsigned size);

void    SocketToggle (bool TurnOn);
void    CheckStatsBonus (void);

void    T_Player (objtype *obj);
void    T_Attack (objtype *obj);

statetype s_player = {0,0,0,T_Player,NULL,NULL};
statetype s_attack = {0,0,0,T_Attack,NULL,NULL};

atkinf_t  attackinfo[NUMWEAPONS][4] =
{
    { {6,0,1},{6,2,2},{6,0,3},{6,-1,4} },       // Auto charge
    { {6,0,1},{6,1,2},{6,0,3},{6,-1,4} },       // Pistol
    { {6,0,1},{6,1,2},{5,3,3},{5,-1,4} },       // Pulse
    { {6,0,1},{6,1,2},{3,4,3},{3,-1,4} },       // ION
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

    if (buttonstate[bt_strafeleft] && !gamestate.turn_around)
    {
        angle = player->angle + ANG90;

        if (angle >= ANG360)
            angle -= ANG360;

        if (buttonstate[bt_run])
            Thrust (angle,RUNMOVE * MOVESCALE * tics);
        else
            Thrust (angle,BASEMOVE * MOVESCALE * tics);
    }

    if (buttonstate[bt_straferight] && !gamestate.turn_around)
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
    if (buttonstate[bt_strafe] && !gamestate.turn_around)
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
        if (gamestate.turn_around)
        {
            controlx = 100 * tics;

            if (gamestate.turn_around < 0)
                controlx = -controlx;
        }

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

        if (gamestate.turn_around)
        {
            if (gamestate.turn_around > 0)
            {
                gamestate.turn_around -= angleunits;

                if (gamestate.turn_around < 0)
                    gamestate.turn_around = 0;
            }
            else
            {
                gamestate.turn_around -= angleunits;

                if (gamestate.turn_around > 0)
                    gamestate.turn_around = 0;
            }

            if (!gamestate.turn_around)
                player->angle = gamestate.turn_angle;
        }
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
    else
    {
        if (bounceOk)
            bounceOk--;
    }

    if (controly)
        bounceOk = 8;
    else if (bounceOk)
        bounceOk--;

    player->dir = ((player->angle + (ANG45 / 2)) % ANG360) / ANG45;
}



/*
=============================================================================

					STATUS WINDOW STUFF

=============================================================================
*/

#define StatusDrawPic(x,y,pic)    VW_DrawPic ((x),(y) + (screen.baseheight - STATUSLINES),(pic))


/*
===============
=
= StatusDrawNumber
=
= Right justifies and pads with blanks
=
===============
*/

void StatusDrawNumber (int x, int y, int width, int32_t number)
{
	unsigned length,wide,c;
	int      pic;

	ltoa (number,str,10);

	length = strlen(str);

	wide = 0;
	pic = N_BLANKPIC;

	while (length < width && wide < width)
	{
		StatusDrawPic (x,y,pic);

		x += pictable[pic - STARTPICS].width;
		wide++;
		length++;
	}

	c = 0;

	while (wide < width)
	{
	    pic = (str[c] - '0') + N_0PIC;

		StatusDrawPic (x,y,pic);

		x += pictable[pic - STARTPICS].width;
		c++;
		wide++;
	}
}


/*
=============================================================================

                    SCORE DISPLAY ROUTINES

=============================================================================
*/


/*
===============
=
= DrawHealth
=
= Marks the Health_NUM to be refreshed durring the next StatusBarRefresh
=
===============
*/

void DrawHealth (void)
{
    char *str = gamestate.health_str;

    itoa (gamestate.health,gamestate.health_str,10);

    while (*str)
        *str++ -= '0';

    DrawHealthNum_COUNT = 3;
}


/*
===============
=
= DrawHealthNum
=
===============
*/

void DrawHealthNum (void)
{
    int i;
    int num,check,pic;

    DrawHealthNum_COUNT--;

    check = 100;
    num = 0;

    for (i = 0; i < 3; i++)
    {
        if (gamestate.health < check)
            pic = NG_BLANKPIC;
        else
            pic = gamestate.health_str[num++] + NG_0PIC;

        StatusDrawPic (128 + (i * pictable[pic - STARTPICS].width),10,pic);

        check /= 10;
    }
}


/*
===============
=
= TakeDamage
=
===============
*/

void TakeDamage (int points, objtype *attacker)
{
    lastattacker = attacker;

    if (gamestate.flags & GS_ATTACK_INFOAREA)
    {
        if (attacker)
        {
            if (LastMsgType == MT_ATTACK && LastInfoAttacker == attacker->obclass)
                MsgTicsRemain = DISPLAY_MSG_STD_TIME;
            else
            {
                if (DISPLAY_TIMED_MSG(ActorInfoMsg[attacker->obclass - rentacopobj],MP_TAKE_DAMAGE,MT_ATTACK))
                {
                    LastInfoAttacker = attacker->obclass;
                    LastInfoAttacker_Cloaked = attacker->flags2 & FL2_CLOAKED;
                }
            }
        }
    }

    if (godmode)
        return;

    if (gamestate.difficulty == gd_baby)
        points >>= 2;

    gamestate.health -= points;

    if (gamestate.health <= 0)
    {
        gamestate.health = 0;
        playstate = ex_died;

        if (attacker)
            attacker->flags |= FL_FREEZE;
    }

    StartDamageFlash (points);

    DrawHealth ();
}


/*
===============
=
= HealSelf
=
===============
*/

void HealSelf (int points)
{
    gamestate.health += points;

    if (gamestate.health > 100)
        gamestate.health = 100;

    DrawHealth ();
}


/*
=============================================================================

                    SCORE DISPLAY ROUTINES

=============================================================================
*/


/*
===============
=
= DrawScore
=
= Marks the Score to be refreshed durring the next StatusBarRefresh
=
===============
*/

void DrawScore (void)
{
    DrawScoreNum_COUNT = 3;
}


/*
===============
=
= DrawScoreNum
=
= NOTE: Could do some sort of "scrolling" animation on LED screen with
= ----  chars and a simple table
=
===============
*/

void DrawScoreNum (void)
{
    int x,y;

    x = 256;
    y = 3;

    if (gamestate.tic_score > MAX_DISPLAY_SCORE)
    {
        if (gamestate.score_roll_wait)
        {
            StatusDrawPic (x,y,N_BLANKPIC);
            StatusDrawPic (x + 1,y,N_DASHPIC);
            StatusDrawPic (x + 2,y,N_RPIC);
            StatusDrawPic (x + 3,y,N_OPIC);
            StatusDrawPic (x + 4,y,N_LPIC);
            StatusDrawPic (x + 5,y,N_LPIC);
            StatusDrawPic (x + 6,y,N_DASHPIC);
        }
        else
            StatusDrawNumber (x,y,7,gamestate.tic_score % (MAX_DISPLAY_SCORE + 1));
    }
    else
    {
        if (gamestate.flags & GS_TICS_FOR_SCORE)
            StatusDrawNumber (x,y,7,realtics);
        else if (gamestate.flags & GS_MUSIC_TEST)
            StatusDrawNumber (x,y,7,musicnum);
        else
            StatusDrawNumber (x,y,7,gamestate.tic_score);
    }
}


/*
===============
=
= UpdateScore
=
===============
*/

void UpdateScore (void)
{
    int32_t scorediff,temptics;

    scorediff = gamestate.score - gamestate.tic_score;

    if (scorediff)
    {
        if (scorediff > 1500)
            temptics = scorediff >> 2;
        else
            temptics = tics << 3;

        if (scorediff > temptics)
            gamestate.tic_score += temptics;
        else
            gamestate.tic_score = gamestate.score;

        DrawScore ();
    }

    if (gamestate.score_roll_wait)
    {
        gamestate.score_roll_wait -= tics;

        if (gamestate.score_roll_wait < 0)
            gamestate.score_roll_wait = 0;

        DrawScore ();
    }
}


/*
===============
=
= GivePoints
=
= score      = Holds real score
= tic_score  = Holds displayed score (tic'ing toward score)
=
===============
*/

void GivePoints (int32_t points, bool add_to_stats)
{
    if (add_to_stats)
        gamestuff.level[gamestate.mapon].stats.accum_points += points;

    CheckPinballBonus (points);

    gamestate.score += points;
}


/*
=============================================================================

                      SECURITY KEY DISPLAY ROUTINES

=============================================================================
*/


/*
===============
=
= DrawKeys
=
= Marks the security key pics to be refreshed during the next StatusBarRefresh
=
===============
*/

void DrawKeys (void)
{
    DrawKeyPics_COUNT = 3;
}


/*
===============
=
= DrawKeyPics
=
===============
*/

void DrawKeyPics (void)
{
    int i;

    DrawKeyPics_COUNT--;

    for (i = 0; i < NUMKEYS; i++)
    {
        if (gamestate.numkeys[i])
            StatusDrawPic (120 + (i * 16),27,RED_KEYPIC + i);
        else
            StatusDrawPic (120 + (i * 16),27,NO_KEYPIC);
    }
}


/*
===============
=
= GiveKey
=
===============
*/

void GiveKey (int key)
{
    gamestate.numkeys[key]++;
    DrawKeys ();
}


/*
===============
=
= TakeKey
=
===============
*/

void TakeKey (int key)
{
    gamestate.numkeys[key]--;
    DrawKeys ();
}


/*
=============================================================================

                       WEAPON DISPLAY ROUTINES

=============================================================================
*/


/*
===============
=
= DrawWeapon
=
= Marks the Weapon pics to be refreshed durring the next StatusBarRefresh
=
===============
*/

void DrawWeapon (void)
{
    DrawWeaponPic_COUNT = 3;
    DrawAmmo (true);
}


/*
===============
=
= DrawWeaponPic
=
===============
*/

void DrawWeaponPic (void)
{
    if (gamestate.weapon == -1)
        return;

    StatusDrawPic (248,24,WEAPON1PIC + gamestate.weapon);

    DrawWeaponPic_COUNT--;
}


/*
===============
=
= GiveWeapon
=
===============
*/

void GiveWeapon (int weapon)
{
    GiveAmmo (6);

    if (!(gamestate.weapons & (1 << weapon)))
    {
        gamestate.weapons |= (1 << weapon);

        if (gamestate.weapon < weapon)
        {
            gamestate.weapon = gamestate.chosenweapon = weapon;
            DrawWeapon ();
        }
    }
}


/*
=============================================================================

                       AMMO DISPLAY ROUTINES

=============================================================================
*/


/*
===============
=
= DrawAmmo
=
= Marks the AMMO NUM & AMMO PIC (if necessary) to be refreshed durring the next StatusBarRefresh
=
= NOTE: This re-computes the number of LEDs to be lit
=
===============
*/

void DrawAmmo (bool ForceRefresh)
{
    int temp;
    int ammo,max_ammo;

    ComputeAvailWeapons ();

    //
    // which weapon are we needing a refresh for?
    //
    switch (gamestate.weapon)
    {
#if 0
        case wp_plasma_detonators:
            DrawAmmoPic_COUNT = 3;
            DrawAmmoNum_COUNT = 0;
            return;
#endif
        case wp_autocharge:
            DrawAmmoPic_COUNT = 3;
            DrawAmmoNum_COUNT = 0;
            return;

        default:
            ammo = gamestate.ammo;
            max_ammo = MAX_AMMO;
            break;
    }

    if (ammo)
    {
        temp = (ammo * NUM_AMMO_SEGS) / max_ammo;

        if (!temp)
            temp = 1;
    }
    else
        temp = 0;

    gamestate.ammo_leds = temp;

    if (temp != gamestate.lastammo_leds || ForceRefresh)
    {
        gamestate.lastammo_leds = temp;
        DrawAmmoPic_COUNT = 3;
    }

    DrawAmmoNum_COUNT = 3;
}


/*
===============
=
= DrawAmmoNum
=
===============
*/

void DrawAmmoNum (void)
{
    if (gamestate.weapon == -1)
        return;

    fontnumber = 2;
    fontcolor = 0x9D;

    PrintX = 252;
    PrintY = screen.baseheight - STATUSLINES + 38;

#if 0
    switch (gamestate.weapon)
    {
        case wp_plasma_detonators:
        case wp_autocharge:
            break;

        default:
            DrawGAmmoNum ();
            break;
    }
#else
    DrawGAmmoNum ();

#endif
    DrawAmmoNum_COUNT--;
}


/*
===============
=
= DrawGAmmoNum
=
===============
*/

void DrawGAmmoNum (void)
{
    if (gamestate.ammo < 100)
    {
        PrintX += AMMO_SMALL_FONT_NUM_WIDTH;

        if (gamestate.ammo < 10)
            PrintX += AMMO_SMALL_FONT_NUM_WIDTH;
    }

    StatusDrawPic (248,32,W1_CORNERPIC + gamestate.weapon);

    snprintf (str,sizeof(str),"%d%%",gamestate.ammo);

    px = PrintX;
    py = PrintY;
    VW_DrawString (str);
}


/*
===============
=
= DrawAmmoPic
=
===============
*/

void DrawAmmoPic (void)
{
    switch (gamestate.weapon)
    {
        case wp_autocharge:
            DrawAmmoMsg ();
            break;
#if 0
        case wp_plasma_detonators:
            DrawPDAmmoMsg ();
            break;
#endif
        default:
            DrawAmmoGauge ();
        break;
    }

    DrawAmmoPic_COUNT--;
}


/*
===============
=
= DrawAmmoMsg
=
===============
*/

void DrawAmmoMsg (void)
{
    if (gamestate.weapon_wait)
        StatusDrawPic (240,0,WAITPIC);
    else
        StatusDrawPic (240,0,READYPIC);
}


/*
===============
=
= DrawPDAmmoMsg
=
===============
*/

void DrawPDAmmoMsg (void)
{
    if (gamestate.plasma_detonators)
        StatusDrawPic (240,0,READYPIC);
    else
        StatusDrawPic (240,0,WAITPIC);
}


/*
===============
=
= UpdateAmmoMsg
=
===============
*/

void UpdateAmmoMsg (void)
{
    if (gamestate.weapon_wait)
    {
        gamestate.weapon_wait -= tics;

        if (gamestate.weapon_wait <= 0)
        {
            gamestate.weapon_wait = 0;
            DrawAmmoPic_COUNT = 3;
        }
    }
}


/*
===============
=
= DrawAmmoGauge
=
===============
*/

void DrawAmmoGauge (void)
{
    DrawLedStrip (243,screen.baseheight - 45,gamestate.ammo_leds,NUM_AMMO_SEGS);
}


/*
===============
=
= UpdateRadarGauge
=
===============
*/

void UpdateRadarGauge (void)
{
    int temp;

    if (gamestate.rpower)
    {
        temp = ((int32_t)gamestate.rpower * NUM_AMMO_SEGS) / MAX_RADAR_ENERGY;

        if (temp > NUM_AMMO_SEGS)
            temp = NUM_AMMO_SEGS;

        if (!temp)
            temp = 1;
    }
    else
        temp = 0;

    gamestate.radar_leds = temp;

    if (temp != gamestate.lastradar_leds)
        gamestate.lastradar_leds = temp;

    DrawRadarGauge_COUNT = 3;
}


/*
===============
=
= DrawRadarGauge
=
===============
*/

void DrawRadarGauge (void)
{
    int zoom;

    DrawLedStrip (235,screen.baseheight - 45,gamestate.radar_leds,NUM_AMMO_SEGS);

    if (gamestate.rpower)
        zoom = gamestate.rzoom;
    else
        zoom = 0;

    StatusDrawPic (176,0,ONEXZOOMPIC + zoom);
}


/*
===============
=
= DrawLedStrip
=
===============
*/

void DrawLedStrip (int x, int y, int leds, int max)
{
    int      ypos = 0;
    unsigned amount;

    if (leds)
        amount = max - leds;
    else
        amount = max;

    //
    // draw dim LEDs
    //
    while (ypos < amount)
    {
        VW_Hlin (x,x + 4,y++,DimAmmo[0][amount]);
        VW_Hlin (x,x + 4,y++,DimAmmo[1][amount]);

        ypos++;
    }

    //
    // draw lit LEDs
    //
    while (ypos < NUM_AMMO_SEGS)
    {
        VW_Hlin (x,x + 4,y++,LitAmmo[0][amount]);
        VW_Hlin (x,x + 4,y++,LitAmmo[1][amount]);

        ypos++;
    }
}


/*
===============
=
= GiveAmmo
=
===============
*/

void GiveAmmo (int ammo)
{
#if MP_NO_MORE_AMMO > MP_BONUS
    if (LastMsgType == MT_OUT_OF_AMMO)
    {
        MsgTicsRemain = 1;
        LastMsgType = MT_CLEAR;
    }
#endif
    gamestate.ammo += ammo;

    if (gamestate.ammo > MAX_AMMO)
        gamestate.ammo = MAX_AMMO;

    DrawAmmo (false);

    if (gamestate.weapon != gamestate.chosenweapon)
    {
        if (gamestate.useable_weapons & (1 << gamestate.chosenweapon))
        {
            gamestate.weapon = gamestate.chosenweapon;
            DrawWeapon ();
        }
    }

    SD_PlaySound (GETAMMOSND);
}


/*
===============
=
= ComputeAvailWeapons
=
= Creates a bitmask for gamestate.weapons according to what
= weapon is available for useage due to ammo avail
=
===============
*/

void ComputeAvailWeapons (void)
{
    //
    // determine what ammo amounts we have avail
    //
    if (gamestate.ammo)
    {
        if (gamestate.ammo >= BFG_ENERGY_USE)
            gamestate.useable_weapons = (1 << wp_bfg_cannon)
                                      | (1 << wp_grenade)
                                      | (1 << wp_ion_cannon)
                                      | (1 << wp_burst_rifle)
                                      | (1 << wp_pistol)
                                      | (1 << wp_autocharge);
        else if (gamestate.ammo >= GRENADE_ENERGY_USE)
            gamestate.useable_weapons = (1 << wp_grenade)
                                      | (1 << wp_ion_cannon)
                                      | (1 << wp_burst_rifle)
                                      | (1 << wp_pistol)
                                      | (1 << wp_autocharge);
        else
            gamestate.useable_weapons = (1 << wp_ion_cannon)
                                      | (1 << wp_burst_rifle)
                                      | (1 << wp_pistol)
                                      | (1 << wp_autocharge);
    }
    else
        gamestate.useable_weapons = (1 << wp_autocharge);

    gamestate.useable_weapons &= gamestate.weapons;
}


/*
===============
=
= TakePlasmaDetonator
=
===============
*/

void TakePlasmaDetonator (int count)
{
    if (gamestate.plasma_detonators < count)
        gamestate.plasma_detonators = 0;
    else
        gamestate.plasma_detonators -= count;
}


/*
===============
=
= GivePlasmaDetonator
=
===============
*/

void GivePlasmaDetonator (int count)
{
    gamestate.plasma_detonators += count;

    if (gamestate.plasma_detonators > MAX_PLASMA_DETONATORS)
        gamestate.plasma_detonators = MAX_PLASMA_DETONATORS;

    ComputeAvailWeapons ();
}


/*
===============
=
= GiveToken
=
===============
*/

void GiveToken (int tokens)
{
#if MP_NO_MORE_TOKENS > MP_BONUS
    if (LastMsgType == MT_NO_MO_FOOD_TOKENS)
    {
        MsgTicsRemain = 1;
        LastMsgType = MT_CLEAR;
    }
#endif
    gamestate.tokens += tokens;

    if (gamestate.tokens > MAX_TOKENS)
        gamestate.tokens = MAX_TOKENS;

    SD_PlaySound (GOTTOKENSND);
}


/*
=============================================================================

                        INFO AREA ROUTINES

=============================================================================
*/


/*
======================
=
= DisplayInfoMsg
=
= Returns if Higher Priority message is holding
=
= SEE MACROS: DISPLAY_TIMED_MSG & DISPLAY_MSG -- 3d_def.h
=
= DISPLAY_TIMED_MSG(msg,pri,type)   - For E-Z Timed Msgs (std. display time)
= DISPLAY_MSG(msg,pri,type)         - For E-Z NON-Timed Msgs.
=
======================
*/

bool DisplayInfoMsg (const char *Msg, int Priority, int DisplayTime, int MsgType)
{
    if (Priority < LastMsgPri)
        return false;

    if (Priority == MP_max_val)   // "System" msgs
        LastMsgPri = MP_min_val;
    else
        LastMsgPri = Priority;

    MsgTicsRemain = DisplayTime;

    if (MsgTicsRemain)
        StatusDrawPic (0,40,BRI_LIGHTPIC);

    if (strlen(Msg) > MSG_BUFFER_LEN)
        Quit ("Message \"%s\" is too long!",Msg);

    snprintf (gamestate.msg,sizeof(gamestate.msg),Msg);

    DrawInfoArea_COUNT = InitInfoArea_COUNT = 3;

    LastMsgType = MsgType;

    if (LastMsgType != MT_ATTACK)
        LastInfoAttacker_Cloaked = 0;

    return true;
}


/*
======================
=
= ClearInfoArea
=
======================
*/

void ClearInfoArea (void)
{
#ifdef IN_DEVELOPMENT
    if (gamestate.flags & GS_SHOW_OVERHEAD)
        return;
#endif
    if (ClearInfoArea_COUNT)
        ClearInfoArea_COUNT--;

    InfoAreaSetup.x = InfoAreaSetup.left_margin;
    InfoAreaSetup.y = INFOAREA_Y;
    InfoAreaSetup.framecount = InfoAreaSetup.numanims = 0;

    StatusDrawPic (0,0,INFOAREAPIC);
}


/*
======================
=
= InitInfoArea
=
======================
*/

void InitInfoArea (void)
{
    InfoAreaSetup.left_margin = INFOAREA_X;
    InfoAreaSetup.text_color = INFOAREA_TCOLOR;
    InfoAreaSetup.backgr_color = INFOAREA_BCOLOR;

    InitInfoArea_COUNT--;

    ClearInfoArea ();
}


/*
======================
=
= UpdateInfoArea
=
======================
*/

void UpdateInfoArea (void)
{
    if (InfoAreaSetup.numanims)
        AnimatePage ();

    if (InitInfoArea_COUNT)
        InitInfoArea ();
    else if (ClearInfoArea_COUNT)
        ClearInfoArea ();

    if (DrawInfoArea_COUNT)
        DrawInfoArea ();
}


/*
======================
=
= UpdateInfoAreaClock
=
= This routine is called ONLY ONCE per refresh to update the
= InfoArea Clock and to release any messages that have expired
=
======================
*/

void UpdateInfoAreaClock (void)
{
    if (playstate == ex_title || playstate == ex_victorious)
        return;

    //
    // check for existing timed messages
    //
    if (LastMsgPri && MsgTicsRemain)
    {
        MsgTicsRemain -= tics;

        if (MsgTicsRemain <= 0)
            DisplayNoMoMsgs ();
    }
}


/*
======================
=
= DisplayNoMoMsgs
=
======================
*/

void DisplayNoMoMsgs (void)
{
    size_t     defmsglen,msglen;
    const char *tokenstr = "FOOD TOKENS: ";
    char       tokens[3],*tokenpos;
    char       *outmsg;

    *str = '\0';

    LastMsgPri = MP_min_val;

    if (BONUS_QUEUE)
    {
        DisplayPinballBonus ();
        return;
    }

    MsgTicsRemain = 0;
    StatusDrawPic (0,40,DIM_LIGHTPIC);

    if (gamestuff.level[gamestate.mapon + 1].locked)
    {
        switch (gamestate.mapon)
        {
            case GOLD_MORPH_LEVEL:
                snprintf (str,sizeof(str),destroyGoldfire_msg);
                break;

            case 20:
            case 21:
            case 22:
            case 23:
                break;

            default:
                if (gamestate.plasma_detonators)
                    snprintf (str,sizeof(str),haveDetonator_msg);
                else
                    snprintf (str,sizeof(str),needDetonator_msg);
                break;
        }
    }

    //
    // copy the default message to a buffer
    //
    defmsglen = strlen(default_msg);
    outmsg = SafeMalloc(defmsglen + 1);

    snprintf (outmsg,defmsglen + 1,default_msg);

    //
    // find the food tokens substring
    //
    tokenpos = strstr(outmsg,tokenstr);

    if (!tokenpos)
        Quit ("Default message has no \"%s\" sub-string!",tokenstr);

    //
    // insert the token count
    //
    snprintf (tokens,sizeof(tokens),"%-d",gamestate.tokens);

    memcpy (&tokenpos[strlen(tokenstr)],tokens,strlen(tokens));

    //
    // copy any extra message to the output
    //
    if (*str)
    {
        msglen = strlen(str);

        memcpy (&outmsg[defmsglen - msglen],str,msglen);
    }

    outmsg[defmsglen] = '\0';    // ensure it's null terminated

    DisplayInfoMsg (outmsg,MP_max_val,0,MT_NOTHING);

    free (outmsg);
}


/*
============================================================================
=
= DrawInfoArea
=
= Active control codes:
=
=  ^ANnn    - define animation
=  ^FCnn    - set font color
=  ^LMnnn   - set left margin (if 'nnn' == "fff" uses current x)
=  ^EP      - end of page (waits for 'M' to read MORE)
=  ^PXnnn   - move x to coordinate 'n'
=  ^PYnnn   - move y to coordinate 'n'
=  ^SHnnn   - display shape 'n' at current x,y
=  ^BGn     - set background color
=  ^DM      - Default Margins
=
= Other info:
=
= All 'n' values are hex numbers (0 - f), case insensitive.
= The number of N's listed is the number of digits REQUIRED by that control
= code. (IE: ^LMnnn MUST have 3 values! --> 003, 1a2, 01f, etc...)
=
= If a line consists only of control codes, the cursor is NOT advanced
= to the next line (the ending <CR><LF> is skipped). If just ONE non-control
= code is added, the number "8" for example, then the "8" is displayed
= and the cursor is advanced to the next line.
=
= The text presenter now handles sprites, but they are NOT masked! Also,
= sprite animations will be difficult to implement unless all frames are
= of the same dimensions.
=
============================================================================
*/

void DrawInfoArea (void)
{
    #define IA_FONT_HEIGHT 6

    int  temp_color;
    char *first_ch,*scan_ch;
    char temp;

#ifdef IN_DEVELOPMENT
    if (gamestate.flags & GS_SHOW_OVERHEAD)
        return;
#endif
    DrawInfoArea_COUNT--;

    if (!*gamestate.msg)
        return;

    first_ch = gamestate.msg;

    fontnumber = 2;
    fontcolor = InfoAreaSetup.text_color;

    while (first_ch && *first_ch)
    {
        if (*first_ch != TP_CONTROL_CHAR)
        {
            scan_ch = first_ch;

            while (*scan_ch && *scan_ch != '\n' && *scan_ch != TP_RETURN_CHAR && *scan_ch != TP_CONTROL_CHAR)
                scan_ch++;

            //
            // print current line
            //
            temp = *scan_ch;
            *scan_ch = '\0';

            if (*first_ch != TP_RETURN_CHAR)
            {
                temp_color = fontcolor;

                fontcolor = INFOAREA_TSHAD_COLOR;
                px = InfoAreaSetup.x + 1;
                py = InfoAreaSetup.y + 1;
                VW_DrawString (first_ch);

                fontcolor = temp_color;
                px = InfoAreaSetup.x;
                py = InfoAreaSetup.y;
                VW_DrawString (first_ch);
            }

            *scan_ch = temp;
            first_ch = scan_ch;

            //
            // skip SPACES / RETURNS at end of line
            //
            if (*first_ch == ' ' || *first_ch == TP_RETURN_CHAR)
                first_ch++;

            //
            // TP_CONTROL_CHARs don't advance to next character line
            //
            if (*scan_ch != TP_CONTROL_CHAR)
            {
                InfoAreaSetup.x = InfoAreaSetup.left_margin;
                InfoAreaSetup.y += IA_FONT_HEIGHT;
            }
            else
                InfoAreaSetup.x = px;
        }
        else
            first_ch = HandleControlCodes(first_ch);
    }
}


/*
======================
=
= HandleControlCodes
=
======================
*/

char *HandleControlCodes (char *first_ch)
{
    piShapeInfo *shape;
    piAnimInfo  *anim;
    int         code,shapenum;

    first_ch++;

    code = ReadShort(first_ch);

    first_ch += 2;

    switch (code)
    {
        //
        // INIT ANIMATION
        //
        case TP_CNVT_CODE('A','N'):
            shapenum = TP_VALUE(first_ch,2);
            first_ch += 2;
            memcpy (&piAnimList[InfoAreaSetup.numanims],&piAnimTable[shapenum],sizeof(piAnimInfo));
            anim = &piAnimList[InfoAreaSetup.numanims++];
            shape = &piShapeTable[anim->baseshape + anim->frame]; // BUG!! (assumes "pia_shapetable")

            anim->y = InfoAreaSetup.y;
            anim->x = DrawShape(InfoAreaSetup.x,InfoAreaSetup.y,shape->shapenum,shape->shapetype);
            InfoAreaSetup.framecount = 3;
            InfoAreaSetup.left_margin = InfoAreaSetup.x;
            break;

        //
        // DRAW SHAPE
        //
        case TP_CNVT_CODE('S','H'):
            //
            // NOTE : This needs to handle the left margin....
            //
            shapenum = TP_VALUE(first_ch,3);
            first_ch += 3;
            shape = &piShapeTable[shapenum];

            DrawShape (InfoAreaSetup.x,InfoAreaSetup.y,shape->shapenum,shape->shapetype);
            InfoAreaSetup.left_margin = InfoAreaSetup.x;
            break;

        //
        // FONT COLOR
        //
        case TP_CNVT_CODE('F','C'):
            fontcolor = TP_VALUE(first_ch,2);
            first_ch += 2;
            InfoAreaSetup.text_color = fontcolor;
            break;

        //
        // BACKGROUND COLOR
        //
        case TP_CNVT_CODE('B','G'):
            InfoAreaSetup.backgr_color = TP_VALUE(first_ch,2);
            first_ch += 2;
            break;

        //
        // DEFAULT MARGINS
        //
        case TP_CNVT_CODE('D','M'):
            InfoAreaSetup.left_margin = INFOAREA_X;
            break;

        //
        // LEFT MARGIN
        //
        case TP_CNVT_CODE('L','M'):
            shapenum = TP_VALUE(first_ch,3);
            first_ch += 3;

            if (shapenum == 0xfff)
                InfoAreaSetup.left_margin = InfoAreaSetup.x;
            else
                InfoAreaSetup.left_margin = shapenum;
            break;
#ifdef UNLOCK_FLOORS
        //
        // UNLOCK FLOOR
        //
        case TP_CNVT_CODE('U','F'):
            shapenum = TP_VALUE(first_ch++,1);
            gamestuff.level[shapenum].locked = false;
        break;
#endif
    }

    return first_ch;
}


/*
======================
=
= DrawShape
=
======================
*/

int DrawShape (int x, int y, int shapenum, int shapetype)
{
    int width,shade;

    //width = TP_BoxAroundShape(x,y,shapenum,shapetype);

    if (LastInfoAttacker_Cloaked)
        shade = 35;
    else
        shade = NO_SHADING;

    switch (shapetype)
    {
        case pis_scaled:
            VW_Bar (x,y,37,37,InfoAreaSetup.backgr_color);
            MegaSimpleScaleShape (x + 20,y + 19,37,shapenum,shade);
            width = 37;
            break;

        case pis_pic:
            x = (x + 7) & 0xfff8;
            width = pictable[shapenum - STARTPICS].width;
            VW_DrawPic (x,y,shapenum);
            break;
    }

    InfoAreaSetup.x += width;

    return x;
}


/*
======================
=
= AnimatePage
=
======================
*/

void AnimatePage (void)
{
    piAnimInfo  *anim;
    piShapeInfo *shape;

    anim = piAnimList;

    anim->delay += tics;

    if (anim->delay >= anim->maxdelay)
    {
        InfoAreaSetup.framecount = 3;
        anim->delay = 0;
    }

    if (InfoAreaSetup.framecount)
    {
        switch (anim->animtype)
        {
            case pia_shapetable:
                shape = &piShapeTable[anim->baseshape + anim->frame];
                DrawShape (anim->x,anim->y,shape->shapenum,shape->shapetype);
                break;

            case pia_grabscript:
                shape = &piShapeTable[anim->baseshape];
                DrawShape (anim->x,anim->y,shape->shapenum + anim->frame,shape->shapetype);
                break;
        }

        InfoAreaSetup.framecount--;

        if (!InfoAreaSetup.framecount)
        {
            anim->frame++;

            if (anim->frame == anim->maxframes)
                anim->frame = 0;
        }
    }
}


/*
=============================================================================

                       STATUS BAR REFRESH ROUTINES

=============================================================================
*/


/*
======================
=
= UpdateStatusBar
=
======================
*/

void UpdateStatusBar (void)
{
    if (playstate == ex_title || playstate == ex_victorious)
        return;
#ifdef NO_STATUS_BAR
    return;
#endif
    //
    // call specific status bar managers
    //
    UpdateScore ();
    UpdateInfoArea ();

    //
    // refresh status area
    //
    if (DrawAmmoPic_COUNT)
        DrawAmmoPic ();

    //if (DrawScoreNum_COUNT)
        DrawScoreNum ();

    if (DrawWeaponPic_COUNT)
        DrawWeaponPic ();

    if (DrawRadarGauge_COUNT)
        DrawRadarGauge ();

    //if (DrawAmmoNum_COUNT)
        DrawAmmoNum ();

    if (DrawKeyPics_COUNT)
        DrawKeyPics ();

    if (DrawHealthNum_COUNT)
        DrawHealthNum ();

    if (gamestate.flags & GS_TICS_FOR_SCORE)
        DrawScore ();
}


/*
======================
=
= ForceUpdateStatusBar
=
======================
*/

void ForceUpdateStatusBar (void)
{
    DrawScore ();
    DrawWeapon ();
    DrawKeys ();
    DrawHealth ();
    UpdateRadarGauge ();
    UpdateStatusBar ();
}


/*
====================
=
= RedrawStatusAreas
=
====================
*/

void RedrawStatusAreas (void)
{
    DrawInfoArea_COUNT = InitInfoArea_COUNT = 3;

    VW_DrawPic (0,0,TOP_STATUSBARPIC);

    StatusDrawPic (0,0,STATUSBARPIC);
    DrawAmmoPic ();
    DrawScoreNum ();
    DrawWeaponPic ();
    DrawAmmoNum ();
    DrawKeyPics ();
    DrawHealthNum ();

    ShadowPrintLocationText (sp_normal);
}


/*
=============================================================================

                        MOVEMENT

=============================================================================
*/


/*
===================
=
= GetBonus
=
= TODO: more arrays can be used to clean up this function
=
===================
*/

unsigned static_points[] =
{
    100,     // money bag
    500,     // loot
    250,     // gold1
    500,     // gold2
    750,     // gold3
    1000,    // major gold!
    5000     // bonus
};

unsigned static_health[][3] =
{
    {100,HEALTH2SND,-1},                    // Full Heal
    { 30,HEALTH1SND,-1},                    // First Aid
    { 20,HEALTH1SND,SPR_STAT_45},           // Steak
    { 15,HEALTH1SND,SPR_STAT_43},           // Chicken Leg
    { 10,HEALTH1SND,SPR_SANDWICH_WRAPER},   // Sandwich
    {  8,HEALTH1SND,SPR_CANDY_WRAPER},      // Candy Bar
    {  5,HEALTH1SND,SPR_STAT_41},           // Water bowl
    {  5,HEALTH1SND,-1},                    // Water puddle
};


void GetBonus (statobj_t *check)
{
    bool givepoints;
    int  shapenum,keynum;
    int  ammo;

    givepoints = false;
    shapenum = -1;

    switch (check->itemnumber)
    {
        case bo_red_key:
        case bo_yellow_key:
        case bo_blue_key:
            keynum = check->itemnumber - bo_red_key;

            if (gamestate.numkeys[keynum] >= MAXKEYS)
                return;

            GiveKey (keynum);
            SD_PlaySound (GETKEYSND);
            TravelTable[check->tilex][check->tiley] &= ~TT_KEYS;
            break;

        case bo_money_bag:
            SD_PlaySound (BONUS1SND);
            givepoints = true;
            break;

        case bo_loot:
            SD_PlaySound (BONUS2SND);
            givepoints = true;
            break;

        case bo_gold1:
        case bo_gold2:
        case bo_gold3:
        case bo_gold:
            SD_PlaySound (BONUS3SND);
            givepoints = true;
            break;

        case bo_bonus:
            SD_PlaySound (BONUS4SND);
            givepoints = true;
            break;

        case bo_water_puddle:
            if (gamestate.health > 15)
                return;
        case bo_fullheal:
        case bo_firstaid:
        case bo_ham:
        case bo_chicken:
        case bo_sandwich:
        case bo_candybar:
        case bo_water:
            if (gamestate.health == 100)
                return;

            SD_PlaySound (static_health[check->itemnumber - bo_fullheal][1]);
            HealSelf (static_health[check->itemnumber - bo_fullheal][0]);
            check->flags &= ~FL_BONUS;
            shapenum = static_health[check->itemnumber - bo_fullheal][2];
            break;

        case bo_clip:
            if (gamestate.ammo == MAX_AMMO)
                return;

            GiveAmmo (8);
            bonus_msg7[45] = '8';    // TODO: sus
            break;

        case bo_clip2:
            if (gamestate.ammo == MAX_AMMO)
                return;

            ammo = 1 + (US_RndT() & 7);
            bonus_msg7[45] = '0' + ammo;
            GiveAmmo (ammo);
            break;

        case bo_plasma_detonator:
            TravelTable[check->tilex][check->tiley] &= ~TT_KEYS;
            GivePlasmaDetonator (1);
            SD_PlaySound (GETDETONATORSND);
            break;

        case bo_pistol:
            SD_PlaySound (GETPISTOLSND);
            GiveWeapon (wp_pistol);
            break;

        case bo_burst_rifle:
            SD_PlaySound (GETBURSTRIFLESND);
            GiveWeapon (wp_burst_rifle);
            break;

        case bo_ion_cannon:
            SD_PlaySound (GETIONCANNONSND);
            GiveWeapon (wp_ion_cannon);
            break;

        case bo_grenade:
            SD_PlaySound (GETCANNONSND);
            GiveWeapon (wp_grenade);
            break;

        case bo_bfg_cannon:
            SD_PlaySound (GETCANNONSND);
            GiveWeapon (wp_bfg_cannon);
            break;

        case bo_coin:
            if (gamestate.tokens == MAX_TOKENS)
                return;

            GiveToken (1);

            WriteTokenString (bonus_msg24);
            break;

        case bo_coin5:
            if (gamestate.tokens == MAX_TOKENS)
                return;

            GiveToken (5);

            WriteTokenString (bonus_msg25);
            break;

        case bo_automapper1:
            if (gamestate.rpower > MAX_RADAR_ENERGY - (RADAR_PAK_VALUE / 8))
                return;

            gamestate.rpower += RADAR_PAK_VALUE;

            SD_PlaySound (RADAR_POWERUPSND);
            UpdateRadarGauge ();
            break;
    }

    if (givepoints)
    {
        GivePoints (static_points[check->itemnumber - bo_money_bag],true);
#ifdef IN_DEVELOPMENT
#ifdef DEBUG_STATICS
        debug_bonus[1][db_count++] = static_points[check->itemnumber - bo_money_bag];
#endif
#endif
    }

    DISPLAY_TIMED_MSG (BonusMsg[check->itemnumber - 1],MP_BONUS,MT_BONUS);

    StartBonusFlash ();

    check->shapenum = shapenum;   // remove from list if shapenum == -1
    check->itemnumber = bo_nothing;
}


/*
===================
=
= WriteTokenString
=
===================
*/

void WriteTokenString (char *string)
{
    char   buffer[3];
    size_t len;

    len = strlen(string) - 2;

    if (gamestate.tokens > 9)
        itoa (gamestate.tokens,buffer,10);
    else
    {
        buffer[0] = '0';
        itoa (gamestate.tokens,buffer + 1,10);
    }

    snprintf (&string[len],len,buffer);
}


/*
===================
=
= TryMove
=
= Returns true if move ok
=
===================
*/

bool TryMove (objtype *obj, unsigned size)
{
    int     xl,yl,xh,yh,x,y;
    objtype *check;
    fixed   deltax,deltay;

    xl = (obj->x - size) >> TILESHIFT;
    yl = (obj->y - size) >> TILESHIFT;
    xh = (obj->x + size) >> TILESHIFT;
    yh = (obj->y + size) >> TILESHIFT;

//
// check for solid walls
//
    for (y = yl; y <= yh; y++)
    {
        for (x = xl; x <= xh; x++)
        {
            check = actorat[x][y];

            if (check)
            {
                if (!ISPOINTER(check) || (check->flags & FL_FAKE_STATIC))
                    return false;
            }
        }
    }

//
// check for actors
//
// TODO: ???
//
    yl -= 2;
    yh += 2;
    xl -= 2;
    xh += 2;

    for (y = yl; y <= yh; y++)
    {
        for (x = xl; x <= xh; x++)
        {
            check = actorat[x & (mapwidth - 1)][y & (mapheight - 1)];

            if (ISPOINTER(check))
            {
                if (check != player && (check->flags & (FL_SOLID | FL_FAKE_STATIC)) == FL_SOLID)
                {
                    deltax = labs(obj->x - check->x);
                    deltay = labs(obj->y - check->y);

                    if (deltax <= MINACTORDIST && deltay <= MINACTORDIST)
                        return false;
                }
            }
        }
    }

    return true;
}


/*
===================
=
= ClipMove
=
= Returns true if object hit a wall
=
===================
*/

bool ClipMove (objtype *obj, fixed xmove, fixed ymove)
{
    fixed basex,basey;

    basex = obj->x;
    basey = obj->y;

    obj->x = basex + xmove;
    obj->y = basey + ymove;

    if (TryMove(obj,PLAYERSIZE))
        return false;

#ifdef IN_DEVELOPMENT
    if (!(gamestate.flags & GS_CLIP_WALLS) && obj == player)
        return true;
#endif
    if (!SD_SoundPlaying())
        SD_PlaySound (HITWALLSND);

    obj->x = basex + xmove;
    obj->y = basey;

    if (TryMove(obj,PLAYERSIZE))
        return true;

    obj->x = basex;
    obj->y = basey + ymove;

    if (TryMove(obj,PLAYERSIZE))
        return true;

    obj->x = basex;
    obj->y = basey;

    return true;
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
    objtype  dumb;
    fixed    xmove,ymove;
    word     *map[2];
    unsigned offset;
    int      dx,dy;
    int      dangle;
    bool     ignore_map1;

    thrustspeed += speed;

//
// moving bounds speed
//
    if (speed >= MINDIST * 2)
        speed = (MINDIST * 2) - 1;

    xmove = FixedMul(speed,costable[angle]);
    ymove = -FixedMul(speed,sintable[angle]);

    ClipMove (player,xmove,ymove);

    player_oldtilex = player->tilex;
    player_oldtiley = player->tiley;
    player->tilex = player->x >> TILESHIFT;
    player->tiley = player->y >> TILESHIFT;
    player->areanumber = GetAreaNumber(player->tilex,player->tiley);

    areabyplayer[player->areanumber] = true;
    TravelTable[player->tilex][player->tiley] |= TT_TRAVELED;

    map[0] = &MAPSPOT(player->tilex,player->tiley,0);
    map[1] = &MAPSPOT(player->tilex,player->tiley,1);

//
// check for trigger tiles
//
    switch (*map[0])
    {
        case DOORTRIGGERTILE:
            dx = *map[1] >> 8;
            dy = *map[1] & 0xff;

            if (OperateSmartSwitch(dx,dy,ST_TOGGLE,false))      // operate & check for removeal
                *map[0] = AREATILE + player->areanumber;        // remove switch

            ignore_map1 = true;
            break;

        case SMART_OFF_TRIGGER:
        case SMART_ON_TRIGGER:
            dx = *map[1] >> 8;
            dy = *map[1] & 0xff;
            OperateSmartSwitch (dx,dy,*map[0] - SMART_OFF_TRIGGER,false);
            ignore_map1 = true;
            break;

        case WINTIGGERTILE:
            playstate = ex_victorious;
            dumb.x = ((fixed)gamestate.wintilex << TILESHIFT) + TILECENTER;
            dumb.y = ((fixed)gamestate.wintiley << TILESHIFT) + TILECENTER;
            dumb.flags = 0;
            dangle = CalcAngle(player,&dumb);
            RotateView (dangle,2);
            ignore_map1 = true;
            break;

        default:
            ignore_map1 = false;
            break;
    }

    if (!ignore_map1)
    {
        //
        // change sky and ground color on-the-fly.
        //
        offset = *(map[1] + 1);

        switch (*map[1])
        {
#ifdef CEILING_FLOOR_COLORS
            case 0xfe00:
                ceilingcolor = offset >> 8;
                floorcolor = offset & 0xff;
                break;
#endif

#if 0
            case 0xf600:
                normalshadediv = offset >> 8;

                if (normalshadediv > 12)
                    Quit ("Shade divisor too large! Must be 1-12!");

                shademax = offset & 0xff;

                if (shademax < 5 || shademax > SHADEMAX)
                    Quit ("Shade max value out of range! Must be 5-63!");

                normalshade = (((viewwidth * 9) >> 4) - 3) / normalshadediv;
                break;
#endif
        }
    }
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
=============================================================================

                            ACTIONS

=============================================================================
*/


/*
===============
=
= Cmd_Fire
=
===============
*/

void Cmd_Fire (void)
{
	if (noShots)
		return;

    if (gamestate.weapon == wp_autocharge && gamestate.weapon_wait)
        return;

    buttonheld[bt_attack] = true;

    player->state = &s_attack;

    gamestate.attackframe = 0;
    gamestate.attackcount = attackinfo[gamestate.weapon][gamestate.attackframe].tics;
    gamestate.weaponframe = attackinfo[gamestate.weapon][gamestate.attackframe].frame;
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
    objtype     *obj,*intobj;
    int         x,y;
    int         checkx,checky,doornum,dir;
    int         new_floor,angle;
    fixed       dx,dy,dist,intdist;
    unsigned    iconnum;
    static byte interrogate_delay = 0;
    bool        tryDetonator = false;

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
    iconnum = MAPSPOT(checkx,checky,1);

    //
    // TODO: not certain if the scopes are correct b/c the STUPID lack of braces made it a mess!
    //
    if (iconnum == PUSHABLETILE)
        PushWall (checkx,checky,dir);
    else if (!buttonheld[bt_use])
    {
        //
        // test for doors / elevator
        //
        if ((doornum & 0x80) && (pwallx != checkx || pwally != checky))
        {
            buttonheld[bt_use] = true;
            OperateDoor (doornum & ~0x80);
        }
        else
        {
            //
            // test for special tile types
            //
            switch (doornum & 63)
            {
                //
                // test for 'display elevator buttons'
                //
                case TRANSPORTERTILE:
                    new_floor = InputFloor();

                    if (new_floor != -1 && new_floor != gamestate.mapon)
                    {
                        angle = player->angle;

                        gamestuff.level[gamestate.mapon].ptilex = player->tilex;
                        gamestuff.level[gamestate.mapon].ptiley = player->tiley;

                        angle = player->angle - ANG180;

                        if (angle < 0)
                            angle += ANG360;

                        gamestuff.level[gamestate.mapon].pangle = angle;

                        playstate = ex_transported;

                        gamestate.lastmapon = gamestate.mapon;
                        gamestate.mapon = new_floor - 1;
                    }
                    else
                        DrawPlayScreen (false);
                    break;

                case DIRECTTRANSPORTTILE:
                    switch (iconnum & 0xff00)
                    {
                        case 0xf400:
                            playstate = ex_transported;
                            gamestate.lastmapon = gamestate.mapon;
                            gamestate.mapon = (iconnum & 0xff) - 1;
                            break;

                        default:
                            //
                            // stay in current level warp to new location
                            //
                            playstate = ex_transported;
                            Warped ();
                            playstate = ex_stillplaying;

                            player->tilex = iconnum >> 8;
                            player->tiley = iconnum & 0xff;
                            player->x = ((fixed)player->tilex << TILESHIFT) + TILECENTER;
                            player->y = ((fixed)player->tiley << TILESHIFT) + TILECENTER;

                            DrawWarpIn ();
                            break;
                    }
                    break;

                //
                // test for wall switch activation
                //
                case OFF_SWITCH:
                case ON_SWITCH:
                    ActivateWallSwitch (iconnum,checkx,checky);
                    break;

                //
                // test for concession machines
                //
                case FOODTILE:
                case SODATILE:
                    OperateConcession ((uintptr_t)actorat[checkx][checky]);
                    break;

                default:
                    tryDetonator = true;
                    break;
            }
        }
    }
    else if (!interrogate_delay)
    {
        intobj = NULL;
        intdist = INTERROGATEDIST + 1;

        for (y = -MDIST; y < MDIST + 1; y++)
        {
            for (x = -MDIST; x < MDIST + 1; x++)
            {
                obj = actorat[player->tilex + x][player->tiley + y];

                if (tilemap[player->tilex + x][player->tiley + y] || !ISPOINTER(obj))
                    continue;

                if (obj->obclass != gen_scientistobj)
                    continue;

                dx = labs(player->x - obj->x);
                dy = labs(player->y - obj->y);
                dist = dx < dy ? dx : dy;

                if (dist >= intdist)
                    continue;

                if ((obj->flags & (FL_FRIENDLY | FL_VISIBLE)) == (FL_FRIENDLY | FL_VISIBLE))  // TODO no need?
                {
                    angle = abs(player->angle - CalcAngle(player,obj));

                    if (angle > INTG_ANGLE / 2)
                        continue;

                    intobj = obj;
                    intdist = dist;
                }
            }
        }

        if (intobj)
        {
            if (Interrogate(intobj))
                interrogate_delay = 20;     // informants have 1/3 sec delay
            else
                interrogate_delay = 120;    // non-informants have 2 sec delay
        }
        else
            tryDetonator = true;
    }
    else
    {
        if (tics < interrogate_delay)
            interrogate_delay -= tics;
        else
            interrogate_delay = 0;

        tryDetonator = true;
    }

    if (tryDetonator)
    {
        if (!tryDetonatorDelay && gamestate.plasma_detonators)
        {
            TryDropPlasmaDetonator ();

            tryDetonatorDelay = 60;
        }
    }
    else
        tryDetonatorDelay = 60;

    if (!buttonheld[bt_use])
        interrogate_delay = 0;
}


/*
============================================================================

                             INTERROGATE CODE

============================================================================
*/


/*
===============
=
= Interrogate
=
===============
*/

bool Interrogate (objtype *obj)
{
    int            i;
    bool           rt_value = true;
    sci_mCacheInfo *ci;
    scientist_t    *st;
    char           msgbuffer[MSG_BUFFER_LEN + 1];
    const char     *msgptr = NULL;

    msgbuffer[0] = '\0';

    if (obj->flags & FL_INFORMANT)
    {
        snprintf (msgbuffer,sizeof(msgbuffer),"%s%s",int_interrogate,int_informant);

        if (obj->flags & FL_INTERROGATED)
        {
            if ((obj->flags & FL_HAS_AMMO) && gamestate.ammo != MAX_AMMO)
            {
                GiveAmmo ((US_RndT() % 8) + 1);
                obj->flags &= ~FL_HAS_AMMO;    // TODO: why not just use obj->ammo member
                msgptr = int_haveammo;
            }
            else if ((obj->flags & FL_HAS_TOKENS) && gamestate.tokens != MAX_TOKENS)
            {
                GiveToken (5);
                obj->flags &= ~FL_HAS_TOKENS;
                msgptr = int_havetoken;
            }
        }

        if (!msgptr)
        {
            //
            // if new areanumber OR no 'area msgs' have been compiled, compile
            // a list of all special messages for this areanumber
            //
            if (LastInfArea == 0xff || LastInfArea != obj->areanumber)
            {
                NumAreaMsgs = 0;

                for (i = 0; i < InfHintList.NumMsgs; i++)
                {
                    ci = &InfHintList.smInfo[i];

                    if (ci->areanumber == 0xff)
                        break;

                    if (ci->areanumber == obj->areanumber)
                        InfAreaMsgs[NumAreaMsgs++] = InfHintList.smInfo[ci->mInfo.local_val].mInfo.mSeg;
                }

                LastInfArea = obj->areanumber;
            }

            //
            // randomly select an informant hint, either specific to areanumber
            // or general hint
            //
            if (NumAreaMsgs)
            {
                if (obj->ammo != obj->areanumber)
                    obj->s_tilex = 0xff;

                obj->ammo = obj->areanumber;

                if (obj->s_tilex == 0xff)
                    obj->s_tilex = Random(NumAreaMsgs);

                msgptr = InfAreaMsgs[obj->s_tilex];
            }
            else
            {
                if (obj->s_tiley == 0xff)
                    obj->s_tiley = FirstGenInfMsg + Random(TotalGenInfMsgs);

                msgptr = InfHintList.smInfo[obj->s_tiley].mInfo.mSeg;
            }

            //
            // still no msgptr, this is a shared message
            //
            if (!msgptr)
                msgptr = InfHintList.smInfo[InfHintList.smInfo[obj->s_tiley].mInfo.local_val].mInfo.mSeg;

            obj->flags |= FL_INTERROGATED;
        }
    }
    else
    {
        snprintf (msgbuffer,sizeof(msgbuffer),int_interrogate);

        rt_value = false;

        if ((obj->flags & FL_MUST_ATTACK) || (US_RndT() & 1))
        {
            //
            // mean
            //
            obj->flags &= ~FL_FRIENDLY;     // make him attack!
            obj->flags |= FL_INTERROGATED;
            st = &MeanSciList;
        }
        else
        {
            //
            // nice
            //
            obj->flags |= FL_MUST_ATTACK;   // make him mean!
            st = &NiceSciList;
        }

        msgptr = st->smInfo[Random(st->NumMsgs)].mInfo.mSeg;
    }

    if (msgptr)
    {
        snprintf (msg,sizeof(msg),"%s%s%s%s",msgbuffer,int_rr,msgptr,int_xx);

        DisplayInfoMsg (msg,MP_INTERROGATE,DISPLAY_MSG_STD_TIME * 2,MT_GENERAL);
        SD_PlaySound (INTERROGATESND);
    }

    return rt_value;
}


/*
============================================================================

                              ELEVATOR CODE

============================================================================
*/


/*
===============
=
= InputFloor
=
===============
*/

int InputFloor (void)
{
    #define RADAR_FLAGS     OV_KEYS
    #define MAX_TELEPORTS   20
    #define MAX_MOVE_DELAY  10

    int     i;
    int     buttonPic,buttonY;
    int     rt_code,tpNum,lastTpNum;
    int     teleX[MAX_TELEPORTS] = {16,40,86,23,44,62,83,27,118,161,161,161,213,213,184,205,226,256,276,276};
    int     teleY[MAX_TELEPORTS] = {13,26, 9,50,50,50,50,62, 42, 17, 26, 35, 41, 50, 62, 62, 62, 10, 10, 30};
    int     moveActive;
    objtype old_player;
    bool    locked,buttonsDrawn;

    rt_code = -2;
    tpNum = gamestate.mapon;
    lastTpNum = tpNum;
    moveActive = 0;
    locked = buttonsDrawn = false;

    SD_StopDigitized ();
    VW_FadeOut ();
    ClearMenuBorders ();

    VW_DrawPic (0,0,TELEPORTBACKTOPPIC);
    VW_DrawPic (0,12 * 8,TELEPORTBACKBOTPIC);
    DisplayTeleportName (tpNum,locked);
    VW_DrawMPic (teleX[tpNum],teleY[tpNum],TELEPORT1ONPIC + tpNum);

    memcpy (&old_player,player,sizeof(old_player));
    player->angle = ANG90;
    player->x = player->y = ((fixed)(MAPSIZE / 2) << TILESHIFT) + TILECENTER;

    ov_buffer = SafeMalloc(mapwidth * mapheight);
    ShowStats (0,0,ss_justcalc,&gamestuff.level[gamestate.mapon].stats);
    memcpy (&ov_stats,&gamestuff.level[gamestate.mapon].stats,sizeof(ov_stats));

    vbuf = VW_LockSurface(screen.buffer);

    if (!vbuf)
        Quit ("Unable to lock surface: %s\n",SDL_GetError());

    vbuf += screen.bufferofs;

    ShowOverhead (TOV_X,TOV_Y,32,0,RADAR_FLAGS);

    VW_UnlockSurface (screen.buffer);
    vbuf = NULL;

    SaveOverheadChunk (tpNum);

    px = 115;
    py = 188;
    fontcolor = 0xaf;
    fontnumber = 2;
    ShPrint (if_help,0,false);

    controlx = controly = 0;
    IN_ClearKeysDown ();

    while (rt_code == -2)
    {
        //
        // handle ABORT and ACCEPT
        //
        //if (!(screen.flags & SC_FADED))
        //    PollControls ();

        IN_ProcessEvents ();
        CalcTics ();

        if (Keyboard[sc_LeftArrow])
            controlx = -1;
        else if (Keyboard[sc_RightArrow])
            controlx = 1;
        else
            controlx = 0;

        if (Keyboard[sc_UpArrow])
            controly = -1;
        else if (Keyboard[sc_DownArrow])
            controly = 1;
        else
            controly = 0;

        if (Keyboard[sc_Escape] || buttonstate[bt_strafe])
        {
            rt_code = -1;   // ABORT

            LoadLocationText (gamestate.mapon + (MAPS_PER_EPISODE * gamestate.episode));
            break;
        }
        else if (Keyboard[sc_Enter] || buttonstate[bt_attack])
        {
            if (locked)
            {
                if (!SD_SoundPlaying())
                    SD_PlaySound (NOWAYSND);
            }
            else
            {
                rt_code = tpNum;    // ACCEPT

                //
                // flash selection
                //
                for (i = 0; i < 10; i++)
                {
                    VW_DrawMPic (teleX[tpNum],teleY[tpNum],TELEPORT1OFFPIC + tpNum);
                    VW_UpdateScreen (screen.buffer);
                    VW_WaitVBL (4);

                    VW_DrawMPic (teleX[tpNum],teleY[tpNum],TELEPORT1ONPIC + tpNum);
                    VW_UpdateScreen (screen.buffer);
                    VW_WaitVBL (4);
                }

                break;
            }
        }

        CheckMusicToggle ();

        //
        // handle delay
        //
        if (moveActive)
        {
            moveActive -= tics;

            if (moveActive < 0)
                moveActive = 0;
        }

        //
        // move to NEXT / PREV teleport?
        //
        buttonY = 0;

        if (controlx > 0 || controly > 0)
        {
            if (!moveActive && tpNum < MAX_TELEPORTS - 1)
            {
                tpNum++;    // MOVE NEXT

                moveActive = MAX_MOVE_DELAY;
            }

            buttonPic = TELEDNONPIC;
            buttonY = 104;
        }
        else if (controlx < 0 || controly < 0)
        {
            if (!moveActive && tpNum)
            {
                tpNum--;    // MOVE PREV

                moveActive = MAX_MOVE_DELAY;
            }

            buttonPic = TELEUPONPIC;
            buttonY = 91;
        }

        if (buttonY)
        {
            //
            // light buttons
            //
            VW_DrawMPic (34,91,TELEUPOFFPIC);
            VW_DrawMPic (270,91,TELEUPOFFPIC);
            VW_DrawMPic (34,104,TELEDNOFFPIC);
            VW_DrawMPic (270,104,TELEDNOFFPIC);

            VW_DrawMPic (34,buttonY,buttonPic);
            VW_DrawMPic (270,buttonY,buttonPic);

            buttonsDrawn = true;
        }
        else if (buttonsDrawn)
        {
            //
            // unlight buttons
            //
            VW_DrawMPic (34,91,TELEUPOFFPIC);
            VW_DrawMPic (270,91,TELEUPOFFPIC);
            VW_DrawMPic (34,104,TELEDNOFFPIC);
            VW_DrawMPic (270,104,TELEDNOFFPIC);

            buttonsDrawn = false;
        }

        //
        // change visual information
        //
        if (tpNum != lastTpNum)
        {
            locked = gamestuff.level[tpNum].locked;
            DisplayTeleportName (tpNum,locked);

            VW_DrawMPic (teleX[lastTpNum],teleY[lastTpNum],TELEPORT1OFFPIC + lastTpNum);
            VW_DrawMPic (teleX[tpNum],teleY[tpNum],TELEPORT1ONPIC + tpNum);

            LoadOverheadChunk (tpNum);
            ShowOverheadChunk ();

            if (ov_noImage)
            {
                fontcolor = 0x57;
                WindowX = WindowW = TOV_X;
                WindowY = WindowH = TOV_Y;
                WindowW += 63;
                WindowH += 63;
                PrintX = TOV_X + 5;
                PrintY = TOV_Y + 13;
                US_Print (if_noImage);
            }

            lastTpNum = tpNum;
        }

        if (locked)
        {
            vbuf = VW_LockSurface(screen.buffer);

            if (!vbuf)
                Quit ("Unable to lock surface: %s\n",SDL_GetError());

            vbuf += screen.bufferofs;

            ShowOverhead (TOV_X,TOV_Y,32,-1,RADAR_FLAGS);

            VW_UnlockSurface (screen.buffer);
            vbuf = NULL;
        }

        CycleColors ();

        if (screen.flags & SC_FADED)
        {
            VW_FadeIn ();
            ShowStats (235,138,ss_normal,&ov_stats);
            IN_ClearKeysDown ();
            controlx = controly = 0;
        }
        else
            VW_UpdateScreen (screen.buffer);
    }

    VW_FadeOut ();
    VW_SetBufferOffset (0);    // back to 3D renderer

    free (ov_buffer);
    ov_buffer = NULL;

    memcpy (player,&old_player,sizeof(old_player));

    DrawPlayScreen (false);
    IN_ClearKeysDown ();

    return rt_code;
}


/*
===============
=
= ShowOverheadChunk
=
===============
*/

void ShowOverheadChunk (void)
{
    VW_MemToScreen (ov_buffer,mapwidth,mapheight,TOV_X,TOV_Y);
    ShowStats (235,138,ss_quick,&ov_stats);
}


/*
===============
=
= LoadOverheadChunk
=
===============
*/

void LoadOverheadChunk (int tpNum)
{
    FILE    *file;
    int32_t buffersize;
    char    chunk[5];

    //
    // open PLAYTEMP file
    //
    MakeDestPath (PLAYTEMP_FILE);

    file = fopen(tempPath,"rb+");

    if (!file)
        CA_CannotOpen (tempPath);

    buffersize = mapwidth * mapheight;

    //
    // find and load chunk
    //
    snprintf (chunk,sizeof(chunk),"OV%02x",tpNum);

    if (FindChunk(file,chunk))
    {
        ov_noImage = false;

        if (!fread(ov_buffer,buffersize,1,file))
            Quit ("Error reading file %s: %s",tempPath,strerror(errno));

        if (!fread(&ov_stats,sizeof(ov_stats),1,file))
            Quit ("Error reading file %s: %s",tempPath,strerror(errno));
    }
    else
    {
        ov_noImage = true;

        memset (ov_buffer,0x52,buffersize);
        memset (&ov_stats,0,sizeof(ov_stats));
    }

    fclose (file);
}


/*
===============
=
= SaveOverheadChunk
=
===============
*/

void SaveOverheadChunk (int tpNum)
{
    FILE    *file;
    int     x,y;
    int32_t buffersize;
    int32_t cksize;
    byte    *source,*dest;
    char    chunk[5];

    //
    // open PLAYTEMP file
    //
    MakeDestPath (PLAYTEMP_FILE);

    file = fopen(tempPath,"rb+");

    if (!file)
        CA_CannotOpen (tempPath);

    //
    // remove level chunk from file
    //
    snprintf (chunk,sizeof(chunk),"OV%02x",tpNum);
    DeleteChunk (file,chunk);

    //
    // fill buffer with un-scaled image
    //
    source = VW_LockSurface(screen.buffer);

    if (!source)
        Quit ("Unable to lock surface: %S\n",SDL_GetError());

    source += screen.bufferofs + ylookup[TOV_Y * screen.scale] + (TOV_X * screen.scale);

    for (y = 0; y < mapheight; y++)
    {
        dest = ov_buffer + (y << MAPSHIFT);

        for (x = 0; x < mapwidth; x++)
            *dest++ = source[ylookup[y * screen.scale] + (x * screen.scale)];
    }

    buffersize = mapwidth * mapheight;
    cksize = buffersize + sizeof(statsInfoType);

    //
    // write chunk ID, SIZE, and IMAGE
    //
    if (!fwrite(chunk,sizeof(chunk) - 1,1,file))
        Quit ("Error writing file %s: %s",tempPath,strerror(errno));

    if (!fwrite(&cksize,sizeof(cksize),1,file))
        Quit ("Error writing file %s: %s",tempPath,strerror(errno));

    if (!fwrite(ov_buffer,buffersize,1,file))
        Quit ("Error writing file %s: %s",tempPath,strerror(errno));

    if (!fwrite(&ov_stats,sizeof(ov_stats),1,file))
        Quit ("Error writing file %s: %s",tempPath,strerror(errno));

    fclose (file);
}


/*
===============
=
= DisplayTeleportName
=
===============
*/

void DisplayTeleportName (int tpNum, bool locked)
{
    const char *disabled = "-- TELEPORT DISABLED --";
    const char *s;
    word       w,h;

    if (locked)
    {
        fontcolor = 0xf5;
        s = disabled;
    }
    else
    {
        fontcolor = 0x57;
        LoadLocationText (tpNum);
        s = LocationText;
    }

    VW_MeasureString (s,&w,&h);

    px = (screen.basewidth / 2) - (w / 2);
    py = 103;

    VW_Bar  (54,101,212,9,0x52);
    ShPrint (s,0,false);
}


/*
=============================================================================

                        MISSION STATISTICS CODE

=============================================================================
*/


/*
===============
=
= ShowStats
=
===============
*/

int ShowStats (int bx, int by, int type, statsInfoType *stats)
{
    int i;
    int floor,total,mission;
    int p1,p2,p3,maxPerFloor;

    total = mission = 0;

    //
    // get max points per floor
    //
    if (stats->total_points || stats->total_inf || stats->total_enemy)
        maxPerFloor = 300;
    else
        maxPerFloor = 0;

    //
    // setup to test for bypassing stats
    //
    LastScan = 0;

    if (type == ss_quick)
        show_stats_quick = true;
    else
        show_stats_quick = false;

    //
    // show ratio for each statistic:
    //
    // TOTAL POINTS, INFORMANTS ALIVE, ENEMY DESTROYED,
    // OVERALL FLOOR, OVERALL MISSION
    //
    p1 = ShowRatio(bx,by,bx + 52,by,stats->total_points,stats->accum_points,type);

    by += 7;
    p2 = ShowRatio(bx,by,bx + 52,by,stats->total_inf,stats->accum_inf,type);

    by += 7;
    p3 = ShowRatio(bx,by,bx + 52,by,stats->total_enemy,stats->accum_enemy,type);

    by += 13;
    floor = p1 + p2 + p3;
    ShowRatio (bx,by,bx + 52,by,maxPerFloor,floor,type);

    by += 7;
    stats->overall_floor = floor;

    for (i = 0; i < MAPS_WITH_STATS; i++)
    {
        total += 300;
        mission += gamestuff.level[i].stats.overall_floor;
    }

    mission = ShowRatio(bx,by,bx + 52,by,total,mission,type);

    if (show_stats_quick)
        VW_UpdateScreen (screen.buffer);

    return mission;
}


/*
===============
=
= ShowRatio
=
===============
*/

int ShowRatio (int bx, int by, int nx, int ny, int32_t total, int32_t perc, int type)
{
    int i;
    int numbars,maxperc;
    int percentage;

    percentage = 1;

    if (total)
    {
        maxperc = LRATIO(100,total,perc,10);
        numbars = LRATIO(48,100,maxperc,10);
    }
    else
    {
        if (type != ss_justcalc)
        {
            fontcolor = 0x57;
            VW_Bar (bx,by,BAR_W,BAR_H,0);
            VW_Bar (nx,ny,PERC_W + 6,PERC_H,0);
            PrintX = nx;
            PrintY = ny;
            US_Print ("N/A");
        }

        return 100;
    }

    if (type == ss_justcalc)
        return maxperc;

    PrintY = ny;
    fontcolor = 0xaf;
    fontnumber = 2;

    VW_Bar (bx,by,BAR_W,BAR_H,0x07);
    PrintStatPercent (nx,ny,0);

    for (i = 0; i < numbars; i++)
    {
        if (LastScan)
            show_stats_quick = true;

        VW_Bar (bx + i,by,1,BAR_H,0xc8);

        //
        // keep up with current percentage
        //
        if (i == numbars - 1)
            percentage = maxperc;
        else
            percentage += 2;

        PrintStatPercent (nx,ny,percentage);

        if (!show_stats_quick)
        {
            if (!(i % 2))
                SD_PlaySound (STATS1SND);

            VW_WaitVBL (1);
            VW_UpdateScreen (screen.buffer);
        }
    }

    if (!show_stats_quick && numbars)
    {
        SD_PlaySound (STATS2SND);

        //
        // TODO: check this loop
        //
        while (SD_SoundPlaying() && !LastScan)
        {
            IN_ProcessEvents ();
            SDL_Delay (3);
        }
    }

    return maxperc;
}


/*
===============
=
= PrintStatPercent
=
===============
*/

void PrintStatPercent (int nx, int ny, int percentage)
{
    if (percentage < 10)
        PrintX = nx + 9;
    else if (percentage < 100)
        PrintX = nx + 4;
    else
        PrintX = nx - 1;

    VW_Bar (nx,ny,PERC_W + 5,PERC_H,0);
    US_PrintSigned (percentage);
    US_Print ("%");
}


/*
===============
=
= PerfectStats
=
===============
*/

bool PerfectStats (void)
{
    if (gamestuff.level[gamestate.mapon].stats.total_points == gamestuff.level[gamestate.mapon].stats.accum_points
     && gamestuff.level[gamestate.mapon].stats.total_inf == gamestuff.level[gamestate.mapon].stats.accum_inf
     && gamestuff.level[gamestate.mapon].stats.total_enemy == gamestuff.level[gamestate.mapon].stats.accum_enemy)
        return true;

    return false;
}


/*
=============================================================================

                        PINBALL BONUS DISPLAY CODE

=============================================================================
*/


/*
===============
=
= B_GAliFunc
=
===============
*/

void B_GAliFunc (void)
{
    if (gamestate.episode == 5)
        DisplayInfoMsg (B_GAlienDead2,MP_PINBALL_BONUS,7 * 60,MT_BONUS);
}


/*
===============
=
= B_EManFunc
=
===============
*/

void B_EManFunc (void)
{
    SD_PlaySound (EXTRA_MANSND);
    fontnumber = 2;

    VW_DrawPic (0,0,TOP_STATUSBARPIC);
    ShadowPrintLocationText (sp_normal);
}


/*
===============
=
= B_MillFunc
=
===============
*/

void B_MillFunc (void)
{
    GiveAmmo (99);
    HealSelf (99);
}


/*
===============
=
= B_RollFunc
=
===============
*/

void B_RollFunc (void)
{
    B_MillFunc ();

    gamestate.score_roll_wait = SCORE_ROLL_WAIT;
}


/*
===============
=
= DisplayPinballBonus
=
===============
*/

void DisplayPinballBonus (void)
{
    int i;

    //
    // check queue for bonuses
    //
    for (i = 0; i < sizeof(gamestuff.level[0].bonus_queue) * 8; i++)
    {
        if ((BONUS_QUEUE & (1 << i)) && LastMsgPri < MP_PINBALL_BONUS)
        {
            //
            // start this bonus!
            //
            SD_PlaySound (ROLL_SCORESND);
            DisplayInfoMsg (PinballBonus[i].BonusText,MP_PINBALL_BONUS,7 * 60,MT_BONUS);

            if (!PinballBonus[i].Recurring)
                BONUS_SHOWN |= (1 << i);

            BONUS_QUEUE &= ~(1 << i);

            GivePoints (PinballBonus[i].Points,false);

            if (PinballBonus[i].func)
                PinballBonus[i].func();
        }
    }
}


/*
===============
=
= CheckPinballBonus
=
===============
*/

void CheckPinballBonus (int32_t points)
{
    int32_t oldscore,newscore;

    oldscore = gamestate.score;
    newscore = gamestate.score + points;

    //
    // check SCORE ROLLED bonus
    //
    if (oldscore <= MAX_DISPLAY_SCORE && newscore > MAX_DISPLAY_SCORE)
        ActivatePinballBonus (B_SCORE_ROLLED);

    //
    // check ONE MILLION bonus
    //
    if (oldscore < 500000l && newscore >= 500000l)
        ActivatePinballBonus (B_ONE_MILLION);

    //
    // check EXTRA MAN bonus
    //
    if (newscore >= gamestate.nextextra)
    {
        gamestate.nextextra += EXTRAPOINTS;

        if (gamestate.lives < MAX_EXTRA_LIVES)
        {
            gamestate.lives++;

            ActivatePinballBonus (B_EXTRA_MAN);
        }
    }

    //
    // check mission stats bonuses
    //
    if (gamestuff.level[gamestate.mapon].stats.total_enemy == gamestuff.level[gamestate.mapon].stats.accum_enemy)
        ActivatePinballBonus (B_ENEMY_DESTROYED);

    if (gamestuff.level[gamestate.mapon].stats.total_points == gamestuff.level[gamestate.mapon].stats.accum_points)
        ActivatePinballBonus (B_TOTAL_POINTS);

    if (gamestuff.level[gamestate.mapon].stats.total_inf == gamestuff.level[gamestate.mapon].stats.accum_inf
     && gamestuff.level[gamestate.mapon].stats.total_inf
     && (BONUS_SHOWN & (B_TOTAL_POINTS | B_ENEMY_DESTROYED)) == (B_TOTAL_POINTS | B_ENEMY_DESTROYED))
        ActivatePinballBonus (B_INFORMANTS_ALIVE);

    if (BONUS_QUEUE)
        DisplayPinballBonus ();
}


/*
=============================================================================

                    PLAYER CONTROL

=============================================================================
*/

void SpawnPlayer (int tilex, int tiley, int dir)
{
    if (gamestuff.level[gamestate.mapon].ptilex && gamestuff.level[gamestate.mapon].ptiley)
    {
        tilex = gamestuff.level[gamestate.mapon].ptilex;
        tiley = gamestuff.level[gamestate.mapon].ptiley;
        dir = 1 + (gamestuff.level[gamestate.mapon].pangle / ANG90);
    }

    player->obclass = playerobj;
    player->active = ac_yes;
    player->tilex = tilex;
    player->tiley = tiley;
    player->areanumber = GetAreaNumber(player->tilex,player->tiley);
    player->x = ((fixed)tilex << TILESHIFT) + TILECENTER;
    player->y = ((fixed)tiley << TILESHIFT) + TILECENTER;
    player->state = &s_player;
    player->flags = FL_NEVERMARK;
    player->angle = (1 - dir) * ANG90;

    if (player->angle < 0)
        player->angle += ANG360;

    Thrust (0,0);    // set some variables
    InitAreas ();
    InitWeaponBounce ();
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
    int     damage;
    int     dx,dy,dist;
    int16_t viewx,viewheight;
    int32_t viewdist;
    fixed   transx;
    bool    skip = false;

    if (gamestate.weapon != wp_autocharge)
        MakeAlertNoise (obj);

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
                TransformActor (check->x,check->y,&viewx,&viewheight,&transx);

                if (abs(viewx - centerx) < shootdelta && transx < viewdist && viewheight)
                {
                    if ((skip && check->obclass == hang_terrotobj) || (check->flags2 & FL2_NOTGUNSHOOTABLE))
                        continue;

                    viewdist = transx;
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

    if (tryDetonatorDelay > tics)
        tryDetonatorDelay -= tics;
    else
        tryDetonatorDelay = 0;

    if (buttonstate[bt_use])
    {
        Cmd_Use ();
        SD_PlaySound (HITWALLSND);
    }

    if (buttonstate[bt_attack] && !buttonheld[bt_attack])
        Cmd_Fire ();

    ControlMovement ();
    HandleWeaponBounce ();

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

    if (noShots)
    {
        obj->state = &s_player;
        gamestate.attackframe = gamestate.weaponframe = 0;
        return;
    }

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
                    SpawnProjectile (obj,grenadeobj);
                    MakeAlertNoise (obj);
                }
                break;

            case 7:
                TryDropPlasmaDetonator ();
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
                    SpawnProjectile (obj,bfg_shotobj);
                    MakeAlertNoise (obj);
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


#if 0
/*
===============
=
= RunBlakeRun
=
= TODO: AOG
=
===============
*/

void RunBlakeRun (void)
{
    #define BLAKE_SPEED (MOVESCALE * 50)

    fixed   xmove,ymove;
    int32_t speed;
    int     startx,starty,dx,dy;
    objtype *blake;

    blake = SpawnPatrol(en_blake,player->tilex,player->tiley,player->dir >> 1);

    //
    // Blake object starts one tile behind player object
    //
    switch (blake->dir)
    {
        case north:
            blake->tiley += 2;
            break;

        case south:
            blake->tiley -= 2;
            break;

        case east:
            blake->tilex -= 2;
            break;

        case west:
            blake->tilex += 2;
            break;
    }

    blake->x = ((fixed)blake->tilex << TILESHIFT) + TILECENTER;
    blake->y = ((fixed)blake->tiley << TILESHIFT) + TILECENTER;
    startx = blake->tilex = blake->x >> TILESHIFT;
    starty = blake->tiley = blake->y >> TILESHIFT;

    //
    // Run, Blake, Run!
    //
    do
    {
        xmove = FixedMul(BLAKE_SPEED,viewcos);
        ymove = -FixedMul(BLAKE_SPEED,viewsin);

        if (ClipMove(blake,xmove,ymove))
            break;

        DoActor (blake);
        ThreeDRefresh ();

        blake->tilex = blake->x >> TILESHIFT;
        blake->tiley = blake->y >> TILESHIFT;

        dx = abs(blake->tilex - startx);
        dy = abs(blake->tiley - starty);

    } while (dx < 6 && dy < 6);
}
#endif

/*
===============
=
= SW_HandleActor
=
= Handle all actors connected to a smart switch
=
===============
*/

void SW_HandleActor (objtype *obj)
{
    if (obj->active == ac_no)
        obj->active = ac_yes;

    switch (obj->obclass)
    {
        case rentacopobj:
        case gen_scientistobj:
        case swatobj:
        case goldsternobj:
        case proguardobj:
            if (!(obj->flags & (FL_ATTACKMODE | FL_FIRSTATTACK)))
                FirstSighting (obj);
            break;

        case morphing_spider_mutantobj:
        case morphing_reptilian_warriorobj:
        case morphing_mutanthuman2obj:
        case crate1obj:
        case crate2obj:
        case crate3obj:
        case podeggobj:
            KillActor (obj);
            break;

        case floatingbombobj:
        case volatiletransportobj:
            if (obj->flags & FL_STATIONARY)
                KillActor (obj);
            else if (!(obj->flags & (FL_ATTACKMODE | FL_FIRSTATTACK)))
                FirstSighting (obj);
            break;

        case spider_mutantobj:
        case breather_beastobj:
        case cyborg_warriorobj:
        case reptilian_warriorobj:
        case acid_dragonobj:
        case mech_guardianobj:
        case liquidobj:
        case genetic_guardobj:
        case mutant_human1obj:
        case mutant_human2obj:
        case lcan_alienobj:
        case scan_alienobj:
        case gurneyobj:
        case podobj:
        case final_boss1obj:
        case final_boss2obj:
        case final_boss3obj:
        case final_boss4obj:
            if (!(obj->flags & (FL_ATTACKMODE | FL_FIRSTATTACK)))
                FirstSighting (obj);
            break;
    }
}


/*
===============
=
= SW_HandleStatic
=
= Handle all statics connected to a smart switch
=
===============
*/

void SW_HandleStatic (statobj_t *stat, int tilex, int tiley)
{
    fixed x,y;

    switch (stat->itemnumber)
    {
        case bo_clip:
        case bo_clip2:
            x = ((fixed)tilex << TILESHIFT) + 0x7FFF;
            y = ((fixed)tiley << TILESHIFT) + 0x7FFF;

            SpawnCusExplosion (x,y,SPR_CLIP_EXP1,7,30 + (US_RndT() & 0x27),explosionobj);

            stat->shapenum = -1;
            stat->itemnumber = bo_nothing;
            break;
    }
}


/*
=============================
=
= OperateSmartSwitch
=
= PARAMETERS:
=   tilex - Tile X coord that the Smart switch points to.
=   tiley - Tile Y coord that the Smart switch points to.
=   force - Force switch operation. Will not check the player's current
=           and last tilex & tiley coords. This is useful for other
=           actors toggling barrier switches.
=
= RETURNS:  TRUE  - Remove switch from map
=           FALSE - Keep switch in map
=
=============================
*/

enum what_is_it
{
    wit_NOTHING,
    wit_DOOR,
    wit_WALL,
    wit_STATIC,
    wit_ACTOR,
};

bool OperateSmartSwitch (int tilex, int tiley, int operation, bool force)
{
    int       WhatItIs;
    objtype   *obj;
    statobj_t *stat = NULL;
    unsigned  tile,doornum;
    unsigned  iconnum;

    //
    // get some information about what
    // this switch is pointing to
    //
    tile = tilemap[tilex][tiley];
    obj = actorat[tilex][tiley];
    iconnum = MAPSPOT(tilex,tiley,1);
    WhatItIs  = wit_NOTHING;

    //
    // determine if the switch points to an
    // actor, door, wall, static or is special
    //
    if (!ISPOINTER(obj))
    {
        if ((uintptr_t)obj == BLOCKTILE && !tile)
            WhatItIs = wit_STATIC;
        else
        {
            if (tile)
            {
                //
                // we have a wall of some type (maybe a door)
                //
                if (tile & 0x80)
                {
                    WhatItIs = wit_DOOR;
                    doornum = tile & ~0x80;
                }
                else
                    WhatItIs = wit_WALL;
            }
            else
            {
                stat = FindStatic(tilex,tiley);

                if (stat)
                    WhatItIs = wit_STATIC;
            }
        }
    }
    else
    {
        if (ISPOINTER(obj))
            WhatItIs = wit_ACTOR;
        else
            WhatItIs = wit_NOTHING;
    }

    switch (WhatItIs)
    {
        case wit_DOOR:
            if (doorobjlist[doornum].action == dr_jammed)
                return false;

            doorobjlist[doornum].lock = kt_none;
            OpenDoor (doornum);
            return false;

        case wit_ACTOR:
            if (!(obj->flags & FL_DEADGUY))
                SW_HandleActor (obj);
            return true;

        case wit_WALL:
            if (force || player_oldtilex != player->tilex || player_oldtiley != player->tiley)
            {
                switch (tile)
                {
                    case OFF_SWITCH:
                        if (operation == ST_TURN_OFF)
                            return false;

                        ActivateWallSwitch (iconnum,tilex,tiley);
                        break;

                    case ON_SWITCH:
                        if (operation == ST_TURN_ON)
                            return false;

                        ActivateWallSwitch (iconnum,tilex,tiley);
                        break;
                }
            }
            return false;

        case wit_STATIC:
            if (!stat)
                break;

            SW_HandleStatic (stat,tilex,tiley);
            return true;

        case wit_NOTHING:
            return true;
    }

    return false;
}


/*
============================================================================

                           WEAPON BOUNCE CODE

============================================================================
*/


/*
===============
=
= InitWeaponBounce
=
===============
*/

void InitWeaponBounce (void)
{
    bounceOffset = wb_MidPoint;
    bounceDest = wb_MaxPoint;
    bounceVel = bounceOk = 0;
}


/*
===============
=
= HandleWeaponBounce
=
===============
*/

void HandleWeaponBounce (void)
{
    int bounceSpeed;

    bounceSpeed = ANG90 - ((20 - viewsize) * 6);

    if (bounceOk)
    {
        if (bounceOffset < bounceDest)
        {
            bounceVel += ((sintable[bounceSpeed] + 1) >> 1) * screen.scale;
            bounceOffset += bounceVel;

            if (bounceOffset > bounceDest)
            {
                bounceDest = wb_MinPoint;
                bounceVel >>= 2;
            }
        }
        else if (bounceOffset > bounceDest)
        {
            bounceVel -= (sintable[bounceSpeed] >> 2) * screen.scale;
            bounceOffset += bounceVel;

            if (bounceOffset < bounceDest)
            {
                bounceDest = wb_MaxPoint;
                bounceVel >>= 2;
            }
        }
    }
    else
    {
        if (bounceOffset > wb_MidPoint)
        {
            bounceOffset -= wb_MinPoint;

            if (bounceOffset < wb_MidPoint)
                bounceOffset = wb_MidPoint;
        }
        else if (bounceOffset < wb_MidPoint)
        {
            bounceOffset += wb_MinPoint;

            if (bounceOffset > wb_MidPoint)
                bounceOffset = wb_MidPoint;
        }

        bounceDest = wb_MaxPoint;
        bounceVel = 0;
    }

    if (bounceOffset > wb_MaxOffset)
        bounceOffset = wb_MaxOffset;
    else if (bounceOffset < wb_MinOffset)
        bounceOffset = wb_MinOffset;
}
