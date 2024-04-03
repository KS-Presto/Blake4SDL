//
// ID Engine
// ID_HEADS.h - Generic types, #defines, etc.
// v1.0d1
//

#ifndef _ID_HEADS_H_
#define _ID_HEADS_H_

#define SDL_MAIN_HANDLED

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <sys\stat.h>
#include <dir.h>
#include <libgen.h>
#include <SDL.h>
#include <SDL_mixer.h>

/**
// Word alignment MUST be off!!!
*/
#pragma pack(1)


#define DEBUG_VALUE
#define CEILING_FLOOR_COLORS


//#define CARMACIZED
#define WOLF
//#define DEMOS_ENABLED
#define GEORGE_CHEAT        false

//
// GAME VERSION TYPES
//
#define SHAREWARE_VERSION   0x0001
#define MISSIONS_1_THR_3    0x0003
#define MISSIONS_4_THR_6    0x0004
#define MISSIONS_1_THR_6    0x0007

//
// CURRENT GAME VERSION DEFINE - Change this define according to the
//           game release versions 1,1-3,4-6, or 1-6.

#define GAME_VERSION            (MISSIONS_1_THR_6)
//#define GAME_VERSION            (MISSIONS_1_THR_3)
//#define GAME_VERSION            (SHAREWARE_VERSION)


#define TECH_SUPPORT_VERSION    false
#define IN_DEVELOPMENT          false

#define DUAL_SWAP_FILES         false    // support for shadowed/non-shadowed page files

#include "gfxv_vsi.h"
#include "audiovsi.h"
#include "mapsvsi.h"
#include "spr_vsi.h"


#ifndef __cplusplus
enum boolean
{
    false,
    true,
};
typedef int8_t   bool;
#endif

typedef uint8_t  byte;
typedef uint16_t word;
typedef uint32_t longword;
typedef int32_t  fixed;
typedef int32_t  ScanCode;

typedef struct
{
    int x,y;
} Point;

typedef struct
{
    Point ul,lr;
} Rect;


#include "id_pm.h"
#include "id_ca.h"
#include "id_vw.h"
#include "id_in.h"
#include "id_sd.h"
#include "id_us.h"

#include "jm_io.h"
#include "jm_lzh.h"
#include "jm_tp.h"
#include "movie.h"

#define Quit(s, ...)    Terminate(__func__,(s),##__VA_ARGS__)

#endif
