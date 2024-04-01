// ID_VW.H

#ifndef _ID_VW_H_
#define _ID_VW_H_


#define WHITE               15      // graphics mode independant colors
#define BLACK               0

#define VW_WaitVBL(v)       SDL_Delay ((v) << 3)
#define VW_ClearScreen(c)   SDL_FillRect (screen.buffer,NULL,(c))
#define VW_FadeOut()        VW_FadePaletteOut (0,255,0,0,0,30)
#define VW_FadeIn()         VW_FadePaletteIn (0,255,gamepal,30)
#define VW_Hlin(x1,x2,y,c)  VW_Bar ((x1),(y),(x2) - (x1) + 1,1,(c))
#define VW_Vlin(y1,y2,x,c)  VW_Bar ((x),(y1),1,(y2) - (y1) + 1,(c))


typedef struct screenstruct
{
    int16_t      width,height;
    int          bits;
    uint32_t     flags;
    const char   *title;

    SDL_Window   *window;
    SDL_Renderer *renderer;
    SDL_Texture  *texture;
    SDL_Surface  *surface,*buffer;
} screen_t;


enum screenflags
{
    SC_FULLSCREEN    = 0x0001,
    SC_FADED         = 0x0002,
    SC_FIZZLEIN      = 0x0004,
    SC_INPUTGRABBED  = 0x0008,
    SC_HWACCEL       = 0x0010,
    SC_VSYNC         = 0x0020,
};


typedef struct
{
    int16_t     width;
    int16_t     height;
} pictabletype;


typedef struct
{
    int16_t     height;
    int16_t     location[256];
    int8_t      width[256];
} fontstruct;


extern  screen_t     screen;

extern  pictabletype *pictable;

extern  uint32_t     *ylookup;

extern  int          fontcolor,backcolor;
extern  int          fontnumber;
extern  int16_t      px,py;

extern  SDL_Color    gamepal[256];


void    VW_Startup (void);
void    VW_Shutdown (void);
void    VW_SetupVideo (void);
void    VW_InitRndMask (void);
void    VW_ConvertPalette (byte *srcpal, SDL_Color *destpal, int numcolors);
void    VW_SetPalette (SDL_Color *palette);
void    VW_FadePaletteOut (int start, int end, int red, int green, int blue, int steps);
void    VW_FadePaletteIn (int start, int end, SDL_Color *palette, int steps);
void    VW_UpdateScreen (SDL_Surface *surface);
void    *VW_LockSurface (SDL_Surface *surface);
void    VW_UnlockSurface (SDL_Surface *surface);
void    VW_DrawTile8 (int x, int y, int tile);
void    VW_DrawPic (int x, int y, int chunk);
void    VW_DrawMPic (int x, int y, int chunk);
void    VW_DePlaneVGA (byte *source, int width, int height);
void    VW_MemToScreen (byte *source, int width, int height, int x, int y);
void    VW_ScreenToMem (byte *dest, int width, int height, int x, int y);
void    VW_Bar (int x, int y, int width, int height, int color);
void    VW_MeasureString (const char *string, word *width, word *height);
void    VW_DrawString (const char *string);
void    VW_Plot (int x, int y, int color);
bool    VW_FizzleFade (int xl, int yl, int width, int height, int frames, bool abortable);

#endif
