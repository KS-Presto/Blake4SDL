//
// ID Engine
// ID_US.c - User Manager - General routines
// v1.1d1
// By Jason Blochowiak
//

#include "3d_def.h"


int            PrintX,PrintY;
int            WindowX,WindowY,WindowW,WindowH;

CustomCursor_t CustomCursor;
bool           usecustomcursor;

HighScore  Scores[MaxScores] =
{
    {"JAM PRODUCTIONS INC.",10000,1,0},
    {"",10000,1,0},
    {"JERRY JONES",10000,1,0},
    {"MICHAEL MAYNARD",10000,1,0},
    {"JAMES T. ROW",10000,1,0},
    {"",10000,1,0},
    {"",10000,1,0},
    {"TO REGISTER CALL",10000,1,0},
    {" 1-800-GAME123",10000,1,0},
    {"",10000,1,0},
};


int  rndindex;

byte rndtable[256] =
{
      0,   8, 109, 220, 222, 241, 149, 107,  75, 248, 254, 140,  16,  66,
     74,  21, 211,  47,  80, 242, 154,  27, 205, 128, 161,  89,  77,  36,
     95, 110,  85,  48, 212, 140, 211, 249,  22,  79, 200,  50,  28, 188,
     52, 140, 202, 120,  68, 145,  62,  70, 184, 190,  91, 197, 152, 224,
    149, 104,  25, 178, 252, 182, 202, 182, 141, 197,   4,  81, 181, 242,
    145,  42,  39, 227, 156, 198, 225, 193, 219,  93, 122, 175, 249,   0,
    175, 143,  70, 239,  46, 246, 163,  53, 163, 109, 168, 135,   2, 235,
     25,  92,  20, 145, 138,  77,  69, 166,  78, 176, 173, 212, 166, 113,
     94, 161,  41,  50, 239,  49, 111, 164,  70,  60,   2,  37, 171,  75,
    136, 156,  11,  56,  42, 146, 138, 229,  73, 146,  77,  61,  98, 196,
    135, 106,  63, 197, 195,  86,  96, 203, 113, 101, 170, 247, 181, 113,
     80, 250, 108,   7, 255, 237, 129, 226,  79, 107, 112, 166, 103, 241,
     24, 223, 239, 120, 198,  58,  60,  82, 128,   3, 184,  66, 143, 224,
    145, 224,  81, 206, 163,  45,  63,  90, 168, 114,  59,  33, 159,  95,
     28, 139, 123,  98, 125, 196,  15,  70, 194, 253,  54,  14, 109, 226,
     71,  17, 161,  93, 186,  87, 244, 138,  20,  52, 123, 251,  26,  36,
     17,  46,  52, 231, 232,  76,  31, 221,  84,  37, 216, 165, 212, 106,
    197, 242,  98,  43,  39, 175, 254, 145, 190,  84, 118, 222, 187, 136,
    120, 163, 236, 249,
};


/*
======================
=
= US_InitRndT
=
======================
*/

void US_InitRndT (bool randomize)
{
    if (randomize)
        rndindex = (SDL_GetTicks() >> 4) & 0xff;
    else
        rndindex = 0;
}


/*
======================
=
= US_RndT
=
======================
*/

int US_RndT (void)
{
    return rndtable[rndindex++ & 0xff];
}


/*
======================
=
= US_Print
=
= Prints a string in the current window. Newlines are supported.
=
======================
*/

void US_Print (const char *string)
{
    int  len;
    word w,h;
	char ch;
	char *strstart,*strend,*str;

	len = strlen(string) + 1;
	strstart = SafeMalloc(len);
    str = strstart;

	snprintf (str,len,string);

	while (*str)
	{
		strend = str;

    	while (((ch = *strend) != '\0') && (ch != '\n'))
			strend++;

    	*strend = '\0';

		VW_MeasureString (str,&w,&h);
		px = PrintX;
		py = PrintY;
		VW_DrawString (str);

		str = strend;

    	if (ch)
		{
			*strend = ch;
			str++;

			PrintX = WindowX;
			PrintY += h;
		}
		else
			PrintX += w;
	}

	free (strstart);
}


/*
======================
=
= US_PrintUnsigned
=
= Prints an unsigned long
=
======================
*/

void US_PrintUnsigned (uint32_t n)
{
    char buffer[32];

    snprintf (buffer,sizeof(buffer),"%u",n);

    US_Print (buffer);
}


/*
======================
=
= US_PrintSigned
=
= Prints a signed long
=
======================
*/

void US_PrintSigned (int32_t n)
{
    char buffer[32];

    US_Print (ltoa(n,buffer,10));
}


/*
======================
=
= US_PrintInCenter
=
= Prints a string in the center of the given rect
=
======================
*/

void US_PrintInCenter (const char *string, Rect r)
{
    word w,h,rw,rh;

    VW_MeasureString (string,&w,&h);

    rw = r.lr.x - r.ul.x;
    rh = r.lr.y - r.ul.y;

    px = r.ul.x + ((rw - w) / 2);
    py = r.ul.y + ((rh - h) / 2);

    VW_DrawString (string);
}


/*
======================
=
= US_PrintCentered
=
= Prints a string centered in the current window.
=
======================
*/

void US_PrintCentered (const char *string)
{
    Rect r;

    r.ul.x = WindowX;
    r.ul.y = WindowY;
    r.lr.x = r.ul.x + WindowW;
    r.lr.y = r.ul.y + WindowH;

    US_PrintInCenter (string,r);
}


/*
======================
=
= US_CPrintLine
=
= Prints a string centered on the current line and
= advances to the next line. Newlines are not supported.
=
======================
*/

void US_CPrintLine (const char *string)
{
    word w,h;

    VW_MeasureString (string,&w,&h);

    if (w > WindowW)
        Quit ("String exceeds width!");

    px = WindowX + ((WindowW - w) / 2);
    py = PrintY;

    VW_DrawString (string);

    PrintY += h;
}


/*
======================
=
= US_CPrint
=
= Prints a string in the current window. Newlines are supported.
=
======================
*/

void US_CPrint (const char *string)
{
    int  len;
    char ch;
	char *strstart,*strend,*str;

    len = strlen(string) + 1;
    strstart = SafeMalloc(len);
    str = strstart;

    snprintf (str,len,string);

	while (*str)
	{
		strend = str;

    	while (((ch = *strend) != '\0') && (ch != '\n'))
			strend++;

    	*strend = '\0';

        US_CPrintLine (str);

		str = strend;

    	if (ch)
		{
			*strend = ch;
			str++;
		}
	}

	free (strstart);
}


/*
===================
=
= US_ResetWindow
=
===================
*/

void US_ResetWindow (int heightofs)
{
    WindowX = 0;
    WindowY = 0;
    WindowW = screen.width;
    WindowH = screen.height - heightofs;
}


/*
======================
=
= US_ClearWindow
=
= Clears the current window to white and homes the cursor
=
======================
*/

void US_ClearWindow (void)
{
    VW_Bar (WindowX,WindowY,WindowW,WindowH,0xEF);

    PrintX = WindowX;
    PrintY = WindowY;
}


/*
======================
=
= US_DrawWindow
=
= Draws a frame and sets the current window parms
=
======================
*/

void US_DrawWindow (int x, int y, int w, int h)
{
    int i;
    int sx,sy,sw,sh;

    WindowX = x * 8;
    WindowY = y * 8;
    WindowW = w * 8;
    WindowH = h * 8;

    PrintX = WindowX;
    PrintY = WindowY;

    sx = (x - 1) * 8;
    sy = (y - 1) * 8;
    sw = (w + 1) * 8;
    sh = (h + 1) * 8;

    US_ClearWindow ();

    VW_DrawTile8 (sx,sy,0);
    VW_DrawTile8 (sx,sy + sh,5);

    for (i = sx + 8; i <= sx + sw - 8; i += 8)
    {
        VW_DrawTile8 (i,sy,1);
        VW_DrawTile8 (i,sy + sh,6);
    }

    VW_DrawTile8 (i,sy,2);
    VW_DrawTile8 (i,sy + sh,7);

    for (i = sy + 8; i <= sy + sh - 8; i += 8)
    {
        VW_DrawTile8 (sx,i,3);
        VW_DrawTile8 (sx + sw,i,4);
    }
}


/*
======================
=
= US_CenterWindow
=
= Generates a window of a given width & height in the middle of the screen
=
======================
*/

void US_CenterWindow (int w, int h)
{
    US_DrawWindow (((MaxX / 8) - w) / 2,((MaxY / 8) - h) / 2,w,h);
}


/*
======================
=
= US_SaveWindow
=
= Saves the current window parms into a record for later restoration
=
======================
*/

void US_SaveWindow (WindowRec *win)
{
    win->x = WindowX;
    win->y = WindowY;
    win->w = WindowW;
    win->h = WindowH;

    win->px = PrintX;
    win->py = PrintY;
}


/*
======================
=
= US_RestoreWindow
=
= Sets the current window parms to those held in the record
=
======================
*/

void US_RestoreWindow (WindowRec *win)
{
    WindowX = win->x;
    WindowY = win->y;
    WindowW = win->w;
    WindowH = win->h;

    PrintX = win->px;
    PrintY = win->py;
}


/*
======================
=
= US_XORICursor
=
= XORs the I-bar text cursor. Used by US_LineInput
=
======================
*/

static void US_XORICursor (int x, int y, const char *string, int cursor)
{
    static bool status;  // VGA doesn't XOR...
    char        buf[MaxString];
    int         temp;
    word        w,h;

    snprintf (buf,sizeof(buf),string);

    buf[cursor] = '\0';

    VW_MeasureString (buf,&w,&h);

    px = x + w - 1;
    py = y;

    VW_WaitVBL (1);

    if (status ^= true)
        VW_DrawString ("\x80");
    else
    {
        temp = fontcolor;
        fontcolor = backcolor;
        VW_DrawString ("\x80");
        fontcolor = temp;
    }
}


/*
======================
=
= US_CustomCursor
=
= Toggle Displays the custom text cursor. Used by US_LineInput
=
======================
*/

static void US_CustomCursor (int x, int y, const char *string, int cursor)
{
    static bool status;  // VGA doesn't XOR...
    char        buf[MaxString];
    int         temp,tempfont;
    word        w,h;

    snprintf (buf,sizeof(buf),string);

    buf[cursor] = '\0';

    VW_MeasureString (buf,&w,&h);

    px = x + w - 1;
    py = y;

    temp = fontcolor;
    tempfont = fontnumber;

    fontnumber = CustomCursor.fontnumber;

    if (status ^= true)
        fontcolor = CustomCursor.cursorcolor;
    else
        fontcolor = backcolor;

    VW_WaitVBL (1);

    VW_DrawString (&CustomCursor.cursorchar);

    fontcolor = temp;
    fontnumber = tempfont;
}


/*
======================
=
= US_LineInput
=
= Gets a line of user input at (x,y), the string defaults to whatever is
= pointed at by def. Input is restricted to maxchars chars or maxwidth
= pixels wide. If the user hits escape (and escok is true), nothing is
= copied into buf, and false is returned. If the user hits return, the
= current string is copied into buf, and true is returned
=
======================
*/

bool US_LineInput (int x, int y, char *buf, const char *def, bool escok, int maxchars, int maxwidth)
{
    bool     redraw,cursorvis,cursormoved,done,result;
    ScanCode scan;
    char     *text,string[MaxString],oldstring[MaxString];
    int      i;
    int      ch,cursor,len;
    word     w,h,temp;
    uint32_t lasttime,curtime;

    if (def)
        snprintf (string,sizeof(string),def);
    else
        string[0] = '\0';

    oldstring[0] = '\0';
    cursor = strlen(string);
    cursormoved = redraw = true;

    cursorvis = done = false;
    lasttime = GetTimeCount();
    LastScan = sc_None;

    IN_ClearTextInput ();

    while (!done)
    {
        IN_ProcessEvents ();

        if (cursorvis)
        {
            if (usecustomcursor)
                US_CustomCursor (x,y,string,cursor);
            else
                US_XORICursor (x,y,string,cursor);
        }

        scan = LastScan;
        LastScan = sc_None;

        curtime = GetTimeCount();

        switch (scan)
        {
            case sc_Return:
                snprintf (buf,sizeof(buf),string);
                done = true;
                result = true;
                break;

            case sc_Escape:
                if (escok)
                {
                    done = true;
                    result = false;
                }
                break;

            case sc_BackSpace:
                if (cursor)
                {
                    len = strlen(&string[--cursor]) + 1;
                    memmove (&string[cursor],&string[cursor + 1],len);
                    redraw = true;
                }

                cursormoved = true;
                break;
        }

        for (text = textinput; *text; text++)
        {
            ch = *text;

            len = strlen(string);

            VW_MeasureString (string,&w,&h);

            if (isprint(ch) && len < MaxString - 1 && (!maxchars || len < maxchars) && (!maxwidth || w < maxwidth))
            {
                for (i = len + 1; i > cursor; i--)
                    string[i] = string[i - 1];

                if (fontnumber == 2)
                    ch = toupper(ch);    // this font has uppercase only

                string[cursor++] = ch;

                redraw = true;
            }
        }

        IN_ClearTextInput ();

        if (redraw)
        {
            px = x;
            py = y;
            temp = fontcolor;
            fontcolor = backcolor;
            VW_DrawString (oldstring);
            fontcolor = temp;
            snprintf (oldstring,sizeof(oldstring),string);

            px = x;
            py = y;
            VW_DrawString (string);

            redraw = false;
        }

        if (cursormoved)
        {
            cursorvis = false;
            lasttime = curtime - TickBase;

            cursormoved = false;
        }

        if (curtime - lasttime > TickBase / 2)
        {
            lasttime = curtime;

            cursorvis ^= true;
        }

        if (cursorvis)
        {
            if (usecustomcursor)
                US_CustomCursor (x,y,string,cursor);
            else
                US_XORICursor (x,y,string,cursor);
        }

        VW_UpdateScreen (screen.buffer);
    }

    if (cursorvis)
    {
        if (usecustomcursor)
            US_CustomCursor (x,y,string,cursor);
        else
            US_XORICursor (x,y,string,cursor);
    }

    if (!result)
    {
        px = x;
        py = y;
        VW_DrawString (oldstring);
    }

    VW_UpdateScreen (screen.buffer);

    IN_ClearKeysDown ();

    return result;
}
