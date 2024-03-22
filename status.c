//
// basic statusbar & info area stuff
//

#include "3d_def.h"


#define MAX_DISPLAY_SCORE       (9999999L)
#define SCORE_ROLL_WAIT         (60 * 10)         // tics

//#define NO_STATUS_BAR

//
// ECG scroll rate (delay)
//
#define HEALTH_SCROLL_RATE      7
#define HEALTH_PULSE            70

//
// text "InfoArea" defines
//
#define INFOAREA_X				3
#define INFOAREA_Y				(200 - STATUSLINES + 3)
#define INFOAREA_W				109
#define INFOAREA_H				37

#define INFOAREA_BCOLOR			0x01
#define INFOAREA_CCOLOR    	    0x1A
#define INFOAREA_TCOLOR			0xA6
#define INFOAREA_TSHAD_COLOR	0x04			// Text Shadow Color

#define NUM_AMMO_SEGS               21

#define AMMO_SMALL_FONT_NUM_WIDTH   5

#define GRENADE_ENERGY_USE          4
#define BFG_ENERGY_USE              (GRENADE_ENERGY_USE << 1)


#define GD0 0x55
#define YD0 0x35
#define RD0 0x15

#define GD1 0x53
#define YD1 0x33
#define RD1 0x13

int DimAmmo[2][22] =
{
    {GD0,GD0,GD0,GD0,GD0,GD0,GD0,YD0,YD0,YD0,YD0,YD0,YD0,YD0,RD0,RD0,RD0,RD0,RD0,RD0,RD0,RD0},
    {GD1,GD1,GD1,GD1,GD1,GD1,GD1,YD1,YD1,YD1,YD1,YD1,YD1,YD1,RD1,RD1,RD1,RD1,RD1,RD1,RD1,RD1},
};

#define GL0 0x58
#define YL0 0x38
#define RL0 0x18

#define GL1 0x56
#define YL1 0x36
#define RL1 0x16

int LitAmmo[2][22] =
{
    {GL0,GL0,GL0,GL0,GL0,GL0,GL0,YL0,YL0,YL0,YL0,YL0,YL0,YL0,RL0,RL0,RL0,RL0,RL0,RL0,RL0,RL0},
    {GL1,GL1,GL1,GL1,GL1,GL1,GL1,YL1,YL1,YL1,YL1,YL1,YL1,YL1,RL1,RL1,RL1,RL1,RL1,RL1,RL1,RL1},
};


#define IA_MAX_LINE         30

typedef struct
{
    int16_t	x,y;
    int16_t	text_color;
    int16_t	backgr_color;
    int16_t left_margin;
    int8_t  delay;
    int8_t  numanims;
    int8_t  framecount;
} InfoArea_t;


unsigned    LastMsgPri;
int         MsgTicsRemain;
int         LastInfoAttacker;
int         LastInfoAttacker_Cloaked;
int         LastMsgType;
InfoArea_t  InfoAreaSetup;

int         DrawRadarGauge_COUNT = 3;
int         DrawAmmoNum_COUNT = 3;
int         DrawAmmoPic_COUNT = 3;
//int         DrawPDAmmoPic_COUNT = 3;
int         DrawScoreNum_COUNT = 3;
int         DrawWeaponPic_COUNT = 3;
int         DrawKeyPics_COUNT = 3;
int         DrawHealthNum_COUNT = 3;

int         DrawInfoArea_COUNT = 3;
int         InitInfoArea_COUNT = 3;
int         ClearInfoArea_COUNT = 3;

void    writeTokenStr (const char *str);

void    ShowOverheadChunk (void);
void    LoadOverheadChunk (int tpNum);
void    SaveOverheadChunk (int tpNum);
void    DisplayTeleportName (char tpNum, bool locked);

void    ForceUpdateStatusBar (void);

void    UpdateRadarGauge (void);
void    DrawLedStrip (int x, int y, int leds, int max);
void    DisplayPinballBonus (void);
void    CheckPinballBonus (int32_t points);
byte    LevelCompleted (void);

void    DrawWeapon (void);
void    GiveWeapon (int weapon);
void    GiveAmmo (int ammo);
void    DrawGAmmoNum (void);
void    DrawMAmmoNum (void);
void    DrawPDAmmoMsg (void);
void    ComputeAvailWeapons (void);

char    *HandleControlCodes (char *first_ch);

