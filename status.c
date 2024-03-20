//
// basic statusbar & info area stuff
//

#include "3d_def.h"


#define MAX_DISPLAY_SCORE       (9999999L)
#define SCORE_ROLL_WAIT         (60 * 10)         // tics

//#define NO_STATUS_BAR

//
// ECG scroll rate (delay)
//
#define HEALTH_SCROLL_RATE      7
#define HEALTH_PULSE            70

//
// text "InfoArea" defines
//
#define INFOAREA_X				3
#define INFOAREA_Y				(200 - STATUSLINES + 3)
#define INFOAREA_W				109
#define INFOAREA_H				37

#define INFOAREA_BCOLOR			0x01
#define INFOAREA_CCOLOR    	    0x1A
#define INFOAREA_TCOLOR			0xA6
#define INFOAREA_TSHAD_COLOR	0x04			// Text Shadow Color

#define NUM_AMMO_SEGS               21

#define AMMO_SMALL_FONT_NUM_WIDTH   5

#define GRENADE_ENERGY_USE          4
#define BFG_ENERGY_USE              (GRENADE_ENERGY_USE << 1)


#define GD0 0x55
#define YD0 0x35
#define RD0 0x15

#define GD1 0x53
#define YD1 0x33
#define RD1 0x13

int DimAmmo[2][22] =
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

int LitAmmo[2][22] =
{
    {GL0,GL0,GL0,GL0,GL0,GL0,GL0,YL0,YL0,YL0,YL0,YL0,YL0,YL0,RL0,RL0,RL0,RL0,RL0,RL0,RL0,RL0},
    {GL1,GL1,GL1,GL1,GL1,GL1,GL1,YL1,YL1,YL1,YL1,YL1,YL1,YL1,RL1,RL1,RL1,RL1,RL1,RL1,RL1,RL1},
};


#define IA_MAX_LINE         30

typedef struct
{
    int16_t	x,y;
    int16_t	text_color;
    int16_t	backgr_color;
    int16_t left_margin;
    int8_t  delay;
    int8_t  numanims;
    int8_t  framecount;
} InfoArea_t;


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

void    writeTokenStr (const char *str);

void    ShowOverheadChunk (void);
void    LoadOverheadChunk (int tpNum);
void    SaveOverheadChunk (int tpNum);
void    DisplayTeleportName (char tpNum, bool locked);

void    ForceUpdateStatusBar (void);

void    UpdateRadarGauge (void);
void    DrawLedStrip (int x, int y, int leds, int max);
void    DisplayPinballBonus (void);
void    CheckPinballBonus (int32_t points);
byte    LevelCompleted (void);

void    DrawWeapon (void);
void    GiveWeapon (int weapon);
void    GiveAmmo (int ammo);
void    DrawGAmmoNum (void);
void    DrawMAmmoNum (void);
void    DrawPDAmmoMsg (void);
void    ComputeAvailWeapons (void);

char    *HandleControlCodes (char *first_ch);


/*
=============================================================================

					STATUS WINDOW STUFF

=============================================================================
*/

#define StatusDrawPic(x,y,pic)    VW_DrawPic ((x),(y) + (200 - STATUSLINES),(pic))


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
#ifdef NOTYET
    StartDamageFlash (points);
#endif
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
#ifdef NOTYET
    CheckPinballBonus (points);
#endif
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
            StatusDrawPic (120 + (i * 2),27,RED_KEYPIC + i);
        else
            StatusDrawPic (120 + (i * 2),27,NO_KEYPIC);
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
    PrintY = 200 - STATUSLINES + 38;

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
    DrawLedStrip (243,155,gamestate.ammo_leds,NUM_AMMO_SEGS);
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
    char zoom;

    DrawLedStrip (235,155,gamestate.radar_leds,NUM_AMMO_SEGS);

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
        StatusDrawPic (0,40,BRI_LIGHTPIC);  // TODO: check this

    gamestate.msg = Msg;

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
#if IN_DEVELOPMENT
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

        if (MsgTicsRemain <= 0)    // TODO: we don't set it back to 0 here? maybe it's because pinball bonus...
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

char default_msg[] = {  "\r    NO MESSAGES."
                        "^FCA8\r    FOOD TOKENS:      "
                        "                                 "};

char needDetonator_msg[] = "\r\r^FC39 FIND THE DETONATOR!";

char haveDetonator_msg[] = "\r\r^FC39DESTROY SECURITY CUBE!";

char destroyGoldfire_msg[] = "\r\r^FC39  DESTROY GOLDFIRE!";

void DisplayNoMoMsgs (void)
{
    LastMsgPri = MP_min_val;
#ifdef NOTYET
    if (BONUS_QUEUE)
    {
        DisplayPinballBonus ();
        return;
    }
#endif
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
    // TODO: disgusting; refactor
    //
    snprintf (&default_msg[40],sizeof(default_msg) - 40,"%-d%s",gamestate.tokens,str);

    DisplayInfoMsg (default_msg,MP_max_val,0,MT_NOTHING);
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
= KS: Modified to manipulate the string via an allocated buffer (const correctness...)
=     It might be simpler to allocate gamestate.msg directly
=
============================================================================
*/

void DrawInfoArea (void)
{
    #define IA_FONT_HEIGHT 6

    int    temp_color;
    char   *msgbuffer;
    char   *first_ch,*scan_ch;
    char   temp;
    size_t len;

#if IN_DEVELOPMENT
    if (gamestate.flags & GS_SHOW_OVERHEAD)
        return;
#endif
    DrawInfoArea_COUNT--;

    if (!gamestate.msg || !*gamestate.msg)
        return;

    len = strlen(gamestate.msg) + 1;
    msgbuffer = SafeMalloc(len);

    snprintf (msgbuffer,len,gamestate.msg);

    first_ch = msgbuffer;

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
            *scan_ch = '\0';    // TODO: buffer is already null terminated from snprintf

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

    free (msgbuffer);
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

#ifndef TP_CASE_SENSITIVE
    *first_ch = toupper(*first_ch);
    *(first_ch + 1) = toupper(*(first_ch + 1));
#endif
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
#if NUMPICS
        case pis_latchpic:  // TODO: make sure this is ok to merge
        case pis_pic:
            x = (x + 7) & 0xfff8;
            width = pictable[shapenum - STARTPICS].width;
            VW_DrawPic (x,y,shapenum);
            break;
#endif
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
= Force Draw status bar onto ALL display pages
=
= TODO: this might not be needed now
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
= TODO: this might not be needed anymore (it was only called twice anyway)
=
====================
*/

void RedrawStatusAreas (void)
{
    DrawInfoArea_COUNT = InitInfoArea_COUNT = 3;

    VW_DrawPic (0,0,TOP_STATUSBARPIC);
#ifdef NOTYET
    ShadowPrintLocationText (sp_normal);
#endif
    StatusDrawPic (0,0,STATUSBARPIC);
    DrawAmmoPic ();
    DrawScoreNum ();
    DrawWeaponPic ();
    DrawAmmoNum ();
    DrawKeyPics ();
    DrawHealthNum ();
}
