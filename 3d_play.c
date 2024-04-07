// 3D_PLAY.C

#include "3d_def.h"


#define JOYSCALE        2


//
// list of songs for each level
//
int songs[MAPS_PER_EPISODE] =
{
    MAJMIN_MUS,              // 0
    STICKS_MUS,              // 1
    MOURNING_MUS,            // 2
    LURKING_MUS,             // 3
    CIRCLES_MUS,             // 4
    TIME_MUS,                // 5
    TOHELL_MUS,              // 6
    FORTRESS_MUS,            // 7
    GIVING_MUS,              // 8
    HARTBEAT_MUS,            // 9
    MOURNING_MUS,            // 10
    MAJMIN_MUS,              // 11
    VACCINAP_MUS,            // 12
    LURKING_MUS,             // 13
    MONASTRY_MUS,            // 14
    TOMBP_MUS,               // 15
    DARKNESS_MUS,            // 16
    MOURNING_MUS,            // 17
    SERPENT_MUS,             // 18
    TIME_MUS,                // 19
    CATACOMB_MUS,            // 20
    PLOT_MUS,                // 21
    GIVING_MUS,              // 22
    VACCINAP_MUS,            // 23
};

int             lastmusicchunk;
int             musicnum;

bool            madenoise;         // true when shooting or screaming
unsigned        alerted,alertedarea;

int             playstate;

bool            PowerBall;

#ifdef TECH_SUPPORT_VERSION
int             DebugOk = true;
#else
int             DebugOk;
#endif
int             bordertime,DebugOk,InstantWin,InstantQuit;

unsigned        ExtraRadarFlags;

#ifdef IN_DEVELOPMENT
int             TestQuickSave,TestAutoMapper;
#endif

objtype         objlist[MAXACTORS],*lastobj;
objtype         *player,*objfreelist;
objtype         dummyobj;

objtype         *actorat[MAPSIZE][MAPSIZE];
#ifdef LOOK_FOR_DEAD_GUYS
objtype         *deadguys[MAXACTORS];
int             numdeadguys;
#endif
int             objcount;

bool            singlestep,godmode;
int             extravbls;

//
// replacing refresh manager
//
int             tics,realtics;
bool            usedummy;

//
// control info
//
bool            mouseenabled,joystickenabled;
int             dirscan[4] = {sc_UpArrow,sc_RightArrow,sc_DownArrow,sc_LeftArrow};
int             buttonscan[NUMBUTTONS] = {sc_Control,sc_Alt,sc_RShift,sc_Space,sc_1,sc_2,sc_3,sc_4,sc_5,sc_6,sc_7};
int             buttonmouse[4] = {bt_attack,bt_strafe,bt_use,bt_nobutton};
int             buttonjoy[32] =
{
    bt_attack, bt_strafe, bt_use, bt_run, bt_strafeleft, bt_straferight, bt_esc, bt_pause,
    bt_prevweapon, bt_nextweapon, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton,

    bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton,
    bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton, bt_nobutton,
};

bool            buttonheld[NUMBUTTONS];

//
// curent user input
//
int             controlx,controly;          // range from -100 to 100 per tic
bool            buttonstate[NUMBUTTONS];
bool            refreshscreen;

bool            demorecord,demoplayback;
int8_t          *demoptr,*lastdemoptr;
int8_t          *demobuffer;
#if (GAME_VERSION != SHAREWARE_VERSION) || defined(GEORGE_CHEAT)
byte            jam_buff_cmp[] = {sc_J,sc_A,sc_M};
byte            jam_buff[sizeof(jam_buff_cmp)];
#endif

const char      *PAUSED_MSG = "^ST1^CEGame Paused\r^CEPress any key to resume.^XX";


void    SpaceEntryExit (bool entry);
void    FinishPaletteShifts (void);
void    ShowQuickInstructions (void);
void    PopupAutoMap (void);


/*
=============================================================================

                        USER CONTROL

=============================================================================
*/


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
= PollMouseButtons
=
===================
*/

void PollMouseButtons (void)
{
    int buttons;

    buttons = IN_GetMouseButtons();

    if (buttons & 1)
        buttonstate[buttonmouse[0]] = true;
    if (buttons & 2)
        buttonstate[buttonmouse[1]] = true;
    if (buttons & 4)
        buttonstate[buttonmouse[2]] = true;
}


/*
===================
=
= PollJoystickButtons
=
===================
*/

void PollJoystickButtons (void)
{
    int      i;
    uint32_t buttons,bits;

    buttons = IN_GetJoyButtons();

    for (i = 0, bits = 1; i < numjoybuttons; i++, bits <<= 1)
    {
        if (buttons & bits)
            buttonstate[buttonjoy[i]] = true;
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
= PollMouseMove
=
===================
*/

void PollMouseMove (void)
{
    int mousexmove,mouseymove;

    SDL_GetRelativeMouseState (&mousexmove,&mouseymove);

    //
    // double speed when shift is pressed
    //
    if (Keyboard[sc_LShift] || Keyboard[sc_RShift] || buttonstate[bt_run])
    {
        controlx += ((mousexmove * 10) / (13 - mouseadjustment)) / 2;
        controly += ((mouseymove * 20) / (13 - mouseadjustment)) * 4;
    }

    controlx += (mousexmove * 10) / (13 - mouseadjustment);
    controly += (mouseymove * 20) / (13 - mouseadjustment);
}


/*
===================
=
= PollJoystickMove
=
===================
*/

void PollJoystickMove (void)
{
    int x,y;
    int delta;

    IN_GetJoyDelta (&x,&y);

    delta = buttonstate[bt_run] ? RUNMOVE * tics : BASEMOVE * tics;

    if (x > 64 || buttonstate[bt_turnright])
        controlx += delta;
    else if (x < -64 || buttonstate[bt_turnleft])
        controlx -= delta;

    if (y > 64 || buttonstate[bt_movebackward])
        controly += delta;
    else if (y < -64 || buttonstate[bt_moveforward])
        controly -= delta;
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
= TODO: demos need better safety checks to prevent overflows
=
===================
*/

void PollControls (void)
{
    int  max,min,i;
    byte buttonbits;

    IN_ProcessEvents ();

    controlx = 0;
    controly = 0;
    memcpy (buttonheld,buttonstate,sizeof(buttonstate));
    memset (buttonstate,0,sizeof(buttonstate));

    if (demoplayback)
    {
        //
        // read commands from demo buffer
        //
        buttonbits = *demoptr++;

        for (i = 0; i < NUMBUTTONS; i++)
        {
            buttonstate[i] = buttonbits & 1;
            buttonbits >>= 1;
        }

        controlx = *demoptr++;
        controly = *demoptr++;
        tics = *demoptr++;

        while (GetTimeCount() - lasttimecount < tics)
            SDL_Delay (3);

        lasttimecount = GetTimeCount();

        if (demoptr >= lastdemoptr)
            playstate = ex_completed;   // demo is done

        controlx *= tics;
        controly *= tics;

        return;
    }

//
// get timing info for last frame
//
    CalcTics ();

//
// get button states
//
    PollKeyboardButtons ();

    if (mouseenabled && (screen.flags & SC_INPUTGRABBED))
        PollMouseButtons ();

    if (joystickenabled)
        PollJoystickButtons ();

//
// get movements
//
    PollKeyboardMove ();

    if (mouseenabled && (screen.flags & SC_INPUTGRABBED))
        PollMouseMove ();

    if (joystickenabled)
        PollJoystickMove ();

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

#ifdef DEMOS_EXTERN
    if (demorecord)
    {
        //
        // save info out to demo buffer
        //
        controlx /= tics;
        controly /= tics;

        buttonbits = 0;

        for (i = NUMBUTTONS - 1; i >= 0; i--)
        {
            buttonbits <<= 1;

            if (buttonstate[i])
                buttonbits |= 1;
        }

        *demoptr++ = buttonbits;
        *demoptr++ = controlx;
        *demoptr++ = controly;
        *demoptr++ = tics;

        if (demoptr >= lastdemoptr)
            Quit ("Demo buffer overflowed!");

        controlx *= tics;
        controly *= tics;
    }
#endif
}


/*
=====================
=
= CheckKeys
=
=====================
*/

void CheckKeys (void)
{
    bool        oneeighty = false;
    bool        oldloadedgame;
    int         i;
    int         lastoffs,oldmusicnum;
    ScanCode    scan;
    static bool Plus_KeyReleased;
    static bool Minus_KeyReleased;
    static bool I_KeyReleased;

    if ((screen.flags & SC_FADED) || demoplayback)  // don't do anything with a faded screen
        return;

    scan = LastScan;

#ifdef IN_DEVELOPMENT
#ifdef ACTIVATE_TERMINAL
    if (Keyboard[sc_9] && Keyboard[sc_0])
        ActivateTerminal (true);
#endif
#endif
    //
    // secret cheat code: 'JAM'
    //
#if GAME_VERSION != SHAREWARE_VERSION
    if (Keyboard[sc_J] || Keyboard[sc_A] || Keyboard[sc_M])
    {
        if (jam_buff[sizeof(jam_buff_cmp) - 1] != LastScan)
        {
            memmove (jam_buff,jam_buff + 1,sizeof(jam_buff_cmp) - 1);
            jam_buff[sizeof(jam_buff_cmp) - 1] = LastScan;
        }
    }
#endif
    CheckMusicToggle ();
    CheckSoundToggle ();

    if (gamestate.rpower)
    {
        if (Keyboard[sc_Equal] || Keyboard[sc_KeyPadPlus])
        {
            if (Plus_KeyReleased && gamestate.rzoom < 2)
            {
                UpdateRadarGauge ();
                gamestate.rzoom++;
                Plus_KeyReleased = false;
            }
        }
        else
            Plus_KeyReleased = true;

        if (Keyboard[sc_Minus] || Keyboard[sc_KeyPadMinus])
        {
            if (Minus_KeyReleased && gamestate.rzoom)
            {
                UpdateRadarGauge ();
                gamestate.rzoom--;
                Minus_KeyReleased = false;
            }
        }
        else
            Minus_KeyReleased = true;
    }

    if (Keyboard[sc_Enter])
    {
#if (GAME_VERSION != SHAREWARE_VERSION) || defined(GEORGE_CHEAT)
        if (!memcmp(jam_buff,jam_buff_cmp,sizeof(jam_buff_cmp)))
        {
            jam_buff[0] = '\0';

            for (i = 0; i < NUMKEYS; i++)
            {
                if (gamestate.numkeys[i] < MAXKEYS)
                    gamestate.numkeys[i] = 1;
            }

            gamestate.health = 100;
            gamestate.ammo = MAX_AMMO;
            gamestate.rpower = MAX_RADAR_ENERGY;

            if (!DebugOk)
            {
                gamestate.score = 0;
                gamestate.nextextra = EXTRAPOINTS;
            }

            gamestate.timecount += 42000L;

            for (i = 0; i < NUMWEAPONS - 1; i++)
                GiveWeapon (i);

            DrawWeapon ();
            DrawHealth ();
            DrawKeys ();
            DrawScore ();
            DISPLAY_TIMED_MSG ("\r\r     YOU CHEATER!",MP_INTERROGATE,MT_GENERAL);
            ForceUpdateStatusBar ();

            SD_StopDigitized ();
            US_ResetWindow (STATUSLINES);

            Message ("\n NOW you're jammin'!! \n");

            IN_ClearKeysDown ();
            IN_Ack ();

            DrawPlayBorder ();
        }
        else
#endif
            oneeighty = true;
    }

    //
    // handle quick turning
    //
    // TODO: change this to use buttonstate so the keys
    // can be redefined
    //
    if (!gamestate.turn_around)
    {
        //
        // 90 degrees left
        //
        if (Keyboard[sc_Q])
        {
            gamestate.turn_around = -ANG90;
            gamestate.turn_angle = player->angle + ANG90;

            if (gamestate.turn_angle >= ANG360)
                gamestate.turn_angle -= ANG360;
        }

        //
        // 180 degrees right
        //
        if (Keyboard[sc_W] || oneeighty)
        {
            gamestate.turn_around = ANG180;
            gamestate.turn_angle = player->angle + ANG180;

            if (gamestate.turn_angle >= ANG360)
                gamestate.turn_angle -= ANG360;
        }

        //
        // 90 degrees right
        //
        if (Keyboard[sc_E])
        {
            gamestate.turn_around = ANG90;
            gamestate.turn_angle = player->angle - ANG90;

            if (gamestate.turn_angle < 0)
                gamestate.turn_angle += ANG360;
        }
    }

    //
    //
    // pause key weirdness can't be checked as a scan code
    //
    if (Paused || Keyboard[sc_P])
    {
        lastoffs = StopMusic();
        fontnumber = 4;
        BMAmsg (PAUSED_MSG);
        IN_Ack ();
        IN_ClearKeysDown ();
        fontnumber = 2;
        RedrawStatusAreas ();
        ContinueMusic (lastoffs);
        Paused = false;
        IN_CenterMouse ();
        return;
    }

#ifdef IN_DEVELOPMENT
    if (TestQuickSave)
    {
        //TestQuickSave--;
        scan = sc_F8;
    }

    if (TestAutoMapper)
        PopupAutoMap ();
#endif
    switch (scan)
    {
        case sc_F7:       // end game
        case sc_F10:      // quit
            FinishPaletteShifts ();
            SD_StopDigitized ();
            ControlPanel (scan);
            DrawPlayBorder ();
            return;

        case sc_F2:       // save mission
        case sc_F8:       // quick save
            SD_StopDigitized ();
            FinishPaletteShifts ();
        case sc_F1:         // help
        case sc_F3:         // load mission
        case sc_F4:         // sound menu
        case sc_F5:         // resize view
        case sc_F6:         // controls menu
        case sc_F9:         // quick load
        case sc_Escape:     // main menu
            refreshscreen = true;

            if (scan < sc_F8)
                VW_FadeOut ();

            StopMusic ();
            SD_StopDigitized ();
            US_ResetWindow (STATUSLINES);
            ControlPanel (scan);

            VW_SetBufferOffset (0);    // going back to the 3D renderer

            if (refreshscreen)
            {
                oldloadedgame = loadedgame;

                loadedgame = false;
                DrawPlayScreen (false);

                loadedgame = oldloadedgame;
            }

            SD_StopDigitized ();

            if (!sqActive || !loadedgame)
                StartMusic ();

            IN_ClearKeysDown ();

            if (loadedgame)
            {
                DrawLevelTransition ();
                loadedgame = false;
                DrawPlayScreen (false);
            }
            else if (!refreshscreen)
                DrawPlayBorder ();

            if (!sqActive)
                StartMusic ();

            return;
    }

    if (Keyboard[sc_Tab])
        PopupAutoMap ();

    if (Keyboard[sc_Tilde])
    {
        Keyboard[sc_Tilde] = false;
        TryDropPlasmaDetonator ();
    }

    if ((DebugOk || (gamestate.flags & GS_MUSIC_TEST)) && Keyboard[sc_BackSpace])
    {
        oldmusicnum = musicnum;

        if (gamestate.flags & GS_MUSIC_TEST)
        {
            if (Keyboard[sc_LeftArrow])
            {
                if (musicnum)
                    musicnum--;

                Keyboard[sc_LeftArrow] = false;
            }
            else if (Keyboard[sc_RightArrow])
            {
                if (musicnum < LASTMUSIC - 1)
                    musicnum++;

                Keyboard[sc_RightArrow] = false;
            }

            if (oldmusicnum != musicnum)
            {
                SD_StopDigitized ();
                StartMusic ();
                DrawScore ();
            }
        }

        if (oldmusicnum == musicnum)
        {
            fontnumber = 4;
            SetFontColor (0,15);

            if (DebugKeys())
            {
                DrawPlayBorder ();

                IN_CenterMouse ();

                lasttimecount = GetTimeCount();
            }

            fontnumber = 2;

            return;
        }
    }

    if (Keyboard[sc_I])
    {
        if (I_KeyReleased)
        {
            gamestate.flags ^= GS_ATTACK_INFOAREA;

            if (gamestate.flags & GS_ATTACK_INFOAREA)
                DISPLAY_TIMED_MSG (attacker_info_enabled,MP_ATTACK_INFO,MT_GENERAL);
            else
                DISPLAY_TIMED_MSG (attacker_info_disabled,MP_ATTACK_INFO,MT_GENERAL);

            I_KeyReleased = false;
        }
    }
    else
        I_KeyReleased = true;

#ifdef CEILING_FLOOR_COLORS
    if (Keyboard[sc_C])
    {
        gamestate.flags ^= GS_DRAW_CEILING;
        Keyboard[sc_C] = false;
    }

    if (Keyboard[sc_F])
    {
        gamestate.flags ^= GS_DRAW_FLOOR;
        Keyboard[sc_F] = false;
#ifdef DUAL_SWAP_FILES
        ChangeSwapFiles (true);
#endif
    }
#endif
    if (Keyboard[sc_L])
    {
        Keyboard[sc_L] = false;
        gamestate.flags ^= GS_LIGHTING;
    }
}


/*
=====================
=
= CheckMusicToggle
=
=====================
*/

void CheckMusicToggle (void)
{
    static bool M_KeyReleased;
    size_t      len;
    const char  *togglestr[2] = {"OFF.","ON. "},*strend = "XXXX";
    char        buffer[strlen(MusicOn) + 1];

    if (Keyboard[sc_M])
    {
        if (M_KeyReleased
#if GAME_VERSION != SHAREWARE_VERSION
            && (jam_buff[0] != sc_J || jam_buff[1] != sc_A)
#endif
            )
        {
            if (MusicMode != smm_Off)
                SD_SetMusicMode (smm_Off);
            else
            {
                SD_SetMusicMode (smm_AdLib);
                StartMusic ();
            }

            snprintf (buffer,sizeof(buffer),MusicOn);

            len = strlen(buffer) - strlen(strend);

            if (strncmp(&buffer[len],strend,strlen(strend)))
                Quit ("MusicOn string MUST end with \"%s\"!",strend);

            snprintf (&buffer[len],sizeof(buffer) - len,togglestr[MusicMode != smm_Off]);

            DISPLAY_TIMED_MSG (buffer,MP_BONUS,MT_GENERAL);
            M_KeyReleased = false;
        }
    }
    else
        M_KeyReleased = true;
}


/*
=====================
=
= CheckSoundToggle
=
=====================
*/

void CheckSoundToggle (void)
{
    static bool S_KeyReleased;
    size_t      len;
    const char  *togglestr[2] = {"OFF.","ON. "},*strend = "XXXX";
    char        buffer[strlen(SoundOn) + 1];

    if (Keyboard[sc_S])
    {
        if (S_KeyReleased)
        {
            if (SoundMode != sdm_Off || DigiMode != sds_Off)
            {
                savedsoundmode = SoundMode;

                if (SoundMode != sdm_Off)
                {
                    SD_WaitSoundDone ();
                    SD_SetSoundMode (sdm_Off);
                }

                if (DigiMode != sds_Off)
                    SD_SetDigiDevice (sds_Off);
            }
            else
            {
                SD_StopDigitized ();
                SD_SetSoundMode (savedsoundmode);
                SD_SetDigiDevice (sds_SoundBlaster);

                CA_LoadAllSounds ();
            }

            snprintf (buffer,sizeof(buffer),SoundOn);

            len = strlen(buffer) - strlen(strend);

            if (strncmp(&buffer[len],strend,strlen(strend)))
                Quit ("SoundOn string MUST end with \"%s\"!",strend);

            snprintf (&buffer[len],sizeof(buffer) - len,togglestr[SoundMode != sdm_Off]);

            DISPLAY_TIMED_MSG (buffer,MP_BONUS,MT_GENERAL);

            S_KeyReleased = false;
        }
    }
    else
        S_KeyReleased = true;
}

#ifdef DUAL_SWAP_FILES
/*
=====================
=
= ChangeSwapFiles
=
= Change out swap files during game play
=
=====================
*/

void ChangeSwapFiles (bool display)
{
    SD_StopDigitized ();

    if (display)
    {
        US_ResetWindow (0);
        Message (computing);
    }

    PM_Shutdown ();
    PM_Startup ();

    if (display)
    {
        IN_UserInput (50);
        IN_ClearKeysDown ();

        DrawPlayBorder ();
    }
}
#endif

/*
=====================
=
= PopupAutoMap
=
=====================
*/

void PopupAutoMap (void)
{
    #define BASE_X 64
    #define BASE_Y 44

    SD_StopSound ();
    SD_StopDigitized ();
    VW_DrawPic (BASE_X,BASE_Y,AUTOMAPPIC);

    ShowStats (BASE_X + 101,BASE_Y + 22,ss_quick,&gamestuff.level[gamestate.mapon].stats);

    IN_Ack ();
    IN_ClearKeysDown ();

    DrawPlayBorder ();
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

    objcount = 0;
#ifdef LOOK_FOR_DEAD_GUYS
    numdeadguys = 0;
    memset (deadguys,0,sizeof(deadguys));
#endif

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
    objtype *obj;

    if (objcount >= MAXACTORS - 1)
    {
        obj = player->next;

        while (obj)
        {
            if ((obj->flags & (FL_DEADGUY | FL_VISIBLE)) == FL_DEADGUY)
            {
                RemoveObj (obj);
                obj = NULL;
            }
            else
                obj = obj->next;
        }
    }

    if (!objfreelist)
    {
        if (usedummy)
        {
            newobj = &dummyobj;
            memset (newobj,0,sizeof(*newobj));
        }
        else
            Quit ("No free spots in objlist!");
    }
    else
    {
        newobj = objfreelist;
        objfreelist = newobj->prev;
        memset (newobj,0,sizeof(*newobj));

        if (lastobj)
            lastobj->next = newobj;

        newobj->prev = lastobj;     // newobj->next is already NULL from memset

        lastobj = newobj;

        objcount++;
    }

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
    if (gone == &dummyobj)
        return;

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

    objcount--;
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
    void    (*think)(objtype *);
    objtype *check;
    int     damage;

    if (obj->flags & FL_FREEZE)
        return;

    if (obj->flags & FL_BARRIER)
    {
        check = actorat[obj->tilex][obj->tiley];

        if (ISPOINTER(check))
        {
            if (BARRIER_STATE(obj) == bt_ON)
            {
                check->flags |= FL_BARRIER_DAMAGE;

                if (US_RndT() < 0x7f && (check->flags & FL_SHOOTABLE))
                {
                    switch (obj->obclass)
                    {
                        case arc_barrierobj:   // arc barrier - mild damage
                            damage = 500;   // 100
                            break;

                        case post_barrierobj:  // post barrier - butt kicker
                            damage = 500;
                            break;

                        default:
                            damage = 0;
                    }

                    DamageActor (check,damage,obj);
                }
            }
            else
                check->flags &= ~FL_BARRIER_DAMAGE;
        }
    }

    if (obj->active == ac_no && !areabyplayer[obj->areanumber])
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

        //
        // TODO: this shouldn't be necessary because a non-transitional obj
        // normally doesn't move. Maybe it's in case its spot got overwritten
        // by another actor?
        //
        if (!(obj->flags & FL_NEVERMARK))
        {
            if ((obj->flags & FL_NONMARK) && actorat[obj->tilex][obj->tiley])
                return;

            actorat[obj->tilex][obj->tiley] = obj;
        }

        return;
    }

//
// transitional object
//
    obj->ticcount -= tics;

    while (obj->ticcount <= 0)
    {
        think = obj->state->action;   // end of state action

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
        // TODO: there's a potential bug here where the action function
        // changes obj->state, but we overwrite it again here. It can
        // be prevented by checking if obj->ticcount is still <= 0, but
        // check first that it doesn't break anything
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

    if (!(obj->flags & FL_NEVERMARK))
    {
        if ((obj->flags & FL_NONMARK) && actorat[obj->tilex][obj->tiley])
            return;

        actorat[obj->tilex][obj->tiley] = obj;
    }
}


/*
=============================================================================

                        MUSIC STUFF

=============================================================================
*/


/*
=================
=
= StopMusic
=
=================
*/

int StopMusic (void)
{
    int lastoffs;

    lastoffs = SD_MusicOff();

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
#if defined(IN_DEVELOPMENT) || GAME_VERSION != SHAREWARE_VERSION || defined(TECH_SUPPORT_VERSION)
    if (gamestate.flags & GS_MUSIC_TEST)
        lastmusicchunk = musicnum;
    else
#endif
    if (playstate == ex_victorious)
        lastmusicchunk = FORTRESS_MUS;
    else
        lastmusicchunk = songs[gamestate.mapon + (MAPS_PER_EPISODE * gamestate.episode)];

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
#if defined(IN_DEVELOPMENT) || GAME_VERSION != SHAREWARE_VERSION || defined(TECH_SUPPORT_VERSION)
    if (gamestate.flags & GS_MUSIC_TEST)
        lastmusicchunk = musicnum;
    else
#endif
    if (playstate == ex_victorious)
        lastmusicchunk = FORTRESS_MUS;
    else
        lastmusicchunk = songs[gamestate.mapon + (MAPS_PER_EPISODE * gamestate.episode)];

    SD_ContinueMusic (STARTMUSIC + lastmusicchunk,offs);
}


/*
=============================================================================

                    PALETTE SHIFTING STUFF

=============================================================================
*/

#define NUMREDSHIFTS    6
#define REDSTEPS        8

#define NUMWHITESHIFTS  3
#define WHITESTEPS      20
#define WHITETICS       6


SDL_Color redshifts[NUMREDSHIFTS][256];
SDL_Color whiteshifts[NUMREDSHIFTS][256];

int       damagecount,bonuscount;
bool      palshifted;


/*
=====================
=
= InitPaletteShifts
=
=====================
*/

void InitPaletteShifts (void)
{
    SDL_Color *workptr,*baseptr;
    int       i,j,delta;

//
// fade through intermediate frames
//
    for (i = 1; i <= NUMREDSHIFTS; i++)
    {
        workptr = redshifts[i - 1];
        baseptr = gamepal;

        for (j = 0; j < 256; j++)
        {
            delta = 256 - baseptr->r;
            workptr->r = baseptr->r + ((delta * i) / REDSTEPS);
            delta = -baseptr->g;
            workptr->g = baseptr->g + ((delta * i) / REDSTEPS);
            delta = -baseptr->b;
            workptr->b = baseptr->b + ((delta * i) / REDSTEPS);
            workptr->a = SDL_ALPHA_OPAQUE;
            baseptr++;
            workptr++;
        }
    }

    for (i = 1; i <= NUMWHITESHIFTS; i++)
    {
        workptr = whiteshifts[i - 1];
        baseptr = gamepal;

        for (j = 0; j < 256; j++)
        {
            delta = 256 - baseptr->r;
            workptr->r = baseptr->r + ((delta * i) / WHITESTEPS);
            delta = 248 - baseptr->g;
            workptr->g = baseptr->g + ((delta * i) / WHITESTEPS);
            delta = -baseptr->b;
            workptr->b = baseptr->b + ((delta * i) / WHITESTEPS);
            workptr->a = SDL_ALPHA_OPAQUE;
            baseptr++;
            workptr++;
        }
    }
}


/*
=====================
=
= ClearPaletteShifts
=
=====================
*/

void ClearPaletteShifts (void)
{
    bonuscount = damagecount = 0;
    palshifted = false;
}


/*
=====================
=
= StartBonusFlash
=
=====================
*/

void StartBonusFlash (void)
{
    bonuscount = NUMWHITESHIFTS * WHITETICS;    // white shift palette
}


/*
=====================
=
= StartDamageFlash
=
=====================
*/

void StartDamageFlash (int damage)
{
    damagecount += damage;
}


/*
=====================
=
= UpdatePaletteShifts
=
=====================
*/

void UpdatePaletteShifts (void)
{
    int red,white;

    if (bonuscount)
    {
        white = (bonuscount / WHITETICS) + 1;

        if (white > NUMWHITESHIFTS)
            white = NUMWHITESHIFTS;

        bonuscount -= tics;

        if (bonuscount < 0)
            bonuscount = 0;
    }
    else
        white = 0;

    if (damagecount)
    {
        red = (damagecount / 10) + 1;

        if (red > NUMREDSHIFTS)
            red = NUMREDSHIFTS;

        damagecount -= tics;

        if (damagecount < 0)
            damagecount = 0;
    }
    else
        red = 0;

    if (red)
    {
        VW_SetPalette (redshifts[red - 1]);
        palshifted = true;
    }
    else if (white)
    {
        VW_SetPalette (whiteshifts[white - 1]);
        palshifted = true;
    }
    else if (palshifted)
    {
        //
        // back to normal
        //
        VW_SetPalette (gamepal);
        palshifted = false;
    }
}


/*
=====================
=
= FinishPaletteShifts
=
= Resets palette to normal if needed
=
=====================
*/

void FinishPaletteShifts (void)
{
    if (palshifted)
    {
        palshifted = false;

        VW_SetPalette (gamepal);
        VW_UpdateScreen (screen.buffer);
    }
}


/*
=============================================================================

                        CORE PLAYLOOP

=============================================================================
*/


/*
===================
=
= PlayLoop
=
===================
*/

void PlayLoop (void)
{
    bool resetareas = false;
    objtype *obj;

    playstate = ex_stillplaying;
    lasttimecount = GetTimeCount();
    framecount = frameon = 0;
    pwallstate = anglefrac = 0;
    resetareas = false;

    memset (buttonstate,0,sizeof(buttonstate));
    ClearPaletteShifts ();
    ForceUpdateStatusBar ();

    IN_CenterMouse ();

    //
    // TODO: why?
    //
    tics = 1;   // for first time through

    if (demoplayback)
        IN_StartAck ();

    while (playstate == ex_stillplaying && !startgame)
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
                if (!resetareas)
                    memset (areabyplayer,1,sizeof(areabyplayer));

                resetareas = true;

                if ((!(obj->flags & FL_INFORMANT) && (obj->flags & FL_SHOOTABLE)) || (obj->obclass == liquidobj && !(obj->flags & FL_DEADGUY)))
                    DamageActor (obj,1000,player);
            }
            else if (resetareas)
            {
                ConnectAreas ();
                resetareas = false;
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

        if (demoplayback && demoptr >= lastdemoptr)
            playstate = ex_title;

        //
        // debug aids
        //
        if (singlestep)
        {
            VW_WaitVBL(14);
            lasttimecount = GetTimeCount();
        }

        if (extravbls)
            VW_WaitVBL (extravbls);

        if (demoplayback && IN_CheckAck())
        {
            IN_ClearKeysDown ();
            playstate = ex_abort;
        }
    }

    if (playstate != ex_died)
        FinishPaletteShifts ();

    gamestate.flags &= ~GS_VIRGIN_LEVEL;
}


/*
===================
=
= ShowQuickInstructions
=
===================
*/

void ShowQuickInstructions (void)
{
    ShowQuickMsg = false;

    if (demoplayback || gamestate.mapon || (gamestate.flags & GS_QUICKRUN))
        return;

    SD_StopDigitized ();

    WindowX = 0;
    WindowY = 16;
    WindowW = screen.basewidth;
    WindowH = (screen.baseheight - STATUSLINES) + WindowY;

    CacheMessage (QUICK_INFO1_TEXT);

    VW_WaitVBL (120);

    CacheMessage (QUICK_INFO2_TEXT);

    IN_Ack ();
    IN_ClearKeysDown ();

    DrawPlayBorder ();
}
