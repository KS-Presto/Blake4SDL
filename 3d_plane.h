// 3D_PLANE.H

#ifndef _3D_PLANE_H_
#define _3D_PLANE_H_


extern  int         ceilingpic,floorpic;

extern  int16_t     *spanstart;

extern  void        (*MapRowPtr) (void);


void    LoadPlanes (void);
void    DrawPlanes (void);
void    MapRow (void);
void    C_MapRow (void);
void    F_MapRow (void);

#endif
