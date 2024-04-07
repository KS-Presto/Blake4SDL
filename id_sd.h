//
// ID Engine
// ID_SD.h - Sound Manager Header
// By Jason Blochowiak
//

#ifndef _ID_SD_H_
#define _ID_SD_H_


#define alOut(n,b)      YM3812Write(oplChip,n,b)

#define TickBase        70      // 70Hz per tick - used as a base for timer 0

enum SDMode
{
    sdm_Off,
    sdm_PC,
    sdm_AdLib,
};

enum SMMode
{
    smm_Off,
    smm_AdLib,
};

enum SDSMode
{
    sds_Off,
    sds_PC,
    sds_SoundBlaster,
};


typedef struct
{
    uint32_t        length;
    uint16_t        priority;
} SoundCommon;


//
// PC Sound stuff
//
#define pcTimer         0x42
#define pcTAccess       0x43
#define pcSpeaker       0x61

#define pcSpkBits       3

typedef struct
{
    SoundCommon     common;
    byte            data[1];
} PCSound;

//
// Sequencing stuff
//
#define sqMaxTracks     10

//
// Register addresses
// Operator stuff
//
#define alChar          0x20
#define alScale         0x40
#define alAttack        0x60
#define alSus           0x80
#define alWave          0xe0

//
// Channel stuff
//
#define alFreqL         0xa0
#define alFreqH         0xb0
#define alFeedCon       0xc0

#define alEffects       0xbd

typedef struct
{
    byte    mChar,cChar;
    byte    mScale,cScale;
    byte    mAttack,cAttack;
    byte    mSus,cSus;
    byte    mWave,cWave;
    byte    nConn;

            // These are only for Muse - these bytes are really unused
    byte    voice;
    byte    mode;
    byte    unused[3];
} Instrument;


typedef struct
{
    SoundCommon     common;
    Instrument      inst;
    byte            block;
    byte            data[1];
} AdLibSound;


typedef struct
{
    bool  valid;
    fixed x,y;
} globalsound_t;

typedef struct
{
    uint32_t startpage;
    uint32_t length;
} digiinfo;

extern  globalsound_t channelSoundPos[MIX_CHANNELS];

extern  bool          sqActive;

extern  bool          SoundPositioned;
extern  int           SoundMode;
extern  int           DigiMode;
extern  int           MusicMode;
extern  int           NumDigi;
extern  int16_t       DigiMap[LASTSOUND];
extern  int8_t        *DigiChannel;
extern  digiinfo      *DigiList;

#define GetTimeCount()  ((SDL_GetTicks() * 7) / 100)

void    Delay (int32_t wolfticks);

void    SD_Startup (void);
void    SD_Shutdown (void);

int     SD_GetChannelForDigi (int which);
void    SD_PositionSound (int leftvol, int rightvol);
int     SD_PlaySound (int sound);
void    SD_SetPosition (int channel, int leftvol, int rightvol);
void    SD_StopSound (void);
void    SD_WaitSoundDone (void);

void    SD_StartMusic (int chunk);
void    SD_ContinueMusic (int chunk, int startoffs);
void    SD_MusicOn (void);
void    SD_FadeOutMusic (void);
int     SD_MusicOff (void);
bool    SD_MusicPlaying (void);
void    SD_SetSoundMode (int mode);
void    SD_SetMusicMode (int mode);
int     SD_SoundPlaying (void);

void    SD_SetDigiDevice (int mode);
void    SD_PrepareSound (int which);
int     SD_PlayDigitized (int which, int leftpos, int rightpos);
void    SD_StopDigitized (void);

#endif
