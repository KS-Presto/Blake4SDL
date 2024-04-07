//
// ID Engine
// ID_SD.c - Sound Manager for Wolfenstein 3D
// v1.3
// By Jason Blochowiak
//
// Just using Wolf4SDL's sound manager for now
//

#include "3d_def.h"
#include "mame/fmopl.h"

#define ORIGSAMPLERATE    7042

typedef struct
{
    char     RIFF[4];
    uint32_t filelenminus8;
    char     WAVE[4];
    char     fmt_[4];
    uint32_t formatlen;
    uint16_t val0x0001;
    uint16_t channels;
    uint32_t samplerate;
    uint32_t bytespersec;
    uint16_t bytespersample;
    uint16_t bitspersample;
} headchunk;

typedef struct
{
    char     chunkid[4];
    uint32_t chunklength;
} wavechunk;

Mix_Chunk     *SoundChunks[LASTSOUND];

globalsound_t channelSoundPos[MIX_CHANNELS];

bool          SD_Started;
bool          SoundPositioned;
int           SoundMode;
int           MusicMode;
int           DigiMode;

byte          **SoundTable;
int           SoundNumber;
int           SoundPriority;
bool          nextsoundpos;
int           LeftPosition;
int           RightPosition;

int           DigiNumber;
int           DigiPriority;
int           NumDigi;
int16_t       DigiMap[LASTSOUND];
int8_t        *DigiChannel;
digiinfo      *DigiList;
bool          DigiPlaying;

//
// PC Sound variables
//
byte          pcLastSample;
byte          *pcSound;
uint32_t      pcLengthLeft;

//
// AdLib variables
//
byte          *alSound;
byte          alBlock;
uint32_t      alLengthLeft;
uint32_t      alTimeCount;
Instrument    alZeroInst;

//
// Sequencer variables
//
bool          sqActive;
word          *sqHack;
word          *sqHackPtr;
int           sqHackLen;
int           sqHackSeqLen;
uint32_t      sqHackTime;

//
// IMF player variables
//
int           soundTimeCounter = 5;
int           samplesPerMusicTick;
int           numreadysamples;
byte          *curAlSound;
byte          *curAlSoundPtr;
uint32_t      curAlLengthLeft;


const int     oplChip = 0;


/*
====================
=
= Delay
=
====================
*/

void Delay (int32_t wolfticks)
{
    if (wolfticks > 0)
        SDL_Delay ((wolfticks * 100) / 7);
}


/*
====================
=
= SD_SoundFinished
=
====================
*/

void SD_SoundFinished (void)
{
    SoundNumber = 0;
    SoundPriority = 0;
}


/*
====================
=
= SD_PCPlaySound
=
= Plays the specified sound on the PC speaker
=
====================
*/

void SD_PCPlaySound (PCSound *sound)
{
    pcLastSample = (byte)-1;    // TODO: why
    pcLengthLeft = sound->common.length;
    pcSound = sound->data;
}


/*
====================
=
= SD_PCStopSound
=
= Stops the current sound playing on the PC Speaker
=
====================
*/

void SD_PCStopSound (void)
{
    pcSound = NULL;
}


/*
====================
=
= SD_PCMixCallback
=
= Adapted from Chocolate Doom (chocolate-doom/pcsound/pcsound_sdl.c)
=
====================
*/

#define SQUARE_WAVE_AMP 0x2000

void SD_PCMixCallback (void *udata, Uint8 *stream, int len)
{
    static int current_remaining = 0;
    static int current_freq = 0;
    static int phase_offset = 0;

    Sint16 *leftptr;
    Sint16 *rightptr;
    Sint16 this_value;
    int    i;
    int    frac;
    int    nsamples;

    //
    // number of samples is quadrupled, because of 16-bit and stereo
    //
    nsamples = len / 4;

    leftptr = (Sint16 *)stream;
    rightptr = ((Sint16 *)stream) + 1;

    //
    // fill the output buffer
    //
    for (i = 0; i < nsamples; i++)
    {
        //
        // has this sound expired? If so, retrieve the next frequency
        //
        while (!current_remaining)
        {
            phase_offset = 0;

            //
            // get the next frequency to play
            //
            if (pcSound)
            {
                //
                // the PC speaker sample rate is 140Hz (see SDL_t0SlowAsmService)
                //
                current_remaining = param_samplerate / 140;

                if (*pcSound != pcLastSample)
                {
                    pcLastSample = *pcSound;

                    //
                    // The PC PIC counts down at 1.193180MHz
                    // So pwm_freq = counter_freq / reload_value
                    // reload_value = pcLastSample * 60 (see SDL_DoFX)
                    //
                    if (pcLastSample)
                        current_freq = 1193180 / (pcLastSample * 60);
                    else
                        current_freq = 0;
                }

                pcSound++;
                pcLengthLeft--;

                if (!pcLengthLeft)
                {
                    SD_PCStopSound ();
                    SD_SoundFinished ();
                }
            }
            else
            {
                current_freq = 0;
                current_remaining = 1;
            }
        }

        //
        // set the value for this sample.
        //
        if (!current_freq)
            this_value = 0;    // silence
        else
        {
            //
            // Determine whether we are at a peak or trough in the current
            // sound. Multiply by 2 so that frac % 2 will give 0 or 1
            // depending on whether we are at a peak or trough.
            //
            frac = (phase_offset * current_freq * 2) / param_samplerate;

            if (!(frac % 2))
                this_value = SQUARE_WAVE_AMP;
            else
                this_value = -SQUARE_WAVE_AMP;

            phase_offset++;
        }

        current_remaining--;

        //
        // use the same value for the left and right channels.
        //
        *leftptr += this_value;
        *rightptr += this_value;

        leftptr += 2;
        rightptr += 2;
    }
}


/*
====================
=
= SD_StopDigitized
=
====================
*/

void SD_StopDigitized (void)
{
    DigiPlaying = false;
    DigiNumber = 0;
    DigiPriority = 0;
    SoundPositioned = false;

    if (DigiMode == sds_PC && SoundMode == sdm_PC)
        SD_SoundFinished ();

    switch (DigiMode)
    {
        case sds_PC:
            SD_PCStopSound ();
            break;

        case sds_SoundBlaster:
            Mix_HaltChannel (-1);
            break;
    }
}


/*
====================
=
= SD_GetChannelForDigi
=
====================
*/

int SD_GetChannelForDigi (int which)
{
    int channel;

    if (DigiChannel[which] != -1)
        return DigiChannel[which];

    channel = Mix_GroupAvailable(1);

    if (channel == -1)
        channel = Mix_GroupOldest(1);

    if (channel == -1)               // all sounds stopped in the meantime?
        return Mix_GroupAvailable(1);

    return channel;
}


/*
====================
=
= SD_SetPosition
=
====================
*/

void SD_SetPosition (int channel, int leftpos, int rightpos)
{
    if (DigiMode != sds_SoundBlaster)
        return;

    if (leftpos < 0 || leftpos > 15 || rightpos < 0 || rightpos > 15 || (leftpos == 15 && rightpos == 15))
        Quit ("Illegal position!");

    Mix_SetPanning (channel,((15 - leftpos) << 4) + 15,((15 - rightpos) << 4) + 15);
}


/*
====================
=
= SD_GetSample
=
====================
*/

Sint16 SD_GetSample (float csample, byte *samples, int size)
{
    float   s0,s1,s2,sf;
    float   value;
    int     cursample;
    int32_t ivalue;

    cursample = (int)csample;
    sf = csample - (float)cursample;

    if (cursample - 1 >= 0)
        s0 = (float)(samples[cursample - 1] - 128);
    else
        s0 = 0;

    s1 = (float)(samples[cursample] - 128);

    if (cursample + 1 < size)
        s2 = (float)(samples[cursample + 1] - 128);
    else
        s2 = 0;

    //
    // TODO: lack of parentheses was a mess; this might be wrong
    //
    value = ((((s0 * sf) * (sf - 1)) / 2) - (s1 * (sf * sf - 1))) + (((s2 * (sf + 1)) * sf) / 2);
    ivalue = (int32_t)(value * 256);

    ivalue = MAX(-32768,MIN(ivalue,32767));

    return (Sint16)ivalue;
}


/*
====================
=
= SD_PrepareSound
=
====================
*/

void SD_PrepareSound (int which)
{
    int       i;
    int       page,size;
    int       buffersize;
    int       destsamples;
    Sint16    *newsamples;
    float     cursample;
    float     samplestep;
    byte      *origsamples;
    byte      *wavebuffer;
    //headchunk head;
    //wavechunk dhead;
    SDL_RWops *src;

    if (!DigiList)
        Quit ("DigiList not initialized!");

    page = DigiList[which].startpage;
    size = DigiList[which].length;

    origsamples = PM_GetSoundPage(page);

    if (origsamples + size >= PM_GetPageEnd())
        Quit ("Sound %d reaches out of page file!",which);

    destsamples = (int)(((float)size * (float)param_samplerate) / (float)ORIGSAMPLERATE);

    buffersize = sizeof(headchunk) + sizeof(wavechunk) + (destsamples * 2);    // dest are 16-bit samples

    wavebuffer = SafeMalloc(buffersize);

    headchunk head = { {'R','I','F','F'},0,{'W','A','V','E'},{'f','m','t',' '},0x10,0x0001,1,(uint32_t)param_samplerate,(uint32_t)(param_samplerate * 2),2,16 };
    wavechunk dhead = {{'d','a','t','a'},(uint32_t)(destsamples * 2)};
    head.filelenminus8 = sizeof(head) + destsamples * 2;
    memcpy (wavebuffer,&head,sizeof(head));
    memcpy (wavebuffer + sizeof(head),&dhead,sizeof(dhead));

    //
    // alignment is correct, as wavebuffer comes from malloc
    // and sizeof(headchunk) % 4 == 0 and sizeof(wavechunk) % 4 == 0
    //
    // TODO: it's still kinda sus; refactor this
    //
    newsamples = (Sint16 *)&wavebuffer[sizeof(headchunk) + sizeof(wavechunk)];
    cursample = 0.F;
    samplestep = (float)ORIGSAMPLERATE / (float)param_samplerate;

    for (i = 0; i < destsamples; i++)
    {
        newsamples[i] = SD_GetSample(((float)size * (float)i) / (float)destsamples,origsamples,size);

        cursample += samplestep;
    }

    src = SDL_RWFromMem(wavebuffer,buffersize);

    SoundChunks[which] = Mix_LoadWAV_RW(src,1);

    free (wavebuffer);
}


/*
====================
=
= SD_PlayDigitized
=
====================
*/

int SD_PlayDigitized (int which, int leftpos, int rightpos)
{
    int       channel;
    Mix_Chunk *sample;

    if (!DigiMode)
        return 0;

    if (which < 0 || which >= NumDigi)
        Quit ("Bad sound number %d!",which);

    channel = SD_GetChannelForDigi(which);

    SD_SetPosition (channel,leftpos,rightpos);

    DigiPlaying = true;

    sample = SoundChunks[which];

    if (!sample)
    {
        snprintf (str,sizeof(str),"SoundChunks[%d] is NULL!",which);
        Error (str);

        return 0;
    }

    if (Mix_PlayChannel(channel,sample,0) == -1)
    {
        snprintf (str,sizeof(str),"Unable to play sound: %s\n",Mix_GetError());
        Error (str);

        return 0;
    }

    return channel;
}


/*
====================
=
= SD_ChannelFinished
=
====================
*/

void SD_ChannelFinished (int channel)
{
    channelSoundPos[channel].valid = false;
}


/*
====================
=
= SD_SetDigiDevice
=
====================
*/

void SD_SetDigiDevice (int mode)
{
    if (mode == DigiMode)
        return;

    SD_StopDigitized ();

    DigiMode = mode;
}


/*
====================
=
= SD_SetupDigi
=
====================
*/

void SD_SetupDigi (void)
{
    int  i;
    int  size;
    int  page,lastpage;
    word *soundInfoPage;

    //
    // correct padding enforced by PM_Startup TODO: nasty!
    //
    soundInfoPage = (word *)PM_GetPage(ChunksInFile - 1);

    NumDigi = (word)PM_GetPageSize(ChunksInFile - 1) / 4;

    DigiList = SafeMalloc(NumDigi * sizeof(*DigiList));

    for (i = 0; i < NumDigi; i++)
    {
        //
        // calculate the size of the digi from the sizes of the pages between
        // the start page and the start page of the next sound
        //
        DigiList[i].startpage = soundInfoPage[i * 2];

        if ((int)DigiList[i].startpage >= ChunksInFile - 1)
        {
            NumDigi = i;
            break;
        }

        if (i < NumDigi - 1)
        {
            lastpage = soundInfoPage[(i * 2) + 2];

            if (!lastpage || lastpage + PMSoundStart > ChunksInFile - 1)
                lastpage = ChunksInFile - 1;
            else
                lastpage += PMSoundStart;
        }
        else
            lastpage = ChunksInFile - 1;

        size = 0;

        for (page = PMSoundStart + DigiList[i].startpage; page < lastpage; page++)
            size += PM_GetPageSize(page);

        //
        // don't include padding of sound info page if padding was added
        //
        if (lastpage == ChunksInFile - 1 && PMSoundInfoPagePadded)
            size--;

        //
        // Patch lower 16-bit of size with size from sound info page.
        // The original VSWAP contains padding which is included in the page size,
        // but not included in the 16-bit size. So we use the more precise value.
        //
        if ((size & 0xffff0000) != 0 && (size & 0xffff) < soundInfoPage[(i * 2) + 1])
            size -= 0x10000;

        size = (size & 0xffff0000) | soundInfoPage[(i * 2) + 1];

        DigiList[i].length = size;
    }

    DigiChannel = SafeMalloc(NumDigi * sizeof(*DigiChannel));

    for (i = 0; i < LASTSOUND; i++)
        DigiMap[i] = -1;

    for (i = 0; i < NumDigi; i++)
        DigiChannel[i] = -1;
}


/*
==========================================================

                            ADLIB CODE

==========================================================
*/


/*
====================
=
= SD_ALStopSound
=
= Turns off any sound effects playing through the AdLib card
=
====================
*/

void SD_ALStopSound (void)
{
    alSound = NULL;
    alOut (alFreqH,0);
}


/*
====================
=
= SD_AlSetFXInst
=
====================
*/

void SD_AlSetFXInst (Instrument *inst)
{
    byte c,m;

    m = 0;      // modulator cell for channel 0
    c = 3;      // carrier cell for channel 0

    alOut (m + alChar,inst->mChar);
    alOut (m + alScale,inst->mScale);
    alOut (m + alAttack,inst->mAttack);
    alOut (m + alSus,inst->mSus);
    alOut (m + alWave,inst->mWave);
    alOut (c + alChar,inst->cChar);
    alOut (c + alScale,inst->cScale);
    alOut (c + alAttack,inst->cAttack);
    alOut (c + alSus,inst->cSus);
    alOut (c + alWave,inst->cWave);

    alOut (alFeedCon,0);
}


/*
====================
=
= SD_ALPlaySound
=
= Plays the specified sound on the AdLib card
=
====================
*/

void SD_ALPlaySound (AdLibSound *sound)
{
    Instrument *inst;
    byte       *data;

    SD_ALStopSound ();

    alLengthLeft = sound->common.length;
    data = sound->data;
    alBlock = ((sound->block & 7) << 2) | 0x20;
    inst = &sound->inst;

    if (!(inst->mSus | inst->cSus))
        Quit ("Bad instrument!");

    SD_AlSetFXInst(inst);
    alSound = (byte *)data;
}


/*
====================
=
= SD_ShutAL
=
= Shuts down the AdLib card for sound effects
=
====================
*/

void SD_ShutAL (void)
{
    // TODO: is it safe to use SD_ALStopSound here? alOut(alEffects,0); might break it...
    alSound = 0;
    alOut (alEffects,0);
    alOut (alFreqH,0);
    SD_AlSetFXInst (&alZeroInst);
}


/*
====================
=
= SD_StartAL
=
= Starts up the AdLib card for sound effects
=
====================
*/

void SD_StartAL (void)
{
    alOut (alEffects,0);
    SD_AlSetFXInst (&alZeroInst);
}


/*
====================
=
= SD_ShutDevice
=
= Turns off whatever device was being used for sound fx
=
====================
*/

void SD_ShutDevice (void)
{
    switch (SoundMode)
    {
        case sdm_Off:
            return;

        case sdm_PC:
            SD_PCStopSound ();
            break;

        case sdm_AdLib:
            SD_ShutAL ();
            break;
    }

    SoundMode = sdm_Off;
}


/*
====================
=
= SD_StartDevice
=
= Turns on whatever device is to be used for sound fx
=
====================
*/

void SD_StartDevice (void)
{
    if (SoundMode == sdm_AdLib)
        SD_StartAL ();

    SD_SoundFinished ();
}


/*
====================
=
= SD_SetSoundMode
=
= Sets which sound hardware to use for sound effects
=
====================
*/

void SD_SetSoundMode (int mode)
{
    unsigned tableoffset;

    SD_StopSound ();

    switch (mode)
    {
        case sdm_Off:
            tableoffset = STARTADLIBSOUNDS;
            break;

        case sdm_PC:
            tableoffset = STARTPCSOUNDS;
            break;

        case sdm_AdLib:
            tableoffset = STARTADLIBSOUNDS;
            break;

        default:
            Quit ("Invalid sound mode %d",mode);
    }

    SoundTable = &audiosegs[tableoffset];

    if (mode != SoundMode)
    {
        SD_ShutDevice ();

        SoundMode = mode;

        SD_StartDevice ();
    }
}


/*
====================
=
= SD_SetMusicMode
=
= Sets the device to use for background music
=
====================
*/

void SD_SetMusicMode (int mode)
{
    SD_FadeOutMusic ();

    while (SD_MusicPlaying())
        SDL_Delay (5);

    MusicMode = mode;
}


/*
====================
=
= SD_IMFMusicPlayer
=
====================
*/

void SD_IMFMusicPlayer (void *udata, Uint8 *stream, int len)
{
    int     stereolen = len >> 1;
    int     sampleslen = stereolen >> 1;
    int16_t *stream16 = (int16_t *)stream;    // expect correct alignment TODO: this is nasty

    while (1)
    {
        if (numreadysamples)
        {
            if (numreadysamples < sampleslen)
            {
                YM3812UpdateOne (oplChip,stream16,numreadysamples);

                stream16 += numreadysamples * 2;
                sampleslen -= numreadysamples;
            }
            else
            {
                YM3812UpdateOne (oplChip,stream16,sampleslen);

                numreadysamples -= sampleslen;
                return;
            }
        }

        soundTimeCounter--;

        if (!soundTimeCounter)
        {
            soundTimeCounter = 5;

            if (curAlSound != alSound)
            {
                curAlSound = curAlSoundPtr = alSound;
                curAlLengthLeft = alLengthLeft;
            }

            if (curAlSound)
            {
                if (*curAlSoundPtr)
                {
                    alOut (alFreqL,*curAlSoundPtr);
                    alOut (alFreqH,alBlock);
                }
                else
                    alOut(alFreqH,0);

                curAlSoundPtr++;
                curAlLengthLeft--;

                if (!curAlLengthLeft)
                {
                    curAlSound = alSound = NULL;

                    SD_SoundFinished ();
                    alOut(alFreqH,0);
                }
            }
        }

        if (sqActive)
        {
            do
            {
                if (sqHackTime > alTimeCount)
                    break;

                sqHackTime = alTimeCount + *(sqHackPtr + 1);

                alOut (*(byte *)sqHackPtr,*(((byte *)sqHackPtr) + 1));

                sqHackPtr += 2;
                sqHackLen -= 4;

            } while (sqHackLen > 0);

            alTimeCount++;

            if (!sqHackLen)
            {
                sqHackPtr = sqHack;
                sqHackLen = sqHackSeqLen;
                sqHackTime = 0;
                alTimeCount = 0;
            }
        }

        numreadysamples = samplesPerMusicTick;
    }
}


/*
====================
=
= SD_Startup
=
= Starts up the Sound Mgr
=
====================
*/

void SD_Startup (void)
{
    int i;

    if (SD_Started)
        return;

    if (Mix_OpenAudio(param_samplerate,AUDIO_S16,2,param_audiobuffer))
    {
        snprintf (str,sizeof(str),"Unable to open audio: %s\n",Mix_GetError());
        Error (str);
        return;
    }

    //
    // reserve player & boss weapon channels and group remaining channels
    //
    Mix_ReserveChannels (2);
    Mix_GroupChannels (2,MIX_CHANNELS - 1,1);

    //
    // init music
    //
    samplesPerMusicTick = param_samplerate / 700;    // SDL_t0FastAsmService played at 700Hz

    if (YM3812Init(1,3579545,param_samplerate))
        Error ("Unable to create virtual OPL!\n");

    for (i = 1; i < 0xf6; i++)
        YM3812Write (oplChip,i,0);

    YM3812Write (oplChip,1,0x20);    // Set WSE = 1

    Mix_HookMusic (SD_IMFMusicPlayer,0);
    Mix_ChannelFinished (SD_ChannelFinished);

    alTimeCount = 0;

    //
    // add PC speaker sound mixer
    //
    Mix_SetPostMix (SD_PCMixCallback,NULL);

    SD_SetSoundMode (sdm_Off);
    SD_SetMusicMode (smm_Off);

    SD_SetupDigi ();

    if (savedsoundmode == -1)
        savedsoundmode = sdm_AdLib;

    if (savedmusicmode == -1)
        savedmusicmode = smm_AdLib;

    if (saveddigimode == -1)
        saveddigimode = sds_SoundBlaster;

    SD_SetMusicMode (savedmusicmode);
    SD_SetSoundMode (savedsoundmode);
    SD_SetDigiDevice (saveddigimode);

    SD_Started = true;
}


/*
====================
=
= SD_Shutdown
=
= Shuts down the Sound Mgr
=
====================
*/

void SD_Shutdown (void)
{
    int i;

    if (!SD_Started)
        return;

    SD_MusicOff ();
    SD_StopSound ();

    for (i = 0; i < STARTMUSIC - STARTDIGISOUNDS; i++)
    {
        if (SoundChunks[i])
            Mix_FreeChunk (SoundChunks[i]);
    }

    free (DigiChannel);
    DigiChannel = NULL;

    free (DigiList);
    DigiList = NULL;

    SD_Started = false;
}


/*
====================
=
= SD_PositionSound
=
= Sets up a stereo imaging location for the next
= sound to be played. Each channel ranges from 0 to 15.
=
====================
*/

void SD_PositionSound (int leftvol, int rightvol)
{
    LeftPosition = leftvol;
    RightPosition = rightvol;
    nextsoundpos = true;
}


/*
====================
=
= SD_PlaySound
=
= Plays the specified sound on the appropriate hardware
=
====================
*/

int SD_PlaySound (int sound)
{
    bool        ispos;
    SoundCommon *s;
    int         lp,rp;
    int         channel;

    lp = LeftPosition;
    rp = RightPosition;
    LeftPosition = 0;
    RightPosition = 0;

    ispos = nextsoundpos;
    nextsoundpos = false;

    if (sound == -1 || (DigiMode == sds_Off && SoundMode == sdm_Off))
        return 0;

    s = (SoundCommon *)SoundTable[sound];

    if (SoundMode != sdm_Off && !s)
        Quit ("Uncached sound!");

    if (DigiMode != sds_Off && DigiMap[sound] != -1)
    {
        if (DigiMode == sds_PC && SoundMode == sdm_PC)
        {
            if (s->priority < SoundPriority)
                return 0;

            SD_PCStopSound ();
            SD_PlayDigitized (DigiMap[sound],lp,rp);

            SoundPositioned = ispos;
            SoundNumber = sound;
            SoundPriority = s->priority;
        }
        else
        {
#ifdef NOTYET
            if (s->priority < DigiPriority)
                return 0;
#endif
            channel = SD_PlayDigitized(DigiMap[sound],lp,rp);

            SoundPositioned = ispos;
            DigiNumber = sound;
            DigiPriority = s->priority;

            return channel + 1;
        }

        return 1;
    }

    if (SoundMode == sdm_Off)
        return 0;

    if (!s->length)
        Quit("Zero length sound!");

    if (s->priority < SoundPriority)
        return 0;

    switch (SoundMode)
    {
        case sdm_PC:
            SD_PCPlaySound ((PCSound *)s);
            break;

        case sdm_AdLib:
            curAlSound = alSound = NULL;
            alOut (alFreqH, 0);

            SD_ALPlaySound ((AdLibSound *)s);
            break;

        default:
            break;
    }

    SoundNumber = sound;
    SoundPriority = s->priority;

    return 0;
}


/*
====================
=
= SD_SoundPlaying
=
= Returns the sound number that's playing, or 0 if
= no sound is playing
=
====================
*/

int SD_SoundPlaying (void)
{
    bool result = false;

    switch (SoundMode)
    {
        case sdm_PC:
            result = pcSound ? true : false;
            break;

        case sdm_AdLib:
            result = alSound ? true : false;
            break;

        default:
            break;
    }

    if (result)
        return SoundNumber;
    else
        return 0;
}


/*
====================
=
= SD_StopSound
=
= If a sound is playing, stops it
=
====================
*/

void SD_StopSound (void)
{
    if (DigiPlaying)
        SD_StopDigitized ();

    switch (SoundMode)
    {
        case sdm_PC:
            SD_PCStopSound ();
            break;

        case sdm_AdLib:
            SD_ALStopSound ();
            break;

        default:
            break;
    }

    SoundPositioned = false;

    SD_SoundFinished ();
}


/*
====================
=
= SD_WaitSoundDone
=
= Waits until the current sound is done playing
=
====================
*/

void SD_WaitSoundDone (void)
{
    while (SD_SoundPlaying())
        SDL_Delay (5);
}


/*
====================
=
= SD_MusicOn
=
= Turns on the sequencer
=
====================
*/

void SD_MusicOn (void)
{
    sqActive = true;
}


/*
====================
=
= SD_MusicOff
=
= Turns off the sequencer and any playing notes
=
= Returns the last music offset for ContinueMusic
=
====================
*/

int SD_MusicOff (void)
{
    int i;

    sqActive = false;

    if (MusicMode == smm_AdLib)
    {
        alOut (alEffects,0);

        for (i = 0; i < sqMaxTracks; i++)
            alOut (alFreqH + i + 1,0);
    }

    return (int)(sqHackPtr - sqHack);
}


/*
====================
=
= SD_StartMusic
=
= Starts playing the given music chunk
=
====================
*/

void SD_StartMusic (int chunk)
{
    int32_t length;

    SD_MusicOff ();

    if (MusicMode == smm_AdLib)
    {
        length = CA_CacheAudioChunk(chunk);

        sqHack = (word *)audiosegs[chunk];     // alignment is correct TODO: nasty!

        if (!*sqHack)
            sqHackLen = sqHackSeqLen = length;
        else
            sqHackLen = sqHackSeqLen = *sqHack++;

        sqHackPtr = sqHack;
        sqHackTime = 0;
        alTimeCount = 0;

        SD_MusicOn ();
    }
}


/*
====================
=
= SD_ContinueMusic
=
= Continues playing the given music chunk at the given offset
=
====================
*/

void SD_ContinueMusic (int chunk, int startoffs)
{
    int     i;
    int32_t length;
    byte    reg,val;

    SD_MusicOff ();

    if (MusicMode == smm_AdLib)
    {
        length = CA_CacheAudioChunk(chunk);

        sqHack = (word *)audiosegs[chunk];     // alignment is correct TODO: nasty!

        if (!*sqHack)
            sqHackLen = sqHackSeqLen = length;
        else
            sqHackLen = sqHackSeqLen = *sqHack++;

        if (startoffs >= sqHackLen)
        {
#ifdef ADDEDFIX // 7                     // Andy, improved by Chris Chokan
            startoffs = 0;
#else
            Quit ("Illegal start offset provided!");
#endif
        }

        //
        // fast forward to correct position
        // (needed to reconstruct the instruments)
        //
        sqHackPtr = sqHack;

        for (i = 0; i < startoffs; i += 2)
        {
            reg = *(byte *)sqHackPtr;
            val = *(((byte *)sqHackPtr) + 1);

            if (reg >= 0xb1 && reg <= 0xb8)
                val &= 0xdf;                // disable play note flag
            else if (reg == 0xbd)
                val &= 0xe0;                // disable drum flags

            alOut (reg,val);
            sqHackPtr += 2;
            sqHackLen -= 4;
        }

        sqHackTime = 0;
        alTimeCount = 0;

        SD_MusicOn ();
    }
}


/*
====================
=
= SD_FadeOutMusic
=
= Starts fading out the music. Call SD_MusicPlaying
= to see if the fadeout is complete
=
====================
*/

void SD_FadeOutMusic (void)
{
    if (MusicMode == smm_AdLib)
        SD_MusicOff ();    // DEBUG - quick hack to turn the music off
}


/*
====================
=
= SD_MusicPlaying
=
= Returns true if music is currently playing
=
====================
*/

bool SD_MusicPlaying (void)
{
    if (MusicMode == smm_AdLib)
        return sqActive;

    return false;
}
