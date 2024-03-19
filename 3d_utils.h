// 3D_UTILS.H

#ifndef _3D_UTILS_H_
#define _3D_UTILS_H_


#define  SafeMalloc(s)   safe_malloc ((s),__FILE__,__LINE__)


fixed    FixedMul (fixed a, fixed b);
fixed    FixedDiv (fixed a, fixed b);

void     *safe_malloc (size_t size, const char *file, uint32_t line);

void     Error (const char *string);
void     Help (const char *string);

uint16_t ReadShort (void *ptr);
uint32_t ReadLong (void *ptr);

#endif
