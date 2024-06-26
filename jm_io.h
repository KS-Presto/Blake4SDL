// JM_IO.H

#ifndef _JM_IO_H_
#define _JM_IO_H_


void    IO_WriteFile (const char *filename, void *source, int32_t length);
int32_t IO_LoadFile (const char *filename, void **dest);
void    IO_CopyFile (FILE *sourcefile, FILE *destfile, int32_t numbytes);

#endif
