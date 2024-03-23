//
// Basic state stuff

#include "3d_def.h"


int   dirtoangle[NUMOBJDIRS] = {0,ANG45,ANG90,ANG135,ANG180,ANG225,ANG270,ANG315,0};


void KillActor (objtype *obj)
{
}

void DamageActor (objtype *obj, int damage, objtype *attacker)
{
}


/*
=====================
=
= CalcAngle
=
= Calculates angle from 1st object to 2nd object
=
=====================
*/

int CalcAngle (objtype *from_obj, objtype *to_obj)
{
    fixed deltax,deltay;
    float angle;
    int   iangle;

    deltax = to_obj->x - from_obj->x;
    deltay = from_obj->y - to_obj->y;

    if (!(deltax | deltay))
        return 1;

    angle = atan2(deltay,deltax);

    if (angle < 0)
        angle = (M_PI * 2) + angle;

    iangle = (angle / (M_PI * 2)) * ANG360;

    return iangle;
}


/*
=====================
=
= CheckLine
=
= Returns true if a straight line between two actors is unobstructed
=
=====================
*/

bool CheckLine (objtype *from_obj, objtype *to_obj)
{
    int      x1,y1,xt1,yt1,x2,y2,xt2,yt2;
    int      x,y;
    int      xdist,ydist,xstep,ystep;
    int      partial,delta;
    int32_t  ltemp;
    int      xfrac,yfrac,deltafrac;
    unsigned value,intercept;

    x1 = from_obj->x >> UNSIGNEDSHIFT;      // 1/256 tile precision
    y1 = from_obj->y >> UNSIGNEDSHIFT;
    xt1 = x1 >> UNSIGNEDSHIFT;
    yt1 = y1 >> UNSIGNEDSHIFT;

    x2 = to_obj->x >> UNSIGNEDSHIFT;
    y2 = to_obj->y >> UNSIGNEDSHIFT;
    xt2 = to_obj->tilex;    // TODO: why not x2/y2 >> UNSIGNEDSHIFT?
    yt2 = to_obj->tiley;


    xdist = abs(xt2 - xt1);

    if (xdist > 0)
    {
        if (xt2 > xt1)
        {
            partial = 256 - (x1 & 0xff);
            xstep = 1;
        }
        else
        {
            partial = x1 & 0xff;
            xstep = -1;
        }

        deltafrac = abs(x2 - x1);

        if (!deltafrac)
            deltafrac = 1;

        delta = y2 - y1;

        ltemp = ((int32_t)delta << UNSIGNEDSHIFT) / deltafrac;

        if (ltemp > 0x7fffl)
            ystep = 0x7fff;
        else if (ltemp < -0x7fffl)
            ystep = -0x7fff;
        else
            ystep = ltemp;

        yfrac = y1 + (((int32_t)ystep * partial) >> UNSIGNEDSHIFT);

        x = xt1 + xstep;
        xt2 += xstep;

        do
        {
            y = yfrac >> UNSIGNEDSHIFT;
            yfrac += ystep;

            value = tilemap[x][y];
            x += xstep;

            if (!value)
                continue;

            if (value < 128 || value > 256)
                return false;

            //
            // see if the door is open enough
            //
            // TODO: this never worked properly in Wolf, and it
            // doesn't even take into account Blake's trek doors
            //
            value &= ~0x80;
            intercept = yfrac - (ystep / 2);

            if (intercept > doorobjlist[value].position)
                return false;

        } while (x != xt2);
    }

    ydist = abs(yt2 - yt1);

    if (ydist > 0)
    {
        if (yt2 > yt1)
        {
            partial = 256 - (y1 & 0xff);
            ystep = 1;
        }
        else
        {
            partial = y1 & 0xff;
            ystep = -1;
        }

        deltafrac = abs(y2 - y1);

        if (!deltafrac)
            deltafrac = 1;

        delta = x2 - x1;
        ltemp = ((int32_t)delta << UNSIGNEDSHIFT) / deltafrac;

        if (ltemp > 0x7fffl)
            xstep = 0x7fff;
        else if (ltemp < -0x7fffl)
            xstep = -0x7fff;
        else
            xstep = ltemp;

        xfrac = x1 + (((int32_t)xstep * partial) >> UNSIGNEDSHIFT);

        y = yt1 + ystep;
        yt2 += ystep;

        do
        {
            x = xfrac >> UNSIGNEDSHIFT;
            xfrac += xstep;

            value = tilemap[x][y];
            y += ystep;

            if (!value)
                continue;

            if (value < 128 || value > 256)
                return false;

            //
            // see if the door is open enough
            //
            value &= ~0x80;
            intercept = xfrac - (xstep / 2);

            if (intercept > doorobjlist[value].position)
                return false;

        } while (y != yt2);
    }

    return true;
}


void FirstSighting (objtype *obj)
{
}
