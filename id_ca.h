// ID_CA.H

#ifndef _ID_CA_H_
#define _ID_CA_H_


#define NUM_EPISODES     1
#define MAPS_PER_EPISODE 25  // TODO: 24?
#define MAPS_WITH_STATS  20

#define NUMMAPS          (NUM_EPISODES * MAPS_PER_EPISODE)
#define MAPPLANES        3

#define THREEBYTEGRSTARTS

#ifdef THREEBYTEGRSTARTS
#define FILEPOSSIZE      3
#else
#define FILEPOSSIZE      4
#endif

#define STR_BAD_VGAHEAD  "Blake4SDL was not compiled for these data files:\n"\
                         "%s contains the wrong number of offsets (%d instead of %d)\n\n"\
                         "Please check whether you are using the right files!"

typedef struct
{
    int32_t  planestart[MAPPLANES];
    word     planelength[MAPPLANES];
    word     width,height;
    char     name[16];
} maptype;


typedef struct
{
    word     bit0,bit1;    // 0-255 is a character, > is a pointer to a node
} huffnode;


typedef struct
{
    word     RLEWtag;
    int32_t  headeroffsets[NUMMAPS];
} mapfiletype;

extern  mapfiletype *tinf;

extern  word        *mapsegs[MAPPLANES];
extern  maptype     *mapheaderseg[NUMMAPS];
extern  byte        *audiosegs[NUMSNDCHUNKS];
extern  byte        *grsegs[NUMCHUNKS];

extern  char        gheadname[13];
extern  char        gfilename[13];
extern  char        gdictname[13];
extern  char        mheadname[13];
extern  char        mfilename[13];
extern  char        aheadname[13];
extern  char        afilename[13];

extern  int32_t     *audiostarts;    // array of offsets in audio / audiot

extern  FILE        *audiofile;
extern  int32_t     chunkcomplen,chunkexplen;

size_t  CA_GetFileLength (FILE *file);

void    CA_RLEWexpand (word *source, word *dest, int32_t length, word rlewtag);

void    CA_Startup (void);
void    CA_Shutdown (void);

int32_t CA_CacheAudioChunk (int chunk);
void    CA_UncacheAudioChunk (int chunk);
void    CA_LoadAllSounds (void);

void    CA_CacheGrChunks (FILE *grfile, int32_t *offset, huffnode *hufftable);
void    CA_CacheMap (int mapnum);

void    CA_SetupAudioFile (void);
void    CA_SetupGrFile (void);
void    CA_SetupMapFile (void);
void    CA_HuffExpand (byte *source, byte *dest, int32_t length, huffnode *hufftable);

void    CA_CannotOpen (const char *string);
#endif
