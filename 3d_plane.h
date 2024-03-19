// 3D_PLANE.H

#ifndef _3D_PLANE_H_
#define _3D_PLANE_H_


extern  int16_t     *spanstart;


void    LoadPlanes (int ceilingpic, int floorpic);
void    DrawPlanes (void);
void    MapRow (void);
void    C_MapRow (void);
void    F_MapRow (void);
void    (*MapRowPtr) (void);

#endif
