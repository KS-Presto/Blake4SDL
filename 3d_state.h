// 3D_STATE.H

#ifndef _3D_STATE_H_
#define _3D_STATE_H_


extern  int      dirtoangle[NUMOBJDIRS];
extern  int      opposite[NUMOBJDIRS];
extern  int      diagonal[NUMOBJDIRS][NUMOBJDIRS];

extern  unsigned actor_points[];


void     SeekPlayerOrStatic (objtype *obj, fixed *deltax, fixed *deltay);
unsigned CheckRunChase (objtype *obj);
void     GetCornerSeek (objtype *obj);
bool     LookForGoodies (objtype *obj, unsigned RunReason);
objtype  *SpawnNewObj (int tilex, int tiley, statetype *state);
void     NewState (objtype *obj, statetype *state);

bool     TryWalk (objtype *obj, bool moveit);
void     SelectChaseDir (objtype *obj);
void     SelectDodgeDir (objtype *obj);
void     MoveObj (objtype *obj, fixed move);

void     KillActor (objtype *obj);
void     DamageActor (objtype *obj, int damage, objtype *attacker);

int      CalcAngle (objtype *from_obj, objtype *to_obj);

bool     CheckLine (objtype *from_obj, objtype *to_obj);
bool     CheckSight (objtype *from_obj, objtype *to_obj);

void     FirstSighting (objtype *obj);
bool     PosVisible (fixed from_x, fixed from_y, fixed to_x, fixed to_y, int from_angle);
bool     PlayerIsBlocking (objtype *obj);

void     MakeAlertNoise (objtype *obj);
objtype  *CheckAndReserve (void);

#endif
