//
// ID Engine
// ID_IN.c - Input Manager
// v1.0d1
// By Jason Blochowiak
//

#include "3d_def.h"


//
// joystick constants
//
#define JoyScaleMax  32768
#define JoyScaleShift 8
//#define MaxJoyValue  5000

/*
=============================================================================

                        GLOBAL VARIABLES

=============================================================================
*/

//
// configuration variables
//
bool   MousePresent;
bool   JoysPresent[MaxJoys];
bool   JoyPadPresent;
bool   NGinstalled;
bool   btnstate[8];

//  Global variables
bool JoystickCalibrated;    // JAM - added
ControlType ControlTypeUsed;    // JAM - added
bool  Keyboard[sc_Last];
bool  Paused;
char  textinput[TEXTINPUTSIZE];
ScanCode LastScan;

JoystickDef JoyDefs[MaxJoys];
ControlType Controls[MaxPlayers];


/*
=============================================================================

     LOCAL VARIABLES

=============================================================================
*/

bool  IN_Started;
#ifdef NOTYET
static bool  CapsLock;
static ScanCode CurCode,LastCode;
#endif

static Direction DirTable[] =  // Quick lookup for total direction
{
    dir_NorthWest, dir_North, dir_NorthEast,
    dir_West,  dir_None, dir_East,
    dir_SouthWest, dir_South, dir_SouthEast
};


char *IN_ParmStrings[] = {"nojoys","nomouse","enablegp",0};


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


#ifdef NOTYET
///////////////////////////////////////////////////////////////////////////
//
// IN_GetJoyAbs() - Reads the absolute position of the specified joystick
//
///////////////////////////////////////////////////////////////////////////
void
IN_GetJoyAbs(word joy,word *xp,word *yp)
{
 byte xb,yb,
   xs,ys;
 word x,y;

// Handle Notebook Gamepad's joystick.
//
 if (NGinstalled)
 {
  unsigned ax,bx;

  joy++;

  _AL=0x01;
  _BX=joy;
  NGjoy(0x01);

  ax=_AX;  bx=_BX;
  *xp=ax;  *yp=bx;

  return;
 }

// Handle normal PC joystick.
//
 x = y = 0;
 xs = joy? 2 : 0;  // Find shift value for x axis
 xb = 1 << xs;   // Use shift value to get x bit mask
 ys = joy? 3 : 1;  // Do the same for y axis
 yb = 1 << ys;

// Read the absolute joystick values
asm  pushf    // Save some registers
asm  push si
asm  push di
asm  cli     // Make sure an interrupt doesn't screw the timings


asm  mov  dx,0x201
asm  in  al,dx
asm  out  dx,al  // Clear the resistors

asm  mov  ah,[xb]  // Get masks into registers
asm  mov  ch,[yb]

asm  xor  si,si  // Clear count registers
asm  xor  di,di
asm  xor  bh,bh  // Clear high byte of bx for later

asm  push bp   // Don't mess up stack frame
asm  mov  bp,MaxJoyValue

loop:
asm  in  al,dx  // Get bits indicating whether all are finished

asm  dec  bp   // Check bounding register
asm  jz  done  // We have a silly value - abort

asm  mov  bl,al  // Duplicate the bits
asm  and  bl,ah  // Mask off useless bits (in [xb])
asm  add  si,bx  // Possibly increment count register
asm  mov  cl,bl  // Save for testing later

asm  mov  bl,al
asm  and  bl,ch  // [yb]
asm  add  di,bx

asm  add  cl,bl
asm  jnz  loop   // If both bits were 0, drop out

done:
asm     pop  bp

asm  mov  cl,[xs]  // Get the number of bits to shift
asm  shr  si,cl  //  and shift the count that many times

asm  mov  cl,[ys]
asm  shr  di,cl

asm  mov  [x],si  // Store the values into the variables
asm  mov  [y],di

asm  pop  di
asm  pop  si
asm  popf    // Restore the registers

 *xp = x;
 *yp = y;
}

///////////////////////////////////////////////////////////////////////////
//
// INL_GetJoyDelta() - Returns the relative movement of the specified
//  joystick (from +/-127)
//
///////////////////////////////////////////////////////////////////////////
void INL_GetJoyDelta(word joy,int *dx,int *dy)
{
 word  x,y;
 longword time;
 JoystickDef *def;
static longword lasttime;

 IN_GetJoyAbs(joy,&x,&y);
 def = JoyDefs + joy;

 if (x < def->threshMinX)
 {
  if (x < def->joyMinX)
   x = def->joyMinX;

  x = -(x - def->threshMinX);
  x *= def->joyMultXL;
  x >>= JoyScaleShift;
  *dx = (x > 127)? -127 : -x;
 }
 else if (x > def->threshMaxX)
 {
  if (x > def->joyMaxX)
   x = def->joyMaxX;

  x = x - def->threshMaxX;
  x *= def->joyMultXH;
  x >>= JoyScaleShift;
  *dx = (x > 127)? 127 : x;
 }
 else
  *dx = 0;

 if (y < def->threshMinY)
 {
  if (y < def->joyMinY)
   y = def->joyMinY;

  y = -(y - def->threshMinY);
  y *= def->joyMultYL;
  y >>= JoyScaleShift;
  *dy = (y > 127)? -127 : -y;
 }
 else if (y > def->threshMaxY)
 {
  if (y > def->joyMaxY)
   y = def->joyMaxY;

  y = y - def->threshMaxY;
  y *= def->joyMultYH;
  y >>= JoyScaleShift;
  *dy = (y > 127)? 127 : y;
 }
 else
  *dy = 0;

 lasttime = TimeCount;
}

///////////////////////////////////////////////////////////////////////////
//
// INL_GetJoyButtons() - Returns the button status of the specified
//  joystick
//
///////////////////////////////////////////////////////////////////////////
static word
INL_GetJoyButtons(word joy)
{
register word result;

// Handle Notebook Gamepad's joystick.
//
 if (NGinstalled)
 {
  unsigned ax,bx;

  joy++;

  _AL=0x01;
  _BX=joy;
  NGjoy(0x00);

  result=_AL;
  result >>= joy? 6 : 4; // Shift into bits 0-1
  result &= 3;    // Mask off the useless bits
  result ^= 3;

  return(result);
 }

// Handle normal PC joystick.
//
 result = inportb(0x201); // Get all the joystick buttons
 result >>= joy? 6 : 4; // Shift into bits 0-1
 result &= 3;    // Mask off the useless bits
 result ^= 3;
 return(result);
}

///////////////////////////////////////////////////////////////////////////
//
// IN_GetJoyButtonsDB() - Returns the de-bounced button status of the
//  specified joystick
//
///////////////////////////////////////////////////////////////////////////
word
IN_GetJoyButtonsDB(word joy)
{
 longword lasttime;
 word  result1,result2;

 do
 {
  result1 = INL_GetJoyButtons(joy);
  lasttime = TimeCount;
  while (TimeCount == lasttime)
   ;
  result2 = INL_GetJoyButtons(joy);
 } while (result1 != result2);
 return(result1);
}


//
// INL_SetJoyScale() - Sets up scaling values for the specified joystick
//
static void
INL_SetJoyScale(word joy)
{
 JoystickDef *def;

 def = &JoyDefs[joy];
 def->joyMultXL = JoyScaleMax / (def->threshMinX - def->joyMinX);
 def->joyMultXH = JoyScaleMax / (def->joyMaxX - def->threshMaxX);
 def->joyMultYL = JoyScaleMax / (def->threshMinY - def->joyMinY);
 def->joyMultYH = JoyScaleMax / (def->joyMaxY - def->threshMaxY);
}

///////////////////////////////////////////////////////////////////////////
//
// IN_SetupJoy() - Sets up thresholding values and calls INL_SetJoyScale()
//  to set up scaling values
//
///////////////////////////////////////////////////////////////////////////
void
IN_SetupJoy(word joy,word minx,word maxx,word miny,word maxy)
{
 word  d,r;
 JoystickDef *def;

 def = &JoyDefs[joy];

 def->joyMinX = minx;
 def->joyMaxX = maxx;
 r = maxx - minx;
 d = r / 5;
 def->threshMinX = ((r / 2) - d) + minx;
 def->threshMaxX = ((r / 2) + d) + minx;

 def->joyMinY = miny;
 def->joyMaxY = maxy;
 r = maxy - miny;
 d = r / 5;
 def->threshMinY = ((r / 2) - d) + miny;
 def->threshMaxY = ((r / 2) + d) + miny;

 INL_SetJoyScale(joy);
}

///////////////////////////////////////////////////////////////////////////
//
// INL_ShutJoy() - Cleans up the joystick stuff
//
///////////////////////////////////////////////////////////////////////////
static void
INL_ShutJoy(word joy)
{
 JoysPresent[joy] = false;
}

#if 0
#if !FREE_FUNCTIONS || IN_DEVELOPMENT

///////////////////////////////////////////////////////////////////////////
//
// INL_StartJoy() - Detects & auto-configures the specified joystick
//     The auto-config assumes the joystick is centered
//
///////////////////////////////////////////////////////////////////////////
bool
INL_StartJoy(word joy)
{
 word  x,y;

 IN_GetJoyAbs(joy,&x,&y);

 if
 (
  ((x == 0) || (x > MaxJoyValue - 10))
 || ((y == 0) || (y > MaxJoyValue - 10))
 )
  return(false);
 else
 {
  IN_SetupJoy(joy,0,x * 2,0,y * 2);
  return(true);
 }
}
#endif
#endif
#endif

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
    else
    {
        switch (key)
        {
            case sc_KeyPadEnter: scan = sc_Enter; break;
            case sc_RShift: scan = sc_LShift; break;
            case sc_RAlt: scan = sc_LAlt; break;
            case sc_RControl: scan = sc_LControl; break;
        }
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
                else if (Keyboard[sc_Minus])
                {
                    viewsize--;

                    if (viewsize < 6)
                        viewsize = 6;

                    SetViewSize (viewsize);
                    DrawPlayScreen (false);
                }
                else if (Keyboard[sc_Equal])
                {
                    viewsize++;

                    if (viewsize > 20)
                        viewsize = 20;

                    SetViewSize (viewsize);
                    DrawPlayScreen (false);
                }
                else if (Keyboard[sc_Escape])
                    playstate = ex_abort;
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
//    bool checkjoys,checkNG;
//    int i;

    IN_ClearKeysDown ();
    IN_CenterMouse ();

    SDL_EventState (SDL_MOUSEMOTION,SDL_IGNORE);

    if (screen.flags & (SC_FULLSCREEN | SC_INPUTGRABBED))
        IN_SetWindowGrab (screen.window);

#ifdef NOTYET
    checkjoys = true;
    checkNG = false;

    if (checkNG)
    {
        #define WORD_CODE(c1,c2) ((c2)|(c1<<8))

        NGjoy(0xf0);
        if ((_AX==WORD_CODE('S','G')) && _BX)
            NGinstalled=true;
    }
#endif
    MousePresent = true;
#ifdef NOTYET
    for (i = 0;i < MaxJoys;i++)
        JoysPresent[i] = checkjoys ? INL_StartJoy(i) : false;
#endif
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
#ifdef NOTYET
    int i;

    for (i = 0;i < MaxJoys;i++)
        INL_ShutJoy(i);
#endif
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
    bool realdelta = false;
    word buttons;
    int  dx,dy;
    int  mx,my;

    dx = dy = 0;
    mx = my = 0;
    buttons = 0;

    IN_ProcessEvents ();

    ControlTypeUsed = ctrl_None;

    if (ControlTypeUsed == ctrl_None)
    {
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

        if (Keyboard[sc_Control])
            buttons += 1;
        if (Keyboard[sc_Alt])
            buttons += 2;

        realdelta = false;

        if (mx || my || buttons)
            ControlTypeUsed = ctrl_Keyboard;
    }
#ifdef NOTYET
    if (MousePresent && ControlTypeUsed == ctrl_None)
    {
        INL_GetMouseDelta(&dx,&dy);
        buttons = INL_GetMouseButtons();
        realdelta = true;

        if (dx || dy || buttons)
            ControlTypeUsed = ctrl_Mouse;
    }
#endif
#ifdef NOTYET
    if (JoystickCalibrated && ControlTypeUsed == ctrl_None)
    {
        INL_GetJoyDelta(ctrl_Joystick1 - ctrl_Joystick,&dx,&dy);
        buttons = INL_GetJoyButtons(ctrl_Joystick1 - ctrl_Joystick);
        realdelta = true;

        if (dx || dy || buttons)
            ControlTypeUsed = ctrl_Joystick;
    }
#endif
    if (realdelta)
    {
        mx = (dx < 0) ? -1 : ((dx > 0) ? 1 : 0);
        my = (dy < 0) ? 1 : ((dy > 0) ? 1 : 0);
    }
    else
    {
        dx = mx * 127;
        dy = my * 127;
    }

    info->x = dx;
    info->xaxis = mx;
    info->y = dy;
    info->yaxis = my;
    info->button0 = buttons & (1 << 0);
    info->button1 = buttons & (1 << 1);
    info->button2 = buttons & (1 << 2);
    info->button3 = buttons & (1 << 3);
    info->dir = DirTable[((my + 1) * 3) + (mx + 1)];
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

    IN_ClearKeysDown();
    memset (btnstate,0,sizeof(btnstate));
#ifdef NOTYET
    buttons = IN_JoyButtons () << 4;
#else
    buttons = 0;
#endif
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
#ifdef NOTYET
    buttons = IN_JoyButtons () << 4;
#else
    buttons = 0;
#endif
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
                    IN_WaitAndProcessEvents();
#ifdef NOTYET
                    buttons = IN_JoyButtons() << 4;
#else
                    buttons = 0;
#endif
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

bool IN_UserInput (longword delay)
{
    longword lasttime;

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


/*
===================
=
= IN_JoyButtons
=
===================
*/
#ifdef NOTYET
byte IN_JoyButtons (void)
{
    unsigned joybits;

    joybits = inportb(0x201); // Get all the joystick buttons
    joybits >>= 4;    // only the high bits are useful
    joybits ^= 15;    // return with 1=pressed

    return joybits;
}
#endif
