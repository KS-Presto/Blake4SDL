//
// So it begins...
//

#define SDL_MAIN_HANDLED

#include "3d_def.h"

char  str[256],error[256];
int   pickquick;
bool  EscPressed;
bool  ShowQuickMsg;

int   param_samplerate = 44100;
int   param_audiobuffer = 2048;
int   lastmusic;


void Message (const char *string)
{
    int        i;
    int        x,y,w,h,mw;
    int        len;
    fontstruct *font;

    w = mw = 0;
    fontnumber = 1;

    font = (fontstruct *)grsegs[STARTFONT + fontnumber];

    h = font->height;
    len = strlen(string);

    for (i = 0; i < len; i++)
    {
        if (string[i] == '\n')
        {
            if (w > mw)
                mw = w;

            w = 0;
            h += font->height;
        }
        else
            w += font->width[(byte)string[i]];
    }

    if (w + 10 > mw)
        mw = w + 10;

    x = 160 - (mw / 2);
    y = (WindowH / 2) - (h / 2);

    //
    // bump down and to the right for shadow
    //
    PrintX = x + 1;
    PrintY = y + 1;
    WindowX = PrintX;

    BevelBox (WindowX - 6,PrintY - 6,mw + 10,h + 10,BORDER_HI_COLOR,BORDER_MED_COLOR,BORDER_LO_COLOR);

    SetFontColor (BORDER_LO_COLOR,BORDER_MED_COLOR);
    US_Print (string);

    WindowX = PrintX = x;
    PrintY = y;

    SetFontColor (BORDER_TEXT_COLOR,BORDER_MED_COLOR);
    US_Print (string);

    VW_UpdateScreen (screen.buffer);
}


unsigned Random (unsigned Max)
{
    return Max;
}


//
// TODO: this can be merged with StartMusic
//
int StartCPMusic (int song)
{
    int lastoffs;

    lastmusic = song;
    lastoffs = SD_MusicOff();
    CA_UncacheAudioChunk (STARTMUSIC + song);

    SD_StartMusic (STARTMUSIC + song);

    return lastoffs;
}


void CycleColors (void)
{
#ifdef NOTYET
    #define NUM_RANGES     5
    #define CRNG_LOW       0xf0
    #define CRNG_HIGH      0xfe
    #define CRNG_SIZE      (CRNG_HIGH - CRNG_LOW + 1)

    static CycleInfo crange[NUM_RANGES] =
    {
        {7,0,0xf0,0xf1},
        {15,0,0xf2,0xf3},
        {30,0,0xf4,0xf5},
        {10,0,0xf6,0xf9},
        {12,0,0xfa,0xfe},
    };

    int       i;
    CycleInfo *c;
    byte      cbuffer[CRNG_SIZE][3],temp[3];
    int       first,last,numregs;
    bool      changes = false;

    for (i = 0; i < NUM_RANGES; i++)
    {
        c = &crange[i];

        if (tics >= c->delay_count)
        {
            if (!changes)
            {
                VW_GetPalette (CRNG_LOW,CRNG_SIZE,(byte *)cbuffer);

                changes = true;
            }

            first = c->firstreg - CRNG_LOW;
            numregs = c->lastreg - c->firstreg;    // is one less than in range
            last = first + numregs;

            memcpy (temp,cbuffer[last],sizeof(temp));
            memmove (cbuffer[first + 1],cbuffer[first],numregs * 3);
            memcpy (cbuffer[first],temp,sizeof(temp));

            c->delay_count = c->init_delay;
        }
        else
            c->delay_count -= tics;
    }

    if (changes)
        VW_SetPalette (CRNG_LOW,CRNG_SIZE,(byte *)cbuffer);
    else
        VW_WaitVBL (1);
#endif
}


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
    SD_SetSoundMode (sdm_AdLib);
    SD_SetDigiDevice (sds_SoundBlaster);

    CA_LoadAllSounds ();
}


int32_t FindChunk (FILE *file, const char *chunk)
{
    int32_t chunklen;
    char    fchunk[5] = {0,0,0,0,0};

    while (1)
    {
        //
        // read chunk id
        //
        if (fread(fchunk,sizeof(fchunk) - 1,1,file) != 4)
            break;

        fread (&chunklen,sizeof(chunklen),1,file);

        //
        // look for chunk (sub-check!)
        //
        if (strstr(fchunk,chunk))
            return chunklen;      // chunk found!

        fseek (file,chunklen,SEEK_CUR);
    }

    fseek (file,0,SEEK_END);      // make sure we're at the end

    return 0;
}


int32_t DeleteChunk (FILE *sourcefile, const char *chunk)
{
    int32_t filesize,cksize,offset,bmove;
    FILE    *destfile;

    filesize = CA_GetFileLength(sourcefile);

    cksize = FindChunk(sourcefile,chunk);

    if (cksize)
    {
        offset = ftell(sourcefile) - 8;
        bmove = filesize - (offset + 8 + cksize);   // figure bytes to move

        if (bmove)
        {
            //
            // move data: FROM --> the start of NEXT chunk through the end of file
            //              TO --> the start of THIS chunk
            //
            // (ie: erase THIS chunk and re-write it at the end of the file!)
            //
            fseek (sourcefile,cksize,SEEK_CUR);     // seek source to NEXT chunk

            MakeDestPath (PLAYTEMP_FILE);

            destfile = fopen(tempPath,"rb+");

            if (!destfile)
                CA_CannotOpen (tempPath);

            fseek (destfile,offset,SEEK_SET);       // seek dest to THIS chunk

            IO_CopyFile (sourcefile,destfile,bmove);

            fclose (destfile);

            fseek (sourcefile,offset + bmove,SEEK_SET); // go to end of data moved
        }
        else
            fseek (sourcefile,offset,SEEK_SET);
    }

    return cksize;
}


char    tempPath[MAX_DEST_PATH_LEN + 15];


void InitPlaytemp (void)
{
    FILE *file;

    MakeDestPath (PLAYTEMP_FILE);

    file = fopen(tempPath,"wb+");  // TODO: need to create the file for now, make sure this changes!!!

    if (!file)
        CA_CannotOpen (tempPath);

    fclose (file);
}


void MakeDestPath (const char *file)
{
    snprintf (tempPath,sizeof(tempPath),file);
}


/*
==========================
=
= ClearNClose
=
= Called when changing levels via standard elevator
=
= This code doesn't CLEAR the elevator door as originally
= planned because actors were coded to stay out of the
= elevator doorway
=
==========================
*/

void ClearNClose (void)
{
    int x,y,tx,ty,px,py;
    int doornum;

    tx = ty = 0;
    px = player->x >> TILESHIFT;
    py = player->y >> TILESHIFT;

 //
 // locate the door
 //
    for (x = -1; x < 2 && !tx; x += 2)
    {
        for (y = -1; y < 2; y += 2)
        {
            if (tilemap[px + x][py + y] & 0x80)
            {
                tx = px + x;
                ty = py + y;
                break;
            }
        }
    }

    if (tx)
    {
        //
        // close the door & make it solid
        //
        doornum = tilemap[tx][ty] & 63;

        doorobjlist[doornum].action = dr_closed;
        doorobjlist[doornum].position = 0;

        actorat[tx][ty] = (objtype *)(doornum | 0x80);
    }
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
    InitPlaytemp ();
    NewGame (gd_baby,0);

    do
    {
        GameLoop ();

    } while (playstate != ex_abort);

    if (playstate != ex_abort)
        Quit ("PlayLoop exited: %d",playstate);
    else
        Quit (NULL);

    return 0;
}
