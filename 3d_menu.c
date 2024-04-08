// 3D_MENU.C

#include "3d_def.h"


#define INSTRUCTIONS_Y_POS      164
#define DESCRIPTIONS_Y_POS      134


enum ctlmenutypes
{
    MOUSEENABLE,
    JOYENABLE,
    MOUSESENS,
    CUSTOMIZE,
};


enum custmenuactions
{
    FIRE,
    STRAFE,
    RUN,
    OPEN,
};


enum custmenumoves
{
    FWRD,
    RIGHT,
    BKWD,
    LEFT,
};


char       mbarray[4][3] = {"B0","B1","B2","B3"};
int        order[4] = {RUN,OPEN,FIRE,STRAFE};
int        moveorder[4] = {LEFT,RIGHT,FWRD,BKWD};

char       LOADSAVE_GAME_MSG[2][25] = {"^ST1^CELoading Game\r^XX",
                                       "^ST1^CESaving Game\r^XX"};

const char *EndGameStr = "    End current game?\n"
                         " Are you sure (Y or N)?";


#define ENDGAMESTR (EndGameStr)

const char *QuitToDosStr = "      Quit to DOS?\n"    // TODO: good ol' DOS
                           " Are you sure (Y or N)?";


bool EscPressed;
int  lastmenumusic;


int     CP_ReadThis (int blank);
int     CP_OrderingInfo (int blank);
void    DrawEpisodePic (int w);
void    DrawAllSoundLights (int which);
void    ReadGameNames (void);
void    FreeMusic (void);
int     CP_GameOptions (int blank);
void    DrawGopMenu (void);
void    CalibrateJoystick (void);
void    ExitGame (void);
int     CP_Switches (int blank);
void    DrawSwitchMenu (void);
void    DrawAllSwitchLights (int which);
void    DrawSwitchDescription (int which);
int     CP_Display (int blank);
void    DrawDisplayMenu (screen_t *scr);
void    DrawAllDisplayLights (int which);
void    DrawDisplayDescription (int which);


CP_itemtype MainMenu[] =
{
    {AT_ENABLED,"NEW MISSION",CP_NewGame,COAL_FONT},
    {AT_READIT,"ORDERING INFO",CP_OrderingInfo},
    {AT_READIT,"INSTRUCTIONS",CP_ReadThis},
    {AT_ENABLED,"STORY",CP_BlakeStoneSaga},
    {AT_DISABLED,"",0},
    {AT_ENABLED,"GAME OPTIONS",CP_GameOptions},
    {AT_ENABLED,"HIGH SCORES",CP_ViewScores},
    {AT_ENABLED,"LOAD MISSION",CP_LoadGame},
    {AT_DISABLED,"SAVE MISSION",CP_SaveGame},
    {AT_DISABLED,"",0},
    {AT_ENABLED,"BACK TO DEMO",CP_ExitOptions},
    {AT_ENABLED,"LOGOFF",0},
};

CP_itemtype GopMenu[] =
{
    {AT_ENABLED,"SOUND",CP_Sound},
    {AT_ENABLED,"CONTROLS",CP_Control},
#ifndef CLASSIC_MENU
    {AT_ENABLED,"DISPLAY",CP_Display},
#endif
    {AT_ENABLED,"CHANGE VIEW",CP_ChangeView},
    {AT_ENABLED,"SWITCHES",CP_Switches},
};

CP_itemtype SndMenu[] =
{
    {AT_ENABLED,"NONE",0},
    {AT_ENABLED,"PC SPEAKER",0},
    {AT_ENABLED,"ADLIB/SOUND BLASTER",0},
    {AT_DISABLED,"",0},
    {AT_DISABLED,"",0},
    {AT_ENABLED,"NONE",0},
    {},
    {AT_ENABLED,"SOUND BLASTER",0},
    {AT_DISABLED,"",0},
    {AT_DISABLED,"",0},
    {AT_ENABLED,"NONE",0},
    {AT_ENABLED,"ADLIB/SOUND BLASTER",0},
};

CP_itemtype CtlMenu[] =
{
    {AT_DISABLED,"MOUSE ENABLED",0},
    {AT_DISABLED,"JOYSTICK ENABLED",0},
    {AT_DISABLED,"MOUSE SENSITIVITY",MouseSensitivity},
    {AT_ENABLED,"CUSTOMIZE CONTROLS",CustomControls},
};

CP_itemtype DisplayMenu[] =
{
    {AT_ENABLED,"VSYNC",0},
    {AT_ENABLED,"HARDWARE ACCELERATION",0},
    {AT_ENABLED,"FULLSCREEN",0},
    {AT_ENABLED,"ASPECT RATIO:",0},
    {AT_ENABLED,"RESOLUTION:",0},
    {AT_ENABLED,"APPLY",0},
};

CP_itemtype SwitchMenu[] =
{
    {AT_ENABLED,"LIGHTING",0},
    {AT_ENABLED,"REBA ATTACK INFO",0},
    {AT_ENABLED,"SHOW CEILINGS",0},
    {AT_ENABLED,"SHOW FLOORS",0},
};

#if 0
CP_itemtype NewEmenu[] =
{
    {AT_ENABLED,"MISSION 1:\nSTAR INSTITUTE",0},
    {AT_NON_SELECTABLE,"MISSION 2:\nFLOATING FORTRESS",0},
    {AT_NON_SELECTABLE,"MISSION 3:\nUNDERGROUND NETWORK",0},
    {AT_NON_SELECTABLE,"MISSION 4:\nSTAR PORT",0},
    {AT_NON_SELECTABLE,"MISSION 5:\nHABITAT II",0},
    {AT_NON_SELECTABLE,"MISSION 6:\nSATELLITE DEFENSE",0},
};
#endif

CP_itemtype NewMenu[] =
{
    {AT_ENABLED,"LEVEL 1:\nNOVICE AGENT",0},
    {AT_ENABLED,"LEVEL 2:\nSKILLED AGENT",0},
    {AT_ENABLED,"LEVEL 3:\nEXPERT AGENT",0},
    {AT_ENABLED,"LEVEL 4:\nVETERAN AGENT",0},
};

CP_itemtype LSMenu[] =
{
    {AT_ENABLED,"",0},
    {AT_ENABLED,"",0},
    {AT_ENABLED,"",0},
    {AT_ENABLED,"",0},
    {AT_ENABLED,"",0},
    {AT_ENABLED,"",0},
    {AT_ENABLED,"",0},
    {AT_ENABLED,"",0},
    {AT_ENABLED,"",0},
    {AT_ENABLED,"",0},
};

CP_itemtype CusMenu[] =
{
    {AT_ENABLED,"",0},
    {0,"",0},
    {AT_ENABLED,"",0},
    {0,"",0},
    {AT_ENABLED,"",0},
    {AT_ENABLED,"",0},
};


CP_iteminfo MainItems =     {MENU_X,MENU_Y,     lengthof(MainMenu),   MM_READ_THIS,0,9,  {77, 1,154,9,1}  },
            GopItems =      {MENU_X,MENU_Y + 30,lengthof(GopMenu),    0,           0,9,  {77, 1,154,9,1}  },
            SndItems =      {SM_X,SM_Y,         lengthof(SndMenu),    0,           0,7,  {87,-1,144,7,1}  },
            LSItems =       {LSM_X,LSM_Y,       lengthof(LSMenu),     0,           0,8,  {86,-1,144,8,1}  },
            CtlItems =      {CTL_X,CTL_Y,       lengthof(CtlMenu),   -1,           0,9,  {87,1,174,9,1}   },
            CusItems =      {CST_X,CST_Y + 7,   lengthof(CusMenu),   -1,           0,15, {54,-1,203,7,1}  },
#if 0
            NewEitems =     {NE_X,NE_Y,         lengthof(NewEMenu),   0,           0,16, {43,-2,119,16,1} },
#endif
            NewItems =      {NM_X,NM_Y,         lengthof(NewMenu),    1,           0,16, {60,-2,105,16,1} },
            SwitchItems =   {MENU_X,MENU_Y + 25,lengthof(SwitchMenu), 0,           0,9,  {87,-1,132,7,1}  },
            DisplayItems =  {MENU_X,MENU_Y + 25,lengthof(DisplayMenu),0,           0,9,  {87,-1,132,7,1}  };


int color_hlite[] =
{
    HIGHLIGHT_DISABLED_COLOR,
    HIGHLIGHT_TEXT_COLOR,
    READHCOLOR,
    HIGHLIGHT_DEACTIAVED_COLOR,
};

int color_norml[] =
{
    DISABLED_TEXT_COLOR,
    ENABLED_TEXT_COLOR,
    READCOLOR,
    DEACTIAVED_TEXT_COLOR,
};

int  EpisodeSelect[6] = {1};

int  StartGame,SoundStatus = 1,pickquick;
bool SaveGamesAvail[MaxSaveGames];
char SaveGameNames[MaxSaveGames][GAME_DESCRIPTION_LEN + 1];
char savefilename[13] = "SAVEGAM?.";

static const char *ScanNames[] =
{
    "?","?","?","?","A","B","C","D",
    "E","F","G","H","I","J","K","L",
    "M","N","O","P","Q","R","S","T",
    "U","V","W","X","Y","Z","1","2",
    "3","4","5","6","7","8","9","0",
    "RETURN","ESC","BKSP","TAB","SPACE","-","=","[",
    "]","#","?",";","'","`",",",".",
    "/","CAPSLK","F1","F2","F3","F4","F5","F6",
    "F7","F8","F9","F10","F11","F12","PRTSC",
    "SCRLK","PAUSE","INS","HOME","PGUP","DEL","END","PGDN",
    "RIGHT","LEFT","DOWN","UP","NUMLK","KP /","KP *","KP -",
    "KP +","ENTER","KP 1","KP 2","KP 3","KP 4","KP 5","KP 6",
    "KP 7","KP 8","KP 9","KP 0","KP .","\\","?","?",
    "?","F13","F14","F15","F16","F17","F18","F19",
    "F20","F21","F22","F23","F24","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","RETURN",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","CTRL","SHIFT","ALT","?","RCTRL","RSHFT","RALT",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?","?","?","?","?","?","?","?",
    "?",
};


//
// TODO: these "special keys" refer to hardcoded
// keys such as those in CheckKeys, and you're prevented
// from assigning them as your controls, but it's a horrible
// idea, especially since it prevents using the popular WASD
// control scheme
//
// Not to worry, it WILL be changed! I think the easiest way
// would be to add another menu screen for "special" keys, so
// you can assign them to whatever keys you want, provided they
// don't conflict with the main controls
//
static ScanCode specialkeys[] =
{
    sc_Tilde,
    sc_Equal,
    sc_Minus,
    sc_L,
    sc_P,
    sc_M,
    sc_S,
    sc_I,
    sc_Q,
    sc_W,
    sc_E,
    sc_Enter,
    sc_1,
    sc_2,
    sc_3,
    sc_4,
    sc_5,
    sc_Tab,
};


/*
===================
=
= HelpScreens
=
===================
*/

void HelpScreens (void)
{
#ifndef ID_CACHE_HELP
    HelpPresenter ("HELP.TXT",false,0,true);
#else
    HelpPresenter (NULL,false,HELPTEXT,true);
#endif
}


/*
===================
=
= HelpPresenter
=
===================
*/

void HelpPresenter (char *fname, bool continuekeys, int idcache, bool startmusic)
{
    PresenterInfo pi;
    int           oldwidth;

    memset (&pi,0,sizeof(pi));

    pi.flags = TPF_SHOW_PAGES;

    if (continuekeys)
        pi.flags |= TPF_CONTINUE;

    VW_FadeOut ();

    //
    // change view size to max (scaler clips shapes on smaller views)
    //
    oldwidth = baseviewwidth / 16;

    if (oldwidth != FULL_VIEW_WIDTH)
        SetViewSize (FULL_VIEW_WIDTH);

    //
    // draw help border
    //
    VW_DrawPic (0,0,H_TOPWINDOWPIC);
    VW_DrawPic (0,8,H_LEFTWINDOWPIC);
    VW_DrawPic (312,8,H_RIGHTWINDOWPIC);
    VW_DrawPic (8,176,H_BOTTOMINFOPIC);

    //
    // setup for text presenter
    //
    pi.xl = 8;
    pi.yl = 8;
    pi.xh = 311;
    pi.yh = 175;
    pi.ltcolor = 0x7b;
    pi.bgcolor = 0x7d;
    pi.dkcolor = 0x7f;
    pi.shcolor = 0x00;
    pi.fontnumber = 4;

    if (continuekeys)
        pi.infoline = " UP / DN - PAGES       ENTER - CONTINUES         ESC - EXITS";
    else
        pi.infoline = "           UP / DN - PAGES            ESC - EXITS";

    if (startmusic)
        StartCPMusic (TEXTSONG);

    TP_LoadScript (fname,&pi,idcache);
    TP_Presenter (&pi);
    TP_FreeScript (&pi,idcache);

    MenuFadeOut ();

    //
    // reset view size
    //
    if (oldwidth != FULL_VIEW_WIDTH)
        SetViewSize (oldwidth);

    if (startmusic && TPscan == sc_Escape)
        StartCPMusic (MENUSONG);

    IN_ClearKeysDown ();
}


/*
===================
=
= ControlPanel
=
= Control Panel! Ta Da!
=
===================
*/

void ControlPanel (ScanCode scan)
{
    int which;

    if (ingame)
    {
        if (CP_CheckQuick(scan))
            return;
    }

    SetupControlPanel ();
    StartCPMusic (MENUSONG);

    //
    // F-keys from within game
    //
    if (scan && scan != sc_Escape)
    {
        switch (scan)
        {
            case sc_F1:
                CleanupControlPanel ();
                HelpScreens ();
                break;

            case sc_F2:
                CP_SaveGame (0);
                CleanupControlPanel ();
                break;

            case sc_F3:
                CP_LoadGame (0);
                CleanupControlPanel ();
                break;

            case sc_F4:
                CP_Sound (0);
                CleanupControlPanel ();
                break;

            case sc_F5:
                CP_ChangeView (0);
                CleanupControlPanel ();
                break;

            case sc_F6:
                CP_Control (0);
                CleanupControlPanel ();
                break;
        }

        return;
    }

    DrawMainMenu ();
    MenuFadeIn ();
    StartGame = 0;

    //
    // main menu loop
    //
    // "exit options" or "new game" exits
    //
    while (!StartGame)
    {
        which = HandleMenu(&MainItems,&MainMenu[0],NULL);

        switch (which)
        {
            case MM_VIEW_SCORES:
                if (MainMenu[MM_VIEW_SCORES].routine == NULL)
                {
                    if (CP_EndGame(0))
                        StartGame = 1;
                }

                DrawMainMenu ();
                MenuFadeIn ();
                break;

            case -1:
            case MM_LOGOFF:
                CP_Quit (0);
                break;

            default:
                if (!StartGame)
                {
                    DrawMainMenu ();
                    MenuFadeIn ();
                }
        }
    }

    CleanupControlPanel ();

    if (!loadedgame)
        StopMusic ();

    //
    // change main menu item
    //
    if (startgame || loadedgame)
    {
        MainMenu[MM_VIEW_SCORES].routine = NULL;
        snprintf (MainMenu[MM_VIEW_SCORES].string,sizeof(MainMenu[MM_VIEW_SCORES].string),"END GAME");
    }

    if (ingame && loadedgame)
        refreshscreen = false;
}


/*
===================
=
= DrawMainMenu
=
===================
*/

void DrawMainMenu (void)
{
    VW_DrawPic (0,0,BACKGROUND_SCREENPIC);

    ClearMenuScreen ();
    DrawMenuTitle ("MAIN OPTIONS");
    DrawInstructions (IT_STANDARD);

    //
    // change "MISSION" and "DEMO"
    //
    if (ingame)
    {
        snprintf (MainMenu[MM_BACK_TO_DEMO].string,sizeof(MainMenu[MM_BACK_TO_DEMO].string),"BACK TO MISSION");
        MainMenu[MM_BACK_TO_DEMO].active = AT_READIT;
    }
    else
    {
        snprintf (MainMenu[MM_BACK_TO_DEMO].string,sizeof(MainMenu[MM_BACK_TO_DEMO].string),"BACK TO DEMO");
        MainMenu[MM_BACK_TO_DEMO].active = AT_ENABLED;
    }

    fontnumber = 4;    // COAL

    DrawMenu (&MainItems,&MainMenu[0]);

    VW_UpdateScreen (screen.buffer);
}


/*
===================
=
= CP_ReadThis
=
===================
*/

int CP_ReadThis (int blank)
{
    HelpScreens ();

    return blank;
}


/*
===================
=
= CP_OrderingInfo
=
===================
*/

int CP_OrderingInfo (int blank)
{
#ifndef ID_CACHE_HELP
    HelpPresenter ("ORDER.TXT",false,0,true);
#else
    HelpPresenter (NULL,false,ORDERTEXT,true);
#endif
    return blank;
}


/*
===================
=
= CP_BlakeStoneSaga
=
===================
*/

int CP_BlakeStoneSaga (int blank)
{
#ifndef ID_CACHE_HELP
    HelpPresenter ("SAGA.TXT",false,0,true);
#else
    HelpPresenter (NULL,false,SAGATEXT,true);
#endif
    return blank;
}


/*
===================
=
= CP_CheckQuick
=
= Check quick-keys & Quit (while in a game)
=
===================
*/

int CP_CheckQuick (int scan)
{
    switch (scan)
    {
        //
        // end game
        //
        case sc_F7:
            WindowH = MaxY - 8;

            if (Confirm(ENDGAMESTR))
            {
                playstate = ex_died;
                pickquick = gamestate.lives = 0;
            }

            WindowH = screen.baseheight;
            fontnumber = 4;
            return 1;

        //
        // quicksave
        //
        case sc_F8:
            if (SaveGamesAvail[LSItems.curpos] && pickquick)
            {
                snprintf (str,sizeof(str),"Quick Save will overwrite:\n\"%s\"?",SaveGameNames[LSItems.curpos]);
#ifdef IN_DEVELOPMENT
                if (TestQuickSave || Confirm(str))
                {
                    if (TestQuickSave)
                        TestQuickSave--;

#else
                if (Confirm(str))
                {
#endif
                    CP_SaveGame (1);
                    fontnumber = 4;
                }
                else
                    refreshscreen = false;
            }
            else
            {
                VW_FadeOut ();

                ClearMenuBorders ();
                StartCPMusic (MENUSONG);

                pickquick = CP_SaveGame(0);

                lasttimecount = GetTimeCount();

                IN_CenterMouse ();
            }
            return 1;

        //
        // quickload
        //
        case sc_F9:
            if (SaveGamesAvail[LSItems.curpos] && pickquick)
            {
                snprintf (str,sizeof(str),"Quick Load:\n\"%s\"?",SaveGameNames[LSItems.curpos]);

                if (Confirm(str))
                    CP_LoadGame (1);
                else
                {
                    refreshscreen = false;
                    return 1;
                }

                fontnumber = 4;
            }
            else
            {
                VW_FadeOut ();

                ClearMenuBorders ();
                StartCPMusic(MENUSONG);

                pickquick = CP_LoadGame(0);

                lasttimecount = GetTimeCount();

                IN_CenterMouse ();
            }

            if (pickquick)
                refreshscreen = false;
            return 1;

        //
        // quit
        //
        case sc_F10:
            US_ResetWindow (40);

            if (Confirm(QuitToDosStr))
                ExitGame ();

            refreshscreen = false;
            WindowH = screen.baseheight;
            fontnumber = 4;

            return 1;
    }

    return 0;
}


/*
===================
=
= CP_EndGame
=
= End the current game
=
===================
*/

int CP_EndGame (int blank)
{
    if (!Confirm(ENDGAMESTR))
        return 0;

    pickquick = gamestate.lives = 0;
    playstate = ex_died;
    InstantQuit = 1;

    return 1;
}


/*
===================
=
= CP_ViewScores
=
= View the high scores
=
===================
*/

int CP_ViewScores (int blank)
{
    fontnumber = 4;
    StartCPMusic (ROSTER_MUS);
    DrawHighScores ();
    MenuFadeIn ();
    fontnumber = 1;

    IN_Ack ();

    StartCPMusic (MENUSONG);
    MenuFadeOut ();

    return blank;
}


/*
===================
=
= CP_NewGame
=
= Start a new game
=
===================
*/

int CP_NewGame (int blank)
{
    int which,episode;

    DrawMenuTitle ("Difficulty Level");
    DrawInstructions (IT_STANDARD);

    episode = 0;

    while (1)
    {
        MenuFadeOut ();

        if (ingame)
            VW_DrawPic (0,0,BACKGROUND_SCREENPIC);

        DrawNewGame ();

        which = HandleMenu(&NewItems,&NewMenu[0],DrawNewGameDiff);

        if (which < 0)
        {
            MenuFadeOut();

            return blank;
        }

        ShootSnd ();
        MenuFadeOut ();

        if (!Breifing(BT_INTRO,episode))
            break;

        VW_DrawPic (0,0,BACKGROUND_SCREENPIC);
    }

    StartGame = 1;

    NewGame (which,episode);

    //
    // change "READ THIS!" to normal color
    //
    MainMenu[MM_READ_THIS].active = AT_ENABLED;

    return blank;
}


/*
===================
=
= DrawMenuTitle
=
===================
*/

void DrawMenuTitle (const char *title)
{
    fontnumber = 3;

    PrintX = WindowX = 32;
    PrintY = WindowY = 32;
    WindowW = 244;
    WindowH = 20;

    SetFontColor (TERM_SHADOW_COLOR,TERM_BACK_COLOR);
    US_PrintCentered (title);

    WindowX--;
    WindowY--;

    SetFontColor (ENABLED_TEXT_COLOR,TERM_BACK_COLOR);
    US_PrintCentered (title);
}


/*
===================
=
= DrawInstructions
=
= Draws instructions centered at the bottom of the view screen
=
= NOTE: Orginal font number or font color is not maintained
=
===================
*/

void DrawInstructions (int type)
{
    const char *instr[MAX_INSTRUCTIONS] =
    {
        "UP/DN SELECTS - ENTER CHOOSES - ESC EXITS",
        "PRESS ANY KEY TO CONTINUE",
        "ENTER YOUR NAME AND PRESS ENTER",
        "RT/LF ARROW SELECTS - ENTER CHOOSES",
    };

    fontnumber = 2;

    WindowX = 48;
    WindowY = INSTRUCTIONS_Y_POS;
    WindowW = 236;
    WindowH = 8;

    VW_Bar (WindowX,WindowY - 1,WindowW,WindowH,TERM_BACK_COLOR);

    SetFontColor (TERM_SHADOW_COLOR,TERM_BACK_COLOR);
    US_PrintCentered (instr[type]);

    WindowX--;
    WindowY--;

    SetFontColor (INSTRUCTIONS_TEXT_COLOR,TERM_BACK_COLOR);
    US_PrintCentered (instr[type]);
}

#if 0
/*
===================
=
= DrawNewEpisode
=
= Draw new episode menu
=
===================
*/

void DrawNewEpisode (void)
{
    ClearMenuScreen ();

    DrawMenuTitle ("CHOOSE A MISSION");
    DrawInstructions (IT_STANDARD);

    PrintY = 51;
    WindowX = 58;

    fontnumber = 2;       // six point font
    DrawMenu (&NewEitems,&NewEmenu7[0]);

    DrawEpisodePic (NewEitems.curpos);

    MenuFadeIn ();
    WaitKeyUp ();
}
#endif

/*
===================
=
= DrawNewGame
=
= Draw new game menu
=
===================
*/

void DrawNewGame (void)
{
    ClearMenuScreen ();
    DrawMenuTitle ("DIFFICULTY LEVEL");
    DrawInstructions (IT_STANDARD);

    fontnumber = 2;       // six point font
    DrawMenu (&NewItems,&NewMenu[0]);

    DrawNewGameDiff (NewItems.curpos);

    px = 48;
    py = INSTRUCTIONS_Y_POS - 24;
    ShPrint ("        HIGHER DIFFICULTY LEVELS CONTAIN",TERM_SHADOW_COLOR,false);

    px = 48;
    py += 6;
    ShPrint ("            MORE, STRONGER ENEMIES",TERM_SHADOW_COLOR,false);

    MenuFadeIn ();
    WaitKeyUp ();
}


/*
===================
=
= DrawNewGameDiff
=
= Draw new game graphic
=
===================
*/

void DrawNewGameDiff (int w)
{
    VW_DrawPic (192,77,w + C_BABYMODEPIC);
}


/*
===================
=
= DrawEpisodePic
=
= Draw new episode graphic
=
===================
*/

void DrawEpisodePic (int w)
{
    VW_DrawPic (176,72,w + C_EPISODE1PIC);
}


/*
===================
=
= CP_GameOptions
=
= Draw the game options menu
=
===================
*/

int CP_GameOptions (int blank)
{
    int which;

    VW_DrawPic (0,0,BACKGROUND_SCREENPIC);

    DrawGopMenu ();
    MenuFadeIn ();
    WaitKeyUp ();

    do
    {
        which = HandleMenu(&GopItems,&GopMenu[0],NULL);

        if (which != -1)
        {
            DrawGopMenu ();
            MenuFadeIn ();
        }

    } while (which >= 0);

    MenuFadeOut ();

    return blank;
}


/*
===================
=
= DrawGopMenu
=
===================
*/

void DrawGopMenu (void)
{
    VW_DrawPic (0,0,BACKGROUND_SCREENPIC);

    ClearMenuScreen ();
    DrawMenuTitle ("GAME OPTIONS");
    DrawInstructions (IT_STANDARD);

    fontnumber = 4;    // coal

    DrawMenu (&GopItems,&GopMenu[0]);

    VW_UpdateScreen (screen.buffer);
}


/*
===================
=
= ChangeSwaps
=
===================
*/

void ChangeSwaps (void)
{
    US_ResetWindow (0);
    Message (computing);

    PM_Shutdown ();
    PM_Startup ();
    SD_StopDigitized ();

    IN_UserInput (50);
    IN_ClearKeysDown ();
}


/*
===================
=
= CP_Display
=
===================
*/

int CP_Display (int blank)
{
    screen_t newscr;
    int      which;

    VW_DrawPic (0,0,BACKGROUND_SCREENPIC);

    DrawDisplayMenu (&screen);
    MenuFadeIn ();
    WaitKeyUp ();

    //
    // these are the only screen struct members newscr
    // should use; DO NOT attempt to use the others in
    // DrawDisplayMenu (unless you REALLY know what you're
    // doing!)
    //
    memset (&newscr,0,sizeof(newscr));

    newscr.width = screen.width;
    newscr.height = screen.height;
    newscr.scale = screen.scale;

    if (screen.heightoffset)
        newscr.heightoffset = 1;

    do
    {
        which = HandleMenu(&DisplayItems,DisplayMenu,DrawAllDisplayLights);

        switch (which)
        {
            case DISP_VSYNC:
                screen.flags ^= SC_VSYNC;
                SDL_RenderSetVSync (screen.renderer,(screen.flags & SC_VSYNC) != 0);
                break;

            case DISP_HWACCEL:
                screen.flags ^= SC_HWACCEL;

                if (!(screen.flags & SC_HWACCEL))
                    screen.flags &= ~SC_VSYNC;
                break;

            case DISP_FULLSCREEN:
                screen.flags ^= SC_FULLSCREEN;
                VW_ChangeDisplay (&screen);
                break;

            case DISP_RATIO:
                newscr.heightoffset ^= 1;
                break;

            case DISP_RES:
                if (++newscr.scale > 6)    // TODO: find the largest scale possible from the desktop display mode
                    newscr.scale = 1;
                break;

            case DISP_APPLY:
                VW_ChangeDisplay (&newscr);
                break;
        }

        if (which != -1)
            DrawDisplayMenu (&newscr);

        ShootSnd ();

    } while (which >= 0);

    MenuFadeOut ();

    return blank;
}


/*
=========================
=
= DrawDisplayMenu
=
=========================
*/

void DrawDisplayMenu (screen_t *scr)
{
    const char *ratio[2] = {"16:10","4:3"};
    char       *dispstr;
    size_t     len;

    VW_DrawPic (0,0,BACKGROUND_SCREENPIC);

    ClearMenuScreen ();
    DrawMenuTitle ("DISPLAY OPTIONS");
    DrawInstructions (IT_STANDARD);

    fontnumber = 2;

    scr->width = scr->scale * 320;
    scr->height = scr->scale * ((scr->heightoffset) ? 240 : 200);

    if (scr->scale != screen.scale || scr->width != screen.width || scr->height != screen.height)
        DisplayMenu[DISP_APPLY].active = AT_ENABLED;
    else
        DisplayMenu[DISP_APPLY].active = AT_DISABLED;

    if (!(screen.flags & SC_HWACCEL))
        DisplayMenu[DISP_VSYNC].active = AT_DISABLED;
    else
        DisplayMenu[DISP_VSYNC].active = AT_ENABLED;

    //
    // update the aspect ratio/resolution strings with
    // the current values
    //
    dispstr = DisplayMenu[DISP_RATIO].string;
    len = sizeof(DisplayMenu[DISP_RATIO].string);

    snprintf (dispstr,len,"ASPECT RATIO: %s",ratio[scr->heightoffset]);

    dispstr = DisplayMenu[DISP_RES].string;
    len = sizeof(DisplayMenu[DISP_RES].string);

    snprintf (dispstr,len,"RESOLUTION: %dX%d",scr->width,scr->height);

    DrawMenu (&DisplayItems,DisplayMenu);
    DrawAllDisplayLights (DisplayItems.curpos);

    VW_UpdateScreen (screen.buffer);
}


/*
===================
=
= DrawAllDisplayLights
=
===================
*/

void DrawAllDisplayLights (int which)
{
    int i;
    int shape;

    for (i = 0; i < DisplayItems.amount; i++)
    {
        if (DisplayMenu[i].string[0])
        {
            shape = C_NOTSELECTEDPIC;

            //
            // draw selected/not selected graphic buttons
            //
            if (DisplayItems.cursor.on)
            {
                if (i == which)   // is the cursor sitting on this pic?
                    shape += 2;
            }

            switch (i)
            {
                case DISP_VSYNC:
                    if (screen.flags & SC_VSYNC)
                        shape++;
                    break;

                case DISP_HWACCEL:
                    if (screen.flags & SC_HWACCEL)
                        shape++;
                    break;

                case DISP_FULLSCREEN:
                    if (screen.flags & SC_FULLSCREEN)
                        shape++;
                    break;

                default:
                    shape = 0;
                    break;
            }

            if (shape)
                VW_DrawPic (DisplayItems.x - 16,DisplayItems.y + (i * DisplayItems.y_spacing) - 1,shape);
        }
    }

    DrawDisplayDescription (which);
}


/*
===================
=
= DrawDisplayDescription
=
===================
*/

void DrawDisplayDescription (int which)
{
    const char *instr[] = {"REDUCES SCREEN TEARING/FLICKERING",
                           "DISABLE TO USE A SOFTWARE FALLBACK FOR RENDERING",
                           "TOGGLES FULLSCREEN",
                           "TOGGLES A SCREEN HEIGHT MULTIPLE OF 240",
                           "CHANGE SCREEN RESOLUTION",
                           "APPLY SCREEN RESOLUTION CHANGES"};

    fontnumber = 2;

    WindowX = 48;
    WindowY = DESCRIPTIONS_Y_POS;
    WindowW = 236;
    WindowH = 8;

    VW_Bar (WindowX - 1,WindowY - 1,WindowW,WindowH,TERM_BACK_COLOR);

    if (instr[which])
    {
        SetFontColor (TERM_SHADOW_COLOR,TERM_BACK_COLOR);
        US_PrintCentered (instr[which]);

        WindowX--;
        WindowY--;

        SetFontColor (INSTRUCTIONS_TEXT_COLOR,TERM_BACK_COLOR);
        US_PrintCentered (instr[which]);
    }
}


/*
===================
=
= CP_Switches
=
= Game switches menu
=
===================
*/

int CP_Switches (int blank)
{
    int which;

    VW_DrawPic (0,0,BACKGROUND_SCREENPIC);

    DrawSwitchMenu ();
    MenuFadeIn ();
    WaitKeyUp ();

    do
    {
        which = HandleMenu(&SwitchItems,&SwitchMenu[0],DrawAllSwitchLights);

        switch (which)
        {
            case SW_LIGHTING:
                gamestate.flags ^= GS_LIGHTING;
                ShootSnd ();
                DrawSwitchMenu ();
                break;

            case SW_REBA_ATTACK_INFO:
                gamestate.flags ^= GS_ATTACK_INFOAREA;
                ShootSnd ();
                DrawSwitchMenu ();
                break;

            case SW_CEILING:
                gamestate.flags ^= GS_DRAW_CEILING;
                ShootSnd ();
                DrawSwitchMenu ();
                break;

            case SW_FLOORS:
                gamestate.flags ^= GS_DRAW_FLOOR;
                ShootSnd ();
                DrawSwitchMenu ();
                break;
        }

    } while (which >= 0);

    MenuFadeOut ();

    return blank;
}


/*
===================
=
= DrawSwitchMenu
=
===================
*/

void DrawSwitchMenu (void)
{
    VW_DrawPic (0,0,BACKGROUND_SCREENPIC);

    ClearMenuScreen ();
    DrawMenuTitle ("GAME SWITCHES");
    DrawInstructions (IT_STANDARD);

    fontnumber = 2;

    DrawMenu (&SwitchItems,&SwitchMenu[0]);
    DrawAllSwitchLights (SwitchItems.curpos);

    VW_UpdateScreen (screen.buffer);
}


/*
===================
=
= DrawAllSwitchLights
=
===================
*/

void DrawAllSwitchLights (int which)
{
    int i;
    int shape;

    for (i = 0; i < SwitchItems.amount; i++)
    {
        if (SwitchMenu[i].string[0])
        {
            shape = C_NOTSELECTEDPIC;

            //
            // draw selected/not selected graphic buttons
            //
            if (SwitchItems.cursor.on)
            {
                if (i == which)   // is the cursor sitting on this pic?
                    shape += 2;
            }

            switch (i)
            {
                case SW_LIGHTING:
                    if (gamestate.flags & GS_LIGHTING)
                        shape++;
                    break;

                case SW_REBA_ATTACK_INFO:
                    if (gamestate.flags & GS_ATTACK_INFOAREA)
                        shape++;
                    break;

                case SW_CEILING:
                    if (gamestate.flags & GS_DRAW_CEILING)
                        shape++;
                    break;

                case SW_FLOORS:
                    if (gamestate.flags & GS_DRAW_FLOOR)
                        shape++;
                    break;
            }

            VW_DrawPic (SwitchItems.x - 16,SwitchItems.y + (i * SwitchItems.y_spacing) - 1,shape);
        }
    }

    DrawSwitchDescription (which);
}


/*
===================
=
= DrawSwitchDescription
=
===================
*/

void DrawSwitchDescription (int which)
{
    const char *instr[] = {"TOGGLES LIGHT SOURCING IN HALLWAYS",
                           "TOGGLES DETAILED ATTACKER INFO",
                           "TOGGLES CEILING MAPPING",
                           "TOGGLES FLOOR MAPPING"};

    fontnumber = 2;

    WindowX = 48;
    WindowY = DESCRIPTIONS_Y_POS;
    WindowW = 236;
    WindowH = 8;

    VW_Bar (WindowX,WindowY - 1,WindowW,WindowH,TERM_BACK_COLOR);

    SetFontColor (TERM_SHADOW_COLOR,TERM_BACK_COLOR);
    US_PrintCentered (instr[which]);

    WindowX--;
    WindowY--;

    SetFontColor (INSTRUCTIONS_TEXT_COLOR,TERM_BACK_COLOR);
    US_PrintCentered (instr[which]);
}


/*
===================
=
= CP_Sound
=
= Handle sound menu
=
===================
*/

int CP_Sound (int blank)
{
    int which;

    VW_DrawPic (0,0,BACKGROUND_SCREENPIC);

    DrawSoundMenu ();
    MenuFadeIn ();
    WaitKeyUp ();

    do
    {
        which = HandleMenu(&SndItems,&SndMenu[0],DrawAllSoundLights);

        switch (which)
        {
            //
            // sound effects
            //
            case 0:
                if (SoundMode != sdm_Off)
                {
                    SD_WaitSoundDone ();
                    SD_SetSoundMode (sdm_Off);
                    SD_SetDigiDevice (sds_Off);
                    DrawSoundMenu ();
                }
                break;

            case 1:
                if (SoundMode != sdm_PC)
                {
                    SD_WaitSoundDone ();
                    SD_SetSoundMode (sdm_PC);
                    CA_LoadAllSounds ();
                    DrawSoundMenu ();
                    ShootSnd ();
                }
                break;

            case 2:
                if (SoundMode != sdm_AdLib)
                {
                    SD_WaitSoundDone ();
                    SD_SetSoundMode (sdm_AdLib);
                    CA_LoadAllSounds ();
                    DrawSoundMenu ();
                    ShootSnd ();
                }
                break;

            //
            // digitized sound
            //
            case 5:
                if (DigiMode != sds_Off)
                {
                    SD_SetDigiDevice (sds_Off);
                    DrawSoundMenu ();
                }
                break;

            case 7:
                if (DigiMode != sds_SoundBlaster)
                {
                    if (SoundMode == sdm_Off)
                        SD_SetSoundMode (sdm_PC);

                    SD_SetDigiDevice (sds_SoundBlaster);
                    CA_LoadAllSounds ();
                    DrawSoundMenu ();
                    ShootSnd ();
                }
                break;

            //
            // music
            //
            case 10:
                if (MusicMode != smm_Off)
                {
                    SD_SetMusicMode (smm_Off);
                    DrawSoundMenu ();
                    ShootSnd ();
                }
                break;

            case 11:
                if (MusicMode != smm_AdLib)
                {
                    SD_SetMusicMode (smm_AdLib);
                    DrawSoundMenu ();
                    ShootSnd ();
                    StartCPMusic (MENUSONG);
                }
                break;
        }

    } while (which >= 0);

    MenuFadeOut ();

    return blank;
}


/*
===================
=
= DrawSoundMenu
=
===================
*/

void DrawSoundMenu (void)
{
    ClearMenuScreen ();
    DrawMenuTitle ("SOUND SETTINGS");
    DrawInstructions (IT_STANDARD);

    fontnumber = 4;

    SetFontColor (DISABLED_TEXT_COLOR,TERM_BACK_COLOR);
    ShadowPrint ("SOUND EFFECTS",105,52);
    ShadowPrint ("DIGITIZED SOUNDS",105,87);
    ShadowPrint ("BACKGROUND MUSIC",105,121);

    fontnumber = 2;
    DrawMenu (&SndItems,&SndMenu[0]);

    DrawAllSoundLights (SndItems.curpos);

    VW_UpdateScreen (screen.buffer);
}


/*
===================
=
= DrawAllSoundLights
=
===================
*/

void DrawAllSoundLights (int which)
{
    int i;
    int shape;

    for (i = 0; i < SndItems.amount; i++)
    {
        if (SndMenu[i].string[0])
        {
            shape = C_NOTSELECTEDPIC;

            //
            // draw selected/not selected graphic buttons
            //
            if (SndItems.cursor.on)
            {
                if (i == which)   // is the cursor sitting on this pic?
                    shape += 2;
            }

            switch (i)
            {
                //
                // sound effects
                //
                case 0: if (SoundMode == sdm_Off) shape++; break;
                case 1: if (SoundMode == sdm_PC) shape++; break;
                case 2: if (SoundMode == sdm_AdLib) shape++; break;

                //
                // digitized sound
                //
                case 5: if (DigiMode == sds_Off) shape++; break;
                case 7: if (DigiMode == sds_SoundBlaster) shape++; break;

                //
                // music
                //
                case 10: if (MusicMode == smm_Off) shape++; break;
                case 11: if (MusicMode == smm_AdLib) shape++; break;
            }

            VW_DrawPic (SndItems.x - 16,SndItems.y + (i * SndItems.y_spacing) - 1,shape);
        }
    }
}


/*
===================
=
= DrawLSAction
=
= Draw load/save in progress
=
===================
*/

void DrawLSAction (int which)
{
    VW_FadeOut ();
    VW_SetBufferOffset (0);         // this is a 3D renderer screen

    DrawTopInfo (sp_loading + which);
    DrawPlayBorder ();
    DisplayPrepingMsg (LOADSAVE_GAME_MSG[which]);

    if (which)
        LoadLevelUpdate (1,1);      // GFX: bar is full when saving

    LS_current = 1;
    LS_total = 19;
    WindowY = screen.baseheight - 19;
}


/*
===================
=
= CP_LoadGame
=
= Load saved games
=
===================
*/

int CP_LoadGame (int quick)
{
    FILE *file;
    int  which,exit = 0;
    char name[13];

    snprintf (name,sizeof(name),"%s%s",savefilename,extension);

    //
    // quickload?
    //
    if (quick)
    {
        which = LSItems.curpos;

        if (SaveGamesAvail[which])
        {
            name[7] = which + '0';

            MakeDestPath (name);

            file = fopen(tempPath,"rb");

            if (!file)
                CA_CannotOpen (tempPath);

            DrawLSAction (0);       // testing

            loadedgame = LoadTheGame(file);

            if (!loadedgame)
                LS_current = -1;    // clean up

            fclose (file);

            return loadedgame;
        }
    }

    DrawLoadSaveScreen (0);

    do
    {
        which = HandleMenu(&LSItems,&LSMenu[0],TrackWhichGame);

        if (which >= 0 && SaveGamesAvail[which])
        {
            ShootSnd ();

            name[7] = which + '0';

            MakeDestPath (name);

            file = fopen(tempPath,"rb");

            if (!file)
                CA_CannotOpen (tempPath);

            DrawLSAction (0);

            if (!LoadTheGame(file))
            {
                exit = StartGame = loadedgame = false;
                LS_current = -1;   // Clean up

                DrawLoadSaveScreen (0);

                continue;
            }

            fclose (file);

            loadedgame = StartGame = true;

            ShootSnd ();

            //
            // change "read this!" to normal color
            //
            MainMenu[MM_READ_THIS].active = AT_ENABLED;
            exit = 1;

            break;
        }

    } while (which >= 0);

    if (which == -1)
        MenuFadeOut ();

    if (loadedgame)
        refreshscreen = false;

    return exit;
}


/*
===================
=
= TrackWhichGame
=
= Highlight current selected entry
=
===================
*/

void TrackWhichGame (int w)
{
    static int lastgameon;

    PrintLSEntry (lastgameon,ENABLED_TEXT_COLOR);
    PrintLSEntry (w,HIGHLIGHT_TEXT_COLOR);

    lastgameon = w;
}


/*
===================
=
= DrawLoadSaveScreen
=
===================
*/

void DrawLoadSaveScreen (int loadsave)
{
    #define DISKX 100
    #define DISKY 0

    int i;

    VW_DrawPic (0,0,BACKGROUND_SCREENPIC);
    ClearMenuScreen ();

    fontnumber = 1;

    if (!loadsave)
        DrawMenuTitle ("Load Mission");
    else
        DrawMenuTitle ("Save Mission");

    DrawInstructions (IT_STANDARD);

    for (i = 0; i < MaxSaveGames; i++)
        PrintLSEntry (i,ENABLED_TEXT_COLOR);

    fontnumber = 4;
    DrawMenu (&LSItems,&LSMenu[0]);

    MenuFadeIn ();
    WaitKeyUp ();
}


/*
===================
=
= PrintLSEntry
=
= Print load/save game entry w/box outline
=
===================
*/

void PrintLSEntry (int w,int color)
{
    SetFontColor (color,BKGDCOLOR);
    DrawOutline (LSM_X + LSItems.indent,LSM_Y + (w * LSItems.y_spacing) - 2,LSM_W - LSItems.indent,8,color,color);

    fontnumber = 2;

    PrintX = LSM_X + LSItems.indent + 2;
    PrintY = LSM_Y + (w * LSItems.y_spacing);

    if (SaveGamesAvail[w])
        US_Print (SaveGameNames[w]);
    else
        US_Print ("       ----- EMPTY -----");

    fontnumber = 1;
}


/*
===================
=
= CP_SaveGame
=
= Save current game
=
===================
*/

int CP_SaveGame (int quick)
{
    FILE           *file;
    int            x,y;
    int            which,exit = 0;
    char           name[13],input[GAME_DESCRIPTION_LEN + 1];
    CustomCursor_t TermCursor = {'@',0,HIGHLIGHT_TEXT_COLOR,2};

    snprintf (name,sizeof(name),"%s%s",savefilename,extension);

    usecustomcursor = true;
    CustomCursor = TermCursor;

    //
    // quicksave?
    //
    if (quick)
    {
        which = LSItems.curpos;

        if (SaveGamesAvail[which])
        {
            DrawLSAction (1);     // testing

            name[7] = which + '0';

            MakeDestPath (name);

            file = fopen(tempPath,"wb");

            if (!file)
                CA_CannotOpen (tempPath);

            SaveTheGame (file,SaveGameNames[which]);

            fclose (file);

            return 1;
        }
    }

    DrawLoadSaveScreen (1);

    do
    {
        which = HandleMenu(&LSItems,&LSMenu[0],TrackWhichGame);

        if (which >= 0)
        {
            //
            // overwrite existing savegame?
            //
            if (SaveGamesAvail[which])
            {
                if (!Confirm(GAMESVD))
                {
                    DrawLoadSaveScreen (1);
                    continue;
                }
                else
                {
                    DrawLoadSaveScreen (1);
                    PrintLSEntry (which,HIGHLIGHT_TEXT_COLOR);
                    VW_UpdateScreen (screen.buffer);
                }
            }

            ShootSnd ();

            snprintf (input,sizeof(input),SaveGameNames[which]);

            name[7] = which + '0';

            fontnumber = 2;

            x = LSM_X + LSItems.indent + 1;
            y = LSM_Y + (which * LSItems.y_spacing) - 1;

            VW_Bar (x,y,LSM_W - LSItems.indent - 1,7,HIGHLIGHT_BOX_COLOR);
            SetFontColor (HIGHLIGHT_TEXT_COLOR,HIGHLIGHT_BOX_COLOR);
            VW_UpdateScreen (screen.buffer);

            if (US_LineInput(x + 1,y + 1,input,input,true,GAME_DESCRIPTION_LEN,LSM_W - LSItems.indent - 10))
            {
                SaveGamesAvail[which] = true;

                snprintf (SaveGameNames[which],sizeof(SaveGameNames[which]),input);

                MakeDestPath (name);

                file = fopen(tempPath,"wb");

                if (!file)
                    CA_CannotOpen (tempPath);

                DrawLSAction (1);
                SaveTheGame (file,input);

                fclose (file);

                ShootSnd ();

                exit = 1;
            }
            else
            {
                VW_Bar (x,y,LSM_W - LSItems.indent - 1,7,TERM_BACK_COLOR);
                PrintLSEntry (which,HIGHLIGHT_TEXT_COLOR);
                VW_UpdateScreen (screen.buffer);
                SD_PlaySound (ESCPRESSEDSND);

                continue;
            }

            fontnumber = 1;

            break;
        }

    } while (which >= 0);

    MenuFadeOut ();

    usecustomcursor = false;

    return exit;
}


/*
===================
=
= CP_ExitOptions
=
===================
*/

int CP_ExitOptions (int blank)
{
    StartGame = 1;

    return blank;
}


/*
===================
=
= CP_Control
=
= Define controls
=
===================
*/

int CP_Control (int blank)
{
    int which;

    VW_DrawPic (0,0,BACKGROUND_SCREENPIC);

    DrawCtlScreen ();
    MenuFadeIn ();
    WaitKeyUp ();

    do
    {
        which = HandleMenu(&CtlItems,&CtlMenu[0],NULL);

        switch (which)
        {
            case MOUSEENABLE:
                mouseenabled ^= true;
                IN_CenterMouse ();
                DrawCtlScreen ();
                CusItems.curpos = -1;
                ShootSnd ();
                break;

            case JOYENABLE:
                joystickenabled ^= true;
                DrawCtlScreen ();
                CusItems.curpos = -1;
                ShootSnd ();
                break;

            case MOUSESENS:
            case CUSTOMIZE:
                DrawCtlScreen ();
                MenuFadeIn ();
                WaitKeyUp ();
                break;
        }

    } while (which >= 0);

    MenuFadeOut ();

    return blank;
}


/*
===================
=
= DrawMousePos
=
===================
*/

void DrawMousePos (void)
{
    int width = screen.basewidth / 2;
    int height = 8;

    VW_Bar (74,92,width,height,HIGHLIGHT_BOX_COLOR);
    DrawOutline (73,91,width + 1,height + 1,ENABLED_TEXT_COLOR,ENABLED_TEXT_COLOR);
    VW_Bar (74 + (16 * mouseadjustment),92,width / 10,height,HIGHLIGHT_TEXT_COLOR);
}


/*
===================
=
= DrawMouseSens
=
= Draw mouse sensitivity screen
=
===================
*/

void DrawMouseSens (void)
{
    ClearMenuScreen ();
    DrawMenuTitle ("MOUSE SENSITIVITY");
    DrawInstructions (IT_MOUSE_SEN);

    fontnumber = 4;

    SetFontColor (HIGHLIGHT_TEXT_COLOR,TERM_BACK_COLOR);
    PrintX = 36;
    PrintY = 91;
    US_Print ("SLOW");
    PrintX = 242;
    US_Print ("FAST");

    DrawMousePos ();

    MenuFadeIn ();
}


/*
===================
=
= MouseSensitivity
=
= Adjust mouse sensitivity
=
===================
*/

int MouseSensitivity (int blank)
{
    ControlInfo ci;
    int         exit = 0,oldMA;

    oldMA = mouseadjustment;
    DrawMouseSens ();

    do
    {
        IN_ReadControl (&ci);

        switch (ci.dir)
        {
            case dir_North:
            case dir_West:
                if (mouseadjustment)
                {
                    mouseadjustment--;
                    DrawMousePos ();
                    VW_UpdateScreen (screen.buffer);
                    SD_PlaySound (MOVEGUN1SND);
                    WaitKeyUp ();
                }
                break;

            case dir_South:
            case dir_East:
                if (mouseadjustment < 9)
                {
                    mouseadjustment++;
                    DrawMousePos ();
                    VW_UpdateScreen (screen.buffer);
                    SD_PlaySound (MOVEGUN1SND);
                    WaitKeyUp ();
                }
                break;
        }

        if (ci.button0 || Keyboard[sc_Space] || Keyboard[sc_Enter])
            exit = 1;
        else if (ci.button1 || Keyboard[sc_Escape])
            exit = 2;

    } while (!exit);

    if (exit == 2)
    {
        mouseadjustment = oldMA;
        SD_PlaySound (ESCPRESSEDSND);
    }
    else
        SD_PlaySound (SHOOTSND);

    WaitKeyUp ();
    MenuFadeOut ();

    return blank;
}


/*
===================
=
= DrawCtlScreen
=
= Draw control menu screen
=
===================
*/

void DrawCtlScreen (void)
{
    #define Y_CTL_PIC_OFS  3

    int i,x,y;

    ClearMenuScreen ();
    DrawMenuTitle ("CONTROL");
    DrawInstructions (IT_STANDARD);

    WindowX = 0;
    WindowW = screen.basewidth;
    SetFontColor (TEXTCOLOR,BKGDCOLOR);

    if (JoystickPresent)
        CtlMenu[1].active = AT_ENABLED;

    if (MousePresent)
        CtlMenu[0].active = CtlMenu[2].active = AT_ENABLED;

    CtlMenu[2].active = mouseenabled;

    fontnumber = 4;

    DrawMenu (&CtlItems,&CtlMenu[0]);

    x = CTL_X + CtlItems.indent - 24;
    y = CTL_Y + Y_CTL_PIC_OFS;

    if (mouseenabled)
        VW_DrawPic (x,y,C_SELECTEDPIC);
    else
        VW_DrawPic (x,y,C_NOTSELECTEDPIC);

    y += 9;

    if (joystickenabled)
        VW_DrawPic (x,y,C_SELECTEDPIC);
    else
        VW_DrawPic( x,y,C_NOTSELECTEDPIC);

    //
    // pick first available spot
    //
    if (CtlItems.curpos < 0 || !CtlMenu[CtlItems.curpos].active)
    {
        for (i = 0; i < CtlItems.amount; i++)
        {
            if (CtlMenu[i].active)
            {
                CtlItems.curpos = i;
                break;
            }
        }
    }

    DrawMenuCursor (&CtlItems);

    VW_UpdateScreen (screen.buffer);
}


/*
===================
=
= CustomControls
=
===================
*/

int CustomControls (int blank)
{
    int which;

    DrawCustomScreen ();

    do
    {
        which = HandleMenu(&CusItems,&CusMenu[0],FixupCustom);

        switch (which)
        {
            case 0:
               DefineMouseBtns ();
               DrawCustMouse (1);
               break;

            case 2:
                DefineJoyBtns ();
                DrawCustJoy (0);
                break;

            case 4:
                DefineKeyBtns ();
                DrawCustKeybd (0);
                break;

            case 5:
                DefineKeyMove ();
                DrawCustKeys (0);
                break;
        }

    } while (which >= 0);

    MenuFadeOut ();

    return blank;
}


/*
===================
=
= DefineMouseBtns
=
===================
*/

void DefineMouseBtns (void)
{
    CustomCtrls mouseallowed = { {1,1,1,1} };

    EnterCtrlData (2,&mouseallowed,DrawCustMouse,PrintCustMouse,MOUSE);
}


/*
===================
=
= DefineJoyBtns
=
===================
*/

void DefineJoyBtns (void)
{
    CustomCtrls joyallowed = { {1,1,1,1} };

    EnterCtrlData (5,&joyallowed,DrawCustJoy,PrintCustJoy,JOYSTICK);
}


/*
===================
=
= DefineKeyBtns
=
===================
*/

void DefineKeyBtns (void)
{
    CustomCtrls keyallowed = { {1,1,1,1} };

    EnterCtrlData (8,&keyallowed,DrawCustKeybd,PrintCustKeybd,KEYBOARDBTNS);
}


/*
===================
=
= DefineKeyMove
=
===================
*/

void DefineKeyMove (void)
{
    CustomCtrls keyallowed = { {1,1,1,1} };

    EnterCtrlData (10,&keyallowed,DrawCustKeys,PrintCustKeys,KEYBOARDMOVE);
}


/*
===================
=
= TestForValidKey
=
===================
*/

bool TestForValidKey (ScanCode scan)
{
    byte *pos;

    pos = memchr(buttonscan,scan,sizeof(buttonscan));

    if (!pos)
        pos = memchr(dirscan,scan,sizeof(dirscan));

    if (pos)
    {
        *pos = sc_None;

        SD_PlaySound (SHOOTDOORSND);
        DrawCustomScreen ();

        return true;
    }

    return false;
}


/*
===================
=
= EnterCtrlData
=
= Enter control data for any type of control
=
===================
*/

void EnterCtrlData (int index, CustomCtrls *cust, void (*DrawRtn)(int), void (*PrintRtn)(int), int type)
{
    int         j,exit,tick,which,x,picked;
    int         button,result;
    int32_t     lastflashtime;
    ControlInfo ci;
    bool        redraw,cleandisplay;

    ShootSnd ();
    PrintY = CST_Y + (13 * index);
    IN_ClearKeysDown ();
    exit = 0;
    redraw = cleandisplay = true;

    //
    // find first spot in allowed array
    //
    for (j = 0; j < lengthof(cust->allowed); j++)
    {
        if (cust->allowed[j])
        {
            which = j;
            break;
        }
    }

    do
    {
        if (redraw)
        {
            x = CST_START + (CST_SPC * which);

            if (DrawRtn)
                DrawRtn (1);

            VW_Bar (x - 1,PrintY - 1,CST_SPC,7,HIGHLIGHT_BOX_COLOR);
            SetFontColor (HIGHLIGHT_TEXT_COLOR,HIGHLIGHT_BOX_COLOR);

            if (PrintRtn)
                PrintRtn (which);

            PrintX = x;

            SetFontColor (HIGHLIGHT_TEXT_COLOR,TERM_BACK_COLOR);
            VW_UpdateScreen (screen.buffer);
            WaitKeyUp ();

            redraw = false;
        }

        IN_ReadControl (&ci);

        if (type == MOUSE || type == JOYSTICK)
        {
            if (Keyboard[sc_Enter] || Keyboard[sc_Control] || Keyboard[sc_Alt])
            {
                IN_ClearKeysDown ();
                ci.button0 = ci.button1 = false;
            }
        }

        //
        // change button value?
        //
        if ((ci.button0 | ci.button1 | ci.button2 | ci.button3) || ((type == KEYBOARDBTNS || type == KEYBOARDMOVE) && LastScan == sc_Enter))
        {
            lastflashtime = GetTimeCount();
            tick = picked = 0;

            SetFontColor (HIGHLIGHT_TEXT_COLOR,HIGHLIGHT_BOX_COLOR);

            if (type == KEYBOARDBTNS || type == KEYBOARDMOVE)
                IN_ClearKeysDown ();

            do
            {
                result = 0;

                //
                // flash cursor
                //
                if (GetTimeCount() - lastflashtime > 10)
                {
                    switch (tick)
                    {
                        case 0:
                            VW_Bar (x - 1,PrintY - 1,CST_SPC,7,HIGHLIGHT_BOX_COLOR);
                            break;

                        case 1:
                            PrintX = x;
                            US_Print ("?");
                            SD_PlaySound (HITWALLSND);
                    }

                    tick ^= 1;
                    lastflashtime = GetTimeCount();
                    VW_UpdateScreen (screen.buffer);
                }
                else
                    SDL_Delay (5);

                //
                // which type of input do we process?
                //
                switch (type)
                {
                    case MOUSE:
                        button = IN_GetMouseButtons();

                        switch (button)
                        {
                            case 1: result = 1; break;
                            case 2: result = 2; break;
                            case 4: result = 3; break;
                        }

                        if (result)
                        {
                            for (j = 0; j < lengthof(order); j++)
                            {
                                if (order[which] == buttonmouse[j])
                                {
                                    buttonmouse[j] = bt_nobutton;
                                    break;
                                }
                            }

                            buttonmouse[result - 1] = order[which];
                            picked = 1;
                            cleandisplay = false;

                            SD_PlaySound (SHOOTDOORSND);
                        }
                        break;

                    case JOYSTICK:
                        if (ci.button0)
                            result = 1;
                        else if (ci.button1)
                            result = 2;
                        else if (ci.button2)
                            result = 3;
                        else if (ci.button3)
                            result = 4;

                        if (result)
                        {
                            for (j = 0; j < lengthof(order); j++)
                            {
                                if (order[which] == buttonjoy[j])
                                {
                                    buttonjoy[j] = bt_nobutton;
                                    break;
                                }
                            }

                            buttonjoy[result - 1] = order[which];
                            picked = 1;
                            cleandisplay = false;

                            SD_PlaySound (SHOOTDOORSND);
                        }
                        break;

                    case KEYBOARDBTNS:
                        if (LastScan)
                        {
                            if (LastScan == sc_Escape)
                                break;

                            if (memchr(specialkeys,LastScan,sizeof(specialkeys)))
                                SD_PlaySound (NOWAYSND);
                            else
                            {
                                cleandisplay = TestForValidKey(LastScan);

                                if (cleandisplay)
                                    ShootSnd ();

                                buttonscan[order[which]] = LastScan;
                                picked = 1;
                            }

                            IN_ClearKeysDown ();
                        }
                        break;

                    case KEYBOARDMOVE:
                        if (LastScan)
                        {
                            if (LastScan == sc_Escape)
                                break;

                            if (memchr(specialkeys,LastScan,sizeof(specialkeys)))
                                SD_PlaySound (NOWAYSND);
                            else
                            {
                                cleandisplay = TestForValidKey(LastScan);

                                if (cleandisplay)
                                    ShootSnd ();

                                dirscan[moveorder[which]] = LastScan;
                                picked = 1;
                            }

                            IN_ClearKeysDown ();
                        }
                        break;
                }

                //
                // exit input?
                //
                if (Keyboard[sc_Escape])
                {
                    picked = 1;

                    continue;
                }

                IN_ReadControl (&ci);

            } while (!picked);

            if (!cleandisplay)
                DrawCustomScreen ();

            SetFontColor (HIGHLIGHT_TEXT_COLOR,TERM_BACK_COLOR);

            redraw = 1;

            WaitKeyUp ();

            continue;
        }

        if (ci.button1 || Keyboard[sc_Escape])
            exit = 1;

        //
        // move to another spot?
        //
        switch (ci.dir)
        {
            case dir_West:
                VW_Bar (x - 1,PrintY - 1,CST_SPC,7,TERM_BACK_COLOR);
                SetFontColor (HIGHLIGHT_TEXT_COLOR,TERM_BACK_COLOR);

                if (PrintRtn)
                    PrintRtn (which);

                do
                {
                    if (--which < 0)
                        which = lengthof(cust->allowed) - 1;

                } while (!cust->allowed[which]);

                redraw = 1;

                SD_PlaySound (MOVEGUN1SND);

                do
                {
                    IN_ReadControl (&ci);

                } while (ci.dir != dir_None);

                IN_ClearKeysDown ();
                break;

            case dir_East:
                VW_Bar (x - 1,PrintY - 1,CST_SPC,7,TERM_BACK_COLOR);
                SetFontColor (HIGHLIGHT_TEXT_COLOR,TERM_BACK_COLOR);

                if (PrintRtn)
                    PrintRtn (which);

                do
                {
                    if (++which > lengthof(cust->allowed) - 1)
                        which = 0;

                } while (!cust->allowed[which]);

                redraw = 1;

                SD_PlaySound (MOVEGUN1SND);

                do
                {
                    IN_ReadControl (&ci);

                } while (ci.dir != dir_None);

                IN_ClearKeysDown ();
                break;

            case dir_North:
            case dir_South:
                exit = 1;
        }

    } while (!exit);

    SD_PlaySound (ESCPRESSEDSND);
    WaitKeyUp ();
}


/*
===================
=
= FixupCustom
=
= Fixup cursor overdraw
=
===================
*/

void FixupCustom (int w)
{
    static int lastwhich = -1;

    switch (w)
    {
        case 0: DrawCustMouse (1); break;
        case 2: DrawCustJoy (1); break;
        case 4: DrawCustKeybd (1); break;
        case 5: DrawCustKeys (1); break;
    }


    if (lastwhich >= 0)
    {
        if (lastwhich!=w)
        {
            switch (lastwhich)
            {
                case 0: DrawCustMouse (0); break;
                case 2: DrawCustJoy (0); break;
                case 4: DrawCustKeybd (0); break;
                case 5: DrawCustKeys (0); break;
            }
        }
    }

    lastwhich = w;
}


/*
===================
=
= DrawCustomScreen
=
===================
*/

void DrawCustomScreen (void)
{
    int i;

    ClearMenuScreen ();
    DrawMenuTitle ("CUSTOMIZE");
    DrawInstructions (IT_STANDARD);

    //
    // mouse
    //
    WindowX = 32;
    WindowW = 244;

    fontnumber = 4;

    SetFontColor (0x0C,TERM_BACK_COLOR);

    PrintY = 49;
    US_CPrint ("MOUSE\n");
    PrintY = 79;
    US_CPrint ("JOYSTICK\n");
    PrintY = 109;
    US_CPrint ("KEYBOARD\n");

    fontnumber = 2;

    SetFontColor (DISABLED_TEXT_COLOR,TERM_BACK_COLOR);

    for (i = 60; i <= 120; i += 30)
    {
        ShadowPrint ("RUN",CST_START,i);
        ShadowPrint ("OPEN",CST_START + CST_SPC,i);
        ShadowPrint ("FIRE",CST_START + (CST_SPC * 2),i);
        ShadowPrint ("STRAFE",CST_START + (CST_SPC * 3),i);
    }

    ShadowPrint ("LEFT",CST_START,135);
    ShadowPrint ("RIGHT",CST_START + CST_SPC,135);
    ShadowPrint ("FWRD",CST_START + (CST_SPC * 2),135);
    ShadowPrint ("BKWRD",CST_START + (CST_SPC * 3),135);

    DrawCustMouse (0);
    DrawCustJoy (0);
    DrawCustKeybd (0);
    DrawCustKeys (0);

    //
    // pick starting point in menu
    //
    if (CusItems.curpos < 0)
    {
        for (i = 0; i < CusItems.amount; i++)
        {
            if (CusMenu[i].active)
            {
                CusItems.curpos = i;
                break;
            }
        }
    }

    MenuFadeIn ();
}


/*
===================
=
= PrintCustMouse
=
===================
*/

void PrintCustMouse (int i)
{
    int j;

    for (j = 0; j < lengthof(buttonmouse); j++)
    {
        if (order[i] == buttonmouse[j])
        {
            PrintX = CST_START + (CST_SPC * i);
            US_Print (mbarray[j]);

            break;
        }
    }
}


/*
===================
=
= DrawCustMouse
=
===================
*/

void DrawCustMouse (int hilight)
{
    int i,color;

    if (hilight)
        color = HIGHLIGHT_TEXT_COLOR;
    else
        color = ENABLED_TEXT_COLOR;

    SetFontColor (color,TERM_BACK_COLOR);

    if (!mouseenabled)
    {
        SetFontColor (DISABLED_TEXT_COLOR,TERM_BACK_COLOR);
        CusMenu[0].active = AT_DISABLED;
    }
    else
        CusMenu[0].active = AT_ENABLED;

    PrintY = CST_Y + 7;

    for (i = 0; i < lengthof(order); i++)
        PrintCustMouse (i);
}


/*
===================
=
= PrintCustJoy
=
===================
*/

void PrintCustJoy (int i)
{
    int j;

    for (j = 0; j < lengthof(buttonjoy); j++)
    {
        if (order[i] == buttonjoy[j])
        {
            PrintX = CST_START + (CST_SPC * i);
            US_Print (mbarray[j]);

            break;
        }
    }
}


/*
===================
=
= DrawCustJoy
=
===================
*/

void DrawCustJoy (int hilight)
{
    int i,color;

    if (hilight)
        color = HIGHLIGHT_TEXT_COLOR;
    else
        color = ENABLED_TEXT_COLOR;

    SetFontColor (color,TERM_BACK_COLOR);

    if (!joystickenabled)
    {
        SetFontColor (DISABLED_TEXT_COLOR,TERM_BACK_COLOR);
        CusMenu[2].active = AT_DISABLED;
    }
    else
        CusMenu[2].active = AT_ENABLED;

    PrintY = CST_Y + 37;

    for (i = 0; i < lengthof(order); i++)
        PrintCustJoy (i);
}


/*
===================
=
= PrintCustKeybd
=
===================
*/

void PrintCustKeybd (int i)
{
    PrintX = CST_START + (CST_SPC * i);

    US_Print (ScanNames[buttonscan[order[i]]]);
}


/*
===================
=
= DrawCustKeybd
=
===================
*/

void DrawCustKeybd (int hilight)
{
    int i,color;

    if (hilight)
        color = HIGHLIGHT_TEXT_COLOR;
    else
        color = ENABLED_TEXT_COLOR;

    SetFontColor (color,TERM_BACK_COLOR);

    PrintY = CST_Y + 67;

    for (i = 0; i < lengthof(order); i++)
        PrintCustKeybd (i);
}


/*
===================
=
= PrintCustKeys
=
===================
*/

void PrintCustKeys (int i)
{
    PrintX = CST_START + (CST_SPC * i);

    US_Print (ScanNames[dirscan[moveorder[i]]]);
}


/*
===================
=
= DrawCustKeys
=
===================
*/

void DrawCustKeys (int hilight)
{
    int i,color;

    if (hilight)
        color = HIGHLIGHT_TEXT_COLOR;
    else
        color = ENABLED_TEXT_COLOR;

    SetFontColor (color,TERM_BACK_COLOR);

    PrintY = CST_Y + 82;

    for (i = 0; i < lengthof(moveorder); i++)
        PrintCustKeys (i);
}


/*
===================
=
= CP_ChangeView
=
===================
*/

int CP_ChangeView (int blank)
{
    int         exit = 0,oldview,newview,lastview;
    ControlInfo ci;

    US_ResetWindow (0);

    VW_SetBufferOffset (0);    // this is a 3D renderer screen

    newview = oldview = lastview = baseviewwidth / 16;

    DrawChangeView (oldview);

    do
    {
        CheckPause ();
        IN_ReadControl (&ci);

        switch (ci.dir)
        {
            case dir_South:
            case dir_West:
                if (--newview < 6)
                    newview = 6;

                ShowViewSize (newview);
                VW_UpdateScreen (screen.buffer);

                if (newview != lastview)
                {
                    SD_PlaySound (HITWALLSND);

                    lastview = newview;
                }

                TicDelay (10);
                break;

            case dir_North:
            case dir_East:
                if (++newview > 20)
                    newview = 20;

                ShowViewSize (newview);
                VW_UpdateScreen (screen.buffer);

                if (newview != lastview)
                {
                    SD_PlaySound (HITWALLSND);

                    lastview = newview;
                }

                TicDelay (10);
                break;
        }

        if (ci.button0 || Keyboard[sc_Enter])
            exit = 1;
        else if (ci.button1 || Keyboard[sc_Escape])
        {
            SD_PlaySound (ESCPRESSEDSND);
            MenuFadeOut ();
            ClearMenuBorders ();

            return blank;
        }

    } while (!exit);

    if (oldview != newview)
    {
        ShootSnd ();
        Message (computing);
        SetViewSize (newview);
    }

    ShootSnd ();
    MenuFadeOut ();
    ClearMenuBorders ();

    return blank;
}


/*
===================
=
= DrawChangeView
=
===================
*/

void DrawChangeView (int view)
{
    DrawTopInfo (sp_changeview);
    ShowViewSize (view);

    fontnumber = 1;

    CacheBMAmsg (CHANGEVIEW_TEXT);

    MenuFadeIn ();
}


/*
===================
=
= CP_Quit
=
===================
*/

int CP_Quit (int blank)
{
    if (Confirm(QuitToDosStr))
        ExitGame ();

    DrawMainMenu ();

    return blank;
}


/*
===================
=
= ClearMenuScreen
=
===================
*/

void ClearMenuScreen (void)
{
    VW_Bar (SCREEN_X,SCREEN_Y,SCREEN_W,SCREEN_H,TERM_BACK_COLOR);
}


/*
===================
=
= ClearMenuBorders
=
===================
*/

void ClearMenuBorders (void)
{
    if (screen.heightoffset)
    {
        VW_SetBufferOffset (0);    // draw to a full screen

        VW_Bar (0,0,screen.basewidth,screen.heightoffset,BLACK);
        VW_Bar (0,screen.baseheight - screen.heightoffset,screen.basewidth,screen.heightoffset,BLACK);

        VW_SetBufferOffset (screen.heightoffset);
    }
}


/*
===================
=
= DrawWindow
=
===================
*/

void DrawWindow (int x, int y, int w, int h, int wcolor)
{
    VW_Bar (x,y,w,h,wcolor);
    DrawOutline (x,y,w,h,BORD2COLOR,DEACTIVE);
}


/*
===================
=
= DrawOutline
=
===================
*/

void DrawOutline (int x, int y, int w, int h, int color1, int color2)
{
    VW_Hlin (x,x + w,y,color2);
    VW_Vlin (y,y + h,x,color2);
    VW_Hlin (x,x + w,y + h,color1);
    VW_Vlin (y,y + h,x + w,color1);
}


/*
===================
=
= SetupControlPanel
=
===================
*/

void SetupControlPanel (void)
{
    ClearMenuBorders ();

    fontnumber = 2;

    WindowH = screen.baseheight - (screen.heightoffset * 2);

    if (!ingame)
        CA_LoadAllSounds ();
    else
        MainMenu[MM_SAVE_MISSION].active = AT_ENABLED;

    ReadGameNames ();
    IN_CenterMouse ();
}


/*
===================
=
= ReadGameNames
=
===================
*/

void ReadGameNames (void)
{
    FILE *file;
    char name[13];
    int  i;

    //
    // see which save game files are available & read string in
    //
    snprintf (name,sizeof(name),"%s%s",savefilename,extension);

    for (i = 0; i < MaxSaveGames; i++)
    {
        name[7] = '0' + i;

        MakeDestPath (name);

        file = fopen(tempPath,"rb");

        if (file)
        {
            SaveGamesAvail[i] = true;

            if (FindChunk(file,"DESC"))
            {
                if (!fread(SaveGameNames[i],sizeof(SaveGameNames[i]),1,file))
                    Quit ("Error reading file %s: %s",tempPath,strerror(errno));
            }
            else
                snprintf (SaveGameNames[i],sizeof(SaveGameNames[i]),"DESCRIPTION LOST");

            fclose (file);
        }
    }
}


/*
===================
=
= CleanupControlPanel
=
===================
*/

void CleanupControlPanel (void)
{
    if (!loadedgame)
        FreeMusic ();

    fontnumber = 4;
}


/*
===================
=
= ShadowPrint
=
= Shadow prints given text at a given x & y in default font
=
===================
*/

void ShadowPrint (const char *string, int x, int y)
{
    int oldbc,oldfc;

    oldfc = fontcolor;
    oldbc = backcolor;

    PrintX = x + 1;
    PrintY = y + 1;
    SetFontColor (TERM_SHADOW_COLOR,TERM_BACK_COLOR);
    US_Print (string);

    PrintX = x;
    PrintY = y;
    SetFontColor (oldfc,oldbc);
    US_Print (string);
}


/*
===================
=
= HandleMenu
=
= Handle moving cursor around a menu
=
===================
*/

int HandleMenu (CP_iteminfo *item_i, CP_itemtype *items, void (*routine)(int))
{
    char        key;
    static int  redrawitem = 1;
    int         i,y,basey,exit,which,flashtics,ok;
    ControlInfo ci;

    which = item_i->curpos;
    basey = item_i->y;
    y = basey + (which * item_i->y_spacing);
    item_i->cursor.on = true;

    DrawCursor (item_i,items,&y,which,basey,routine);

    SetItemColor (items + which,true);

    if (redrawitem)
        ShadowPrint ((items + which)->string,item_i->x + item_i->indent,item_i->y + (which * item_i->y_spacing));

    //
    // call custom routine if it is needed
    //
    if (routine)
        routine (which);

    VW_UpdateScreen (screen.buffer);

    flashtics = 40;
    exit = 0;
    IN_ClearKeysDown ();
    IN_ClearTextInput ();

    do
    {
        CalcTics ();

        flashtics -= tics;

        CycleColors ();

        //
        // change cursor shape
        //
        if (flashtics <= 0)
        {
            flashtics = 40;

            item_i->cursor.on ^= true;

            if (item_i->cursor.on)
                DrawCursor (item_i,items,&y,which,basey,routine);
            else
            {
                EraseCursor (item_i,items,y,which);

                if (routine)
                    routine (which);
            }

            VW_UpdateScreen (screen.buffer);
        }
        else
            SDL_Delay (5);

        CheckPause ();

        //
        // see if any keys are pressed for initial char finding
        //
        key = *textinput;

        IN_ClearTextInput ();

        if (key)
        {
            ok = false;

            if (key >= 'a')
                key -= 'a' - 'A';

            for (i = which + 1; i < item_i->amount; i++)
            {
                if ((items + i)->active && (items + i)->string[0] == key)
                {
                    EraseCursor (item_i,items,y,which);

                    which = i;
                    item_i->curpos = which;
                    item_i->cursor.on = true;
                    ok = true;

                    DrawCursor (item_i,items,&y,which,basey,routine);
                    VW_UpdateScreen (screen.buffer);

                    IN_ClearKeysDown ();

                    break;
                }
            }

            //
            // didn't find a match first time through, check again
            //
            if (!ok)
            {
                for (i = 0; i < which; i++)
                {
                    if ((items + i)->active && (items + i)->string[0] == key)
                    {
                        EraseCursor (item_i,items,y,which);

                        which = i;
                        item_i->curpos = which;
                        item_i->cursor.on = true;

                        DrawCursor (item_i,items,&y,which,basey,routine);
                        VW_UpdateScreen (screen.buffer);

                        IN_ClearKeysDown ();

                        break;
                    }
                }
            }
        }

        //
        // get input
        //
        IN_ReadControl (&ci);

        switch (ci.dir)
        {
            case dir_North:
                EraseCursor (item_i,items,y,which);

                do
                {
                    if (!which)
                        which = item_i->amount - 1;
                    else
                        which--;

                } while (!(items + which)->active);

                item_i->curpos = which;
                item_i->cursor.on = true;

                DrawCursor (item_i,items,&y,which,basey,routine);
                VW_UpdateScreen (screen.buffer);

                TicDelay (20);
                break;

            case dir_South:
                EraseCursor (item_i,items,y,which);

                do
                {
                    if (which == item_i->amount - 1)
                        which = 0;
                    else
                        which++;

                } while (!(items + which)->active);

                item_i->curpos = which;
                item_i->cursor.on = true;

                DrawCursor (item_i,items,&y,which,basey,routine);
                VW_UpdateScreen (screen.buffer);

                TicDelay (20);
                break;
        }

        if (ci.button0 || Keyboard[sc_Space] || Keyboard[sc_Enter])
            exit = 1;

        if (ci.button1 || Keyboard[sc_Escape])
            exit = 2;

    } while (!exit);

    IN_ClearKeysDown ();

    //
    // erase everything
    //
    item_i->cursor.on = false;
    redrawitem = 1;
    EraseCursor (item_i,items,y,which);

    if (routine)
        routine (which);

    VW_UpdateScreen (screen.buffer);

    item_i->curpos = which;

    switch (exit)
    {
        case 1:
            //
            // call the routine
            //
            if ((items + which)->routine)
            {
                //
                // already in a game?
                //
                if (ingame && (items + which)->routine == CP_NewGame)
                {
                    if (!Confirm(CURGAME))
                    {
                        MenuFadeOut ();

                        return 0;
                    }
                }

                ShootSnd ();
                MenuFadeOut ();
                (items + which)->routine (0);
            }

            return which;

        case 2:
            SD_PlaySound (ESCPRESSEDSND);
            return -1;
    }

    return 0;
}


/*
===================
=
= EraseCursor
=
= Erase cursor & de-highlight string
=
===================
*/

void EraseCursor (CP_iteminfo *item_i, CP_itemtype *items, int y, int which)
{
    VW_Bar (item_i->cursor.x,y + item_i->cursor.y_ofs,item_i->cursor.width,item_i->cursor.height,TERM_BACK_COLOR);
    SetItemColor (items + which,false);

    ShadowPrint ((items + which)->string,item_i->x + item_i->indent,y);
}


/*
===================
=
= DrawCursor
=
= Draw cursor at new position
=
===================
*/

void DrawCursor (CP_iteminfo *item_i, CP_itemtype *items, int *y, int which, int basey, void (*routine)(int))
{
    *y = basey + (which * item_i->y_spacing);

    VW_Bar (item_i->cursor.x,*y + item_i->cursor.y_ofs,item_i->cursor.width,item_i->cursor.height,HIGHLIGHT_BOX_COLOR);
    SetItemColor (items + which,1);

    ShadowPrint ((items + which)->string,item_i->x + item_i->indent,item_i->y + (which * item_i->y_spacing));

    //
    // call custom routine if it is needed
    //
    if (routine)
        routine(which);
}


/*
===================
=
= TicDelay
=
= Delay for an amount of tics or until controls are inactive
=
===================
*/

void TicDelay (int count)
{
    ControlInfo ci;
    int32_t     starttime;

    starttime = GetTimeCount();

    do
    {
        SDL_Delay (5);
        IN_ReadControl (&ci);

    } while (GetTimeCount() - starttime < count && ci.dir != dir_None);
}


/*
===================
=
= DrawMenu
=
= This also calculates the Y position of the current items
=
===================
*/

void DrawMenu (CP_iteminfo *item_i, CP_itemtype *items)
{
    int i,which = item_i->curpos;

    WindowX = PrintX = item_i->x + item_i->indent;
    WindowY = PrintY = item_i->y;

    WindowW = screen.basewidth;
    WindowH = screen.baseheight - (screen.heightoffset * 2);

    for (i = 0; i < item_i->amount; i++)
    {
        SetItemColor (items + i,which == i);
        ShadowPrint ((items + i)->string,WindowX,item_i->y + (i * item_i->y_spacing));
    }
}


/*
===================
=
= SetItemColor
=
= Set an item's text color (highlight or no)
=
===================
*/

void SetItemColor (CP_itemtype *items, bool hlight)
{
    if (hlight)
        SetFontColor (color_hlite[items->active],TERM_BACK_COLOR);
    else
        SetFontColor (color_norml[items->active],TERM_BACK_COLOR);
}


/*
===================
=
= WaitKeyUp
=
= Wait for ctrlkey-up or button-up
=
= TODO: this is disgusting and doesn't even work now (if it ever did)
=
===================
*/

void WaitKeyUp (void)
{
    ControlInfo ci;

    while (IN_ReadControl(&ci),ci.button0 |
           ci.button1 |
           ci.button2 |
           ci.button3|
           Keyboard[sc_Space]|
           Keyboard[sc_Enter]|
           Keyboard[sc_Escape]);
}


/*
===================
=
= Confirm
=
= Draw dialog and confirm yes or no to question
=
===================
*/

int Confirm (const char *string)
{
    int     x,y;
    int     whichsnd[2] = {ESCPRESSEDSND,SHOOTSND};
    int     exit = 0,tick = 0;
    int32_t lastblinktime;

    Message (string);

    SetFontColor (BORDER_TEXT_COLOR,BORDER_MED_COLOR);

    IN_ClearKeysDown ();

    //
    // blink cursor
    //
    x = PrintX;
    y = PrintY;
    lastblinktime = GetTimeCount();

    do
    {
        IN_ProcessEvents ();

        if (GetTimeCount() - lastblinktime >= 10)
        {
            switch (tick)
            {
                case 0:
                    VW_Bar (x,y,8,13,BORDER_MED_COLOR);
                    break;

                case 1:
                    PrintX = x;
                    PrintY = y;
                    US_Print ("_");
                    break;
            }

            VW_UpdateScreen (screen.buffer);
            tick ^= 1;
            lastblinktime = GetTimeCount();
        }
        else
            SDL_Delay (5);

    } while (!Keyboard[sc_Y] && !Keyboard[sc_N] && !Keyboard[sc_Escape]);

    if (Keyboard[sc_Y])
    {
        exit = 1;
        ShootSnd ();
    }

    WaitKeyUp ();

    IN_ClearKeysDown ();
    SD_PlaySound (whichsnd[exit]);

    return exit;
}


/*
===================
=
= Message
=
= Print a message in a window
=
===================
*/

void Message (const char *string)
{
    int        i;
    int        x,y,w,h,mw;
    int        len;
    fontstruct *font;

    w = mw = 0;
    fontnumber = 1;

    font = (fontstruct *)grsegs[STARTFONT + fontnumber];

    h = font->height;
    len = strlen(string);

    for (i = 0; i < len; i++)
    {
        if (string[i] == '\n')
        {
            if (w > mw)
                mw = w;

            w = 0;
            h += font->height;
        }
        else
            w += font->width[(byte)string[i]];
    }

    if (w + 10 > mw)
        mw = w + 10;

    x = (screen.basewidth / 2) - (mw / 2);
    y = (WindowH / 2) - (h / 2);

    //
    // bump down and to the right for shadow
    //
    PrintX = x + 1;
    PrintY = y + 1;
    WindowX = PrintX;

    BevelBox (WindowX - 6,PrintY - 6,mw + 10,h + 10,BORDER_HI_COLOR,BORDER_MED_COLOR,BORDER_LO_COLOR);

    SetFontColor (BORDER_LO_COLOR,BORDER_MED_COLOR);
    US_Print (string);

    WindowX = PrintX = x;
    PrintY = y;

    SetFontColor (BORDER_TEXT_COLOR,BORDER_MED_COLOR);
    US_Print (string);

    VW_UpdateScreen (screen.buffer);
}


/*
===================
=
= TerminateString
=
= Searches for an "^XX" and replaces with a null terminator
=
= KS: This will modify the message stored in grsegs, so beware
= of any further problems caused by the lack of the special "^XX"
= command in the message. It might be safer to duplicate the string
= and modify that instead, so the original remains unchanged
=
===================
*/

void TerminateString (char *pos)
{
    pos = strstr(pos,int_xx);

#ifdef IN_DEVELOPMENT
    if (!pos)
        Quit ("A cached message was NOT terminated with \"%s\"!",int_xx);
#endif
    if (pos)
        *pos = '\0';
}


/*
===================
=
= CacheMessage
=
= Caches and prints a message in a window
=
===================
*/

void CacheMessage (int messagenum)
{
    char *string;

    string = (char *)grsegs[messagenum];

    TerminateString (string);

    Message (string);
}


/*
===================
=
= StartCPMusic
=
= TODO: this can be merged with StartMusic
=
===================
*/

int StartCPMusic (int song)
{
    int lastoffs;

    lastmenumusic = song;
    lastoffs = SD_MusicOff();
    CA_UncacheAudioChunk (STARTMUSIC + song);

    SD_StartMusic (STARTMUSIC + song);

    return lastoffs;
}


/*
===================
=
= FreeMusic
=
===================
*/

void FreeMusic (void)
{
    SD_MusicOff ();

    CA_UncacheAudioChunk (STARTMUSIC + lastmenumusic);
}


/*
===================
=
= CheckPause
=
= Check for pause key (for music only)
=
===================
*/

void CheckPause (void)
{
    static int lastoffs;

    if (Paused)
    {
        if (SoundStatus)
            lastoffs = SD_MusicOff();
        else if (lastoffs)
            SD_ContinueMusic (lastmenumusic,lastoffs);
        else
            SD_MusicOn ();

        VW_WaitVBL (3);
        IN_ClearKeysDown ();

        SoundStatus ^= 1;
        Paused = false;
    }
}


/*
===================
=
= DrawMenuCursor
=
= Draw cursor at correct position in menu
=
===================
*/

void DrawMenuCursor (CP_iteminfo *iteminfo)
{
    int x,y;

    x = iteminfo->cursor.x;
    y = iteminfo->y + (iteminfo->curpos * iteminfo->y_spacing) + iteminfo->cursor.y_ofs;

    VW_Bar (x,y,iteminfo->cursor.width,iteminfo->cursor.height,HIGHLIGHT_BOX_COLOR);
}


/*
===================
=
= ShootSnd
=
===================
*/

void ShootSnd (void)
{
    SD_PlaySound (SHOOTSND);
}

#if GAME_VERSION == SHAREWARE_VERSION
/*
===================
=
= ShowPromo
=
===================
*/

void ShowPromo (void)
{
    #define PROMO_MUSIC HIDINGA_MUS

    SD_StartMusic (STARTMUSIC + PROMO_MUSIC);

    //
    // show promo screen 1
    //
    MenuFadeOut ();
    VW_DrawPic (0,0,PROMO1PIC);
    MenuFadeIn ();
    IN_UserInput (TickBase * 20);

    //
    // show promo screen 2
    //
    MenuFadeOut ();
    VW_DrawPic (0,0,PROMO2PIC);
    MenuFadeIn ();
    IN_UserInput (TickBase * 20);

    StopMusic ();
}
#endif

/*
===================
=
= ExitGame
=
===================
*/

void ExitGame (void)
{
    VW_FadeOut ();
#if GAME_VERSION == SHAREWARE_VERSION
    ShowPromo ();
#endif

    SD_MusicOff ();
    SD_StopSound ();

    Quit (NULL);
}


/*
===================
=
= CheckForEpisodes
=
= Check for version/extension
=
===================
*/

void CheckForEpisodes (void)
{
    struct stat statbuf;

#if (GAME_VERSION != SHAREWARE_VERSION)
    if (stat("*.VSI",&statbuf))
        snprintf (extension,sizeof(extension),"VSI");
#else
    if (stat("*.FSW",&statbuf))
        snprintf (extension,sizeof(extension),"FSW");
#endif
    else
        Quit ("No Fire Strike data files found!");

#ifdef DUAL_SWAP_FILES
    char altpagefilename[13];

    snprintf (altpagefilename,sizeof(altpagefilename),"SVSWAP.%s",extension);

    ShadowsAvail = stat(altpagefilename,&statbuf);
#endif
}
