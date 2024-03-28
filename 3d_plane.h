// 3D_PLANE.H

#ifndef _3D_PLANE_H_
#define _3D_PLANE_H_


extern  int         ceilingpic,floorpic;

extern  int16_t     *spanstart;


void    LoadPlanes (void);
void    DrawPlanes (void);
void    MapRow (void);
void    C_MapRow (void);
void    F_MapRow (void);
void    (*MapRowPtr) (void);

#endif
