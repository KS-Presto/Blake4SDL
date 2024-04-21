// PLATUTIL.C
//
// Platform specific macros, functions, etc.
//

#include "3d_def.h"


#ifdef _WIN32
    #define MakeDir(s)    _mkdir(s)
#else
    #define MakeDir(s)    mkdir(s,0755)
#endif

/*
===================
=
= GetMaxPathLen
=
===================
*/

int GetMaxPathLen (void)
{
    int len;
#ifdef _WIN32
    len = PATH_MAX;
#else
    errno = 0;

    len = pathconf("/",_PC_PATH_MAX);

    if (len == -1)
    {
        if (errno)
            Quit ("Error querying PATH_MAX: %s",strerror(errno));

        len = 4096;    // fallback to a reasonable default
    }
#endif
    return len;
}


/*
===================
=
= InitConfigPath
=
= On Linux like systems, the config path defaults to $HOME/gamedir
=
===================
*/

void InitConfigPath (void)
{
    size_t     len;
    const char *basedir;
#ifndef _WIN32
    const char *gamedir = "/.blake4sdl";
#endif
    if (!configpath)
    {
        //
        // see if the Apogee CD path exists
        //
        // KS: I don't know if this code works or if
        // it's even useful now, and I'm unable to test it
        //
        // The user can probably just set a custom path to
        // the CD if desired
        //
        basedir = getenv("APOGEECD");

        if (basedir)
        {
            len = strlen(basedir) + sizeof(configname);

            if (len > GetMaxPathLen())
                Quit ("APOGEECD path too long!");

            configpath = SafeMalloc(len);

            snprintf (configpath,len,"%s",basedir);

            if (configpath[len - 1] == DIR_SEPARATOR)
                configpath[len - 1] = '\0';

            if (MakeDir(configpath) && errno != EEXIST)
                Quit ("APOGEECD directory not found!");

            configpath[len - 1] = DIR_SEPARATOR;
        }
        else
        {
#ifdef _WIN32
            //
            // set config location to game directory
            //
            configpath = SafeMalloc(sizeof(configname));

            *configpath = '\0';
#else
            //
            // set config location to home directory for multi-user support
            //
            basedir = getenv("HOME");

            if (!basedir)
                Quit ("Your $HOME directory is not defined. You must set this before playing.");

            len = strlen(basedir) + strlen(gamedir) + sizeof(configname);

            if (len > GetMaxPathLen())
                Quit ("Your $HOME directory path is too long. It cannot be used for saving games.");

            configpath = SafeMalloc(len);

            snprintf (configpath,len,"%s%s",basedir,gamedir);
#endif
        }
    }

    if (configpath && *configpath)
    {
        configpathlen = strlen(configpath);    // save pos so we can append strings

        //
        // ensure config directory exists, creating it if necessary
        //
        if (MakeDir(configpath) && errno != EEXIST)
            Quit ("Unable to create config directory: %s",strerror(errno));
    }
}

#ifndef _WIN32
/*
===================
=
= ltoa
=
===================
*/

char *ltoa (intmax_t value, char *string, int radix)
{
    char digits[] = "0123456789abcdef";
    char temp;
    int  i = 0,j = 0;

    switch (radix)
    {
        case 2:
        case 8:
        case 10:
        case 16:
            break;

        default:
            *string = '\0';

            return NULL;    // invalid radix
    }

    if (value < 0)
    {
        if (radix == 10)
            j = 1;

        value = -value;
    }

    do
    {
        string[i++] = digits[value % radix];
        value /= radix;

    } while (value);

    if (j)
        string[i++] = '-';

    string[i] = '\0';

    for (j = 0; j < i / 2; j++)
    {
        temp = string[j];
        string[j] = string[i - 1 - j];
        string[i - 1 - j] = temp;
    }

    return string;
}

#endif
