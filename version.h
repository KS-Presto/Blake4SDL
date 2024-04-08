// VERSION.H
//
// Defines which version shall be built and configures supported extra features
//

#ifndef _VERSION_H_
#define _VERSION_H_


//
// game versions
//
#define _VERSION_           "V1.01"

#define SHAREWARE_VERSION   0x0001
#define MISSIONS_1_THR_3    0x0003
#define MISSIONS_4_THR_6    0x0004
#define MISSIONS_1_THR_6    0x0007

//
// change this define according to the
// game release versions 1,1-3,4-6, or 1-6.
#define GAME_VERSION        MISSIONS_1_THR_6
//#define GAME_VERSION        MISSIONS_1_THR_3
//#define GAME_VERSION        SHAREWARE_VERSION

#define CHECK_FOR_EPISODES

//
// game features
//
#define CEILING_FLOOR_COLORS
//#define DEMOS_ENABLED
//#define DEMOS_EXTERN         // demo files are stored in the game directory
//#define GEORGE_CHEAT
//#define DUAL_SWAP_FILES      // support for shadowed/non-shadowed page files

//
// debugging stuff
//
//#define TECH_SUPPORT_VERSION
//#define IN_DEVELOPMENT
//#define TRACK_ENEMY_COUNT
//#define DEBUG_STATICS

//
// code features
//
#define USE_TRANSFORMAREAS
#define UPDATE_OTHER_SWITCHES
//#define CLASSIC_MENU              // disables the extra menu screens/options
//#define WOLFDOORS               // doors open from the side
//#define MASKABLE_DOORS          // doors are drawn with transparent pixels (not currently working)


//
// actor features
//
//#define LOOK_FOR_DEAD_GUYS      // actor will be alerted by dead actors
#define LIMITED_AMMO            // actor will run out of ammo
//#define ONE_TRACK_MIND          // actor will pick up only the object he's searching for
#define EXPLODE_STATIC_SCAN
#define BFG_SHOT_STOPS
//#define DODGE_N_CHASE


//
// misc
//
#define OBJ_RESERV              // reserve certain actors/statics to ensure they're available

#define ID_CACHE_BRIEFS
#define ID_CACHE_HELP
#define ID_CACHE_LOSE
#define ID_CACHE_CREDITS

//#define SKIP_TITLE_AND_CREDITS

#endif
