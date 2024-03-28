// 3D_PLANE.C

#include "3d_def.h"

byte     *ceilingsource,*floorsource;

void     (*MapRowPtr) (void);

int      ceilingpic,floorpic;

int16_t  *spanstart;

unsigned mr_rowofs;
int      mr_count;
fixed    mr_xstep;
fixed    mr_ystep;
fixed    mr_xfrac;
fixed    mr_yfrac;
byte     *mr_dest;


/*
==============
=
= DrawSpans
=
= Height ranges from 0 (infinity) to centery (nearest)
=
==============
*/

void DrawSpans (int x1, int x2, int height)
{
    int   prestep;
    fixed basedist;;

    mr_count = x2 - x1;

    if (!mr_count)
        return;

    SetShading (height,0);

    prestep = centerx - x1;
    basedist = FixedDiv(scale,height + 1) >> 1;

    mr_dest = vbuf + ylookup[centery - 1 - height] + x1;
    mr_rowofs = ylookup[(height << 1) + 1];

    mr_xstep = (viewsin >> 1) / (height + 1);
    mr_ystep = (viewcos >> 1) / (height + 1);

    mr_xfrac = (viewx + FixedMul(basedist,viewcos)) - (mr_xstep * prestep);
    mr_yfrac = (viewy - FixedMul(basedist,viewsin)) - (mr_ystep * prestep);

    MapRowPtr ();
}


/*
===================
=
= LoadPlanes
=
===================
*/

void LoadPlanes (void)
{
    if (ceilingpic > 0)
        ceilingsource = PM_GetPage(ceilingpic);
    else
        ceilingsource = NULL;

    if (floorpic > 0)
        floorsource = PM_GetPage(floorpic);
    else
        floorsource = NULL;
}


/*
===================
=
= DrawPlanes
=
===================
*/

void DrawPlanes (void)
{
    int height;
    int x,y;

//
// loop over all columns
//
    y = centery;

    for (x = 0; x < viewwidth; x++)
    {
        height = wallheight[x] >> 3;

        if (height < y)
        {
            //
            // more starts
            //
            while (y > height)
                spanstart[--y] = x;
        }
        else if (height > y)
        {
            //
            // draw clipped spans
            //
            if (height > centery)
                height = centery;

            while (y < height)
            {
                if (y > 0)
                    DrawSpans (spanstart[y],x,y);

                y++;
            }
        }
    }

    //
    // draw spans
    //
    height = centery;

    while (y < height)
    {
        if (y > 0)
            DrawSpans (spanstart[y],x,y);

        y++;
    }
}


void MapRow (void)
{
    int texture;

    while (mr_count--)
    {
        texture = ((mr_xfrac >> 4) & 4032) + ((mr_yfrac >> 10) & 63);

        *mr_dest = shadingtable[ceilingsource[texture]];
        mr_dest[mr_rowofs] = shadingtable[floorsource[texture]];

        mr_dest++;
        mr_xfrac += mr_xstep;
        mr_yfrac += mr_ystep;
    }
}


void C_MapRow (void)
{
    int texture;

    while (mr_count--)
    {
        texture = ((mr_xfrac >> 4) & 4032) + ((mr_yfrac >> 10) & 63);

        *mr_dest++ = shadingtable[ceilingsource[texture]];

        mr_xfrac += mr_xstep;
        mr_yfrac += mr_ystep;
    }
}


void F_MapRow (void)
{
    int texture;

    while (mr_count--)
    {
        texture = ((mr_xfrac >> 4) & 4032) + ((mr_yfrac >> 10) & 63);

        mr_dest[mr_rowofs] = shadingtable[floorsource[texture]];

        mr_dest++;
        mr_xfrac += mr_xstep;
        mr_yfrac += mr_ystep;
    }
}
