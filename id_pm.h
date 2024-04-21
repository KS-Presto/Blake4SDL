// ID_PM.H

#ifndef _ID_PM_H_
#define _ID_PM_H_

#define PMPageSize             (TEXTURESIZE * TEXTURESIZE)

#define PM_GetSpritePage(v)    PM_GetPage (PMSpriteStart + (v))
#define PM_GetSoundPage(v)     PM_GetPage (PMSoundStart + (v))

extern  word ChunksInFile;
extern  word PMSpriteStart;
extern  word PMSoundStart;

extern  bool PMSoundInfoPagePadded;

extern  word *pageLengths;
extern  byte **PMPages;

extern  char pagefilename[13];
extern  char altpagefilename[13];


void     PM_Startup (void);
void     PM_Shutdown (void);
uint32_t PM_GetPageSize (int page);
byte     *PM_GetPage (int page);
byte     *PM_GetPageEnd (void);

#endif
