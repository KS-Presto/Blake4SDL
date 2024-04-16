// 3D_DRAW.C

#include "3d_def.h"


#define DOORWALL  (PMSpriteStart - NUMDOORPICS)

#define ACTORSIZE 0x4000

byte        *vbuf;                          // screenbuffer->pixels pointer for each frame

visobj_t    visobjlist[MAXVISIBLE];

byte        tilemap[MAPSIZE][MAPSIZE];
bool        spotvis[MAPSIZE][MAPSIZE];
byte        TravelTable[MAPSIZE][MAPSIZE];

//
// player interface stuff
//
int         weaponchangetics,itemchangetics,bodychangetics;
int         plaqueon,plaquetime,getpic;

//star_t      *firststar,*laststar;

int32_t     lasttimecount;
int32_t     frameon;
int32_t     framecount;

int16_t     *wallheight;

//
// math tables
//
int16_t     *pixelangle;
int32_t     finetangent[DEG90];
fixed       sintable[ANG450],*costable;

//
// projection variables
//
fixed       focallength;                    // distance behind viewx/y for center of projection
int         viewwidth,viewheight;
int         baseviewwidth,baseviewheight;
int         centerx,centery;


//
// derived constants
//
fixed       scale;
fixed       heightnumerator;


//
// refresh variables
//
int         viewtilex,viewtiley;
fixed       viewx,viewy;        // the focal point
int         viewangle;
fixed       viewsin,viewcos;


//
// ray tracing variables
//
int         midangle;
unsigned    xpartial,ypartial;
unsigned    xpartialup,xpartialdown,ypartialup,ypartialdown;

int         tilehit;
int         pixx;

int         xtile,ytile;
int         xinttile,yinttile;
int         xtilestep,ytilestep;
fixed       xintercept,yintercept;
fixed       xstep,ystep;

byte        *postsource;
int         postx;
int         postheight;

byte        wallpics[MAXWALLTILES];

//
// light shading variables
//
int         normalshade;
int         normalshadediv = 1;
int         shademax = 36;

int         nsd_table[SHADEDIV] = { 1, 6, 3, 4, 1, 2};
int         sm_table[SHADEDIV] =  {36,51,62,63,18,52};

byte        *lightsource;
byte        *shadingtable;


void    BuildTables (void);
void    ClearScreen (void);
void    DrawScaleds (void);
void    CalcTics (void);
void    ThreeDRefresh (void);
void    DrawRadar (void);


/*
===================
=
= SetupWalls
=
= Map tile values to scaled pics
=
===================
*/

void SetupWalls (void)
{
    int i;

    for (i = 1; i < MAXWALLTILES; i++)
        wallpics[i] = (i - 1) * 2;
}


/*
========================
=
= Init3DRenderer
=
========================
*/

void Init3DRenderer (void)
{
    pixelangle = SafeMalloc(screen.width * sizeof(*pixelangle));
    wallheight = SafeMalloc(screen.width * sizeof(*wallheight));
    spanstart = SafeMalloc((screen.height >> 1) * sizeof(*spanstart));

    SetupWalls ();
    BuildTables ();
    InitPaletteShifts ();
    SetViewSize (viewsize);

    lightsource = colormap;
}


/*
========================
=
= SetShading
=
= Height in screen pixels (normally height >> 3)
=
========================
*/

void SetShading (int height, int lighting)
{
    int level;

    if (!(gamestate.flags & GS_LIGHTING))
        lighting = NO_SHADING;

    level = (shademax - ((SHADEMAX * height) / normalshade)) - lighting;

    if (level < 0)
        level = 0;
    else if (level > SHADEMAX)
        level = SHADEMAX;

    shadingtable = &lightsource[level << 8];
}


/*
========================
=
= Shutdown3DRenderer
=
========================
*/

void Shutdown3DRenderer (void)
{
    free (pixelangle);
    pixelangle = NULL;

    free (wallheight);
    wallheight = NULL;

    free (spanstart);
    spanstart = NULL;
}


/*
===================
=
= BuildTables
=
= Calculate fine tangents and sines
= costable overlays sintable with a quarter phase shift
=
===================
*/

void BuildTables (void)
{
    int   i;
    fixed value;

    for (i = 0; i < DEG90; i++)
    {
        value = (fixed)(tan(((double)i + 0.5) * (M_PI / DEG180)) * FRACUNIT);

        finetangent[i] = value;
    }

    for (i = 0; i < ANG90; i++)
    {
        value = (fixed)(sin((double)i * (M_PI / ANG180)) * FRACUNIT);

        sintable[i] = sintable[i + ANG360] = sintable[ANG180 - i] = value;
        sintable[ANG360 - i] = sintable[ANG180 + i] = -value;
    }

    sintable[ANG90] = FRACUNIT;
    sintable[ANG270] = -FRACUNIT;
    costable = &sintable[ANG90];
}


/*
===================
=
= CalcProjection
=
===================
*/

void CalcProjection (fixed focal)
{
    int    i;
    int    iangle;
    fixed  facedist;
    double fangle;
    double tang;

    focallength = focal;
    facedist = focal + MINDIST;

    //
    // calculate scale value for vertical height and sprite x calculations
    //
    scale = FixedMul(viewwidth,facedist);

    //
    // divide heightnumerator by a post's distance to get the post's height
    // The pixel height is height >> 2
    //
    heightnumerator = scale << 10;

    //
    // calculate the angle offset from view angle of each pixel's ray
    //
    for (i = 0; i < centerx; i++)
    {
        //
        // start 1/2 pixel over, so view angle bisects two middle pixels
        //
        tang = FixedDiv(i,viewwidth) / (double)facedist;
        fangle = atan(tang);
        iangle = (int)(fangle * (DEG180 / M_PI));
        pixelangle[centerx - 1 - i] = iangle;
        pixelangle[centerx + i] = -iangle;
    }
}


/*
========================
=
= TransformActor
=
= globals:
=   viewx,viewy     : point of view
=   viewcos,viewsin : sin/cos of viewangle
=   scale           : conversion from global value to screen value
=
= sets:
=   viewx,transx,viewheight: projected edge location and size
=
========================
*/

void TransformActor (fixed x, fixed y, int16_t *dispx, int16_t *dispheight, fixed *transx)
{
    fixed gx,gy,gxt,gyt,nx,ny;

//
// translate point to view centered coordinates
//
    gx = x - viewx;
    gy = y - viewy;

//
// calculate nx
//
    gxt = FixedMul(gx,viewcos);
    gyt = FixedMul(gy,viewsin);
    nx = gxt - gyt;

//
// fudge the shape forward a bit, because the midpoint
// could put parts of the shape into an adjacent wall
//
    nx -= ACTORSIZE;

//
// calculate ny
//
    gxt = FixedMul(gx,viewsin);
    gyt = FixedMul(gy,viewcos);
    ny = gyt + gxt;

//
// calculate perspective ratio
//
    if (transx)
        *transx = nx;

    if (nx < MINDIST)       // too close, don't overflow the divide
        *dispheight = 0;
    else
    {
        *dispx = (int)(centerx + ((ny * scale) / nx));
        *dispheight = (int)(heightnumerator / (nx >> 8));
    }
}


/*
========================
=
= TransformTile
=
= Takes paramaters:
=   tx,ty           : tile the object is centered in
=
= globals:
=   viewx,viewy     : point of view
=   viewcos,viewsin : sin/cos of viewangle
=   scale           : conversion from global value to screen value
=
= sets:
=   dispx,dispheight: projected edge location and size
=
= Returns true if the tile is withing getting distance
=
========================
*/

bool TransformTile (int tx, int ty, int16_t *dispx, int16_t *dispheight)
{
    fixed gx,gy,gxt,gyt,nx,ny;

//
// translate point to view centered coordinates
//
    gx = (((fixed)tx << TILESHIFT) + TILECENTER) - viewx;
    gy = (((fixed)ty << TILESHIFT) + TILECENTER) - viewy;

//
// calculate nx
//
    gxt = FixedMul(gx,viewcos);
    gyt = FixedMul(gy,viewsin);
    nx = gxt - gyt - 0x2000;        // 0x2000 is size of object

//
// calculate ny
//
    gxt = FixedMul(gx,viewsin);
    gyt = FixedMul(gy,viewcos);
    ny = gyt + gxt;


//
// calculate perspective ratio
//
    if (nx < MINDIST)       // too close, don't overflow the divide
        *dispheight = 0;
    else
    {
        *dispx = (int16_t)(centerx + ((ny * scale) / nx));
        *dispheight = (int16_t)(heightnumerator / (nx >> 8));
    }

//
// see if it should be grabbed
//
    if (nx < TILEGLOBAL && ny > -TILECENTER && ny < TILECENTER)
        return true;

    return false;
}


/*
====================
=
= CalcHeight
=
= Calculates the height of xintercept,yintercept from viewx,viewy
=
====================
*/

int CalcHeight (void)
{
    int   height;
    fixed gx,gy,gxt,gyt,nx;

//
// translate point to view centered coordinates
//
    gx = xintercept - viewx;
    gy = yintercept - viewy;

//
// calculate nx
//
    gxt = FixedMul(gx,viewcos);
    gyt = FixedMul(gy,viewsin);
    nx = gxt - gyt;

//
// calculate perspective ratio
//
    if (nx < MINDIST)
        nx = MINDIST;             // don't let divide overflow

    height = (int)(heightnumerator / (nx >> 8));

    if (height < 8)
        height = 8;

    return height;
}


/*
===================
=
= ScalePost
=
= Draws an unmasked post centered in the viewport
=
===================
*/

void ScalePost (void)
{
    byte     *src,*dest;
    int      srcofs;
    int      height;
    fixed    frac,fracstep;
    uint32_t doubPitch;
    uint32_t destofs;

    height = wallheight[postx] >> 3;

    if (!height)
        return;

    SetShading (height,0);

    fracstep = FixedDiv(TEXTURESIZE / 2,height);
    frac = fracstep >> 1;

    destofs = screen.buffer->pitch;
    doubPitch = screen.buffer->pitch << 1;

    src = &postsource[(TEXTURESIZE / 2) - 1];

    dest = vbuf + ylookup[centery - 1] + postx;

    if (height > centery)
        height = centery;

    while (height--)
    {
        srcofs = frac >> FRACBITS;

        //
        // write top half
        //
        *dest = shadingtable[src[-srcofs]];

        //
        // write bottom half
        //
        dest[destofs] = shadingtable[src[srcofs + 1]];

        dest -= screen.buffer->pitch;
        destofs += doubPitch;
        frac += fracstep;
    }
}


/*
===================
=
= ScaleMPost
=
= Draws a masked post centered in the viewport
=
===================
*/
#ifdef MASKABLE_DOORS
void ScaleMPost (void)
{
    byte     *src,*dest;
    int      srcofs,srcindex;
    int      height;
    fixed    frac,fracstep;
    uint32_t doubPitch;
    uint32_t destofs;

    height = wallheight[postx] >> 3;

    if (!height)
        return;

    SetShading (height,0);

    fracstep = FixedDiv(TEXTURESIZE / 2,height);
    frac = fracstep >> 1;

    destofs = screen.buffer->pitch;
    doubPitch = screen.buffer->pitch << 1;

    src = &postsource[(TEXTURESIZE / 2) - 1];

    dest = vbuf + ylookup[centery - 1] + postx;

    if (height > centery)
        height = centery;

    while (height--)
    {
        srcofs = frac >> FRACBITS;

        //
        // write top half
        //
        srcindex = src[-srcofs];

        if (srcindex != MASKCOLOR)
            *dest = shadingtable[src[-srcofs]];

        //
        // write bottom half
        //
        srcindex = src[srcofs + 1];

        if (srcindex != MASKCOLOR)
            dest[destofs] = shadingtable[src[srcofs + 1]];

        dest -= screen.buffer->pitch;
        destofs += doubPitch;
        frac += fracstep;
    }
}
#endif

/*
====================
=
= HitVertWall
=
= tilehit bit 7 is 0, because it's not a door tile
= if bit 6 is 1 and the adjacent tile is a door tile, use door side pic
=
====================
*/

unsigned DoorJams[] =
{
    BIO_JAM,     // dr_bio
    SPACE_JAM_2,   // dr_normal
    STEEL_JAM,    // dr_prison
    SPACE_JAM_2,   // dr_elevator
    STEEL_JAM,    // dr_high_sec
    OFFICE_JAM,    // dr_office
    STEEL_JAM,    // dr_oneway_left
    STEEL_JAM,    // dr_oneway_up
    STEEL_JAM,    // dr_oneway_right
    STEEL_JAM,    // dr_oneway_down
    SPACE_JAM,    // dr_space
};


void HitVertWall (void)
{
    int wallpage;
    int texture;
    int doornum;

    texture = (yintercept >> FIXED2TEXSHIFT) & TEXTUREMASK;

    if (xtilestep == -1)
    {
        texture ^= TEXTUREMASK;
        xintercept += TILEGLOBAL;
    }

    wallheight[pixx] = CalcHeight();

    postx = pixx;

    if (tilehit & 0x40)
    {
        //
        // check for adjacent doors
        //
        doornum = tilemap[xtile - xtilestep][yinttile];

        if (doornum & 0x80)
            wallpage = DOORWALL + DoorJams[doorobjlist[doornum & ~0x80].type];
        else
            wallpage = wallpics[tilehit & ~0x40];
    }
    else
        wallpage = wallpics[tilehit];

    postsource = PM_GetPage(wallpage + 1) + texture;

    ScalePost ();
}


/*
====================
=
= HitHorizWall
=
= tilehit bit 7 is 0, because it's not a door tile
= if bit 6 is 1 and the adjacent tile is a door tile, use door side pic
=
====================
*/

void HitHorizWall (void)
{
    int wallpage;
    int texture;
    int doornum;

    texture = (xintercept >> FIXED2TEXSHIFT) & TEXTUREMASK;

    if (ytilestep == -1)
        yintercept += TILEGLOBAL;
    else
        texture ^= TEXTUREMASK;

    wallheight[pixx] = CalcHeight();

    postx = pixx;

    if (tilehit & 0x40)
    {
        //
        // check for adjacent doors
        //
        doornum = tilemap[xinttile][ytile - ytilestep];

        if (doornum & 0x80)
            wallpage = DOORWALL + DoorJams[doorobjlist[doornum & ~0x80].type];
        else
            wallpage = wallpics[tilehit & ~0x40];
    }
    else
        wallpage = wallpics[tilehit];

    postsource = PM_GetPage(wallpage) + texture;

    ScalePost ();
}


/*
====================
=
= HitHorizDoor
=
====================
*/

void HitHorizDoor (void)
{
    int  texture;
    int  doorpage,doornum;
    bool lockable = true;

    doornum = tilehit & ~0x80;

    if (doorobjlist[doornum].action == dr_jammed)
        return;

#ifdef WOLFDOORS
    texture = ((xintercept - doorobjlist[doornum].position) >> FIXED2TEXSHIFT) & TEXTUREMASK;
#else
    if ((word)xintercept >= TILECENTER)
        texture = (((word)xintercept - (doorobjlist[doornum].position >> 1)) >> FIXED2TEXSHIFT) & TEXTUREMASK;
    else
        texture = (((word)xintercept + (doorobjlist[doornum].position >> 1)) >> FIXED2TEXSHIFT) & TEXTUREMASK;
#endif
    wallheight[pixx] = CalcHeight();

    postx = pixx;

    switch (doorobjlist[doornum].type)
    {
        case dr_normal:
            doorpage = L_METAL;
            break;

        case dr_elevator:
            doorpage = L_ELEVATOR;
            break;

        case dr_prison:
            doorpage = L_PRISON;
            break;

        case dr_space:
            doorpage = L_SPACE;
            break;

        case dr_bio:
            doorpage = L_BIO;
            break;

        case dr_high_security:
            doorpage = L_HIGH_SECURITY;          // Reverse View
            break;

        case dr_oneway_up:
        case dr_oneway_left:
            if (player->tiley > doorobjlist[doornum].tiley)
                doorpage = L_ENTER_ONLY;    // normal view
            else
            {
                doorpage = NOEXIT;           // Reverse View
                lockable = false;
            }
            break;

        case dr_oneway_right:
        case dr_oneway_down:
            if (player->tiley > doorobjlist[doornum].tiley)
            {
                doorpage = NOEXIT;      // normal view
                lockable = false;
            }
            else
                doorpage = L_ENTER_ONLY;   // Reverse View
            break;

        case dr_office:
            doorpage = L_HIGH_TECH;
            break;

        default:
            Quit ("Unknown door type!");
    }

    if (lockable && doorobjlist[doornum].lock == kt_none)
        doorpage += UL_METAL;

    postsource = PM_GetPage(DOORWALL + doorpage) + texture;
#ifdef MASKABLE_DOORS
    ScaleMPost ();
#else
    ScalePost ();
#endif
}


/*
====================
=
= HitVertDoor
=
====================
*/

void HitVertDoor (void)
{
    int  texture;
    int  doorpage,doornum;
    bool lockable = true;

    doornum = tilehit & ~0x80;

    if (doorobjlist[doornum].action == dr_jammed)
        return;

#ifdef WOLFDOORS
    texture = ((yintercept - doorobjlist[doornum].position) >> FIXED2TEXSHIFT) & TEXTUREMASK;
#else
    if ((word)yintercept >= TILECENTER)
        texture = (((word)yintercept - (doorobjlist[doornum].position >> 1)) >> FIXED2TEXSHIFT) & TEXTUREMASK;
    else
        texture = (((word)yintercept + (doorobjlist[doornum].position >> 1)) >> FIXED2TEXSHIFT) & TEXTUREMASK;
#endif
    wallheight[pixx] = CalcHeight();

    postx = pixx;

    switch (doorobjlist[doornum].type)
    {
        case dr_normal:
            doorpage = L_METAL;
            break;

        case dr_elevator:
            doorpage = L_ELEVATOR;
            break;

        case dr_prison:
            doorpage = L_PRISON;
            break;

        case dr_space:
            doorpage = L_SPACE;
            break;

        case dr_bio:
            doorpage = L_BIO;    // KS: L_BIO_SHADE was never used here even in AoG 1.0. bug or intended?
            break;

        case dr_high_security:
            doorpage = L_HIGH_SECURITY;
        break;

        case dr_oneway_left:
        case dr_oneway_up:
            if (player->tilex > doorobjlist[doornum].tilex)
                doorpage = L_ENTER_ONLY;   // Reverse View
            else
            {
                doorpage = NOEXIT;          // Normal view
                lockable = false;
            }
            break;

        case dr_oneway_right:
        case dr_oneway_down:
            if (player->tilex > doorobjlist[doornum].tilex)
            {
                doorpage = NOEXIT;         // Reverse View
                lockable = false;
            }
            else
                doorpage = L_ENTER_ONLY;  // Normal View
            break;

        case dr_office:
            doorpage = L_HIGH_TECH;
            break;

        default:
            Quit ("Unknown door type!");
    }

    if (lockable && doorobjlist[doornum].lock == kt_none)
        doorpage += UL_METAL;

    postsource = PM_GetPage(DOORWALL + doorpage + 1) + texture;
#ifdef MASKABLE_DOORS
    ScaleMPost ();
#else
    ScalePost ();
#endif
}


/*
====================
=
= HitHorizPWall
=
= A pushable wall in action has been hit
=
====================
*/

void HitHorizPWall (void)
{
    int wallpage;
    int texture,offset;

    texture = (xintercept >> FIXED2TEXSHIFT) & TEXTUREMASK;
    offset = pwallpos << 10;

    if (ytilestep == -1)
        yintercept += TILEGLOBAL - offset;
    else
    {
        texture ^= TEXTUREMASK;
        yintercept += offset;
    }

    wallheight[pixx] = CalcHeight();

    postx = pixx;

    wallpage = wallpics[tilehit & 63];

    postsource = PM_GetPage(wallpage) + texture;

    ScalePost ();
}


/*
====================
=
= HitVertPWall
=
= A pushable wall in action has been hit
=
====================
*/

void HitVertPWall (void)
{
    int wallpage;
    int texture,offset;

    texture = (yintercept >> FIXED2TEXSHIFT) & TEXTUREMASK;
    offset = pwallpos << 10;

    if (xtilestep == -1)
    {
        texture ^= TEXTUREMASK;
        xintercept += TILEGLOBAL - offset;
    }
    else
        xintercept += offset;

    wallheight[pixx] = CalcHeight();

    postx = pixx;

    wallpage = wallpics[tilehit & 63];

    postsource = PM_GetPage(wallpage + 1) + texture;

    ScalePost ();
}

#ifdef CEILING_FLOOR_COLORS
/*
=====================
=
= ClearScreen
=
= NOTE: Before calling this function - Check to see if there even needs
= ====  to be a solid floor or solid ceiling color drawn.
=
=====================
*/

void ClearScreen (void)
{
    int  y;
    byte *dest;

    if (!(gamestate.flags & GS_DRAW_CEILING))
    {
        dest = vbuf + ylookup[centery - 1];

        y = centery;

        while (y--)
        {
            memset (dest,ceilingcolor,viewwidth);
            dest -= screen.buffer->pitch;
        }
    }

    if (!(gamestate.flags & GS_DRAW_FLOOR))
    {
        dest = vbuf + ylookup[viewheight - 1];

        y = centery;

        while (y--)
        {
            memset (dest,floorcolor,viewwidth);
            dest -= screen.buffer->pitch;
        }
    }
}

#endif

/*
=====================
=
= CalcRotate
=
=====================
*/

int CalcRotate (objtype *obj, int viewx)
{
    int angle,viewangle;
    int dir = obj->dir;

    //
    // this isn't exactly correct, as it should vary by a trig value,
    // but it is close enough with only eight rotations
    //
    viewangle = player->angle + ((centerx - viewx) / ((8 * viewwidth) / screen.basewidth));

    if (dir == nodir)
        dir = obj->trydir & 127;

    angle = (viewangle - ANG180) - dirtoangle[dir];

    angle += ANG45 / 2;

    while (angle >= ANG360)
        angle -= ANG360;
    while (angle < 0)
        angle += ANG360;

    if (obj->state->flags & SF_PAIN)        // 2 rotation pain frame
        return 4 * (angle / ANG180);        // seperated by 3 (art layout...)

    return angle / ANG45;
}


/*
=====================
=
= DrawScaleds
=
= Draws all objects that are visible
=
=====================
*/

void DrawScaleds (void)
{
    int       i,least,numvisible,height;
    bool      *visspot;
    byte      *tilespot;
    statobj_t *statptr;
    objtype   *obj;
    visobj_t  *visptr,*visstep,*farthest;

    visptr = &visobjlist[0];

    //
    // place static objects
    //
    for (statptr = &statobjlist[0]; statptr != laststatobj; statptr++)
    {
        visptr->shapenum = statptr->shapenum;

        if (visptr->shapenum == -1)
            continue;               // object has been deleted

        if ((Keyboard[sc_6] && (Keyboard[sc_7] || Keyboard[sc_8]) && DebugOk) && (statptr->flags & FL_BONUS))
        {
            GetBonus (statptr);

            if (statptr->shapenum == -1)
                continue;
        }

        if (!spotvis[statptr->tilex][statptr->tiley])
            continue;               // not visible

        if (TransformTile(statptr->tilex,statptr->tiley,&visptr->viewx,&visptr->viewheight))
        {
            if (statptr->flags & FL_BONUS)
            {
                GetBonus (statptr);

                if (statptr->shapenum == -1)
                    continue;
            }
        }

        if (!visptr->viewheight)
            continue;               // too close to the object

        visptr->cloaked = false;
        visptr->lighting = statptr->lighting;   // could add additional flashing/lighting

        if (visptr < &visobjlist[MAXVISIBLE - 1])  // don't let it overflow
            visptr++;
    }

    //
    // place active objects
    //
    for (obj = player->next; obj; obj = obj->next)
    {
        visptr->shapenum = obj->state->shapenum;

        if (obj->flags & FL_OFFSET_STATES)
            visptr->shapenum += obj->temp1;

        if (!visptr->shapenum)
            continue;           // no shape

        if (!obj->tilex || !obj->tiley || obj->tilex >= mapwidth - 1 || obj->tiley >= mapheight - 1)
            continue;           // don't bother with actors on the map edges

        visspot = &spotvis[obj->tilex][obj->tiley];
        tilespot = &tilemap[obj->tilex][obj->tiley];

        //
        // obj could be in its tile or any of the eight surrounding tiles
        //
        // TODO: tilespot can make actors disappear in doorways, check
        // that removing it doesn't break anything
        //
        if (*visspot
        || (*(visspot - 1) && !*(tilespot - 1))
        || (*(visspot + 1) && !*(tilespot + 1))
        || (*(visspot - (mapwidth + 1)) && !*(tilespot - (mapwidth + 1)))
        || (*(visspot - mapwidth) && !*(tilespot - mapwidth))
        || (*(visspot - (mapwidth - 1)) && !*(tilespot - (mapwidth - 1)))
        || (*(visspot + (mapwidth + 1)) && !*(tilespot + (mapwidth + 1)))
        || (*(visspot + mapwidth) && !*(tilespot + mapwidth))
        || (*(visspot + (mapwidth - 1)) && !*(tilespot + (mapwidth - 1))))
        {
            obj->active = ac_yes;

            TransformActor (obj->x,obj->y,&visptr->viewx,&visptr->viewheight,NULL);

            if (!visptr->viewheight)
                continue;           // too close or far away

            if ((obj->flags2 & (FL2_CLOAKED | FL2_DAMAGE_CLOAK)) == FL2_CLOAKED)
            {
                visptr->cloaked = true;
                visptr->lighting = 0;
            }
            else
            {
                visptr->cloaked = false;
                visptr->lighting = obj->lighting;
            }

            if (!(obj->flags & FL_DEADGUY))
                obj->flags2 &= ~FL2_DAMAGE_CLOAK;

            if (visptr->shapenum == -1)
                visptr->shapenum = obj->temp1;      // special shape

            if (obj->state->flags & SF_ROTATE)
                visptr->shapenum += CalcRotate(obj,visptr->viewx);

            if (visptr < &visobjlist[MAXVISIBLE - 1])  // don't let it overflow
                visptr++;

            obj->flags |= FL_VISIBLE;
        }
        else
            obj->flags &= ~FL_VISIBLE;
    }

    //
    // draw from back to front
    //
    numvisible = (int)(visptr - &visobjlist[0]);

    if (!numvisible)
        return;         // no visible objects

    for (i = 0; i < numvisible; i++)
    {
        least = 0x7fff;

        for (visstep = &visobjlist[0]; visstep < visptr; visstep++)
        {
            height = visstep->viewheight;

            if (height < least)
            {
                least = height;
                farthest = visstep;
            }
        }

        //
        // draw farthest
        //
        ScaleShape (farthest);

        farthest->viewheight = 0x7fff;
    }
}


/*
==============
=
= DrawPlayerWeapon
=
= Draw the player's hands
=
==============
*/

int weaponscale[NUMWEAPONS] =
{
    SPR_KNIFEREADY,
    SPR_PISTOLREADY,
    SPR_MACHINEGUNREADY,
    SPR_CHAINREADY,
    SPR_GRENADEREADY,
    SPR_BFG_WEAPON1,
    0,
};


void DrawPlayerWeapon (void)
{
    int v_table[15] = {88,82,78,64,62,61,57,54,51,48,44,42,40,36,32};
    int c_table[15] = {88,85,81,80,75,70,64,59,55,50,44,39,34,28,24};
    int ycenter,vheight;
    int shapenum;

    if (playstate == ex_victorious)
        return;

    if (gamestate.weapon != -1)
    {
        shapenum = weaponscale[gamestate.weapon] + gamestate.weaponframe;

        if (shapenum)
        {
            useBounceOffset = true;
            ycenter = (c_table[20 - viewsize] + screen.heightoffset) * screen.scale;
            vheight = (v_table[20 - viewsize] + (screen.heightoffset * 2)) * screen.scale;

            SimpleScaleShape (centerx,ycenter,vheight,shapenum,NO_SHADING);

            useBounceOffset = false;
        }
    }
}


/*
=====================
=
= CalcTics
=
=====================
*/

void CalcTics (void)
{
    uint32_t curtime;

    //
    // calculate tics since last refresh for adaptive timing
    //
    if (lasttimecount > (int32_t)GetTimeCount())
        lasttimecount = GetTimeCount();    // if the game was paused a LONG time

    curtime = SDL_GetTicks();
    tics = (curtime * 7) / 100 - lasttimecount;

    if (!tics)
    {
        //
        // wait until end of current tic
        //
        SDL_Delay ((((lasttimecount + 1) * 100) / 7) - curtime);
        tics = 1;
    }

    lasttimecount += tics;

    if (tics > MAXTICS)
        tics = MAXTICS;
}


/*
====================
=
= DoorIntercept
=
= The trace hit the door plane at pixel position intercept,
= see if the door is closed that much
=
====================
*/

bool DoorIntercept (fixed intercept, unsigned position)
{
#ifdef WOLFDOORS
    if ((word)intercept < position)
        return false;
#else
    if ((word)intercept >= (TILECENTER - (position >> 1))
    && ((word)intercept < (TILECENTER + (position >> 1))))
        return false;
#endif
    return true;
}


/*
====================
=
= WallRefresh
=
====================
*/

void WallRefresh (void)
{
    int       angle;
    fixed     xinttemp,yinttemp;
    unsigned  xpartial,ypartial;
    doorobj_t *door;

    for (pixx = 0; pixx < viewwidth; pixx++)
    {
        //
        // setup to trace a ray through pixx view pixel
        //
        angle = midangle + pixelangle[pixx];                // delta for this pixel

        if (angle < 0)                                      // -90 - -1 degree arc
            angle += DEG360;                                // -90 is the same as 270
        if (angle >= DEG360)                                // 360-449 degree arc
            angle -= DEG360;                                // -449 is the same as 89

        //
        // setup xstep/ystep based on angle
        //
        if (angle < DEG90)                                  // 0-89 degree arc
        {
            xtilestep = 1;
            ytilestep = -1;
            xstep = finetangent[DEG90 - 1 - angle];
            ystep = -finetangent[angle];
            xpartial = xpartialup;
            ypartial = ypartialdown;
        }
        else if (angle < DEG180)                            // 90-179 degree arc
        {
            xtilestep = -1;
            ytilestep = -1;
            xstep = -finetangent[angle - DEG90];
            ystep = -finetangent[DEG180 - 1 - angle];
            xpartial = xpartialdown;
            ypartial = ypartialdown;
        }
        else if (angle < DEG270)                            // 180-269 degree arc
        {
            xtilestep = -1;
            ytilestep = 1;
            xstep = -finetangent[DEG270 - 1 - angle];
            ystep = finetangent[angle - DEG180];
            xpartial = xpartialdown;
            ypartial = ypartialup;
        }
        else if (angle < DEG360)                            // 270-359 degree arc
        {
            xtilestep = 1;
            ytilestep = 1;
            xstep = finetangent[angle - DEG270];
            ystep = finetangent[DEG360 - 1 - angle];
            xpartial = xpartialup;
            ypartial = ypartialup;
        }

        //
        // initialize variables for intersection testing
        //
        yintercept = FixedMul(ystep,xpartial) + viewy;
        yinttile = yintercept >> TILESHIFT;
        xtile = viewtilex + xtilestep;

        xintercept = FixedMul(xstep,ypartial) + viewx;
        xinttile = xintercept >> TILESHIFT;
        ytile = viewtiley + ytilestep;

//
// trace along this angle until we hit a wall
//
// CORE LOOP!
//
        while (1)
        {
            //
            // check intersections with vertical walls
            //
            if ((xtile - xtilestep) == xinttile && (ytile - ytilestep) == ytile)
                yinttile = ytile;

            if ((ytilestep == -1 && yinttile <= ytile) || (ytilestep == 1 && yinttile >= ytile))
                goto horizentry;
vertentry:
            tilehit = tilemap[xtile][yinttile];

            if (tilehit)
            {
                if (tilehit & 0x80)
                {
                    if ((tilehit & 0xc0) == 0xc0)
                    {
                        //
                        // hit a sliding vertical wall
                        //
                        // add partial step to current intercept
                        //
                        yinttemp = yintercept + FixedMul(ystep,pwallpos << 10);

                        if (yinttemp >> TILESHIFT == yinttile)
                        {
                            //
                            // still in the same tile
                            //
                            yintercept = yinttemp;
                            xintercept = (fixed)xtile << TILESHIFT;

                            HitVertPWall ();

                            break;
                        }
                    }
                    else
                    {
                        //
                        // hit a vertical door, so find which coordinate the door would be
                        // intersected at, and check to see if the door is open past that point
                        //
                        door = &doorobjlist[tilehit & ~0x80];

                        if (door->action != dr_open)
                        {
                            yinttemp = yintercept + (ystep >> 1);    // add halfstep to current intercept position

                            if (yinttemp >> TILESHIFT == yinttile)
                            {
                                //
                                // still in the same tile, see if it hit the door plane
                                //
                                if (DoorIntercept(yinttemp,door->position))
                                {
                                    yintercept = yinttemp;
                                    xintercept = ((fixed)xtile << TILESHIFT) + TILECENTER;

                                    HitVertDoor ();

                                    break;
                                }
                            }
                        }
                    }
                }
                else
                {
                    xintercept = (fixed)xtile << TILESHIFT;

                    HitVertWall ();

                    break;
                }
            }

            //
            // mark the tile as visible and setup for next step
            //
            spotvis[xtile][yinttile] = true;
            xtile += xtilestep;
            yintercept += ystep;
            yinttile = yintercept >> TILESHIFT;
        }

        continue;

        while (1)
        {
            //
            // check intersections with horizontal walls
            //
            if ((xtile - xtilestep) == xinttile && (ytile - ytilestep) == yinttile)
                xinttile = xtile;

            if ((xtilestep == -1 && xinttile <= xtile) || (xtilestep == 1 && xinttile >= xtile))
                goto vertentry;

horizentry:
            tilehit = tilemap[xinttile][ytile];

            if (tilehit)
            {
                if (tilehit & 0x80)
                {
                    if ((tilehit & 0xc0) == 0xc0)
                    {
                        //
                        // hit a sliding horizontal wall
                        //
                        // add partial step to current intercept
                        //
                        xinttemp = xintercept + FixedMul(xstep,pwallpos << 10);

                        if (xinttemp >> TILESHIFT == xinttile)
                        {
                            //
                            // still in the same tile
                            //
                            xintercept = xinttemp;
                            yintercept = (fixed)ytile << TILESHIFT;

                            HitHorizPWall ();

                            break;
                        }
                    }
                    else
                    {
                        //
                        // hit a horizontal door, so find which coordinate the door would be
                        // intersected at, and check to see if the door is open past that point
                        //
                        door = &doorobjlist[tilehit & ~0x80];

                        if (door->action != dr_open)
                        {
                            xinttemp = xintercept + (xstep >> 1);    // add half step to current intercept position

                            if (xinttemp >> TILESHIFT == xinttile)
                            {
                                //
                                // still in the same time, see if it hit the door plane
                                //
                                if (DoorIntercept(xinttemp,door->position))
                                {
                                    xintercept = xinttemp;
                                    yintercept = ((fixed)ytile << TILESHIFT) + TILECENTER;

                                    HitHorizDoor ();

                                    break;
                                }
                            }
                        }
                    }
                }
                else
                {
                    yintercept = (fixed)ytile << TILESHIFT;

                    HitHorizWall ();

                    break;
                }
            }

            //
            // mark the tile as visible and setup for next step
            //
            spotvis[xinttile][ytile] = true;
            ytile += ytilestep;
            xintercept += xstep;
            xinttile = xintercept >> TILESHIFT;
        }
    }
}


/*
====================
=
= WallRefresh
=
====================
*/

void Setup3DView (void)
{
    viewangle = player->angle;
    midangle = viewangle * (DEG360 / ANG360);

    viewsin = sintable[viewangle];
    viewcos = costable[viewangle];

    viewx = player->x - FixedMul(focallength,viewcos);
    viewy = player->y + FixedMul(focallength,viewsin);

    viewtilex = viewx >> TILESHIFT;
    viewtiley = viewy >> TILESHIFT;

    xpartialdown = viewx & (TILEGLOBAL - 1);
    xpartialup = xpartialdown ^ (TILEGLOBAL - 1);
    ypartialdown = viewy & (TILEGLOBAL - 1);
    ypartialup = ypartialdown ^ (TILEGLOBAL - 1);
}


/*
========================
=
= ThreeDRefresh
=
========================
*/

void ThreeDRefresh (void)
{
//
// clear out the traced array
//
    memset (spotvis,false,sizeof(spotvis));
    spotvis[player->tilex][player->tiley] = true;

//
// call any functions that may draw to a full screen
//
    UpdateInfoAreaClock ();
    UpdateStatusBar ();

    if (screen.bufferofs)
        Quit ("Screen buffer offset must be 0 while 3D rendering!");

//
// get the screenbuffer pointer and offset it to draw to the viewport
//
    vbuf = VW_LockSurface(screen.buffer);

    if (!vbuf)
        Quit ("Unable to lock surface: %s\n",SDL_GetError());

    vbuf += viewscreenofs;

    Setup3DView ();

#ifdef CEILING_FLOOR_COLORS
    switch (gamestate.flags & (GS_DRAW_FLOOR | GS_DRAW_CEILING))
    {
        case GS_DRAW_FLOOR | GS_DRAW_CEILING:
            MapRowPtr = MapRow;
            WallRefresh ();
            DrawPlanes ();
            break;

        case GS_DRAW_FLOOR:
            MapRowPtr = F_MapRow;
            ClearScreen ();
            WallRefresh ();
            DrawPlanes ();
            break;

        case GS_DRAW_CEILING:
            MapRowPtr = C_MapRow;
            ClearScreen ();
            WallRefresh ();
            DrawPlanes ();
            break;

        default:
            ClearScreen ();
            WallRefresh ();
            break;
    }
#else
    MapRowPtr = MapRow;
    WallRefresh ();
    DrawPlanes ();
#endif
    UpdateTravelTable ();

//
// draw all the scaled images
//
    DrawScaleds ();      // draw scaled stuff

    DrawPlayerWeapon (); // draw player's hands

    DrawRadar ();

//
// show screen and time last cycle
//
    VW_UnlockSurface (screen.buffer);
    vbuf = NULL;

    if (screen.flags & SC_FIZZLEIN)
    {
        VW_FizzleFade (fizzlex,fizzley,fizzlewidth,fizzleheight,70,false);

        screen.flags &= ~SC_FIZZLEIN;
        fizzlex = fizzley = 0;
        fizzlewidth = baseviewwidth;
        fizzleheight = baseviewheight;

        lasttimecount = GetTimeCount();  // don't make a big tic count
    }
    else if (screen.flags & SC_FADED)
        VW_FadeIn ();
    else
        VW_UpdateScreen (screen.buffer);

    frameon += tics;
}


/*
========================
=
= UpdateTravelTable
=
========================
*/

void UpdateTravelTable (void)
{
    int x,y;

    for (y = 0; y < mapheight; y++)
    {
        for (x = 0; x < mapwidth; x++)
            TravelTable[x][y] |= spotvis[x][y];
    }
}


/*
========================
=
= DrawRadar
=
========================
*/

void DrawRadar (void)
{
    int      zoom;
    unsigned flags = OV_KEYS | OV_PUSHWALLS | OV_ACTORS;

    if (gamestate.rpower)
    {
        zoom = gamestate.rzoom;

        if ((frameon & 1) && !godmode)
        {
            if (zoom)
                gamestate.rpower -= tics << zoom;
        }

        if (gamestate.rpower <= 0)
        {
            gamestate.rpower = 0;
            DISPLAY_TIMED_MSG (RadarEnergyGoneMsg,MP_WEAPON_AVAIL,MT_GENERAL);
        }

        UpdateRadarGauge ();
    }
    else
        zoom = 0;

    ShowOverhead (192 - baseviewscreenx,(screen.baseheight - 44) - baseviewscreeny,16,zoom,flags);
}


/*
========================
=
= ShowOverhead
=
========================
*/

void ShowOverhead (int bx, int by, int radius, int zoom, unsigned flags)
{
    #define PLAYER_COLOR    0xf1
    #define UNMAPPED_COLOR  0x52
    #define MAPPED_COLOR    0x55

    byte     color;
    byte     tile,door;
    byte     *dest,*basedest;
    objtype  *obj;
    unsigned iconnum;
    fixed    lmx,lmy,baselmx,baselmy;
    fixed    psin,pcos;
    int      zoomwidth;
    int      rx,ry,mx,my;
    int      i,rndindex;
    bool     drawplayerok = true;
    bool     snow = false;

//
// -zoom == make it snow!
//
    if (zoom < 0)
    {
        zoom = 0;
        snow = true;
        rndindex = US_RndT();
    }

    zoom = 1 << zoom;
    zoomwidth = zoom;
    radius /= zoom;

//
// convert radius to fixed integer and calc rotation
//
    psin = sintable[player->angle];
    pcos = costable[player->angle];

    baselmx = player->x + ((radius * pcos) - (radius * psin));
    baselmy = player->y - ((radius * psin) + (radius * pcos));

//
// calculate starting destination address
//
    if (screen.scale > 1)
    {
        bx *= screen.scale;
        by *= screen.scale;
        zoomwidth *= screen.scale;
    }

    basedest = vbuf + ylookup[by] + bx;

//
// draw rotated radar
//
    rx = radius * 2;

    while (rx--)
    {
        lmx = baselmx;
        lmy = baselmy;

        dest = basedest;

        ry = radius * 2;

        while (ry--)
        {
            if (snow)
                color = 0x42 + (rndtable[rndindex++ & 0xff] & 3);
            else
            {
                color = UNMAPPED_COLOR;

                //
                // don't evaluate if point is outside of map
                //
                mx = lmx >> TILESHIFT;
                my = lmy >> TILESHIFT;

                if (mx >= 0 && mx <= (mapwidth - 1) && my >= 0 && my <= (mapheight - 1))
                {
                    if (drawplayerok && player->tilex == mx && player->tiley == my)
                    {
                        //
                        // show player
                        //
                        color = PLAYER_COLOR;
                        drawplayerok = false;
                    }
                    else
                    {
                        //
                        // show traveled
                        //
                        if ((TravelTable[mx][my] & TT_TRAVELED) || (flags & OV_SHOWALL))
                        {
                            //
                            // what's at this map location?
                            //
                            tile = tilemap[mx][my];
                            door = tile & 0x3f;

                            //
                            // evaluate wall or floor?
                            //
                            if (tile)
                            {
                                //
                                // show doors
                                //
                                if (tile & 0x80)
                                {
                                    if ((tile & 0xc0) == 0xc0)
                                        color = MAPPED_COLOR;
                                    else
                                    {
                                        if (doorobjlist[door].lock != kt_none)
                                            color = 0x18;                   // locked!
                                        else
                                        {
                                            if (doorobjlist[door].action == dr_closed)
                                                color = 0x58;               // closed!
                                            else
                                                color = MAPPED_COLOR;       // floor!
                                        }
                                    }
                                }
                            }
                            else
                                color = MAPPED_COLOR;                   // floor!

                            //
                            // show keys
                            //
                            if ((flags & OV_KEYS) && (TravelTable[mx][my] & TT_KEYS))
                                color = 0xf3;

                            if (zoom > 1 || (ExtraRadarFlags & OV_ACTORS))
                            {
                                obj = actorat[mx][my];

                                //
                                // show actors
                                //
                                if ((flags & OV_ACTORS) && ISPOINTER(obj) && !(obj->flags & FL_DEADGUY)
                                 && obj->obclass > deadobj && obj->obclass < SPACER1_OBJ)
                                    color = 0x10 + obj->obclass;
                            }

                            if ((zoom == 4 || (ExtraRadarFlags & OV_PUSHWALLS)) && !(TravelTable[mx][my] & TT_PWALLMOVED))
                            {
                                iconnum = MAPSPOT(mx,my,1);

                                //
                                // show pushwalls
                                //
                                if ((flags & OV_PUSHWALLS) && iconnum == PUSHABLETILE)
                                    color = 0x79;
                            }
                        }
                        else
                            color = UNMAPPED_COLOR;
                    }
                }
            }

            //
            // display pixels for this quadrant and add x/y increments
            //
            for (i = 0; i < zoomwidth; i++)
            {
                memset (dest,color,zoomwidth);

                dest += screen.buffer->pitch;
            }

            lmx -= pcos;
            lmy += psin;
        }

        baselmx += psin;
        baselmy += pcos;

        basedest += zoomwidth;
    }
}
