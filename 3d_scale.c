// 3D_SCALE.C
//
// These functions are FULL of bugs in Blake;
// just use Wolf4SDL's stuff for now
//

#include "3d_def.h"


bool     useBounceOffset;
fixed    bounceOffset;


/*
===================
=
= DrawColumn
=
===================
*/

void DrawColumn (int count, int src, byte *dest)
{
    while (count--)
    {
        *dest = src;
        dest += screen.buffer->pitch;
    }
}


/*
===================
=
= DrawTranslucentColumn
=
= TODO: shading table needs intensifying?
=
===================
*/

void DrawTranslucentColumn (int count, byte *dest)
{
    while (count--)
    {
        *dest = shadingtable[*dest];
        dest += screen.buffer->pitch;
    }
}


/*
===================
=
= ScaleLine
=
= Reconstruct a sprite and draw it
=
= each vertical line of the shape has a pointer to segment data:
= 	end of segment pixel*2 (0 terminates line)
= 	top of virtual line with segment in proper place
=	start of segment pixel*2
=	<repeat>
=
= [top] must be cast to signed short because of the silly way it was
= calculated during sprite building
=
===================
*/

void ScaleLine (compline_t *sline, int ycenter)
{
    byte  *src,*dest;
    byte  *linecmds;
    int   count;
    int   start,end,top;
    int   startpix,endpix;
    fixed screenscale;

    linecmds = sline->commandtable;

    for (end = ReadShort(linecmds) / 2; end; end = ReadShort(linecmds) / 2)
    {
        top = (int16_t)ReadShort(linecmds + 2);
        start = ReadShort(linecmds + 4) / 2;

        screenscale = start * sline->stepscale;

        endpix = (screenscale >> FRACBITS) + ycenter;

        for (src = &sline->source[top + start]; start != end; start++, src++)
        {
            startpix = endpix;

            if (startpix >= viewheight)
                break;                          // off the bottom of the view area

            screenscale += sline->stepscale;
            endpix = (screenscale >> FRACBITS) + ycenter;

            if (endpix < 0)
                continue;                       // not into the view area

            if (startpix < 0)
                startpix = 0;                   // clip upper boundary

            if (endpix > viewheight)
                endpix = viewheight;            // clip lower boundary

            count = endpix - startpix;

            if (count)
            {
                dest = vbuf + ylookup[startpix] + sline->x;

                if (sline->cloaked)
                    DrawTranslucentColumn (count,dest);
                else
                    DrawColumn (count,shadingtable[*src],dest);
            }
        }

        linecmds += 6;                          // next segment list
    }
}


/*
===================
=
= ScaleShape
=
= Draws a compiled shape at [height] pixels high
=
===================
*/

void ScaleShape (visobj_t *sprite)
{
    int         i;
    compline_t  sline;
    compshape_t *shape;
    int         height,ycenter;
    int         x1,x2,xcenter;
    fixed       screenscale;

    height = sprite->viewheight >> 3;    // low three bits are fractional

    if (!height)
        return;                          // too close or far away

    sline.source = PM_GetSpritePage(sprite->shapenum);
    sline.stepscale = FixedDiv(height,TEXTURESIZE / 2);
    sline.cloaked = sprite->cloaked;

    SetShading (height,sprite->lighting);

    shape = (compshape_t *)sline.source;

    xcenter = sprite->viewx - height;
    ycenter = centery - height;

    screenscale = shape->leftpix * sline.stepscale;

    x2 = (screenscale >> FRACBITS) + xcenter;

    for (i = shape->leftpix; i <= shape->rightpix; i++)
    {
        //
        // calculate edges of the shape
        //
        x1 = x2;

        if (x1 >= viewwidth)
            break;                // off the right side of the view area

        screenscale += sline.stepscale;
        x2 = (screenscale >> FRACBITS) + xcenter;

        if (x2 < 0)
            continue;             // not into the view area

        if (x1 < 0)
            x1 = 0;               // clip left boundary

        if (x2 > viewwidth)
            x2 = viewwidth;       // clip right boundary

        sline.commandtable = &sline.source[shape->dataofs[i - shape->leftpix]];

        while (x1 < x2)
        {
            if (wallheight[x1] < sprite->viewheight)
            {
                sline.x = x1;

                ScaleLine (&sline,ycenter);
            }

            x1++;
        }
    }
}


/*
===================
=
= SimpleScaleShape
=
= NO CLIPPING, height in pixels
=
= Draws a compiled shape at [height] pixels high
=
===================
*/

void SimpleScaleShape (int dispx, int dispy, int dispheight, int shapenum, int shade)
{
    int         i;
    compline_t  sline;
    compshape_t *shape;
    int         height,ycenter;
    int         x1,x2,xcenter;
    fixed       screenscale;

    height = dispheight >> 1;

    sline.source = PM_GetSpritePage(shapenum);
    sline.stepscale = FixedDiv(height,TEXTURESIZE / 2);
    sline.cloaked = false;

    SetShading (height,shade);

    shape = (compshape_t *)sline.source;

    xcenter = dispx - height;
    ycenter = dispy - height;    // TODO: + bounceOffset / 2

    screenscale = shape->leftpix * sline.stepscale;

    x2 = (screenscale >> FRACBITS) + xcenter;

    for (i = shape->leftpix; i <= shape->rightpix; i++)
    {
        //
        // calculate edges of the shape
        //
        x1 = x2;

        screenscale += sline.stepscale;
        x2 = (screenscale >> FRACBITS) + xcenter;

        sline.commandtable = &sline.source[shape->dataofs[i - shape->leftpix]];

        while (x1 < x2)
        {
            sline.x = x1;

            ScaleLine (&sline,ycenter);

            x1++;
        }
    }
}


/*
===================
=
= MegaSimpleScaleShape
=
= Adjusts viewheight to draw anywhere on the screen
=
===================
*/

void MegaSimpleScaleShape (int dispx, int dispy, int dispheight, int shapenum, int shade)
{
    int oldviewheight = viewheight;

    viewheight = screen.height;

    SimpleScaleShape (dispx,dispy,dispheight,shapenum,shade);

    viewheight = oldviewheight;
}
