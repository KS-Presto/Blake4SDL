// 3D_PLAY.H

#ifndef _3D_PLAY_H_
#define _3D_PLAY_H_


#define BASEMOVE        35
#define RUNMOVE         70
#define BASETURN        35
#define RUNTURN         70


enum objdirtype
{
    east,
    northeast,
    north,
    northwest,
    west,
    southwest,
    south,
    southeast,
    nodir,

    NUMOBJDIRS
};


enum objactivetype
{
    ac_badobject = -1,
    ac_no,
    ac_yes,
    ac_always,
};


extern  int         playstate;

extern  int         lastmusicoffset;

extern  int         musicnum;

extern  objtype     objlist[MAXACTORS],*lastobj;
extern  objtype     *player,*objfreelist,*lastattcker;
extern  objtype     dummyobj;

extern  objtype     *actorat[MAPSIZE][MAPSIZE];
extern  objtype     *deadguys[MAXACTORS];
extern  int         numdeadguys;

extern  int         objcount;

extern  int         bordertime;

extern  bool        madenoise,usedummy;
extern  unsigned    alerted,alertedarea;

extern  bool        singlestep,godmode;
extern  int         DebugOk,extravbls;
extern  int         InstantWin,InstantQuit;

extern  bool        PowerBall;
extern  bool        ShowQuickMsg;
extern  int         TestQuickSave,TestAutoMapper;
extern  unsigned    ExtraRadarFlags;

extern  int         tics,realtics;

//
// control info
//
extern  bool        mouseenabled,joystickenabled,freelookenabled;
extern  int         dirscan[4];
extern  int         buttonscan[NUMBUTTONS];
extern  int         buttonmouse[4];
extern  int         buttonjoy[32];

extern  bool        buttonheld[NUMBUTTONS];

//
// curent user input
//
extern  int         controlx,controly;          // range from -100 to 100
extern  bool        buttonstate[NUMBUTTONS];
extern  bool        refreshscreen;

extern  bool        demorecord,demoplayback;
extern  int8_t      *demoptr,*lastdemoptr;
extern  int8_t      *demobuffer;


void    InitObjList (void);
objtype *GetNewObj (void);
void    RemoveObj (objtype *gone);
void    DoActor (objtype *obj);
void    PollControls (void);
int     StopMusic (void);
void    StartMusic (void);
void    ContinueMusic (int offs);
void    InitPaletteShifts (void);
void    FinishPaletteShifts (void);
void    StartBonusFlash (void);
void    StartDamageFlash (int damage);
void    PlayLoop (void);

void    ChangeSwapFiles (bool display);
void    OpenPageFile (void);
void    CheckMusicToggle (void);
void    CheckSoundToggle (void);

#endif
