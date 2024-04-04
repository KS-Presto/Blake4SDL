// 3D_MAIN.C

#include "3d_def.h"


/*
=============================================================================

                        BLAKE STONE
            (C)opyright 1993, JAM Productions, Inc.

            3D engine licensed by ID Software, Inc.
        Shareware distribution by Apogee Software, Inc.

=============================================================================
*/

#define MAX_DEST_PATH_LEN       30
#define LZH_WORK_BUFFER_SIZE    8192


void    *lzh_work_buffer;
int32_t checksum;

int     starting_episode,starting_level,starting_difficulty;

char    destPath[MAX_DEST_PATH_LEN + 1];
char    tempPath[MAX_DEST_PATH_LEN + 15];

char    str[256],error[256];
char    configname[13] = "CONFIG.";

int     mouseadjustment;

bool    ForceLoadDefault;
bool    ShowQuickMsg;

#ifdef IN_DEVELOPMENT
int     democount,jim;
#endif
bool    param_demorecord;
bool    param_windowed;
int     param_samplerate;
int     param_audiobuffer;
int     param_joystickindex;
int     param_joystickhat = -1;

//
// TODO: Aliens of Gold, huh?
//
const char *SavegameInfoText =  "\n\r"
                                "\n\r"
                                "-------------------------------------\n\r"
                                "    Blake Stone: Aliens Of Gold\n\r"
                                "Copyright 1993, JAM Productions, Inc.\n\r"
                                "\n\r"
                                "SAVEGAME file is from version: " _VERSION_"\n\r"
                                " Compile Date :" __DATE__" : " __TIME__"\n\r"
                                "-------------------------------------\n\r"
                                "\x1a";

void    DrawCreditsPage (void);
void    ShowPromo (void);

bool    LevelInPlaytemp (int levelnum);


/*
====================
=
= WriteConfig
=
====================
*/

void WriteConfig (void)
{
    FILE *file;
    char fname[13];

    snprintf (fname,sizeof(fname),"%s%s",configname,extension);

    file = fopen(fname,"wb");

    if (file)
    {
        fwrite (Scores,sizeof(Scores),1,file);

        fwrite (&SoundMode,sizeof(SoundMode),1,file);
        fwrite (&MusicMode,sizeof(MusicMode),1,file);
        fwrite (&DigiMode,sizeof(DigiMode),1,file);

        fwrite (&mouseenabled,sizeof(mouseenabled),1,file);
        fwrite (&joystickenabled,sizeof(joystickenabled),1,file);

        fwrite (&dirscan,sizeof(dirscan),1,file);
        fwrite (&buttonscan,sizeof(buttonscan),1,file);
        fwrite (&buttonmouse,sizeof(buttonmouse),1,file);
        fwrite (&buttonjoy,sizeof(buttonjoy),1,file);

        fwrite (&viewsize,sizeof(viewsize),1,file);
        fwrite (&mouseadjustment,sizeof(mouseadjustment),1,file);

        fwrite (&gamestate.flags,sizeof(gamestate.flags),1,file);

        fclose (file);
    }
}


/*
====================
=
= ReadConfig
=
====================
*/

void ReadConfig (void)
{
    FILE     *file;
    char     fname[13];
    int      sd,sm,sds;
    bool     configfound = false;
    unsigned flags = gamestate.flags;

    snprintf (fname,sizeof(fname),"%s%s",configname,extension);

    file = fopen(fname,"rb");

    if (file)
    {
        //
        // valid config file
        //
        fread (Scores,sizeof(Scores),1,file);

        fread (&sd,sizeof(sd),1,file);
        fread (&sm,sizeof(sm),1,file);
        fread (&sds,sizeof(sds),1,file);

        fread (&mouseenabled,sizeof(mouseenabled),1,file);
        fread (&joystickenabled,sizeof(joystickenabled),1,file);

        fread (&dirscan,sizeof(dirscan),1,file);
        fread (&buttonscan,sizeof(buttonscan),1,file);
        fread (&buttonmouse,sizeof(buttonmouse),1,file);
        fread (&buttonjoy,sizeof(buttonjoy),1,file);

        fread (&viewsize,sizeof(viewsize),1,file);
        fread (&mouseadjustment,sizeof(mouseadjustment),1,file);

        fread (&flags,sizeof(flags),1,file);    // use temp so we don't destroy pre-sets

        flags &= GS_HEARTB_SOUND | GS_ATTACK_INFOAREA | GS_LIGHTING | GS_DRAW_CEILING | GS_DRAW_FLOOR;

        gamestate.flags |= flags;               // must "OR", some flags are already set

        fclose (file);

        if (sd == sdm_AdLib && (!AdLibPresent || !SoundBlasterPresent))
        {
            sd = sdm_PC;
            sd = smm_Off;
        }

        if (sds == sds_SoundBlaster && !SoundBlasterPresent)
            sds = sds_Off;

        if (!MousePresent)
            mouseenabled = false;
        if (!JoystickPresent)
            joystickenabled = false;

        MainMenu[6].active = AT_ENABLED;
        MainItems.curpos = 0;

        configfound = true;
    }

    if (!configfound || !viewsize)
    {
        //
        // no config file, so select by hardware
        //
        if (SoundBlasterPresent || AdLibPresent)
        {
            sd = sdm_AdLib;
            sm = smm_AdLib;
        }
        else
        {
            sd = sdm_PC;
            sm = smm_Off;
        }

        if (SoundBlasterPresent)
            sds = sds_SoundBlaster;
        else
            sds = sds_Off;

        if (MousePresent)
            mouseenabled = true;

        if (JoystickPresent)
            joystickenabled = true;

        viewsize = 17;
        mouseadjustment = 5;
        gamestate.flags |= GS_HEARTB_SOUND | GS_ATTACK_INFOAREA;

#ifdef CEILING_FLOOR_COLORS
        gamestate.flags |= GS_DRAW_CEILING | GS_DRAW_FLOOR | GS_LIGHTING;
#else
        gamestate.flags |= GS_LIGHTING;
#endif
    }

    SD_SetMusicMode (sm);
    SD_SetSoundMode (sd);
    SD_SetDigiDevice (sds);
}


/*
============================================================================

               'LOAD/SAVE game' and 'LOAD/SAVE level' code

============================================================================
*/


/*
====================
=
= InitPlaytemp
=
====================
*/

void InitPlaytemp (void)
{
    FILE *file;

    MakeDestPath (PLAYTEMP_FILE);

    file = fopen(tempPath,"wb");

    if (!file)
        CA_CannotOpen (tempPath);

    fclose (file);
}


/*
====================
=
= DoChecksum
=
====================
*/

int32_t DoChecksum (void *source, uint32_t size, int32_t checksum)
{
    uint32_t i;
    byte     *src;

    src = source;

    for (i = 0; i < size - 1; i++)
        checksum += src[i] ^ src[i + 1];

    return checksum;
}


/*
====================
=
= FindChunk
=
====================
*/

int32_t FindChunk (FILE *file, const char *chunk)
{
    int32_t chunklen;
    char    fchunk[5] = {0,0,0,0,0};

    while (1)
    {
        //
        // read chunk id
        //
        if (fread(fchunk,1,sizeof(fchunk) - 1,file) != sizeof(fchunk) - 1)
            break;

        fread (&chunklen,sizeof(chunklen),1,file);

        //
        // look for chunk (sub-check!)
        //
        if (strstr(fchunk,chunk))
            return chunklen;      // chunk found!

        fseek (file,chunklen,SEEK_CUR);
    }

    fseek (file,0,SEEK_END);      // make sure we're at the end

    return 0;
}


/*
====================
=
= NextChunk
=
====================
*/

int32_t NextChunk (FILE *file)
{
    int32_t chunklen;
    char    fchunk[5] = {0,0,0,0,0};

    //
    // read chunk id
    //
    if (fread(fchunk,1,sizeof(fchunk) - 1,file) != sizeof(fchunk) - 1)
    {
        fseek (file,0,SEEK_END);    // make sure we're at the end

        return 0;
    }

    fread (&chunklen,sizeof(chunklen),1,file);

    return chunklen;
}


/*
====================
=
= DeleteChunk
=
====================
*/

int32_t DeleteChunk (FILE *sourcefile, const char *chunk)
{
    int32_t filesize,cksize,offset,bmove;
    FILE    *destfile;

    filesize = CA_GetFileLength(sourcefile);

    cksize = FindChunk(sourcefile,chunk);

    if (cksize)
    {
        //
        // skip over the header
        //
        offset = ftell(sourcefile) - 8;
        bmove = filesize - (offset + 8 + cksize);       // figure bytes to move

        if (bmove)
        {
            //
            // move data: FROM --> the start of NEXT chunk through the end of file
            //              TO --> the start of THIS chunk
            //
            // (ie: erase THIS chunk and re-write it at the end of the file!)
            //
            fseek (sourcefile,cksize,SEEK_CUR);         // seek source to NEXT chunk

            MakeDestPath (PLAYTEMP_FILE);

            destfile = fopen(tempPath,"rb+");

            if (!destfile)
                CA_CannotOpen (tempPath);

            fseek (destfile,offset,SEEK_SET);           // seek dest to THIS chunk

            IO_CopyFile (sourcefile,destfile,bmove);

            fclose (destfile);

            fseek (sourcefile,offset + bmove,SEEK_SET); // go to end of data moved
        }
        else
            fseek (sourcefile,offset,SEEK_SET);
    }

    return cksize;
}


/*
====================
=
= ReadInfo
=
====================
*/

void ReadInfo (bool decompress, void *dest, uint32_t size, FILE *file)
{
    uint32_t dsize,csize;

    LoadLevelUpdate (LS_current++,LS_total);

    if (decompress)
    {
        if (!fread(&csize,sizeof(csize),1,file))
            Quit ("Error reading file: %s",strerror(errno));

        if (!fread(lzh_work_buffer,csize,1,file))
            Quit ("Error reading file: %s",strerror(errno));

        checksum = DoChecksum(lzh_work_buffer,csize,checksum);
        dsize = LZH_Decompress(lzh_work_buffer,dest,size,csize);

        if (dsize != size)
            Quit ("Bad decompression during game load: chunk size is %d, expected size is %d",dsize,size);
    }
    else
    {
        if (!fread(dest,size,1,file))
            Quit ("Error reading file: %s",strerror(errno));

        checksum = DoChecksum(dest,size,checksum);
    }
}


/*
====================
=
= WriteInfo
=
====================
*/

uint32_t WriteInfo (bool compress, void *src, uint32_t size, FILE *file)
{
    uint32_t csize;

    LoadLevelUpdate (LS_current++,LS_total);

    if (compress)
    {
        csize = LZH_Compress(src,lzh_work_buffer,size);

        if (csize > LZH_WORK_BUFFER_SIZE)
            Quit ("Save game compression buffer too small!");

        if (!fwrite(&csize,sizeof(csize),1,file))
            Quit ("Error writing file: %s",strerror(errno));

        if (!fwrite(lzh_work_buffer,csize,1,file))
            Quit ("Error writing file: %s",strerror(errno));

        checksum = DoChecksum(lzh_work_buffer,csize,checksum);
        csize += sizeof(csize);
    }
    else
    {
        if (!fwrite(src,size,1,file))
            Quit ("Error writing file: %s",strerror(errno));

        checksum = DoChecksum(src,size,checksum);
        csize = size;
    }

    return csize;
}


/*
====================
=
= LoadLevel
=
====================
*/

void LoadLevel (int levelnum)
{
    bool      oldloaded = loadedgame;
    int32_t   oldchecksum,size;
    objtype   *newobj,*obj;
    FILE      *file;
    int       x,y;
    int       mod;
    int       oldwx,oldwy,oldww,oldwh;
    int       oldpx,oldpy;
    word      actornum,laststatobjnum;
    word      *maptable;
    unsigned  count;
    byte      *temp,*ptr;
    char      chunk[5];

    WindowY = 181;
    gamestuff.level[levelnum].locked = false;

    mod = gamestate.mapon % SHADEDIV;
    normalshadediv = nsd_table[mod];
    shademax = sm_table[mod];
    normalshade = (((viewwidth * 9) >> 4) - 3) / normalshadediv;

    //
    // open PLAYTEMP file
    //
    MakeDestPath (PLAYTEMP_FILE);

    file = fopen(tempPath,"rb");

    //
    // if level exists in PLAYTEMP file, use it; otherwise, load it from scratch!
    //
    snprintf (chunk,sizeof(chunk),"LV%02x",levelnum);

    if (!file || !FindChunk(file,chunk) || ForceLoadDefault)
    {
        fclose (file);

        LoadLevelUpdate (LS_current + ((LS_total - LS_current) >> 1),LS_total);

        SetupGameLevel ();

        gamestate.flags |= GS_VIRGIN_LEVEL;
        gamestate.turn_around = 0;

        LoadLevelUpdate (1,1);

        ForceLoadDefault = false;

        return;
    }

    gamestate.flags &= ~GS_VIRGIN_LEVEL;

    //
    // setup for LZH decompression
    //
    LZH_Startup ();
    lzh_work_buffer = SafeMalloc(LZH_WORK_BUFFER_SIZE);

    //
    // read all sorts of stuff
    //
    checksum = 0;

    loadedgame = true;
    SetupGameLevel ();
    loadedgame = oldloaded;

    ReadInfo (true,tilemap,sizeof(tilemap),file);

    size = (mapwidth * mapheight) * sizeof(*maptable);
    maptable = SafeMalloc(size);

    ReadInfo (true,maptable,size,file);

    for (y = 0; y < mapheight; y++)
    {
        for (x = 0; x < mapwidth; x++)
        {
            actornum = maptable[(y << MAPSHIFT) + x];

            if (actornum & 0x8000)
                actorat[x][y] = &objlist[actornum & 0x7fff];
            else
                actorat[x][y] = (objtype *)(uintptr_t)actornum;
        }
    }

    free (maptable);
    maptable = NULL;

    ReadInfo (true,areaconnect,sizeof(areaconnect),file);
    ReadInfo (true,areabyplayer,sizeof(areabyplayer),file);

    //
    // restore save game actors
    //
    ReadInfo (false,&count,sizeof(count),file);

    temp = SafeMalloc(count * sizeof(*newobj));
    ptr = temp;

    ReadInfo (true,ptr,count * sizeof(*newobj),file);

    InitObjList ();

    newobj = player;

    while (1)
    {
        memcpy (newobj,ptr,sizeof(*newobj) - (sizeof(newobj->next) + sizeof(newobj->prev)));

        if (newobj == player)
            newobj->state = (statetype *)((uintptr_t)newobj->state + (uintptr_t)&s_player);
        else
            newobj->state = (statetype *)((uintptr_t)newobj->state + (uintptr_t)&s_ofs_stand);

        newobj->tempobj = (objtype *)((uintptr_t)newobj->tempobj + (uintptr_t)objlist);

        //
        // KS: I remember that dead actor spots can become solid after loading
        // a saved game, and this line is probably the reason why. It should
        // test for FL_NONMARK | FL_NEVERMARK before updating actorat
        //
        actorat[newobj->tilex][newobj->tiley] = newobj;
#ifdef LOOK_FOR_DEAD_GUYS
        if (newobj->flags & FL_DEADGUY)
            deadguys[numdeadguys++] = newobj;
#endif
        if (!--count)
            break;

        ptr += sizeof(*newobj);

        newobj = GetNewObj();
    }

    free (temp);

    //
    //  re-establish links to barrier switches
    //
    for (obj = player->next; obj; obj = obj->next)
    {
        switch (obj->obclass)
        {
            case arc_barrierobj:
            case post_barrierobj:
            case vspike_barrierobj:
            case vpost_barrierobj:
                obj->temp2 = ScanBarrierTable(obj->tilex,obj->tiley);
                break;
        }
    }

    ConnectBarriers ();

    //
    // read all sorts of stuff
    //
    ReadInfo (false,&laststatobjnum,sizeof(laststatobjnum),file);
    laststatobj = &statobjlist[laststatobjnum];

    ReadInfo (true,statobjlist,sizeof(statobjlist),file);
    ReadInfo (true,doorobjlist,sizeof(doorobjlist),file);
    ReadInfo (false,&pwallstate,sizeof(pwallstate),file);
    ReadInfo (false,&pwallx,sizeof(pwallx),file);
    ReadInfo (false,&pwally,sizeof(pwally),file);
    ReadInfo (false,&pwalldir,sizeof(pwalldir),file);
    ReadInfo (false,&pwallpos,sizeof(pwallpos),file);
    ReadInfo (false,&pwalldist,sizeof(pwalldist),file);
    ReadInfo (true,TravelTable,sizeof(TravelTable),file);
    ReadInfo (true,&ConHintList,sizeof(ConHintList),file);
    ReadInfo (true,eaList,sizeof(eaList),file);
    ReadInfo (true,&GoldsternInfo,sizeof(GoldsternInfo),file);
    ReadInfo (true,&GoldieList,sizeof(GoldieList),file);
    ReadInfo (false,gamestate.barrier_table,sizeof(gamestate.barrier_table),file);
    ReadInfo (false,&gamestate.plasma_detonators,sizeof(gamestate.plasma_detonators),file);

    //
    // read and evaluate checksum
    //
    LoadLevelUpdate (LS_current++,LS_total);

    if (!fread(&oldchecksum,sizeof(oldchecksum),1,file))
        Quit ("Error reading file: %s",strerror(errno));

    if (oldchecksum != checksum)
    {
        oldwx = WindowX;
        oldwy = WindowY;
        oldww = WindowW;
        oldwh = WindowH,
        oldpx = px;
        oldpy = py;

        WindowX = 0;
        WindowY = 16;
        WindowW = 320;
        WindowH = 168;

        CacheMessage (BADINFO_TEXT);

        WindowX = oldwx;
        WindowY = oldwy;
        WindowW = oldww;
        WindowH = oldwh;
        px = oldpx;
        py = oldpy;

        IN_ClearKeysDown ();
        IN_Ack ();

        gamestate.score = 0;
        gamestate.nextextra = EXTRAPOINTS;
        gamestate.lives = 1;

        gamestate.weapon = gamestate.chosenweapon = wp_autocharge;
        gamestate.weapons = 1 << wp_autocharge;

        gamestate.ammo = 8;
    }

    fclose (file);

    //
    // clean-up LZH compression
    //
    free (lzh_work_buffer);
    LZH_Shutdown ();

    SetViewSize (viewsize);

    //
    // check for strange door and actor combos
    //
    CleanUpDoors_N_Actors ();
}


/*
====================
=
= SaveLevel
=
====================
*/

void SaveLevel (int levelnum)
{
    objtype   *obj,*tempobj;
    statetype *tempstate;
    FILE      *file;
    int       x,y;
    int       oldmapon;
    int32_t   size,cksize;
    char      chunk[5];
    word      actornum,laststatobjnum;
    word      *maptable;
    unsigned  count;
    unsigned  gflags = gamestate.flags;
    byte      *temp,*ptr;

    WindowY = 181;

    //
    // make sure floor stats are saved
    //
    oldmapon = gamestate.mapon;
    gamestate.mapon = gamestate.lastmapon;

    ShowStats (0,0,ss_justcalc,&gamestuff.level[gamestate.mapon].stats);

    gamestate.mapon = oldmapon;
    gamestate.flags &= ~GS_VIRGIN_LEVEL;

    //
    // open PLAYTEMP file
    //
    MakeDestPath (PLAYTEMP_FILE);

    file = fopen(tempPath,"rb+");

    if (!file)
        CA_CannotOpen (tempPath);

    //
    // remove level chunk from file
    //
    snprintf (chunk,sizeof(chunk),"LV%02x",levelnum);
    DeleteChunk (file,chunk);

    //
    // setup LZH compression
    //
    LZH_Startup ();
    lzh_work_buffer = SafeMalloc(LZH_WORK_BUFFER_SIZE);

    //
    // write level chunk id
    //
    if (!fwrite(chunk,sizeof(chunk - 1),1,file))
        Quit ("Error writing file %s: %s",tempPath,strerror(errno));

    fseek (file,sizeof(chunk) - 1,SEEK_CUR);    // leave space for chunk size

    //
    // write all sorts of info
    //
    checksum = cksize = 0;

    cksize += WriteInfo(true,tilemap,sizeof(tilemap),file);

    size = (mapwidth * mapheight) * sizeof(*maptable);
    maptable = SafeMalloc(size);

    for (y = 0; y < mapheight; y++)
    {
        for (x = 0; x < mapwidth; x++)
        {
            obj = actorat[x][y];

            if (ISPOINTER(obj))
                actornum = 0x8000 | (word)(obj - objlist);
            else
                actornum = (word)(uintptr_t)obj;

            maptable[(y << MAPSHIFT) + x] = actornum;
        }
    }

    cksize += WriteInfo(true,maptable,size,file);

    free (maptable);
    maptable = NULL;

    cksize += WriteInfo(true,areaconnect,sizeof(areaconnect),file);
    cksize += WriteInfo(true,areabyplayer,sizeof(areabyplayer),file);

    //
    // write actor list
    //
    temp = SafeMalloc(sizeof(objlist));
    ptr = temp;

    count = 0;

    for (obj = player; obj; obj = obj->next)
    {
        tempstate = obj->state;
        tempobj = obj->tempobj;

        if (obj->obclass == playerobj)
            obj->state = (statetype *)((uintptr_t)obj->state - (uintptr_t)&s_player);
        else
            obj->state = (statetype *)((uintptr_t)obj->state - (uintptr_t)&s_ofs_stand);

        obj->tempobj = (objtype *)((uintptr_t)obj->tempobj - (uintptr_t)objlist);

        memcpy (ptr,obj,sizeof(*obj));

        obj->state = tempstate;
        obj->tempobj = tempobj;

        ptr += sizeof(*obj);
        count++;
    }

    cksize += WriteInfo(false,&count,sizeof(count),file);
    cksize += WriteInfo(true,temp,count * sizeof(*obj),file);

    free (temp);

    //
    // write all sorts of info
    //
    laststatobjnum = STATICNUM(laststatobj);

    cksize += WriteInfo(false,&laststatobjnum,sizeof(laststatobjnum),file);
    cksize += WriteInfo(true,statobjlist,sizeof(statobjlist),file);
    cksize += WriteInfo(true,doorobjlist,sizeof(doorobjlist),file);
    cksize += WriteInfo(false,&pwallstate,sizeof(pwallstate),file);
    cksize += WriteInfo(false,&pwallx,sizeof(pwallx),file);
    cksize += WriteInfo(false,&pwally,sizeof(pwally),file);
    cksize += WriteInfo(false,&pwalldir,sizeof(pwalldir),file);
    cksize += WriteInfo(false,&pwallpos,sizeof(pwallpos),file);
    cksize += WriteInfo(false,&pwalldist,sizeof(pwalldist),file);
    cksize += WriteInfo(true,TravelTable,sizeof(TravelTable),file);
    cksize += WriteInfo(true,&ConHintList,sizeof(ConHintList),file);
    cksize += WriteInfo(true,eaList,sizeof(eaList),file);
    cksize += WriteInfo(true,&GoldsternInfo,sizeof(GoldsternInfo),file);
    cksize += WriteInfo(true,&GoldieList,sizeof(GoldieList),file);
    cksize += WriteInfo(false,gamestate.barrier_table,sizeof(gamestate.barrier_table),file);
    cksize += WriteInfo(false,&gamestate.plasma_detonators,sizeof(gamestate.plasma_detonators),file);

    //
    // write checksum
    //
    cksize += WriteInfo(false,&checksum,sizeof(checksum),file);

    //
    // write chunk size & set file size
    //
    size = ftell(file);

    fseek (file,-(cksize + sizeof(cksize)),SEEK_CUR);

    if (!fwrite(&cksize,sizeof(cksize),1,file))
        Quit ("Error writing file %s: %s",tempPath,strerror(errno));

    if (ftruncate(fileno(file),size) == -1)
        Quit ("Error truncating file %s: %s",tempPath,strerror(errno));

    fclose (file);

    //
    // clean-up LZH compression
    //
    free (lzh_work_buffer);
    LZH_Shutdown ();

    SetViewSize (viewsize);

    gamestate.flags = gflags;
}


/*
====================
=
= LoadTheGame
=
====================
*/

bool LoadTheGame (FILE *file)
{
    FILE       *tempfile;
#ifdef NOTYET
    bool       oldversion;
    int        oldwx,oldwy,oldww,oldwh;
    int        oldpx,oldpy;
    size_t     len;
    char       *infospace;
#endif
    int32_t    cksize,headersize;
    byte       *temp;
    const char *vers = "VERS",*head = "HEAD";

    //
    // read in VERSion chunk
    //
    if (!FindChunk(file,vers))
        return false;

    //
    // TODO: don't bother with the version check for now
    //
#ifndef NOTYET
    fseek (file,strlen(SavegameInfoText) + 1,SEEK_CUR);
#else
    len = strlen(SavegameInfoText) + 1;
    infospace = SafeMalloc(len);

    if (!fread(infospace,len,1,file))
        Quit ("Error reading file: %s, %d",strerror(errno),ftell(file));

    infospace[len - 1] = '\0';

    oldversion = memcmp(infospace,SavegameInfoText,len);

    free (infospace);

    if (oldversion)
    {
        //
        // old version of game
        //
        oldwx = WindowX;
        oldwy = WindowY;
        oldww = WindowW;
        oldwh = WindowH;
        oldpx = px;
        oldpy = py;

        WindowX = 0;
        WindowY = 16;
        WindowW = 320;
        WindowH = 168;

        CacheMessage (BADSAVEGAME_TEXT);
        SD_PlaySound (NOWAYSND);

        WindowX = oldwx;
        WindowY = oldwy;
        WindowW = oldww;
        WindowH = oldwh;
        px = oldpx;
        py = oldpy;

        IN_ClearKeysDown ();
        IN_Ack ();

        VW_FadeOut ();

        return false;
    }
#endif
    //
    // read in HEAD chunk
    //
    if (FindChunk(file,head))
    {
        //
        // setup LZH decompression
        //
        LZH_Startup ();
        lzh_work_buffer = SafeMalloc(LZH_WORK_BUFFER_SIZE);

        ReadInfo (true,&gamestate,sizeof(gamestate),file);
        ReadInfo (true,&gamestuff,sizeof(gamestuff),file);

        //
        // clean-up LZH decompression
        //
        free (lzh_work_buffer);
        LZH_Shutdown ();
    }
    else
        return false;

    //
    // reinitialize page manager
    //
#ifdef DUAL_SWAP_FILES
    PM_Shutdown ();
    PM_Startup ();
#endif
    //
    // start music for the starting level in this loaded game
    //
    FreeMusic ();
    StartMusic ();

    //
    // copy all remaining chunks to PLAYTEMP file
    //
    MakeDestPath (PLAYTEMP_FILE);

    tempfile = fopen(tempPath,"wb");

    if (!tempfile)
        return false;

    headersize = strlen(head) + sizeof(int32_t);    // chunk ID and LENGTH

    for (cksize = NextChunk(file); cksize; cksize = NextChunk(file))
    {
        cksize += headersize;                   // include header

        fseek (file,-headersize,SEEK_CUR);      // seek to start of chunk

        temp = SafeMalloc(cksize);

        //
        // read chunk from SAVEGAME file
        //
        if (!fread(temp,cksize,1,file))
            Quit ("Error reading file: %s",strerror(errno));

        //
        // write chunk to PLAYTEMP file
        //
        if (!fwrite(temp,cksize,1,tempfile))
            Quit ("Error writing file %s: %s",tempPath,strerror(errno));

        free (temp);
    }

    fclose (tempfile);

    SetViewSize (viewsize);

    //
    // load current level
    //
    LoadLevel (0xff);
    ShowQuickMsg = false;

    return true;
}


/*
====================
=
= SaveTheGame
=
====================
*/

bool SaveTheGame (FILE *file, const char *description)
{
    struct stat statbuf;
    uint32_t    cksize;
    FILE        *tempfile;
    const char  *vers = "VERS",*desc = "DESC",*head = "HEAD";

    //
    // save current level into PLAYTEMP
    //
    SaveLevel (0xff);

    //
    // write VERSion chunk
    //
    cksize = strlen(SavegameInfoText) + 1;

    if (!fwrite(vers,strlen(vers),1,file))
        Quit ("Error writing file: %s",strerror(errno));

    if (!fwrite(&cksize,sizeof(cksize),1,file))
        Quit ("Error writing file: %s",strerror(errno));

    if (!fwrite(SavegameInfoText,cksize,1,file))
        Quit ("Error writing file: %s",strerror(errno));

    //
    // write DESC chunk
    //
    cksize = strlen(description) + 1;

    if (!fwrite(desc,strlen(desc),1,file))
        Quit ("Error writing file: %s",strerror(errno));

    if (!fwrite(&cksize,sizeof(cksize),1,file))
        Quit ("Error writing file: %s",strerror(errno));

    if (!fwrite(description,cksize,1,file))
        Quit ("Error writing file: %s",strerror(errno));

    //
    // write HEAD chunk
    //
    cksize = 0;

    if (!fwrite(head,strlen(head),1,file))
        Quit ("Error writing file: %s",strerror(errno));

    fseek (file,sizeof(cksize),SEEK_CUR);   // leave space for chunk size

    //
    // setup LZH compression
    //
    LZH_Startup ();
    lzh_work_buffer = SafeMalloc(LZH_WORK_BUFFER_SIZE);

    cksize += WriteInfo(true,&gamestate,sizeof(gamestate),file);
    cksize += WriteInfo(true,&gamestuff,sizeof(gamestuff),file);

    //
    // clean-up LZH compression
    //
    free (lzh_work_buffer);
    LZH_Shutdown ();

    //
    // write chunk size
    //
    fseek (file,-(cksize + sizeof(cksize)),SEEK_CUR);

    if (!fwrite(&cksize,sizeof(cksize),1,file))
        Quit ("Error writing file: %s",strerror(errno));

    //
    // append PLAYTEMP file to savegame file
    //
    fseek (file,cksize,SEEK_CUR);

    MakeDestPath (PLAYTEMP_FILE);

    //
    // TODO: might need access()
    //
    if (stat(tempPath,&statbuf))
        return false;

    tempfile = fopen(tempPath,"rb");

    if (!tempfile)
        return false;

    IO_CopyFile (tempfile,file,-1);

    fclose (tempfile);

    SetViewSize (viewsize);

    return true;
}


/*
====================
=
= LevelInPlaytemp
=
====================
*/

bool LevelInPlaytemp (int levelnum)
{
    FILE *file;
    char chunk[5];
    bool retval;

    MakeDestPath(PLAYTEMP_FILE);

    file = fopen(tempPath,"rb");

    //
    // see if level exists in PLAYTEMP file
    //
    snprintf (chunk,sizeof(chunk),"LV%02x",levelnum);

    retval = FindChunk(file,chunk) != 0;

    fclose (file);

    return retval;
}


/*
====================
=
= CleanUpDoors_N_Actors
=
====================
*/

void CleanUpDoors_N_Actors (void)
{
    int      x,y;
    objtype  *check;
    unsigned tile,doornum;

    for (y = 0; y < mapheight; y++)
    {
        for (x = 0; x < mapwidth; x++)
        {
            tile = tilemap[x][y];

            if (tile & 0x80)
            {
                //
                // found a door
                //
                check = actorat[x][y];

                if (ISPOINTER(check))
                {
                    //
                    // found an actor
                    //
                    doornum = tile & 0x3f;

                    if ((check->flags & (FL_SOLID | FL_DEADGUY)) == (FL_SOLID | FL_DEADGUY))
                        check->flags &= ~(FL_SHOOTABLE | FL_SOLID | FL_FAKE_STATIC);

                    //
                    // make sure door is open
                    //
                    doorobjlist[doornum].ticcount = 0;
                    doorobjlist[doornum].action = dr_open;
                    doorobjlist[doornum].position = TILEGLOBAL - 1;
                }
            }
        }
    }
}


/*
====================
=
= ClearNClose
=
= Called when changing levels via standard elevator
=
= This code doesn't CLEAR the elevator door as originally
= planned because actors were coded to stay out of the
= elevator doorway
=
====================
*/

void ClearNClose (void)
{
    int x,y,tx,ty,px,py;
    int doornum;

    tx = ty = 0;
    px = player->x >> TILESHIFT;
    py = player->y >> TILESHIFT;

    //
    // locate the door
    //
    for (x = -1; x < 2 && !tx; x += 2)
    {
        for (y = -1; y < 2; y += 2)
        {
            if (tilemap[px + x][py + y] & 0x80)
            {
                tx = px + x;
                ty = py + y;
                break;
            }
        }
    }

    if (tx)
    {
        //
        // close the door & make it solid
        //
        doornum = tilemap[tx][ty] & 0x3f;

        doorobjlist[doornum].action = dr_closed;
        doorobjlist[doornum].position = 0;

        actorat[tx][ty] = (objtype *)(doornum | 0x80);
    }
}


/*
====================
=
= CycleColors
=
====================
*/

void CycleColors (void)
{
#ifdef NOTYET
    #define NUM_RANGES     5
    #define CRNG_LOW       0xf0
    #define CRNG_HIGH      0xfe
    #define CRNG_SIZE      (CRNG_HIGH - CRNG_LOW + 1)

    static CycleInfo crange[NUM_RANGES] =
    {
        {7,0,0xf0,0xf1},
        {15,0,0xf2,0xf3},
        {30,0,0xf4,0xf5},
        {10,0,0xf6,0xf9},
        {12,0,0xfa,0xfe},
    };

    int       i;
    CycleInfo *c;
    byte      cbuffer[CRNG_SIZE][3],temp[3];
    int       first,last,numregs;
    bool      changes = false;

    for (i = 0; i < NUM_RANGES; i++)
    {
        c = &crange[i];

        if (tics >= c->delay_count)
        {
            if (!changes)
            {
                VW_GetPalette (CRNG_LOW,CRNG_SIZE,(byte *)cbuffer);

                changes = true;
            }

            first = c->firstreg - CRNG_LOW;
            numregs = c->lastreg - c->firstreg;    // is one less than in range
            last = first + numregs;

            memcpy (temp,cbuffer[last],sizeof(temp));
            memmove (cbuffer[first + 1],cbuffer[first],numregs * 3);
            memcpy (cbuffer[first],temp,sizeof(temp));

            c->delay_count = c->init_delay;
        }
        else
            c->delay_count -= tics;
    }

    if (changes)
        VW_SetPalette (CRNG_LOW,CRNG_SIZE,(byte *)cbuffer);
    else
        VW_WaitVBL (1);
#endif
}


/*
=====================
=
= InitDigiMap
=
= Using Wolf4SDL's mapping
=
=====================
*/

//
// channel mapping:
//  -1: any non reserved channel
//   0: player weapons
//   1: boss weapons
//
struct digimapstruct
{
    int soundname;
    int chunk;
    int channel;
} digimap[] =
{
    {ATKIONCANNONSND,       0,  0},
    {ATKCHARGEDSND,         1,  0},
    {ATKBURSTRIFLESND,      2,  0},
    {ATKGRENADESND,        46,  0},

    {OPENDOORSND,           3, -1},
    {CLOSEDOORSND,          4, -1},
    {HTECHDOOROPENSND,      5, -1},
    {HTECHDOORCLOSESND,     6, -1},

    {INFORMANTDEATHSND,     7, -1},
    {SCIENTISTHALTSND,     19, -1},
    {SCIENTISTDEATHSND,    20, -1},

    {GOLDSTERNHALTSND,      8, -1},
    {GOLDSTERNLAUGHSND,    24, -1},

    {HALTSND,               9, -1},     // Rent-A-Cop 1st sighting
    {RENTDEATH1SND,        10, -1},     // Rent-A-Cop Death

    {EXPLODE1SND,          11, -1},

    {GGUARDHALTSND,        12, -1},
    {GGUARDDEATHSND,       17, -1},

    {PROHALTSND,           16, -1},
    {PROGUARDDEATHSND,     13, -1},

    {BLUEBOYDEATHSND,      18, -1},
    {BLUEBOYHALTSND,       51, -1},

    {SWATHALTSND,          22, -1},
    {SWATDIESND,           47, -1},

    {SCANHALTSND,          15, -1},
    {SCANDEATHSND,         23, -1},

    {PODHATCHSND,          26, -1},
    {PODHALTSND,           50, -1},
    {PODDEATHSND,          49, -1},

    {ELECTSHOTSND,         27, -1},

    {DOGBOYHALTSND,        14, -1},
    {DOGBOYDEATHSND,       21, -1},
    {ELECARCDAMAGESND,     25, -1},
    {ELECAPPEARSND,        28, -1},
    {ELECDIESND,           29, -1},

    {INFORMDEATH2SND,      39, -1},     // Informant Death #2
    {RENTDEATH2SND,        34, -1},     // Rent-A-Cop Death #2
    {PRODEATH2SND,         42, -1},     // PRO Death #2
    {SWATDEATH2SND,        48, -1},     // SWAT Death #2
    {SCIDEATH2SND,         53, -1},     // Gen. Sci Death #2

    {LIQUIDDIESND,         30, -1},

    {GURNEYSND,            31, -1},
    {GURNEYDEATHSND,       41, -1},

    {WARPINSND,            32, -1},
    {WARPOUTSND,           33, -1},

    {EXPLODE2SND,          35, -1},

    {LCANHALTSND,          36, -1},
    {LCANDEATHSND,         37, -1},

    //{RENTDEATH3SND,        38, -1},     // Rent-A-Cop Death #3
    {INFORMDEATH3SND,      40, -1},     // Informant Death #3
    //{PRODEATH3SND,         43, -1},     // PRO Death #3
    //{SWATDEATH3SND,        52, -1},     // Swat Guard #3
    {SCIDEATH3SND,         54, -1},     // Gen. Sci Death #3

    {LCANBREAKSND,         44, -1},
    {SCANBREAKSND,         45, -1},
    {CLAWATTACKSND,        56, -1},
    {SPITATTACKSND,        55, -1},
    {PUNCHATTACKSND,       57, -1},

    {LASTSOUND}
};


void InitDigiMap (void)
{
    struct digimapstruct *map;

    for (map = &digimap[0]; map->soundname != LASTSOUND; map++)
    {
        DigiMap[map->soundname] = map->chunk;
        DigiChannel[map->chunk] = map->channel;

        SD_PrepareSound (map->chunk);
    }
}


/*
==========================
=
= ShutdownId
=
= Shuts down all ID_?? managers
=
==========================
*/

void ShutdownId (void)
{
    SD_Shutdown ();
    PM_Shutdown ();
    IN_Shutdown ();
    VW_Shutdown ();
    CA_Shutdown ();
}


/*
==========================
=
= InitGame
=
= Load a few things right away
=
==========================
*/

void InitGame (void)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0)
        Quit ("Unable to init SDL: %s\n",SDL_GetError());

    VW_Startup ();
    CA_Startup ();
    IN_Startup ();
    PM_Startup ();
    SD_Startup ();

#ifdef IN_DEVELOPMENT
    //
    // clear monochrome
    //
    VW_ClearScreen (0);
#endif
    InitDigiMap ();

    ReadConfig ();

    Init3DRenderer ();
}


/*
==========================
=
= DoMovie
=
==========================
*/

bool DoMovie (int movie, SDL_Color *palette)
{
    bool retval;

    //StopMusic ();
    SD_StopSound ();

    CA_LoadAllSounds ();

    Movies[movie].palette = palette;

    retval = MOVIE_Play(&Movies[movie]);

    SD_StopSound ();

    return retval;
}


/*
===================
=
= Terminates the program with the appropriate error message
=
===================
*/

void Terminate (const char *func, const char *string, ...)
{
    int     ret;
    va_list vlist;

    if (string)
    {
        va_start (vlist,string);

        if (func)
        {
            snprintf (str,sizeof(str),"%s: %s",func,string);
            vsnprintf (error,sizeof(error),str,vlist);
        }
        else
            vsnprintf (error,sizeof(error),string,vlist);

        va_end (vlist);
    }
    else
        error[0] = '\0';

    ret = *error != '\0';

    //
    // cleanup
    //
    MakeDestPath (PLAYTEMP_FILE);
    remove (tempPath);

    SD_StopDigitized ();

    if (!ret)
        WriteConfig ();

    ShutdownId ();
    Shutdown3DRenderer ();

    //
    // abnormal exit?
    //
    if (ret)
        Error (error);

    SDL_Quit ();
    exit (ret);
}


/*
=====================
=
= PreDemo
=
=====================
*/

void PreDemo (void)
{
    int       i;
    SDL_Color palette[256];

#ifdef TECH_SUPPORT_VERSION
    fontnumber = 4;
    SetFontColor (0,15 * 3);
    CenterWindow (26,7);
    US_Print (EnterBetaCode);
    VW_UpdateScreen (screen.buffer);
    CA_LoadAllSounds ();
    SD_PlaySound (INFORMDEATH2SND);  // Nooooo!
    IN_UserInput (TickBase * 20);
    SD_StopDigitized ();
#endif
    if (!(gamestate.flags & GS_NOWAIT))
    {
        //
        // Apogee presents
        //
        SD_StartMusic (STARTMUSIC + APOGFNFM_MUS);

        VW_ConvertPalette (grsegs[APOGEEPALETTE],palette,lengthof(palette));
        VW_FadePaletteOut (0,255,25,29,53,20);
        VW_DrawPic (0,0,APOGEEPIC);
        VW_FadePaletteIn (0,255,palette,30);

        //
        // wait for end of fanfare
        //
#ifdef NOTYET
        if (MusicMode == smm_AdLib)
        {
            IN_StartAck ();

            while (!sqPlayedOnce && !IN_CheckAck());
        }
        else
#endif
            IN_UserInput (TickBase * 6);

        SD_MusicOff ();

        CA_UncacheAudioChunk (STARTMUSIC + APOGFNFM_MUS);

        //
        // do a blue flash!
        //
        VW_FadePaletteOut (0,255,25,29,53,20);
        screen.flags &= ~SC_FADED;
        VW_FadeOut ();

        //
        // JAM logo intro
        //
        SD_StartMusic (STARTMUSIC + TITLE_LOOP_MUSIC);

        if (!DoMovie(mv_intro,gamepal))
            Quit ("JAM animation (IANIM.%s) does not exist!",extension);

        if (PowerBall)
        {
            for (i = 0; i < 60 && !DebugOk; i++)
            {
                VW_WaitVBL (1);

                if (Keyboard[sc_LShift] && Keyboard[sc_RShift])
                {
                    CA_LoadAllSounds ();
                    SD_MusicOff ();

                    SD_PlaySound (SHOOTDOORSND);
                    SD_WaitSoundDone ();

                    SD_StopDigitized ();
                    DebugOk = true;

                    SD_StartMusic (STARTMUSIC + TITLE_LOOP_MUSIC);
                }
            }
        }

        //
        // PC-13
        //
        VW_ClearScreen (0x14);
        VW_DrawPic (0,64,PC13PIC);
        VW_FadeIn ();

        IN_UserInput (TickBase * 2);

        //
        // do a red flash!
        //
        VW_FadePaletteOut (0,255,39,0,0,20);
        screen.flags &= ~SC_FADED;
        VW_FadeOut ();
    }
}


/*
=====================
=
= DemoLoop
=
=====================
*/

void DemoLoop (void)
{
#ifdef DEMOS_ENABLED
    int       LastDemo = 0;
#endif
    bool      breakit;
    SDL_Color titlepal[256];

    while (1)
    {
        playstate = ex_title;

        VW_FadeOut ();

        while (!(gamestate.flags & GS_NOWAIT))
        {
            //
            // start music when coming from menu
            //
            if (!sqActive)
                SD_StartMusic (STARTMUSIC + TITLE_LOOP_MUSIC);

            //
            // title page
            //
#ifndef SKIP_TITLE_AND_CREDITS
            breakit = false;

            VW_ConvertPalette (grsegs[TITLEPALETTE],titlepal,lengthof(titlepal));
            VW_DrawPic (0,0,TITLE1PIC);

            fontnumber = 2;
            PrintX = WindowX = 270;
            PrintY = WindowY = 179;
            WindowW = 29;
            WindowH = 8;
            VW_Bar (WindowX,WindowY - 1,WindowW,WindowH,VERSION_TEXT_BKCOLOR);
            SetFontColor (VERSION_TEXT_COLOR,VERSION_TEXT_BKCOLOR);
            US_Print (_VERSION_);

            VW_FadePaletteIn (0,255,titlepal,30);

            if (IN_UserInput(TickBase * 6))
                breakit = true;

            //
            // draw title screen 2 with warnings and copyrights
            //
            // KS: Congrats! You made this screen almost as annoying as the ones in modern games! ^^+
            //
            VW_DrawPic (0,0,TITLE2PIC);
            fontnumber = 2;
            PrintX = WindowX = 270;
            PrintY = WindowY = 179;
            WindowW = 29;
            WindowH = 8;
            VW_Bar (WindowX,WindowY - 1,WindowW,WindowH,VERSION_TEXT_BKCOLOR);
            SetFontColor (VERSION_TEXT_COLOR,VERSION_TEXT_BKCOLOR);
            US_Print (_VERSION_);

            //
            // fizzle whole screen incase of any last minute changes needed
            // on title intro
            //
            // KS: change it to be abortable? heh heh
            //
            VW_FizzleFade (0,0,320,200,70,false);

            IN_UserInput (TickBase * 2);

            if (breakit || IN_UserInput(TickBase * 6))
                break;

            VW_FadeOut ();

            //
            // credits page
            //
            DrawCreditsPage ();

            VW_FadeIn ();

            if (IN_UserInput(TickBase * 6))
                break;

            VW_FadeOut ();
#endif

            //
            // demo
            //
#ifdef DEMOS_ENABLED
            if (!param_demorecord)
                PlayDemo (LastDemo++ % 6);

            if (playstate == ex_abort)
                break;
            else
            {
                //
                // start music when coming from menu
                //
                if (!sqActive)    //if (!SD_MusicPlaying())
                    SD_StartMusic (STARTMUSIC + TITLE_LOOP_MUSIC);
            }
#endif
            //
            // high scores
            //
#ifndef SKIP_TITLE_AND_CREDITS
            VW_DrawPic (0,0,BACKGROUND_SCREENPIC);
            DrawHighScores ();

            VW_FadeIn ();

            if (IN_UserInput(TickBase * 9))
                break;

            VW_FadeOut ();
#endif
        }

        CA_UncacheAudioChunk (STARTMUSIC + TITLE_LOOP_MUSIC);

        VW_FadeOut ();
#ifdef DEMOS_EXTERN
        if (param_demorecord)
            RecordDemo ();
        else
#endif
        {
#if defined(IN_DEVELOPMENT) || defined(TECH_SUPPORT_VERSION)
            if (gamestate.flags & GS_QUICKRUN)
            {
                ReadGameNames ();
                CA_LoadAllSounds ();
                NewGame (gd_medium,gamestate.episode);
            }
            else
#endif
                ControlPanel (sc_None);
        }

        if (startgame || loadedgame)
            GameLoop ();
    }
}


/*
=====================
=
= DrawCreditsPage
=
=====================
*/

void DrawCreditsPage (void)
{
    PresenterInfo pi;

    VW_DrawPic (0,0,BACKGROUND_SCREENPIC);

    memset (&pi,0,sizeof(pi));

    pi.flags = TPF_CACHE_NO_GFX;
    pi.xl = 38;
    pi.yl = 28;
    pi.xh = 281;
    pi.yh = 170;
    pi.bgcolor = 2;
    pi.ltcolor = BORDER_HI_COLOR;
    fontcolor = BORDER_TEXT_COLOR;
    pi.shcolor = pi.dkcolor = 0;
    pi.fontnumber = fontnumber;

#ifdef ID_CACHE_CREDITS
    TP_LoadScript (NULL,&pi,CREDITSTEXT);
#else
    TP_LoadScript ("CREDITS.TXT",&pi,0);
#endif
    TP_Presenter (&pi);
}


/*
=====================
=
= CheckParameters
=
=====================
*/

void CheckParameters (int argc, char *argv[])
{
    const char *header = "Blake4SDL pre-release\n"
                         "Original Blake Stone by JAM Productions\n"
                         "Usage: Blake4SDL [options]\n"
                         "See Options.txt for help\n\n";

    const char *cinfo = "Planet Strike\n"
                        "Copyright (c) 1993 - JAM Productions, Inc.\n"
                        "All rights reserved.\n";

    int    i;
    size_t len;
    char   *arg,*helpstr;

    param_samplerate = 44100;
    param_audiobuffer = 2048;

    error[0] = '\0';
    str[0] = '\0';

    for (i = 1; i < argc; i++)
    {
        if (*error || *str)
            break;

        arg = argv[i];

        if (!strcmp(arg,"--q"))
            gamestate.flags |= GS_QUICKRUN;
        else if (!strcmp(arg,"--nowait"))
            gamestate.flags |= GS_NOWAIT;
#if defined(IN_DEVELOPMENT) || defined(TECH_SUPPORT_VERSION)
        else if (!strcmp(arg,"--l"))
        {
            i++;

            if (i >= argc || !isdigit(*argv[i]))
                snprintf (error,sizeof(error),"The l option is missing the level argument!");
            else
            {
                gamestate.flags |= GS_STARTLEVEL;
                starting_level = atoi(argv[i]);
            }
        }
        else if (!strcmp(arg,"--e"))
        {
            i++;

            if (i >= argc || !isdigit(*argv[i]))
                snprintf (error,sizeof(error),"The e option is missing the level argument!");
            else
            {
                gamestate.flags |= GS_STARTLEVEL;
                starting_episode = atoi(argv[i]) - 1;
            }
        }
        else if (!strcmp(arg,"--d"))
        {
            i++;

            if (i >= argc || !isdigit(*argv[i]))
                snprintf (error,sizeof(error),"The d option is missing the difficulty argument!");
            else
            {
                gamestate.flags |= GS_STARTLEVEL;
                starting_difficulty = atoi(argv[i]) - 1;
            }
        }
#endif
        else if (!strcmp(arg,"--version"))
            snprintf (str,sizeof(str),"%s\nVersion: %s\nCompile date: %s\n\n",cinfo,_VERSION_,__DATE__);
#ifdef DEMOS_EXTERN
        else if (!strcmp(arg,"--recdemo"))
            param_demorecord = true;
#endif
        else if (!strcmp(arg,"--tics"))
            gamestate.flags |= GS_TICS_FOR_SCORE;
        else if (!strcmp(arg,"--powerball"))
#ifdef IN_DEVELOPMENT
            DebugOk = true;
#else
            PowerBall = true;
#endif
        else if (!strcmp(arg,"--music"))
            gamestate.flags |= GS_MUSIC_TEST;
#ifdef IN_DEVELOPMENT
        else if (!strcmp(arg,"--radar"))
            gamestate.flags |= GS_SHOW_OVERHEAD;
#endif
        else if (!strcmp(arg,"--windowed"))
            param_windowed = true;
        else if (!strcmp(arg,"--extravbls"))
        {
            i++;

            if (i >= argc || !isdigit(*argv[i]))
                snprintf (error,sizeof(error),"The extravbls option is missing the vbls argument!");
            else
            {
                extravbls = atoi(argv[i]);

                if (extravbls < 0)
                    extravbls = -extravbls;
                else if (extravbls > 8)
                    snprintf (error,sizeof(error),"Maximum extravbls value is 8!");
            }
        }
        else if (!strcmp(arg,"--joystick"))
        {
            i++;

            if (i >= argc || !isdigit(*argv[i]))
                snprintf (error,sizeof(error),"The joystick option is missing the index argument!");
            else
                param_joystickindex = atoi(argv[i]);
        }
        else if (!strcmp(arg,"--joystickhat"))
        {
            i++;

            if (i >= argc || !isdigit(*argv[i]))
                snprintf (error,sizeof(error),"The joystickhat option is missing the index argument!");
            else
                param_joystickhat = atoi(argv[i]);
        }
        else if (!strcmp(arg,"--samplerate"))
        {
            i++;

            if (i >= argc || !isdigit(*argv[i]))
                snprintf (error,sizeof(error),"The samplerate option is missing the rate argument!");
            else
                param_samplerate = atoi(argv[i]);
        }
        else if (!strcmp(arg,"--audiobuffer"))
        {
            i++;

            if (i >= argc || !isdigit(*argv[i]))
                snprintf (error,sizeof(error),"The audiobuffer option is missing the size argument!");
            else
                param_audiobuffer = atoi(argv[i]);
        }
        else
            snprintf (error,sizeof(error),"Invalid parameter(s)!");
    }

    if (*error || *str)
    {
        if (*error)
        {
            len = strlen(header) + strlen(error) + 1;
            helpstr = SafeMalloc(len);

            snprintf (helpstr,len,"%s%s",header,error);

            Error (helpstr);
        }
        else
        {
            len = strlen(header) + strlen(str) + 1;
            helpstr = SafeMalloc(len);

            snprintf (helpstr,len,"%s%s",header,str);

            Help (helpstr);
        }

        free (helpstr);

        exit (*error != '\0');
    }

    if (param_samplerate != 44100 && param_audiobuffer == 2048)
        param_audiobuffer = 2048 / (44100 / param_samplerate);
}


/*
==========================
=
= InitDestPath
=
==========================
*/

void InitDestPath (void)
{
    struct stat statbuf;
    int         len;
    const char  *str;
    char        *buffer;

    str = getenv("APOGEECD");

    if (str)
    {
        len = strlen(str);

        if (len > MAX_DEST_PATH_LEN)
            Quit ("APOGEECD path too long!");

        buffer = SafeMalloc(len + 1);
        snprintf (buffer,len + 1,str);

        if (buffer[len] == '\\')
            buffer[len] = '\0';

        if (stat(buffer,&statbuf))
            Quit ("APOGEECD directory not found!");

        snprintf (destPath,sizeof(destPath),"%s\\",buffer);

        free (buffer);
    }
    else
        *destPath = '\0';
}


/*
==========================
=
= MakeDestPath
=
==========================
*/

void MakeDestPath (const char *file)
{
    snprintf (tempPath,sizeof(tempPath),file);
}


/*
==========================
=
= main
=
==========================
*/

int main (int argc, char *argv[])
{
    //
    // TODO: check if the file exists first and that we're allowed to delete it
    //
    MakeDestPath (PLAYTEMP_FILE);
    remove (tempPath);

    InitDestPath ();

    CheckForEpisodes ();

    CheckParameters (argc,argv);

    InitGame ();

    PreDemo ();

    DemoLoop ();

    Quit (NULL);

    return 0;
}
