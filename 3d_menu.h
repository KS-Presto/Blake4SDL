// 3D_MENU.H

#ifndef _3D_MENU_H_
#define _3D_MENU_H_


#include "3d_def.h"


#define GAME_DESCRIPTION_LEN 31

//
// menu color defines
//
#define HIGHLIGHT_BOX_COLOR         0x52   // box behind text for cursor
#define HIGHLIGHT_TEXT_COLOR        0x59   // color for text on cursor
#define HIGHLIGHT_DISABLED_COLOR    0x56   // color for text on cursor for a turned off item
#define HIGHLIGHT_DEACTIAVED_COLOR  0x55

#define ENABLED_TEXT_COLOR          0x56
#define DISABLED_TEXT_COLOR         0x53
#define DEACTIAVED_TEXT_COLOR       0x52

#define INSTRUCTIONS_TEXT_COLOR     0x53

#define TERM_BACK_COLOR             0x02
#define TERM_SHADOW_COLOR           0x01

//
// clearable menu terminal area
//
#define SCREEN_X        32
#define SCREEN_Y        28
#define SCREEN_W        244
#define SCREEN_H        132


#define BORDCOLOR       0x78
#define BORD2COLOR      0x74
#define DEACTIVE        0x72
#define BKGDCOLOR       0x76
#define STRIPE          0x2c

#define MenuFadeOut()   VW_FadePaletteOut (0,255,40,44,44,10)
#define MenuFadeIn()    VW_FadePaletteIn (0,255,gamepal,10)

#define READCOLOR       0x4a
#define READHCOLOR      0x47
#define VIEWCOLOR       0x7f
#define TEXTCOLOR       WHITE
#define HIGHLIGHT       0x13

#define MENUSONG        LASTLAFF_MUS
#define ROSTER_MUS      HISCORE_MUS
#define TEXTSONG        TOHELL_MUS

#define QUITSUR "Are you sure you want\n"\
                "to quit this great game? (Y/N)"

#define CURGAME "   Continuing past this\n"\
                "  point will end the game\n"\
                " you're currently playing.\n"\
                "\n"\
                " Start a NEW game? (Y/N)"

#define GAMESVD "There's already a game\n"\
                "saved at this position.\n"\
                "\n"\
                "    Overwrite? (Y/N)"

#define SENSITIVE   60

#define MENU_X      111
#define MENU_Y      50

#define SM_X        121
#define SM_Y        64
#define SM_W        54

#define CTL_X       100
#define CTL_Y       70

#define LSM_X       85
#define LSM_Y       55
#define LSM_W       144
#define LSM_H       ((10 * 13) + 10)

#define NM_X        71
#define NM_Y        66

#define NE_X        58
#define NE_Y        54

#define CST_X       77
#define CST_Y       60

#define CST_START   77
#define CST_SPC     41

#define LSA_X       96
#define LSA_Y       80
#define LSA_W       130
#define LSA_H       42

enum mm_labels
{
    MM_NEW_MISSION,
    MM_ORDERING_INFO,
    MM_READ_THIS,
    MM_BLAKE_STONE_SAGA,
    MM_BLANK1,
    MM_GAME_OPTIONS,
    MM_VIEW_SCORES,
    MM_LOAD_MISSION,
    MM_SAVE_MISSION,
    MM_BLANK2,
    MM_BACK_TO_DEMO,
    MM_LOGOFF,
};

//
// CP_Switch menu labels
//
enum sw_labels
{
    SW_LIGHTING,
    SW_REBA_ATTACK_INFO,
    SW_CEILING,
    SW_FLOORS,
};

//
// CP_Display menu labels
//
enum disp_labels
{
    DISP_VSYNC,
#ifdef NOTYET
    DISP_HWACCEL,
#endif
    DISP_FULLSCREEN,
    DISP_RATIO,
    DISP_RES,
    DISP_APPLY,
};

//
// flags for menu options (see CP_itemtype.active)
//
enum menuactivetypes
{
    AT_DISABLED,
    AT_ENABLED,
    AT_READIT,
    AT_NON_SELECTABLE,    // menu bar/separator
};


typedef struct
{
    byte   x;
    int8_t y_ofs;
    byte   width;
    byte   height;
    bool   on;
} CP_cursortype;

typedef struct
{
    byte          x;
    byte          y;
    byte          amount;
    int8_t        curpos;
    byte          indent;
    byte          y_spacing;
    CP_cursortype cursor;
} CP_iteminfo;


typedef struct
{
    byte active;
    char string[36];
    int  (*routine)(int);
    byte fontnumber;
    byte height;        // height of text (y offset from previous line)
} CP_itemtype;


typedef struct
{
    int16_t allowed[4];
} CustomCtrls;


enum inputtypes
{
    MOUSE,
    JOYSTICK,
    KEYBOARDBTNS,
    KEYBOARDMOVE,
};


extern  int         StartGame,SoundStatus;
extern  bool        SaveGamesAvail[MaxSaveGames];
extern  char        SaveGameNames[MaxSaveGames][GAME_DESCRIPTION_LEN + 1],savefilename[13];

extern  CP_itemtype MainMenu[],NewEMenu[];
extern  CP_iteminfo MainItems;

extern  int         lastmenumusic;
extern  bool        EscPressed;
extern  int         pickquick;

extern  int         mouseadjustment;


void     SetupControlPanel (void);
void     CleanupControlPanel (void);
void     ControlPanel (ScanCode scan);

void     DrawMenu (CP_iteminfo *item_i, CP_itemtype *items);
int      HandleMenu (CP_iteminfo *item_i, CP_itemtype *items, void (*routine)(int));
void     ClearMenuScreen (void);
void     ClearMenuBorders (void);
void     DrawWindow (int x, int y, int w, int h, int wcolor);
void     DrawOutline (int x, int y, int w, int h, int color1, int color2);
void     WaitKeyUp (void);
void     TicDelay (int count);
int      StartCPMusic (int song);
int      Confirm (const char *string);
void     Message (const char *string);
void     CheckPause (void);
void     ShootSnd (void);
void     CheckSecretMissions (void);
void     DrawInstructions (int type);
void     CacheMessage (int messagenum);
void     TerminateString (char *pos);

void     DrawCursor (CP_iteminfo *item_i, CP_itemtype *items, int *y, int which, int basey, void (*routine)(int));
void     EraseCursor (CP_iteminfo *item_i, CP_itemtype *items, int y, int which);
void     SetItemColor (CP_itemtype *items, bool hlight);
void     DrawMenuCursor (CP_iteminfo *iteminfo);

void     DefineMouseBtns (void);
void     DefineJoyBtns (void);
void     DefineKeyBtns (void);
void     DefineKeyMove (void);
void     EnterCtrlData (int index, CustomCtrls *cust, void (*DrawRtn)(int),void (*PrintRtn)(int), int type);

void     DrawMainMenu (void);
void     DrawSoundMenu (void);
void     DrawLoadSaveScreen (int loadsave);
void     DrawNewEpisode (void);
void     DrawNewGame (void);
void     DrawChangeView (int view);
void     DrawMouseSens (void);
void     DrawCtlScreen (void);
void     DrawCustomScreen (void);
void     DrawLSAction (int which);
void     DrawCustMouse (int hilight);
void     DrawCustJoy (int hilight);
void     DrawCustKeybd (int hilight);
void     DrawCustKeys (int hilight);
void     PrintCustMouse (int i);
void     PrintCustJoy (int i);
void     PrintCustKeybd (int i);
void     PrintCustKeys (int i);

void     PrintLSEntry (int w, int color);
void     TrackWhichGame (int w);
void     DrawNewGameDiff (int w);
void     FixupCustom (int w);

int      CP_BlakeStoneSaga (int blank);
int      CP_NewGame (int blank);
int      CP_Sound (int blank);
int      CP_LoadGame (int quick);
int      CP_SaveGame (int quick);
int      CP_Control (int blank);
int      CP_ChangeView (int blank);
int      CP_ExitOptions (int blank);
int      CP_Quit (int blank);
int      CP_ViewScores (int blank);
int      CP_EndGame (int blank);
int      CP_CheckQuick (int scan);
int      CustomControls (int blank);
int      MouseSensitivity (int blank);

void     DrawMenuTitle (const char *title);
void     CheckForEpisodes (void);
void     HelpPresenter (char *fname, bool continuekeys, int idcache, bool startmusic);
void     ShadowPrint (const char *string, int x, int y);

void     FreeMusic (void);
#endif
