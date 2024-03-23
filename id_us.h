//
// ID Engine
// ID_US.h - Header file for the User Manager
// v1.0d1
// By Jason Blochowiak
//

#ifndef _ID_US_H_
#define _ID_US_H_


#define MaxX         320
#define MaxY         160

#define MaxHighName  57
#define MaxScores    10

#define MaxGameName  32

#define MaxString    128    // maximum input string size


typedef struct
{
    char     name[MaxHighName + 1];
    int32_t  score;
    uint16_t completed,episode,ratio;
} HighScore;


typedef struct
{
   int       x,y;
   int       w,h;
   int       px,py;
} WindowRec;

//
// Custom Cursor struct type for US_LineInput
//
typedef struct
{
    char     cursorchar;
    char     donotuse;          // space holder for ASCZ string
    uint16_t cursorcolor;
    uint16_t fontnumber;
} CustomCursor_t;


extern  byte            rndtable[256];
extern  bool            NoWait;
extern  word            PrintX,PrintY;     // Current printing location in the window
extern  word            WindowX,WindowY;   // Current location of window
extern  word            WindowW,WindowH;   // Current size of window

extern  HighScore       Scores[MaxScores];

extern  CustomCursor_t  CustomCursor;
extern  bool            usecustomcursor;

#define US_HomeWindow() {PrintX = WindowX; PrintY = WindowY;}

void    US_InitRndT (bool randomize);
void    US_DrawWindow (int x, int y, int w, int h);
void    US_CenterWindow (int w, int h);
void    US_SaveWindow (WindowRec *win);
void    US_RestoreWindow (WindowRec *win);
void    US_ClearWindow (void);
void    US_PrintCentered (const char *string);
void    US_CPrint (const char *string);
void    US_CPrintLine (const char *string);
void    US_Print (const char *string);
void    US_PrintInCenter (const char *string, Rect r);
void    US_PrintUnsigned (uint32_t n);
void    US_PrintSigned (int32_t n);

bool    US_LineInput (int x, int y, char *buf, const char *def, bool escok, int maxchars, int maxwidth);
int     US_RndT (void);

#endif
