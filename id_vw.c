// ID_VW.C

#include "3d_def.h"


//
// Uncomment the following line if you get destination out of bounds
// assertion errors and want to ignore them during debugging
//

//#define IGNORE_BAD_DEST

#ifdef IGNORE_BAD_DEST
#undef Assert
#define Assert(x,s)     if (!(x)) return
#endif

#define RGB(r,g,b)    { ((r) * 255) / 63, ((g) * 255) / 63, ((b) * 255) / 63, SDL_ALPHA_OPAQUE }

screen_t     screen;
pictabletype *pictable;

uint32_t     *ylookup;

int          px,py;
int          fontcolor,backcolor;
int          fontnumber;

SDL_Color    palette1[256],palette2[256];
SDL_Color    curpal[256];
SDL_Color    gamepal[256] =
{
    #include "blakepal.inc"
};


//
// fizzle fade stuff
//
static byte     rndbits;
static uint32_t rndmask;

//
// XOR masks for the pseudo-random number sequence starting with n = 17 bits
//
static const uint32_t rndmasks[] =
{
                    // n    XNOR from (starting at 1, not 0 as usual)
    0x00012000,     // 17   17,14
    0x00020400,     // 18   18,11
    0x00040023,     // 19   19,6,2,1
    0x00090000,     // 20   20,17
    0x00140000,     // 21   21,19
    0x00300000,     // 22   22,21
    0x00420000,     // 23   23,18
    0x00e10000,     // 24   24,23,22,17
    0x01200000,     // 25   25,22     (this is enough for 8191x4095)
};


/*
=======================
=
= VW_Startup
=
=======================
*/

void VW_Startup (void)
{
    int      x,y;
    int      w,h;
    uint32_t flags = 0;

    if (screen.flags & SC_FULLSCREEN)
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    else
    {
        flags |= SDL_WINDOW_RESIZABLE;

        if (screen.flags & SC_INPUTGRABBED)
            flags |= SDL_WINDOW_INPUT_GRABBED;
    }

    x = SDL_WINDOWPOS_CENTERED;
    y = SDL_WINDOWPOS_CENTERED;
    w = screen.width;
    h = screen.height;

    screen.title = "Blake4SDL";

    screen.window = SDL_CreateWindow(screen.title,x,y,w,h,flags);

    if (!screen.window)
        Quit ("Unable to create window: %s\n",SDL_GetError());

    VW_SetupVideo ();
    VW_InitRndMask ();
}


/*
===================
=
= VW_ClearTexture
=
= Deallocate the rendering texture and its
= associated surfaces
=
===================
*/

void VW_ClearTexture (void)
{
    SDL_DestroyTexture (screen.texture);
    screen.texture = NULL;

    SDL_FreeSurface (screen.surface);
    screen.surface = NULL;

    SDL_FreeSurface (screen.buffer);
    screen.buffer = NULL;

    free (ylookup);
    ylookup = NULL;
}


/*
=======================
=
= VW_Shutdown
=
= Deallocate everything
=
=======================
*/

void VW_Shutdown (void)
{
    VW_ClearTexture ();

    SDL_DestroyRenderer (screen.renderer);
    screen.renderer = NULL;

    SDL_DestroyWindow (screen.window);
    screen.window = NULL;
}


/*
=======================
=
= VW_SetupVideo
=
=======================
*/

void VW_SetupVideo (void)
{
    int      i;
    int      w,h;
    uint32_t a,r,g,b;
    uint32_t flags = 0;

    w = screen.width;
    h = screen.height;

    if (!screen.renderer)
    {
        if (!(screen.flags & SC_HWACCEL))
            flags |= SDL_RENDERER_SOFTWARE;
        else
        {
            flags |= SDL_RENDERER_ACCELERATED;

            if (screen.flags & SC_VSYNC)
                flags |= SDL_RENDERER_PRESENTVSYNC;
        }

        screen.renderer = SDL_CreateRenderer(screen.window,-1,flags);

        if (!screen.renderer)
            Quit ("Unable to create renderer: %s\n",SDL_GetError());

        SDL_RenderSetLogicalSize (screen.renderer,w,h);
        SDL_RenderSetViewport (screen.renderer,NULL);
        SDL_RenderSetVSync (screen.renderer,(screen.flags & SC_VSYNC) != 0);
    }

    SDL_PixelFormatEnumToMasks (SDL_PIXELFORMAT_ARGB8888,&screen.bits,&r,&g,&b,&a);

    screen.surface = SDL_CreateRGBSurface(0,w,h,screen.bits,r,g,b,a);

    if (!screen.surface)
        Quit ("Unable to create %dx%dx%d surface: %s\n",w,h,screen.bits,SDL_GetError());

    screen.texture = SDL_CreateTexture(screen.renderer,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STATIC,w,h);

    if (!screen.texture)
        Quit ("Unable to create texture: %s\n",SDL_GetError());

    //
    // create 8 bit screen buffer for drawing
    //
    screen.buffer = SDL_CreateRGBSurface(0,w,h,8,0,0,0,0);

    if (!screen.buffer)
        Quit ("Unable to create screen buffer surface: %s\n",SDL_GetError());

    VW_SetPalette (gamepal);

    ylookup = SafeMalloc(screen.height * sizeof(*ylookup));

    for (i = 0; i < screen.height; i++)
        ylookup[i] = i * screen.buffer->pitch;

    screen.scale = w / 320;
    screen.basewidth = w / screen.scale;
    screen.baseheight = h / screen.scale;
    screen.heightoffset = (screen.baseheight % 200) / 2;

    VW_SetBufferOffset (screen.heightoffset);

    SDL_SetWindowMinimumSize (screen.window,screen.basewidth,screen.baseheight);
}


/*
===================
=
= VW_ChangeDisplay
=
===================
*/

void VW_ChangeDisplay (screen_t *scr)
{
    VW_ClearScreen (BLACK);
    VW_UpdateScreen (screen.buffer);

    VW_ChangeWindow (scr);

    if (scr->scale != screen.scale || scr->width != screen.width || scr->height != screen.height)
    {
        //
        // update screen variables and re-allocate everything
        //
        screen.width = scr->width;
        screen.height = scr->height;

        Shutdown3DRenderer ();
        VW_ClearTexture ();

        VW_SetupVideo ();
        VW_InitRndMask ();
        Init3DRenderer ();
    }
}


/*
===================
=
= VW_ChangeWindow
=
= Change the current window resolution and/or
= go to/from fullscreen
=
===================
*/

void VW_ChangeWindow (screen_t *scr)
{
    uint32_t        flags;
    SDL_DisplayMode dm;

    flags = SDL_GetWindowFlags(screen.window);

    if (screen.flags & SC_FULLSCREEN)
    {
        if (scr->scale != screen.scale || scr->width != screen.width || scr->height != screen.height)
        {
            if (SDL_GetWindowDisplayMode(screen.window,&dm))
                Quit ("Unable to get display mode: %s\n",SDL_GetError());

            dm.w = scr->width;
            dm.h = scr->height;

            if (SDL_SetWindowDisplayMode(screen.window,&dm))
                Quit ("Unable to set display mode: %s\n",SDL_GetError());
        }

        if (!(flags & SDL_WINDOW_FULLSCREEN))
        {
            if (SDL_SetWindowFullscreen(screen.window,SDL_WINDOW_FULLSCREEN_DESKTOP))
                Quit ("Unable to set fullscreen mode: %s\n",SDL_GetError());
        }
    }
    else
    {
        if (flags & SDL_WINDOW_FULLSCREEN)
        {
            if (SDL_SetWindowFullscreen(screen.window,0))
                Quit ("Unable to set windowed mode: %s\n",SDL_GetError());
        }

        //
        // KS: there's a weird bug here where switching back from 320x240
        // to 320x200 will not resize the window height - it stays at 240 and
        // scales up the screen. None of the other resolutions do this, but I
        // can't find out why it happens...
        //
        SDL_SetWindowSize (screen.window,scr->width,scr->height);
        SDL_SetWindowPosition (screen.window,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED);
    }

    SDL_RenderSetLogicalSize (screen.renderer,scr->width,scr->height);
    SDL_RenderSetViewport (screen.renderer,NULL);
}


/*
=============================================================================

                        PALETTE OPS

=============================================================================
*/


/*
=================
=
= VW_ConvertPalette
=
= Converts a VGA palette to an SDL_Color one
=
=================
*/

void VW_ConvertPalette (byte *srcpal, SDL_Color *destpal, int numcolors)
{
    int i;

    for (i = 0; i < numcolors; i++)
    {
        destpal[i].r = (*srcpal++ * 255) / 63;
        destpal[i].g = (*srcpal++ * 255) / 63;
        destpal[i].b = (*srcpal++ * 255) / 63;
        destpal[i].a = SDL_ALPHA_OPAQUE;
    }
}


/*
=================
=
= VW_FillPalette
=
=================
*/

void VW_FillPalette (int red, int green, int blue)
{
    int       i;
    SDL_Color palette[256];

    for (i = 0; i < 256; i++)
    {
        palette[i].r = red;
        palette[i].g = green;
        palette[i].b = blue;
        palette[i].a = SDL_ALPHA_OPAQUE;
    }

    VW_SetPalette (palette);
    VW_UpdateScreen (screen.buffer);
}


/*
=================
=
= VW_SetPalette
=
=================
*/

void VW_SetPalette (SDL_Color *palette)
{
    memcpy (curpal,palette,sizeof(*palette) * 256);

    SDL_SetPaletteColors (screen.buffer->format->palette,palette,0,256);
}


/*
=================
=
= VW_GetPalette
=
=================
*/

void VW_GetPalette (SDL_Color *palette)
{
    memcpy (palette,curpal,sizeof(*palette) * 256);
}


/*
=================
=
= VW_FadePaletteOut
=
= Fades the current palette to the given color in the given number of steps
=
=================
*/

void VW_FadePaletteOut (int start, int end, int red, int green, int blue, int steps)
{
    int       i,j,delta;
    SDL_Color *origptr,*newptr;

    if (screen.flags & SC_FADED)
        return;

    red = (red * 255) / 63;
    green = (green * 255) / 63;
    blue = (blue * 255) / 63;

    VW_GetPalette (palette1);
    memcpy (palette2,palette1,sizeof(*palette1) * 256);

    //
    // fade through intermediate frames
    //
    for (i = 0; i < steps; i++)
    {
        origptr = &palette1[start];
        newptr = &palette2[start];

        for (j = start; j <= end; j++)
        {
           delta = red - origptr->r;
           newptr->r = origptr->r + ((delta * i) / steps);
           delta = green - origptr->g;
           newptr->g = origptr->g + ((delta * i) / steps);
           delta = blue - origptr->b;
           newptr->b = origptr->b + ((delta * i) / steps);
           newptr->a = SDL_ALPHA_OPAQUE;
           origptr++;
           newptr++;
        }

        VW_SetPalette (palette2);
        VW_UpdateScreen (screen.buffer);
    }

    //
    // final color
    //
    VW_FillPalette (red,green,blue);

    screen.flags |= SC_FADED;
}


/*
=================
=
= VW_FadePaletteIn
=
=================
*/

void VW_FadePaletteIn (int start, int end, SDL_Color *palette, int steps)
{
    int       i,j,delta;
    SDL_Color *origptr,*newptr;

    if (!(screen.flags & SC_FADED))
        return;

    VW_GetPalette (palette1);
    memcpy (palette2,palette1,sizeof(*palette1) * 256);

    //
    // fade through intermediate frames
    //
    for (i = 0; i < steps; i++)
    {
        origptr = &palette1[start];
        newptr = &palette2[start];

        for (j = start; j <= end; j++)
        {
            delta = palette[j].r - origptr->r;
            newptr->r = origptr->r + ((delta * i) / steps);
            delta = palette[j].g - origptr->g;
            newptr->g = origptr->g + ((delta * i) / steps);
            delta = palette[j].b - origptr->b;
            newptr->b = origptr->b + ((delta * i) / steps);
            newptr->a = SDL_ALPHA_OPAQUE;
            origptr++;
            newptr++;
        }

        VW_SetPalette (palette2);
        VW_UpdateScreen (screen.buffer);
    }

    //
    // final color
    //
    VW_SetPalette (palette);
    VW_UpdateScreen (screen.buffer);

    screen.flags &= ~SC_FADED;
}


/*
=============================================================================

                            PIXEL OPS

=============================================================================
*/


/*
=================
=
= VW_LockSurface
=
=================
*/

void *VW_LockSurface (SDL_Surface *surface)
{
    if (SDL_MUSTLOCK(surface))
    {
        if (SDL_LockSurface(surface) < 0)
            return NULL;
    }

    return surface->pixels;
}


/*
=================
=
= VW_UnlockSurface
=
=================
*/

void VW_UnlockSurface (SDL_Surface *surface)
{
    if (SDL_MUSTLOCK(surface))
        SDL_UnlockSurface (surface);
}


/*
=================
=
= VW_SetBufferOffset
=
= Set the offset in the screen buffer to start drawing
=
= The offset MUST be 0 while in the 3D renderer!
=
=================
*/

void VW_SetBufferOffset (unsigned offset)
{
    offset *= screen.scale;

    Assert (offset < screen.height,"Invalid buffer offset!");

    screen.bufferofs = ylookup[offset];
}


/*
=================
=
= VW_Plot
=
=================
*/

void VW_Plot (int x, int y, int color)
{
    byte *dest;

    if (screen.scale > 1)
    {
        VW_Bar (x,y,1,1,color);
        return;
    }

    Assert (x >= 0 && x < screen.width
           && y >= 0 && y < screen.height,
           "Destination pixel out of bounds!");

    dest = VW_LockSurface(screen.buffer);

    if (!dest)
        Quit ("Unable to lock surface: %s\n",SDL_GetError());

    dest[screen.bufferofs + ylookup[y] + x] = color;

    VW_UnlockSurface (screen.buffer);
}


/*
=================
=
= VW_Bar
=
=================
*/

void VW_Bar (int x, int y, int width, int height, int color)
{
    byte *dest;

    if (screen.scale > 1)
    {
        x *= screen.scale;
        y *= screen.scale;
        width *= screen.scale;
        height *= screen.scale;
    }

    Assert (x >= 0 && x + width <= screen.width
           && y >= 0 && y + height <= screen.height
           && width > 0 && height > 0,"Destination rectangle out of bounds!");

    dest = VW_LockSurface(screen.buffer);

    if (!dest)
        Quit ("Unable to lock surface: %s\n",SDL_GetError());

    dest += screen.bufferofs + ylookup[y] + x;

    while (height--)
    {
        memset (dest,color,width);
        dest += screen.buffer->pitch;
    }

    VW_UnlockSurface (screen.buffer);
}


/*
============================================================================

                            MEMORY OPS

============================================================================
*/


/*
=================
=
= VW_DePlaneVGA
=
= Unweave a VGA graphic to simplify drawing
=
=================
*/

void VW_DePlaneVGA (byte *source, int width, int height)
{
    int  x,y,plane;
    int  size,pwidth;
    byte *temp,*dest,*srcline;

    size = width * height;

    if (width & 3)
        Quit ("Width not divisible by 4!");

    temp = SafeMalloc(size);

//
// munge pic into the temp buffer
//
    srcline = source;
    pwidth = width >> 2;

    for (plane = 0; plane < 4; plane++)
    {
        dest = temp;

        for (y = 0; y < height; y++)
        {
            for (x = 0; x < pwidth; x++)
                *(dest + (x << 2) + plane) = *srcline++;

            dest += width;
        }
    }

//
// copy the temp buffer back into the original source
//
    memcpy (source,temp,size);

    free (temp);
}


/*
=================
=
= VW_MemToScreen
=
= Draws a block of data to the screen
=
=================
*/

void VW_MemToScreen (byte *source, int width, int height, int x, int y)
{
    int   w,h;
    int   srcwidth;
    fixed frac,fracstep;
    byte  *basedest,*dest,*src;

    srcwidth = width;

    if (screen.scale > 1)
    {
        x *= screen.scale;
        y *= screen.scale;
        width *= screen.scale;
        height *= screen.scale;
    }

    Assert (x >= 0 && x + width <= screen.width
         && y >= 0 && y + height <= screen.height
         && width > 0 && height > 0,"Destination rectangle out of bounds!");

    basedest = VW_LockSurface(screen.buffer);

    if (!basedest)
        Quit ("Unable to lock surface: %s\n",SDL_GetError());

    basedest += screen.bufferofs + ylookup[y] + x;

    fracstep = 0xffffffff / (screen.scale << FRACBITS);

    for (h = 0; h < height; h++)
    {
        src = &source[(h / screen.scale) * srcwidth];
        dest = basedest;

        w = width;
        frac = fracstep;

        while (w--)
        {
            *dest++ = src[frac >> FRACBITS];
            frac += fracstep;
        }

        basedest += screen.buffer->pitch;
    }

    VW_UnlockSurface (screen.buffer);
}


/*
=================
=
= VW_MaskMemToScreen
=
= Draws a masked block of data to the screen
=
=================
*/

void VW_MaskMemToScreen (byte *source, int width, int height, int x, int y, int mask)
{
    int   w,h;
    int   srcwidth,color;
    fixed frac,fracstep;
    byte  *basedest,*dest,*src;

    srcwidth = width;

    if (screen.scale > 1)
    {
        x *= screen.scale;
        y *= screen.scale;
        width *= screen.scale;
        height *= screen.scale;
    }

    Assert (x >= 0 && x + width <= screen.width
         && y >= 0 && y + height <= screen.height
         && width > 0 && height > 0,"Destination rectangle out of bounds!");

    basedest = VW_LockSurface(screen.buffer);

    if (!basedest)
        Quit ("Unable to lock surface: %s\n",SDL_GetError());

    basedest += screen.bufferofs + ylookup[y] + x;

    fracstep = 0xffffffff / (screen.scale << FRACBITS);

    for (h = 0; h < height; h++)
    {
        src = &source[(h / screen.scale) * srcwidth];
        dest = basedest;

        w = width;
        frac = fracstep;

        while (w--)
        {
            color = src[frac >> FRACBITS];

            if (color != mask)
                *dest = color;

            dest++;
            frac += fracstep;
        }

        basedest += screen.buffer->pitch;
    }

    VW_UnlockSurface (screen.buffer);
}


/*
=================
=
= VW_ScreenToMem
=
= Copies a block of screen pixels to a buffer
=
=================
*/

void VW_ScreenToMem (byte *dest, int width, int height, int x, int y)
{
    byte *source;

    if (screen.scale > 1)
    {
        x *= screen.scale;
        y *= screen.scale;
        width *= screen.scale;
        height *= screen.scale;
    }

    Assert (x >= 0 && x + width <= screen.width
         && y >= 0 && y + height <= screen.height
         && width > 0 && height > 0,"Destination rectangle out of bounds!");

    source = VW_LockSurface(screen.buffer);

    if (!source)
        Quit ("Unable to lock surface: %s\n",SDL_GetError());

    source += screen.bufferofs + ylookup[y] + x;

    while (height--)
    {
        memcpy (dest,source,width);

        source += screen.buffer->pitch;
        dest += width;
    }

    VW_UnlockSurface (screen.buffer);
}


/*
=================
=
= VW_ScreenToScreen
=
= Moves a block of screen pixels in the screen buffer
=
= Only vertical offsets currently supported
=
= KS: I'm unable to find a definite confirmation that
= SDL_BlitSurface can safely work with overlapping regions,
= but it seems to work alright.
=
=================
*/

void VW_ScreenToScreen (int width, int height, int x, int y, int srcofs, int destofs)
{
    int heightofs = screen.heightoffset;

    if (screen.scale > 1)
    {
        x *= screen.scale;
        y *= screen.scale;
        srcofs *= screen.scale;
        destofs *= screen.scale;
        width *= screen.scale;
        height *= screen.scale;
        heightofs *= screen.scale;
    }

    Assert (x >= 0 && x + width <= screen.width
         && y + destofs >= 0 && y + destofs + height <= screen.height
         && y + srcofs >= 0 && y + srcofs + height <= screen.height
         && width > 0 && height > 0,"Destination rectangle out of bounds!");

    SDL_Rect srcrect = {x,y + srcofs + heightofs,width,height};
    SDL_Rect destrect = {x,y + destofs + heightofs,width,height};

    SDL_BlitSurface (screen.buffer,&srcrect,screen.buffer,&destrect);
}


/*
=================
=
= VW_DrawString
=
= Only the starting bounds are checked here, so be
= sure that the caller handles the string going off
= the right edge of the screen!
=
=================
*/

void VW_DrawString (const char *string)
{
    fontstruct *font;
    int        x,y;
    int        h,step;
    int        width,height;
    byte       *source,*basedest,*dest;
    byte       ch;

    font = (fontstruct *)grsegs[STARTFONT + fontnumber];

    x = px;
    y = py;
    height = font->height;

    if (screen.scale > 1)
    {
        x *= screen.scale;
        y *= screen.scale;
        height *= screen.scale;
    }

    Assert (x >= 0 && x < screen.width
           && y >= 0 && y + height <= screen.height && height > 0,
           "Destination rectangle out of bounds!");

    basedest = VW_LockSurface(screen.buffer);

    if (!basedest)
        Quit ("Unable to lock surface: %s\n",SDL_GetError());

    basedest += screen.bufferofs + ylookup[y];

    for (ch = (byte)*string; ch; ch = (byte)*string)
    {
        source = (byte *)font + font->location[ch];

        width = step = font->width[ch];
        px += width;

        while (width--)
        {
            dest = basedest + x;

            for (h = 0; h < height; h++)
            {
                if (source[(h / screen.scale) * step])
                    memset (dest,fontcolor,screen.scale);

                dest += screen.buffer->pitch;
            }

            source++;
            x += screen.scale;
        }

        string++;
    }

    VW_UnlockSurface (screen.buffer);
}


/*
=================
=
= VW_MeasureString
=
= Get the proportional width of the string
=
=================
*/

void VW_MeasureString (const char *string, word *width, word *height)
{
    fontstruct *font;

    font = (fontstruct *)grsegs[STARTFONT + fontnumber];

    *height = font->height;
    *width = 0;

    while (*string)
        *width += font->width[(byte)*string++];
}


/*
=============================================================================

				DOUBLE BUFFER MANAGEMENT ROUTINES

=============================================================================
*/


/*
=================
=
= VW_UpdateScreen
=
=================
*/

void VW_UpdateScreen (SDL_Surface *surface)
{
    SDL_BlitSurface (surface,NULL,screen.surface,NULL);

    SDL_UpdateTexture (screen.texture,NULL,screen.surface->pixels,screen.surface->pitch);
    SDL_RenderCopy (screen.renderer,screen.texture,NULL,NULL);
    SDL_RenderPresent (screen.renderer);
}


/*
=================
=
= VW_DrawTile8
=
=================
*/

void VW_DrawTile8 (int x, int y, int tile)
{
    VW_MemToScreen (grsegs[STARTTILE8] + (tile << 6),8,8,x,y);
}


/*
=================
=
= VW_DrawPic
=
=================
*/

void VW_DrawPic (int x, int y, int chunk)
{
    int width = pictable[chunk - STARTPICS].width;
    int height = pictable[chunk - STARTPICS].height;

    VW_MemToScreen (grsegs[chunk],width,height,x,y);
}


/*
=================
=
= VW_DrawMPic
=
=================
*/

void VW_DrawMPic (int x, int y, int chunk)
{
    int width = pictable[chunk - STARTPICS].width;
    int height = pictable[chunk - STARTPICS].height;

    VW_MaskMemToScreen (grsegs[chunk],width,height,x,y,255);
}


/*
=============================================================================

						FIZZLE FADE STUFF

=============================================================================
*/


/*
===================
=
= VW_InitRndMask
=
= Init rndmask for VW_FizzleFade
=
===================
*/

void VW_InitRndMask (void)
{
    int i;
    int bitsx,bitsy;

    for (i = 1, bitsx = 0; i < screen.width; i <<= 1)
        bitsx++;

    for (i = 1, bitsy = 0; i < screen.height; i <<= 1)
        bitsy++;

    i = bitsx + bitsy;

    if (i < 17)
        i = 17;         // no problem, just a bit slower
    else if (i > 25)
        i = 25;         // will not fill entire screen

    rndmask = rndmasks[i - 17];

    rndbits = bitsy;    // use the lower bit count
}


/*
===================
=
= VW_FizzleFade
=
= Uses maximum-length Linear Feedback Shift Register (LFSR) counters.
= You can find a list of them with lengths from 3 to 168 at:
= http://www.xilinx.com/support/documentation/application_notes/xapp052.pdf
= Many thanks to Xilinx for this list!!!
=
===================
*/

bool VW_FizzleFade (int x1, int y1, int width, int height, int frames, bool abortable)
{
    int             x,y,p;
    int             px,py;
    int             bpp,color;
    int32_t         rndval;
    uint32_t        frame,pixperframe,fullcolor;
    byte            *src,*dest;
    SDL_PixelFormat *format;
    SDL_Color       *pal;

    if (!frames)
        Quit ("0 frames!");

    if (screen.scale > 1)
    {
        x1 *= screen.scale;
        y1 *= screen.scale;
        width *= screen.scale;
        height *= screen.scale;
    }

    Assert (x1 >= 0 && x1 + width <= screen.width
         && y1 >= 0 && y1 + height <= screen.height
         && width > 0 && height > 0,"Destination rectangle out of bounds!");

    rndval = 1;
    pixperframe = (width * height) / frames;

    IN_StartAck ();

    frame = GetTimeCount();

    src = VW_LockSurface(screen.buffer);

    if (!src)
        Quit ("Unable to lock source surface: %s\n",SDL_GetError());

    format = screen.surface->format;
    bpp = format->BytesPerPixel;

    while (1)
    {
        IN_ProcessEvents ();

        if (abortable && IN_CheckAck())
        {
            VW_UnlockSurface (screen.buffer);
            VW_UpdateScreen (screen.buffer);

            return true;
        }

        dest = VW_LockSurface(screen.surface);

        if (!dest)
            Quit ("Unable to lock dest surface: %s\n",SDL_GetError());

        for (p = 0; p < pixperframe; p++)
        {
            //
            // seperate random value into x/y pair
            //
            x = rndval >> rndbits;
            y = rndval & ((1 << rndbits) - 1);

            //
            // advance to next random element
            //
            if (rndval & 1)
                rndval = (rndval >> 1) ^ rndmask;
            else
                rndval >>= 1;

            if (x >= width || y >= height)
                p--;                         // not into the view area; get a new pair
            else
            {
                //
                // copy one pixel
                //
                px = x1 + x;
                py = y1 + y;
                color = *(src + ylookup[py] + px);

                if (screen.bits == 8)
                    *(dest + (py * screen.surface->pitch) + px) = color;
                else
                {
                    pal = &curpal[color];

                    fullcolor = SDL_MapRGBA(format,pal->r,pal->g,pal->b,SDL_ALPHA_OPAQUE);

                    memcpy (&dest[(py * screen.surface->pitch) + (px * bpp)],&fullcolor,bpp);
                }
            }

            if (rndval == 1)
            {
                //
                // entire sequence has been completed
                //
                VW_UnlockSurface (screen.buffer);
                VW_UnlockSurface (screen.surface);
                VW_UpdateScreen (screen.buffer);

                return false;
            }
        }

        VW_UnlockSurface (screen.surface);

        SDL_UpdateTexture (screen.texture,NULL,screen.surface->pixels,screen.surface->pitch);
        SDL_RenderCopy (screen.renderer,screen.texture,NULL,NULL);
        SDL_RenderPresent (screen.renderer);

        frame++;
        Delay (frame - GetTimeCount());        // don't go too fast
    }

    return false;
}
