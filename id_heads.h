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
#ifdef _WIN32
#include <io.h>
#include <direct.h>
#endif
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <libgen.h>
#include <SDL.h>
#include <SDL_mixer.h>

/**
// Word alignment MUST be off!!!
*/
#pragma pack(1)

#include "version.h"
#include "gfxv_vsi.h"
#include "audiovsi.h"
#include "spr_vsi.h"

#define INTBUFFERSIZE    ((sizeof(intmax_t) * CHAR_BIT) + 1)    // enough for base 2 SIZE_MAX + null byte

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
#include "platutil.h"

#define Quit(s, ...)    Terminate(__func__,(s),##__VA_ARGS__)

#endif
