//
// basic player movement

#include "3d_def.h"


#define JOYSCALE        2


int             playstate;

int             lastmusicchunk;
int             lastmusicoffset;

int             musicnum;

int             DebugOk = 0;

bool            godmode;

objtype         objlist[MAXACTORS],*lastobj;
objtype         *player,*objfreelist,*lastattacker;

objtype         *actorat[MAPSIZE][MAPSIZE];

int             tics,realtics;

unsigned        ExtraRadarFlags;

int  dirscan[4] = { sc_UpArrow, sc_RightArrow, sc_DownArrow, sc_LeftArrow };
int  buttonscan[NUMBUTTONS] = { sc_Control, sc_Alt, sc_LShift, sc_Space, sc_1, sc_2, sc_3, sc_4 };
bool buttonheld[NUMBUTTONS];

//
// current user input
//
int  controlx,controly;         // range from -100 to 100 per tic
bool buttonstate[NUMBUTTONS];

bool demorecord,demoplayback;

void    InitObjList (void);
objtype *GetNewObj (void);
void    RemoveObj (objtype *gone);
void    DoActor (objtype *obj);


/*
=============================================================================

                            MUSIC STUFF

=============================================================================
*/

//
// list of songs for each level
//
int songs[] =
{
    MAJMIN_MUS,
    STICKS_MUS,
    MOURNING_MUS,
    LURKING_MUS,
    CIRCLES_MUS,
    TIME_MUS,
    TOHELL_MUS,
    FORTRESS_MUS,
    GIVING_MUS,
    HARTBEAT_MUS,
    MOURNING_MUS,
    MAJMIN_MUS,
    VACCINAP_MUS,
    LURKING_MUS,
    MONASTRY_MUS,
    TOMBP_MUS,
    DARKNESS_MUS,
    MOURNING_MUS,
    SERPENT_MUS,
    TIME_MUS,
    CATACOMB_MUS,
    PLOT_MUS,
    GIVING_MUS,
    VACCINAP_MUS,
};


/*
=================
=
= StopMusic
=
=================
*/

int StopMusic (void)
{
    int lastoffs = SD_MusicOff();

    CA_UncacheAudioChunk (STARTMUSIC + lastmusicchunk);

    return lastoffs;
}


/*
=================
=
= StartMusic
=
=================
*/

void StartMusic (void)
{
    SD_MusicOff ();

    lastmusicchunk = songs[currentsong];

    SD_StartMusic (STARTMUSIC + lastmusicchunk);
}


/*
=================
=
= ContinueMusic
=
=================
*/

void ContinueMusic (int offs)
{
    SD_MusicOff ();

    lastmusicchunk = songs[currentsong];

    SD_ContinueMusic (STARTMUSIC + lastmusicchunk,offs);
}


/*
===================
=
= PollKeyboardButtons
=
===================
*/

void PollKeyboardButtons (void)
{
    int i;

    for (i = 0; i < NUMBUTTONS; i++)
    {
        if (Keyboard[buttonscan[i]])
            buttonstate[i] = true;
    }
}


/*
===================
=
= PollKeyboardMove
=
===================
*/

void PollKeyboardMove (void)
{
    int delta = buttonstate[bt_run] ? RUNMOVE * tics : BASEMOVE * tics;

    if (Keyboard[dirscan[di_north]])
        controly -= delta;
    if (Keyboard[dirscan[di_south]])
        controly += delta;
    if (Keyboard[dirscan[di_west]])
        controlx -= delta;
    if (Keyboard[dirscan[di_east]])
        controlx += delta;
}


/*
===================
=
= PollControls
=
= Gets user or demo input, call once each frame
=
= controlx      set between -100 and 100 per tic
= controly
= buttonheld[]  the state of the buttons LAST frame
= buttonstate[] the state of the buttons THIS frame
=
===================
*/

void PollControls (void)
{
    int max,min;

    IN_ProcessEvents ();

//
// get timing info for last frame
//
    CalcTics ();

    controlx = 0;
    controly = 0;
    memcpy (buttonheld,buttonstate,sizeof(buttonstate));
    memset (buttonstate,0,sizeof(buttonstate));

//
// get button states
//
    PollKeyboardButtons ();

//
// get movements
//
    PollKeyboardMove ();

//
// bound movement to a maximum
//
    max = 100 * tics;
    min = -max;
    if (controlx > max)
        controlx = max;
    else if (controlx < min)
        controlx = min;

    if (controly > max)
        controly = max;
    else if (controly < min)
        controly = min;
}


/*
=============================================================================

                            OBJLIST DATA STRUCTURE

=============================================================================
=
= The objlist contains structures for every actor currently playing. The structure
= is accessed as a linked list starting at *player, ending when obj->next == NULL.
= GetNewObj inserts a new object at the end of the list, meaning that if an actor
= spawns another actor, the new one WILL get to think and react the same frame.
= RemoveObj unlinks the given object and returns it to the free list, but does
= not damage the object's ->next pointer, so if the current object removes itself,
= a linked list following loop can still safely get to the next element.
=
= <backwardly linked free list>
=
=============================================================================
*/


/*
=========================
=
= InitObjList
=
= Call to clear out the actor object lists returning them all to the free
= list. Allocates a special spot for the player.
=
=========================
*/

void InitObjList (void)
{
    int i;

    //
    // init the actor lists and nullify global pointers
    //
    // TODO: make sure fixing the overflow doesn't break Blake
    //
    for (i = 0; i < MAXACTORS; i++)
    {
        objlist[i].prev = &objlist[i + 1];
        objlist[i].next = NULL;
    }

    objlist[MAXACTORS - 1].prev = NULL;

    objfreelist = &objlist[0];
    lastobj = lastattacker = NULL;

    //
    // give the player the first free spots
    //
    player = GetNewObj();
}


/*
=========================
=
= GetNewObj
=
= Returns a pointer to a free spot in objlist.
= The free spot is inserted at the end of the liked list
=
= When the object list is full, the caller can either have it bomb out ot
= return a dummy object pointer that will never get used
=
=========================
*/

objtype *GetNewObj (void)
{
    objtype *newobj = NULL;

    if (!objfreelist)
        Quit ("No free spots in objlist!");

    newobj = objfreelist;
    objfreelist = newobj->prev;
    memset (newobj,0,sizeof(*newobj));

    if (lastobj)
        lastobj->next = newobj;

    newobj->prev = lastobj;     // newobj->next is already NULL from memset
    newobj->active = ac_no;
    lastobj = newobj;

    return newobj;
}


/*
=========================
=
= RemoveObj
=
= Add the given object back into the free list, and unlink it from it's
= neighbors
=
=========================
*/

void RemoveObj (objtype *gone)
{
    if (gone == player)
        Quit ("Tried to remove the player!");

    gone->state = NULL;

    //
    // fix the next object's back link
    //
    if (gone == lastobj)
        lastobj = gone->prev;
    else
        gone->next->prev = gone->prev;

    //
    // fix the previous object's forward link
    //
    gone->prev->next = gone->next;

    //
    // add it back in to the free list
    //
    gone->prev = objfreelist;
    objfreelist = gone;
}


/*
=====================
=
= DoActor
=
=====================
*/

void DoActor (objtype *obj)
{
    void (*think)(objtype *);

    if ((obj->flags & FL_FREEZE) || (!obj->active && !areabyplayer[obj->areanumber]))
		return;

    if (!(obj->flags & (FL_NONMARK | FL_NEVERMARK)))
        actorat[obj->tilex][obj->tiley] = NULL;

//
// non transitional object
//
    if (!obj->ticcount)
    {
        think = obj->state->think;

        if (think)
        {
            think (obj);

            if (!obj->state)
            {
                RemoveObj (obj);
                return;
            }
        }

        if (obj->flags & FL_NEVERMARK)
            return;

        if ((obj->flags & FL_NONMARK) && actorat[obj->tilex][obj->tiley])
            return;

        actorat[obj->tilex][obj->tiley] = obj;
        return;
    }

//
// transitional object
//
    obj->ticcount -= tics;

    while (obj->ticcount <= 0)
    {
        think = obj->state->action;        // end of state action

        if (think)
        {
            think (obj);

            if (!obj->state)
            {
                RemoveObj (obj);
                return;
            }
        }

        //
        // TODO: action (obj) could have changed obj->state; we should test
        // if obj->ticcount is still <= 0 before changing the state here
        //
        obj->state = obj->state->next;

        if (!obj->state)
        {
            RemoveObj (obj);
            return;
        }

        if (!obj->state->tictime)
        {
            obj->ticcount = 0;
            break;
        }

        obj->ticcount += obj->state->tictime;
    }

    //
    // think
    //
    think = obj->state->think;

    if (think)
    {
        think (obj);

        if (!obj->state)
        {
            RemoveObj (obj);
            return;
        }
    }

    if (obj->flags & FL_NEVERMARK)
        return;

    if ((obj->flags & FL_NONMARK) && actorat[obj->tilex][obj->tiley])
        return;

    actorat[obj->tilex][obj->tiley] = obj;
}


/*
===================
=
= PlayLoop
=
===================
*/
#ifndef NOTYET
void PlayLoop (void)
{
    objtype *obj;

    playstate = ex_stillplaying;
    lasttimecount = framecount = frameon = 0;
    pwallstate = anglefrac = 0;
    memset (buttonstate,0,sizeof(buttonstate));
    ForceUpdateStatusBar ();

    if (MousePresent)
        IN_CenterMouse ();

    do
    {
        PollControls ();

        //
        // actor thinking
        //
        MoveDoors ();
        MovePWalls ();

        for (obj = player; obj; obj = obj->next)
            DoActor (obj);

        ThreeDRefresh ();

        gamestate.timecount += tics;

        UpdateSoundLoc ();

    } while (playstate == ex_stillplaying);

    gamestate.flags &= ~GS_VIRGIN_LEVEL;
}
#else
void PlayLoop (void)
{
    bool    reset_areas = false;
    int     give;
    objtype *obj;

    playstate = ex_stillplaying;
    lasttimecount = framecount = frameon = 0;
    pwallstate = anglefrac = 0;
    memset (buttonstate,0,sizeof(buttonstate));
    ClearPaletteShifts ();
    ForceUpdateStatusBar ();

    if (MousePresent)
        IN_CenterMouse ();

    tics = 1;   // for first time through

    if (demoplayback)
        IN_StartAck ();

    do
    {
        PollControls ();

        //
        // actor thinking
        //
        madenoise = false;

        if (alerted)
            alerted--;

        MoveDoors ();
        MovePWalls ();

        for (obj = player; obj; obj = obj->next)
        {
            if (obj != player && (Keyboard[sc_6] || Keyboard[sc_7]) && Keyboard[sc_8] && DebugOk)
            {
                if (!reset_areas)
                    memset (areabyplayer,1,sizeof(areabyplayer));

                reset_areas = true;

                if (((!(obj->flags & FL_INFORMANT) && (obj->flags & FL_SHOOTABLE))
                 || (obj->obclass == liquidobj && !(obj->flags & FL_DEADGUY)))
                    DamageActor (obj,1000,player);
            }
            else if (reset_areas)
            {
                ConnectAreas ();
                reset_areas = false;
            }

            DoActor (obj);
        }

        if (NumEAWalls)
            CheckSpawnEA ();

        if (!GoldsternInfo.GoldSpawned && GoldsternInfo.SpawnCnt)
            CheckSpawnGoldstern ();

        UpdatePaletteShifts ();

        ThreeDRefresh ();

        gamestate.timecount += tics;

        UpdateSoundLoc ();

        //
        // display first-time instructions
        //
        if (ShowQuickMsg)
            ShowQuickInstructions ();

        CheckKeys ();

        if (demoplayback && demoptr == lastdemoptr)
            playstate = ex_title;

        //
        // debug aids
        //
        if (singlestep)
        {
            VW_WaitVBL (14);
            lasttimecount = GetTimeCount();
        }

        if (extravbls)
            VW_WaitVBL (extravbls);

        if (demoplayback && IN_CheckAck())
        {
            IN_ClearKeysDown ();
            playstate = ex_abort;
        }

    } while (!playstate && !startgame);

    if (playstate != ex_died)
        FinishPaletteShifts ();

    gamestate.flags &= ~GS_VIRGIN_LEVEL;
}
#endif
