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


enum objclasstype
{
    nothing,
    playerobj,
    inertobj,
    fixup_inertobj,
    deadobj,

    //
    // Start of ordered list for ActorInfoMsg[] for attacking actor REBA messages
    //
    rentacopobj,
    hang_terrotobj,
    gen_scientistobj,
    podobj,
    electroobj,
    electrosphereobj,
    proguardobj,
    genetic_guardobj,
    mutant_human1obj,
    mutant_human2obj,
    lcan_wait_alienobj,
    lcan_alienobj,
    scan_wait_alienobj,
    scan_alienobj,
    gurney_waitobj,
    gurneyobj,
    liquidobj,
    swatobj,
    goldsternobj,
    gold_morphobj,
    volatiletransportobj,
    floatingbombobj,
    rotating_cubeobj,

    spider_mutantobj,
    breather_beastobj,
    cyborg_warriorobj,
    reptilian_warriorobj,
    acid_dragonobj,
    mech_guardianobj,

    final_boss1obj,
    final_boss2obj,
    final_boss3obj,
    final_boss4obj,

    blakeobj,

    crate1obj,
    crate2obj,
    crate3obj,

    green_oozeobj,
    black_oozeobj,
    green2_oozeobj,
    black2_oozeobj,
    podeggobj,

    morphing_spider_mutantobj,
    morphing_reptilian_warriorobj,
    morphing_mutanthuman2obj,

    SPACER1_OBJ,

    electroshotobj,   // NON-HITPOINT objects
    post_barrierobj,
    arc_barrierobj,
    vpost_barrierobj,
    vspike_barrierobj,
    goldmorphshotobj,

    security_lightobj,
    explosionobj,
    steamgrateobj,
    steampipeobj,

    liquidshotobj,

    lcanshotobj,
    podshotobj,
    scanshotobj,
    dogshotobj,
    mut_hum1shotobj,

    ventdripobj,
    playerspshotobj,
    flickerlightobj,

    plasma_detonatorobj,
    plasma_detonator_reserveobj,

    grenadeobj,
    bfg_shotobj,
    bfg_explosionobj,
    pd_explosionobj,

    spider_mutantshotobj,
    breather_beastshotobj,
    cyborg_warriorshotobj,
    reptilian_warriorshotobj,
    acid_dragonshotobj,
    mech_guardianshotobj,
    final_boss2shotobj,
    final_boss4shotobj,

    doorexplodeobj,                 // door explosion_anim actor
    gr_explosionobj,
    gold_morphingobj,
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

extern  int         bordertime;

extern  bool        madenoise,usedummy,nevermark;
extern  unsigned    alerted,alertedareanum;

extern  unsigned    farmapylookup[MAPSIZE];

extern  bool        singlestep,godmode,noclip;
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
extern  bool        mouseenabled,joystickenabled,joypadenabled,joystickprogressive;
extern  int         joystickport;
extern  int         dirscan[4];
extern  int         buttonscan[NUMBUTTONS];
extern  int         buttonmouse[4];
extern  int         buttonjoy[4];

extern  bool        buttonheld[NUMBUTTONS];

//
// curent user input
//
extern  int         controlx,controly;          // range from -100 to 100
extern  bool        buttonstate[NUMBUTTONS];

extern  bool        demorecord,demoplayback;
extern  int8_t      *demoptr,*lastdemoptr;
extern  int8_t      *demobuffer;

extern  const char  *Computing;


void    InitObjList (void);
objtype *GetNewObj (void);
void    RemoveObj (objtype *gone);
void    DoActor (objtype *obj);
void    PollControls (void);
int     StopMusic (void);
void    StartMusic (void);
void    ContinueMusic (int offs);
void    PlayLoop (void);

void    ChangeSwapFiles (bool display);
void    OpenPageFile (void);
void    CheckMusicToggle (void);

#endif
