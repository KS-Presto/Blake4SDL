// PLATUTIL.H

#ifndef _PLATUTIL_H_
#define _PLATUTIL_H_


#define DIR_SEPARATOR    '/'


int  GetMaxPathLen (void);
void InitConfigPath (void);

#ifndef _WIN32

#define itoa(v,s,r)    ltoa ((int)(v),(s),(r))

char *ltoa (intmax_t value, char *string, int radix);

#endif

#endif
