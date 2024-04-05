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
        snprintf (str,sizeof(str),file);
        Terminate (NULL,"Error allocating memory: %s\nFile: %s Line: %u",strerror(errno),basename(str),line);
    }

    return ptr;
}


/*
===================
=
= CustomAssert
=
===================
*/

void CustomAssert (bool cond, const char *file, uint32_t line, const char *string)
{
    if (!cond)
    {
        snprintf (str,sizeof(str),file);
        snprintf (error,sizeof(error),"Assertion failed: %s\nFile: %s Line: %u",string,basename(str),line);
        Terminate (NULL,error);
    }
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

unsigned ReadShort (const void *ptr)
{
    const byte *data;
    unsigned   value;

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

uint32_t ReadLong (const void *ptr)
{
    const byte *data;
    uint32_t   value;

    data = ptr;

    value = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);

    return value;
}
