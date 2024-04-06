// 3D_INTER.C

#include "3d_def.h"


#define SCORE_Y_SPACING   7


const char *prep_msg = "^ST1^CEGet Ready, Blake!\r^XX";

#ifndef ID_CACHE_BRIEFS
char       BreifingText[13] = "BRIEF_Wx.TXT";
#endif

/*
==================
=
= Breifing
=
==================
*/

bool Breifing (int BreifingType, int episode)
{
#ifndef ID_CACHE_BRIEFS
    char chars[3] = {'L','W','I'};

    BreifingText[6] = chars[BreifingType];
    BreifingText[7] = '1' + episode;

    HelpPresenter (BreifingText,true,0,false);
#else
    HelpPresenter (NULL,true,((BRIEF_W1 + (episode * 2)) + BreifingType) - 1,false);
#endif
    return EscPressed;
}


/*
===================
=
= ShPrint
=
===================
*/

void ShPrint (const char *text, int shadowcolor, bool singlechar)
{
    size_t len;
    int    oldcolor,oldx,oldy;
    char   *str,buf[2] = {'\0','\0'};

    oldcolor = fontcolor;
    oldx = px;
    oldy = py;

    if (singlechar)
    {
        str = buf;
        buf[0] = *text;
    }
    else
    {
        len = strlen(text) + 1;
        str = SafeMalloc(len);

        snprintf (str,len,text);
    }

    fontcolor = shadowcolor;
    px++;
    py++;
    VW_DrawString (str);

    fontcolor = oldcolor;
    px = oldx;
    py = oldy;
    VW_DrawString (str);

    if (!singlechar)
        free (str);
}


/*
=================
=
= DisplayPrepingMsg
=
=================
*/

void DisplayPrepingMsg (const char *text)
{
    fontnumber = 1;

    BMAmsg (text);

//
// set thermometer boundaries
//
    WindowX = 36;
    WindowY = 188;
    WindowW = 260;
    WindowH = 32;

    VW_Bar (WindowX,WindowY - 7,WindowW - 10,2,BORDER_LO_COLOR);
    VW_Bar (WindowX,WindowY - 7,WindowW - 11,1,BORDER_TEXT_COLOR - 15);
    VW_Bar (WindowX,WindowY,WindowW - 10,2,BORDER_LO_COLOR);
    VW_Bar (WindowX,WindowY,WindowW - 11,1,BORDER_TEXT_COLOR - 15);

    if (screen.flags & SC_FADED)
        VW_FadeIn ();
    else
        VW_UpdateScreen (screen.buffer);
}


/*
=================
=
= LoadLevelUpdate
=
=================
*/

void LoadLevelUpdate (int current, int total)
{
    int w;

    if (!total)
        return;

    if (current > total)
        current = total;

    w = ((int32_t)(WindowW - 10) * current) / total;

    if (w)
        VW_Bar (WindowX,WindowY,w - 1,1,BORDER_TEXT_COLOR);

    VW_UpdateScreen (screen.buffer);
}


/*
=================
=
= DrawLevelTransition
=
=================
*/

void DrawLevelTransition (void)
{
    WindowY = 188;

    if (!(gamestate.flags & GS_QUICKRUN))
        VW_FadeIn ();

    LoadLevelUpdate (1,1);
    IN_UserInput (70);

    if (playstate != ex_transported)
        VW_FadeOut ();

    DrawPlayBorder ();
    VW_UpdateScreen (screen.buffer);
}


/*
==================
=
= DrawHighScores
=
==================
*/

void DrawHighScores (void)
{
    char      buffer[16];
    int       i;
    word      w,h;
    HighScore *s;

    ClearMenuBorders ();
    ClearMenuScreen ();
    VW_DrawPic (0,0,BACKGROUND_SCREENPIC);
    DrawMenuTitle ("HIGH SCORES");

    if (playstate != ex_title)
        DrawInstructions (IT_HIGHSCORES);

    fontnumber = 2;
    SetFontColor (ENABLED_TEXT_COLOR,TERM_BACK_COLOR);

    ShadowPrint ("NAME",86,60);
    //ShadowPrint("MISSION",150,60);
    ShadowPrint ("SCORE",175,60);
    ShadowPrint ("MISSION",247,53);
    ShadowPrint ("RATIO",254,60);

    for (i = 0, s = Scores; i < MaxScores; i++, s++)
    {
        SetFontColor (HIGHLIGHT_TEXT_COLOR - 1,TERM_BACK_COLOR);

        //
        // name
        //
        if (*s->name)
            ShadowPrint (s->name,45,68 + (SCORE_Y_SPACING * i));

#if 0
        //
        // mission
        //
        ltoa (s->episode + 1,buffer,10);
        ShadowPrint (buffer,165,68 + (SCORE_Y_SPACING * i));
#endif
        //
        // score
        //
        if (s->score > MAX_DISPLAY_SCORE)
            SetFontColor (HIGHLIGHT_TEXT_COLOR + 1,TERM_BACK_COLOR);

        ltoa (s->score,buffer,10);
        VW_MeasureString (buffer,&w,&h);
        ShadowPrint (buffer,205 - w,68 + (SCORE_Y_SPACING * i));  // 235

        //
        // mission ratio
        //
        ltoa (s->ratio,buffer,10);
        VW_MeasureString (buffer,&w,&h);
        ShadowPrint (buffer,272 - w,68 + (SCORE_Y_SPACING * i));
    }

    VW_UpdateScreen (screen.buffer);
}


/*
=======================
=
= CheckHighScore
=
=======================
*/

void CheckHighScore (int32_t score, int other)
{
    int            i,j;
    int            n;
    HighScore      myscore;
    CustomCursor_t TermCursor = {'@',0,HIGHLIGHT_TEXT_COLOR,2};

    //
    // check for cheaters
    //
    if (DebugOk)
    {
        SD_PlaySound (NOWAYSND);
        return;
    }

    myscore.name[0] = '\0';
    myscore.score = score;
    myscore.episode = gamestate.episode;
    myscore.completed = other;
    myscore.ratio = ShowStats(0,0,ss_justcalc,&gamestuff.level[gamestate.mapon].stats);

    for (i = 0, n = -1; i < MaxScores; i++)
    {
        if (myscore.score > Scores[i].score || (myscore.score == Scores[i].score && myscore.completed > Scores[i].completed))
        {
            for (j = MaxScores - 1; j > i; j--)
                Scores[j] = Scores[j - 1];

            Scores[i] = myscore;
            n = i;
            break;
        }
    }

    StartCPMusic (ROSTER_MUS);
    DrawHighScores ();

    VW_FadeIn ();

    if (n != -1)
    {
        //
        // got a high score
        //
        DrawInstructions (IT_ENTER_HIGHSCORE);
        SetFontColor (HIGHLIGHT_TEXT_COLOR,TERM_BACK_COLOR);

        PrintX = 45;
        PrintY = 68 + (SCORE_Y_SPACING * n);
        usecustomcursor = true;
        CustomCursor = TermCursor;

        US_LineInput (PrintX,PrintY,Scores[n].name,NULL,true,MaxHighName,100);
    }
    else
    {
        IN_ClearKeysDown ();
        IN_UserInput (500);
    }

    StopMusic ();

    usecustomcursor = false;
}
