// 3D_UTILS.C

#include "3d_def.h"


/*
===================
=
= FixedMul
=
===================
*/

fixed FixedMul (fixed a, fixed b)
{
    return (fixed)(((int64_t)a * (int64_t)b) >> FRACBITS);
}


/*
===================
=
= FixedDiv
=
===================
*/

fixed FixedDiv (fixed a, fixed b)
{
    return (fixed)(((int64_t)a << FRACBITS) / (int64_t)b);
}


/*
===================
=
= Wrapper for malloc with a NULL check
=
===================
*/

void *safe_malloc (size_t size, const char *file, uint32_t line)
{
    void *ptr;

    ptr = malloc(size);

    if (!ptr)
    {
        snprintf (str,sizeof(str),"%s",file);
        Quit ("Error allocating memory: %s\nFile: %s Line: %u",strerror(errno),basename(str),line);
    }

    return ptr;
}


/*
===================
=
= Random
=
===================
*/

int Random (int max)
{
    int value;

    if (max)
    {
        if (max > 255)
            value = (US_RndT() << 8) + US_RndT();
        else
            value = US_RndT();

        value %= max;
    }
    else
        value = 0;

    return value;
}


/*
===================
=
= Error
=
===================
*/

void Error (const char *string)
{
    SDL_ShowSimpleMessageBox (SDL_MESSAGEBOX_ERROR,screen.title,string,screen.window);
}


/*
===================
=
= Help
=
===================
*/

void Help (const char *string)
{
    SDL_ShowSimpleMessageBox (SDL_MESSAGEBOX_INFORMATION,screen.title,string,screen.window);
}


/*
===================
=
= ReadShort
=
===================
*/

uint16_t ReadShort (void *ptr)
{
    byte     *data;
    uint16_t value;

    data = ptr;

    value = data[0] | (data[1] << 8);

    return value;
}


/*
===================
=
= ReadLong
=
===================
*/

uint32_t ReadLong (void *ptr)
{
    byte     *data;
    uint32_t value;

    data = ptr;

    value = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);

    return value;
}
