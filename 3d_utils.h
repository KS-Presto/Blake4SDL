// 3D_UTILS.H

#ifndef _3D_UTILS_H_
#define _3D_UTILS_H_


#define  SafeMalloc(s)   safe_malloc ((s),__FILE__,__LINE__)
#define  Assert(c,s)     CustomAssert ((c),__FILE__,__LINE__,(s))

fixed    FixedMul (fixed a, fixed b);
fixed    FixedDiv (fixed a, fixed b);

void     *safe_malloc (size_t size, const char *file, uint32_t line);
void     CustomAssert (bool cond, const char *file, uint32_t line, const char *string);

int      Random (int max);

void     Error (const char *string);
void     Help (const char *string);

unsigned ReadShort (const void *ptr);
uint32_t ReadLong (const void *ptr);

#endif
