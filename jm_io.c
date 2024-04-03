// JM_IO.C

#include "3d_def.h"


/*
===================
=
= IO_WriteFile
=
===================
*/

void IO_WriteFile (char *filename, void *source, int32_t length)
{
    FILE *file;

    file = fopen(filename,"wb");

    if (!file)
        CA_CannotOpen (filename);

    if (!fwrite(source,length,1,file))
        Quit ("Error writing file %s: %s",filename,strerror(errno));

    fclose (file);
}


/*
===================
=
= IO_LoadFile
=
= Don't forget to free *dest!
=
===================
*/

int32_t IO_LoadFile (char *filename, void **dest)
{
    FILE       *file;
    const char *id = "JAMP";
    char       *buffer;
    int        len;
    int32_t    size;

    file = fopen(filename,"rb");

    if (!file)
        CA_CannotOpen (filename);

    len = strlen(id) + 1;
    buffer = SafeMalloc(len);

    fread (buffer,len - 1,1,file);

    if (!strcmp(buffer,id))
        Quit ("JAMPAK compression not supported!");
    else
    {
        size = CA_GetFileLength(file);
        *dest = SafeMalloc(size);

        if (!fread(*dest,size,1,file))
            Quit ("Error reading file %s: %s",filename,strerror(errno));
    }

    fclose (file);

    free (buffer);

    return size;
}


/*
===================
=
= IO_CopyFile
=
===================
*/

void IO_CopyFile (FILE *sourcefile, FILE *destfile, int32_t numbytes)
{
    void   *src;
    size_t fsize;

    if (numbytes == -1)
        fsize = CA_GetFileLength(sourcefile);
    else
        fsize = numbytes;

    src = SafeMalloc(fsize);

    if (!fread(src,fsize,1,sourcefile))
        Quit ("Error reading source file: %s",strerror(errno));

    if (!fwrite(src,fsize,1,destfile))
        Quit ("Error writing destination file: %s",strerror(errno));

    free (src);
}
