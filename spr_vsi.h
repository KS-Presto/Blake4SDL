//
// Sprite definitions for Planet Strike
//

#ifndef _SPR_VSI_H_
#define _SPR_VSI_H_


enum sprites
{
    SPR_DEMO,

    //
    // static sprites
    //
    SPR_STAT_0,
#if GAME_VERSION != SHAREWARE_VERSION
    SPR_STAT_1,
#endif
    SPR_STAT_2,
#if GAME_VERSION != SHAREWARE_VERSION
    SPR_STAT_3,SPR_STAT_4,
    SPR_STAT_5,SPR_STAT_6,SPR_STAT_7,
#endif

    SPR_STAT_8,SPR_STAT_9,SPR_STAT_10,SPR_STAT_11,
    SPR_STAT_12,SPR_STAT_13,SPR_STAT_14,SPR_STAT_15,

#if GAME_VERSION != SHAREWARE_VERSION
    SPR_STAT_16,SPR_STAT_17,
#endif
    SPR_STAT_18,
#if GAME_VERSION != SHAREWARE_VERSION
    SPR_STAT_19,
    SPR_STAT_20,SPR_STAT_21,SPR_STAT_22,SPR_STAT_23,
#endif

    SPR_STAT_24,
#if GAME_VERSION != SHAREWARE_VERSION
    SPR_STAT_25,
#endif
    SPR_STAT_26,SPR_STAT_27,
    SPR_STAT_28,SPR_STAT_29,
#if GAME_VERSION != SHAREWARE_VERSION
    SPR_STAT_30,
#endif
    SPR_STAT_31,

    SPR_STAT_32,SPR_STAT_33,SPR_STAT_34,SPR_STAT_35,
    SPR_STAT_36,
#if GAME_VERSION != SHAREWARE_VERSION
    SPR_STAT_37,
#endif
    SPR_STAT_38,
#if GAME_VERSION != SHAREWARE_VERSION
    SPR_STAT_39,
#endif

    SPR_STAT_40,SPR_STAT_41,SPR_STAT_42,SPR_STAT_43,
    SPR_STAT_44,SPR_STAT_45,SPR_STAT_46,SPR_STAT_47,

    SPR_STAT_48,SPR_STAT_49,
    SPR_STAT_50,SPR_STAT_51,

    SPR_STAT_52,SPR_STAT_53,SPR_STAT_54,SPR_STAT_55,
    SPR_STAT_56,SPR_CRATE_1,
#if GAME_VERSION != SHAREWARE_VERSION
    SPR_CRATE_2,SPR_CRATE_3,
#endif

    SPR_STAT_57,
#if GAME_VERSION != SHAREWARE_VERSION
    SPR_STAT_58,SPR_STAT_59,
#endif
    SPR_STAT_60,
    SPR_STAT_61,SPR_STAT_62,SPR_STAT_63,
#if GAME_VERSION != SHAREWARE_VERSION
    SPR_STAT_64,
#endif

    SPR_STAT_65,SPR_STAT_66,SPR_STAT_67,SPR_STAT_68,
    SPR_STAT_69,
#if GAME_VERSION != SHAREWARE_VERSION
    SPR_STAT_70,SPR_STAT_71,SPR_STAT_72,
#endif

#if GAME_VERSION != SHAREWARE_VERSION
    SPR_STAT_73,SPR_STAT_74,SPR_STAT_75,SPR_STAT_76,
#endif
    SPR_STAT_77,SPR_STAT_78,SPR_STAT_79,

    SPR_DOORBOMB,SPR_ALT_DOORBOMB,SPR_RUBBLE,SPR_BONZI_TREE,
    SPR_AUTOMAPPER,SPR_POT_PLANT,SPR_TUBE_PLANT,SPR_HITECH_CHAIR,

    //
    // aesthetics
    //
    SPR_AIR_VENT,
    SPR_BLOOD_DRIP1,SPR_BLOOD_DRIP2,SPR_BLOOD_DRIP3,SPR_BLOOD_DRIP4,
    SPR_WATER_DRIP1,SPR_WATER_DRIP2,SPR_WATER_DRIP3,SPR_WATER_DRIP4,

    SPR_DECO_ARC_1,SPR_DECO_ARC_2,SPR_DECO_ARC_3,

    SPR_GRATE,
    SPR_STEAM_1,SPR_STEAM_2,SPR_STEAM_3,SPR_STEAM_4,

    SPR_STEAM_PIPE,
    SPR_PIPE_STEAM_1,SPR_PIPE_STEAM_2,SPR_PIPE_STEAM_3,SPR_PIPE_STEAM_4,

    //
    // Dead Actors (from Blake Stone: AOG)
    //
    SPR_DEAD_RENT,SPR_DEAD_PRO,SPR_DEAD_SWAT,

    //
    // Rent-A-Cop
    //
    SPR_RENT_S_1,SPR_RENT_S_2,SPR_RENT_S_3,SPR_RENT_S_4,
    SPR_RENT_S_5,SPR_RENT_S_6,SPR_RENT_S_7,SPR_RENT_S_8,

    SPR_RENT_W1_1,SPR_RENT_W1_2,SPR_RENT_W1_3,SPR_RENT_W1_4,
    SPR_RENT_W1_5,SPR_RENT_W1_6,SPR_RENT_W1_7,SPR_RENT_W1_8,

    SPR_RENT_W2_1,SPR_RENT_W2_2,SPR_RENT_W2_3,SPR_RENT_W2_4,
    SPR_RENT_W2_5,SPR_RENT_W2_6,SPR_RENT_W2_7,SPR_RENT_W2_8,

    SPR_RENT_W3_1,SPR_RENT_W3_2,SPR_RENT_W3_3,SPR_RENT_W3_4,
    SPR_RENT_W3_5,SPR_RENT_W3_6,SPR_RENT_W3_7,SPR_RENT_W3_8,

    SPR_RENT_W4_1,SPR_RENT_W4_2,SPR_RENT_W4_3,SPR_RENT_W4_4,
    SPR_RENT_W4_5,SPR_RENT_W4_6,SPR_RENT_W4_7,SPR_RENT_W4_8,

    SPR_RENT_DIE_1,SPR_RENT_DIE_2,SPR_RENT_DIE_3,SPR_RENT_DIE_4,
    SPR_RENT_PAIN_1,SPR_RENT_DEAD,

    SPR_RENT_SHOOT1,SPR_RENT_SHOOT2,SPR_RENT_SHOOT3,

    //
    // PRO-GUARD
    //
    SPR_PRO_S_1,SPR_PRO_S_2,SPR_PRO_S_3,SPR_PRO_S_4,
    SPR_PRO_S_5,SPR_PRO_S_6,SPR_PRO_S_7,SPR_PRO_S_8,

    SPR_PRO_W1_1,SPR_PRO_W1_2,SPR_PRO_W1_3,SPR_PRO_W1_4,
    SPR_PRO_W1_5,SPR_PRO_W1_6,SPR_PRO_W1_7,SPR_PRO_W1_8,

    SPR_PRO_W2_1,SPR_PRO_W2_2,SPR_PRO_W2_3,SPR_PRO_W2_4,
    SPR_PRO_W2_5,SPR_PRO_W2_6,SPR_PRO_W2_7,SPR_PRO_W2_8,

    SPR_PRO_W3_1,SPR_PRO_W3_2,SPR_PRO_W3_3,SPR_PRO_W3_4,
    SPR_PRO_W3_5,SPR_PRO_W3_6,SPR_PRO_W3_7,SPR_PRO_W3_8,

    SPR_PRO_W4_1,SPR_PRO_W4_2,SPR_PRO_W4_3,SPR_PRO_W4_4,
    SPR_PRO_W4_5,SPR_PRO_W4_6,SPR_PRO_W4_7,SPR_PRO_W4_8,

    SPR_PRO_PAIN_1,SPR_PRO_DIE_1,SPR_PRO_DIE_2,SPR_PRO_DIE_3,
    SPR_PRO_PAIN_2,SPR_PRO_DIE_4,SPR_PRO_DEAD,

    SPR_PRO_SHOOT1,SPR_PRO_SHOOT2,SPR_PRO_SHOOT3,

    //
    // swat
    //
    SPR_SWAT_S_1,SPR_SWAT_S_2,SPR_SWAT_S_3,SPR_SWAT_S_4,
    SPR_SWAT_S_5,SPR_SWAT_S_6,SPR_SWAT_S_7,SPR_SWAT_S_8,

    SPR_SWAT_W1_1,SPR_SWAT_W1_2,SPR_SWAT_W1_3,SPR_SWAT_W1_4,
    SPR_SWAT_W1_5,SPR_SWAT_W1_6,SPR_SWAT_W1_7,SPR_SWAT_W1_8,

    SPR_SWAT_W2_1,SPR_SWAT_W2_2,SPR_SWAT_W2_3,SPR_SWAT_W2_4,
    SPR_SWAT_W2_5,SPR_SWAT_W2_6,SPR_SWAT_W2_7,SPR_SWAT_W2_8,

    SPR_SWAT_W3_1,SPR_SWAT_W3_2,SPR_SWAT_W3_3,SPR_SWAT_W3_4,
    SPR_SWAT_W3_5,SPR_SWAT_W3_6,SPR_SWAT_W3_7,SPR_SWAT_W3_8,

    SPR_SWAT_W4_1,SPR_SWAT_W4_2,SPR_SWAT_W4_3,SPR_SWAT_W4_4,
    SPR_SWAT_W4_5,SPR_SWAT_W4_6,SPR_SWAT_W4_7,SPR_SWAT_W4_8,

    SPR_SWAT_PAIN_1,SPR_SWAT_DIE_1,SPR_SWAT_DIE_2,SPR_SWAT_DIE_3,
    SPR_SWAT_PAIN_2,SPR_SWAT_DIE_4,SPR_SWAT_DEAD,

    SPR_SWAT_SHOOT1,SPR_SWAT_SHOOT2,SPR_SWAT_SHOOT3,

    SPR_SWAT_WOUNDED1,SPR_SWAT_WOUNDED2,SPR_SWAT_WOUNDED3,SPR_SWAT_WOUNDED4,

    //
    // GENERAL SCIENTIST (Bio-tech)
    //
    SPR_BIO_S_1,SPR_BIO_S_2,SPR_BIO_S_3,SPR_BIO_S_4,
    SPR_BIO_S_5,SPR_BIO_S_6,SPR_BIO_S_7,SPR_BIO_S_8,

    SPR_BIO_W1_1,SPR_BIO_W1_2,SPR_BIO_W1_3,SPR_BIO_W1_4,
    SPR_BIO_W1_5,SPR_BIO_W1_6,SPR_BIO_W1_7,SPR_BIO_W1_8,

    SPR_BIO_W2_1,SPR_BIO_W2_2,SPR_BIO_W2_3,SPR_BIO_W2_4,
    SPR_BIO_W2_5,SPR_BIO_W2_6,SPR_BIO_W2_7,SPR_BIO_W2_8,

    SPR_BIO_W3_1,SPR_BIO_W3_2,SPR_BIO_W3_3,SPR_BIO_W3_4,
    SPR_BIO_W3_5,SPR_BIO_W3_6,SPR_BIO_W3_7,SPR_BIO_W3_8,

    SPR_BIO_W4_1,SPR_BIO_W4_2,SPR_BIO_W4_3,SPR_BIO_W4_4,
    SPR_BIO_W4_5,SPR_BIO_W4_6,SPR_BIO_W4_7,SPR_BIO_W4_8,

    SPR_BIO_PAIN_1,SPR_BIO_DIE_1,SPR_BIO_DIE_2,SPR_BIO_DIE_3,
    SPR_BIO_PAIN_2,SPR_BIO_DIE_4,SPR_BIO_DEAD,

    SPR_BIO_SHOOT1,SPR_BIO_SHOOT2,SPR_BIO_SHOOT3,

    //
    // Bad Boy Dr. Goldstern
    //
    SPR_GOLD_S_1,SPR_GOLD_S_2,SPR_GOLD_S_3,SPR_GOLD_S_4,
    SPR_GOLD_S_5,SPR_GOLD_S_6,SPR_GOLD_S_7,SPR_GOLD_S_8,

    SPR_GOLD_W1_1,SPR_GOLD_W1_2,SPR_GOLD_W1_3,SPR_GOLD_W1_4,
    SPR_GOLD_W1_5,SPR_GOLD_W1_6,SPR_GOLD_W1_7,SPR_GOLD_W1_8,

    SPR_GOLD_W2_1,SPR_GOLD_W2_2,SPR_GOLD_W2_3,SPR_GOLD_W2_4,
    SPR_GOLD_W2_5,SPR_GOLD_W2_6,SPR_GOLD_W2_7,SPR_GOLD_W2_8,

    SPR_GOLD_W3_1,SPR_GOLD_W3_2,SPR_GOLD_W3_3,SPR_GOLD_W3_4,
    SPR_GOLD_W3_5,SPR_GOLD_W3_6,SPR_GOLD_W3_7,SPR_GOLD_W3_8,

    SPR_GOLD_W4_1,SPR_GOLD_W4_2,SPR_GOLD_W4_3,SPR_GOLD_W4_4,
    SPR_GOLD_W4_5,SPR_GOLD_W4_6,SPR_GOLD_W4_7,SPR_GOLD_W4_8,

    SPR_GOLD_PAIN_1,

    SPR_GOLD_WRIST_1,SPR_GOLD_WRIST_2,

    SPR_GOLD_SHOOT1,SPR_GOLD_SHOOT2,SPR_GOLD_SHOOT3,

    SPR_GOLD_WARP1,SPR_GOLD_WARP2,SPR_GOLD_WARP3,SPR_GOLD_WARP4,
    SPR_GOLD_WARP5,

    SPR_GOLD_DEATH1,SPR_GOLD_DEATH2,SPR_GOLD_DEATH3,SPR_GOLD_DEATH4,
    SPR_GOLD_DEATH5,SPR_MGOLD_OUCH,

    SPR_GOLD_MORPH1,SPR_GOLD_MORPH2,SPR_GOLD_MORPH3,SPR_GOLD_MORPH4,
    SPR_GOLD_MORPH5,SPR_GOLD_MORPH6,SPR_GOLD_MORPH7,SPR_GOLD_MORPH8,

    SPR_MGOLD_WALK1,SPR_MGOLD_WALK2,SPR_MGOLD_WALK3,SPR_MGOLD_WALK4,
    SPR_MGOLD_ATTACK1,SPR_MGOLD_ATTACK2,SPR_MGOLD_ATTACK3,SPR_MGOLD_ATTACK4,

    SPR_MGOLD_SHOT1,SPR_MGOLD_SHOT2,SPR_MGOLD_SHOT3,
    SPR_MGOLD_SHOT_EXP1,SPR_MGOLD_SHOT_EXP2,SPR_MGOLD_SHOT_EXP3,

    //
    // GROUND SCOUT
    //
#if GAME_VERSION != SHAREWARE_VERSION
    SPR_GSCOUT_W1_1,SPR_GSCOUT_W1_2,SPR_GSCOUT_W1_3,SPR_GSCOUT_W1_4,
    SPR_GSCOUT_W1_5,SPR_GSCOUT_W1_6,SPR_GSCOUT_W1_7,SPR_GSCOUT_W1_8,

    SPR_GSCOUT_W2_1,SPR_GSCOUT_W2_2,SPR_GSCOUT_W2_3,SPR_GSCOUT_W2_4,
    SPR_GSCOUT_W2_5,SPR_GSCOUT_W2_6,SPR_GSCOUT_W2_7,SPR_GSCOUT_W2_8,

    SPR_GSCOUT_W3_1,SPR_GSCOUT_W3_2,SPR_GSCOUT_W3_3,SPR_GSCOUT_W3_4,
    SPR_GSCOUT_W3_5,SPR_GSCOUT_W3_6,SPR_GSCOUT_W3_7,SPR_GSCOUT_W3_8,

    SPR_GSCOUT_W4_1,SPR_GSCOUT_W4_2,SPR_GSCOUT_W4_3,SPR_GSCOUT_W4_4,
    SPR_GSCOUT_W4_5,SPR_GSCOUT_W4_6,SPR_GSCOUT_W4_7,SPR_GSCOUT_W4_8,

    SPR_GSCOUT_DIE1,SPR_GSCOUT_DIE2,SPR_GSCOUT_DIE3,SPR_GSCOUT_DIE4,
    SPR_GSCOUT_DIE5,SPR_GSCOUT_DIE6,SPR_GSCOUT_DIE7,SPR_GSCOUT_DIE8,

    SPR_GSCOUT_DEAD,
#endif

    //
    // FLOATING SCOUT
    //
    SPR_FSCOUT_W1_1,SPR_FSCOUT_W1_2,SPR_FSCOUT_W1_3,SPR_FSCOUT_W1_4,
    SPR_FSCOUT_W1_5,SPR_FSCOUT_W1_6,SPR_FSCOUT_W1_7,SPR_FSCOUT_W1_8,

#if GAME_VERSION != SHAREWARE_VERSION
    SPR_FSCOUT_W2_1,SPR_FSCOUT_W2_2,SPR_FSCOUT_W2_3,SPR_FSCOUT_W2_4,
    SPR_FSCOUT_W2_5,SPR_FSCOUT_W2_6,SPR_FSCOUT_W2_7,SPR_FSCOUT_W2_8,

    SPR_FSCOUT_W3_1,SPR_FSCOUT_W3_2,SPR_FSCOUT_W3_3,SPR_FSCOUT_W3_4,
    SPR_FSCOUT_W3_5,SPR_FSCOUT_W3_6,SPR_FSCOUT_W3_7,SPR_FSCOUT_W3_8,

    SPR_FSCOUT_W4_1,SPR_FSCOUT_W4_2,SPR_FSCOUT_W4_3,SPR_FSCOUT_W4_4,
    SPR_FSCOUT_W4_5,SPR_FSCOUT_W4_6,SPR_FSCOUT_W4_7,SPR_FSCOUT_W4_8,
#endif

    SPR_FSCOUT_DIE1,SPR_FSCOUT_DIE2,SPR_FSCOUT_DIE3,SPR_FSCOUT_DIE4,
    SPR_FSCOUT_DIE5,SPR_FSCOUT_DIE6,SPR_FSCOUT_DIE7,SPR_FSCOUT_DEAD,

    //
    // GENERAL EXPLOSION ANIM
    //
    SPR_EXPLOSION_1,SPR_EXPLOSION_2,SPR_EXPLOSION_3,SPR_EXPLOSION_4,
    SPR_EXPLOSION_5,

    //
    // VITAL DEFENCE OBJECT
    //
//#if GAME_VERSION != SHAREWARE_VERSION
    //SPR_VITAL_STAND,
    //SPR_VITAL_DIE_1,SPR_VITAL_DIE_2,SPR_VITAL_DIE_3,SPR_VITAL_DIE_4,
    //SPR_VITAL_DIE_5,SPR_VITAL_DIE_6,SPR_VITAL_DIE_7,SPR_VITAL_DIE_8,
    //SPR_VITAL_DEAD_1,SPR_VITAL_DEAD_2,SPR_VITAL_DEAD_3,
    //SPR_VITAL_OUCH,
//#endif

    //
    // ROTATING CUBE
    //
    SPR_CUBE1,SPR_CUBE2,SPR_CUBE3,SPR_CUBE4,
    SPR_CUBE5,SPR_CUBE6,SPR_CUBE7,SPR_CUBE8,
    SPR_CUBE9,SPR_CUBE10,
    SPR_CUBE_EXP1,SPR_CUBE_EXP2,SPR_CUBE_EXP3,SPR_CUBE_EXP4,
    SPR_CUBE_EXP5,SPR_CUBE_EXP6,SPR_CUBE_EXP7,SPR_CUBE_EXP8,
    SPR_DEAD_CUBE,

    //
    // RED SECURITY LIGHTS
    //
    SPR_SECURITY_NORMAL,SPR_SECURITY_ALERT,

    //
    // P.O.D. Alien
    //
    SPR_POD_EGG,SPR_POD_HATCH1,SPR_POD_HATCH2,SPR_POD_HATCH3,
    SPR_POD_WALK1,SPR_POD_WALK2,SPR_POD_WALK3,SPR_POD_WALK4,
    SPR_POD_ATTACK1,SPR_POD_ATTACK2,SPR_POD_ATTACK3,
    SPR_POD_OUCH,
    SPR_POD_DIE1,SPR_POD_DIE2,SPR_POD_DIE3,
    SPR_POD_SPIT1,SPR_POD_SPIT2,SPR_POD_SPIT3,

    //
    // Electro-Alien
    //
    SPR_ELEC_APPEAR1,SPR_ELEC_APPEAR2,SPR_ELEC_APPEAR3,
    SPR_ELEC_WALK1,SPR_ELEC_WALK2,SPR_ELEC_WALK3,SPR_ELEC_WALK4,
    SPR_ELEC_OUCH,
    SPR_ELEC_SHOOT1,SPR_ELEC_SHOOT2,SPR_ELEC_SHOOT3,
    SPR_ELEC_DIE1,SPR_ELEC_DIE2,SPR_ELEC_DIE3,
    SPR_ELEC_SHOT1,SPR_ELEC_SHOT2,
    SPR_ELEC_SHOT_EXP1,SPR_ELEC_SHOT_EXP2,

    //
    // ElectroSphere
    //
    SPR_ELECTRO_SPHERE_ROAM1,SPR_ELECTRO_SPHERE_ROAM2,SPR_ELECTRO_SPHERE_ROAM3,
    SPR_ELECTRO_SPHERE_OUCH,
    SPR_ELECTRO_SPHERE_DIE1,SPR_ELECTRO_SPHERE_DIE2,SPR_ELECTRO_SPHERE_DIE3,
    SPR_ELECTRO_SPHERE_DIE4,

    //
    // Genetic Guard
    //
    SPR_GENETIC_W1,SPR_GENETIC_W2,SPR_GENETIC_W3,SPR_GENETIC_W4,
    SPR_GENETIC_SWING1,SPR_GENETIC_SWING2,SPR_GENETIC_SWING3,
    SPR_GENETIC_DEAD,
    SPR_GENETIC_DIE1,SPR_GENETIC_DIE2,SPR_GENETIC_DIE3,SPR_GENETIC_DIE4,
    SPR_GENETIC_OUCH,
    SPR_GENETIC_SHOOT1,SPR_GENETIC_SHOOT2,SPR_GENETIC_SHOOT3,

    //
    // Muntant human type 1
    //
    SPR_MUTHUM1_W1,SPR_MUTHUM1_W2,SPR_MUTHUM1_W3,SPR_MUTHUM1_W4,
    SPR_MUTHUM1_SWING1,SPR_MUTHUM1_SWING2,SPR_MUTHUM1_SWING3,
    SPR_MUTHUM1_DEAD,
    SPR_MUTHUM1_DIE1,SPR_MUTHUM1_DIE2,SPR_MUTHUM1_DIE3,SPR_MUTHUM1_DIE4,
    SPR_MUTHUM1_OUCH,
    SPR_MUTHUM1_SPIT1,SPR_MUTHUM1_SPIT2,SPR_MUTHUM1_SPIT3,

    //
    // Muntant human type 2
    //
#if GAME_VERSION != SHAREWARE_VERSION
    SPR_MUTHUM2_W1,SPR_MUTHUM2_W2,SPR_MUTHUM2_W3,SPR_MUTHUM2_W4,
    SPR_MUTHUM2_SWING1,SPR_MUTHUM2_SWING2,SPR_MUTHUM2_SWING3,
    SPR_MUTHUM2_DEAD,
    SPR_MUTHUM2_DIE1,SPR_MUTHUM2_DIE2,SPR_MUTHUM2_DIE3,SPR_MUTHUM2_DIE4,
    SPR_MUTHUM2_OUCH,
    SPR_MUTHUM2_SPIT1,SPR_MUTHUM2_SPIT2,SPR_MUTHUM2_SPIT3,

    SPR_MUTHUM2_MORPH1,SPR_MUTHUM2_MORPH2,SPR_MUTHUM2_MORPH3,SPR_MUTHUM2_MORPH4,
    SPR_MUTHUM2_MORPH5,SPR_MUTHUM2_MORPH6,SPR_MUTHUM2_MORPH7,SPR_MUTHUM2_MORPH8,
    SPR_MUTHUM2_MORPH9,
#endif

    //
    // Large Cantained Alien
    //
#if GAME_VERSION != SHAREWARE_VERSION
    SPR_LCAN_ALIEN_READY,SPR_LCAN_ALIEN_B1,SPR_LCAN_ALIEN_B2,
    SPR_LCAN_ALIEN_B3,SPR_LCAN_ALIEN_EMPTY,

    SPR_LCAN_ALIEN_W1,SPR_LCAN_ALIEN_W2,SPR_LCAN_ALIEN_W3,SPR_LCAN_ALIEN_W4,
    SPR_LCAN_ALIEN_SWING1,SPR_LCAN_ALIEN_SWING2,SPR_LCAN_ALIEN_SWING3,
    SPR_LCAN_ALIEN_DEAD,
    SPR_LCAN_ALIEN_DIE1,SPR_LCAN_ALIEN_DIE2,SPR_LCAN_ALIEN_DIE3,
    SPR_LCAN_ALIEN_DIE4,SPR_LCAN_ALIEN_OUCH,
    SPR_LCAN_ALIEN_SPIT1,SPR_LCAN_ALIEN_SPIT2,SPR_LCAN_ALIEN_SPIT3,
#endif

    //
    // Small Canister Alien
    //
    SPR_SCAN_ALIEN_READY,SPR_SCAN_ALIEN_B1,SPR_SCAN_ALIEN_B2,
    SPR_SCAN_ALIEN_B3,SPR_SCAN_ALIEN_EMPTY,

    SPR_SCAN_ALIEN_W1,SPR_SCAN_ALIEN_W2,SPR_SCAN_ALIEN_W3,SPR_SCAN_ALIEN_W4,
    SPR_SCAN_ALIEN_SWING1,SPR_SCAN_ALIEN_SWING2,SPR_SCAN_ALIEN_SWING3,
    SPR_SCAN_ALIEN_DEAD,
    SPR_SCAN_ALIEN_DIE1,SPR_SCAN_ALIEN_DIE2,SPR_SCAN_ALIEN_DIE3,
    SPR_SCAN_ALIEN_DIE4,SPR_SCAN_ALIEN_OUCH,

    SPR_SCAN_ALIEN_SPIT1,SPR_SCAN_ALIEN_SPIT2,SPR_SCAN_ALIEN_SPIT3,

    //
    // Gurney Mutant
    //
#if GAME_VERSION != SHAREWARE_VERSION
    SPR_GURNEY_MUT_READY,SPR_GURNEY_MUT_B1,SPR_GURNEY_MUT_B2,
    SPR_GURNEY_MUT_B3,SPR_GURNEY_MUT_EMPTY,

    SPR_GURNEY_MUT_W1,SPR_GURNEY_MUT_W2,SPR_GURNEY_MUT_W3,SPR_GURNEY_MUT_W4,
    SPR_GURNEY_MUT_SWING1,SPR_GURNEY_MUT_SWING2,SPR_GURNEY_MUT_SWING3,
    SPR_GURNEY_MUT_DEAD,
    SPR_GURNEY_MUT_DIE1,SPR_GURNEY_MUT_DIE2,SPR_GURNEY_MUT_DIE3,
    SPR_GURNEY_MUT_DIE4,SPR_GURNEY_MUT_OUCH,
#endif

    //
    // Liquid Alien
    //
#if GAME_VERSION != SHAREWARE_VERSION
    SPR_LIQUID_M1,SPR_LIQUID_M2,SPR_LIQUID_M3,
    SPR_LIQUID_R1,SPR_LIQUID_R2,SPR_LIQUID_R3,SPR_LIQUID_R4,
    SPR_LIQUID_S1,SPR_LIQUID_S2,SPR_LIQUID_S3,
    SPR_LIQUID_OUCH,
    SPR_LIQUID_DIE_1,SPR_LIQUID_DIE_2,SPR_LIQUID_DIE_3,SPR_LIQUID_DIE_4,
    SPR_LIQUID_DEAD,
    SPR_LIQUID_SHOT_FLY_1,SPR_LIQUID_SHOT_FLY_2,SPR_LIQUID_SHOT_FLY_3,
    SPR_LIQUID_SHOT_BURST_1,SPR_LIQUID_SHOT_BURST_2,SPR_LIQUID_SHOT_BURST_3,
#endif

    //
    //  SPIT SHOTS
    //
    SPR_SPIT1_1,SPR_SPIT1_2,SPR_SPIT1_3,
    SPR_SPIT_EXP1_1,SPR_SPIT_EXP1_2,SPR_SPIT_EXP1_3,

    SPR_SPIT2_1,SPR_SPIT2_2,SPR_SPIT2_3,
    SPR_SPIT_EXP2_1,SPR_SPIT_EXP2_2,SPR_SPIT_EXP2_3,

    SPR_SPIT3_1,SPR_SPIT3_2,SPR_SPIT3_3,
    SPR_SPIT_EXP3_1,SPR_SPIT_EXP3_2,SPR_SPIT_EXP3_3,

    //
    // Hanging Terrot
    //

    SPR_TERROT_1,SPR_TERROT_2,SPR_TERROT_3,SPR_TERROT_4,
    SPR_TERROT_5,SPR_TERROT_6,SPR_TERROT_7,SPR_TERROT_8,

    SPR_TERROT_FIRE_1,SPR_TERROT_FIRE_2,SPR_TERROT_DIE_1,SPR_TERROT_DIE_2,
    SPR_TERROT_DIE_3,SPR_TERROT_DIE_4,SPR_TERROT_DEAD,

    //
    // player attack frames
    //
    SPR_KNIFEREADY,SPR_KNIFEATK1,SPR_KNIFEATK2,SPR_KNIFEATK3,
    SPR_KNIFEATK4,

    SPR_PISTOLREADY,SPR_PISTOLATK1,SPR_PISTOLATK2,SPR_PISTOLATK3,
    SPR_PISTOLATK4,

    SPR_MACHINEGUNREADY,SPR_MACHINEGUNATK1,SPR_MACHINEGUNATK2,MACHINEGUNATK3,
    SPR_MACHINEGUNATK4,

    SPR_CHAINREADY,SPR_CHAINATK1,SPR_CHAINATK2,SPR_CHAINATK3,
    SPR_CHAINATK4,

    SPR_GRENADEREADY,SPR_GRENADEATK1,SPR_GRENADEATK2,SPR_GRENADEATK3,
    SPR_GRENADEATK4,

    SPR_GRENADE_FLY1,SPR_GRENADE_FLY2,SPR_GRENADE_FLY3,SPR_GRENADE_FLY4,
    SPR_GRENADE_EXPLODE1,SPR_GRENADE_EXPLODE2,SPR_GRENADE_EXPLODE3,SPR_GRENADE_EXPLODE4,
    SPR_GRENADE_EXPLODE5,

    SPR_ELEC_ARC1,SPR_ELEC_ARC2,SPR_ELEC_ARC3,SPR_ELEC_ARC4,
    SPR_ELEC_POST1,SPR_ELEC_POST2,SPR_ELEC_POST3,SPR_ELEC_POST4,

    SPR_VPOST1,SPR_VPOST2,SPR_VPOST3,SPR_VPOST4,
    SPR_VPOST5,SPR_VPOST6,SPR_VPOST7,SPR_VPOST8,

    SPR_VSPIKE1,SPR_VSPIKE2,SPR_VSPIKE3,SPR_VSPIKE4,
    SPR_VSPIKE5,SPR_VSPIKE6,SPR_VSPIKE7,SPR_VSPIKE8,

    SPR_GREEN_OOZE1,SPR_GREEN_OOZE2,SPR_GREEN_OOZE3,
    SPR_BLACK_OOZE1,SPR_BLACK_OOZE2,SPR_BLACK_OOZE3,

    SPR_GREEN2_OOZE1,SPR_GREEN2_OOZE2,SPR_GREEN2_OOZE3,
    SPR_BLACK2_OOZE1,SPR_BLACK2_OOZE2,SPR_BLACK2_OOZE3,

    SPR_CANDY_BAR,SPR_CANDY_WRAPER,
    SPR_SANDWICH,SPR_SANDWICH_WRAPER,

    SPR_BLAKE_W1,SPR_BLAKE_W2,SPR_BLAKE_W3,SPR_BLAKE_W4,

    //
    // BOSS 1
    //
    SPR_BOSS1_W1,SPR_BOSS1_W2,SPR_BOSS1_W3,SPR_BOSS1_W4,
    SPR_BOSS1_SWING1,SPR_BOSS1_SWING2,SPR_BOSS1_SWING3,
    SPR_BOSS1_DEAD,
    SPR_BOSS1_DIE1,SPR_BOSS1_DIE2,SPR_BOSS1_DIE3,SPR_BOSS1_DIE4,
    SPR_BOSS1_OUCH,
    SPR_BOSS1_PROJ1,SPR_BOSS1_PROJ2,SPR_BOSS1_PROJ3,
    SPR_BOSS1_EXP1,SPR_BOSS1_EXP2,SPR_BOSS1_EXP3,
    SPR_BOSS1_MORPH1,SPR_BOSS1_MORPH2,SPR_BOSS1_MORPH3,SPR_BOSS1_MORPH4,
    SPR_BOSS1_MORPH5,SPR_BOSS1_MORPH6,SPR_BOSS1_MORPH7,SPR_BOSS1_MORPH8,
    SPR_BOSS1_MORPH9,

    //
    // BOSS 2
    //
#if GAME_VERSION != SHAREWARE_VERSION
    SPR_BOSS2_W1,SPR_BOSS2_W2,SPR_BOSS2_W3,SPR_BOSS2_W4,
    SPR_BOSS2_SWING1,SPR_BOSS2_SWING2,SPR_BOSS2_SWING3,
    SPR_BOSS2_DEAD,
    SPR_BOSS2_DIE1,SPR_BOSS2_DIE2,SPR_BOSS2_DIE3,SPR_BOSS2_DIE4,
    SPR_BOSS2_OUCH,

    //
    // BOSS 3
    //
    SPR_BOSS3_W1,SPR_BOSS3_W2,SPR_BOSS3_W3,SPR_BOSS3_W4,
    SPR_BOSS3_SWING1,SPR_BOSS3_SWING2,SPR_BOSS3_SWING3,
    SPR_BOSS3_DEAD,
    SPR_BOSS3_DIE1,SPR_BOSS3_DIE2,SPR_BOSS3_DIE3,SPR_BOSS3_DIE4,
    SPR_BOSS3_OUCH,

    //
    // BOSS 4
    //
    SPR_BOSS4_W1,SPR_BOSS4_W2,SPR_BOSS4_W3,SPR_BOSS4_W4,
    SPR_BOSS4_SWING1,SPR_BOSS4_SWING2,SPR_BOSS4_SWING3,
    SPR_BOSS4_DEAD,
    SPR_BOSS4_DIE1,SPR_BOSS4_DIE2,SPR_BOSS4_DIE3,SPR_BOSS4_DIE4,
    SPR_BOSS4_OUCH,
    SPR_BOSS4_MORPH1,SPR_BOSS4_MORPH2,SPR_BOSS4_MORPH3,SPR_BOSS4_MORPH4,
    SPR_BOSS4_MORPH5,SPR_BOSS4_MORPH6,SPR_BOSS4_MORPH7,SPR_BOSS4_MORPH8,
    SPR_BOSS4_MORPH9,

    //
    // BOSS 5
    //
    SPR_BOSS5_W1,SPR_BOSS5_W2,SPR_BOSS5_W3,SPR_BOSS5_W4,
    SPR_BOSS5_SWING1,SPR_BOSS5_SWING2,SPR_BOSS5_SWING3,
    SPR_BOSS5_DEAD,
    SPR_BOSS5_DIE1,SPR_BOSS5_DIE2,SPR_BOSS5_DIE3,SPR_BOSS5_DIE4,
    SPR_BOSS5_OUCH,
    SPR_BOSS5_PROJ1,SPR_BOSS5_PROJ2,SPR_BOSS5_PROJ3,
    SPR_BOSS5_EXP1,SPR_BOSS5_EXP2,SPR_BOSS5_EXP3,

    //
    // BOSS 6
    //
    SPR_BOSS6_W1,SPR_BOSS6_W2,SPR_BOSS6_W3,SPR_BOSS6_W4,
    SPR_BOSS6_SWING1,SPR_BOSS6_SWING2,SPR_BOSS6_SWING3,
    SPR_BOSS6_DEAD,
    SPR_BOSS6_DIE1,SPR_BOSS6_DIE2,SPR_BOSS6_DIE3,SPR_BOSS6_DIE4,
    SPR_BOSS6_OUCH,

    //
    // BOSS 7
    //
    SPR_BOSS7_W1,SPR_BOSS7_W2,SPR_BOSS7_W3,SPR_BOSS7_W4,
    SPR_BOSS7_SHOOT1,SPR_BOSS7_SHOOT2,SPR_BOSS7_SHOOT3,
    SPR_BOSS7_DEAD,
    SPR_BOSS7_DIE1,SPR_BOSS7_DIE2,SPR_BOSS7_DIE3,SPR_BOSS7_DIE4,
    SPR_BOSS7_OUCH,

    //
    // BOSS 8
    //
    SPR_BOSS8_W1,SPR_BOSS8_W2,SPR_BOSS8_W3,SPR_BOSS8_W4,
    SPR_BOSS8_SHOOT1,SPR_BOSS8_SHOOT2,SPR_BOSS8_SHOOT3,
    SPR_BOSS8_DIE1,SPR_BOSS8_DIE2,SPR_BOSS8_DIE3,SPR_BOSS8_DIE4,
    SPR_BOSS8_DEAD,SPR_BOSS8_OUCH,

    //
    // BOSS 9
    //
    SPR_BOSS9_W1,SPR_BOSS9_W2,SPR_BOSS9_W3,SPR_BOSS9_W4,
    SPR_BOSS9_SHOOT1,SPR_BOSS9_SHOOT2,SPR_BOSS9_SHOOT3,
    SPR_BOSS9_DIE1,SPR_BOSS9_DIE2,SPR_BOSS9_DIE3,SPR_BOSS9_DIE4,
    SPR_BOSS9_DEAD,
    SPR_BOSS9_OUCH,

    //
    // BOSS 10
    //
    SPR_BOSS10_W1,SPR_BOSS10_W2,SPR_BOSS10_W3,SPR_BOSS10_W4,
    SPR_BOSS10_SHOOT1,SPR_BOSS10_SHOOT2,SPR_BOSS10_SHOOT3,
    SPR_BOSS10_DEAD,
    SPR_BOSS10_DIE1,SPR_BOSS10_DIE2,SPR_BOSS10_DIE3,SPR_BOSS10_DIE4,
    SPR_BOSS10_OUCH,

    SPR_BOSS10_SPIT1,SPR_BOSS10_SPIT2,SPR_BOSS10_SPIT3,
    SPR_BOSS10_SPIT_EXP1,SPR_BOSS10_SPIT_EXP2,SPR_BOSS10_SPIT_EXP3,
#endif

    //
    // PLASMA DETONATOR EXPLOSION
    //
    SPR_DETONATOR_EXP1,SPR_DETONATOR_EXP2,SPR_DETONATOR_EXP3,SPR_DETONATOR_EXP4,
    SPR_DETONATOR_EXP5,SPR_DETONATOR_EXP6,SPR_DETONATOR_EXP7,SPR_DETONATOR_EXP8,

    //
    // CLIP EXPLOSION
    //
    SPR_CLIP_EXP1,SPR_CLIP_EXP2,SPR_CLIP_EXP3,SPR_CLIP_EXP4,
    SPR_CLIP_EXP5,SPR_CLIP_EXP6,SPR_CLIP_EXP7,SPR_CLIP_EXP8,

    //
    // BFG CANNON WEAPON
    //
    SPR_BFG_WEAPON1,SPR_BFG_WEAPON2,SPR_BFG_WEAPON3,SPR_BFG_WEAPON4,SPR_BFG_WEAPON5,

    //
    // BFG CANNON WEAPON SHOTS
    //
    SPR_BFG_WEAPON_SHOT1,SPR_BFG_WEAPON_SHOT2,SPR_BFG_WEAPON_SHOT3,

    //
    // BFG SHOT EXPLOSION
    //
    SPR_BFG_EXP1,SPR_BFG_EXP2,SPR_BFG_EXP3,SPR_BFG_EXP4,
    SPR_BFG_EXP5,SPR_BFG_EXP6,SPR_BFG_EXP7,SPR_BFG_EXP8,
};

#if GAME_VERSION == SHAREWARE_VERSION
enum dummysprites
{
    SPR_STAT_1 = 0,
    SPR_STAT_3 = 0,
    SPR_STAT_4 = 0,
    SPR_STAT_5 = 0,
    SPR_STAT_6 = 0,
    SPR_STAT_7 = 0,
    SPR_STAT_16 = 0,
    SPR_STAT_17 = 0,
    SPR_STAT_19 = 0,
    SPR_STAT_20 = 0,
    SPR_STAT_21 = 0,
    SPR_STAT_22 = 0,
    SPR_STAT_23 = 0,
    SPR_STAT_25 = 0,
    SPR_STAT_30 = 0,
    SPR_STAT_37 = 0,
    SPR_STAT_39 = 0,
    SPR_CRATE_2 = 0,
    SPR_CRATE_3 = 0,
    SPR_STAT_58 = 0,
    SPR_STAT_59 = 0,
    SPR_STAT_64 = 0,
    SPR_STAT_70 = 0,
    SPR_STAT_71 = 0,
    SPR_STAT_72 = 0,
    SPR_STAT_73 = 0,
    SPR_STAT_74 = 0,
    SPR_STAT_75 = 0,
    SPR_STAT_76 = 0,

    SPR_GSCOUT_W1_1,SPR_GSCOUT_W1_2,SPR_GSCOUT_W1_3,SPR_GSCOUT_W1_4,
    SPR_GSCOUT_W1_5,SPR_GSCOUT_W1_6,SPR_GSCOUT_W1_7,SPR_GSCOUT_W1_8,

    SPR_GSCOUT_W2_1,SPR_GSCOUT_W2_2,SPR_GSCOUT_W2_3,SPR_GSCOUT_W2_4,
    SPR_GSCOUT_W2_5,SPR_GSCOUT_W2_6,SPR_GSCOUT_W2_7,SPR_GSCOUT_W2_8,

    SPR_GSCOUT_W3_1,SPR_GSCOUT_W3_2,SPR_GSCOUT_W3_3,SPR_GSCOUT_W3_4,
    SPR_GSCOUT_W3_5,SPR_GSCOUT_W3_6,SPR_GSCOUT_W3_7,SPR_GSCOUT_W3_8,

    SPR_GSCOUT_W4_1,SPR_GSCOUT_W4_2,SPR_GSCOUT_W4_3,SPR_GSCOUT_W4_4,
    SPR_GSCOUT_W4_5,SPR_GSCOUT_W4_6,SPR_GSCOUT_W4_7,SPR_GSCOUT_W4_8,

    SPR_GSCOUT_DIE1,SPR_GSCOUT_DIE2,SPR_GSCOUT_DIE3,SPR_GSCOUT_DIE4,
    SPR_GSCOUT_DIE5,SPR_GSCOUT_DIE6,SPR_GSCOUT_DIE7,SPR_GSCOUT_DIE8,

    SPR_GSCOUT_DEAD,

    SPR_FSCOUT_W2_1,SPR_FSCOUT_W2_2,SPR_FSCOUT_W2_3,SPR_FSCOUT_W2_4,
    SPR_FSCOUT_W2_5,SPR_FSCOUT_W2_6,SPR_FSCOUT_W2_7,SPR_FSCOUT_W2_8,

    SPR_FSCOUT_W3_1,SPR_FSCOUT_W3_2,SPR_FSCOUT_W3_3,SPR_FSCOUT_W3_4,
    SPR_FSCOUT_W3_5,SPR_FSCOUT_W3_6,SPR_FSCOUT_W3_7,SPR_FSCOUT_W3_8,

    SPR_FSCOUT_W4_1,SPR_FSCOUT_W4_2,SPR_FSCOUT_W4_3,SPR_FSCOUT_W4_4,
    SPR_FSCOUT_W4_5,SPR_FSCOUT_W4_6,SPR_FSCOUT_W4_7,SPR_FSCOUT_W4_8,

    SPR_VITAL_STAND,
    SPR_VITAL_DIE_1,SPR_VITAL_DIE_2,SPR_VITAL_DIE_3,SPR_VITAL_DIE_4,
    SPR_VITAL_DIE_5,SPR_VITAL_DIE_6,SPR_VITAL_DIE_7,SPR_VITAL_DIE_8,
    SPR_VITAL_DEAD_1,SPR_VITAL_DEAD_2,SPR_VITAL_DEAD_3,
    SPR_VITAL_OUCH,

    SPR_MUTHUM2_W1,SPR_MUTHUM2_W2,SPR_MUTHUM2_W3,SPR_MUTHUM2_W4,
    SPR_MUTHUM2_SWING1,SPR_MUTHUM2_SWING2,SPR_MUTHUM2_SWING3,
    SPR_MUTHUM2_DEAD,
    SPR_MUTHUM2_DIE1,SPR_MUTHUM2_DIE2,SPR_MUTHUM2_DIE3,SPR_MUTHUM2_DIE4,
    SPR_MUTHUM2_OUCH,
    SPR_MUTHUM2_SPIT1,SPR_MUTHUM2_SPIT2,SPR_MUTHUM2_SPIT3,

    SPR_LCAN_ALIEN_READY,SPR_LCAN_ALIEN_B1,SPR_LCAN_ALIEN_B2,
    SPR_LCAN_ALIEN_B3,SPR_LCAN_ALIEN_EMPTY,

    SPR_LCAN_ALIEN_W1,SPR_LCAN_ALIEN_W2,SPR_LCAN_ALIEN_W3,SPR_LCAN_ALIEN_W4,
    SPR_LCAN_ALIEN_SWING1,SPR_LCAN_ALIEN_SWING2,SPR_LCAN_ALIEN_SWING3,
    SPR_LCAN_ALIEN_DEAD,
    SPR_LCAN_ALIEN_DIE1,SPR_LCAN_ALIEN_DIE2,SPR_LCAN_ALIEN_DIE3,
    SPR_LCAN_ALIEN_DIE4,SPR_LCAN_ALIEN_OUCH,
    SPR_LCAN_ALIEN_SPIT1,SPR_LCAN_ALIEN_SPIT2,SPR_LCAN_ALIEN_SPIT3,

    SPR_GURNEY_MUT_READY,SPR_GURNEY_MUT_B1,SPR_GURNEY_MUT_B2,
    SPR_GURNEY_MUT_B3,SPR_GURNEY_MUT_EMPTY,

    SPR_GURNEY_MUT_W1,SPR_GURNEY_MUT_W2,SPR_GURNEY_MUT_W3,SPR_GURNEY_MUT_W4,
    SPR_GURNEY_MUT_SWING1,SPR_GURNEY_MUT_SWING2,SPR_GURNEY_MUT_SWING3,
    SPR_GURNEY_MUT_DEAD,
    SPR_GURNEY_MUT_DIE1,SPR_GURNEY_MUT_DIE2,SPR_GURNEY_MUT_DIE3,
    SPR_GURNEY_MUT_DIE4,SPR_GURNEY_MUT_OUCH,

    SPR_LIQUID_M1,SPR_LIQUID_M2,SPR_LIQUID_M3,
    SPR_LIQUID_R1,SPR_LIQUID_R2,SPR_LIQUID_R3,SPR_LIQUID_R4,
    SPR_LIQUID_S1,SPR_LIQUID_S2,SPR_LIQUID_S3,
    SPR_LIQUID_OUCH,
    SPR_LIQUID_DIE_1,SPR_LIQUID_DIE_2,SPR_LIQUID_DIE_3,SPR_LIQUID_DIE_4,
    SPR_LIQUID_DEAD,
    SPR_LIQUID_SHOT_FLY_1,SPR_LIQUID_SHOT_FLY_2,SPR_LIQUID_SHOT_FLY_3,
    SPR_LIQUID_SHOT_BURST_1,SPR_LIQUID_SHOT_BURST_2,SPR_LIQUID_SHOT_BURST_3,

    SPR_GREEN_OOZE1,SPR_GREEN_OOZE2,SPR_GREEN_OOZE3,
    SPR_BLACK_OOZE1,SPR_BLACK_OOZE2,SPR_BLACK_OOZE3,

    SPR_BOSS2_W1,SPR_BOSS2_W2,SPR_BOSS2_W3,SPR_BOSS2_W4,
    SPR_BOSS2_SWING1,SPR_BOSS2_SWING2,SPR_BOSS2_SWING3,
    SPR_BOSS2_DEAD,
    SPR_BOSS2_DIE1,SPR_BOSS2_DIE2,SPR_BOSS2_DIE3,SPR_BOSS2_DIE4,
    SPR_BOSS2_OUCH,
    SPR_BOSS2_SHOOT1,SPR_BOSS2_SHOOT2,SPR_BOSS2_SHOOT3,
    SPR_BOSS2_PROJ1,SPR_BOSS2_PROJ2,SPR_BOSS2_PROJ3,
    SPR_BOSS2_EXP1,SPR_BOSS2_EXP2,SPR_BOSS2_EXP3,

    SPR_BOSS3_W1,SPR_BOSS3_W2,SPR_BOSS3_W3,SPR_BOSS3_W4,
    SPR_BOSS3_SWING1,SPR_BOSS3_SWING2,SPR_BOSS3_SWING3,
    SPR_BOSS3_DEAD,
    SPR_BOSS3_DIE1,SPR_BOSS3_DIE2,SPR_BOSS3_DIE3,SPR_BOSS3_DIE4,
    SPR_BOSS3_OUCH,
    SPR_BOSS3_SHOOT1,SPR_BOSS3_SHOOT2,SPR_BOSS3_SHOOT3,
    SPR_BOSS3_PROJ1,SPR_BOSS3_PROJ2,SPR_BOSS3_PROJ3,
    SPR_BOSS3_EXP1,SPR_BOSS3_EXP2,SPR_BOSS3_EXP3,

    SPR_BOSS4_W1,SPR_BOSS4_W2,SPR_BOSS4_W3,SPR_BOSS4_W4,
    SPR_BOSS4_SWING1,SPR_BOSS4_SWING2,SPR_BOSS4_SWING3,
    SPR_BOSS4_DEAD,
    SPR_BOSS4_DIE1,SPR_BOSS4_DIE2,SPR_BOSS4_DIE3,SPR_BOSS4_DIE4,
    SPR_BOSS4_OUCH,
    SPR_BOSS4_SHOOT1,SPR_BOSS4_SHOOT2,SPR_BOSS4_SHOOT3,
    SPR_BOSS4_PROJ1,SPR_BOSS4_PROJ2,SPR_BOSS4_PROJ3,
    SPR_BOSS4_EXP1,SPR_BOSS4_EXP2,SPR_BOSS4_EXP3,

    SPR_BOSS5_W1,SPR_BOSS5_W2,SPR_BOSS5_W3,SPR_BOSS5_W4,
    SPR_BOSS5_SWING1,SPR_BOSS5_SWING2,SPR_BOSS5_SWING3,
    SPR_BOSS5_DEAD,
    SPR_BOSS5_DIE1,SPR_BOSS5_DIE2,SPR_BOSS5_DIE3,SPR_BOSS5_DIE4,
    SPR_BOSS5_OUCH,
    SPR_BOSS5_SHOOT1,SPR_BOSS5_SHOOT2,SPR_BOSS5_SHOOT3,
    SPR_BOSS5_PROJ1,SPR_BOSS5_PROJ2,SPR_BOSS5_PROJ3,
    SPR_BOSS5_EXP1,SPR_BOSS5_EXP2,SPR_BOSS5_EXP3,

    SPR_BOSS6_W1,SPR_BOSS6_W2,SPR_BOSS6_W3,SPR_BOSS6_W4,
    SPR_BOSS6_SWING1,SPR_BOSS6_SWING2,SPR_BOSS6_SWING3,
    SPR_BOSS6_DEAD,
    SPR_BOSS6_DIE1,SPR_BOSS6_DIE2,SPR_BOSS6_DIE3,SPR_BOSS6_DIE4,
    SPR_BOSS6_OUCH,
    SPR_BOSS6_SHOOT1,SPR_BOSS6_SHOOT2,SPR_BOSS6_SHOOT3,
    SPR_BOSS6_PROJ1,SPR_BOSS6_PROJ2,SPR_BOSS6_PROJ3,
    SPR_BOSS6_EXP1,SPR_BOSS6_EXP2,SPR_BOSS6_EXP3,
};
#endif

#endif
