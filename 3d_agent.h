// 3D_AGENT.H

#ifndef _3D_AGENT_H_
#define _3D_AGENT_H_


#define MAX_DISPLAY_SCORE       9999999L


extern  scientist_t InfHintList;
extern  scientist_t NiceSciList;
extern  scientist_t MeanSciList;

extern  unsigned    static_points[];
extern  bool        GAN_HiddenArea;
extern  int         mapoffset[8];
extern  void        *InfAreaMsgs[];
extern  int         NumAreaMsgs,LastInfArea;
extern  int         FirstGenInfMsg,TotalGenInfMsgs;
extern  int         LastInfoAttacker;
extern  int         LastInfoAttacker_Cloaked;

extern  char        term_com_name[];
extern  char        term_msg_name[];

extern  atkinf_t    attackinfo[NUMWEAPONS][4];

extern  int         shootdelta;

//
// player state info
//
extern  bool        commandmode;
extern  fixed       thrustspeed;
extern  bool        PlayerInvisable;
extern  int         DrawInfoArea_COUNT;
extern  int         InitInfoArea_COUNT;

extern  int         player_oldtilex;
extern  int         player_oldtiley;

//
// terminal variables
//
extern  unsigned    RadarSw;

//
// location bar message string
//
extern  char        LocationText[MAX_LOCATION_DESC_LEN];

//
// search / move info
//
extern  unsigned    searchon;       // held object number looking at
extern  int         searchtics;     // when it reaches SEARCHTICS, get an obj
extern  objtype     *searchobj;     // current object being searched
extern  unsigned    foundflag;      // only show NOTHING if nothing was found
extern  objtype     *moveobj;       // current object being dragged

extern  int         anglefrac;
extern  int         facecount;

extern  objtype     *lastattacker;

extern  int         LastMsgType;
extern  unsigned    LastMsgPri;
extern  int         MsgTicsRemain;

extern  bool        useBounceOffset;
extern  fixed       bounceOffset;


void     GunAttack (objtype *obj);

void     GivePoints (int32_t score, bool add_to_stats);
void     SpawnPlayer (int tilex, int tiley, int dir);
void     DrawCash (void);
void     UpdateHealth (void);
void     UpdateRadarGauge (void);
void     UpdateAmmoMsg (void);

void     DrawAmmoGauge (void);
void     DrawAmmoMsg (void);
void     DrawWeapon (void);
void     DrawAmmo (bool ForceRefresh);
void     DrawHealth (void);
void     DrawKeys (void);
void     DrawScore (void);
void     InitInfoArea (void);
void     ForceUpdateStatusBar (void);
void     RedrawStatusAreas (void);
bool     DisplayInfoMsg (const char *Msg, int Priority, int DisplayTime, int MessageType);
void     UpdateInfoAreaClock (void);
void     UpdateInfoArea (void);
void     UpdateStatusBar (void);
void     DrawHealthMonitor (void);
void     CalcHealthDisplay (void);
void     UpdateScore (void);

void     GetBonus (statobj_t *check);

void     WriteTokenString (char *string);

unsigned ValidAreaTile (unsigned area);
unsigned GetAreaNumber (int tilex, int tiley);
int      InputFloor (void);

void     RestoreInfoArea (void);
void     DrawHeartPic (void);
void     DrawInfoArea (void);
int      DrawShape (int x, int y, int shapenum, int shapetype);

void     AnimatePage (void);

void     ActivateTerminal (bool skiplink);
void     TerminalPrint (const char *msg, bool FastPrint);
void     FloorCheat (unsigned RadarFlags);
bool     Interrogate (objtype *obj);

void     GiveWeapon (int weapon);
void     HealSelf (int points);
void     GiveKey (int key);
void     TakeKey (int key);
void     GiveToken (int tokens);

void     TakePlasmaDetonator (int count);
void     GivePlasmaDetonator (int count);

void     LoadTerminalCommands (void);

void     ActivateWallSwitch (unsigned iconnum, int x, int y);
unsigned UpdateBarrierTable (int x, int y, bool OnOff);
unsigned ScanBarrierTable (int x, int y);
void     DisplaySwitchOperateMsg (unsigned coords);

void     DisplayNoMoMsgs (void);
void     PrintStatPercent (int nx, int ny, int percentage);
int      ShowStats (int bx, int by, int type, statsInfoType *stats);
bool     PerfectStats (void);
bool     CheckPerfectStats (void);
bool     OperateSmartSwitch (int tilex, int tiley, int operation, bool force);

#endif
