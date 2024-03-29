// 3D_ACT1.H

#ifndef _3D_ACT1_H_
#define _3D_ACT1_H_


#define USE_TRANSFORMAREAS


enum doorpics
{
    // LOCKED DOORS

    L_METAL,
    L_METAL_SHADE,

    L_BIO,
    L_BIO_SHADE,

    L_ELEVATOR,
    L_ELEVATOR_SHADE,

    L_SPACE,
    L_SPACE_SHADE,

    L_PRISON,
    L_PRISON_SHADE,

    L_HIGH_SECURITY,
    L_HIGH_SECURITY_SHADE,

    L_ENTER_ONLY,
    L_ENTER_ONLY_SHADE,

    L_HIGH_TECH,
    L_HIGH_TECH_SHADE,

    // UNLOCKED DOORS

    UL_METAL,
    UL_METAL_SHADE,

    UL_BIO,
    UL_BIO_SHADE,

    UL_ELEVATOR,
    UL_ELEVATOR_SHADE,

    UL_SPACE,
    UL_SPACE_SHADE,

    UL_PRISON,
    UL_PRISON_SHADE,

    UL_HIGH_SECURITY,
    UL_HIGH_SECURITY_SHADE,

    UL_ENTER_ONLY,
    UL_ENTER_ONLY_SHADE,

    UL_HIGH_TECH,
    UL_HIGH_TECH_SHADE,

    // MISC DOORS

    NOEXIT,
    NOEXIT_SHADE,

    STEEL_JAM,
    STEEL_JAM_SHADE,

    SPACE_JAM,
    SPACE_JAM_SHADE,

    OFFICE_JAM,
    OFFICE_JAM_SHADE,

    BIO_JAM,
    BIO_JAM_SHADE,

    SPACE_JAM_2,
    SPACE_JAM_2_SHADE,

    NUMDOORPICS
};


enum doortypes
{   // NOTE - This enum list is ORDERED!
    dr_bio,
    dr_normal,
    dr_prison,
    dr_elevator,
    dr_high_security,
    dr_office,
    dr_oneway_left,
    dr_oneway_up,
    dr_oneway_right,
    dr_oneway_down,
    dr_space,

    NUMDOORTYPES
};


enum dooractions
{
    dr_open,
    dr_closed,
    dr_opening,
    dr_closing,
    dr_jammed,
};


//
// NOTE: When adding bonus objects, make sure that they are added at
// ----- the bottom of the list or that BonusMsg[] is correctly updated
//
enum stattypes
{
    dressing,

    bo_red_key,
    bo_yellow_key,
    bo_blue_key,

    bo_clip,
    bo_clip2,
    bo_pistol,
    bo_burst_rifle,
    bo_ion_cannon,
    bo_grenade,
    bo_bfg_cannon,

    // START of Bonus Health Ordered list

    bo_fullheal,
    bo_firstaid,
    bo_ham,
    bo_chicken,
    bo_sandwich,
    bo_candybar,
    bo_water,
    bo_water_puddle,

    // END of ordered ...

    bo_money_bag,
    bo_loot,

    bo_gold1,
    bo_gold2,
    bo_gold3,

    bo_gold,
    bo_bonus,

    bo_plainvent,
    bo_bloodvent,
    bo_watervent,

    bo_coin,
    bo_coin5,

    bo_plasma_detonator,
    bo_automapper1,

    bo_nothing,

    block,
};


typedef struct
{
    int16_t     picnum;
    int16_t     type;
} stattype;


typedef struct statstruct
{
    byte        tilex,tiley;
    byte        areanumber;
    int16_t     shapenum;        // if shapenum == -1 the obj has been removed
    uint16_t    flags;
    byte        itemnumber;
    byte        lighting;
} statobj_t;


typedef struct doorstruct
{
    byte        tilex,tiley;
    bool        vertical;
    int8_t      flags;
    int8_t      lock;
    byte        type;
    byte        action;
    int16_t     ticcount;
    uint16_t    position;        // leading edge of door (0 = closed, 0xffff = fully open)
    byte        areanumber[2];
} doorobj_t;


extern  int          xy_offset[8][2];
extern  stattype     statinfo[];
extern  concession_t ConHintList;

extern  statobj_t    statobjlist[MAXSTATS],*laststatobj;

extern  doorobj_t    doorobjlist[MAXDOORS],*lastdoorobj;
extern  int          doornum;

extern  byte         areaconnect[NUMAREAS][NUMAREAS];

extern  bool         areabyplayer[NUMAREAS];

extern  unsigned     pwallstate;
extern  unsigned     pwallpos;               // amount a pushable wall has been moved (0-63)
extern  unsigned     pwallx,pwally;
extern  int          pwalldir,pwalldist;


void      InitStatObjList (void);
statobj_t *ReserveStatic (void);
void      SpawnStatic (int tilex, int tiley, int type);
void      PlaceItemType (int itemtype, int tilex, int tiley);
void      PlaceItemNearTile (int itemtype, int tilex, int tiley);
void      InitAreas (void);
void      ConnectAreas (void);
void      InitDoorObjList (void);
void      SpawnDoor (int tilex, int tiley, bool vertical, int lock, int type);
void      OperateDoor (int door);
void      OpenDoor (int door);
void      CloseDoor (int door);
void      MoveDoors (void);
void      PushWall (int checkx, int checky, int dir);
void      MovePWalls (void);

void      OperateConcession (int concession);
void      SpawnConcession (int tilex, int tiley, int credits, int machinetype);
unsigned  LoadConcessionHint (unsigned MsgNum);
void      CacheInfoAreaMsg (unsigned block, unsigned MsgNum, const char *hint_buffer, unsigned MaxBufferLen);
void      CheckSpawnEA(void);

unsigned  TransformAreas (int tilex, int tiley, int xform);

void      CheckSpawnGoldstern (void);
void      FindNewGoldieSpawnSite (void);

void      DropPlasmaDetonator (void);
void      TryDropPlasmaDetonator (void);
void      BlockDoorOpen (int door);
void      BlastNearDoors (int tilex, int tiley);
void      TryBlastDoor (int door);

statobj_t *FindStatic (int tilex, int tiley);
statobj_t *UseReservedStatic (int itemtype, int tilex, int tiley);
void      PlaceReservedItemNearTile (int itemtype, int tilex, int tiley);
void      ExplodeStatics (int tilex, int tiley);

#endif
