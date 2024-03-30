// 3D_ACT2.H

#ifndef _3D_ACT2_H_
#define _3D_ACT2_H_


#define SLIDE_TEMP(obj)     ((obj)->hitpoints)
#define BARRIER_STATE(obj)  ((obj)->ammo)

#define InitSmartAnim(obj,shapenum,startofs,maxofs,type,dir)      \
        InitSmartSpeedAnim(obj,shapenum,startofs,maxofs,type,dir,7)


extern  bool      barrierdamage;
extern  bool      noShots;
extern  int       morphwaittime;
extern  int       detonators_spawned;
extern  int       starthitpoints[NUMDIFFICULTY][NUMHITENEMIES];
extern  int       MorphClass[];
extern  unsigned  scanvalue;


void     T_PainThink (objtype *obj);
void     T_ExplodeScout (objtype *obj);
void     T_Security (objtype *obj);
void     T_ChangeShape (objtype *obj);
void     T_MakeOffset (objtype *obj);
void     T_LiquidStand (objtype *obj);
void     T_Hit (objtype *obj);
void     T_SpawnExplosion (objtype *obj);
void     T_ExplodeDamage (objtype *obj);
void     T_Seek (objtype *obj);

void     A_DeathScream (objtype *obj);

void     SpawnCusExplosion (fixed x, fixed y, int startframe, int numframes, int delay, int obclass);
void     SpawnProjectile (objtype *shooter, int obclass);
void     SpawnHiddenOfs (int which, int tilex, int tiley);
objtype  *SpawnOffsetObj (int which, int tilex, int tiley);
objtype  *SpawnStand (int which, int tilex, int tiley, int dir);
objtype  *SpawnPatrol (int which, int tilex, int tiley, int dir);
void     SpawnBarrier (int which, int tilex, int tiley, bool OnOff);

void     DoAttack (objtype *obj);
void     ChangeShootMode (objtype *obj);

void     InitSmartAnimStruct (objtype *obj, int shapenum, int startofs, int maxofs, int type, int dir);
void     InitAnim (objtype *obj, int shapenum, int startofs, int maxofs, int type, int dir, int delay, int waitdelay);
bool     AnimateOfsObj (objtype *obj);
void     AdvanceAnimREV (objtype *obj);
void     AdvanceAnimFWD (objtype *obj);

void     ExplodeRadius (objtype *obj, int damage, bool damageplayer);
void     ConnectBarriers (void);
void     ToggleBarrier (objtype *obj);
void     TurnPostOff (objtype *obj);

objtype  *FindObj (int which, int tilex, int tiley);
objtype  *FindHiddenOfs (int which);
objtype  *MoveHiddenOfs (int which_class, int new_class, fixed x, fixed y);

void     CheckForSpecialTile (objtype *obj, int tilex, int tiley);
void     DropCargo (objtype *obj);
void     InitGoldsternInfo (void);

#endif
