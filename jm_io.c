// JM_IO.C

#include "3d_def.h"

#ifdef NOTYET
#include "jm_cio.h"
#include "jm_lzh.h"
#endif

#ifdef DEMOS_EXTERN

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

#endif

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
#ifdef NOTYET
    JAMPHeader head;
#endif
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
    {
#ifdef NOTYET
        fread (&head,sizeof(head),1,file);
        size = head.OriginalLen;

        switch (head.CompType)
        {
            case ct_LZH:

                LZH_Startup ();

                *dest = SafeMalloc(head.OriginalLen);

                LZH_Decompress ((void far *)handle,*dest,size,head.CompressLen,SRC_FILE | DEST_MEM);
                LZH_Shutdown();
                break;

            case ct_LZW:
                Quit ("No code for LZW compression!");
                break;

            default:
                Quit ("Unknown compression type!");
                break;
        }
#else
        Quit ("Compression not yet supported!");
#endif
    }
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
