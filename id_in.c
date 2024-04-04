//
// ID Engine
// ID_IN.c - Input Manager
// v1.0d1
// By Jason Blochowiak
//

#include "3d_def.h"


#define SENSITIVE     60


//
// configuration variables
//
bool         MousePresent;
bool         JoystickPresent;
bool         btnstate[8];

bool         Keyboard[sc_Last];
bool         Paused;
char         textinput[TEXTINPUTSIZE];
ScanCode     LastScan;

SDL_Joystick *Joystick;
int          numjoybuttons;
int          numjoyhats;

int controldirtable[NUMDIRECTIONS] =
{
    dir_NorthWest,
    dir_North,
    dir_NorthEast,
    dir_West,
    dir_None,
    dir_East,
    dir_SouthWest,
    dir_South,
    dir_SouthEast,
};


/*
=====================
=
= IN_GetMouseButtons
=
= Get the status of the mouse buttons from the mouse driver
=
=====================
*/

uint32_t IN_GetMouseButtons (void)
{
    if (!MousePresent)
        return 0;

    int buttons = SDL_GetMouseState(NULL,NULL);
    int middlePressed = buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE);
    int rightPressed = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);

    buttons &= ~(SDL_BUTTON(SDL_BUTTON_MIDDLE) | SDL_BUTTON(SDL_BUTTON_RIGHT));

    if (middlePressed)
        buttons |= 1 << 2;
    if (rightPressed)
        buttons |= 1 << 1;

    return buttons;
}


/*
===================
=
= IN_GetJoyDelta
=
= Returns the relative movement of the specified joystick (from +/-127)
=
===================
*/

void IN_GetJoyDelta (int *dx, int *dy)
{
    int  x,y;
    byte hatstate;

    if (!Joystick)
    {
        *dx = 0;
        *dy = 0;

        return;
    }

    SDL_JoystickUpdate ();

    x = SDL_JoystickGetAxis(Joystick,0) >> 8;
    y = SDL_JoystickGetAxis(Joystick,1) >> 8;

    if (param_joystickhat != -1)
    {
        hatstate = SDL_JoystickGetHat(Joystick,param_joystickhat);

        if (hatstate & SDL_HAT_RIGHT)
            x += 127;
        else if (hatstate & SDL_HAT_LEFT)
            x -= 127;

        if (hatstate & SDL_HAT_DOWN)
            y += 127;
        else if (hatstate & SDL_HAT_UP)
            y -= 127;

        x = MAX(-128,MIN(x,127));
        y = MAX(-128,MIN(y,127));
    }

    *dx = x;
    *dy = y;
}


/*
===================
=
= IN_GetJoyFineDelta
=
= Returns the relative movement of the specified joystick
= without dividing the results by 256 (from +/-127)
=
===================
*/

void IN_GetJoyFineDelta (int *dx, int *dy)
{
    int x,y;

    if (!Joystick)
    {
        *dx = 0;
        *dy = 0;

        return;
    }

    SDL_JoystickUpdate ();

    x = SDL_JoystickGetAxis(Joystick,0);
    y = SDL_JoystickGetAxis(Joystick,1);

    x = MAX(-128,MIN(x,127));
    y = MAX(-128,MIN(y,127));

    *dx = x;
    *dy = y;
}


/*
===================
=
= IN_GetJoyButtons
=
= Returns the button status of the joystick
=
===================
*/

uint32_t IN_GetJoyButtons (void)
{
    int      i;
    uint32_t buttons;

    if (!Joystick)
        return 0;

    SDL_JoystickUpdate ();

    buttons = 0;

    for (i = 0; i < numjoybuttons; i++)
        buttons |= SDL_JoystickGetButton(Joystick,i) << i;

    return buttons;
}


/*
===================
=
= Clear accumulated mouse movement
=
===================
*/

void IN_CenterMouse (void)
{
    if (screen.flags & SC_INPUTGRABBED)
        SDL_WarpMouseInWindow (screen.window,screen.width / 2,screen.height / 2);
}


/*
===================
=
= Map certain keys to their defaults
=
===================
*/

ScanCode IN_MapKey (int key)
{
    ScanCode scan = key;

    switch (key)
    {
        case sc_KeyPadEnter: scan = sc_Enter; break;
        case sc_RAlt: scan = sc_LAlt; break;
        case sc_RControl: scan = sc_LControl; break;

        case sc_KeyPad2:
        case sc_KeyPad4:
        case sc_KeyPad6:
        case sc_KeyPad8:
            if (!(SDL_GetModState() & KMOD_NUM))
            {
                switch (key)
                {
                    case sc_KeyPad2: scan = sc_DownArrow; break;
                    case sc_KeyPad4: scan = sc_LeftArrow; break;
                    case sc_KeyPad6: scan = sc_RightArrow; break;
                    case sc_KeyPad8: scan = sc_UpArrow; break;
                }
            }
            break;
    }

    return scan;
}


/*
===================
=
= IN_SetWindowGrab
=
===================
*/

void IN_SetWindowGrab (SDL_Window *window)
{
    const char *which[] = {"hide","show"};

    bool grabinput = (screen.flags & SC_INPUTGRABBED) != 0;

    if (SDL_ShowCursor(!grabinput) < 0)
        Quit ("Unable to %s cursor: %s\n",which[!grabinput],SDL_GetError());

    SDL_SetWindowGrab (window,grabinput);

    if (SDL_SetRelativeMouseMode(grabinput))
        Quit ("Unable to set relative mode for mouse: %s\n",SDL_GetError());
}


/*
=============================================================================

                          INPUT PROCESSING

=============================================================================
=
= Input processing is not done via interrupts anymore! Instead you have to call IN_ProcessEvents
= in order to process any events like key presses or mouse movements. Alternatively you can call
= IN_WaitAndProcessEvents, which waits for an event before processing if none were available.
=
= If you have a loop with a blinking cursor where you are waiting for some key to be pressed,
= this loop MUST contain an IN_ProcessEvents/IN_WaitAndProcessEvents call, otherwise the program
= will never notice any keypress.
=
= Don't write something like, for example, "while (!keyboard[sc_X]) IN_ProcessEvents();" to
= wait for the player to press the X key, as this would result in 100% CPU usage. Either
= use IN_WaitAndProcessEvents, or add SDL_Delay(5) to the loop, which waits for 5 ms before
= it continues.
=
===========================================
*/


/*
===================
=
= IN_HandleEvent
=
===================
*/

static void IN_HandleEvent (SDL_Event *event)
{
    int      key;
    ScanCode scan;

    key = event->key.keysym.scancode;

    switch (event->type)
    {
        case SDL_QUIT:
            Quit (NULL);
            break;

        case SDL_KEYDOWN:
            scan = IN_MapKey(key);

            if (scan < sc_Last)
            {
                LastScan = scan;

                Keyboard[LastScan] = true;

                if (Keyboard[sc_Alt] && Keyboard[sc_F4])
                    Quit (NULL);
                else if (Keyboard[sc_ScrollLock])
                {
                    screen.flags ^= SC_INPUTGRABBED;

                    IN_SetWindowGrab (screen.window);
                }
            }
            break;

        case SDL_KEYUP:
            scan = IN_MapKey(key);

            if (scan < sc_Last)
                Keyboard[scan] = false;
            break;

        case SDL_TEXTINPUT:
            snprintf (textinput,sizeof(textinput),"%s",event->text.text);
            break;
    }
}


/*
===================
=
= IN_ProcessEvents
=
===================
*/

void IN_ProcessEvents (void)
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
        IN_HandleEvent (&event);
}


/*
===================
=
= Wait for an event
=
===================
*/

void IN_WaitEvent (void)
{
    if (!SDL_WaitEvent(NULL))
        Quit ("Error waiting for event: %s\n",SDL_GetError());
}


/*
===================
=
= Wait for an event before processing
=
===================
*/

void IN_WaitAndProcessEvents (void)
{
    IN_WaitEvent ();
    IN_ProcessEvents ();
}


/*
===================
=
= Start up the Input Mgr
=
===================
*/

void IN_Startup (void)
{
    int numjoysticks;

    IN_ClearKeysDown ();

    numjoysticks = SDL_NumJoysticks();

    if (param_joystickindex && (param_joystickindex < -1 || param_joystickindex >= numjoysticks))
    {
        if (!numjoysticks)
            Quit ("No joysticks are available to SDL!");
        else
            Quit ("The joystick index must be between -1 and %d!",numjoysticks - 1);
    }
    else if (param_joystickindex >= 0 && param_joystickindex < numjoysticks)
    {
        Joystick = SDL_JoystickOpen(param_joystickindex);

        if (Joystick)
        {
            numjoybuttons = SDL_JoystickNumButtons(Joystick);

            if (numjoybuttons > 32)
                numjoybuttons = 32;      // only up to 32 buttons are supported

            numjoyhats = SDL_JoystickNumHats(Joystick);

            if (param_joystickhat < -1 || param_joystickhat >= numjoyhats)
                Quit ("The joystickhat param must be between 0 and %d!",numjoyhats - 1);
        }
    }

    IN_CenterMouse ();

    SDL_EventState (SDL_MOUSEMOTION,SDL_IGNORE);

    if (screen.flags & (SC_FULLSCREEN | SC_INPUTGRABBED))
        IN_SetWindowGrab (screen.window);

    MousePresent = true;
    JoystickPresent = Joystick != NULL;
}


/*
===================
=
= Shutdown the Input Mgr
=
===================
*/

void IN_Shutdown (void)
{
    if (Joystick)
    {
        SDL_JoystickClose (Joystick);

        Joystick = NULL;
        JoystickPresent = false;
    }
}


/*
===================
=
= IN_ClearKey
=
===================
*/

void IN_ClearKey (ScanCode code)
{
    Keyboard[code] = false;

    if (LastScan == code)
        LastScan = sc_None;
}

/*
===================
=
= Clear the keyboard array
=
===================
*/

void IN_ClearKeysDown (void)
{
    LastScan = sc_None;
    memset (Keyboard,0,sizeof(Keyboard));
}


/*
===================
=
= IN_ClearTextInput
=
===================
*/

void IN_ClearTextInput (void)
{
    memset (textinput,0,sizeof(textinput));
}


/*
===================
=
= Fill in the control info struct
=
===================
*/

void IN_ReadControl (ControlInfo *info)
{
    static int totalmousex,totalmousey;
    bool       rightpressed,middlepressed;
    bool       mouseactive;
    uint32_t   buttons;
    int        mx,my;
    int        jx,jy;
    int        mousex,mousey;

    mx = my = 0;
    buttons = 0;
    mouseactive = false;

    IN_ProcessEvents ();

    if (Keyboard[sc_Home])
    {
        mx = -1;
        my = -1;
    }
    else if (Keyboard[sc_PgUp])
    {
        mx = 1;
        my = -1;
    }
    else if (Keyboard[sc_End])
    {
        mx = -1;
        my = 1;
    }
    else if (Keyboard[sc_PgDn])
    {
        mx = 1;
        my = 1;
    }

    if (Keyboard[sc_LeftArrow])
        mx = -1;
    else if (Keyboard[sc_RightArrow])
        mx = 1;

    if (Keyboard[sc_UpArrow])
        my = -1;
    else if (Keyboard[sc_DownArrow])
        my = 1;

    if (mouseenabled && (screen.flags & SC_INPUTGRABBED))
    {
        buttons = SDL_GetRelativeMouseState(&mousex,&mousey);
        rightpressed = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
        middlepressed = buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE);
        buttons &= ~(SDL_BUTTON(SDL_BUTTON_MIDDLE) | SDL_BUTTON(SDL_BUTTON_RIGHT));

        if (rightpressed)
            buttons |= 1 << 1;
        if (middlepressed)
            buttons |= 1 << 2;

        totalmousex += mousex;
        totalmousey += mousey;

        if (totalmousex  < -SENSITIVE)
        {
            mx = -1;
            mouseactive = true;
        }
        else if (totalmousex  > SENSITIVE)
        {
            mx = 1;
            mouseactive = true;
        }

        if (totalmousey < -SENSITIVE)
        {
            my = -1;
            mouseactive = true;
        }
        else if (totalmousey > SENSITIVE)
        {
            my = 1;
            mouseactive = true;
        }

        if (mouseactive)
            totalmousex = totalmousey = 0;

        if (buttons)
            mouseactive = true;
    }

    if (joystickenabled && !mouseactive)
    {
        IN_GetJoyDelta (&jx,&jy);

        if (jx < -SENSITIVE)
            mx = -1;
        else if (jx > SENSITIVE)
            mx = 1;

        if (jy < -SENSITIVE)
            my = -1;
        else if (jy > SENSITIVE)
            my = 1;

        buttons = IN_GetJoyButtons();
    }

    info->x = mx * 127;
    info->y = my * 127;
    info->xaxis = mx;
    info->yaxis = my;
    info->button0 = buttons & 1;
    info->button1 = buttons & (1 << 1);
    info->button2 = buttons & (1 << 2);
    info->button3 = buttons & (1 << 3);
    info->dir = controldirtable[((my + 1) * 3) + (mx + 1)];
}


/*
===================
=
= Waits for a scan code, then clears LastScan and returns the scan code
=
===================
*/

ScanCode IN_WaitForKey (void)
{
    ScanCode result;

    for (result = LastScan; result == sc_None; result = LastScan)
        IN_WaitAndProcessEvents ();

    LastScan = sc_None;

    return result;
}


/*
===================
=
= Get initial state of inputs
=
===================
*/

void IN_StartAck (void)
{
    int      i;
    uint32_t buttons;

    IN_ClearKeysDown ();
    memset (btnstate,0,sizeof(btnstate));

    //
    // TODO: with 32 buttons this can overflow,
    // but what kind of madlad uses a controller
    // and a mouse at the same time? It's probably
    // better to just get buttons from one input type,
    // and if the result is 0, test the other type
    //
    buttons = IN_GetJoyButtons() << 4;

    buttons |= IN_GetMouseButtons();

    for (i = 0; i < 8; i++, buttons >>= 1)
    {
        if (buttons & 1)
            btnstate[i] = true;
    }
}


/*
===================
=
= IN_CheckAck
=
===================
*/

bool IN_CheckAck (void)
{
    int      i;
    uint32_t buttons;

//
// see if something has been pressed
//
    if (LastScan)
        return true;

    buttons = IN_GetJoyButtons() << 4;
    buttons |= IN_GetMouseButtons();

    for (i = 0; i < 8; i++, buttons >>= 1)
    {
        if (buttons & 1)
        {
            if (!btnstate[i])
            {
                //
                // wait until button has been released
                //
                do
                {
                    IN_WaitAndProcessEvents ();

                    buttons = IN_GetJoyButtons() << 4;
                    buttons |= IN_GetMouseButtons();

                } while (buttons & (1 << i));

                return true;
            }
        }
        else
            btnstate[i] = false;
    }

    return false;
}


/*
===================
=
= Waits for a button or key press. If a button is down upon
= calling, it must be released for it to be recognized
=
===================
*/

void IN_Ack (void)
{
    IN_StartAck ();

    while (!IN_CheckAck())
        IN_WaitAndProcessEvents ();
}


/*
===================
=
= Waits for the specified delay time (in ticks) or the
= user pressing a key or a mouse button.
=
===================
*/

bool IN_UserInput (uint32_t delay)
{
    uint32_t lasttime;

    lasttime = GetTimeCount();

    IN_StartAck ();

    do
    {
        IN_ProcessEvents ();

        VW_WaitVBL (1);

        if (IN_CheckAck())
            return true;

    } while (GetTimeCount() - lasttime < delay);

    return false;
}
