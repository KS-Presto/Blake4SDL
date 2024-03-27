// MOVIE.H

#ifndef _MOVIE_H_
#define _MOVIE_H_


enum movietype
{
    mv_intro,
    mv_final,

    mv_NUM_MOVIES,
};


typedef struct
{
    uint16_t code;
    int32_t  blocknum;
    int32_t  recsize;
} anim_frame;


typedef struct
{
    uint16_t command;
    uint16_t offset;
    uint16_t length;
} anim_chunk;


typedef struct
{
    char      fname[13];
    int8_t    ticdelay;
    SDL_Color *palette;
} MovieStuff_t;


extern  MovieStuff_t Movies[mv_NUM_MOVIES];


void    MOVIE_ShowFrame (byte *inpic);
bool    MOVIE_Play (MovieStuff_t *MovieStuff);
void    SetupMovie (MovieStuff_t *MovieStuff);
void    ShutdownMovie (void);

#endif
