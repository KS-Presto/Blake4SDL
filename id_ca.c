// ID_CA.C
// Id Software Caching Manager
//
// This has been customized for BLAKE STONE
//

#include "3d_def.h"

mapfiletype *tinf;

word        *mapsegs[MAPPLANES];
maptype     *mapheaderseg[NUMMAPS];
byte        *audiosegs[NUMSNDCHUNKS];
byte        *grsegs[NUMCHUNKS];

char        gheadname[13];
char        gfilename[13];
char        gdictname[13];
char        mheadname[13];
#ifdef CARMACIZED
char        mfilename[13];
#else
char        mfilename[13];
#endif
char        aheadname[13];
char        afilename[13];

int32_t     *audiostarts;  // array of offsets in audio / audiot

FILE        *audiofile;

int32_t     chunkcomplen,chunkexplen;

byte        oldsoundmode;


/*
=============================================================================

                    LOW LEVEL ROUTINES

=============================================================================
*/


/*
======================
=
= CA_GetFileLength
=
======================
*/

size_t CA_GetFileLength (FILE *file)
{
    size_t length;

    fseek (file,0,SEEK_END);
    length = ftell(file);
    fseek (file,0,SEEK_SET);

    return length;
}


/*
============================================================================

                COMPRESSION routines, see JHUFF.C for more

============================================================================
*/


/*
======================
=
= CA_HuffExpand
=
= Length is the length of the EXPANDED data
=
======================
*/

void CA_HuffExpand (byte *source, byte *dest, int32_t length, huffnode *hufftable)
{
    unsigned mask,nodevalue;
    huffnode *headptr,*nodeon;
    byte     *end;

    if (!length || !dest)
        Quit ("Length or dest is NULL!");

    headptr = &hufftable[254];    // head node is always node 254

    end = &dest[length];
    mask = 1;
    nodeon = headptr;

    while (dest < end)
    {
        if (*source & mask)
            nodevalue = nodeon->bit1;
        else
            nodevalue = nodeon->bit0;

        if (mask == 128)
        {
            mask = 1;
            source++;
        }
        else
            mask <<= 1;

        if (nodevalue < 256)
        {
            *dest++ = (byte)nodevalue;
            nodeon = headptr;
        }
        else
            nodeon = &hufftable[nodevalue - 256];
    }
}


/*
======================
=
= CA_CarmackExpand
=
= Length is the length of the EXPANDED data
=
======================
*/

#define NEARTAG 0xa7
#define FARTAG 0xa8

#ifdef CARMACIZED

void CA_CarmackExpand (word *source, word *dest, word length)
{
    unsigned ch,chhigh,count,offset;
    word    *copyptr,*inptr,*outptr;

    length /= 2;

    inptr = source;
    outptr = dest;

    while (length)
    {
        ch = *inptr++;
        chhigh = ch >> 8;

        if (chhigh == NEARTAG)
        {
            count = ch & 0xff;

            if (!count)
            {
                //
                // have to insert a word containing the tag byte
                //
                ch |= *((byte *)inptr)++;
                *outptr++ = ch;
                length--;
            }
            else
            {
                offset = *((byte *)inptr)++;
                copyptr = outptr - offset;
                length -= count;

                while (count--)
                    *outptr++ = *copyptr++;
            }
        }
        else if (chhigh == FARTAG)
        {
            count = ch & 0xff;

            if (!count)
            {
                //
                // have to insert a word containing the tag byte
                //
                ch |= *((byte *)inptr)++;
                *outptr++ = ch;
                length --;
            }
            else
            {
                offset = *inptr++;
                copyptr = dest + offset;
                length -= count;

                while (count--)
                    *outptr++ = *copyptr++;
            }
        }
        else
        {
            *outptr++ = ch;
            length--;
        }
    }
}

#endif

/*
======================
=
= CA_RLEWexpand
=
= length is EXPANDED length
=
======================
*/

void CA_RLEWexpand (word *source, word *dest, int32_t length, word rlewtag)
{
    unsigned value,count;
    word     *end;

    end = &dest[length / 2];

//
// expand it
//
    while (dest < end)
    {
        value = *source++;

        if (value != rlewtag)
            *dest++ = value;    // uncompressed
        else
        {
            //
            // compressed string
            //
            count = *source++;
            value = *source++;

            while (count--)
                *dest++ = value;
        }
    }
}


/*
=============================================================================

                    CACHE MANAGER ROUTINES

=============================================================================
*/


/*
======================
=
= CA_SetupGrFile
=
======================
*/

void CA_SetupGrFile (void)
{
    void     *compseg;
    int      i;
    int32_t  value;
    int32_t  *grstarts;         // array of offsets in vgagraph, -1 for sparse
    byte     *offsets,*ofsptr;
    size_t   length,headersize;
    FILE     *file;
    huffnode *grhuffman;

//
// load vgadict (huffman dictionary for graphics files)
//
    file = fopen(gdictname,"rb");

    if (!file)
        CA_CannotOpen (gdictname);

    length = CA_GetFileLength(file);

    grhuffman = SafeMalloc(length);

    fread (grhuffman,length,1,file);
    fclose (file);

//
// load the data offsets from vgahead
//
    file = fopen(gheadname,"rb");

    if (!file)
        CA_CannotOpen (gheadname);

    headersize = CA_GetFileLength(file) / FILEPOSSIZE;

    if (headersize != NUMCHUNKS + 1)
        Terminate (NULL,STR_BAD_VGAHEAD,gheadname,headersize,NUMCHUNKS + 1);

    grstarts = SafeMalloc((NUMCHUNKS + 1) * sizeof(*grstarts));

    offsets = SafeMalloc((NUMCHUNKS + 1) * FILEPOSSIZE);
    ofsptr = offsets;

    fread (ofsptr,(NUMCHUNKS + 1) * FILEPOSSIZE,1,file);

    for (i = 0; i < NUMCHUNKS + 1; i++)
    {
        if (FILEPOSSIZE == 3)
        {
            value = ofsptr[0] | (ofsptr[1] << 8) | (ofsptr[2] << 16);

            if (value == 0xffffff)
                value = -1;

            ofsptr += FILEPOSSIZE;
        }
        else
            value = ReadLong(&ofsptr[i]);

        grstarts[i] = value;
    }

    free (offsets);

    fclose (file);

//
// Open the graphics file
//
    file = fopen(gfilename,"rb");

    if (!file)
        CA_CannotOpen (gfilename);

//
// load the pic headers
//
    pictable = SafeMalloc(NUMPICS * sizeof(*pictable));

    fseek (file,grstarts[STRUCTPIC],SEEK_SET);
    fread (&chunkexplen,sizeof(chunkexplen),1,file);
    chunkcomplen = grstarts[STRUCTPIC + 1] - grstarts[STRUCTPIC] - sizeof(chunkexplen);

    compseg = SafeMalloc(chunkcomplen);
    fread (compseg,chunkcomplen,1,file);
    CA_HuffExpand (compseg,(byte *)pictable,NUMPICS * sizeof(*pictable),grhuffman);
    free (compseg);

//
// load in all the graphics chunks
//
    CA_CacheGrChunks (file,grstarts,grhuffman);

    fclose (file);

    free (grstarts);
    free (grhuffman);
}


/*
======================
=
= CA_SetupMapFile
=
======================
*/

void CA_SetupMapFile (void)
{
    int     i;
    int32_t pos;
    FILE    *file;

//
// load maphead.ext (offsets and tileinfo for map file)
//
    file = fopen(mheadname,"rb");

    if (!file)
        CA_CannotOpen (mheadname);

    tinf = SafeMalloc(sizeof(*tinf));

    fread (tinf,sizeof(*tinf),1,file);
    fclose (file);

//
// open the data file
//
    file = fopen(mfilename,"rb");

    if (!file)
        CA_CannotOpen (mfilename);

//
// load all map header
//
    for (i = 0; i < NUMMAPS; i++)
    {
        pos = tinf->headeroffsets[i];

        if (pos < 0)                // $FFFFFFFF start is a sparse map
            continue;

        mapheaderseg[i] = SafeMalloc(sizeof(*mapheaderseg[i]));

        fseek (file,pos,SEEK_SET);
        fread (mapheaderseg[i],sizeof(*mapheaderseg[i]),1,file);
    }

    fclose (file);

//
// allocate space for all planes
//
    for (i = 0; i < MAPPLANES; i++)
        mapsegs[i] = SafeMalloc((MAPSIZE * MAPSIZE) * sizeof(*mapsegs[i]));
}


/*
======================
=
= CA_SetupAudioFile
=
======================
*/

void CA_SetupAudioFile (void)
{
    size_t length;

    audiofile = fopen(aheadname,"rb");

    if (!audiofile)
        CA_CannotOpen (aheadname);

    length = CA_GetFileLength(audiofile);

    audiostarts = SafeMalloc(length);

    fread (audiostarts,length,1,audiofile);
    fclose (audiofile);

//
// open the data file
//
// TODO: see if it's worth reading it all in and then closing the file
//
    audiofile = fopen(afilename,"rb");

    if (!audiofile)
        CA_CannotOpen (afilename);
}


/*
======================
=
= CA_Startup
=
= Open all files and load in headers
=
======================
*/

void CA_Startup (void)
{
    CA_SetupGrFile ();
    CA_SetupMapFile ();
    CA_SetupAudioFile ();
}


/*
======================
=
= CA_Shutdown
=
= Close all files & deallocate everything
=
======================
*/

void CA_Shutdown (void)
{
    int i;

    if (audiofile)
        fclose (audiofile);

    for (i = 0; i < NUMCHUNKS; i++)
    {
        free (grsegs[i]);
        grsegs[i] = NULL;
    }

    for (i = 0; i < NUMMAPS; i++)
    {
        free (mapheaderseg[i]);
        mapheaderseg[i] = NULL;
    }

    for (i = 0; i < MAPPLANES; i++)
    {
        free (mapsegs[i]);
        mapsegs[i] = NULL;
    }

    for (i = 0; i < NUMSNDCHUNKS; i++)
        CA_UncacheAudioChunk (i);

    free (pictable);
    pictable = NULL;

    free (tinf);
    tinf = NULL;

    free (audiostarts);
    audiostarts = NULL;
}


/*
======================
=
= CA_UncacheAudioChunk
=
= Free the given audio chunk
=
======================
*/

void CA_UncacheAudioChunk (int chunk)
{
    free (audiosegs[chunk]);
    audiosegs[chunk] = NULL;
}


/*
======================
=
= CA_UncacheAudioChunks
=
= Deallocate audio chunks for the current mode
=
======================
*/

void CA_UncacheAudioChunks (int start)
{
    int i;

    for (i = 0; i < NUMSOUNDS; i++, start++)
        CA_UncacheAudioChunk (start);
}


/*
======================
=
= CA_CacheAudioChunk
=
= Load the given audio chunk into the audio buffer
=
= Returns the chunk's length
=
======================
*/

int32_t CA_CacheAudioChunk (int chunk)
{
    int32_t pos,length;

//
// load the chunk into a buffer
//
    pos = audiostarts[chunk];
    length = audiostarts[chunk + 1] - pos;

    if (audiosegs[chunk])
        return length;          // already in memory

    audiosegs[chunk] = SafeMalloc(length);

    fseek (audiofile,pos,SEEK_SET);
    fread (audiosegs[chunk],length,1,audiofile);

    return length;
}


/*
======================
=
= CA_LoadAllSounds
=
= Purges all sounds, then loads all new ones (mode switch)
=
======================
*/

void CA_LoadAllSounds (void)
{
    int start,i;

    if (oldsoundmode != sdm_Off)
    {
        switch (oldsoundmode)
        {
            case sdm_PC:
                start = STARTPCSOUNDS;
                break;

            case sdm_AdLib:
                start = STARTADLIBSOUNDS;
                break;
        }

        CA_UncacheAudioChunks (start);
    }

    oldsoundmode = SoundMode;

    switch (SoundMode)
    {
        case sdm_Off:
            start = STARTADLIBSOUNDS;   // needed for priorities
            break;

        case sdm_PC:
            start = STARTPCSOUNDS;
            break;

        case sdm_AdLib:
            start = STARTADLIBSOUNDS;
            break;
    }

    for (i = 0; i < NUMSOUNDS; i++, start++)
        CA_CacheAudioChunk (start);
}


/*
======================
=
= CA_DePlaneGrChunk
=
======================
*/

void CA_DePlaneGrChunk (int chunk)
{
    int i;
    int width,height;

    if (chunk == STARTTILE8)
    {
        width = height = 8;

        for (i = 0; i < NUMTILE8; i++)
            VW_DePlaneVGA (grsegs[chunk] + (i * (width * height)),width,height);
    }
    else
    {
        width = pictable[chunk - STARTPICS].width;
        height = pictable[chunk - STARTPICS].height;

        VW_DePlaneVGA (grsegs[chunk],width,height);
    }
}


/*
======================
=
= CA_ExpandGrChunk
=
= Does whatever is needed with a pointer to a compressed chunk
=
======================
*/

void CA_ExpandGrChunk (int chunk, byte *source, huffnode *hufftable)
{
    int32_t expanded;

    if (chunk >= STARTTILE8 && chunk < STARTEXTERNS)
    {
        //
        // expanded sizes of tile8/16/32 are implicit
        //
        if (chunk == STARTTILE8)   // tile 8s are all in one chunk!
            expanded = 64 * NUMTILE8;
    }
    else
    {
        //
        // everything else has an explicit size
        //
        expanded = ReadLong(source);
        source += sizeof(expanded);           // skip over length
    }

    //
    // allocate final space & decompress it
    //
    grsegs[chunk] = SafeMalloc(expanded);

    CA_HuffExpand (source,grsegs[chunk],expanded,hufftable);
}


/*
======================
=
= CA_CacheGrChunks
=
= Load all graphics chunks
=
======================
*/

void CA_CacheGrChunks (FILE *grfile, int32_t *offset, huffnode *hufftable)
{
    int32_t pos,compressed;
    byte    *source;
    int     chunk,next;

    for (chunk = STRUCTPIC; chunk < NUMCHUNKS; chunk++)
    {
        if (grsegs[chunk])
            continue;       // already in memory

        //
        // load the chunk into a buffer
        //
        pos = offset[chunk];

        if (pos < 0)       // $FFFFFFFF start is a sparse tile
            continue;

        next = chunk + 1;

        while (offset[next] == -1)  // skip past any sparse tiles
            next++;

        compressed = offset[next] - pos;

        fseek (grfile,pos,SEEK_SET);

        source = SafeMalloc(compressed);
        fread (source,compressed,1,grfile);

        CA_ExpandGrChunk (chunk,source,hufftable);

        if (chunk >= STARTPICS && chunk < STARTEXTERNS)
            CA_DePlaneGrChunk (chunk);

        free (source);
    }
}


/*
======================
=
= CA_CacheMap
=
= This is specialized for a 64*64 map size
=
======================
*/

void CA_CacheMap (int mapnum)
{
    FILE    *file;
    int     plane;
    int32_t pos,compressed;
    int32_t size;
    word    *source,*dest;
#ifdef CARMACIZED
    word    *rlewtable;
    int32_t expanded;
#endif
    file = fopen(mfilename,"rb");

    if (!file)
        CA_CannotOpen (mfilename);

//
// load the planes into the already allocated buffers
//
    size = (MAPSIZE * MAPSIZE) * sizeof(*dest);

    for (plane = 0; plane < MAPPLANES; plane++)
    {
        pos = mapheaderseg[mapnum]->planestart[plane];
        compressed = mapheaderseg[mapnum]->planelength[plane];

        if (!compressed)
            continue;    // empty plane

        dest = mapsegs[plane];

        fseek (file,pos,SEEK_SET);

        source = SafeMalloc(compressed);

        fread (source,compressed,1,file);

#ifdef CARMACIZED
        //
        // unhuffman, then unRLEW
        // The huffman'd chunk has a two byte expanded length first
        // The resulting RLEW chunk also does, even though it's not really
        // needed
        //
        expanded = *source;

        rlewtable = SafeMalloc(expanded);

        CA_CarmackExpand (source + 1,rlewtable,expanded);
        CA_RLEWexpand (rlewtable + 1,*dest,size,tinf->RLEWtag);

        free (rlewtable);
        rlewtable = NULL;
#else
        //
        // unRLEW, skipping expanded length
        //
        CA_RLEWexpand (source + 1,dest,size,tinf->RLEWtag);
#endif
        free (source);
        source = NULL;
    }

    fclose (file);
}


/*
======================
=
= CA_CannotOpen
=
======================
*/

void CA_CannotOpen (const char *string)
{
    Quit ("Can't open %s!",string);
}
