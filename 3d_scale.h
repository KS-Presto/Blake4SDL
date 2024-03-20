// 3D_SCALE.H

#ifndef _3D_SCALE_H_
#define _3D_SCALE_H_


typedef struct
{
    uint16_t leftpix,rightpix;
    uint16_t dataofs[1];    // table data after dataofs[rightpix - leftpix + 1]
} compshape_t;


typedef struct
{
    int16_t  x;
    fixed    stepscale;
    byte     *source;
    byte     *commandtable;
    bool     cloaked;
} compline_t;


extern  bool    useBounceOffset;


void    ScaleShape (visobj_t *sprite);
void    SimpleScaleShape (int dispx, int dispy, int dispheight, int shapenum, int shade);
void    MegaSimpleScaleShape (int dispx, int dispy, int dispheight, int shapenum, int shade);

#endif
