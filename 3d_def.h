// 3D_DEF.H

#ifndef _3D_DEF_H_
#define _3D_DEF_H_


#include "id_heads.h"

#ifdef NOTYET
#include "jm_cio.h"
#endif

//#define DEMOS_EXTERN
//#define MYPROFILE
//#define TRACK_ENEMY_COUNT
#define OBJ_RESERV

#define GOLD_MORPH_LEVEL      19   // level which Dr. GoldFire Morphs

#define VERSION_TEXT_COLOR    (0x82)
#define VERSION_TEXT_BKCOLOR  (0x80)

#define NO_SHADING          128
#define LAMP_ON_SHADING     12
#define EXPLOSION_SHADING   52

#define BETA_TEST           false
#define LOOK_FOR_DEAD_GUYS  false

#define BETA_CODE           "NEWGAME"

#define LIMITED_AMMO

#define PLAYTEMP_FILE       "PLAYTEMP"
#define OLD_PLAYTEMP_FILE   "OLDPTEMP"

#define BORDER_HI_COLOR     0x85
#define BORDER_MED_COLOR    0x82
#define BORDER_LO_COLOR     0x80
#define BORDER_TEXT_COLOR   0xaf

//
// FONT DEFINES
//
#define ID_FONT         (STARTFONT)
#define BIG_FONT        (STARTFONT + 1)
#define SIXPOINT_FONT   (STARTFONT + 2)
#define TALL_FONT       (STARTFONT + 3)
#define COAL_FONT       (STARTFONT + 4)
#define RADAR_FONT      (STARTFONT + 5)

//#define LOCKED_FLOORS

#define ID_CACHE_BRIEFS
#define ID_CACHE_HELP
#define ID_CACHE_LOSE
#define ID_CACHE_CREDITS


//#define DEBUG_STATICS
//#define DEBUG_ACTORS

/*
=============================================================================

                        GAME VERSION DEPENDANT DEFINATIONS

=============================================================================
*/

void jsprintf (const char *msg, ...);

/*
=============================================================================

                        MACROS

=============================================================================
*/


#define BASE_GRAY   0x8
#define HIGH_GRAY   0xb
#define LOW_GRAY    0x5
#define BASE_GRN    0x76
#define HIGH_GRN    0x78
#define LOW_GRN     0x74


#define MAX_DEST_PATH_LEN	30

#define MAX_GUN_DELAY       12

#define MAX_RADAR_ENERGY    14400
#define RADAR_PAK_VALUE     (60 * 15)

#define MAXKEYS             1

#define MAPSPOT(x,y,plane) (mapsegs[plane][((y) << MAPSHIFT) + (x)])

#define ISPOINTER(x)       ((((uintptr_t)(x)) & ~0xffff) != 0)

#define MAX(a,b)           (((a) > (b)) ? (a) : (b))
#define MIN(a,b)           (((a) < (b)) ? (a) : (b))

#define FMAPWIDTH          ((fixed)mapwidth << TILESHIFT)
#define FMAPHEIGHT         ((fixed)mapheight << TILESHIFT)

#define ObjVisable(from_obj,to_obj)  PosVisable(from_obj->x,from_obj->y,to_obj->x,to_obj->y,from_obj->angle)

// SmartAnim macro
#define ANIM_INFO(o)     ((ofs_anim_t *)&(o)->temp3)


#define DISPLAY_MSG_STD_TIME            (5 * 60)    // tics display len
#define DISPLAY_TIMED_MSG(msg,pri,type) DisplayInfoMsg((msg),(pri),DISPLAY_MSG_STD_TIME,(type))
#define DISPLAY_MSG(msg,pri,type)       DisplayInfoMsg((msg),(pri),0,(type))


//
// SMART_ACTORS is the "case" used for certain switch statements
//
#define SMART_ACTORS    proguardobj:    \
                  case  rentacopobj:            \
                  case  gen_scientistobj


#define SECURITY_LAMPS_ALERTED   madenoise

//
// barrier code stuff
//
#define MAX_BARRIER_SWITCHES     40     // max number level wall switches


#define SLIDE_TEMP(obj)          (obj->hitpoints)

//
// M_BASE1 - represents 100 percent in 1st base
// M_BASE2 - represents 100 percent in 2nd base
// F_BASE2 - fractional portion of 2nd base
// SCALE - arbitrary scaling value (bigger number means more accurate)
//
// RETURNS: F_BASE1 - represents fractional portion of 1st base
//
// ex: RATIO(320,16,8,7)    returns  160
//
// Make sure values used won't overflow a WORD! In general, if largest number
// to be used (320 in ex: above) * (1<<SCALE) is greater than 65535, use
// LRATIO or a lower SCALE. Using a SCALE of 8 in the example above would
// overflow a WORD in some circumstances!
//
// LRATIO is to be used for larger SCALEs, thus, giving you massive accuracy!
//
#define RATIO(M_BASE1,M_BASE2,F_BASE2,SCALE)  ((unsigned)(M_BASE1 * ((F_BASE2 << SCALE) / M_BASE2)) >> SCALE)
#define LRATIO(M_BASE1,M_BASE2,F_BASE2,SCALE) (((int32_t)M_BASE1 * (((int32_t)F_BASE2 << SCALE) / M_BASE2)) >> SCALE)


#define MAX_INF_AREA_MSGS       6
#define MAX_LOCATION_DESC_LEN   45

#define DOOR_RUBBLE_STATNUM     112

#define SpawnExplosion(a,b)                 SpawnCusExplosion((a),(b),SPR_EXPLOSION_1,4,5,explosionobj)
#define SpawnFlash(a,b)                     SpawnCusExplosion((a),(b),SPR_EXPLOSION_1,4,5,deadobj)
#define InitSmartSpeedAnim(a,b,c,d,e,f,g)   InitAnim((a),(b),(c),(d),(e),(f),(g),(g))


/*
=============================================================================

                        GLOBAL CONSTANTS

=============================================================================
*/

#define OV_ACTORS       0x0001
#define OV_SHOWALL      0x0002
#define OV_KEYS         0x0004
#define OV_PUSHWALLS    0x0008

#define TT_TRAVELED     0x01
#define TT_KEYS         0x02


//#define CON_HINTS               // Un/Comment to support concession hints

#define MAXACTORS         150
#define MAXSTATS          400
#define MAXDOORS          64
#define MAXCONCESSIONS    15      // max number of concession machines
#define MAXVISIBLE        50
#define MAXWALLTILES      64
#define MAXEAWALLS        12      // max electro-alien wall sockets


#define GS_NEEDCOORD      0
#define GS_FIRSTTIME      1
#define GS_COORDFOUND     2
#define GS_NO_MORE      ` 3

#define GOLDIE_MAX_SPAWNS       10              // max number of spawn points for Goldstern
#define MIN_GOLDIE_FIRST_WAIT   (5 * 60)        // min wait time for FIRST goldstern (5 Secs)
#define MAX_GOLDIE_FIRST_WAIT   (15 * 60)       // max wait time for FIRST goldstern (15 Secs)
#define MIN_GOLDIE_WAIT         (30 * 60)       // min wait time for next goldstern (30 Secs)
#define MAX_GOLDIE_WAIT         (4 * 60 * 60)   // max wait time for next goldstern (4 Mins)


//
// tile constants
//
#define ICONARROWS              90
#define PUSHABLETILE            98
#define EXITTILE                99                              // at end of castle
#define AREATILE                108                             // first of NUMAREAS floor tiles
#define HIDDENAREATILE          162           // first of hidden floor tiles
#define NUMAREAS                45
#define DOORTRIGGERTILE         158
#define SMART_OFF_TRIGGER       159
#define SMART_ON_TRIGGER        160
#define ELEVATORTILE2           27           // Space Will Switch
#define TRANSPORTERTILE         21
#define DIRECTTRANSPORTTILE     32           // Wall to use on direct level transporters
#define SODATILE                15
#define TERMINALTILE            17
#define FOODTILE                18
#define AMBUSHTILE              106
#define RKEY_TILE               72
#define YKEY_TILE               73
#define BKEY_TILE               74
#define BFG_TILE                75
#define ION_TILE                76
#define DETONATOR_TILE          77
#define CLOAK_TILE              78
#define LINC_TILE               79
#define CLOAK_AMBUSH_TILE       80
#define EATILE                  24
#define ON_SWITCH               45          // ON Wall Switch -
#define OFF_SWITCH              57          // OFF Wall Switch -
#define WINTIGGERTILE           157         // Win Tile
#define NUMBERCHARS             9
#define BLOCKTILE               64     // TODO: check if 64 conflicts with anything else

#define START_TEXTURES          125         // Start of Textures - (Also LAST_WALL_TILE NUM)
#define NUM_TILES               PMSpriteStart


#define EXTRAPOINTS             400000

#define MAX_EXTRA_LIVES         4

#define RUNSPEED                6000

#define HEIGHTRATIO             0.41
#define TOP_STRIP_HEIGHT        16       // pix height of top strip
#define STATUSLINES             48


#define TILESHIFT       16
#define TILEGLOBAL      (1 << TILESHIFT)
#define TILECENTER      (TILEGLOBAL >> 1)
#define FRACBITS        16
#define FRACUNIT        (1 << FRACBITS)
#define UNSIGNEDSHIFT   8

#define ANG45      45
#define ANG90      90
#define ANG135     135
#define ANG180     180
#define ANG225     225
#define ANG270     270
#define ANG315     315
#define ANG360     360
#define ANG450     450

#define DEG45      450
#define DEG90      900
#define DEG135     1350
#define DEG180     1800
#define DEG225     2250
#define DEG270     2700
#define DEG315     3150
#define DEG360     3600
#define DEG450     4500

#define FOCALLENGTH     0x5700L
#define MINDIST         0x5800l
#define PLAYERSIZE      MINDIST      // player radius
#define MINACTORDIST    TILEGLOBAL   // minimum dist from player center to any actor center


#define MAX_WVIEW_DIST  44           // max wrap_view dist in TILES
#define FULL_VIEW_WIDTH 19

#define TEXTURESHIFT    6
#define FIXED2TEXSHIFT  4
#define TEXTURESIZE     (1 << TEXTURESHIFT)
#define TEXTUREMASK     (TEXTURESIZE * (TEXTURESIZE - 1))

#define MAPSHIFT        6
#define MAPSIZE         (1 << MAPSHIFT)
#define MAPAREA         (MAPSIZE * MAPSIZE)

#define NORTH   0
#define EAST    1
#define SOUTH   2
#define WEST    3


#define STARTAMMO       8

#define MAXTICS         10
#define DEMOTICS        4

//
// token definitions
//
#define MAX_TOKENS     25


//
// ammo/weapon definitions
//
//#define NUM_AMMO_SEGS           42          // 42 color segments (OR 21 2-color segs)
#define MAX_AMMO                100         // max amount of ammo for any weapon
#define AUTOCHARGE_WAIT         50          // tics wait time for a full charge
#define MAX_PLASMA_DETONATORS   100         // max number of plasma detonators
#define PLASMA_DETONATORS_DELAY (60 * 4)    // number of tics before plasma detonator explodes


//
// gamestate.flags flag values
//
#define GS_HEARTB_SOUND         0x0001

#ifdef CEILING_FLOOR_COLORS
#define GS_DRAW_CEILING         0x0002
#endif

#define GS_CLIP_WALLS           0x0004

#ifdef CEILING_FLOOR_COLORS
#define GS_DRAW_FLOOR           0x0008
#endif

#define GS_VIRGIN_LEVEL         0x0010
#define GS_CHECK_STATS_BONUS    0x0020
#define GS_ATTACK_INFOAREA      0x0040
#define GS_KILL_INF_WARN        0x0080
#define GS_SHOW_OVERHEAD        0x0100
#define GS_BAD_DIZ_FILE         0x0200
#define GS_MUSIC_TEST           0x0400
#define GS_LIGHTING             0x0800
#define GS_TICS_FOR_SCORE       0x1000
#define GS_NOWAIT               0x2000
#define GS_STARTLEVEL           0x4000
#define GS_QUICKRUN             0x8000


enum objflags
{
    FL_SHOOTABLE                = 0x00000001,
    FL_BONUS                    = 0x00000002,
    FL_NEVERMARK                = 0x00000004,
    FL_VISIBLE                  = 0x00000008,
    FL_ATTACKMODE               = 0x00000010,
    FL_FIRSTATTACK              = 0x00000020,
    FL_AMBUSH                   = 0x00000040,
    FL_NONMARK                  = 0x00000080,
    FL_SOLID                    = 0x00000100,
    FL_STATIONARY               = 0x00000200,
    FL_FRIENDLY                 = 0x00000400,
    FL_DEADGUY                  = 0x00000800,
    FL_RUNAWAY                  = 0x00001000,
    FL_RUNTOSTATIC              = 0x00002000,
    FL_OFFSET_STATES            = 0x00004000,
    FL_INFORMANT                = 0x00008000,
    FL_INTERROGATED             = 0x00010000,
    FL_RANDOM_TURN              = 0x00020000,
    FL_NO_SLIDE                 = 0x00040000,
    FL_MUST_ATTACK              = 0x00080000,
    FL_ALERTED                  = 0x00100000,
    FL_FREEZE                   = 0x00200000,
    FL_HAS_AMMO                 = 0x00400000,
    FL_PROJ_TRANSPARENT         = 0x00800000,
    FL_PROJ_CHECK_TRANSPARENT   = 0x01000000,
    FL_HAS_TOKENS               = 0x02000000,
    FL_LOCKED_STATE             = 0x04000000,
    FL_BARRIER                  = 0x08000000,
    FL_SHOOTMODE                = 0x10000000,
    FL_SLIDE_INIT               = 0x20000000,
    FL_STORED_OBJPTR            = 0x40000000,
    FL_FAKE_STATIC              = 0x80000000,
    FL_BARRIER_DAMAGE           = FL_HAS_TOKENS,
};

//
// TODO: way too many flags! Some of them can probably be controlled via other objstruct members
//
enum objflags2
{
    FL2_BFGSHOT_SOLID           = 0x0001,
    FL2_BFG_SHOOTABLE           = 0x0002,
    FL2_NOTGUNSHOOTABLE         = 0x0004,
    FL2_SCARED                  = 0x0008,
    FL2_DROP_RKEY               = 0x0010,
    FL2_DROP_YKEY               = 0x0020,
    FL2_DROP_BKEY               = 0x0040,
    FL2_DROP_BFG                = 0x0080,
    FL2_DROP_ION                = 0x0100,
    FL2_DROP_DETONATOR          = 0x0200,
    FL2_CLOAKED                 = 0x0400,
    FL2_LINC                    = 0x0800,
    FL2_DAMAGE_CLOAK            = 0x1000,
};


//
// run reason flags
//
#define RR_AMMO             0x0001
#define RR_HEALTH           0x0002
#define RR_INTERROGATED     0x0004
#define RR_CASUAL_PICKUP    0x0008
#define RR_SCARED           0x0010

//
// 0xFFxx special tile flags (flags in MASKED plane of Ted maps)
//
#define TF_STARFIELD        0x01
#define TF_WRAP_VIEW        0x02


//
// concession machine types
//
#define CT_HINT      0x0
#define CT_FOOD      0x1
#define CT_BEVS      0x2


//
// radar switches for terminal floor cheat
//
#define RS_GOLDSTERN_TRACKER    0x0001
#define RS_PERSONNEL_TRACKER    0x0002
#define RS_SECURITY_STATUS      0x0004


//
// door flags
//
#define DR_BLASTABLE    0x01

//
// smart trigger possiblities
//
#define ST_TURN_OFF     0x00
#define ST_TURN_ON      0x01
#define ST_TOGGLE       0x02


#define LT_GUN_DELAY    gamestate.lastammo_leds
#define RT_GUN_DELAY    gamestate.ammo_leds
#define GUN_TOGGLE      gamestate.ammo


#define CANT_SAVE_GAME_TXT  " Can't save this game! \n"     \
                            "    Hard Disk FULL!"

enum ss_type
{
    ss_normal,
    ss_quick,
    ss_justcalc,
};


enum cds_io_type
{
    cds_dos_print,
    cds_id_print,
    cds_menu_print,
};


enum sp_type
{
    sp_normal,
    sp_loading,
    sp_saving,
    sp_changeview,
    sp_teleporting,
};


//
// breifing types - note these are ordered to an char array in Breifing().
//
enum breifing_type
{
    BT_LOSE,
    BT_WIN,
    BT_INTRO,
};

//
// terminal messages - these correspond to the order in which they are
//                     grabbed in VGAGRAPH.BS?.  See BSTONEV.I
//
enum term_msg_type
{
    TM_HINT,
    TM_JAM,
    TM_HELP,
    TM_APOGEE,
    TM_ID,
    TM_GOOBERS,
    TM_MIKE,
    TM_JIM,
    TM_JERRY,

    TM_JOSHUA,

    TM_STAR,

    TM_VITALS1,
    TM_VITALS2,

    TM_DEACTIVATE_TURRET,
    TM_TURRETS_ACTIVATED,
    TM_TURRETS_DEACTIVATED,

    TM_LINK,
    TM_LINK_OK,
    TM_LINK_BAD,

    TM_RADAR_OFF,
    TM_RADAR_ON,

    TM_SOUND_OFF,
    TM_SOUND_ON,

    TM_GOLDSTERN_TRACK_OFF,
    TM_GOLDSTERN_TRACK_ON,
    TM_GOLDSTERN_ARRIVED,
    TM_GOLDSTERN_WILL_AR,
    TM_GOLDSTERN_NO_PICK,
    TM_GOLDSTERN_NO_INFO,

    TM_RESET_SECURITY,
    TM_SECURITY_STATUS_OFF,
    TM_SECURITY_STATUS_ON,

    TM_TURRET_DEACTIVATE_BAD,
    TM_TURRET_DEACTIVATE_GOOD,

    TM_UNRECOGNIZED_COMMAND,
    TM_READY,
    TM_RETURN,
    TM_SECONDS,
    TM_CHEATER,
    TM_BLUEPRINTS,

    TM_PROFILE_WHO,
    TM_PROFILE_SARA,
    TM_PROFILE_BLAKE,
    TM_PROFILE_GOLDSTERN,
    TM_PROFILE_UNKNOWN,
    TM_DEACTIVATE_SOCKETS,
    TM_ACTIVATE_SOCKETS,
    TM_UNABLE_TO_PERFORM,
    TM_NO_SOCKETS,
    TM_ALREADY_ACTIVATED,
    TM_ALREADY_DEACTIVATED,
    TM_LAST,
};

//
// terminal commands - the first set of commands TC_HINT - TC_end_of_1to1
//                     are directly mapped 1 to 1 to the terminal msgs.
//
enum term_cmd_type
{
    TC_HINT,
    TC_JAM,
    TC_HELP,
    TC_APOGEE,
    TC_THANKS,
    TC_GOOBERS,
    TC_MIKE,
    TC_JIM,
    TC_JERRY,

    // END OF ONE TO ONE LIST

    TC_JOSHUA,
    TC_STAR,
    TC_BLUEPRINT,
    TC_EXIT,
    TC_QUIT,
    TC_OFF,
    TC_BYE,
    TC_DISPLAY_PERSONNEL,
    TC_SOUND,
    TC_DISPLAY_GOLDSTERN,
    TC_ARRIVAL_GOLDSTERN,
    TC_DEACTIVATE_SECURITY,
    TC_DISPLAY_SECURITY,
    TC_SATALITE_STATUS,
    TC_DEACTIVATE_TURRETS,
    TC_TURRET_STATUS,
    TC_PROFILE,
    TC_SSTONE,
    TC_BSTONE,
    TC_GOLDSTERN,
    TC_DEACTIVATE_SOCKETS,
    TC_ACTIVATE_SOCKETS,

    TC_LAST,
};

//
// barrier state transitions
//
enum barrier_state_type
{
    bt_OFF,
    bt_ON,
    bt_DISABLING,
    bt_DISABLED,

    // for physical barriers

    bt_OPENING,
    bt_CLOSING,
};


//
// Display priority is determined by the order of these bits!
// And, this order must match the PinballBonus table in 3D_AGENT.C!
//
#define B_GALIEN_DESTROYED  0x0001
#define B_SCORE_ROLLED      0x0002
#define B_ONE_MILLION       0x0004
#define B_EXTRA_MAN         0x0008
#define B_ENEMY_DESTROYED   0x0010
#define B_TOTAL_POINTS      0x0020
#define B_INFORMANTS_ALIVE  0x0040

#define BONUS_QUEUE         gamestuff.level[gamestate.mapon].bonus_queue
#define BONUS_SHOWN         gamestuff.level[gamestate.mapon].bonus_shown

#define PinballBonusShown(bonus)    (BONUS_SHOWN & bonus)
#define ActivatePinballBonus(bonus) if (!PinballBonusShown(bonus)) BONUS_QUEUE |= bonus

typedef struct
{
    const char  *BonusText;         // REBA text pointer
    int32_t     Points;             // Score for this bonus
    bool        Recurring;          // Appear multiple times in a single level?
    void        (*func)(void);      // Code to execute when you get this bonus.
} PinballBonusInfo;


typedef struct
{
    byte   tics;
    int8_t attack;
    byte   frame;  // attack is 1 for gun, 2 for autocharge
} atkinf_t;


enum movietype   // NOTE - This enum list is ORDERED!
{
    mv_intro,
    mv_final,

    mv_NUM_MOVIES,
};


enum controldirtypes
{
    di_north,
    di_east,
    di_south,
    di_west,
};


enum keytypes
{
    kt_none = -1,
    kt_red,
    kt_yellow,
    kt_blue,

    NUMKEYS,
};


enum enemytypes
{
    en_rentacop,    // Actors with hitpoints (normal actors)
    en_hang_terrot,
    en_gen_scientist,
    en_pod,
    en_electro_alien,
    en_electrosphere,
    en_proguard,
    en_genetic_guard,
    en_mutant_human1,
    en_mutant_human2,
    en_lcan_wait_alien,
    en_lcan_alien,
    en_scan_wait_alien,
    en_scan_alien,
    en_gurney_wait,
    en_gurney,
    en_liquid,
    en_swat,
    en_goldstern,
    en_gold_morph,
    en_volatiletransport,
    en_floatingbomb,
    en_rotating_cube,

    en_spider_mutant,
    en_breather_beast,
    en_cyborg_warrior,
    en_reptilian_warrior,
    en_acid_dragon,
    en_mech_guardian,

    en_final_boss1,
    en_final_boss2,
    en_final_boss3,
    en_final_boss4,

    en_blake,

    en_crate1,
    en_crate2,
    en_crate3,

    en_green_ooze,
    en_black_ooze,
    en_green2_ooze,
    en_black2_ooze,
    en_podegg,

    en_morphing_spider_mutant,
    en_morphing_reptilian_warrior,
    en_morphing_mutanthuman2,

    NUMHITENEMIES,

    en_electro_shot,   // Actors WITHOUT hitpoints (abnormal actors?)
    en_post_barrier,
    en_arc_barrier,
    en_vpost_barrier,
    en_vspike_barrier,
    en_goldmorphshot,

    en_security_light,
    en_explosion,
    en_steamgrate,
    en_steampipe,

    en_liquidshot,

    en_lcanshot,
    en_podshot,
    en_scanshot,
    en_dogshot,
    en_mut_hum1shot,

    en_ventdrip,
    en_playerspshotobj,
    en_flickerlight,

    en_plasma_detonator,
    en_plasma_detonator_reserve,

    en_vertsphere,    // Actor types only used for spawning.
    en_horzsphere,
    en_diagsphere,
    en_bloodvent,
    en_watervent,

    NUMENEMIES
};


typedef struct objstruct
{
    byte        tilex,tiley;
    byte        areanumber;

    int8_t      active;
    int16_t     ticcount;
    byte        obclass;
    struct      statestruct *state;

    uint32_t    flags;
    uint16_t    flags2;         // Aux flags

    int32_t     distance;       // if negative, wait for that door to open
    byte        dir;
    byte        trydir;         // "bit 7" == "direction to turn" flag

    fixed       x,y;
    byte        s_tilex,s_tiley;  // s_tilex == 0, running for corner

    int16_t     viewx;
    int16_t     viewheight;
    fixed       transx,transy;    // in global coord

    int16_t     hitpoints;
    byte        ammo;
    byte        lighting;
    uint16_t    linc;
    int16_t     angle;
    int32_t     speed;

    int16_t     temp1,temp2;
    uint16_t    temp3;          // holds 'last door used' by 'smart' actors

    /**
    // DO NOT ADD ANY MEMBERS AFTER THESE!!!
    */
    struct      objstruct *next,*prev;
} objtype;


#define SF_ROTATE    0x01
#define SF_PAINFRAME 0x02

typedef struct statestruct
{
    byte    flags;
    int16_t shapenum;           // a shapenum of -1 means get from ob->temp1
    int16_t tictime;
    void    (*think)(objtype *),(*action)(objtype *);
    struct  statestruct *next;
} statetype;


enum buttontypes
{
    bt_nobutton = -1,
    bt_attack = 0,
    bt_strafe,
    bt_run,
    bt_use,
    bt_ready_autocharge,
    bt_ready_pistol,
    bt_ready_burst_rifle,
    bt_ready_ion_cannon,
    bt_ready_grenade,
    bt_ready_bfg_cannon,
    bt_ready_plasma_detonators,

    // TODO: make sure these don't break anything

    bt_nextweapon,
    bt_prevweapon,
    bt_esc,
    bt_pause,
    bt_strafeleft,
    bt_straferight,
    bt_moveforward,
    bt_movebackward,
    bt_turnleft,
    bt_turnright,

    bt_SPACER,  // TODO: why?

    NUMBUTTONS,
};


enum weapontypes
{
    wp_autocharge,
    wp_pistol,
    wp_burst_rifle,
    wp_ion_cannon,
    wp_grenade,
    wp_bfg_cannon,
    // wp_plasma_detonators,

    wp_SPACER,

    NUMWEAPONS,
};


enum difficultytypes
{
    gd_baby,
    gd_easy,
    gd_medium,
    gd_hard,

    NUMDIFFICULTY
};


enum backgroundtypes
{
    ELEVATOR_BACK,
    TRANSPORTER_BACK,
};


//
// general coord (tile) structure
//
typedef struct
{
    byte tilex,tiley;
} tilecoord_t;


//-----------------------------------
//
// barrier coord/table structure
//
//-----------------------------------

typedef struct
{
    byte tilex,tiley;
    byte on;
} barrier_t;


//---------------
//
// gamestate structure
//
//---------------

typedef struct statsInfoType
{
    int32_t  total_points,accum_points;
    byte     total_enemy,accum_enemy;
    byte     total_inf,accum_inf;
    int16_t  overall_floor;
} statsInfoType;


typedef struct
{
    uint16_t      bonus_queue;    // bonuses that need to be shown
    uint16_t      bonus_shown;    // bonuses that have been shown
    bool          locked;
    statsInfoType stats;
    byte          ptilex,ptiley;
    int16_t       pangle;
} levelinfo;


typedef struct
{
    levelinfo  old_levelinfo[MAPS_PER_EPISODE];
    levelinfo  level[MAPS_PER_EPISODE];
} fargametype;


typedef struct
{
    int16_t    turn_around,turn_angle;
    uint16_t   flags;
    int16_t    mapon,lastmapon;
    int16_t    difficulty;
    int16_t    status_refresh;
    int32_t    oldscore,tic_score,score,nextextra;
    int16_t    score_roll_wait;
    int16_t    lives;
    int16_t    health;
    int16_t    health_delay;
    char       health_str[4];

    int16_t    rpower,old_rpower;
    int8_t     rzoom;
    int8_t     radar_leds,lastradar_leds;

    int8_t     lastammo_leds;
    int8_t     ammo_leds;
    int16_t    ammo,old_ammo;

    int16_t    plasma_detonators,old_plasma_detonators;

    int8_t     useable_weapons;
    int8_t     weapons,weapon,chosenweapon,old_weapons[4];
    int8_t     weapon_wait;
    int8_t     key_floor;

    int16_t    attackframe,attackcount,weaponframe;
    int8_t     episode;
    int32_t    timecount;
    fixed      killx,killy;
    const char *msg;        // InfoArea msg

    int8_t     numkeys[NUMKEYS],old_numkeys[NUMKEYS];
    barrier_t  barrier_table[MAX_BARRIER_SWITCHES];
    barrier_t  old_barrier_table[MAX_BARRIER_SWITCHES];
    uint16_t   tokens,old_tokens;
    bool       boss_key_dropped,old_boss_key_dropped;
    int16_t    wintilex,wintiley;
} gametype;


enum exittypes
{
    ex_stillplaying,
    ex_completed,
    ex_transported,
    ex_died,
    ex_warped,
    ex_resetgame,
    ex_loadedgame,
    ex_victorious,
    ex_abort,
    ex_demodone,
    ex_secretlevel,
    ex_title,
};


typedef struct
{
    byte init_delay;
    byte delay_count;
    byte firstreg;
    byte lastreg;
} CycleInfo;


enum animtypes
{
    at_NONE,
    at_CYCLE,
    at_REBOUND,
    at_ONCE,
};


enum animdirtypes
{
    ad_FWD,
    ad_REV
};

//
// TODO: this is an abomination
//
typedef struct ofs_anim_struct
{
    uint16_t animtype:2;  // animtypes
    uint16_t curframe:5;
    uint16_t maxframe:5;
    uint16_t animdir:1;  // animdirtypes
    uint16_t extra:3;
} ofs_anim_t;


//
// displayInfoMsg priorities - duplicate values are allowed
//
// ("enum" list used simply for debugging use like object classes)
//
enum msgpriorities
{
    MP_min_val                  = 0,

    MP_HEARTB_SND               = 0x0200,
    MP_WALLSWITCH_OPERATE       = 0x0200,

    MP_DOOR_OPERATE             = 0x0200,
    MP_CONCESSION_OPERATE       = 0x0200,
    MP_WEAPON_AVAIL             = 0x0200,

    MP_ATTACK_INFO              = 0x0200,
    MP_NO_MORE_AMMO             = 0x0200,
    MP_WEAPON_MALFUNCTION       = 0x0200,

    MP_INTERROGATE              = 0x0200,
    MP_CONCESSION_HINT          = 0x0200,
    MP_NO_MORE_TOKENS           = 0x0200,
    MP_CONCESSION_OUT_ORDER     = 0x0200,

    MP_BONUS                    = 0x0200,

    MP_TAKE_DAMAGE              = 0x0200,
    MP_DETONATOR                = 0x0200,

    MP_PINBALL_BONUS            = 0x3000,
    MP_FLOOR_UNLOCKED           = 0x3000,

    MP_POWERUP                  = 0x7000,   // Power-Up/Game-Start Value

    MP_max_val                  = 0x7fff,   // DO NOT USE/EXCEED - MAX Val
};


enum infomsgtypes
{
    MT_NOTHING,
    MT_CLEAR,
    MT_ATTACK,
    MT_GENERAL,
    MT_OUT_OF_AMMO,
    MT_MALFUNCTION,
    MT_NO_MO_FOOD_TOKENS,
    MT_BONUS,
};


//
// menu instruction text types
//
enum insttypes
{
    IT_STANDARD,
    IT_HIGHSCORES,
    IT_ENTER_HIGHSCORE,
    IT_MOUSE_SEN,

    MAX_INSTRUCTIONS,
};


#define MAX_CACHE_MSGS   30
#define MAX_CACHE_MSG_LEN 190

//------------------------- BASIC STRUCTURES -----------------------------

//
// basic 'message info' structure
//
typedef struct
{
    byte local_val;     // where msg is in 'local' list
    byte global_val;    // where msg was in 'global' list
    void *mSeg;         // pointer to message
} mCacheInfo;

//
// basic 'message list' structure
//
typedef struct
{
    int16_t    NumMsgs;                 // number of messages
    mCacheInfo mInfo[MAX_CACHE_MSGS];   // table of message 'info'
} mCacheList;


//----------------------- CONCESSION STRUCTURES --------------------------

//
// concession 'message info' structure
//
typedef struct
{
    mCacheInfo mInfo;
    byte       type;            // type of concession
    byte       operate_cnt;     // # of times req'd to operate
} con_mCacheInfo;

//
// concession 'message list' structure
//
typedef struct
{
    int16_t        NumMsgs;          // also, num concessions
    con_mCacheInfo cmInfo[MAX_CACHE_MSGS];
} concession_t;

//------------------------ INFORMANT STRUCTURES --------------------------

//
// informant 'message info' structure
//
typedef struct
{
    mCacheInfo mInfo;
    byte       areanumber;        // 'where' msg can be used
} sci_mCacheInfo;

//
// informant 'message list' structure
//
typedef struct
{
    int16_t        NumMsgs;
    sci_mCacheInfo smInfo[MAX_CACHE_MSGS];
} scientist_t;

//------------------------------------------------------------------------

//
// electro-alien controller
//
typedef struct
{
    byte    tilex,tiley;    // where this controller is in the map
    byte    aliens_out;     // aliens spawned by this controller
    int16_t delay;          // delay before spawning another alien
} eaWallInfo;


//
// general structure to hold goldstern specific stuff
//
typedef struct
{
    byte     LastIndex;         // Last Spawn Coord Index
    byte     SpawnCnt;          // Num of Spawnpoints for Goldstern
    uint16_t flags;             // What type of command/operation is needed...
    uint16_t WaitTime;          // Wait time for Goldstern Spawn (current & Next)
    bool     GoldSpawned;       // Has Goldstern been spawned?
} GoldsternInfo_t;


typedef struct
{
    fixed x,y,z;
    byte  color;
} star_t;


#define MAX_SPACE_THRUST    ((int32_t)0x4fff)

#define MAX_STARS           128
#define MAX_SPACE_STATS     ((sizeof(statobjlist) - ((MAX_STARS + 1) * sizeof(star_t))) / sizeof(statobj_t))


#include "3d_act1.h"
#include "3d_agent.h"
#include "3d_draw.h"
#include "3d_game.h"
#include "3d_plane.h"
#include "3d_play.h"
#include "3d_scale.h"
#include "3d_state.h"
#include "3d_utils.h"


//
// temp definitions
//
extern  int     currentsong;

int     StartCPMusic (int song);


/*
=============================================================================

                        3D_MAIN DEFINITIONS

=============================================================================
*/

#define TITLE_LOOP_MUSIC    PLOT_MUS

#define CANT_PLAY_TXT "\n"                 \
                      "ERROR: Insufficient disk space.\n"                  \
                      "Try deleting some files from your hard disk.\n\n"

extern  char      tempPath[MAX_DEST_PATH_LEN + 15];

extern  levelinfo default_level[MAPS_PER_EPISODE];
extern  int       view_xl,view_xh,view_yl,view_yh;
extern  int       starting_level,debug_value,starting_episode,starting_difficulty;

extern  int       lastmap_tilex,lastmap_tiley;
extern  char      str[256],error[256];

extern  int       mouseadjustment;

extern  char      configname[13];

extern  bool      ShowQuickMsg;

extern  byte      colormap[16896];

extern  int       param_samplerate;
extern  int       param_audiobuffer;


void     LoadFonts (void);
void     ClearNClose (void);
void     CycleColors (void);
void     LoadAccessCodes (void);
void     AlignPlayerInElevator (void);
void     HelpScreens (void);
void     OrderingInfo (void);
unsigned scan_atoi (const char *s);
void     AlignPlayerOnTransporter (void);

unsigned UseFunc (char *first, char *next);  // TODO: oh boy, huge pointers
bool     DoMovie (int movie, void *palette);    // TODO: SDL_Color?
bool     CheckDiskSpace (int32_t needed, const char *text, int io_type);
bool     SaveTheGame (int handle, const char *description);
int32_t  ChecksumFile (const char *file, int32_t checksum);
void     BadChecksum (void);
void     InvalidLevels (void);
void     CheckValidity (const char *file, int32_t valid_checksum);
void     UnauthorizedLevels (void);
void     ShowChecksums (void);
void     fprint (const char *text);

void     InitDigiMap (void);

void     CleanUpDoors_N_Actors (void);

void     Terminate (const char *func, const char *string, ...);

void     MakeDestPath (const char *file);
void     InitDestPath (void);

int32_t  FindChunk (FILE *file, const char *chunk);
int32_t  NextChunk (FILE *file);
int32_t  DeleteChunk (FILE *sourcefile, const char *chunk);


/*
=============================================================================

                        3D_DEBUG

=============================================================================
*/

int  DebugKeys (void);
void CalcMemFree (void);


/*
=============================================================================

                        3D_ACT2 DEFINITIONS

=============================================================================
*/

#define BARRIER_STATE(obj) ((obj)->ammo)

#define InitSmartAnim(obj,ShapeNum,StartOfs, MaxOfs,AnimType, AnimDir)      \
        InitSmartSpeedAnim(obj,ShapeNum,StartOfs,MaxOfs,AnimType,AnimDir,7)


extern  bool      noShots;
extern  int       detonators_spawned;
extern  int       starthitpoints[NUMDIFFICULTY][NUMHITENEMIES];
extern  unsigned  MorphClass[];

extern  statetype s_ofs_bounce;

extern  statetype s_ofs_esphere_death1;
extern  statetype s_ofs_ouch;

extern  statetype s_ofs_static;

extern  statetype s_rent_die1;
extern  statetype s_ofcdie1;
extern  statetype s_swatdie1;
extern  statetype s_prodie1;
extern  statetype s_proshoot1;

extern  statetype s_rent_chase1;
extern  statetype s_ofcchase1;
extern  statetype s_prochase1;
extern  statetype s_swatchase1;

extern  statetype s_rent_pain;
extern  statetype s_ofcpain;
extern  statetype s_propain;
extern  statetype s_swatpain;

extern  statetype s_hold;

extern  statetype s_swatwounded1;

extern  statetype s_terrot_wait;
extern  statetype s_terrot_seek1;

extern  statetype s_ofs_stand;
extern  statetype s_ofs_chase1;
extern  statetype s_ofs_pain;
extern  statetype s_ofs_die1;
extern  statetype s_ofs_pod_death1;
extern  statetype s_ofs_pod_ouch;
extern  statetype s_ofs_attack1;

extern  statetype s_electro_appear1;
extern  statetype s_electro_chase1;
extern  statetype s_electro_ouch;
extern  statetype s_electro_shoot1;
extern  statetype s_electro_die1;

extern  statetype s_liquid_wait;
extern  statetype s_liquid_move;
extern  statetype s_liquid_rise1;
extern  statetype s_liquid_shoot1;
extern  statetype s_liquid_ouch;
extern  statetype s_liquid_shot;
extern  statetype s_liquid_die1;
extern  statetype s_liquid_shot_exp1;

extern  statetype s_goldstand;
extern  statetype s_goldpath1;
extern  statetype s_goldpain;
extern  statetype s_goldshoot1;
extern  statetype s_goldchase1;
extern  statetype s_goldwarp_it;    // Warp In w/button pressing
extern  statetype s_goldwarp_out1;
extern  statetype s_goldwarp_in1;

extern  statetype s_goldmorph1;
extern  statetype s_goldmorph2;
extern  statetype s_goldmorph3;
extern  statetype s_goldmorph4;
extern  statetype s_goldmorph5;
extern  statetype s_goldmorph6;
extern  statetype s_goldmorph7;
extern  statetype s_goldmorph8;

extern  statetype s_mgold_chase1;
extern  statetype s_mgold_chase2;
extern  statetype s_mgold_chase3;
extern  statetype s_mgold_chase4;

extern  statetype s_mgold_shoot1;
extern  statetype s_mgold_shoot2;
extern  statetype s_mgold_shoot3;
extern  statetype s_mgold_shoot4;

extern  statetype s_mgold_pain;

extern  statetype s_security_light;

extern  statetype s_scout_path1;
extern  statetype s_scout_path2;
extern  statetype s_scout_path3;
extern  statetype s_scout_path4;
extern  statetype s_scout_pain;
extern  statetype s_scout_run;
extern  statetype s_scout_run2;
extern  statetype s_scout_run3;
extern  statetype s_scout_run4;
extern  statetype s_scout_die1;
extern  statetype s_scout_stand;
extern  statetype s_scout_dead;

extern  statetype s_explosion1;

extern  statetype s_steamgrate;
extern  statetype s_vital;

extern  statetype s_vital_ouch;
extern  statetype s_vital_die;
extern  statetype s_vital_die1;
extern  statetype s_vital_die2;
extern  statetype s_vital_die3;
extern  statetype s_vital_die4;
extern  statetype s_vital_die5;
extern  statetype s_vital_die6;
extern  statetype s_vital_die7;
extern  statetype s_vital_die8;

extern  statetype s_ooze_chase;
extern  statetype s_vpost_barrier;
extern  statetype s_spike_barrier;

extern  statetype s_barrier_transition;
extern  statetype s_barrier_shutdown;


void     MakeFakeStatic (objtype *obj);
void     UnmakeFakeStatic (objtype *obj);

void     T_PainThink (objtype *obj);
void     T_ExplodeScout (objtype *obj);

void     T_Security (objtype *obj);

void     T_ChangeShape (objtype *obj);
void     T_MakeOffset (objtype *obj);
void     T_LiquidStand (objtype *obj);

void     PlaceTowardPlayer (objtype *obj);

void     T_Seek (objtype *obj);

void     SpawnProjectile (objtype *shooter, int obclass);
void     SpawnStand (int which, int tilex, int tiley, int dir);
void     SpawnPatrol (int which, int tilex, int tiley, int dir);

int      IntSqrt (int32_t va);
unsigned CalcDistance (unsigned x1, unsigned y1, unsigned x2, unsigned y2);

void     T_Hit (objtype *obj);
void     SpawnOffsetObj (int which, int tilex, int tiley);

void     InitSmartAnimStruct (objtype *obj, unsigned ShapeNum, byte StartOfs, byte MaxOfs, int AnimType, int AnimDir);
bool     AnimateOfsObj (objtype *obj);
void     AdvanceAnimREV (objtype *obj);
void     AdvanceAnimFWD (objtype *obj);

void     SpawnCusExplosion( fixed x, fixed y, unsigned StartFrame, unsigned NumFrames, unsigned Delay, unsigned Class);
void     T_SpawnExplosion (objtype *obj);
void     T_ExplodeDamage (objtype *obj);

void     ExplodeRadius (objtype *obj, int damage, bool damageplayer);

void     SpawnBarrier (int which, int tilex, int tiley, bool OnOff);
void     ToggleBarrier (objtype *obj);

void     InitAnim (objtype *obj, unsigned ShapeNum, byte StartOfs, byte MaxOfs, int AnimType, int AnimDir, unsigned Delay, unsigned WaitDelay);

objtype  *FindObj (int which, int tilex, int tiley);
objtype  *FindHiddenOfs (int which);
void     SpawnHiddenOfs (int which, int tilex, int tiley);
objtype  *MoveHiddenOfs (int which_class, int new_class, fixed x, fixed y);

void     CheckForSpecialTile (objtype *obj, int tilex, int tiley);
void     DropCargo (objtype *obj);


/*
=============================================================================

                        3D_TEXT DEFINITIONS

=============================================================================
*/

extern  char    helpfilename[13],endfilename[13];

void    HelpScreens (void);
void    EndText (void);


/*
=============================================================================

                        3D_MSGS TEXT DEFINITIONS

=============================================================================
*/


extern  char  noeat_msg1[];
extern  char  bevs_msg1[];
extern  char  food_msg1[];

extern  char  bonus_msg7[];
extern  char  bonus_msg24[];
extern  char  bonus_msg25[];
extern  char  bonus_msg26[];

extern  char  B_GAlienDead2[];
extern  char  B_GAlienDead[];
extern  char  B_ScoreRolled[];
extern  char  B_OneMillion[];
extern  char  B_ExtraMan[];
extern  char  B_EnemyDestroyed[];
extern  char  B_TotalPoints[];
extern  char  B_InformantsAlive[];

extern  char  *BonusMsg[];
extern  char  *ActorInfoMsg[];
extern  char  ekg_heartbeat_enabled[];
extern  char  ekg_heartbeat_disabled[];
extern  char  attacker_info_enabled[];
extern  char  attacker_info_disabled[];
extern  char  WeaponNotAvailMsg[];
extern  char  WeaponAvailMsg[];
extern  char  RadarAvailMsg[];
extern  char  RadarEnergyGoneMsg[];
extern  char  WeaponAutoSelectMsg[];
extern  char  EnergyPackDepleted[];
extern  char  NotEnoughEnergyForWeapon[];

extern  char  WeaponMalfunction[];

extern  char  SwitchNotActivateMsg[];
extern  char  NoFoodTokens[];
extern  char  ExtraMan[];
extern  char  OneMillion[];
extern  char  TenMillion[];

extern  char  NoAdLibCard[];
extern  char  MusicOn[];
extern  char  MusicOff[];
extern  char  SoundOn[];
extern  char  SoundOff[];

extern  char  pd_dropped[];
extern  char  pd_nomore[];
extern  char  pd_switching[];
extern  char  pd_notnear[];
extern  char  pd_getcloser[];
extern  char  pd_floorunlocked[];
extern  char  pd_donthaveany[];
extern  char  pd_no_computer[];
extern  char  pd_floornotlocked[];

extern  char  int_xx[];

void      InitMsgCache (mCacheList *mList, int listsize, int infosize);
void      FreeMsgCache (mCacheList *mList, int listsize);
void      CacheMsg (mCacheInfo *ci, int segnum, int msgnum);
int       LoadMsg (char *buffer, int segnum, int msgnum, int maxlen);
void      CacheConcessionMsg (void);
bool      ReuseMsg (mCacheInfo *ci, int count, int structsize);


/*
=============================================================================

                        3D_INTER DEFINITIONS

=============================================================================
*/

extern  char BreifingText[];

void     UpdateScreenPic (void);
void     DisplayPrepingMsg (const char *text);
bool     Breifing (int BreifingType, int episode);
void     ShPrint (const char *text, int shadowcolor, bool singlechar);
unsigned Random (unsigned Max);


/*
=============================================================================

                        3D_MENU DEFINITIONS

=============================================================================
*/

extern   bool  EscPressed;

void     DrawInstructions (int Type);
void     CacheMessage (unsigned MessageNum);
void     TerminateStr (const char *pos);
uint32_t CacheCompData (unsigned ItemNum, void **dest_loc);
bool     CheckForSpecialCode (unsigned ItemNum);

#endif
