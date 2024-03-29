// 3D_ACT2.C

#include "3d_def.h"


#define EXPLODE_STATIC_SCAN
#define BFG_SHOT_STOPS
// #define DODGE_N_CHASE


#define SPDPATROL       512
#define SPDPROJ         7168
#define PROJSIZE        TILECENTER          // collision with actor range
#define PROJCHECKSIZE   TILECENTER          // scan for actor range
#define PROJWALLSIZE    (TILECENTER / 4)    // collision with wall range

#define PROJECTILESIZE  (TILEGLOBAL - (TILECENTER / 2))    // 3/4ths of a tile

#define SEEK_TURN_DELAY 30     // tics

#define CLOSE_RANGE     1      // tiles

#define ALIENSPEED      SPDPATROL
#define ALIENAMMOINIT   (30 + (US_RndT() % 60))

#define GR_DAMAGE       (40 + (US_RndT() & 0x7f))   // 20 & 3f
#define BFG_DAMAGE      (GR_DAMAGE << 1)            // hit point damage caused by BFG

#define PLASMA_DETONATOR_DAMAGE     500
#define DETONATOR_FLASH_RATE        20
#define EXPLODE_DAMAGE              20      // 5
#define EXPLODE_RADIUS              2       // tiles out from center

#define OOZE_ANIMATE_SPEED          20

#define VPOST_BARRIER_SPEED         7       // tics per anim step
#define VPOST_WAIT_DELAY            90      // tic delay in cycling

#define DR_MIN_STATICS              50      // min # of statics avail before 50/50 chance of rubble on exp. doors


typedef struct
{
    bool    buffer[(EXPLODE_RADIUS * 2) + 1][(EXPLODE_RADIUS * 2) + 1];
    bool    damageplayer;
    int16_t damage;
    objtype *obj;
} expfill_t;


int     starthitpoints[NUMDIFFICULTY][NUMHITENEMIES] =
{
    {
        4,      // Rent-A-Cop
        4,      // Hang Terrot
        4,      // general scientist
        22,     // pod alien
        13,     // Electro-Alien
        1,      // Electro-Sphere
        33,     // ProGuard
        20,     // Genetic Guard
        24,     // Mutant Human Type 1
        24,     // Mutant Human Type 2
        1,      // Large Canister Alien - CONTAINER HP
        34,     // Large Can-Alien - ALIEN HP
        1,      // Small Canister Alien - CONTAINER HP
        25,     // Small Can-Alien - ALIEN HP
        1,      // Gurney Mutant - Waiting HP
        27,     // Gurney Mutant - Spawned HP
        30,     // Liquid Alien -
        78,     // Swat Guards
        90,     // Goldstern
        4000,   // Morphed Goldstern
        17,     // Volatile Material Transport
        15,     // Floating Bomb
        175,    // Vital Defence Obj  - DON'T CHANGE
        60,     // spider_mutant
        65,     // breather_beast
        70,     // cyborg_warror
        65,     // reptilian_warrior
        60,     // acid_dragon
        65,     // mech_guardian
        1600,   // final_boss 1
        1700,   // final_boss 2
        1800,   // final_boss 3
        1900,   // final_boss 4
        1,      // Crate 1
        1,      // Crate 2
        1,      // Crate 3
        1,      // Pod egg
        1,      // morphing_spider_mutant
        1,      // morphing_reptilian_warrior
        1,      // morphing_Mutant Human Type 2
    },

    {
        9,      // Rent-A-Cop
        9,      // Hang Terrot
        9,      // general scientist
        60,     // pod alien
        37,     // Electro-Alien
        1,      // Electro-Sphere
        63,     // ProGuard
        60,     // Genetic Guard
        50,     // Mutant Human Type 1
        50,     // Mutant Human Type 2
        1,      // Large Canister Alien - CONTAINER HP
        75,     // Large Can-Alien - ALIEN HP
        1,      // Small Canister Alien - CONTAINER HP
        60,     // Small Can-Alien - ALIEN HP
        1,      // Gurney Mutant - Waiting HP
        37,     // Gurney Mutant - Spawned HP
        66,     // Liquid Alien -
        112,    // SWAT Guards
        100,    // Goldstern
        4500,   // Morphed Goldstern
        25,     // Volatile Material Transport
        40,     // Floating Bomb
        175,    // Vital Defence Obj  - DON'T CHANGE
        100,    // spider_mutant
        115,    // breather_beast
        100,    // cyborg_warror
        115,    // reptilian_warrior
        100,    // acid_dragon
        115,    // mech_guardian
        1700,   // final_boss 1
        1800,   // final_boss 2
        1900,   // final_boss 3
        2000,   // final_boss 4
        1,      // Crate 1
        1,      // Crate 2
        1,      // Crate 3
        1,      // Pod egg
        1,      // morphing_spider_mutant
        1,      // morphing_reptilian_warrior
        1,      // morphing_Mutant Human Type 2
    },

    {
        25,     // Rent-A-Cop
        23,     // Hang Terrot
        23,     // general scientist
        160,    // pod alien
        112,    // Electro-Alien
        1,      // Electro-Sphere
        150,    // ProGuard
        180,    // Genetic Guard
        155,    // Mutant Human Type 1
        155,    // Mutant Human Type 2
        1,      // Large Canister Alien - CONTAINER HP
        225,    // Large Can-Alien - ALIEN HP
        1,      // Small Canister Alien - CONTAINER HP
        180,    // Small Can-Alien - ALIEN HP
        1,      // Gurney Mutant - Waiting HP
        150,    // Gurney Mutant - Spawned HP
        163,    // Liquid Alien -
        325,    // SWAT Guards
        150,    // Goldstern
        4800,   // Morphed Goldstern
        63,     // Volatile Material Transport
        60,     // Floating Bomb
        175,    // Vital Defence Obj  - DON'T CHANGE
        150,    // spider_mutant
        165,    // breather_beast
        150,    // cyborg_warror
        165,    // reptilian_warrior
        150,    // acid_dragon
        165,    // mech_guardian
        1800,   // final_boss 1
        1900,   // final_boss 2
        2000,   // final_boss 3
        2100,   // final_boss 4
        1,      // Crate 1
        1,      // Crate 2
        1,      // Crate 3
        1,      // Pod egg
        1,      // morphing_spider_mutant
        1,      // morphing_reptilian_warrior
        1,      // morphing_Mutant Human Type 2
    },

    {
        38,     // Rent-A-Cop
        28,     // Hang Terrot
        28,     // general scientist
        210,    // pod alien
        150,    // Electro-Alien
        1,      // Electro-Sphere
        175,    // ProGuard
        240,    // Genetic Guard
        210,    // Mutant Human Type 1
        210,    // Mutant Human Type 2
        1,      // Large Canister Alien - CONTAINER HP
        300,    // Large Can-Alien - ALIEN HP
        1,      // Small Canister Alien - CONTAINER HP
        240,    // Small Can-Alien - ALIEN HP
        1,      // Gurney Mutant - Waiting HP
        200,    // Gurney Mutant - Spawned HP
        210,    // Liquid Alien -
        425,    // SWAT Gaurds
        250,    // Goldstern
        5400,   // Morphed Goldstern
        75,     // Volatile Material Transport
        85,     // Floating Bomb
        175,    // Vital Defence Obj - DON'T CHANGE
        200,    // spider_mutant
        225,    // breather_beast
        200,    // cyborg_warror
        225,    // reptilian_warrior
        200,    // acid_dragon
        225,    // mech_guardian
        1900,   // final_boss 1
        2000,   // final_boss 2
        2100,   // final_boss 3
        2200,   // final_boss 4
        1,      // Crate 1
        1,      // Crate 2
        1,      // Crate 3
        1,      // Pod egg
        1,      // morphing_spider_mutant
        1,      // morphing_reptilian_warrior
        1,      // morphing_Mutant Human Type 2
    }
};


objtype  *projcheck;
unsigned projwall;

unsigned scanvalue;
int      detonators_spawned;
int      barsconnected;
int      morphangleadj;
bool     noShots;
int      morphwaittime;

int      BossShapes[] =
{
    SPR_BOSS1_W1,SPR_BOSS2_W1,SPR_BOSS3_W1,SPR_BOSS4_W1,
    SPR_BOSS5_W1,SPR_BOSS6_W1,SPR_BOSS7_W1,SPR_BOSS8_W1,
    SPR_BOSS9_W1,SPR_BOSS10_W1,
};

int      BossShotShapes[] = {SPR_BOSS1_PROJ1,0,0,0,SPR_BOSS5_PROJ1,0,0,SPR_BOSS10_SPIT1};
int      MorphShapes[] = {SPR_BOSS1_MORPH1,SPR_BOSS4_MORPH1,SPR_MUTHUM2_MORPH1};
int      MorphClass[] = {spider_mutantobj,reptilian_warriorobj,mutant_human2obj};
int      MorphEndShapes[] = {SPR_BOSS1_W1,SPR_BOSS4_W1,SPR_MUTHUM2_W1};
int      MorphSounds[] = {SCANHALTSND,GGUARDHALTSND,DOGBOYHALTSND};

uint32_t SpecialSpawnFlags[] =
{
    FL2_DROP_RKEY,FL2_DROP_YKEY,FL2_DROP_BKEY,FL2_DROP_BFG,
    FL2_DROP_ION,FL2_DROP_DETONATOR,
};


int      grenade_shapes[] =
{
    SPR_GRENADE_FLY3,SPR_GRENADE_FLY3,SPR_GRENADE_FLY2,
    SPR_GRENADE_FLY1,SPR_GRENADE_FLY2,SPR_GRENADE_FLY2,
    SPR_GRENADE_FLY3,SPR_GRENADE_FLY4,0,
};


void    T_Path (objtype *obj);
void    T_Shoot (objtype *obj);
void    T_Shade (objtype *obj);
void    T_Chase (objtype *obj);
void    T_Projectile (objtype *obj);
void    T_Stand (objtype *obj);
void    T_OfsThink (objtype *obj);
void    T_OfsChase (objtype *obj);
void    T_SwatWound (objtype *obj);
void    T_SpaceShip (objtype *obj);
void    T_BlowBack (objtype *obj);
void    T_SmartThink (objtype *obj);
void    T_SmartThought (objtype *obj);
void    T_Action (objtype *obj);
void    T_Projectile (objtype *obj);
void    T_OfsShoot (int which, objtype *obj);
void    T_OfsBounce (objtype *obj);
void    T_BarrierTransition (objtype *obj);
void    T_BarrierShutdown (objtype *obj);
void    T_LiquidStandCheck (objtype *obj);
void    T_LiquidMove (objtype *obj);
void    T_Solid (objtype *obj);
void    T_GoldMorph (objtype *obj);
void    T_GoldMorphWait (objtype *obj);
void    T_FlipShape (objtype *obj);
void    T_SteamObj (objtype *obj);

void    A_ScoutAlert (objtype *obj);
void    A_Beep (objtype *obj);
void    A_Laugh (objtype *obj);
void    A_WarpIn (objtype *obj);
void    A_WarpOut (objtype *obj);

bool    ProjectileTryMove (objtype *obj, fixed deltax, fixed deltay);

void    SphereStartDir (objtype *obj);
int     RandomSphereDir (int which);
void    CheckForcedMove (objtype *obj);

bool    MoveTrappedDiag (objtype *obj);
bool    CheckTrappedDiag (objtype *obj);

void    ExplodeFill (expfill_t *ff, int tilex, int tiley);


/*
=============================================================================

                        OFFSET OBJECT ROUTINES

=============================================================================
=
= * NOTES: OffsetObjects require the use of temp1 of objstruct
=
= * NOTES: THINK function AnimateOfsObj() requires the use of temp3
=          of objstruct
=
=============================================================================
*/

statetype s_ofs_stand = {0,SPR_GENETIC_W1 - SPR_GENETIC_W1,0,T_OfsThink,NULL,&s_ofs_stand};

statetype s_ofs_chase1  = {0,SPR_GENETIC_W1 - SPR_GENETIC_W1,10,T_Chase,NULL,&s_ofs_chase1s};
statetype s_ofs_chase1s = {0,SPR_GENETIC_W1 - SPR_GENETIC_W1,5,NULL,NULL,&s_ofs_chase2};
statetype s_ofs_chase2  = {0,SPR_GENETIC_W2 - SPR_GENETIC_W1,8,T_Chase,NULL,&s_ofs_chase3};
statetype s_ofs_chase3  = {0,SPR_GENETIC_W3 - SPR_GENETIC_W1,10,T_Chase,NULL,&s_ofs_chase3s};
statetype s_ofs_chase3s = {0,SPR_GENETIC_W3 - SPR_GENETIC_W1,5,NULL,NULL,&s_ofs_chase4};
statetype s_ofs_chase4  = {0,SPR_GENETIC_W4 - SPR_GENETIC_W1,8,T_Chase,NULL,&s_ofs_chase1};

statetype s_ofs_pain  = {0,SPR_GENETIC_OUCH - SPR_GENETIC_W1,15,NULL,NULL,&s_ofs_chase1};

statetype s_ofs_die1  = {0,SPR_GENETIC_OUCH - SPR_GENETIC_W1,18,T_BlowBack,A_DeathScream,&s_ofs_die1s};
statetype s_ofs_die1s = {0,SPR_GENETIC_DIE1 - SPR_GENETIC_W1,20,T_BlowBack,NULL,&s_ofs_die2};
statetype s_ofs_die2  = {0,SPR_GENETIC_DIE2 - SPR_GENETIC_W1,22,T_BlowBack,NULL,&s_ofs_die3};
statetype s_ofs_die3  = {0,SPR_GENETIC_DIE3 - SPR_GENETIC_W1,20,T_BlowBack,NULL,&s_ofs_die4};
statetype s_ofs_die4  = {0,SPR_GENETIC_DIE4 - SPR_GENETIC_W1,18,T_BlowBack,NULL,&s_ofs_die5};
statetype s_ofs_die5  = {0,SPR_GENETIC_DEAD - SPR_GENETIC_W1,0,NULL,NULL,&s_ofs_die5};

statetype s_ofs_attack1 = {0,SPR_GENETIC_SWING1 - SPR_GENETIC_W1,20,NULL,NULL,&s_ofs_attack2};
statetype s_ofs_attack2 = {0,SPR_GENETIC_SWING2 - SPR_GENETIC_W1,20,NULL,T_Hit,&s_ofs_attack3};
statetype s_ofs_attack3 = {0,SPR_GENETIC_SWING3 - SPR_GENETIC_W1,30,NULL,NULL,&s_ofs_chase1};

statetype s_ofs_spit1 = {0,SPR_GENETIC_SHOOT1 - SPR_GENETIC_W1,20,NULL,NULL,&s_ofs_spit2};
statetype s_ofs_spit2 = {0,SPR_GENETIC_SHOOT2 - SPR_GENETIC_W1,20,NULL,T_Shoot,&s_ofs_spit3};
statetype s_ofs_spit3 = {0,SPR_GENETIC_SHOOT3 - SPR_GENETIC_W1,10,NULL,T_Shade,&s_ofs_chase1};

statetype s_ofs_shoot1 = {0,SPR_GENETIC_SWING1 - SPR_GENETIC_W1,10,NULL,NULL,&s_ofs_shoot2};
statetype s_ofs_shoot2 = {0,SPR_GENETIC_SWING2 - SPR_GENETIC_W1,10,NULL,T_Shoot,&s_ofs_shoot3};
statetype s_ofs_shoot3 = {0,SPR_GENETIC_SWING3 - SPR_GENETIC_W1,10,NULL,T_Shade,&s_ofs_chase1};

//
// general offset-object states
//
statetype s_ofs_pod_attack1 = {0,SPR_POD_ATTACK1 - SPR_POD_WALK1,8,NULL,NULL,&s_ofs_pod_attack1a};
statetype s_ofs_pod_attack1a = {0,SPR_POD_ATTACK2 - SPR_POD_WALK1,8,NULL,NULL,&s_ofs_attack2};
statetype s_ofs_pod_attack2 = {0,SPR_POD_ATTACK3 - SPR_POD_WALK1,8,NULL,T_Hit,&s_ofs_chase1};

statetype s_ofs_pod_spit1 = {0,SPR_POD_SPIT1 - SPR_POD_WALK1,8,NULL,NULL,&s_ofs_pod_spit2};
statetype s_ofs_pod_spit2 = {0,SPR_POD_SPIT2 - SPR_POD_WALK1,8,NULL,NULL,&s_ofs_pod_spit3};
statetype s_ofs_pod_spit3 = {0,SPR_POD_SPIT3 - SPR_POD_WALK1,8,NULL,T_Shoot,&s_ofs_chase1};

statetype s_ofs_pod_death1 = {0,0,60,NULL,NULL,&s_ofs_pod_death2};
statetype s_ofs_pod_death2 = {0,1,30,NULL,NULL,&s_ofs_pod_death3};
statetype s_ofs_pod_death3 = {0,2,0,NULL,NULL,&s_ofs_pod_death3};

statetype s_ofs_pod_ouch = {0,SPR_POD_OUCH - SPR_POD_WALK1,11,NULL,NULL,&s_ofs_chase1};

statetype s_ofs_bounce = {0,0,10,T_OfsBounce,T_OfsThink,&s_ofs_bounce};

statetype s_ofs_ouch = {0,0,15,T_OfsBounce,NULL,&s_ofs_bounce};

statetype s_ofs_esphere_death1 = {0,0,15,NULL,A_DeathScream,&s_ofs_esphere_death2};
statetype s_ofs_esphere_death2 = {0,1,15,NULL,NULL,&s_ofs_esphere_death3};
statetype s_ofs_esphere_death3 = {0,2,15,NULL,NULL,NULL};

statetype s_ofs_shot1 = {0,0,1,T_Projectile,NULL,&s_ofs_shot2};
statetype s_ofs_shot2 = {0,1,1,T_Projectile,NULL,&s_ofs_shot1};

statetype s_ofs_random = {0,0,1,T_OfsThink,NULL,&s_ofs_random};

statetype s_ofs_static = {0,0,1,NULL,NULL,&s_ofs_static};

statetype s_hold = {0,0,1,NULL,NULL,&s_hold};


/*
===============
=
= SpawnOffsetObj
=
= Returns a pointer to an initialized new actor
=
===============
*/

objtype *SpawnOffsetObj (int which, int tilex, int tiley)
{
    objtype *newobj;
    int     dirwhich = -1;

    switch (which)
    {
        case en_vertsphere:
        case en_horzsphere:
        case en_diagsphere:
            dirwhich = which;
            which = en_electrosphere;
            break;

        case en_bloodvent:
        case en_watervent:
            dirwhich = which;
            which = en_ventdrip;
            break;
    }

    newobj = SpawnNewObj(tilex,tiley,&s_ofs_stand);

    newobj->flags |= FL_SHOOTABLE | FL_SOLID | FL_OFFSET_STATES;
    newobj->obclass = rentacopobj + which;

    switch (which)
    {
        case en_final_boss2:
            newobj->lighting = NO_SHADING;
        case en_spider_mutant:
        case en_breather_beast:
        case en_cyborg_warrior:
        case en_reptilian_warrior:
        case en_acid_dragon:
        case en_mech_guardian:
        case en_final_boss1:
        case en_final_boss3:
        case en_final_boss4:
            newobj->temp1 = BossShapes[which - en_spider_mutant];
            newobj->speed = ALIENSPEED;
            newobj->ammo = ALIENAMMOINIT;
            newobj->flags |= FL_PROJ_TRANSPARENT;
            newobj->flags2 = FL2_BFG_SHOOTABLE | FL2_BFGSHOT_SOLID;
            break;

        case en_green_ooze:
            InitSmartSpeedAnim (newobj,SPR_GREEN_OOZE1,US_RndT() % 3,2,at_CYCLE,ad_FWD,5 + (US_RndT() & 2));
            newobj->flags &= ~(FL_SHOOTABLE | FL_SOLID);
            break;

        case en_black_ooze:
            InitSmartSpeedAnim (newobj,SPR_BLACK_OOZE1,US_RndT() % 3,2,at_CYCLE,ad_FWD,5 + (US_RndT() & 2));
            newobj->flags &= ~(FL_SHOOTABLE | FL_SOLID);
            break;

        case en_green2_ooze:
            InitSmartSpeedAnim (newobj,SPR_GREEN2_OOZE1,US_RndT() % 3,2,at_CYCLE,ad_FWD,5 + (US_RndT() & 2));
            newobj->flags &= ~(FL_SHOOTABLE | FL_SOLID);
            break;

        case en_black2_ooze:
            InitSmartSpeedAnim (newobj,SPR_BLACK2_OOZE1,US_RndT() % 3,2,at_CYCLE,ad_FWD,5 + (US_RndT() & 2));
            newobj->flags &= ~(FL_SHOOTABLE | FL_SOLID);
            break;

        case en_crate1:
        case en_crate2:
        case en_crate3:
            //
            // NOTE: ->temp2 is modified in ScanInfoPlane to determine if
            //       this object is a "blastable" or not
            //
            newobj->temp1 = SPR_CRATE_1 + which - en_crate1;
            newobj->flags |= FL_NO_SLIDE | FL_FAKE_STATIC;
            break;

        case en_rotating_cube:
            InitSmartSpeedAnim (newobj,SPR_CUBE1,0,9,at_CYCLE,ad_FWD,5);
            newobj->flags2 = FL2_BFGSHOT_SOLID;
            newobj->lighting = LAMP_ON_SHADING;
            break;

        case en_ventdrip:
            if (dirwhich == en_bloodvent)
                newobj->temp1 = SPR_BLOOD_DRIP1;
            else
                newobj->temp1 = SPR_WATER_DRIP1;

            newobj->temp2 = 5 + (US_RndT() % 10);
            newobj->temp3 = 0;
            NewState (newobj,&s_ofs_random);
            newobj->flags &= ~(FL_SHOOTABLE | FL_SOLID);
            break;

        case en_plasma_detonator:
        case en_plasma_detonator_reserve:
            NewState (newobj,&s_ofs_random);
            newobj->temp1 = SPR_DOORBOMB;
            newobj->temp3 = PLASMA_DETONATORS_DELAY;
            newobj->flags &= ~(FL_SOLID|FL_SHOOTABLE);
            if (detonators_spawned++)
                Quit ("Too many Fission/Plasma Detonators in map %d! Must have only one!",gamestate.mapon + 1);
            break;

        case en_flickerlight:
            newobj->temp1 = SPR_DECO_ARC_1;
            newobj->temp2 = (2 + (US_RndT() % 3)) * 60;
            NewState (newobj,&s_ofs_random);
            newobj->flags |= FL_NONMARK | FL_NEVERMARK;
            newobj->flags &= ~(FL_SOLID | FL_SHOOTABLE);
            newobj->lighting = LAMP_ON_SHADING;
            break;

        case en_security_light:
            newobj->flags &= ~(FL_SOLID | FL_SHOOTABLE);
            NewState (newobj,&s_security_light);
            break;

        case en_electrosphere:
            newobj->trydir = dirwhich;
            newobj->flags &= ~FL_SOLID;
            newobj->temp1 = SPR_ELECTRO_SPHERE_ROAM1;
            newobj->speed = 3096;
            newobj->lighting = NO_SHADING;
            NewState (newobj,&s_ofs_bounce);
            SphereStartDir (newobj);
            break;

        case en_podegg:
            newobj->temp1 = SPR_POD_EGG;

            if (scanvalue == 0xffff)
                newobj->temp2 = (60 * 5) + (60 * (US_RndT() % 20));
            else
                newobj->temp2 = scanvalue * 60;

            newobj->speed = ALIENSPEED;
            newobj->ammo = ALIENAMMOINIT;

            if (newobj->temp2 == 0xff * 60)
                newobj->flags &= ~FL_SHOOTABLE;

            newobj->flags |= FL_PROJ_TRANSPARENT | FL_NO_SLIDE | FL_FAKE_STATIC;
            newobj->flags2 = FL2_BFGSHOT_SOLID | FL2_BFG_SHOOTABLE;
            break;

        case en_pod:
            newobj->temp1 = SPR_POD_WALK1;
            newobj->speed = SPDPATROL;
            newobj->ammo = -1;
            newobj->flags |= FL_PROJ_TRANSPARENT | FL_NO_SLIDE;
            newobj->flags2 = FL2_BFG_SHOOTABLE;
            break;

        case en_genetic_guard:
            newobj->temp1 = SPR_GENETIC_W1;
            newobj->speed = ALIENSPEED;
            newobj->ammo = ALIENAMMOINIT;
            newobj->flags |= FL_PROJ_TRANSPARENT | FL_NO_SLIDE;
            newobj->flags2 = FL2_BFG_SHOOTABLE;
            break;

        case en_mutant_human1:
            newobj->temp1 = SPR_MUTHUM1_W1;
            newobj->speed = ALIENSPEED;
            newobj->ammo = ALIENAMMOINIT;
            newobj->flags |= FL_PROJ_TRANSPARENT | FL_NO_SLIDE;
            newobj->flags2 = FL2_BFGSHOT_SOLID | FL2_BFG_SHOOTABLE;
            break;

        case en_mutant_human2:
            newobj->temp1 = SPR_MUTHUM2_W1;
            newobj->speed = ALIENSPEED;
            newobj->ammo = ALIENAMMOINIT;
            newobj->flags |= FL_PROJ_TRANSPARENT | FL_NO_SLIDE;
            newobj->flags2 = FL2_BFGSHOT_SOLID | FL2_BFG_SHOOTABLE;
            break;

        case en_scan_wait_alien:
            newobj->temp1 = SPR_SCAN_ALIEN_READY;
            newobj->flags |= FL_STATIONARY | FL_NO_SLIDE | FL_FAKE_STATIC;
            newobj->flags2 = FL2_BFGSHOT_SOLID | FL2_BFG_SHOOTABLE;
            break;

        case en_lcan_wait_alien:
            newobj->temp1 = SPR_LCAN_ALIEN_READY;
            newobj->flags |= FL_STATIONARY | FL_NO_SLIDE | FL_FAKE_STATIC;
            newobj->flags2 = FL2_BFGSHOT_SOLID | FL2_BFG_SHOOTABLE;
            break;

        case en_morphing_spider_mutant:
        case en_morphing_reptilian_warrior:
        case en_morphing_mutanthuman2:
            if (scanvalue == 0xffff)
                newobj->temp2 = 0xffff;   // set to max!   // (60 * 5) + (60 * (US_RndT() % 20));
            else
                newobj->temp2 = scanvalue * 60;

            if (newobj->temp2 == 0xff * 60)
                newobj->flags &= ~FL_SHOOTABLE;

            newobj->speed = ALIENSPEED;
            newobj->ammo = ALIENAMMOINIT;
            newobj->temp1 = MorphShapes[which - en_morphing_spider_mutant];
            newobj->flags |= FL_FAKE_STATIC;
            newobj->flags2 = FL2_BFGSHOT_SOLID | FL2_BFG_SHOOTABLE;
            NewState (newobj,&s_ofs_random);
            break;

        case en_gurney_wait:
            if (scanvalue == 0xffff)
                newobj->temp3 = 0;
            else
                newobj->temp3 = (scanvalue & 0xff) * 60;

            newobj->temp1 = SPR_GURNEY_MUT_READY;
            newobj->flags |= FL_STATIONARY | FL_PROJ_TRANSPARENT | FL_NO_SLIDE | FL_FAKE_STATIC;
            newobj->flags2 = FL2_BFGSHOT_SOLID | FL2_BFG_SHOOTABLE;
            break;

        case en_lcan_alien:
            newobj->temp1 = SPR_LCAN_ALIEN_W1;
            newobj->speed = ALIENSPEED;
            newobj->ammo = ALIENAMMOINIT;
            newobj->flags |= FL_PROJ_TRANSPARENT | FL_NO_SLIDE;
            newobj->flags2 = FL2_BFG_SHOOTABLE;
            break;

        case en_scan_alien:
            newobj->temp1 = SPR_SCAN_ALIEN_W1;
            newobj->speed = ALIENSPEED;
            newobj->ammo = ALIENAMMOINIT;
            newobj->flags |= FL_PROJ_TRANSPARENT;
            newobj->flags2 = FL2_BFG_SHOOTABLE;
            break;

        case en_gurney:
            newobj->temp1 = SPR_GURNEY_MUT_W1;
            newobj->speed = ALIENSPEED;
            newobj->flags |= FL_PROJ_TRANSPARENT | FL_NO_SLIDE;
            newobj->ammo = ALIENAMMOINIT;
            newobj->flags2 = FL2_BFG_SHOOTABLE;
            break;
    }

    CheckForSpecialTile (newobj,tilex,tiley);

    if (which < SPACER1_OBJ)
        newobj->hitpoints = starthitpoints[gamestate.difficulty][which];

    return newobj;
}


/*
===============
=
= T_OfsThink
=
= Think for general Offset Objects
=
= NOTE: This think is used for NON-SmartAnim objects
=
===============
*/

void T_OfsThink (objtype *obj)
{
    int dx,dy,dist;
    int oldofs,ofs;
    int temp;

    switch (obj->obclass)
    {
        case plasma_detonator_reserveobj:
            break;

        case plasma_detonatorobj:
            if (obj->temp3 < tics)
            {
                BlastNearDoors (obj->tilex,obj->tiley);

                obj->lighting = NO_SHADING;
                obj->flags &= ~FL_SHOOTABLE;
                obj->obclass = pd_explosionobj;
                A_DeathScream (obj);
                InitSmartSpeedAnim (obj,SPR_DETONATOR_EXP1,0,7,at_ONCE,ad_FWD,3 + (US_RndT() & 3));
                return;
            }
            else
                obj->temp3 -= tics;

            if (obj->ammo >= tics)
                obj->ammo -= tics;
            else
            {
                obj->ammo = DETONATOR_FLASH_RATE;

                if (obj->temp1 == SPR_DOORBOMB)
                    obj->temp1 = SPR_ALT_DOORBOMB;
                else
                    obj->temp1 = SPR_DOORBOMB;
            }
            break;

        case grenadeobj:
            T_Projectile (obj);

            //
            // check for range and update shape
            //
            if (obj->obclass == grenadeobj)
            {
                //
                // has not exploded yet
                //
                dx = abs(obj->s_tilex - obj->tilex);
                dy = abs(obj->s_tiley - obj->tiley);
                dist = dx > dy ? dx : dy;

                //
                // reached end of range?
                //
                obj->temp1 = grenade_shapes[dist];

                if (!obj->temp1)
                {
                    obj->obclass = gr_explosionobj;
                    InitSmartSpeedAnim (obj,SPR_GRENADE_EXPLODE1,0,4,at_ONCE,ad_FWD,3 + (US_RndT() & 3));
                }
            }

            if (obj->obclass != grenadeobj)
            {
                ExplodeRadius (obj,50 + (US_RndT() & 0x7f),false);
                A_DeathScream (obj);
            }
            break;


        case bfg_shotobj:
            T_Projectile (obj);

            if (obj->temp1 != SPR_BFG_WEAPON_SHOT3 && obj->obclass == bfg_shotobj)
            {
                //
                // has not exploded yet
                //
                dx = abs(obj->s_tilex - obj->tilex);
                dy = abs(obj->s_tiley - obj->tiley);
                dist = dx > dy ? dx : dy;

                obj->temp1 = SPR_BFG_WEAPON_SHOT2 + (dist >> 2);
            }

            if (obj->obclass != bfg_shotobj)
            {
                ExplodeRadius (obj,50 + (US_RndT() & 0x7f),false);
                A_DeathScream (obj);
            }
            break;

        case ventdripobj:
            if (tics < obj->temp2)
            {
                obj->temp2 -= tics;
                break;
            }

            //
            // assign random delay and next shape
            //
            obj->temp2 = 5 + (US_RndT() % 10);

            obj->temp1 -= obj->temp3;
            obj->temp3 = (obj->temp3 + 1) % 4;
            obj->temp1 += obj->temp3;
            break;

        case flickerlightobj:
            if (tics < obj->temp2)
            {
                obj->temp2 -= tics;
                break;
            }

            if (obj->temp1 == SPR_DECO_ARC_1 || (US_RndT() & 15))
            {
                //
                // become darker
                //
                temp = US_RndT() & 1;

                obj->lighting = LAMP_ON_SHADING + ((temp + 3) << 2);
                obj->temp1 = SPR_DECO_ARC_2 + temp;
                obj->temp2 = 2 + (US_RndT() & 1);
            }
            else
            {
                obj->temp1 = SPR_DECO_ARC_1;
                obj->temp2 = (4 + (US_RndT() % 10)) * 60;
                obj->lighting = LAMP_ON_SHADING;
            }
            break;

        case electrosphereobj:
            oldofs = obj->temp1 - SPR_ELECTRO_SPHERE_ROAM1;
            ofs = US_RndT() & 3;

            if (ofs == oldofs)
                ofs = (ofs + 1) & 3;

            obj->temp1 = SPR_ELECTRO_SPHERE_ROAM1 + ofs;
            break;

        case podobj:
            if (obj->flags & FL_VISIBLE)
                FirstSighting (obj);
            break;

        case podeggobj:
#if IN_DEVELOPMENT || TECH_SUPPORT_VERSION
            if (!((Keyboard[sc_6] || Keyboard[sc_7]) && Keyboard[sc_8] && DebugOk))
#endif
            if (!(obj->flags & FL_VISIBLE))
                break;

            if (obj->temp2 == 0xff * 60)
                break;

#if IN_DEVELOPMENT || TECH_SUPPORT_VERSION
            if ((Keyboard[sc_6] || Keyboard[sc_7]) && Keyboard[sc_8] && DebugOk)
                obj->temp2 = 0;
#endif

            if (obj->temp2 > tics)
            {
                obj->temp2 -= tics;
                break;
            }

            PlaySoundLocActor (PODHATCHSND,obj);
            InitSmartSpeedAnim (obj,SPR_POD_HATCH1,0,2,at_ONCE,ad_FWD,7);
            break;

        case morphing_spider_mutantobj:
        case morphing_reptilian_warriorobj:
        case morphing_mutanthuman2obj:
#if IN_DEVELOPMENT || TECH_SUPPORT_VERSION
            if (!((Keyboard[sc_6] || Keyboard[sc_7]) && Keyboard[sc_8] && DebugOk))
#endif
            if (!(obj->flags & FL_VISIBLE))
                break;

#if IN_DEVELOPMENT || TECH_SUPPORT_VERSION
            if ((Keyboard[sc_6] || Keyboard[sc_7]) && Keyboard[sc_8] && DebugOk)
                obj->temp2 = 0;
#endif

            if (obj->temp2 > tics)
            {
                obj->temp2 -= tics;
                break;
            }

            obj->flags &= ~FL_SHOOTABLE;
            InitSmartSpeedAnim (obj,obj->temp1,0,8,at_ONCE,ad_FWD,2);
            break;


        case crate1obj:
        case crate2obj:
        case crate3obj:
        case lcan_wait_alienobj:  // these objs wait till they are destroyed
        case scan_wait_alienobj:  // before doing anything
            break;

        case gurney_waitobj:
            if (obj->flags & FL_VISIBLE)
                SightPlayer (obj);
            break;

        case spider_mutantshotobj:
        case final_boss4shotobj:
        case acid_dragonshotobj:
            obj->temp1 = BossShotShapes[obj->obclass - spider_mutantshotobj] + (US_RndT() % 3);
            T_Projectile (obj);
            break;

        default:
            T_Stand (obj);
            break;
    }
}


/*
===============
=
= RandomSphereDir
=
===============
*/

int RandomSphereDir (int which)
{
    int dir;

    switch (which)
    {
        case en_vertsphere:
            dir = ((US_RndT() % 2) * 4) + 2;
            break;

        case en_horzsphere:
            dir = (US_RndT() % 2) * 4;
            break;

        case en_diagsphere:
            dir = ((US_RndT() % 4) * 2) + 1;
            break;
    }

    return dir;
}


/*
===============
=
= SphereStartDir
=
===============
*/

void SphereStartDir (objtype *obj)
{
    int i;

    actorat[obj->tilex][obj->tiley] = NULL;

    for (i = 0; i < 3; i++)
    {
        obj->dir = RandomSphereDir(en_vertsphere + ((obj->trydir - en_vertsphere + i) % 3));

        if (!TryWalk(obj,true))
        {
            obj->dir = opposite[obj->dir];

            if (!TryWalk(obj,true))
                obj->dir = nodir;
            else
                break;
        }
        else
            break;
    }

    actorat[obj->tilex][obj->tiley] = obj;
}


/*
===============
=
= CheckForcedMove
=
===============
*/

void CheckForcedMove (objtype *obj)
{
    int olddir;

    olddir = obj->dir;
    obj->dir = RandomSphereDir(obj->trydir);

    if (!TryWalk(obj,false))
    {
        obj->dir = opposite[obj->dir];

        if (!TryWalk(obj,false))
            obj->dir = olddir;
    }
}


/*
===============
=
= T_OfsBounce
=
===============
*/

void T_OfsBounce (objtype *obj)
{
    int   oldtx,oldty;
    int   orgx,orgy;
    fixed move,dx,dy,dist;

//
// should Electro-Sphere decrease player's health?
//
    dx = labs(player->x - obj->x);
    dy = labs(player->y - obj->y);
    dist = dx > dy ? dx : dy;

    if (dist < TILEGLOBAL)
    {
        PlaySoundLocActor (ELECARCDAMAGESND,obj);
        TakeDamage (4,obj);
    }

//
// try to get a nodir actor moving again
//
    if (obj->dir == nodir)
    {
        SphereStartDir (obj);

        if (obj->dir == nodir)
            return;
    }

//
// make obj bounce around the map!
//
    move = obj->speed * tics;

    while (move)
    {
        //
        // can actor move without reaching destination tile?
        //
        if (move < obj->distance)
        {
            MoveObj (obj,move);       // yes
            break;
        }

        //
        // fix position to account for round off while moving
        //
        obj->x = ((fixed)obj->tilex << TILESHIFT) + TILECENTER;
        obj->y = ((fixed)obj->tiley << TILESHIFT) + TILECENTER;

        move -= obj->distance;
        obj->distance = TILEGLOBAL;

        //
        // can obj continue moving in current direction?
        //
        oldtx = obj->tilex;
        oldty = obj->tiley;

        if (!TryWalk(obj,true))
        {
            obj->tilex = oldtx;
            obj->tiley = oldty;

            //
            // direction change is based on current direction
            //
            switch (obj->dir)
            {
                case northeast:
                case northwest:
                case southeast:
                case southwest:
                    if (obj->trydir != en_diagsphere)
                    {
                        if (!MoveTrappedDiag(obj))
                            SphereStartDir (obj);

                        continue;
                    }

                    obj->dir = (obj->dir + 2) % 8;      // try 90 degrees to the left

                    if (TryWalk(obj,false))
                        break;

                    obj->dir = opposite[obj->dir];      // try 90 degrees to the right

                    if (TryWalk(obj,false))
                        break;

                    //
                    // must be in a corner
                    //
                    obj->dir = opposite[(obj->dir + 2) % 8];

                    if (!TryWalk(obj,false))
                        obj->dir = nodir;
                    break;

                case north:
                case south:
                    if (obj->trydir != en_vertsphere)
                    {
                        if (!MoveTrappedDiag(obj))
                            SphereStartDir (obj);

                        continue;
                    }

                    obj->dir = opposite[obj->dir];

                    if (!TryWalk(obj,false))
                        obj->dir = nodir;
                    break;

                case east:
                case west:
                    if (obj->trydir != en_horzsphere)
                    {
                        if (!MoveTrappedDiag(obj))
                            SphereStartDir (obj);

                        continue;
                    }

                    obj->dir = opposite[obj->dir];

                    if (!TryWalk(obj,false))
                        obj->dir = nodir;
                    break;
            }

            if (!TryWalk(obj,true))
                obj->dir = nodir;
        }
        else
        {
            orgx = obj->tilex;
            orgy = obj->tiley;

            obj->tilex = oldtx;
            obj->tiley = oldty;

            if (!MoveTrappedDiag(obj))
            {
                obj->tilex = orgx;
                obj->tiley = orgy;
            }
        }
    }
}


/*
===============
=
= MoveTrappedDiag
=
===============
*/

bool MoveTrappedDiag (objtype *obj)
{
    //
    // don't mess with HORZs, VERTs, or normal DIAGs
    //
    if (obj->trydir != en_diagsphere || ((obj->dir >> 1) << 1) != obj->dir)
        return false;

    //
    // when a DIAG gets to an open area, change it to diagonal movement
    //
    if (!CheckTrappedDiag(obj))
        return true;

    //
    // dIAGs have a 50/50 chance of changing direction
    //
    if (US_RndT() & 1)
        return false;

    if (obj->dir == north || obj->dir == south)
        obj->trydir = en_horzsphere;
    else
        obj->trydir = en_vertsphere;

    CheckForcedMove (obj);

    obj->trydir = en_diagsphere;

    return TryWalk(obj,true);
}


/*
===============
=
= CheckTrappedDiag
=
===============
*/

bool CheckTrappedDiag (objtype *obj)
{
    int olddir = obj->dir;

    for (obj->dir = northeast; obj->dir <= southeast; obj->dir += 2)
    {
        if (TryWalk(obj,false))
            break;
    }

    if (obj->dir > southeast)
    {
        obj->dir = olddir;

        return true;
    }
    else
    {
        TryWalk (obj,true);

        return false;
    }
}


/*
===============
=
= FindObj
=
= Searches objlist for an object of the given type at the given tile
=
= A tilex/tiley of -1 == don't care
=
===============
*/

objtype *FindObj (int which, int tilex, int tiley)
{
    objtype *check;

    for (check = player->next; check; check = check->next)
    {
        if (check->obclass == which)
        {
            if (tilex != -1)
            {
                if (tilex != check->tilex)
                    continue;
            }

            if (tiley != -1)
            {
                if (tiley != check->tiley)
                    continue;
            }

            return check;
        }
    }

    return NULL;
}


/*
===============
=
= SpawnHiddenOfs
=
= Spawns a given offset obj at the given tile and then hides the obj at 0,0
=
= For spawning reserve objects that will later be moved from their hidden tile
= to a new location
=
= NOTE: When obj is moved from its hidden tile, you will need to
=       update the area number that obj is standing in
=
===============
*/

void SpawnHiddenOfs (int which, int tilex, int tiley)
{
    objtype *newobj;

    nevermark = true;

    newobj = SpawnOffsetObj(which,tilex,tiley);

    nevermark = false;

    newobj->tilex = 0;
    newobj->tiley = 0;
    newobj->x = TILECENTER;
    newobj->y = TILECENTER;
}


/*
===============
=
= FindHiddenOfs
=
= Finds a hidden ofs obj which was hidden using SpawnHiddenOfs and
= returns a pointer to the obj in objlist
=
= NOTE: When this actor is moved from its gidden tile, you will need
=       to update the area number that this actor is standing in or
=       use MoveHiddenOfs
=
===============
*/

objtype *FindHiddenOfs (int which)
{
    objtype *check;

    check = FindObj(which,-1,-1);

    if (!check)
        Quit ("Unable to find a hidden actor at location 0,0!");

    return check;
}


/*
===============
=
= MoveHiddenOfs
=
= Finds a hidden ofs obj which was hidden using SpawnHiddenOfs and
= will move the obj to the given GLOBAL coords and change the obj's class
=
= Returns a pointer to the object that was moved or NULL on failure
=
= NOTE: Updates the obj's areanumber to the one at the new position
=
===============
*/

objtype *MoveHiddenOfs (int whichclass, int newclass, fixed x, fixed y)
{
    objtype *obj;

    obj = FindHiddenOfs(whichclass);

    if (obj)
    {
        obj->obclass = newclass;
        obj->x = x;
        obj->y = y;
        obj->tilex = x >> TILESHIFT;
        obj->tiley = y >> TILESHIFT;
        obj->areanumber = GetAreaNumber(obj->tilex,obj->tiley);

        return obj;
    }

    return NULL;
}


/*
=============================================================================

                      SMART_ANIM ANIMATION ROUTINES

=============================================================================
*/


/*
==============================
=
= InitSmartAnim
=
= Sets up an object for a SmartAnimation
=
= ShapeNum  - first shape number in anim
= StartOfs  - starting offset in the animation to start with
= MaxOfs    - ending offset in the animation
= AnimType  - type of animation (at_CYCLE,at_ONCE,or at_REBOUND)
= AnimDir   - direction of animation to start (ad_FWD, or ad_REV)
=
= ** This function should ALWAYS be used to init/start SmartAnimations! **
=
= NOTE: It is the programmer's responsiblity to watch bit field ranges on
=       the passed parameters
=
= NOTE: THINK function AnimateOfsObj() requires the use of temp3 of objstruct!
=
==============================
*/

void InitSmartAnimStruct (objtype *obj, int shapenum, int startofs, int maxofs, int type, int dir)
{
    ofs_anim_t AnimStruct;

    AnimStruct.curframe = startofs;
    AnimStruct.maxframe = maxofs;
    AnimStruct.animtype = type;
    AnimStruct.animdir = dir;

    obj->temp1 = shapenum + AnimStruct.curframe;

    *(ofs_anim_t *)&obj->temp3 = AnimStruct;    // TODO: ewww
}

void InitAnim (objtype *obj, int shapenum, int startofs, int maxofs, int type, int dir, int delay, int waitdelay)
{
    InitSmartAnimStruct (obj,shapenum,startofs,maxofs,type,dir);
    obj->s_tilex = waitdelay;
    obj->s_tiley = delay;

    NewState (obj,&s_ofs_smart_anim);
}


//
// "s_ofs_smart_anim" - The following structure is designed for "Smart Animations"
//
// OVERVIEW:  This structure will allow the flexiblity to perform ANY type
//            of animation, custom, etc. using one single structure.
//            All of the animations are using "Offset" techniques, where
//            the current shapenumber of the object is in temp1 of
//            objstruct. Code is then written to handle changing
//            the shapenumber of each object individually.
//
// FUNCTIONS: T_SmartThink - called continuously and is
//            special coded for an object/actor
//
//            T_SmartThought - alled only once allowing for other
//            special coding, (eg. the spawning of another animation at
//            any point in the current animation, death screams, sounds, etc)
//
// * NOTES:   T_SmartThink could be modified to handle generic animations
//            like cycle, once, rebound, etc. using flags in objstruct.
//
//

//statetype s_ofs_smart_anim = {0,0,1,NULL,T_SmartThought,&s_ofs_smart_anim};
//statetype s_ofs_smart_anim2 = {0,0,5,NULL,T_SmartThought,&s_ofs_smart_anim2};

statetype s_ofs_smart_anim = {0,0,1,T_SmartThought,NULL,&s_ofs_smart_anim};
statetype s_ofs_smart_anim2 = {0,0,1,T_SmartThought,NULL,&s_ofs_smart_anim2};


/*
===============
=
= T_SmartThought
=
= A think for offset objects that is called ONLY ONCE
= per state change
=
===============
*/

void T_SmartThought (objtype *obj)
{
    objtype *newobj;
    int     i,avail,total;
    fixed   dx,dy;

    switch (obj->obclass)
    {
        case green_oozeobj:
        case black_oozeobj:
        case green2_oozeobj:
        case black2_oozeobj:
            if (((US_RndT() & 7) == 7) && ((ofs_anim_t *)&obj->temp3)->curframe == 2)
            {
                if (obj->tilex == player->tilex && obj->tiley == player->tiley)
                    TakeDamage (4,obj);
            }
            break;

        case arc_barrierobj:
            if (BARRIER_STATE(obj) == bt_DISABLED)
                return;

            if (US_RndT() < 0x10)
            {
                dx = labs(player->x - obj->x);
                dy = labs(player->y - obj->y);

                if (dy <= 0x16000 && dx <= 0x16000)
                {
                    PlaySoundLocActor (ELECARCDAMAGESND,obj);
                    TakeDamage (4,obj);
                }
            }

        case post_barrierobj:
            //
            // check for turn offs
            //
            if ((unsigned)obj->temp2 != 0xffff)    // TODO: what is this
            {
                if (!gamestate.barrier_table[obj->temp2].on)
                    ToggleBarrier (obj);
            }
            break;

        case volatiletransportobj:
        case floatingbombobj:
            if (obj->lighting)
            {
                //
                // slowly inc back to normal
                //
                obj->lighting += ANIM_INFO(obj)->curframe;

                if (obj->lighting > 0)
                    obj->lighting = 0;
            }
            break;
    }

    if (ANIM_INFO(obj)->animtype)
    {
        if (AnimateOfsObj(obj))
        {
            switch (obj->obclass)
            {
                case morphing_spider_mutantobj:
                case morphing_reptilian_warriorobj:
                case morphing_mutanthuman2obj:
                    dx = obj->obclass - morphing_spider_mutantobj;
                    obj->temp1 = MorphEndShapes[dx];
                    PlaySoundLocActor (MorphSounds[dx],obj);
                    obj->obclass = MorphClass[dx];
                    obj->hitpoints = starthitpoints[gamestate.difficulty][MorphClass[dx] - rentacopobj];
                    obj->flags &= ~FL_FAKE_STATIC;
                    obj->flags |= FL_PROJ_TRANSPARENT | FL_SHOOTABLE;
                    NewState (obj,&s_ofs_chase1);
                    break;

                case podeggobj:
                    obj->flags |= FL_SHOOTABLE;
                    obj->obclass = podobj;
                    obj->temp1 = SPR_POD_WALK1;
                    NewState (obj,&s_ofs_chase1);
                    obj->hitpoints = starthitpoints[gamestate.difficulty][en_pod];
                    break;

                case rotating_cubeobj:
                    DISPLAY_TIMED_MSG (pd_floorunlocked,MP_FLOOR_UNLOCKED,MT_GENERAL);
                    SD_PlaySound (ROLL_SCORESND);
                    obj->lighting = 0;
                    break;

                case inertobj:
                case fixup_inertobj:
                case scan_wait_alienobj:
                case lcan_wait_alienobj:
                case gurney_waitobj:
                    break;

                case gold_morphobj:
                    //
                    // game completed!
                    //
                    playstate = ex_victorious;
                    obj->state = NULL;
                    break;


                case volatiletransportobj:
                case floatingbombobj:
                    NewState (obj,&s_scout_dead);
                    obj->lighting = 0;
                    return;

                default:
                    obj->state = NULL;    // mark for removal
                    break;
            }
        }

        if (ANIM_INFO(obj)->curframe == 3)
        {
            switch (obj->obclass)
            {
                case doorexplodeobj:
                    if (!obj->temp2)
                    {
                        //
                        // make sure that there are at least DR_MIN_STATICS
                        //
                        avail = MAXSTATS;
                        total = (int)(laststatobj - statobjlist);

                        for (i = 0; i < total; i++)
                        {
                            if (statobjlist[i].shapenum != -1)
                                avail--;
                        }

                        if (avail > DR_MIN_STATICS && (US_RndT() & 1))
                            SpawnStatic (obj->tilex,obj->tiley,DOOR_RUBBLE_STATNUM);
                    }
                    break;

                case explosionobj:
                    if (!obj->temp2)
                    {
                        ExplodeRadius (obj,20,true);
                        MakeAlertNoise (obj);
                        obj->temp2 = 1;
                    }
                    break;

                case pd_explosionobj:
                    if (!obj->temp2)
                    {
                        ExplodeRadius (obj,PLASMA_DETONATOR_DAMAGE,true);
                        MakeAlertNoise (obj);
                        obj->temp2 = 1;
                    }

                case bfg_explosionobj:
                    if (!obj->temp2)
                    {
                        ExplodeRadius (obj,BFG_DAMAGE,true);
                        MakeAlertNoise (obj);
                        obj->temp2 = 1;
                    }
                    break;

                case gurney_waitobj:
#ifdef OBJ_RESERV
#if IN_DEVELOPMENT
                    if ((unsigned)obj->temp2 < (uintptr_t)objlist || (unsigned)obj->temp2 > (uintptr_t)&objlist[MAXACTORS])
                        Quit ("Gurney->temp2 out of range!");
#endif
                    if (obj->temp2)
                        RemoveObj ((objtype *)obj->temp2);
#endif
                    SpawnOffsetObj (en_gurney,obj->tilex,obj->tiley);
                    NewState (obj,&s_ofs_static);
                    break;

                case scan_wait_alienobj:
#ifdef OBJ_RESERV
#if IN_DEVELOPMENT
                    if ((unsigned)obj->temp2 < (uintptr_t)objlist || (unsigned)obj->temp2 >= (uintptr_t)&objlist[MAXACTORS])
                        Quit ("Scan->temp2 out of range!");
#endif
                    if (obj->temp2)
                        RemoveObj ((objtype *)obj->temp2);
#endif
                    SpawnOffsetObj (en_scan_alien,obj->tilex,obj->tiley);
                    NewState (obj,&s_ofs_static);
                    break;

                case lcan_wait_alienobj:
#ifdef OBJ_RESERV
#if IN_DEVELOPMENT
                if ((unsigned)obj->temp2 < (uintptr_t)objlist || (unsigned)obj->temp2 >= (uintptr_t)&objlist[MAXACTORS])
                    Quit ("Scan->temp2 out of range!");
#endif
                if (obj->temp2)
                    RemoveObj ((objtype *)obj->temp2);
#endif
                SpawnOffsetObj (en_lcan_alien,obj->tilex,obj->tiley);
                NewState (obj,&s_ofs_static);
                break;
            }
        }

        if (ANIM_INFO(obj)->curframe == 2)
        {
            switch (obj->obclass)
            {
                case volatiletransportobj:
                    if (!(obj->flags & FL_INTERROGATED))
                    {
                        if (US_RndT() < 0x7f)
                        {
                            usedummy = true;

                            newobj = SpawnOffsetObj(en_green_ooze,obj->tilex,obj->tiley);
                            newobj->x = obj->x + (US_RndT() << 7);
                            newobj->y = obj->y + (US_RndT() << 7);
                            newobj->tilex = newobj->x >> TILESHIFT;
                            newobj->tiley = newobj->y >> TILESHIFT;

                            usedummy = false;
                        }
                    }

                case floatingbombobj:
                    if (!(obj->flags & FL_INTERROGATED))
                    {
                        T_ExplodeDamage (obj);
                        obj->flags |= FL_INTERROGATED;
                    }
                    break;
            }
        }
    }
}


/*
==============================
=
= AnimateOfsObj
=
= Animation routine for OffsetObjects
=
= IMPORTANT NOTE: This does NOT work on the same principle as the old
= """"""""""""""  AnimateObj function!
=
= obj->temp1    - holds the current shape number
=
= obj->temp3.maxframe  - frames in anim. (0 == 1 Frame, 1 == 2 Frames, etc)
=
= obj->temp3.curframe  - holds the the shape offset which temp1 is off from
=                        the 1st shape in the anim. ALWAYS POSITIVE NUMBER!
=                        REVerse animations have the "curframe" on the last
=                        offset and temp1 - CURFRAME should equal the first
=                        frame and MAXFRAME should equal the offset for the
=                        first frame
=
= obj->temp3.animdir   - determines the direction of the animation
=
= obj->s_tilex         - current number of tics remaining before advancing
=                        to next frame
=
= obj->s_tiley         - delay tic value per frame - this value is copied
=                        into obj->s_tilex upon reaching the end of a frame
=
= If you want to do a reverse animation then you would need to init
= temp1 to the shape number of the LAST shape, "curframe" to the offset
= value from the FIRST FRAME in anim and set "animdir" to at_REV.
=
= * NOTES: This is an OffsetObject which requires the use of temp1 of
=          objstruct!
=
= * NOTES: The use of a SmartAnim requires the use of temp3 of objstruct!
=          Therefore, any other THINKS (like LookForGoodies) AN NOT be used
=          while this routine is being called also.
=
= * NOTES: ALL SmartAnimations have the SAME animation delay rates! Sorry!
=
= * NOTES: The SmartAnimations use s_tilex & s_tiley for animation delay
=          values - All SMART actors can not be used if they are using
=          a "smart" think!
=
= KS: Using an AnimStruct pointer in objstruct would lift these limitations
=     as well as get rid of the nasty use of type-punning to hold memory
=     addresses in ->temp3. It was a clever method of getting around the
=     memory cost, but there's no reason not to add it now (and it would
=     only have been another 2 bytes per actor in DOS anyway...)
=
==============================
*/

bool AnimateOfsObj (objtype *obj)
{
    bool done = false;

    //
    // check animation delay
    //
    if (obj->s_tilex)
    {
        if (obj->s_tilex < tics)
            obj->s_tilex = 0;
        else
            obj->s_tilex -= tics;

        return false;
    }

    switch (ANIM_INFO(obj)->animtype)
    {
        case at_ONCE:
        case at_CYCLE:
            switch (ANIM_INFO(obj)->animdir)
            {
                case ad_FWD:
                    if (ANIM_INFO(obj)->curframe < ANIM_INFO(obj)->maxframe)
                        AdvanceAnimFWD (obj);
                    else if (ANIM_INFO(obj)->animtype == at_CYCLE)
                    {
                        obj->temp1-= ANIM_INFO(obj)->curframe; // pull shapenum back to start

                        //
                        // reset cycle animation
                        //
                        ANIM_INFO(obj)->curframe = 0;

                        obj->s_tilex = obj->s_tiley;
                    }
                    else
                    {
                        //
                        // terminate ONCE anim type
                        //
                        ANIM_INFO(obj)->animtype = at_NONE;
                        done = true;
                    }
                    break;

            }
            break;
    }

    return done;
}


/*
===============
=
= AdvanceAnimFWD
=
===============
*/

void AdvanceAnimFWD (objtype *obj)
{
    ANIM_INFO(obj)->curframe++;

    obj->temp1++;
    obj->s_tilex = obj->s_tiley;
}


/*
============================================================================

                    WALL SWITCH ACTIVATION

============================================================================
*/


/*
===============
=
= ActivateWallSwitch
=
= Updates the map, actors, and tables for wall switches
=
===============
*/

void ActivateWallSwitch (unsigned iconnum, int tilex, int tiley)
{
    #define UPDATE_OTHER_SWITCHES

    int       states[] = {OFF_SWITCH,ON_SWITCH};
    int       mapx,mapy;
    unsigned  icon,num;
    unsigned  newwall;
    barrier_t *barrier;

    if ((iconnum & 0xff00) == 0xf800)
    {
        //
        // update tile map & display switch'd message
        //
        num = iconnum & 0xff;

        barrier = &gamestate.barrier_table[num];
        barrier->on ^= true;
        newwall = tilemap[tilex][tiley] = states[barrier->on];

        DisplaySwitchOperateMsg (num);
        PlaySoundLocActor (SWITCHSND,player);

#ifdef UPDATE_OTHER_SWITCHES
        for (mapy = 0; mapy < mapheight; mapy++)
        {
            for (mapx = 0; mapx < mapwidth; mapx++)
            {
                if (tilemap[mapx][mapy] == OFF_SWITCH || tilemap[mapx][mapy] == ON_SWITCH)
                {
                    icon = MAPSPOT(mapx,mapy,1);

                    if (icon == iconnum)
                    {
                        tilemap[mapx][mapy] = newwall;
                        actorat[mapx][mapy] = (objtype *)(uintptr_t)newwall;
                    }
                }
            }
        }
#endif
    }
    else
    {
        DISPLAY_TIMED_MSG (SwitchNotActivateMsg,MP_WALLSWITCH_OPERATE,MT_GENERAL);
        SD_PlaySound (NOWAYSND);
    }
}


/*
===============
=
= DisplaySwitchOperateMsg
=
===============
*/

void DisplaySwitchOperateMsg (unsigned coords)
{
    const char *msg[2] = {"\r\r DEACTIVATING BARRIER","\r\r  ACTIVATING BARRIER"};
    barrier_t  *barrier;

    barrier = &gamestate.barrier_table[coords];

    DISPLAY_TIMED_MSG (msg[barrier->on != 0],MP_WALLSWITCH_OPERATE,MT_GENERAL);
}


/*
===============
=
= UpdateBarrierTable
=
= Finds/Inserts arc entry in arc list
=
= Returns offset into barrier_table[] for a particular arc
=
===============
*/

unsigned UpdateBarrierTable (int tilex, int tiley, bool OnOff)
{
    barrier_t *barrier;
    int       num;

    //
    // scan table
    //
    barrier = gamestate.barrier_table;

    for (num = 0; num < lengthof(gamestate.barrier_table); num++, barrier++)
    {
        if (barrier->coord.tilex == tilex && barrier->coord.tiley == tiley)
            return num;
        else if (barrier->on == 0xff)    // empty?
        {
            //
            // we have hit end of list - add
            //
            barrier->coord.tilex = tilex;
            barrier->coord.tiley = tiley;
            barrier->on = OnOff;

            return num;
        }
    }

    Quit ("Too many different barriers hooked up to switches!");

    return 0;
}


/*
===============
=
= ScanBarrierTable
=
= Scans a switch table for an arc in this level
=
= Returns : 0xFFFF  - Not found in table
=           barrier - barrier_table of the barrier for [num]
=
===============
*/

unsigned ScanBarrierTable (int tilex, int tiley)
{
    barrier_t *barrier;
    unsigned  num;

    barrier = gamestate.barrier_table;

    for (num = 0; num < lengthof(gamestate.barrier_table); num++, barrier++)
    {
        if (barrier->coord.tilex == tilex && barrier->coord.tiley == tiley)
            return num;     // found switch
    }

    return 0xffff;          // mark as empty
}


/*
===============
=
= CheckActor
=
= Checks to see if the barrier obj is free
=
===============
*/

bool CheckActor (objtype *check, unsigned code)
{
    if ((unsigned)check->temp2 == 0xffff)
    {
        //
        // connect actor to barrier switch (code is index into barrier table)
        //
        check->temp2 = code;  // this actor is no longer a cycle actor

        return true;
    }

    return false;
}


/*
===============
=
= CheckAndConnect
=
===============
*/

int CheckAndConnect (int tilex, int tiley, unsigned code)
{
    objtype *check;
    int     offsets[] = {-1,0,1,0};
    int     i;

    for (check = player; check; check = check->next)
    {
        switch (check->obclass)
        {
            case arc_barrierobj:
            case post_barrierobj:
            case vpost_barrierobj:
            case vspike_barrierobj:
                for (i = 0; i < 4; i++)
                {
                    if (check->tilex == tilex + offsets[i] && check->tiley == tiley + offsets[3 - i])
                    {
                        barsconnected++;

                        if (CheckActor(check,code))
                            CheckAndConnect (tilex + offsets[i],tiley + offsets[3 - i],code);
                    }
                }
                break;
        }
    }

    return barsconnected;
}


/*
===============
=
= ConnectBarriers
=
= Scans objlist and finds the single barrier that is connected
= by switch and checks to see if there are any surrounding barriers
= that need to be connected also
=
===============
*/

void ConnectBarriers (void)
{
    barrier_t *barrier;
    unsigned  num;

    barrier = gamestate.barrier_table;

    for (num = 0; num < lengthof(gamestate.barrier_table); num++, barrier++)
    {
        if (barrier->on != 0xff)
        {
            barsconnected = 0;

            if (!CheckAndConnect(barrier->coord.tilex,barrier->coord.tiley,num))
                Quit ("A barrier switch was not connect to any barriers!");
        }
    }
}


/*
=============================================================================

                        BARRIERS

=============================================================================
*/


statetype s_barrier_transition  = {0,0,15,T_BarrierTransition,NULL,&s_barrier_transition};
statetype s_vpost_barrier       = {0,SPR_VPOST1,15,T_BarrierTransition,NULL,&s_vpost_barrier};
statetype s_spike_barrier       = {0,SPR_VSPIKE1,15,T_BarrierTransition,NULL,&s_spike_barrier};
statetype s_barrier_shutdown    = {0,0,15,T_BarrierShutdown,NULL,&s_barrier_shutdown};


/*
===============
=
= SpawnBarrier
=
===============
*/

void SpawnBarrier (int which, int tilex, int tiley, bool OnOff)
{
    objtype *newobj;

    nevermark = !OnOff;

    newobj = SpawnNewObj(tilex,tiley,&s_ofs_stand);

    nevermark = false;

    if (OnOff)
        newobj->flags = FL_OFFSET_STATES | FL_BARRIER | FL_FAKE_STATIC | FL_SOLID;
    else
        newobj->flags = FL_OFFSET_STATES | FL_BARRIER;

    newobj->obclass = rentacopobj + which;
    BARRIER_STATE(newobj) = OnOff;
    newobj->temp2 = ScanBarrierTable(tilex,tiley);
    newobj->flags2 = FL2_BFGSHOT_SOLID;

    switch (which)
    {
        case en_arc_barrier:
            newobj->flags2 |= FL2_BFG_SHOOTABLE;

            if (OnOff)
            {
                InitSmartSpeedAnim (newobj,SPR_ELEC_ARC1,US_RndT() % 3,2,at_CYCLE,ad_FWD,3 + (US_RndT() & 3));
                newobj->lighting = LAMP_ON_SHADING;
            }
            else
            {
                TurnPostOff (newobj);
                newobj->temp1 = SPR_ELEC_ARC4;
            }
            break;

        case en_post_barrier:
            if (OnOff)
            {
                InitSmartSpeedAnim (newobj,SPR_ELEC_POST1,US_RndT() % 3,2,at_CYCLE,ad_FWD,3 + (US_RndT() & 3));
                newobj->lighting = LAMP_ON_SHADING;
            }
            else
            {
                TurnPostOff (newobj);
                newobj->temp1 = SPR_ELEC_POST4;
            }
            break;

        case en_vpost_barrier:
            NewState (newobj,&s_vpost_barrier);

            if (OnOff)
                newobj->temp1 = SPR_VPOST8 - SPR_VPOST1;
            break;

        case en_vspike_barrier:
            NewState (newobj,&s_spike_barrier);

            if (OnOff)
                newobj->temp1 = SPR_VSPIKE8 - SPR_VSPIKE1;
            break;
    }
}


/*
===============
=
= ToggleBarrier
=
= OBJECT STATES:
=
= bt_ON  -> bt_TURNING_OFF and think is changed to T_BarrierTransition
=
===============
*/

void TurnPostOff (objtype *obj)
{
    NewState (obj,&s_barrier_transition);
    obj->temp3 = 0;
    obj->flags &= ~(FL_SOLID | FL_FAKE_STATIC);
    obj->flags |= (FL_NEVERMARK | FL_NONMARK);
    obj->lighting = 0;
    BARRIER_STATE(obj) = bt_OFF;
}


void TurnPostOn(objtype *obj)
{
    obj->flags |= (FL_SOLID | FL_FAKE_STATIC);
    obj->flags &= ~(FL_NEVERMARK | FL_NONMARK);
    obj->lighting = LAMP_ON_SHADING;
    BARRIER_STATE(obj) = bt_ON;
}


void ToggleBarrier (objtype *obj)
{
    switch (BARRIER_STATE(obj))
    {
        case bt_ON:    // same as closed
        case bt_CLOSING:
            //
            // turn off/open
            //
            switch (obj->obclass)
            {
                case post_barrierobj:
                    obj->temp1 = SPR_ELEC_POST4;
                    TurnPostOff (obj);
                    break;

                case arc_barrierobj:
                    obj->temp1 = SPR_ELEC_ARC4;
                    TurnPostOff (obj);
                    break;

                case vpost_barrierobj:
                case vspike_barrierobj:
                    BARRIER_STATE(obj) = bt_OPENING;
                    break;
            }
            break;

        case bt_OFF:   // same as open
        case bt_OPENING:
            //
            // turn on/closed
            //
            switch (obj->obclass)
            {
                case post_barrierobj:
                    InitSmartSpeedAnim (obj,SPR_ELEC_POST1,US_RndT() % 3,2,at_CYCLE,ad_FWD,3 + (US_RndT() & 3));
                    TurnPostOn (obj);
                    break;

                case arc_barrierobj:
                    InitSmartSpeedAnim (obj,SPR_ELEC_ARC1,US_RndT() % 3,2,at_CYCLE,ad_FWD,3 + (US_RndT() & 3));
                    TurnPostOn (obj);
                    break;

                case vpost_barrierobj:
                case vspike_barrierobj:
                    BARRIER_STATE(obj) = bt_CLOSING;
                    break;
            }
            break;
    }
}


/*
===============
=
= T_BarrierShutdown
=
= This is used ONLY for electric arc barriers
= which "flicker" out when disabled/destroyed
=
===============
*/

void T_BarrierShutdown (objtype *obj)
{
    switch (BARRIER_STATE(obj))
    {
        case bt_DISABLING:
            if (obj->hitpoints)
            {
                if (obj->temp2 < tics)
                {
                    if (obj->temp1 == SPR_ELEC_ARC4)
                    {
                        obj->temp1 = SPR_ELEC_ARC1 + obj->temp3;
                        obj->temp3 = (obj->temp3 + 1) % 4;

                        PlaySoundLocActor (ELECARCDAMAGESND,obj);
                        obj->flags |= (FL_SOLID | FL_FAKE_STATIC);
                        obj->flags &= ~(FL_NEVERMARK | FL_NONMARK);
                        obj->lighting = LAMP_ON_SHADING;
                        obj->temp2 = US_RndT() & 7;
                    }
                    else
                    {
                        obj->temp1 = SPR_ELEC_ARC4;
                        obj->flags &= ~(FL_SOLID | FL_FAKE_STATIC);
                        obj->flags |= (FL_NEVERMARK | FL_NONMARK);
                        obj->lighting = 0;
                        obj->temp2 = (5 + US_RndT()) & 15;
                    }

                    obj->hitpoints--;
                }
                else
                    obj->temp2 -= tics;
            }
            else
                BARRIER_STATE(obj) = bt_DISABLED;
            break;

        case bt_DISABLED:
            obj->flags |= (FL_NEVERMARK | FL_NONMARK);
            obj->flags &= ~(FL_SOLID | FL_FAKE_STATIC);
            obj->lighting = 0;

            if (obj->obclass == post_barrierobj)
                obj->temp1 = SPR_ELEC_POST4;
            else
                obj->temp1 = SPR_ELEC_ARC4;

            obj->temp3 = 0;
            NewState (obj,&s_ofs_smart_anim);
            break;
    }
}


/*
===============
=
= T_BarrierTransition
=
===============
*/

void T_BarrierTransition (objtype *obj)
{
    fixed dx,dy;

    switch (BARRIER_STATE(obj))
    {
        //
        // on/closed position
        //
        case bt_ON:
            if ((unsigned)obj->temp2 == 0xffff)
            {
                if (obj->temp3 < tics)
                    ToggleBarrier (obj);
                else
                    obj->temp3 -= tics;
            }
            else if (!gamestate.barrier_table[obj->temp2].on)
                ToggleBarrier (obj);
            break;


        //
        // off/open position
        //
        case bt_OFF:
            if ((unsigned)obj->temp2 == 0xffff)
            {
                if (obj->temp3 < tics)
                    ToggleBarrier (obj);
                else
                    obj->temp3 -= tics;
            }
            else if (gamestate.barrier_table[obj->temp2].on)
                ToggleBarrier (obj);
            break;

        //
        // closing/turning on
        //
        case bt_CLOSING:
            //
            // check for damaging the player
            //
            dx = labs(player->x - obj->x);
            dy = labs(player->y - obj->y);

            if (dy < 0x18000 && dx < 0x18000)
            {
                if (dy < TILECENTER && dx < TILECENTER)
                    TakeDamage (2,obj);

                break;
            }

            if (obj->temp3 < tics)
            {
                obj->temp3 = VPOST_BARRIER_SPEED;
                obj->temp1++;

                if (obj->temp1 > 7)
                    obj->temp1 = 7;

                switch (obj->temp1)
                {
                    //
                    // closed enough to be solid
                    //
                    case 3:
                        obj->flags |= (FL_SOLID | FL_FAKE_STATIC);
                        obj->flags &= ~(FL_NEVERMARK | FL_NONMARK);
                        break;

                    //
                    // fully closed
                    //
                    case 7:
                        BARRIER_STATE(obj) = bt_ON;

                        if ((unsigned)obj->temp2 == 0xffff)
                            obj->temp3 = VPOST_WAIT_DELAY;
                        break;
                }
            }
            else
                obj->temp3 -= tics;

            //
            // check to see if it was toggled
            //
            if ((unsigned)obj->temp2 != 0xffff)
            {
                if (!gamestate.barrier_table[obj->temp2].on)
                    ToggleBarrier (obj);
            }
            break;

        //
        // opening/turning off
        //
        case bt_OPENING:
            if (obj->temp3 < tics)
            {
                obj->temp3 = VPOST_BARRIER_SPEED;
                obj->temp1--;

                if (obj->temp1 < 0)
                    obj->temp1 = 0;

                switch (obj->temp1)
                {
                    //
                    // open enough not to be solid
                    //
                    case 2:
                        obj->flags &= ~(FL_SOLID | FL_FAKE_STATIC);
                        obj->flags |= (FL_NEVERMARK | FL_NONMARK);
                        break;

                    //
                    // fully open
                    //
                    case 0:
                        BARRIER_STATE(obj) = bt_OFF;

                        if ((unsigned)obj->temp2 == 0xffff)
                            obj->temp3 = VPOST_WAIT_DELAY;
                        break;
                }
            }
            else
                obj->temp3 -= tics;

            if ((unsigned)obj->temp2 != 0xffff)
            {
                if (gamestate.barrier_table[obj->temp2].on)
                    ToggleBarrier (obj);
            }
            break;
    }
}


/*
=============================================================================

                        GUARD

=============================================================================
*/


//
// Rent-A-Cop
//
statetype s_rent_stand = {SF_ROTATE,SPR_RENT_S_1,0,T_Stand,NULL,&s_rent_stand};

statetype s_rent_path1  = {SF_ROTATE,SPR_RENT_W1_1,20,T_Path,NULL,&s_rent_path1s};
statetype s_rent_path1s = {SF_ROTATE,SPR_RENT_W1_1,5,NULL,NULL,&s_rent_path2};
statetype s_rent_path2  = {SF_ROTATE,SPR_RENT_W2_1,15,T_Path,NULL,&s_rent_path3};
statetype s_rent_path3  = {SF_ROTATE,SPR_RENT_W3_1,20,T_Path,NULL,&s_rent_path3s};
statetype s_rent_path3s = {SF_ROTATE,SPR_RENT_W3_1,5,NULL,NULL,&s_rent_path4};
statetype s_rent_path4  = {SF_ROTATE,SPR_RENT_W4_1,15,T_Path,NULL,&s_rent_path1};

statetype s_rent_pain = {SF_PAIN,SPR_RENT_PAIN_1,15,NULL,NULL,&s_rent_chase1};

statetype s_rent_shoot1  = {0,SPR_RENT_SHOOT1,20,NULL,NULL,&s_rent_shoot2};
statetype s_rent_shoot2  = {0,SPR_RENT_SHOOT2,20,NULL,T_Shoot,&s_rent_shoot3};
statetype s_rent_shoot3  = {0,SPR_RENT_SHOOT3,20,NULL,T_Shade,&s_rent_chase1};

statetype s_rent_chase1  = {SF_ROTATE,SPR_RENT_W1_1,10,T_Chase,NULL,&s_rent_chase1s};
statetype s_rent_chase1s = {SF_ROTATE,SPR_RENT_W1_1,3,NULL,NULL,&s_rent_chase2};
statetype s_rent_chase2  = {SF_ROTATE,SPR_RENT_W2_1,8,T_Chase,NULL,&s_rent_chase3};
statetype s_rent_chase3  = {SF_ROTATE,SPR_RENT_W3_1,10,T_Chase,NULL,&s_rent_chase3s};
statetype s_rent_chase3s = {SF_ROTATE,SPR_RENT_W3_1,3,NULL,NULL,&s_rent_chase4};
statetype s_rent_chase4  = {SF_ROTATE,SPR_RENT_W4_1,8,T_Chase,NULL,&s_rent_chase1};

statetype s_rent_die1  = {0,SPR_RENT_DIE_1,17,T_BlowBack,A_DeathScream,&s_rent_die2};
statetype s_rent_die2  = {0,SPR_RENT_DIE_2,21,T_BlowBack,NULL,&s_rent_die3};
statetype s_rent_die3  = {0,SPR_RENT_DIE_3,16,T_BlowBack,NULL,&s_rent_die3s};
statetype s_rent_die3s = {0,SPR_RENT_DIE_4,13,T_BlowBack,NULL,&s_rent_die4};
statetype s_rent_die4  = {0,SPR_RENT_DEAD,0,NULL,NULL,&s_rent_die4};

//
// Bio-tech
//
statetype s_biostand = {SF_ROTATE,SPR_BIO_S_1,0,T_Stand,NULL,&s_biostand};

statetype s_biopath1  = {SF_ROTATE,SPR_BIO_W1_1,20,T_Path,NULL,&s_biopath1s};
statetype s_biopath1s = {SF_ROTATE,SPR_BIO_W1_1,5,NULL,NULL,&s_biopath2};
statetype s_biopath2  = {SF_ROTATE,SPR_BIO_W2_1,15,T_Path,NULL,&s_biopath3};
statetype s_biopath3  = {SF_ROTATE,SPR_BIO_W3_1,20,T_Path,NULL,&s_biopath3s};
statetype s_biopath3s = {SF_ROTATE,SPR_BIO_W3_1,5,NULL,NULL,&s_biopath4};
statetype s_biopath4  = {SF_ROTATE,SPR_BIO_W4_1,15,T_Path,NULL,&s_biopath1};

statetype s_biopain  = {SF_PAIN,SPR_BIO_PAIN_1,15,NULL,NULL,&s_biochase1};

statetype s_bioshoot1  = {0,SPR_BIO_SHOOT1,6,NULL,NULL,&s_bioshoot2};
statetype s_bioshoot2  = {0,SPR_BIO_SHOOT2,20,NULL,T_Shoot,&s_bioshoot3};
statetype s_bioshoot3  = {0,SPR_BIO_SHOOT3,10,NULL,T_Shade,&s_biochase1};

statetype s_biochase1  = {SF_ROTATE,SPR_BIO_W1_1,10,T_Chase,NULL,&s_biochase1s};
statetype s_biochase1s = {SF_ROTATE,SPR_BIO_W1_1,3,NULL,NULL,&s_biochase2};
statetype s_biochase2  = {SF_ROTATE,SPR_BIO_W2_1,8,T_Chase,NULL,&s_biochase3};
statetype s_biochase3  = {SF_ROTATE,SPR_BIO_W3_1,10,T_Chase,NULL,&s_biochase3s};
statetype s_biochase3s = {SF_ROTATE,SPR_BIO_W3_1,3,NULL,NULL,&s_biochase4};
statetype s_biochase4  = {SF_ROTATE,SPR_BIO_W4_1,8,T_Chase,NULL,&s_biochase1};

statetype s_biodie1  = {0,SPR_BIO_DIE_1,15,T_BlowBack,A_DeathScream,&s_biodie2};
statetype s_biodie2  = {0,SPR_BIO_DIE_2,15,T_BlowBack,NULL,&s_biodie3};
statetype s_biodie3  = {0,SPR_BIO_DIE_3,15,T_BlowBack,NULL,&s_biodie4};
statetype s_biodie4  = {0,SPR_BIO_DIE_4,15,T_BlowBack,NULL,&s_biodie5};
statetype s_biodie5  = {0,SPR_BIO_DEAD,0,NULL,NULL,&s_biodie5};


//
// SWAT
//
statetype s_swatstand = {SF_ROTATE,SPR_SWAT_S_1,0,T_Stand,NULL,&s_swatstand};

statetype s_swatpath1  = {SF_ROTATE,SPR_SWAT_W1_1,20,T_Path,NULL,&s_swatpath1s};
statetype s_swatpath1s = {SF_ROTATE,SPR_SWAT_W1_1,5,NULL,NULL,&s_swatpath2};
statetype s_swatpath2  = {SF_ROTATE,SPR_SWAT_W2_1,15,T_Path,NULL,&s_swatpath3};
statetype s_swatpath3  = {SF_ROTATE,SPR_SWAT_W3_1,20,T_Path,NULL,&s_swatpath3s};
statetype s_swatpath3s = {SF_ROTATE,SPR_SWAT_W3_1,5,NULL,NULL,&s_swatpath4};
statetype s_swatpath4  = {SF_ROTATE,SPR_SWAT_W4_1,15,T_Path,NULL,&s_swatpath1};

statetype s_swatpain  = {SF_PAIN,SPR_SWAT_PAIN_1,15,NULL,NULL,&s_swatshoot1};

statetype s_swatshoot1  = {0,SPR_SWAT_SHOOT1,10,NULL,NULL,&s_swatshoot2};
statetype s_swatshoot2  = {0,SPR_SWAT_SHOOT2,20,NULL,T_Shoot,&s_swatshoot3};
statetype s_swatshoot3  = {0,SPR_SWAT_SHOOT3,10,NULL,T_Shade,&s_swatshoot4};
statetype s_swatshoot4  = {0,SPR_SWAT_SHOOT2,10,NULL,T_Shoot,&s_swatshoot5};
statetype s_swatshoot5  = {0,SPR_SWAT_SHOOT3,10,NULL,T_Shade,&s_swatshoot6};
statetype s_swatshoot6  = {0,SPR_SWAT_SHOOT2,10,NULL,T_Shoot,&s_swatshoot7};
statetype s_swatshoot7  = {0,SPR_SWAT_SHOOT3,10,NULL,T_Shade,&s_swatchase1};

statetype s_swatchase1  = {SF_ROTATE,SPR_SWAT_W1_1,10,T_Chase,NULL,&s_swatchase1s};
statetype s_swatchase1s = {SF_ROTATE,SPR_SWAT_W1_1,3,NULL,NULL,&s_swatchase2};
statetype s_swatchase2  = {SF_ROTATE,SPR_SWAT_W2_1,8,T_Chase,NULL,&s_swatchase3};
statetype s_swatchase3  = {SF_ROTATE,SPR_SWAT_W3_1,10,T_Chase,NULL,&s_swatchase3s};
statetype s_swatchase3s = {SF_ROTATE,SPR_SWAT_W3_1,3,NULL,NULL,&s_swatchase4};
statetype s_swatchase4  = {SF_ROTATE,SPR_SWAT_W4_1,8,T_Chase,NULL,&s_swatchase1};

statetype s_swatwounded1 = {0,SPR_SWAT_WOUNDED1,10,NULL,NULL,&s_swatwounded2};
statetype s_swatwounded2 = {0,SPR_SWAT_WOUNDED2,10,NULL,NULL,&s_swatwounded3};
statetype s_swatwounded3 = {0,SPR_SWAT_WOUNDED3,10,NULL,NULL,&s_swatwounded4};
statetype s_swatwounded4 = {0,SPR_SWAT_WOUNDED4,10,T_SwatWound,NULL,&s_swatwounded4};

statetype s_swatunwounded1 = {0,SPR_SWAT_WOUNDED4,10,NULL,NULL,&s_swatunwounded2};
statetype s_swatunwounded2 = {0,SPR_SWAT_WOUNDED3,10,NULL,NULL,&s_swatunwounded3};
statetype s_swatunwounded3 = {0,SPR_SWAT_WOUNDED2,10,NULL,NULL,&s_swatunwounded4};
statetype s_swatunwounded4 = {0,SPR_SWAT_WOUNDED1,10,NULL,T_SwatWound,&s_swatchase1};

statetype s_swatdie1  = {0,SPR_SWAT_DIE_1,20,T_BlowBack,A_DeathScream,&s_swatdie2};
statetype s_swatdie2  = {0,SPR_SWAT_DIE_2,20,T_BlowBack,NULL,&s_swatdie3};
statetype s_swatdie3  = {0,SPR_SWAT_DIE_3,20,T_BlowBack,NULL,&s_swatdie4};
statetype s_swatdie4  = {0,SPR_SWAT_DIE_4,20,T_BlowBack,NULL,&s_swatdie5};
statetype s_swatdie5  = {0,SPR_SWAT_DEAD,0,NULL,NULL,&s_swatdie5};


//
// PRO Guard
//
statetype s_prostand = {SF_ROTATE,SPR_PRO_S_1,0,T_Stand,NULL,&s_prostand};

statetype s_propath1  = {SF_ROTATE,SPR_PRO_W1_1,20,T_Path,NULL,&s_propath1s};
statetype s_propath1s = {SF_ROTATE,SPR_PRO_W1_1,5,NULL,NULL,&s_propath2};
statetype s_propath2  = {SF_ROTATE,SPR_PRO_W2_1,15,T_Path,NULL,&s_propath3};
statetype s_propath3  = {SF_ROTATE,SPR_PRO_W3_1,20,T_Path,NULL,&s_propath3s};
statetype s_propath3s = {SF_ROTATE,SPR_PRO_W3_1,5,NULL,NULL,&s_propath4};
statetype s_propath4  = {SF_ROTATE,SPR_PRO_W4_1,15,T_Path,NULL,&s_propath1};

statetype s_propain  = {SF_PAIN,SPR_PRO_PAIN_1,15,NULL,NULL,&s_prochase1};

statetype s_proshoot1  = {0,SPR_PRO_SHOOT1,20,NULL,NULL,&s_proshoot2};
statetype s_proshoot2  = {0,SPR_PRO_SHOOT2,20,NULL,T_Shoot,&s_proshoot3};
statetype s_proshoot3  = {0,SPR_PRO_SHOOT3,10,NULL,T_Shade,&s_proshoot4};
statetype s_proshoot4  = {0,SPR_PRO_SHOOT2,10,NULL,T_Shoot,&s_proshoot5};
statetype s_proshoot5  = {0,SPR_PRO_SHOOT3,10,NULL,T_Shade,&s_proshoot6};
statetype s_proshoot6  = {0,SPR_PRO_SHOOT2,10,NULL,T_Shoot,&s_proshoot6a};
statetype s_proshoot6a  = {0,SPR_PRO_SHOOT3,10,NULL,T_Shade,&s_prochase1};

statetype s_prochase1  = {SF_ROTATE,SPR_PRO_W1_1,10,T_Chase,NULL,&s_prochase1s};
statetype s_prochase1s  = {SF_ROTATE,SPR_PRO_W1_1,3,NULL,NULL,&s_prochase2};
statetype s_prochase2  = {SF_ROTATE,SPR_PRO_W2_1,8,T_Chase,NULL,&s_prochase3};
statetype s_prochase3  = {SF_ROTATE,SPR_PRO_W3_1,10,T_Chase,NULL,&s_prochase3s};
statetype s_prochase3s  = {SF_ROTATE,SPR_PRO_W3_1,3,NULL,NULL,&s_prochase4};
statetype s_prochase4  = {SF_ROTATE,SPR_PRO_W4_1,8,T_Chase,NULL,&s_prochase1};

statetype s_prodie1  = {0,SPR_PRO_DIE_1,20,T_BlowBack,A_DeathScream,&s_prodie2};
statetype s_prodie2  = {0,SPR_PRO_DIE_2,20,T_BlowBack,NULL,&s_prodie3};
statetype s_prodie3  = {0,SPR_PRO_DIE_3,20,T_BlowBack,NULL,&s_prodie3a};
statetype s_prodie3a  = {0,SPR_PRO_DIE_4,20,T_BlowBack,NULL,&s_prodie4};
statetype s_prodie4  = {0,SPR_PRO_DEAD,0,NULL,NULL,&s_prodie4};


//
// Electro-Alien
//
statetype s_electro_appear1 = {0,SPR_ELEC_APPEAR1,14,NULL,NULL,&s_electro_appear2};
statetype s_electro_appear2 = {0,SPR_ELEC_APPEAR2,14,NULL,NULL,&s_electro_appear3};
statetype s_electro_appear3 = {0,SPR_ELEC_APPEAR3,14,NULL,NULL,&s_electro_chase1};

statetype s_electro_chase1 = {0,SPR_ELEC_WALK1,14,T_Chase,NULL,&s_electro_chase2};
statetype s_electro_chase2 = {0,SPR_ELEC_WALK2,14,NULL,NULL,&s_electro_chase3};
statetype s_electro_chase3 = {0,SPR_ELEC_WALK3,14,T_Chase,NULL,&s_electro_chase4};
statetype s_electro_chase4 = {0,SPR_ELEC_WALK4,14,NULL,NULL,&s_electro_chase1};

statetype s_electro_ouch = {0,SPR_ELEC_OUCH,14,NULL,NULL,&s_electro_chase1};

statetype s_electro_shoot1 = {0,SPR_ELEC_SHOOT1,14,NULL,NULL,&s_electro_shoot2};
statetype s_electro_shoot2 = {0,SPR_ELEC_SHOOT2,14,T_Shoot,NULL,&s_electro_shoot3};
statetype s_electro_shoot3 = {0,SPR_ELEC_SHOOT3,14,NULL,NULL,&s_electro_chase1};

statetype s_electro_shot1 = {0,SPR_ELEC_SHOT1,1,T_Projectile,NULL,&s_electro_shot2};
statetype s_electro_shot2 = {0,SPR_ELEC_SHOT2,1,T_Projectile,NULL,&s_electro_shot1};

statetype s_electro_die1 = {0,SPR_ELEC_DIE1,14,NULL,A_DeathScream,&s_electro_die2};
statetype s_electro_die2 = {0,SPR_ELEC_DIE2,14,NULL,NULL,&s_electro_die3};
statetype s_electro_die3 = {0,SPR_ELEC_DIE3,14,NULL,NULL,NULL};


//
// Fluid Alien
//
statetype s_liquid_wait = {0,SPR_LIQUID_M1,14,T_Stand,NULL,&s_liquid_wait};

statetype s_liquid_move = {0,SPR_LIQUID_M1,14,T_LiquidMove,T_ChangeShape,&s_liquid_move};

statetype s_liquid_rise1 = {0,SPR_LIQUID_R1,12,NULL,NULL,&s_liquid_rise2};
statetype s_liquid_rise2 = {0,SPR_LIQUID_R2,12,NULL,NULL,&s_liquid_rise3};
statetype s_liquid_rise3 = {0,SPR_LIQUID_R3,12,NULL,T_Solid,&s_liquid_shoot1};

statetype s_liquid_stand = {0,SPR_LIQUID_R4,40,T_LiquidStand,NULL,&s_liquid_stand};

statetype s_liquid_fall1 = {0,SPR_LIQUID_R3,15,NULL,NULL,&s_liquid_fall2};
statetype s_liquid_fall2 = {0,SPR_LIQUID_R2,15,NULL,NULL,&s_liquid_fall3};
statetype s_liquid_fall3 = {0,SPR_LIQUID_R1,15,NULL,NULL,&s_liquid_move};

statetype s_liquid_shoot1 = {0,SPR_LIQUID_S1,12,NULL,NULL,&s_liquid_shoot2};
statetype s_liquid_shoot2 = {0,SPR_LIQUID_S2,12,NULL,NULL,&s_liquid_shoot3};
statetype s_liquid_shoot3 = {0,SPR_LIQUID_S3,12,NULL,T_Shoot,&s_liquid_stand};

statetype s_liquid_ouch = {0,SPR_LIQUID_OUCH,7,NULL,NULL,&s_liquid_shoot1};

statetype s_liquid_die1 = {0,SPR_LIQUID_DIE_1,20,NULL,A_DeathScream,&s_liquid_die2};
statetype s_liquid_die2 = {0,SPR_LIQUID_DIE_2,20,NULL,NULL,&s_liquid_die3};
statetype s_liquid_die3 = {0,SPR_LIQUID_DIE_3,20,NULL,NULL,&s_liquid_die4};
statetype s_liquid_die4 = {0,SPR_LIQUID_DIE_4,20,NULL,NULL,&s_liquid_dead};
statetype s_liquid_dead = {0,SPR_LIQUID_DEAD,20,NULL,NULL,NULL};

statetype s_liquid_shot = {0,0,10,T_Projectile,T_ChangeShape,&s_liquid_shot};


//
// Blake Stone
//
statetype s_blake1 = {0,SPR_BLAKE_W1,12,NULL,NULL,&s_blake2};
statetype s_blake2 = {0,SPR_BLAKE_W2,12,NULL,NULL,&s_blake3};
statetype s_blake3 = {0,SPR_BLAKE_W3,12,NULL,NULL,&s_blake4};
statetype s_blake4 = {0,SPR_BLAKE_W4,12,NULL,NULL,&s_blake1};


/*
===============
=
= T_ChangeShape
=
===============
*/

void T_ChangeShape (objtype *obj)
{
    obj->temp1 = obj->temp2 + (US_RndT() % 3);
}


/*
===============
=
= T_MakeOffset
=
===============
*/

void T_MakeOffset (objtype *obj)
{
    obj->flags |= FL_OFFSET_STATES;
    obj->flags &= ~(FL_SOLID | FL_SHOOTABLE);
}


/*
===============
=
= T_Solid
=
===============
*/

void T_Solid (objtype *obj)
{
    obj->flags |= (FL_SOLID | FL_SHOOTABLE);
}


/*
===============
=
= T_LiquidMove
=
===============
*/

void T_LiquidMove (objtype *obj)
{
    int dx,dy,dist;

    //
    // check to see if obj is VERY close, then FORCE up and
    // start firing
    //
    dx = abs(obj->tilex - player->tilex);
    dy = abs(obj->tiley - player->tiley);
    dist = dx > dy ? dx : dy;

    if (dist < 6 && dx > 1 && dy > 1)
    {
        obj->flags &= ~FL_OFFSET_STATES;
        obj->flags |= FL_SOLID;
        NewState (obj,&s_liquid_rise1);
    }
    else
        T_Chase (obj);
}


/*
===============
=
= T_LiquidStand
=
===============
*/

void T_LiquidStand (objtype *obj)
{
    int dx,dy;

    obj->flags |= FL_SHOOTABLE | FL_SOLID;

    if (US_RndT() < 80 && obj->temp2 < 5)
    {
        obj->temp2++;
        NewState (obj,&s_liquid_shoot1);
    }
    else
    {
        dx = abs(obj->tilex - player->tilex);
        dy = abs(obj->tiley - player->tiley);

        if (dx > 1 || dy > 1)
        {
            if (!(obj->flags & FL_VISIBLE) || US_RndT() < 40 || obj->temp2 == 5)
            {
                NewState (obj,&s_liquid_fall1);
                obj->flags |= FL_OFFSET_STATES;
                obj->flags &= ~(FL_SOLID | FL_SHOOTABLE);
                obj->temp2 = 0;
            }
        }
        else
            obj->temp2 = 0;
    }
}


/*
===============
=
= T_SwatWound
=
===============
*/

void T_SwatWound (objtype *obj)
{
    fixed dx,dy;

    if (obj->state == &s_swatunwounded4)
        obj->flags |= FL_SOLID | FL_SHOOTABLE;
    else
    {
        if (obj->temp2)
        {
            if (tics < obj->temp2)
            {
                obj->temp2 -= tics;

                return;
            }
        }

        obj->temp2 = 0;

        dx = labs(player->x - obj->x);
        dy = labs(player->y - obj->y);

        if (dy > TILEGLOBAL || dx > TILEGLOBAL)
        {
            obj->flags |= FL_SOLID | FL_SHOOTABLE;
            NewState (obj,&s_swatunwounded1);
        }
    }
}


/*
===============
=
= SpawnStand
=
===============
*/

objtype *SpawnStand (int which, int tilex, int tiley, int dir)
{
    objtype *newobj;
    int     ammo = 8;

    switch (which)
    {
        case en_goldstern:
            newobj = SpawnNewObj(tilex,tiley,&s_goldwarp_in1);
            newobj->flags = FL_SHOOTABLE | FL_SOLID;
            newobj->flags2 = FL2_BFG_SHOOTABLE;
            newobj->speed = SPDPATROL;

            if (gamestate.mapon == 9)  // TODO: GOLD_MORPH_LEVEL?
                newobj->hitpoints = starthitpoints[gamestate.difficulty][which] * 15;

            ammo = 25;
            break;

        case en_electro_alien:
            newobj = SpawnNewObj(tilex,tiley,&s_electro_appear1);
            newobj->flags = FL_SHOOTABLE | FL_SOLID | FL_PROJ_TRANSPARENT;
            newobj->speed = SPDPATROL;
            newobj->lighting = NO_SHADING;
            break;

        case en_liquid:
            newobj = SpawnNewObj(tilex,tiley,&s_liquid_wait);
            newobj->flags = FL_OFFSET_STATES | FL_PROJ_TRANSPARENT;
            newobj->flags2 = FL2_BFG_SHOOTABLE;
            newobj->speed = SPDPATROL * 3;
            break;

        case en_rentacop:
            newobj = SpawnNewObj(tilex,tiley,&s_rent_stand);
            newobj->flags = FL_SHOOTABLE | FL_SOLID;
            newobj->flags2 = FL2_BFG_SHOOTABLE;
            newobj->speed = SPDPATROL;
            break;

        case en_gen_scientist:
            newobj = SpawnNewObj(tilex,tiley,&s_biostand);
            newobj->flags = FL_SHOOTABLE | FL_SOLID | FL_FRIENDLY | FL_RANDOM_TURN;
            newobj->flags2 = FL2_BFG_SHOOTABLE;

            if (US_RndT() & 1)
                newobj->flags |= FL_INFORMANT;

            newobj->speed = SPDPATROL;
            break;

        case en_swat:
            newobj = SpawnNewObj(tilex,tiley,&s_swatstand);
            newobj->flags = FL_SHOOTABLE | FL_SOLID;
            newobj->flags2 = FL2_BFG_SHOOTABLE;
            newobj->speed = SPDPATROL;
            ammo = 30;

            if (scanvalue == 0xffff)
                newobj->temp1 = US_RndT() & 1;
            else
                newobj->temp1 = scanvalue;
            break;

        case en_proguard:
            newobj = SpawnNewObj(tilex,tiley,&s_prostand);
            newobj->flags = FL_SHOOTABLE | FL_SOLID;
            newobj->flags2 = FL2_BFG_SHOOTABLE;
            newobj->speed = SPDPATROL;
            ammo = 25;
            break;

        case en_hang_terrot:
            newobj = SpawnNewObj(tilex,tiley,&s_terrot_wait);
            newobj->flags = FL_SHOOTABLE | FL_NONMARK | FL_NEVERMARK;
            newobj->speed = SPDPATROL;
            break;

        case en_floatingbomb:
            newobj = SpawnNewObj(tilex,tiley,&s_scout_stand);
            newobj->speed = SPDPATROL;
            newobj->temp1 = SPR_FSCOUT_W1_1;
            newobj->flags2 = FL2_BFGSHOT_SOLID | FL2_BFG_SHOOTABLE;
            newobj->flags = FL_SHOOTABLE | FL_SOLID | FL_OFFSET_STATES | FL_FAKE_STATIC;
            break;

        case en_volatiletransport:
            newobj = SpawnNewObj(tilex,tiley,&s_scout_stand);
            newobj->speed = SPDPATROL;
            newobj->temp1 = SPR_GSCOUT_W1_1;
            newobj->flags = FL_SHOOTABLE | FL_SOLID | FL_OFFSET_STATES | FL_STATIONARY | FL_FAKE_STATIC;
            newobj->flags2 = FL2_BFGSHOT_SOLID | FL2_BFG_SHOOTABLE;
            break;

        case en_steamgrate:
            newobj = SpawnNewObj(tilex,tiley,&s_steamgrate);
            newobj->flags = FL_OFFSET_STATES;
            newobj->temp1 = SPR_GRATE;
            newobj->temp2 = 60 * 4;
            break;

        case en_steampipe:
            nevermark = true;

            newobj = SpawnNewObj(tilex,tiley,&s_steamgrate);

            nevermark = false;

            newobj->flags = FL_OFFSET_STATES | FL_NONMARK | FL_NEVERMARK;
            newobj->temp1 = SPR_STEAM_PIPE;
            newobj->temp2 = 60 * 4;
            break;
    }

    CheckForSpecialTile (newobj,tilex,tiley);

    newobj->ammo = ammo;
    newobj->obclass = rentacopobj + which;
    newobj->hitpoints += starthitpoints[gamestate.difficulty][which];
    newobj->dir = dir << 1;

    if (newobj->flags & FL_INFORMANT)
    {
        newobj->hitpoints = 1;
        newobj->ammo = 0;
        newobj->flags |= FL_HAS_AMMO | FL_HAS_TOKENS;
        newobj->s_tilex = newobj->s_tiley = 0xff;
    }

    return newobj;
}


/*
===============
=
= CheckForSpecialTile
=
= Adds special attributes to obj if standing on special tiles
=
===============
*/

void CheckForSpecialTile (objtype *obj, int tilex, int tiley)
{
    word *map,*map1;

    //
    // only shootables can use special tiles...
    //
    // (this also tests to make sure that the plasma_detonatorobj &
    //  plasma_detonator_reservedobj will not enter this function)
    //
    if (!(obj->flags & FL_SHOOTABLE))
        return;

    map = &MAPSPOT(tilex,tiley,0);

    switch (*map)
    {
        case CLOAK_AMBUSH_TILE:
            obj->flags2 |= FL2_CLOAKED;
        case AMBUSHTILE:
            obj->flags |= FL_AMBUSH | FL_SHOOTABLE | FL_SOLID;
            break;

        case DETONATOR_TILE:
            SpawnHiddenOfs (en_plasma_detonator_reserve,tilex,tiley);
            obj->flags &= ~FL_INFORMANT;
        case RKEY_TILE:
        case YKEY_TILE:
        case BKEY_TILE:
        case BFG_TILE:
        case ION_TILE:
            ReserveStatic ();
            obj->flags2 |= SpecialSpawnFlags[*map - RKEY_TILE];
            break;

        case CLOAK_TILE:
            obj->flags2 |= FL2_CLOAKED;
            break;

        case LINC_TILE:
            obj->flags2 |= FL2_LINC;
            obj->flags &= ~FL_INFORMANT;    // make sure informants dont have links

            map1 = &MAPSPOT(tilex + 1,tiley,1);
            obj->linc = *map1;
            *map1 = 0;
            break;

        default:
            return;
    }

    *map = obj->areanumber = GetAreaNumber(tilex,tiley);

#if IN_DEVELOPMENT
    if (obj->areanumber >= NUMAREAS)
        Quit ("Actor spawned on wall at %dx%d",tilex,tiley);
#endif
}


/*
===============
=
= SpawnPatrol
=
===============
*/

objtype *SpawnPatrol (int which, int tilex, int tiley, int dir)
{
    objtype *newobj;
    int     ammo = 8;
#if IN_DEVELOPMENT
    int     oldtx,oldty;
#endif
    switch (which)
    {
        case en_blake:
            newobj = SpawnNewObj(tilex,tiley,&s_blake1);
            newobj->speed = SPDPATROL * 2;
            break;

        case en_rentacop:
            newobj = SpawnNewObj(tilex,tiley,&s_rent_path1);
            newobj->flags2 = FL2_BFG_SHOOTABLE;
            newobj->speed = SPDPATROL;
            break;

        case en_gen_scientist:
            newobj = SpawnNewObj(tilex,tiley,&s_biopath1);
            newobj->flags = FL_FRIENDLY | FL_RANDOM_TURN;
            newobj->flags2 = FL2_BFG_SHOOTABLE;

            if (US_RndT() & 1)
                newobj->flags |= FL_INFORMANT;

            newobj->speed = SPDPATROL;
            break;

        case en_proguard:
            newobj = SpawnNewObj(tilex,tiley,&s_propath1);
            newobj->speed = SPDPATROL;
            newobj->flags2 = FL2_BFG_SHOOTABLE;
            ammo = 25;
            break;

        case en_swat:
            newobj = SpawnNewObj(tilex,tiley,&s_swatpath1);
            newobj->speed = SPDPATROL;
            ammo = 30;

            if (scanvalue == 0xffff)
                newobj->temp1 = US_RndT() & 1;
            else
                newobj->temp1 = scanvalue;

            newobj->flags2 = FL2_BFG_SHOOTABLE;
            break;

        case en_floatingbomb:
            newobj = SpawnNewObj(tilex,tiley,&s_scout_path1);
            newobj->speed = SPDPATROL;
            newobj->temp1 = SPR_FSCOUT_W1_1;
            newobj->flags = FL_OFFSET_STATES;
            newobj->flags2 = FL2_BFGSHOT_SOLID | FL2_BFG_SHOOTABLE;
            break;

        case en_volatiletransport:
            newobj = SpawnNewObj(tilex,tiley,&s_scout_path1);
            newobj->speed = SPDPATROL;
            newobj->temp1 = SPR_GSCOUT_W1_1;
            newobj->flags = FL_OFFSET_STATES;
            newobj->flags2 = FL2_BFGSHOT_SOLID | FL2_BFG_SHOOTABLE;
            break;
    }

    newobj->ammo = ammo;
    newobj->obclass = rentacopobj + which;
    newobj->dir = dir << 1;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][which];
    newobj->distance = 0;

    if (newobj->obclass != blakeobj)
        newobj->flags |= FL_SHOOTABLE | FL_SOLID;

    newobj->active = ac_yes;

    if (newobj->flags & FL_INFORMANT)
    {
        newobj->hitpoints = 1;
        newobj->ammo = 0;
        newobj->flags |= FL_HAS_AMMO | FL_HAS_TOKENS;
        newobj->s_tilex = newobj->s_tiley = 0xff;
    }

    CheckForSpecialTile (newobj,tilex,tiley);

    actorat[newobj->tilex][newobj->tiley] = NULL;   // don't use original spot

#if IN_DEVELOPMENT
    oldtx = newobj->tilex;
    oldty = newobj->tiley;
#endif
    TryWalk (newobj,true);

#if IN_DEVELOPMENT
    if (newobj->obclass != blakeobj)
    {
        if ((uintptr_t)actorat[newobj->tilex][newobj->tiley] == BLOCKTILE)
            Quit ("Actor spawned toward a solid static at %dx%d",oldtx,oldty);

        if (GetAreaNumber(newobj->tilex,newobj->tiley) >= NUMAREAS)
            Quit ("Actor spawned toward wall at %dx%d",oldtx,oldty);
    }
#endif
    actorat[newobj->tilex][newobj->tiley] = newobj;

    return newobj;
}


/*
==================
=
= A_DeathScream
=
==================
*/

void A_DeathScream (objtype *obj)
{
    switch (obj->obclass)
    {
        case swatobj:
            {
#if (GAME_VERSION != SHAREWARE_VERSION)
                int sounds[2] =
                {
                    SWATDIESND,
                    SWATDEATH2SND,
                };

                PlaySoundLocActor (sounds[US_RndT() % lengthof(sounds)],obj);
#else
                PlaySoundLocActor (SWATDIESND,obj);
#endif
            }
            break;

        case rentacopobj:
            {
#if (GAME_VERSION != SHAREWARE_VERSION)
                int sounds[2] =
                {
                    RENTDEATH1SND,
                    RENTDEATH2SND,
                };

                PlaySoundLocActor (sounds[US_RndT() % lengthof(sounds)],obj);
#else
                PlaySoundLocActor (RENTDEATH1SND,obj);
#endif
            }
            break;

        case mutant_human1obj:
        case hang_terrotobj:
        case floatingbombobj:
        case volatiletransportobj:
        case explosionobj:
        case gr_explosionobj:
        case bfg_explosionobj:
        case pd_explosionobj:
        case doorexplodeobj:
            {
#if GAME_VERSION != SHAREWARE_VERSION
                int sounds[2] =
                {
                    EXPLODE1SND,
                    EXPLODE2SND,
                };

                PlaySoundLocActor (sounds[US_RndT() % lengthof(sounds)],obj);
#else
                PlaySoundLocActor (EXPLODE1SND,obj);
#endif
            }
            break;

        case rotating_cubeobj:
            SD_PlaySound (VITAL_GONESND);
            break;

#if (GAME_VERSION != SHAREWARE_VERSION)
        case gen_scientistobj:
            if (obj->flags & FL_INFORMANT)
            {
                int sounds[3] =
                {
                    INFORMANTDEATHSND,
                    INFORMDEATH2SND,
                    INFORMDEATH3SND,
                };

                PlaySoundLocActor(sounds[US_RndT() % lengthof(sounds)],obj);
            }
            else
            {
                int sounds[3] =
                {
                    SCIENTISTDEATHSND,
                    SCIDEATH2SND,
                    SCIDEATH3SND,
                };

                PlaySoundLocActor (sounds[US_RndT() % lengthof(sounds)],obj);
            }
            break;
#else
        case gen_scientistobj:
            if (obj->flags & FL_INFORMANT)
                PlaySoundLocActor (INFORMANTDEATHSND,obj);
            else
                PlaySoundLocActor (SCIENTISTDEATHSND,obj);
            break;
#endif
        case breather_beastobj:
        case cyborg_warriorobj:
        case genetic_guardobj:
        case acid_dragonobj:
        case podobj:
            PlaySoundLocActor (PODDEATHSND,obj);
            break;

        case liquidobj:
            PlaySoundLocActor (LIQUIDDIESND,obj);
            break;

        case proguardobj:
            {
#if (GAME_VERSION != SHAREWARE_VERSION)
                int sounds[2] =
                {
                    PROGUARDDEATHSND,
                    PRODEATH2SND,
                };

                PlaySoundLocActor (sounds[US_RndT() % lengthof(sounds)],obj);
#else
                PlaySoundLocActor (PROGUARDDEATHSND,obj);
#endif
            }
            break;

        case final_boss1obj:
        case spider_mutantobj:
            PlaySoundLocActor (BLUEBOYDEATHSND,obj);
            break;

        case mech_guardianobj:
        case final_boss3obj:
        case mutant_human2obj:
            PlaySoundLocActor (DOGBOYDEATHSND,obj);
            break;

        case reptilian_warriorobj:
        case scan_alienobj:
            PlaySoundLocActor (SCANDEATHSND,obj);
            break;

        case lcan_alienobj:
        case final_boss4obj:
            PlaySoundLocActor (LCANDEATHSND,obj);
            break;

        case gurneyobj:
            PlaySoundLocActor (GURNEYDEATHSND,obj);
            break;

        case lcan_wait_alienobj:
            PlaySoundLocActor (LCANBREAKSND,obj);
            break;

        case scan_wait_alienobj:
            PlaySoundLocActor (SCANBREAKSND,obj);
            break;
    }
}


/*
==============================================================================

                            DROP

==============================================================================
*/


/*
==================
=
= DropCargo
=
==================
*/

void DropCargo (objtype *obj)
{
    if (obj->flags2 & FL2_DROP_RKEY)
        PlaceReservedItemNearTile (bo_red_key,obj->tilex,obj->tiley);

    if (obj->flags2 & FL2_DROP_YKEY)
        PlaceReservedItemNearTile (bo_yellow_key,obj->tilex,obj->tiley);

    if (obj->flags2 & FL2_DROP_BKEY)
        PlaceReservedItemNearTile (bo_blue_key,obj->tilex,obj->tiley);

    if (obj->flags2 & FL2_DROP_BFG)
        PlaceReservedItemNearTile (bo_bfg_cannon,obj->tilex,obj->tiley);

    if (obj->flags2 & FL2_DROP_ION)
        PlaceReservedItemNearTile (bo_ion_cannon,obj->tilex,obj->tiley);

    if (obj->flags2 & FL2_DROP_DETONATOR)
        PlaceReservedItemNearTile (bo_plasma_detonator,obj->tilex,obj->tiley);

    if ((obj->flags2 & FL2_LINC) && obj->linc)
        OperateSmartSwitch (obj->linc >> 8,obj->linc & 0xff,ST_TURN_OFF,true);
}


/*
============================================================================

                            STAND

============================================================================
*/


/*
===============
=
= T_Stand
=
===============
*/

void T_Stand (objtype *obj)
{
    SightPlayer (obj);
}


/*
============================================================================

                            CHASE

============================================================================
*/


/*
=================
=
= T_Chase
=
=================
*/

void T_Chase (objtype *obj)
{
    fixed move;
    int   dx,dy,dist,chance;
#ifdef DODGE_N_CHASE
    bool  dodge;
#endif
    bool  nearattack = false;

    obj->flags &= ~FL_LOCKED_STATE;

    if (obj->flags & (FL_STATIONARY | FL_BARRIER_DAMAGE))
        return;

    if (obj->ammo)
    {
#ifdef DODGE_N_CHASE
        dodge = false;
#endif
        if (CheckLine(obj,player) && !PlayerInvisible)      // got a shot at player?
        {
            dx = abs(obj->tilex - player->tilex);
            dy = abs(obj->tiley - player->tiley);
            dist = dx > dy ? dx : dy;

            if (!dist)
                dist = 1;

            if (dist == 1 && obj->distance < TILEGLOBAL / 4)
                nearattack = true;

            //
            // time to toggle shootmode?
            //
            switch (obj->obclass)
            {
                case mutant_human1obj:
                case genetic_guardobj:
                case gurneyobj:
                case podobj:
                case mutant_human2obj:
                case scan_alienobj:
                case lcan_alienobj:
                case spider_mutantobj:
                case breather_beastobj:
                case cyborg_warriorobj:
                case reptilian_warriorobj:
                case acid_dragonobj:
                case mech_guardianobj:
                case gold_morphobj:
                case final_boss1obj:
                case final_boss2obj:
                case final_boss3obj:
                case final_boss4obj:
                    //
                    // check for mode change
                    //
                    if (obj->ammo > tics)
                        obj->ammo -= tics;
                    else
                    {
                        ChangeShootMode (obj);

                        //
                        // move half as far when doing near attacks
                        //
                        if (!(obj->flags & FL_SHOOTMODE))
                            obj->ammo >>= 1;
                    }
                    break;
            }

            if (nearattack)
            {
                //
                // always shoot when in shootmode; never shoot when not
                //
                if (obj->flags & FL_SHOOTMODE)
                    chance = 300;
                else
                    chance = 0;
            }
            else
            {
                switch (obj->obclass)
                {
                    case mutant_human1obj:
                    case genetic_guardobj:
                    case gurneyobj:
                    case podobj:
                    case mutant_human2obj:
                    case scan_alienobj:
                    case lcan_alienobj:
                    case spider_mutantobj:
                    case breather_beastobj:
                    case cyborg_warriorobj:
                    case reptilian_warriorobj:
                    case acid_dragonobj:
                    case mech_guardianobj:
                    case gold_morphobj:
                    case final_boss1obj:
                    case final_boss2obj:
                    case final_boss3obj:
                    case final_boss4obj:
                        if (obj->flags & FL_SHOOTMODE)
                            chance = 300;
                        else
                            chance = 0;
                        break;

                    default:
                        chance = (tics << 4) / dist;
                        break;
                }
            }

            if (US_RndT() < chance && obj->ammo && !(obj->flags & FL_INTERROGATED))
            {
                DoAttack (obj);
                return;
            }

#ifdef DODGE_N_CHASE
            dodge = true;
#endif
        }
        else
            ChangeShootMode (obj);
    }

    if (obj->dir == nodir)
    {
#ifdef DODGE_N_CHASE
        if (dodge)
            SelectDodgeDir (obj);
        else
            SelectChaseDir (obj);
#else
        switch (obj->obclass)
        {
            case floatingbombobj:
                SelectChaseDir (obj);
                break;

            default:
                SelectDodgeDir (obj);
                break;
        }
#endif
        if (obj->dir == nodir)
            return;       // object is blocked in
    }

    move = obj->speed * tics;

    while (move)
    {
        if (obj->distance < 0)
        {
            //
            // waiting for a door to open
            //
            OpenDoor (~obj->distance);

            if (doorobjlist[~obj->distance].action != dr_open)
                return;

            obj->distance = TILEGLOBAL;     // go ahead, the door is now open
        }

        if (move < obj->distance)
        {
            MoveObj (obj,move);
            break;
        }

        //
        // fix position to account for round off while moving
        //
        obj->x = ((fixed)obj->tilex << TILESHIFT) + TILECENTER;
        obj->y = ((fixed)obj->tiley << TILESHIFT) + TILECENTER;

        //
        // reached goal tile, so select another one
        //
        move -= obj->distance;

#ifdef DODGE_N_CHASE
        if (dodge)
            SelectDodgeDir (obj);
        else
            SelectChaseDir (obj);
#else
        switch (obj->obclass)
        {
            case floatingbombobj:
                SelectChaseDir (obj);
                break;

            default:
                SelectDodgeDir (obj);
                break;
        }
#endif
        if (obj->dir == nodir)
            return;       // object is blocked in
    }
}


/*
=================
=
= ChangeShootMode
=
=================
*/

void ChangeShootMode (objtype *obj)
{
    if (obj->flags & FL_SHOOTMODE)
    {
        obj->flags &= ~FL_SHOOTMODE;
        obj->ammo = 60 + (US_RndT() % 60);
    }
    else
    {
        obj->flags |= FL_SHOOTMODE;
        obj->ammo = 1 + (US_RndT() % 2);

        if (obj->obclass == gold_morphobj)
            obj->ammo += 3 + (US_RndT() % 5);
    }
}


/*
=================
=
= DoAttack
=
=================
*/

void DoAttack (objtype *obj)
{
    int dx,dy,dist;

    dx = abs(obj->tilex - player->tilex);
    dy = abs(obj->tiley - player->tiley);
    dist = dx>dy ? dx : dy;

    if (!dist)
        dist = 1;

    switch (obj->obclass)
    {
        case floatingbombobj:
            if (dist <= 1)
            {
                obj->flags &= ~(FL_SHOOTABLE | FL_SOLID);
                obj->flags |= FL_NONMARK | FL_DEADGUY;

                KillActor (obj);

                return;
            }
            break;

        case goldsternobj:
            NewState (obj,&s_goldshoot1);
            break;

        case gold_morphobj:
            NewState (obj,&s_mgold_shoot1);
            break;

        case rentacopobj:
            NewState (obj,&s_rent_shoot1);
            break;

        case gen_scientistobj:
            NewState (obj,&s_bioshoot1);
            break;

        case swatobj:
            NewState (obj,&s_swatshoot1);
            break;

        case liquidobj:
            if ((dx > 2 || dy > 2) && US_RndT() < 30)
            {
                obj->flags &= ~(FL_OFFSET_STATES);
                obj->flags |= FL_SOLID;

                NewState (obj,&s_liquid_rise1);
            }
            break;

        case proguardobj:
            NewState (obj,&s_proshoot1);
            break;

        case electroobj:
            NewState (obj,&s_electro_shoot1);
            break;

        case podobj:
            if (dist > CLOSE_RANGE)
                NewState (obj,&s_ofs_pod_spit1);
            else
                NewState (obj,&s_ofs_pod_attack1);
            break;

        case spider_mutantobj:
        case acid_dragonobj:
        case mech_guardianobj:
        case breather_beastobj:
        case cyborg_warriorobj:
        case reptilian_warriorobj:
        case gurneyobj:
        case mutant_human1obj:
        case final_boss1obj:
        case final_boss2obj:
        case final_boss3obj:
        case final_boss4obj:
            NewState (obj,&s_ofs_shoot1);
            break;

        case genetic_guardobj:
        case mutant_human2obj:
        case lcan_alienobj:
        case scan_alienobj:
            if (dist > CLOSE_RANGE)
                NewState (obj,&s_ofs_spit1);
            else
                NewState (obj,&s_ofs_attack1);
            break;
    }
}


/*
============================================================================

                            PATH

============================================================================
*/


/*
===============
=
= SelectPathDir
=
===============
*/

int SelectPathDir (objtype *obj)
{
    bool     cantwalk = false,randturn = false;
    unsigned spot;

    //
    // look for directional arrows
    //
    spot = MAPSPOT(obj->tilex,obj->tiley,1) - ICONARROWS;

    if (spot < 8)
        obj->dir = spot;

    //
    // reset move distance and try to walk/turn
    //
    obj->distance = TILEGLOBAL;

    if (obj->flags & FL_RANDOM_TURN)
        randturn = US_RndT() > 180;
    else
        randturn = false;

    cantwalk = !TryWalk(obj,false);

    //
    // handle random turns and hitting walls
    //
    if (randturn || cantwalk)
    {
        //
        // directional arrows have priority
        //
        if (!cantwalk && spot < 8)
        {
            if (obj->dir != nodir)
            {
                TryWalk (obj,true);

                obj->trydir = nodir;
            }

            return obj->dir;
        }

        //
        // either path is blocked or obj is randomly turning
        //
        if (obj->trydir == nodir)
            obj->trydir |= US_RndT() & 128;
        else
            obj->dir = obj->trydir & 127;

        if (obj->trydir & 128)
        {
            //
            // turn clockwise
            //
            if (--obj->dir < east)
                obj->dir = nodir - 1;
        }
        else
        {
            //
            // turn counter-clockwise
            //
            if (++obj->dir >= nodir)
                obj->dir = east;
        }

        obj->trydir = (obj->trydir & 128) | obj->dir;

        //
        // walk into new direction
        //
        if (!TryWalk(obj,false))
            obj->dir = nodir;
    }

    if (obj->dir != nodir)
    {
        TryWalk (obj,true);

        obj->trydir = nodir;
    }

    return obj->dir;
}


/*
===============
=
= T_Path
=
===============
*/

void T_Path (objtype *obj)
{
    fixed move;

    if (obj->flags & FL_STATIONARY)
        return;

    switch (obj->obclass)
    {
        case volatiletransportobj:
            break;

        default:
            if (!(obj->flags & FL_FRIENDLY) || madenoise)
            {
                if (SightPlayer(obj))
                    return;
            }

#if LOOK_FOR_DEAD_GUYS
            if (LookForDeadGuys(obj))
                return;
#endif
            break;
    }

    if (obj->dir == nodir)
    {
        if (SelectPathDir(obj) == nodir)
            return;
    }

    move = obj->speed * tics;

    while (move)
    {
        if (obj->distance < 0)
        {
            //
            // waiting for a door to open
            //
            OpenDoor (~obj->distance);

            if (doorobjlist[~obj->distance].action != dr_open)
                return;

            obj->distance = TILEGLOBAL;     // go ahead, the door is now open
        }

        if (move < obj->distance)
        {
            MoveObj (obj,move);
            break;
        }

        if (!obj->tilex || obj->tilex > mapwidth - 1 || !obj->tiley || obj->tiley > mapheight - 1)
            Quit ("T_Path left the map at %ux%u, dir %u",obj->tilex,obj->tiley,obj->dir);

        obj->x = ((fixed)obj->tilex << TILESHIFT) + TILECENTER;
        obj->y = ((fixed)obj->tiley << TILESHIFT) + TILECENTER;
        move -= obj->distance;

        if (SelectPathDir(obj) == nodir)
            return;
    }
}


/*
=============================================================================

                            FIGHT

=============================================================================
*/


/*
===============
=
= T_Shoot
=
===============
*/

void T_Shoot (objtype *obj)
{
    int dx,dy,dist;
    int hitchance,damage;
    int chance;

    switch (obj->obclass)
    {
        case SMART_ACTORS:
            if (!obj->ammo)
                return;
    }

    switch (obj->obclass)
    {
        case electroobj:
        case mutant_human2obj:
            SpawnProjectile (obj,electroshotobj);
            break;

        case liquidobj:
            SpawnProjectile (obj,liquidshotobj);
            break;

        case lcan_alienobj:
            SpawnProjectile(obj,lcanshotobj);
            break;

        case podobj:
            SpawnProjectile (obj,podshotobj);
            break;

        case scan_alienobj:
            SpawnProjectile (obj,scanshotobj);
            break;

        case gold_morphobj:
            SpawnProjectile (obj,goldmorphshotobj);

            if (obj->hitpoints < 500)
                chance = 255 / 2;
            else
                chance = 255 / 4;

            if (US_RndT() < chance)
            {
                morphangleadj = 24;
                SpawnProjectile (obj,goldmorphshotobj);
                morphangleadj = -24;
                SpawnProjectile (obj,goldmorphshotobj);
                morphangleadj = 16;
                SpawnProjectile (obj,goldmorphshotobj);
                morphangleadj = -16;
                SpawnProjectile (obj,goldmorphshotobj);
                morphangleadj = 8;
                SpawnProjectile (obj,goldmorphshotobj);
                morphangleadj = -8;
                SpawnProjectile (obj,goldmorphshotobj);
                morphangleadj = 0;
            }
            break;

        case spider_mutantobj:
        case acid_dragonobj:
            SpawnProjectile (obj,spider_mutantshotobj + (obj->obclass - spider_mutantobj));
            break;

        case final_boss2obj:
            SpawnProjectile (obj,final_boss2shotobj);
            break;

        case final_boss4obj:
            SpawnProjectile (obj,final_boss4shotobj);
            break;

        default:
            hitchance = 128;

            obj->lighting = SHOOT_SHADING;

            if (!areabyplayer[obj->areanumber])
                return;

            switch (obj->obclass)
            {
                case proguardobj:
                case swatobj:
                    PlaySoundLocActor (ATKBURSTRIFLESND,obj);
                    break;

                default:
                    PlaySoundLocActor (ATKCHARGEDSND,obj);
                    break;
            }

            if (!CheckLine(obj,player))         // player is behind a wall
                return;

            dx = abs(obj->tilex - player->tilex);
            dy = abs(obj->tiley - player->tiley);
            dist = dx > dy ? dx : dy;

            if (obj->obclass == swatobj)
            {
                if (dist)
                    dist = (dist * 2) / 3;     // swat are better shots
            }

            if (thrustspeed >= RUNSPEED)
            {
                if (obj->flags & FL_VISIBLE)
                    hitchance = 160 - (dist * 16);  // player can see to dodge
                else
                    hitchance = 160 - (dist * 8);
            }
            else
            {
                if (obj->flags & FL_VISIBLE)
                    hitchance = 256 - (dist * 16);  // player can see to dodge
                else
                    hitchance = 256 - (dist * 8);
            }

            //
            // see if the shot was a hit
            //
            if (US_RndT() < hitchance)
            {
                if (dist < 2)
                    damage = US_RndT() >> 2;
                else if (dist < 4)
                    damage = US_RndT() >> 3;
                else
                    damage = US_RndT() >> 4;

                TakeDamage (damage,obj);
            }

#ifdef LIMITED_AMMO
            switch (obj->obclass)
            {
                case SMART_ACTORS:
                    obj->ammo--;
                    CheckRunChase (obj);
                    break;
            }
#endif
            MakeAlertNoise (obj);
            break;
    }

    switch (obj->obclass)
    {
        case proguardobj:
        case swatobj:
            break;

        default:
            obj->flags &= ~FL_LOCKED_STATE;
            break;
    }
}


/*
===============
=
= T_Shade
=
===============
*/

void T_Shade (objtype *obj)
{
    switch (obj->obclass)
    {
        case final_boss2obj:
            break;

        default:
            obj->lighting = 0;
            break;
    }
}


/*
===============
=
= T_Hit
=
===============
*/

void T_Hit (objtype *obj)
{
    fixed dx,dy;
    int   hitchance,damage;

    switch (obj->obclass)
    {
        case scan_alienobj:
        case lcan_alienobj:
        case podobj:
            hitchance = 220;
            damage = (US_RndT() >> 3) | 1;
            PlaySoundLocActor (CLAWATTACKSND,obj);
            break;

        case genetic_guardobj:
        case mutant_human2obj:
            hitchance = 220;
            damage = (US_RndT() >> 3) | 1;
            PlaySoundLocActor (PUNCHATTACKSND,obj);
            break;

        default:
            hitchance = 200;
            damage = US_RndT() >> 4;
            break;

    }

    MakeAlertNoise (obj);

    dx = labs(player->x - obj->x) - TILEGLOBAL;
    dy = labs(player->y - obj->y) - TILEGLOBAL;

    if (dx <= MINACTORDIST && dy <= MINACTORDIST)
    {
        if (US_RndT() < hitchance)
        {
            TakeDamage (damage,obj);
            return;
        }
    }

    return;
}


/*
============================================================================

                    DR. GOLDSTERN STATES & ROUTINES

============================================================================
*/

statetype s_goldstand  = {SF_ROTATE,SPR_GOLD_S_1,20,T_Stand,NULL,&s_goldpath1};

statetype s_goldpath1  = {SF_ROTATE,SPR_GOLD_W1_1,20,T_Path,NULL,&s_goldpath1s};
statetype s_goldpath1s = {SF_ROTATE,SPR_GOLD_W1_1,5,NULL,NULL,&s_goldpath2};
statetype s_goldpath2  = {SF_ROTATE,SPR_GOLD_W2_1,15,T_Path,NULL,&s_goldpath3};
statetype s_goldpath3  = {SF_ROTATE,SPR_GOLD_W3_1,20,T_Path,NULL,&s_goldpath3s};
statetype s_goldpath3s = {SF_ROTATE,SPR_GOLD_W3_1,5,NULL,NULL,&s_goldpath4};
statetype s_goldpath4  = {SF_ROTATE,SPR_GOLD_W4_1,15,T_Path,NULL,&s_goldpath1};

statetype s_goldpain  = {0,SPR_GOLD_PAIN_1,15,NULL,NULL,&s_goldchase1};

statetype s_goldshoot1  = {0,SPR_GOLD_SHOOT1,20,NULL,NULL,&s_goldshoot2};
statetype s_goldshoot2  = {0,SPR_GOLD_SHOOT2,20,NULL,T_Shoot,&s_goldshoot3};
statetype s_goldshoot3  = {0,SPR_GOLD_SHOOT3,10,NULL,T_Shade,&s_goldshoot4};
statetype s_goldshoot4  = {0,SPR_GOLD_SHOOT2,10,NULL,T_Shoot,&s_goldshoot5};
statetype s_goldshoot5  = {0,SPR_GOLD_SHOOT3,10,NULL,T_Shade,&s_goldshoot6};
statetype s_goldshoot6  = {0,SPR_GOLD_SHOOT2,10,NULL,T_Shoot,&s_goldshoot7};
statetype s_goldshoot7  = {0,SPR_GOLD_SHOOT3,10,NULL,T_Shade,&s_goldchase1};

statetype s_goldchase1  = {SF_ROTATE,SPR_GOLD_W1_1,10,T_Chase,NULL,&s_goldchase1s};
statetype s_goldchase1s = {SF_ROTATE,SPR_GOLD_W1_1,3,NULL,NULL,&s_goldchase2};
statetype s_goldchase2  = {SF_ROTATE,SPR_GOLD_W2_1,8,T_Chase,NULL,&s_goldchase3};
statetype s_goldchase3  = {SF_ROTATE,SPR_GOLD_W3_1,10,T_Chase,NULL,&s_goldchase3s};
statetype s_goldchase3s = {SF_ROTATE,SPR_GOLD_W3_1,3,NULL,NULL,&s_goldchase4};
statetype s_goldchase4  = {SF_ROTATE,SPR_GOLD_W4_1,8,T_Chase,NULL,&s_goldchase1};

statetype s_goldwarp_it  = {0,SPR_GOLD_S_1,45,NULL,A_Laugh,&s_goldwarp_it1};
statetype s_goldwarp_it1 = {0,SPR_GOLD_WRIST_1,25,NULL,NULL,&s_goldwarp_it2};
statetype s_goldwarp_it2 = {0,SPR_GOLD_WRIST_2,25,NULL,A_Beep,&s_goldwarp_it3};
statetype s_goldwarp_it3 = {0,SPR_GOLD_WRIST_1,15,NULL,NULL,&s_goldwarp_it4};
statetype s_goldwarp_it4 = {0,SPR_GOLD_WRIST_2,25,NULL,A_Beep,&s_goldwarp_it5};
statetype s_goldwarp_it5 = {0,SPR_GOLD_S_1,15,NULL,NULL,&s_goldwarp_out1};

statetype s_goldwarp_out1 = {0,SPR_GOLD_WARP1,30,NULL,A_WarpOut,&s_goldwarp_out2};
statetype s_goldwarp_out2 = {0,SPR_GOLD_WARP2,30,NULL,NULL,&s_goldwarp_out3};
statetype s_goldwarp_out3 = {0,SPR_GOLD_WARP3,30,NULL,NULL,&s_goldwarp_out4};
statetype s_goldwarp_out4 = {0,SPR_GOLD_WARP4,30,NULL,NULL,&s_goldwarp_out5};
statetype s_goldwarp_out5 = {0,SPR_GOLD_WARP5,40,NULL,NULL,NULL};

statetype s_goldwarp_in1 = {0,SPR_GOLD_WARP5,15,NULL,A_WarpIn,&s_goldwarp_in2};
statetype s_goldwarp_in2 = {0,SPR_GOLD_WARP4,15,NULL,NULL,&s_goldwarp_in3};
statetype s_goldwarp_in3 = {0,SPR_GOLD_WARP3,15,NULL,NULL,&s_goldwarp_in4};
statetype s_goldwarp_in4 = {0,SPR_GOLD_WARP2,15,NULL,NULL,&s_goldwarp_in5};
statetype s_goldwarp_in5 = {0,SPR_GOLD_WARP1,15,NULL,NULL,&s_goldpath1};

statetype s_goldmorphwait1 = {0,SPR_GOLD_WRIST_1,10,NULL,T_GoldMorphWait,&s_goldmorphwait1};

statetype s_goldmorph1  = {0,SPR_GOLD_MORPH1,10,NULL,NULL,&s_goldmorph2};
statetype s_goldmorph2  = {0,SPR_GOLD_MORPH2,10,NULL,NULL,&s_goldmorph3};
statetype s_goldmorph3  = {0,SPR_GOLD_MORPH3,10,NULL,NULL,&s_goldmorph4};
statetype s_goldmorph4  = {0,SPR_GOLD_MORPH4,10,NULL,NULL,&s_goldmorph5};
statetype s_goldmorph5  = {0,SPR_GOLD_MORPH5,10,NULL,NULL,&s_goldmorph6};
statetype s_goldmorph6  = {0,SPR_GOLD_MORPH6,10,NULL,NULL,&s_goldmorph7};
statetype s_goldmorph7  = {0,SPR_GOLD_MORPH7,10,NULL,NULL,&s_goldmorph8};
statetype s_goldmorph8  = {0,SPR_GOLD_MORPH8,10,NULL,T_GoldMorph,&s_mgold_chase1};

statetype s_mgold_chase1  = {0,SPR_MGOLD_WALK1,10,T_Chase,NULL,&s_mgold_chase2};
statetype s_mgold_chase2  = {0,SPR_MGOLD_WALK2,8,NULL,NULL,&s_mgold_chase3};
statetype s_mgold_chase3  = {0,SPR_MGOLD_WALK3,10,T_Chase,NULL,&s_mgold_chase4};
statetype s_mgold_chase4  = {0,SPR_MGOLD_WALK4,8,NULL,NULL,&s_mgold_chase1};

statetype s_mgold_shoot1  = {0,SPR_MGOLD_ATTACK1,10,NULL,NULL,&s_mgold_shoot2};
statetype s_mgold_shoot2  = {0,SPR_MGOLD_ATTACK2,20,NULL,NULL,&s_mgold_shoot3};
statetype s_mgold_shoot3  = {0,SPR_MGOLD_ATTACK3,14,T_Shoot,NULL,&s_mgold_shoot4};
statetype s_mgold_shoot4  = {0,SPR_MGOLD_ATTACK4,12,T_Shade,NULL,&s_mgold_chase1};

statetype s_mgold_pain   = {0,SPR_MGOLD_OUCH,25,NULL,NULL,&s_mgold_chase1};


/*
===============
=
= T_GoldMorphWait
=
===============
*/

void T_GoldMorphWait (objtype *obj)
{
    morphwaittime -= tics;

    if (morphwaittime < 0)
        NewState (obj,&s_goldmorph1);
}


/*
===============
=
= T_GoldMorph
=
===============
*/

void T_GoldMorph (objtype *obj)
{
    obj->speed = ALIENSPEED << 2;
    obj->ammo = ALIENAMMOINIT;
    obj->flags |= FL_PROJ_TRANSPARENT | FL_NO_SLIDE | FL_SHOOTABLE | FL_SOLID;
    obj->flags2 = FL2_BFGSHOT_SOLID | FL2_BFG_SHOOTABLE;
    obj->hitpoints = starthitpoints[gamestate.difficulty][en_gold_morph];
    obj->obclass = gold_morphobj;

    noShots = false;
}


/*
===============
=
= Play Goldstern sounds
=
===============
*/

void A_Laugh (objtype *obj)
{
    PlaySoundLocActor (GOLDSTERNLAUGHSND,obj);
}

void A_WarpIn (objtype *obj)
{
    PlaySoundLocActor (WARPINSND,obj);
}

void A_WarpOut (objtype *obj)
{
    PlaySoundLocActor (WARPOUTSND,obj);
}

void A_Beep (objtype *obj)
{
    PlaySoundLocActor (ELEV_BUTTONSND,obj);
}


/*
===============
=
= InitGoldsternInfo
=
===============
*/

void InitGoldsternInfo (void)
{
    memset (&GoldsternInfo,0,sizeof(GoldsternInfo));
    GoldsternInfo.LastIndex = GOLDIE_MAX_SPAWNS;
}


/*
============================================================================

                    RED FLASHING SECURITY LAMPS

=============================================================================
*/

statetype s_security_light  = {0,SPR_SECURITY_NORMAL,20,T_Security,T_FlipShape,&s_security_light};


/*
===============
=
= T_FlipShape
=
===============
*/

void T_FlipShape (objtype *obj)
{
    if (obj->flags & FL_ALERTED)
    {
        if (obj->temp1 ^= 1)
            obj->lighting = LAMP_ON_SHADING;
        else
            obj->lighting = 0;
    }
}


/*
===============
=
= T_Security
=
===============
*/

void T_Security (objtype *obj)
{
    if (!(obj->flags & FL_ALERTED))
    {
        if (alerted && areabyplayer[obj->areanumber])
            obj->flags |= FL_ALERTED;
    }
}


/*
============================================================================

                  GROUND & FLOATING SCOUT ROUTINES

============================================================================
*/

statetype s_scout_stand  = {SF_ROTATE,SPR_GSCOUT_W1_1 - SPR_GSCOUT_W1_1,20,T_Stand,NULL,&s_scout_stand};

statetype s_scout_path1  = {SF_ROTATE,SPR_GSCOUT_W1_1 - SPR_GSCOUT_W1_1,15,T_Path,NULL,&s_scout_path1};
statetype s_scout_path2  = {SF_ROTATE,SPR_GSCOUT_W2_1 - SPR_GSCOUT_W1_1,15,T_Path,NULL,&s_scout_path2};
statetype s_scout_path3  = {SF_ROTATE,SPR_GSCOUT_W3_1 - SPR_GSCOUT_W1_1,15,T_Path,NULL,&s_scout_path3};
statetype s_scout_path4  = {SF_ROTATE,SPR_GSCOUT_W4_1 - SPR_GSCOUT_W1_1,15,T_Path,NULL,&s_scout_path4};

statetype s_scout_run   = {SF_ROTATE,SPR_GSCOUT_W1_1 - SPR_GSCOUT_W1_1,10,T_Chase,NULL,&s_scout_run};
statetype s_scout_run2   = {SF_ROTATE,SPR_GSCOUT_W2_1 - SPR_GSCOUT_W1_1,10,T_Chase,NULL,&s_scout_run2};
statetype s_scout_run3   = {SF_ROTATE,SPR_GSCOUT_W3_1 - SPR_GSCOUT_W1_1,10,T_Chase,NULL,&s_scout_run3};
statetype s_scout_run4   = {SF_ROTATE,SPR_GSCOUT_W4_1 - SPR_GSCOUT_W1_1,10,T_Chase,NULL,&s_scout_run4};

statetype s_scout_dead  = {0,SPR_GSCOUT_W1_1 - SPR_GSCOUT_W1_1,20,NULL,NULL,&s_scout_dead};


/*
===============
=
= A_ScoutAlert
=
===============
*/

void A_ScoutAlert (objtype *obj)
{
    PlaySoundLocActor (SCOUT_ALERTSND,obj);
    MakeAlertNoise (obj);
}


/*
===============
=
= T_ExplodeScout
=
===============
*/

void T_ExplodeScout (objtype *obj)
{
    #define RADIUS    (TILEGLOBAL / 4)

    SpawnExplosion (obj->x + RADIUS + (US_RndT() << 5),obj->y + RADIUS + (US_RndT() << 5));
    SpawnExplosion (obj->x - RADIUS - (US_RndT() << 5),obj->y + RADIUS + (US_RndT() << 5));
    SpawnExplosion (obj->x - RADIUS - (US_RndT() << 5),obj->y - RADIUS - (US_RndT() << 5));
    SpawnExplosion (obj->x + RADIUS + (US_RndT() << 5),obj->y - RADIUS - (US_RndT() << 5));
}


/*
===============
=
= T_ExplodeDamage
=
===============
*/

void T_ExplodeDamage (objtype *obj)
{
    ExplodeRadius (obj,EXPLODE_DAMAGE,true);
}

#if GAME_VERSION != SHAREWARE_VERSION
/*
===============
=
= T_PainThink
=
===============
*/

void T_PainThink( objtype *obj)
{
    int fullhp = starthitpoints[gamestate.difficulty][obj->obclass - rentacopobj];

    if (obj->hitpoints > (fullhp >> 1) + (fullhp >> 2))
    {
        //
        // full hp
        //
        switch (obj->obclass)
        {
            case floatingbombobj:
                NewState (obj,&s_scout_run);
                break;

            case volatiletransportobj:
                NewState (obj,&s_scout_path1);
                break;
        }
    }
    else if (obj->hitpoints > (fullhp >> 1))
    {
        //
        // 3/4 hp
        //
        switch (obj->obclass)
        {
            case floatingbombobj:
                NewState (obj,&s_scout_run2);
                break;

            case volatiletransportobj:
                NewState (obj,&s_scout_path2);
                break;
        }
    }
    else if (obj->hitpoints > (fullhp >> 2))
    {
        //
        // 1/2 hp
        //
        switch (obj->obclass)
        {
            case floatingbombobj:
                NewState (obj,&s_scout_run3);
                break;

            case volatiletransportobj:
                NewState (obj,&s_scout_path3);
                break;
        }
    }
    else
    {
        //
        // 1/4 hp
        //
        switch (obj->obclass)
        {
            case floatingbombobj:
                NewState (obj,&s_scout_run4);
            break;

            case volatiletransportobj:
                NewState (obj,&s_scout_path4);
                break;
        }
    }
}
#endif

/*
============================================================================

                        EXPLOSION STUFF

============================================================================
*/


/*
===============
=
= SpawnCusExplosion
=
= Spawns an explosion at the given GLOBAL coordinates
=
===============
*/

void SpawnCusExplosion (fixed x, fixed y, int startframe, int numframes, int delay, int obclass)
{
    objtype *newobj;

    usedummy = nevermark = true;

    newobj = SpawnNewObj(x >> TILESHIFT,y >> TILESHIFT,&s_ofs_smart_anim);

    usedummy = nevermark = false;

    newobj->x = x;
    newobj->y = y;
    newobj->flags = FL_OFFSET_STATES | FL_NONMARK | FL_NEVERMARK;
    newobj->obclass = obclass;
    newobj->lighting = NO_SHADING;
    InitAnim (newobj,startframe,0,numframes,at_ONCE,ad_FWD,US_RndT() & 7,delay);
    A_DeathScream (newobj);
    MakeAlertNoise (newobj);
}


/*
===============
=
= T_SpawnExplosion
=
===============
*/

void T_SpawnExplosion (objtype *obj)
{
    SpawnCusExplosion (obj->x,obj->y,SPR_EXPLOSION_1,4,4,explosionobj);
}


/*
============================================================================

                        STEAM OBJECT STUFF

============================================================================
*/

statetype s_steamgrate  = {0,0,1,T_SteamObj,NULL,&s_steamgrate};

statetype s_steamrelease1 = {0,1,1,NULL,A_DeathScream,&s_steamrelease2};
statetype s_steamrelease2 = {0,2,14,NULL,NULL,&s_steamrelease3};
statetype s_steamrelease3 = {0,3,14,NULL,NULL,&s_steamrelease4};
statetype s_steamrelease4 = {0,2,14,NULL,NULL,&s_steamrelease5};
statetype s_steamrelease5 = {0,3,14,NULL,NULL,&s_steamrelease6};
statetype s_steamrelease6 = {0,4,16,NULL,NULL,&s_steamgrate};


/*
===============
=
= T_SteamObj
=
===============
*/

void T_SteamObj (objtype *obj)
{
    if (obj->flags & FL_VISIBLE)
    {
        //
        // TODO: if the first test fails it will decrement obj->temp2 twice
        //
        if ((obj->temp2 -= tics) <= 0)
        {
            NewState (obj,&s_steamrelease1);
            obj->temp2 = US_RndT() << 3;    // up to 34 seconds
        }
        else
            obj->temp2 -= tics;
    }
}


/*
===============
=
= CheckPosition
=
===============
*/

bool CheckPosition (objtype *obj)
{
    int x,y,xl,yl,xh,yh;
    objtype *check;

    xl = (obj->x - PLAYERSIZE) >> TILESHIFT;
    yl = (obj->y - PLAYERSIZE) >> TILESHIFT;

    xh = (obj->x + PLAYERSIZE) >> TILESHIFT;
    yh = (obj->y + PLAYERSIZE) >> TILESHIFT;

    //
    // check for solid walls
    //
    for (y = yl; y <= yh; y++)
    {
        for (x = xl; x <= xh; x++)
        {
            check = actorat[x][y];

            if (check && !ISPOINTER(check))
                return false;
        }
    }

    return true;
}


/*
=============================================================================

                        HANGING TERROT GUN

=============================================================================
*/

statetype s_terrot_wait  = {SF_ROTATE,SPR_TERROT_1,1,T_Seek,NULL,&s_terrot_wait};
statetype s_terrot_found = {0,SPR_TERROT_1,0,T_Seek,NULL,&s_terrot_found};

statetype s_terrot_shoot1  = {0,SPR_TERROT_FIRE_1,10,NULL,T_Shoot,&s_terrot_shoot2};
statetype s_terrot_shoot2  = {0,SPR_TERROT_FIRE_2,20,NULL,T_Shade,&s_terrot_shoot3};
statetype s_terrot_shoot3  = {0,SPR_TERROT_FIRE_1,10,NULL,T_Shoot,&s_terrot_shoot4};
statetype s_terrot_shoot4  = {0,SPR_TERROT_FIRE_2,20,NULL,T_Shade,&s_terrot_seek1};

statetype s_terrot_seek1  = {SF_ROTATE,SPR_TERROT_1,10,T_Seek,NULL,&s_terrot_seek1s};
statetype s_terrot_seek1s = {SF_ROTATE,SPR_TERROT_1,3, NULL,NULL,&s_terrot_seek1};

statetype s_terrot_die1  = {0,SPR_TERROT_DIE_1,15,NULL,A_DeathScream,&s_terrot_die2};
statetype s_terrot_die2  = {0,SPR_TERROT_DIE_2,15,NULL,NULL,&s_terrot_die3};
statetype s_terrot_die3  = {0,SPR_TERROT_DIE_3,15,NULL,NULL,&s_terrot_die4};
statetype s_terrot_die4  = {0,SPR_TERROT_DIE_4,15,NULL,T_Shade,&s_terrot_die5};
statetype s_terrot_die5  = {0,SPR_TERROT_DEAD ,0,NULL,NULL,&s_terrot_die5};


/*
==============================================================================

                            SEEK

==============================================================================
*/


/*
===============
=
= T_Seek
=
= Will rotate an object (not moving) until it is able
= to see the player in the facing direction
=
===============
*/

void T_Seek (objtype *obj)
{
    #define MAX_VIS_DIST  15

    int  dx,dy,dist,chance;
    bool targetfound;

    targetfound = false;

    if (!PlayerInvisible && (player->tilex != obj->tilex || player->tiley != obj->tiley))
    {
        if (CheckView(obj,player))
        {
            dx = labs(obj->tilex - player->tilex);
            dy = labs(obj->tiley - player->tiley);

            if (dy < MAX_VIS_DIST && dx < MAX_VIS_DIST)
            {
                dist = dx > dy ? dx : dy;

                if (!dist || (dist == 1 && obj->distance < (TILEGLOBAL / 4)))
                    chance = 300;
                else
                    chance = US_RndT() / dist;

                if (US_RndT() < chance)
                {
                    NewState (obj,&s_terrot_shoot1);
                    return;
                }

                targetfound = true;
            }
        }
    }

    if (!(obj->flags & FL_STATIONARY))
    {
        if (targetfound)
            NewState (obj,&s_terrot_found);
        else
        {
            obj->temp2 -= tics;

            if (obj->temp2 <= 0)
            {
                NewState (obj,&s_terrot_seek1);

                obj->dir++;
                obj->temp2 = SEEK_TURN_DELAY;

                if (obj->dir >= nodir)
                    obj->dir = east;
            }
        }
    }
}


/*
===============
=
= SpawnProjectile
=
===============
*/

void SpawnProjectile (objtype *shooter, int obclass)
{
    objtype *newobj;
    int     angleadj;
    int     temp;
    int     tilex,tiley;
    fixed   x,y;

    x = shooter->x;
    y = shooter->y;
    tilex = x >> TILESHIFT;
    tiley = y >> TILESHIFT;
    angleadj = temp = 0;

    usedummy = nevermark = true;

    switch (obclass)
    {
        case spider_mutantshotobj:
        case acid_dragonshotobj:
        case final_boss4shotobj:
            newobj = SpawnNewObj(tilex,tiley,&s_ofs_random);
            PlaySoundLocActor(SPITATTACKSND,newobj);
            newobj->speed = SPDPROJ;
            angleadj = 1 - (US_RndT() & 3);
            newobj->temp1 = BossShotShapes[obclass - spider_mutantshotobj];
            newobj->flags = FL_OFFSET_STATES | FL_PROJ_CHECK_TRANSPARENT | FL_STORED_OBJPTR;
            newobj->temp3 = (uint16_t)shooter;
            break;

        case mut_hum1shotobj:
        case goldmorphshotobj:
        case electroshotobj:
        case final_boss2shotobj:
            newobj = SpawnNewObj(tilex,tiley,&s_ofs_shot1);
            PlaySoundLocActor (ELECTSHOTSND,newobj);
            newobj->speed = SPDPROJ;
            angleadj = 1 - (US_RndT() & 3);
            newobj->temp1 = SPR_ELEC_SHOT1;
            newobj->flags = FL_OFFSET_STATES | FL_PROJ_CHECK_TRANSPARENT | FL_STORED_OBJPTR;
            newobj->temp3 = (uint16_t)shooter;

            switch (obclass)
            {
                case final_boss2shotobj:
                case goldmorphshotobj:
                    newobj->temp1 = SPR_MGOLD_SHOT1;
                case electroshotobj:
                    newobj->lighting = NO_SHADING;
                    break;
            }
            break;

        case lcanshotobj:
        case podshotobj:
            temp = SPR_SPIT3_1 - SPR_SPIT1_1;
        case scanshotobj:
        case dogshotobj:
            newobj = SpawnNewObj(tilex,tiley,&s_liquid_shot);
            PlaySoundLocActor (SPITATTACKSND,newobj);
            newobj->temp2 = SPR_SPIT1_1 + temp;
            newobj->flags = FL_OFFSET_STATES | FL_PROJ_CHECK_TRANSPARENT | FL_STORED_OBJPTR;
            newobj->speed = SPDPROJ + US_RndT();
            angleadj = 2 - (US_RndT() % 5);
            newobj->temp3 = (uint16_t)shooter;
            break;

        case liquidshotobj:
            newobj = SpawnNewObj(tilex,tiley,&s_liquid_shot);
            newobj->temp2 = SPR_LIQUID_SHOT_FLY_1;
            newobj->flags = FL_OFFSET_STATES | FL_PROJ_CHECK_TRANSPARENT | FL_STORED_OBJPTR;
            newobj->speed = SPDPROJ + US_RndT();
            angleadj = 2 - (US_RndT() % 5);
            newobj->s_tilex = newobj->s_tiley = 0;
            newobj->temp3 = (uint16_t)shooter;
            break;

        case grenadeobj:
            newobj = SpawnNewObj(tilex,tiley,&s_ofs_random);
            newobj->speed = SPDPROJ+Random(SPDPROJ >> 1);
            newobj->angle = player->angle + 1 - (US_RndT() & 3);
            newobj->temp1 = grenade_shapes[0];
            newobj->flags = FL_OFFSET_STATES;
            newobj->lighting = NO_SHADING;
            newobj->s_tilex = tilex;
            newobj->s_tiley = tiley;
            break;

        case bfg_shotobj:
            newobj = SpawnNewObj(tilex,tiley,&s_ofs_random);
            newobj->speed = SPDPROJ + Random(SPDPROJ);
            newobj->angle = player->angle + 1 - (US_RndT() & 3);
            newobj->temp1 = SPR_BFG_WEAPON_SHOT2;
            newobj->flags = FL_OFFSET_STATES;
            newobj->lighting = NO_SHADING;
            newobj->s_tilex = tilex;
            newobj->s_tiley = tiley;
            break;
    }

    usedummy = nevermark = false;

    if (newobj == &dummyobj)
        return;

    //
    // restore original position
    //
    newobj->x = x;
    newobj->y = y;

    newobj->active = ac_yes;
    newobj->obclass = obclass;

    if (obclass != grenadeobj && obclass != bfg_shotobj)
        newobj->angle = CalcAngle(newobj,player) + angleadj;

    if (shooter->obclass == gold_morphobj)
        newobj->angle += morphangleadj;

    if (newobj->angle < 0)
        newobj->angle += ANG360;
    else if (newobj->angle >= ANG360)
        newobj->angle -= ANG360;

    newobj->flags |= (FL_NONMARK | FL_NEVERMARK);

    //
    // move grenade slightly in front of player so you can see instant
    // explosions (ie: when you're face up against a wall)
    //
    if (obclass == grenadeobj || obclass == bfg_shotobj)
    {
        newobj->x += FixedMul(MINDIST + (MINDIST >> 3),costable[newobj->angle]);
        newobj->y += -FixedMul(MINDIST + (MINDIST >> 3),sintable[newobj->angle]);
    }
}


/*
===============
=
= ProjectileTryMove
=
===============
*/

bool ProjectileTryMove (objtype *obj, fixed deltax, fixed deltay)
{
    int   xl,yl,xh,yh,x,y,steps;
    fixed dx,dy;

    projwall = 0;
    steps = tics;

    while (steps--)
    {
        obj->x += deltax;
        obj->y += deltay;

        xl = (obj->x - PROJWALLSIZE) >> TILESHIFT;
        yl = (obj->y - PROJWALLSIZE) >> TILESHIFT;

        xh = (obj->x + PROJWALLSIZE) >> TILESHIFT;
        yh = (obj->y + PROJWALLSIZE) >> TILESHIFT;

        //
        // check for solid walls and actors
        //
        for (y = yl; y <= yh; y++)
        {
            for (x = xl; x <= xh; x++)
            {
                projcheck = actorat[x][y];

                if (projcheck && !ISPOINTER(projcheck))
                {
                    if ((uintptr_t)projcheck == BLOCKTILE && !tilemap[x][y])
                    {
                        //
                        // we have a static object
                        //
                        // test for collision radius using CENTER of static
                        // NOT tile size bounds
                        //
                        dx = labs(((x << TILESHIFT) + (TILECENTER - 1)) - obj->x);
                        dy = labs(((y << TILESHIFT) + (TILECENTER - 1)) - obj->y);

                        if (dx < PROJCHECKSIZE && dy < PROJCHECKSIZE)
                        {
                            projcheck = NULL;
                            projwall = 0;
                            obj->tilex = obj->x >> TILESHIFT;
                            obj->tiley = obj->y >> TILESHIFT;

                            return false;
                        }

                    }
                    else
                    {
                        //
                        // we have a wall
                        //
                        projwall = (uintptr_t)projcheck;
                        projcheck = NULL;
                        obj->tilex = obj->x >> TILESHIFT;
                        obj->tiley = obj->y >> TILESHIFT;

                        return false;
                    }
                }
                else if (projcheck)
                {
                    if ((obj->flags & FL_PROJ_CHECK_TRANSPARENT) && (projcheck->flags & FL_PROJ_TRANSPARENT))
                        break;
                    else if (projcheck->flags & FL_SOLID)
                    {
                        obj->tilex = obj->x >> TILESHIFT;
                        obj->tiley = obj->y >> TILESHIFT;

                        return false;
                    }
                }
            }
        }
    }

    return true;
}


/*
===============
=
= T_Projectile
=
===============
*/

void T_Projectile (objtype *obj)
{
    fixed   deltax,deltay;
    int     damage;
    objtype *attacker;

    deltax = FixedMul(obj->speed,costable[obj->angle]);
    deltay = -FixedMul(obj->speed,sintable[obj->angle]);

    projcheck = NULL;

    if (!ProjectileTryMove (obj,deltax,deltay))
    {
        switch (obj->obclass)
        {
            case spider_mutantshotobj:
                InitSmartSpeedAnim (obj,SPR_BOSS1_EXP1,0,2,at_ONCE,ad_FWD,5);
                return;

            case acid_dragonshotobj:
                InitSmartSpeedAnim (obj,SPR_BOSS5_EXP1,0,2,at_ONCE,ad_FWD,5);
                return;

            case mut_hum1shotobj:
            case electroshotobj:      // explode on walls
                InitSmartSpeedAnim (obj,SPR_ELEC_SHOT_EXP1,0,1,at_ONCE,ad_FWD,5 + (US_RndT() & 3));
                return;

            case final_boss2shotobj:
            case goldmorphshotobj:
                InitSmartSpeedAnim (obj,SPR_MGOLD_SHOT_EXP1,0,1,at_ONCE,ad_FWD,5 + (US_RndT() & 3));
                return;

            case final_boss4shotobj:
                InitSmartSpeedAnim (obj,SPR_BOSS10_SPIT_EXP1,0,1,at_ONCE,ad_FWD,5 + (US_RndT() & 3));
                return;

            case lcanshotobj:      // explode on walls
            case podshotobj:
                InitSmartSpeedAnim (obj,SPR_SPIT_EXP3_1,0,2,at_ONCE,ad_FWD,5 + (US_RndT() & 3));
                return;

            case scanshotobj:      // explode on walls
            case dogshotobj:
                InitSmartSpeedAnim (obj,SPR_SPIT_EXP1_1,0,2,at_ONCE,ad_FWD,5 + (US_RndT() & 7));
                return;

            case liquidshotobj:    // explode on walls
                InitSmartSpeedAnim (obj,SPR_LIQUID_SHOT_BURST_1,0,2,at_ONCE,ad_FWD,5 + (US_RndT() & 7));
                return;

            case grenadeobj:
                if (ISPOINTER(projcheck))
                {
                    if (projcheck->flags & FL_SHOOTABLE)
                        DamageActor (projcheck,GR_DAMAGE,obj);
                }

                //
                // mark as exploded
                //
                obj->obclass = gr_explosionobj;
                InitSmartSpeedAnim (obj,SPR_EXPLOSION_1,0,4,at_ONCE,ad_FWD,3 + (US_RndT() & 7));
                A_DeathScream (obj);
                return;

            case bfg_shotobj:
#ifdef BFG_SHOT_STOPS
                //
                // check to see if a collison has already occurred at this
                // tilex and tiley
                //
                if (obj->s_tilex == obj->tilex && obj->s_tiley == obj->tiley)
                    return;

                obj->s_tilex = obj->tilex;
                obj->s_tilex = obj->tilex;
#endif

                if (projwall & 0x80)
                    TryBlastDoor (projwall & ~0x80);

                if (ISPOINTER(projcheck))
                {
                    if (projcheck->flags2 & FL2_BFG_SHOOTABLE)
                        DamageActor (projcheck,BFG_DAMAGE >> 1,obj);
#ifdef BFG_SHOT_STOPS
                    //
                    // stop on actors that aren't killed
                    //
                    if (!(projcheck->flags2 & FL2_BFGSHOT_SOLID))
                    {
                        if (projcheck->flags & FL_DEADGUY)
                            return;
                    }
#endif
                }

                //
                // mark as exploded
                //
                obj->obclass = bfg_explosionobj;
                InitAnim (obj,SPR_BFG_EXP1,0,7,at_ONCE,ad_FWD,(US_RndT() & 7),5);
                A_DeathScream (obj);
                return;
        }
    }

    //
    // determine if projectile hit player
    //
    if (obj->obclass != grenadeobj && obj->obclass != bfg_shotobj)
    {
        deltax = labs(obj->x - player->x);
        deltay = labs(obj->y - player->y);

        if (deltax < PROJECTILESIZE && deltay < PROJECTILESIZE)
        {
            obj->x -= FixedMul(PROJECTILESIZE,costable[obj->angle]);
            obj->y -= -FixedMul(PROJECTILESIZE,sintable[obj->angle]);

            //
            // KS: nasty!
            //
            if (obj->flags & FL_STORED_OBJPTR)
                attacker = (objtype *)obj->temp3;
            else
                attacker = obj;

            switch (obj->obclass)
            {
                case mut_hum1shotobj:
                case electroshotobj:
                    damage = US_RndT() >> 5;
                    InitSmartSpeedAnim (obj,SPR_ELEC_SHOT_EXP1,0,1,at_ONCE,ad_FWD,3 + (US_RndT() & 7));
                    break;

                case final_boss4shotobj:
                    damage = US_RndT() >> 4;
                    InitSmartSpeedAnim (obj,SPR_BOSS10_SPIT_EXP1,0,1,at_ONCE,ad_FWD,3 + (US_RndT() & 3));
                    break;

                case goldmorphshotobj:
                case final_boss2shotobj:
                    damage = US_RndT() >> 4;
                    InitSmartSpeedAnim (obj,SPR_MGOLD_SHOT_EXP1,0,1,at_ONCE,ad_FWD,5 + (US_RndT() & 7));
                    break;

                case lcanshotobj:
                case podshotobj:
                    damage = US_RndT() >> 4;
                    InitSmartSpeedAnim (obj,SPR_SPIT_EXP3_1,0,2,at_ONCE,ad_FWD,5 + (US_RndT() & 7));
                    break;

                case scanshotobj:
                case dogshotobj:
                    damage = US_RndT() >> 4;
                    InitSmartSpeedAnim (obj,SPR_SPIT_EXP1_1,0,2,at_ONCE,ad_FWD,5 + (US_RndT() & 7));
                    break;

                case liquidshotobj:
                    damage = (US_RndT() >> 4);
                    InitSmartSpeedAnim (obj,SPR_LIQUID_SHOT_BURST_1,0,2,at_ONCE,ad_FWD,5 + (US_RndT() & 7));
                    break;

                case spider_mutantshotobj:
                    damage = US_RndT() >> 4;
                    InitSmartSpeedAnim (obj,SPR_BOSS1_EXP1,0,2,at_ONCE,ad_FWD,5 + (US_RndT() & 7));
                    break;

                case acid_dragonshotobj:
                    damage = US_RndT() >> 4;
                    InitSmartSpeedAnim (obj,SPR_BOSS5_EXP1,0,2,at_ONCE,ad_FWD,5 + (US_RndT() & 7));
                    break;
            }

            TakeDamage (damage,attacker);
            return;
        }
    }

    //
    // align tile coordinates on boundaries
    //
    obj->tilex = obj->x >> TILESHIFT;
    obj->tiley = obj->y >> TILESHIFT;
}


/*
===============
=
= ExplodeRadius
=
===============
*/

void ExplodeRadius (objtype *obj, int damage, bool damageplayer)
{
    expfill_t ff;

    if (tilemap[obj->tilex][obj->tiley])
        return;

//
// check to see if player is a baby and should not be hurt by explosions,
// except from actors that use explosions for attacks (eg. PerScan Drones).
//
    if (gamestate.difficulty > gd_baby || obj->obclass == floatingbombobj)
        ff.damageplayer = damageplayer;
    else
        ff.damageplayer = false;

//
// start flood-fill explosion!
//
    ff.obj = obj;
    ff.damage = damage;
    memset (ff.buffer,0,sizeof(ff.buffer));

    ExplodeFill (&ff,obj->tilex,obj->tiley);
    ExplodeStatics (obj->tilex,obj->tiley);
}


/*
===============
=
= ExplodeFill
=
===============
*/

void ExplodeFill (expfill_t *ff, int tilex, int tiley)
{
    int      bx,by;
    unsigned door,nowall;

    bx = (tilex - ff->obj->tilex) + EXPLODE_RADIUS;
    by = (tiley - ff->obj->tiley) + EXPLODE_RADIUS;

//
// damage actors on this spot!
//
    if (ff->damageplayer && tilex == player->tilex && tiley == player->tiley)
        TakeDamage (EXPLODE_DAMAGE,ff->obj);
    else
    {
        projcheck = actorat[tilex & (mapwidth - 1)][tiley & (mapheight - 1)];

        if (ISPOINTER(projcheck))
        {
            if ((projcheck->flags & FL_SHOOTABLE))
            {
                switch (projcheck->obclass)
                {
                    //
                    // detonate all floating bombs & VMTs
                    //
                    case floatingbombobj:
                    case volatiletransportobj:
                        DamageActor (projcheck,500,ff->obj);
                        break;

                    //
                    // hanging turrets & barriers are not effected by
                    // concussion weapons
                    //
                    case hang_terrotobj:
                    case arc_barrierobj:
                    case post_barrierobj:
                    case vpost_barrierobj:
                    case vspike_barrierobj:
                        break;

                    //
                    // test for Level completion object
                    //
                    case rotating_cubeobj:
                        if (ff->obj->obclass == pd_explosionobj)
                        {
                            projcheck->lighting = EXPLOSION_SHADING;
                            projcheck->flags &= ~(FL_SOLID | FL_SHOOTABLE);
                            InitSmartSpeedAnim (projcheck,SPR_CUBE_EXP1,0,8,at_ONCE,ad_FWD,5);
                            PlaySoundLocActor (EXPLODE1SND,projcheck);

                            //
                            // unlock next floor
                            //
                            gamestate.key_floor = gamestate.mapon + 1;
                            gamestuff.level[gamestate.key_floor].locked = false;
                        }
                        break;

                    //
                    // Plasma/Fission Detonators (already armed)
                    //
                    case plasma_detonatorobj:
                        //
                        // player shot it with gun or direct Hit with grenade?
                        //
                        if (ff->obj == player || (ff->obj->tilex == tilex && ff->obj->tiley == tiley))
                            DamageActor (projcheck,1,ff->obj);
                        else
                            DamageActor (projcheck,20,ff->obj);    // an explosion has started a chain reaction
                        break;

                    default:
                        if (!(projcheck->flags2 & FL2_CLOAKED))
                            SpawnFlash (projcheck->x,projcheck->y);

                        DamageActor (projcheck,ff->damage,ff->obj);
                        break;
                }
            }
        }
    }

    ff->buffer[bx][by] = true;    // mark spot as exploded

//
// explode to the EAST
//
    bx++;
    tilex++;

    door = tilemap[tilex][tiley];

    if (door & 0x80)
        nowall = doorobjlist[door & ~0x80].action != dr_closed;
    else
        nowall = !tilemap[tilex][tiley];

    if (!ff->buffer[bx][by] && nowall && bx <= EXPLODE_RADIUS * 2)
        ExplodeFill (ff,tilex,tiley);

//
// explode to the WEST
//
    bx -= 2;
    tilex -= 2;

    door = tilemap[tilex][tiley];

    if (door & 0x80)
        nowall = doorobjlist[door & ~0x80].action != dr_closed;
    else
        nowall = !tilemap[tilex][tiley];

    if (!ff->buffer[bx][by] && nowall && bx >= 0)
        ExplodeFill (ff,tilex,tiley);

//
// explode to the SOUTH
//
    bx++;
    by++;
    tilex++;
    tiley++;

    door = tilemap[tilex][tiley];

    if (door & 0x80)
        nowall = doorobjlist[door & ~0x80].action != dr_closed;
    else
        nowall = !tilemap[tilex][tiley];

    if (!ff->buffer[bx][by] && nowall && by <= EXPLODE_RADIUS * 2)
        ExplodeFill (ff,tilex,tiley);

//
// explode to the NORTH
//
    by -= 2;
    tiley -= 2;

    door = tilemap[tilex][tiley];

    if (door & 0x80)
        nowall = doorobjlist[door & ~0x80].action != dr_closed;
    else
        nowall = !tilemap[tilex][tiley];

    if (!ff->buffer[bx][by] && nowall && by >= 0)
        ExplodeFill (ff,tilex,tiley);
}


/*
===============
=
= T_BlowBack
=
===============
*/

void T_BlowBack (objtype *obj)
{
    #define SLIDE_SPEED   0x2000

    unsigned dist_table[] =
    {
        0x1000,  // wp_autocharge
        0x2000,  // wp_pistol
        0x3000,  // wp_burst_rifle
        0x4000,  // wp_ion_cannon
        0x5000,  // wp_grenade
    };

    fixed    deltax,deltay;
    int      dist;
    objtype  *killer;

    if (obj->flags & FL_NO_SLIDE)
        return;

    if (!(obj->flags & FL_SLIDE_INIT))
    {
        //
        // KS: ugly
        //
        killer = (objtype *)SLIDE_TEMP(obj);

        if (!killer)
        {
            obj->flags |= FL_NO_SLIDE;
            return;
        }

        obj->angle = CalcAngle(killer,obj);

        killer = (objtype *)SLIDE_TEMP(obj);

        if (killer == player)
            SLIDE_TEMP(obj) = dist_table[gamestate.weapon];
        else
            SLIDE_TEMP(obj) = dist_table[wp_grenade];

        obj->flags |= FL_SLIDE_INIT;
    }

    if (SLIDE_TEMP(obj) > SLIDE_SPEED)
    {
        dist = SLIDE_SPEED;
        SLIDE_TEMP(obj) -= SLIDE_SPEED;
    }
    else
    {
        dist = SLIDE_TEMP(obj);
        obj->flags |= FL_NO_SLIDE;      // stop any more sliding
    }

    deltax = FixedMul(dist,costable[obj->angle]);
    deltay = -FixedMul(dist,sintable[obj->angle]);

    if (ClipMove(obj,deltax,deltay))
        obj->flags |= FL_NO_SLIDE;

    obj->tilex = obj->x >> TILESHIFT;
    obj->tiley = obj->y >> TILESHIFT;

    if (obj->flags & FL_NO_SLIDE)
    {
        //
        // set actor where it slid in actorat if there's a door
        //
        if (tilemap[obj->tilex][obj->tiley] & 0x80)
        {
            actorat[obj->tilex][obj->tiley] = obj;
            obj->flags &= ~FL_NEVERMARK;
        }
    }
}
