// 3D_GAME.H

#ifndef _3D_GAME_H_
#define _3D_GAME_H_


#define PlaySoundLocActor(obj)  PlaySoundLocGlobal((s),(obj)->x,(obj)->y)
#define PlaySoundLocTile(s,x,y) PlaySoundLocGlobal ((s),((fixed)(x) << TILESHIFT) + TILECENTER,((fixed)(y) << TILESHIFT) + TILECENTER)


extern  int             db_count;
extern  int             debug_bonus[2][800];
extern  fargametype     gamestuff;
extern  tilecoord_t     GoldieList[GOLDIE_MAX_SPAWNS];
extern  GoldsternInfo_t GoldsternInfo;

extern  int             VitalsRemain,VitalsOnFloor;
extern  unsigned        TopColor,BottomColor;

extern  eaWallInfo      eaList[];
extern  int             NumEAWalls,NumEASpawned;
extern  bool            startgame,ingame,loadedgame;
extern  gametype        gamestate;
extern  int             viewsize;
extern  int             viewscreenx,viewscreeny;
extern  unsigned        screenofs;
extern  int             mapwidth,mapheight;

extern  char            demoname[13];


void    NewGame (int difficulty, int episode);
void    DrawPlayBorder (void);
void    ScanInfoPlane (void);
void    UpdateSoundLoc (void);
void    SetupGameLevel (void);
void    NormalScreen (void);
void    DrawPlayScreen (bool InitInfoMsg);
void    DrawTopInfo (int type);
void    SetViewSize (int size);
void    NewViewSize (int width);
void    FizzleOut (void);
void    GameLoop (void);

void    PlaySoundLocGlobal (int sound, fixed gx, fixed gy);

void    Warped (void);
void    RotateView (int DestAngle, unsigned RotSpeed);
void    DrawWarpIn (void);
void    BMAmsg (const char *msg);
void    CacheBMAmsg (unsigned MsgNum);
void    BevelBox (int xl, int yl, int w, int h, int hi, int med, int lo);

void    AddTotalPoints (unsigned points);
void    AddTotalInformants (int informants);
void    AddTotalEnemy (unsigned enemies);

void    SetFontColor (int f, int b);
void    ShadowPrintLocationText (int type);
void    LoseScreen (void);
void    LoadLocationText (int textnum);

#endif
