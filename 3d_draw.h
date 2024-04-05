// 3D_DRAW.H

#ifndef _3D_DRAW_H_
#define _3D_DRAW_H_


#define NO_SHADING          128
#define SHOOT_SHADING       10
#define LAMP_ON_SHADING     12
#define EXPLOSION_SHADING   52

#define SHADEMAX            63
#define SHADEDIV            6


typedef struct
{
    int16_t viewx;
    int16_t viewheight;
    int16_t shapenum;
    byte    lighting;
    bool    cloaked;
} visobj_t;

extern  visobj_t    visobjlist[MAXVISIBLE];

extern  byte        *vbuf;

extern  byte        tilemap[MAPSIZE][MAPSIZE];
extern  bool        spotvis[MAPSIZE][MAPSIZE];
extern  byte        TravelTable[MAPSIZE][MAPSIZE];

extern  int         weaponchangetics,itemchangetics,bodychangetics;
extern  int         plaqueon,plaquetime,plaquetimefrac,getpic;

extern  statobj_t   *firststarobj;

extern  int32_t     space_xmove,space_ymove;

extern  int32_t     lasttimecount;
extern  int32_t     framecount;
extern  int32_t     frameon;

extern  int16_t     *wallheight;

//
// projection variables
//
extern  fixed       focallength;
extern  int         viewwidth,viewheight;
extern  int         baseviewwidth,baseviewheight;
extern  int         centerx,centery;

//
// math tables
//
extern  int16_t     *pixelangle;
extern  int32_t     finetangent[DEG90];
extern  fixed       sintable[ANG450],*costable;

//
// derived constants
//
extern  fixed       scale;
extern  int32_t     heightnumerator;

//
// refresh variables
//
extern  fixed       viewx,viewy;                    // the focal point
extern  int         viewangle;
extern  fixed       viewsin,viewcos;

extern  byte        *postsource;
extern  int         postx;
extern  int         postheight;

//
// light shading variables
//
extern  int         normalshade;
extern  int         normalshadediv;
extern  int         shademax;

extern  int         nsd_table[SHADEDIV];
extern  int         sm_table[SHADEDIV];

extern  byte        *lightsource;
extern  byte        *shadingtable;


void    Init3DRenderer (void);
void    Shutdown3DRenderer (void);
void    SetupWalls (void);
void    BuildTables (void);
void    CalcProjection (fixed focal);
void    ClearScreen (void);
void    SetShading (int height, int lighting);
void    DrawScaleds (void);
void    CalcTics (void);
void    ThreeDRefresh (void);
void    ScalePost (void);
void    DrawStars (void);
void    TransformActor (fixed x, fixed y, int16_t *dispx, int16_t *dispheight, fixed *transx);
bool    TransformTile (int tx, int ty, int16_t *dispx, int16_t *dispheight);
void    ShowOverhead (int bx, int by, int radius, int zoom, unsigned flags);
void    UpdateTravelTable (void);

#endif
