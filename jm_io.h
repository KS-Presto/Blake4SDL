// JM_IO.H

#ifndef _JM_IO_H_
#define _JM_IO_H_


void    IO_ReadFile (char *filename, void **dest);
void    IO_WriteFile (char *filename, void *source, int32_t length);
int32_t IO_LoadFile (char *filename, void **dest);
void    IO_CopyHandle (FILE *sourcefile, FILE *destfile, int32_t numbytes);

#endif
