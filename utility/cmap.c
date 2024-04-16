//
// Convert a Blake Stone COLORMAP.BIN file to
// a C array
//
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "string.h"

int main (void)
{
    #define EXPECTED_FILE_SIZE    (256 * 66)

    FILE       *srcfile,*destfile;
    uint8_t    *buffer;
    size_t     length;
    int        count,num;
    const char *infilename = "COLORMAP.BIN";
    const char *outfilename = "colormap.c";

    srcfile = fopen(infilename,"rb");

    if (!srcfile)
    {
        fprintf (stderr,"Unable to open %s!",infilename);
        exit (1);
    }

    fseek (srcfile,0,SEEK_END);
    length = ftell(srcfile);

    if (length != EXPECTED_FILE_SIZE)
    {
        fprintf (stderr,"File %s is the wrong size (%zu instead of %d)",infilename,length,EXPECTED_FILE_SIZE);
        exit (1);
    }

    fseek (srcfile,0,SEEK_SET);

    buffer = malloc(length);

    if (!buffer)
    {
        fprintf (stderr,"Error allocating %zu bytes!",length);
        exit (1);
    }

    if (!fread(buffer,length,1,srcfile))
    {
        fprintf (stderr,"Error reading file %s!",infilename);
        exit (1);
    }

    fclose (srcfile);

    destfile = fopen(outfilename,"w");

    if (!destfile)
    {
        fprintf (stderr,"Unable to open %s!",outfilename);
        exit (1);
    }

    fprintf (destfile,"// COLORMAP.C\n");
    fprintf (destfile,"\n#include \"3d_def.h\"\n");
    fprintf (destfile,"\nbyte colormap[%zu] =",length);
    fprintf (destfile,"\n{");

    for (count = 0; count < length; count++)
    {
        num = buffer[count];

        if (!(count % 16))
            fprintf (destfile,"\n    0x%02X, ",num);
        else if ((count % 16) == 15)
            fprintf (destfile,"0x%02X,",num);
        else
            fprintf (destfile,"0x%02X, ",num);
    }

    fprintf (destfile,"\n};\n");

    fclose (destfile);
    free (buffer);

    printf ("File %s written successfully!",outfilename);

    return 0;
}
