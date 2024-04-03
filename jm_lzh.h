// JM_LZH.H

#ifndef _JM_LZH_H_
#define _JM_LZH_H_


bool    LZH_Startup (void);
void    LZH_Shutdown (void);
int32_t LZH_Compress (void *infile, void *outfile, uint32_t DataLength);
int32_t LZH_Decompress (void *infile, void *outfile, uint32_t OrginalLength, uint32_t CompressLength);

#endif
