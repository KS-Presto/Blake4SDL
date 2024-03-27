// MOVIE.C

#include "3d_def.h"
#include "an_codes.h"


//
// various types for various purposes
//
enum FADES
{
    FADE_NONE,
    FADE_SET,
    FADE_IN,
    FADE_OUT,
    FADE_IN_FRAME,
    FADE_OUT_FRAME,
};


enum MOVIE_FLAGS
{
    MV_NONE,
    MV_FILL,
    MV_SKIP,
    MV_READ,
};


FILE       *moviefile;

unsigned   fadeflags,fi_type,fo_type;
byte       fi_rate,fo_rate;

void       *MovieBuffer;
byte       *BufferPtr;        // ptr to next frame in MovieBuffer

unsigned   movieflag;
bool       ExitMovie;
bool       EverFaded;
int32_t    movietimecount;


//
// MOVIE definitions for external movies
//
MovieStuff_t Movies[mv_NUM_MOVIES] =
{
    {{"IANIM."},3},     // mv_intro
    {{"EANIM."},30},    // mv_final
};


/*
====================
=
= SetupMovie
=
= Inits all the internal routines for the Movie Presenter
=
====================
*/

void SetupMovie (MovieStuff_t *MovieStuff)
{
    size_t length;

    movieflag = MV_FILL;
    ExitMovie = false;
    EverFaded = false;

    IN_ClearKeysDown ();

    VW_ClearScreen (0);

    length = CA_GetFileLength(moviefile);

    MovieBuffer = SafeMalloc(length);
    BufferPtr = MovieBuffer;

    if (!fread(BufferPtr,length,1,moviefile))
        Quit ("Animation file is corrupt or truncated!");
}


/*
====================
=
= ShutdownMovie
=
====================
*/

void ShutdownMovie (void)
{
    free (MovieBuffer);
    MovieBuffer = BufferPtr = NULL;

    fclose (moviefile);
}


/*
====================
=
= Movie_DrawSeg
=
= Draws a segment of [length] pixels starting at the given
= offset in the screen buffer
=
====================
*/

void Movie_DrawSeg (byte *source, int offset, int length)
{
    byte *dest;

    dest = VW_LockSurface(screen.buffer);

    if (!dest)
        Quit ("Unable to lock surface: %s\n",SDL_GetError());

    memcpy (dest + offset,source,length);

    VW_UnlockSurface (screen.buffer);
}


/*
====================
=
= MOVIE_ShowFrame
=
====================
*/

void MOVIE_ShowFrame (byte *inpic)
{
    anim_chunk *ah;

    if (!inpic)
        return;

    while (1)
    {
        ah = (anim_chunk *)inpic;

        if (!ah->command)
            break;

        inpic += sizeof(anim_chunk);

        Movie_DrawSeg (inpic,ah->offset,ah->length);

        inpic += ah->length;
    }
}


/*
====================
=
= MOVIE_HandlePage
=
= Handles the current page of data from the ram buffer
=
====================
*/

void MOVIE_HandlePage (MovieStuff_t *MovieStuff)
{
    anim_frame  blk;
    byte        *frame;
    unsigned    value;
    unsigned    waittime;
    int32_t     lasttime;
    ControlInfo ci;

    memcpy (&blk,BufferPtr,sizeof(blk));

    if (blk.code == AN_END_OF_ANIM)
    {
        ExitMovie = true;
        return;
    }

    BufferPtr += sizeof(blk);
    frame = BufferPtr;

    IN_ReadControl (&ci);

    switch (blk.code)
    {
        case AN_SOUND:
            value = ReadShort(frame);
            SD_PlaySound (value);
            break;

        case AN_FADE_IN_FRAME:
            VW_FadePaletteIn (0,255,MovieStuff->palette,30);
            fadeflags = FADE_NONE;
            EverFaded = true;
            break;

        case AN_FADE_OUT_FRAME:
            VW_FadeOut ();
            fadeflags = FADE_NONE;
            break;

        case AN_PAUSE:
            value = ReadShort(frame);
            IN_UserInput (value);
            break;

        case AN_GRPAGE:
            if (movieflag == MV_FILL)
            {
                //
                // first page coming in; fill screen with fill color
                //
                movieflag = MV_NONE;
                VW_ClearScreen (*frame++);
            }

            MOVIE_ShowFrame (frame);

            VW_UpdateScreen (screen.buffer);

            lasttime = GetTimeCount() - movietimecount;

            if (lasttime < MovieStuff->ticdelay)
            {
                waittime = MovieStuff->ticdelay - lasttime;

                VW_WaitVBL (waittime);
            }
            else
                VW_WaitVBL (1);

            movietimecount = 0;

            if (!(screen.flags & SC_FADED) && (ci.button0 || ci.button1 || LastScan))
            {
                ExitMovie = true;

                if (EverFaded)     // this needs to be a passed flag
                    VW_FadeOut ();
            }
            break;

        case AN_END_OF_ANIM:
            ExitMovie = true;
            break;

        default:
            Quit ("Unrecognized anim code!");
            break;
    }

    BufferPtr += blk.recsize;
}


/*
====================
=
= MOVIE_Play
=
= RETURNS: true  - Movie file was found and played
=          false - Movie file was NOT found!
=
====================
*/

bool MOVIE_Play (MovieStuff_t *MovieStuff)
{
    char fname[13];

    snprintf (fname,sizeof(fname),"%s%s",MovieStuff->fname,extension);

    moviefile = fopen(fname,"rb");

    SetupMovie (MovieStuff);

    //
    // start the anim process
    //
    if (moviefile)
    {
        movietimecount = GetTimeCount();

        while (!ExitMovie)
            MOVIE_HandlePage (MovieStuff);
    }

    ShutdownMovie ();

    return true;
}
