//
// So it begins...
//

#define SDL_MAIN_HANDLED

#include "3d_def.h"

char  str[256],error[256];
int   loop,currentsong;

int   param_samplerate = 44100;
int   param_audiobuffer = 2048;


/*
===================
=
= Terminates the program with the appropriate error message
=
===================
*/

void Terminate (const char *func, const char *string, ...)
{
    int     ret;
    va_list vlist;

    if (string)
    {
        va_start (vlist,string);

        if (func)
        {
            snprintf (str,sizeof(str),"%s: %s",func,string);
            vsnprintf (error,sizeof(error),str,vlist);
        }
        else
            vsnprintf (error,sizeof(error),string,vlist);

        va_end (vlist);
    }
    else
        error[0] = '\0';

    ret = *error != '\0';

    SD_Shutdown ();
    PM_Shutdown ();
    IN_Shutdown ();
    VW_Shutdown ();
    CA_Shutdown ();
    Shutdown3DRenderer ();

    //
    // abnormal exit?
    //
    if (ret)
        Error (error);

    SDL_Quit ();
    exit (ret);
}


/*
=====================
=
= InitDigiMap
=
= Using Wolf4SDL's mapping
=
=====================
*/

//
// channel mapping:
//  -1: any non reserved channel
//   0: player weapons
//   1: boss weapons
//
struct digimapstruct
{
    int soundname;
    int chunk;
    int channel;
} digimap[] =
{
    {ATKIONCANNONSND,       0,  0},
    {ATKCHARGEDSND,         1,  0},
    {ATKBURSTRIFLESND,      2,  0},
    {ATKGRENADESND,        46,  0},

    {OPENDOORSND,           3, -1},
    {CLOSEDOORSND,          4, -1},
    {HTECHDOOROPENSND,      5, -1},
    {HTECHDOORCLOSESND,     6, -1},

    {INFORMANTDEATHSND,     7, -1},
    {SCIENTISTHALTSND,     19, -1},
    {SCIENTISTDEATHSND,    20, -1},

    {GOLDSTERNHALTSND,      8, -1},
    {GOLDSTERNLAUGHSND,    24, -1},

    {HALTSND,               9, -1},     // Rent-A-Cop 1st sighting
    {RENTDEATH1SND,        10, -1},     // Rent-A-Cop Death

    {EXPLODE1SND,          11, -1},

    {GGUARDHALTSND,        12, -1},
    {GGUARDDEATHSND,       17, -1},

    {PROHALTSND,           16, -1},
    {PROGUARDDEATHSND,     13, -1},

    {BLUEBOYDEATHSND,      18, -1},
    {BLUEBOYHALTSND,       51, -1},

    {SWATHALTSND,          22, -1},
    {SWATDIESND,           47, -1},

    {SCANHALTSND,          15, -1},
    {SCANDEATHSND,         23, -1},

    {PODHATCHSND,          26, -1},
    {PODHALTSND,           50, -1},
    {PODDEATHSND,          49, -1},

    {ELECTSHOTSND,         27, -1},

    {DOGBOYHALTSND,        14, -1},
    {DOGBOYDEATHSND,       21, -1},
    {ELECARCDAMAGESND,     25, -1},
    {ELECAPPEARSND,        28, -1},
    {ELECDIESND,           29, -1},

    {INFORMDEATH2SND,      39, -1},     // Informant Death #2
    {RENTDEATH2SND,        34, -1},     // Rent-A-Cop Death #2
    {PRODEATH2SND,         42, -1},     // PRO Death #2
    {SWATDEATH2SND,        48, -1},     // SWAT Death #2
    {SCIDEATH2SND,         53, -1},     // Gen. Sci Death #2

    {LIQUIDDIESND,         30, -1},

    {GURNEYSND,            31, -1},
    {GURNEYDEATHSND,       41, -1},

    {WARPINSND,            32, -1},
    {WARPOUTSND,           33, -1},

    {EXPLODE2SND,          35, -1},

    {LCANHALTSND,          36, -1},
    {LCANDEATHSND,         37, -1},

    //{RENTDEATH3SND,        38, -1},     // Rent-A-Cop Death #3
    {INFORMDEATH3SND,      40, -1},     // Informant Death #3
    //{PRODEATH3SND,         43, -1},     // PRO Death #3
    //{SWATDEATH3SND,        52, -1},     // Swat Guard #3
    {SCIDEATH3SND,         54, -1},     // Gen. Sci Death #3

    {LCANBREAKSND,         44, -1},
    {SCANBREAKSND,         45, -1},
    {CLAWATTACKSND,        56, -1},
    {SPITATTACKSND,        55, -1},
    {PUNCHATTACKSND,       57, -1},

    {LASTSOUND}
};


void InitDigiMap (void)
{
    struct digimapstruct *map;

    for (map = &digimap[0]; map->soundname != LASTSOUND; map++)
    {
        DigiMap[map->soundname] = map->chunk;
        DigiChannel[map->chunk] = map->channel;

        SD_PrepareSound (map->chunk);
    }
}


void SetupSound (void)
{
    InitDigiMap ();

    SD_SetMusicMode (smm_AdLib);
    SD_SetSoundMode (sdm_PC);
    SD_SetDigiDevice (sds_SoundBlaster);

    CA_LoadAllSounds ();
}


int main (void)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
        Quit ("Unable to init SDL: %s\n",SDL_GetError());

    VW_Startup ();
    IN_Startup ();
    PM_Startup ();
    SD_Startup ();
    CA_Startup ();
    SetupSound ();
    Init3DRenderer ();

    NewGame (gd_baby,0);
    SetupGameLevel ();
    LoadPlanes (126,126);

    DrawPlayScreen (false);

    PlayLoop ();

    Quit (NULL);

    return 0;
}
