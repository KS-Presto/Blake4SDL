// 3D_DEBUG.C

#include "3d_def.h"


/*
==================
=
= DebugMemory
=
==================
*/

void DebugMemory (void)
{
    US_CenterWindow (22,15);
#ifdef NOTYET
    US_CPrint ("Memory Usage");
    US_CPrint ("------------");
    US_Print ("Total     :");
    US_PrintUnsigned (mminfo.mainmem / 1024);
    US_Print ("k\nFree      :");
    US_PrintUnsigned (MM_UnusedMemory() / 1024);
    US_Print ("k\nWith purge:");
    US_PrintUnsigned (MM_TotalFree() / 1024);
    US_Print ("k\nTics      :");
#else
    US_Print ("Tics      :");
    US_PrintSigned (tics);
    US_Print ("\nReal Tics :");
    US_PrintSigned (realtics);
#endif
#ifdef CEILING_FLOOR_COLORS
    if (gamestate.flags & GS_DRAW_CEILING)
#endif
    {
        US_Print ("\n\nCeiling TEX: ");
        US_PrintSigned (ceilingpic - START_TEXTURES);

        US_Print (" Floor TEX: ");
        US_PrintSigned (floorpic - START_TEXTURES);
    }
#ifdef CEILING_FLOOR_COLORS
    else
    {
        US_Print ("\n\nTop COL: ");
        US_PrintUnsigned (ceilingcolor);

        US_Print (" Bottom COL: ");
        US_PrintUnsigned (floorcolor);
    }
#endif
    if (gamestate.flags & GS_LIGHTING)
    {
        US_Print ("\nShade div :");
        US_PrintSigned (normalshadediv);

        US_Print ("\nShade max :");
        US_PrintSigned (shademax);
    }

    VW_UpdateScreen (screen.buffer);
    IN_Ack ();

    WindowW = 253;
    WindowH = 8;
    fontnumber = 2;

    VW_DrawPic (0,0,TOP_STATUSBARPIC);
    ShadowPrintLocationText (sp_normal);
}


/*
==================
=
= CountObjects
=
==================
*/

void CountObjects (void)
{
    int     i,total,count,active,inactive;
    objtype *obj;

    US_CenterWindow (16,7);
    active = inactive = count = 0;

    US_Print ("Total statics :");
    total = STATICNUM(laststatobj);
    US_PrintSigned (total);

    US_Print ("\nIn use statics:");

    for (i = 0; i < total; i++)
    {
        if (statobjlist[i].shapenum != -1)
            count++;
    }

    US_PrintSigned (count);

    US_Print ("\nDoors         :");
    US_PrintSigned (doornum);

    for (obj = player->next; obj; obj = obj->next)
    {
        if (obj->active == ac_yes)
            active++;
        else
            inactive++;
    }

    US_Print ("\nTotal actors  :");
    US_PrintUnsigned (active + inactive);

    US_Print ("\nActive actors :");
    US_PrintUnsigned (active);

    VW_UpdateScreen (screen.buffer);
    IN_Ack ();
}


/*
==================
=
= CountTotals
=
==================
*/

void CountTotals(void)
{
    US_CenterWindow (20,11);

    US_Print ("  CURRENT MAP TOTALS\n");

    US_Print ("\nTotal Enemy:\n");
    US_PrintUnsigned (gamestuff.level[gamestate.mapon].stats.total_enemy);

    US_Print ("\nTotal Points:\n");
    US_PrintSigned (gamestuff.level[gamestate.mapon].stats.total_points);

    US_Print ("\nTotal Informants:\n");
    US_PrintUnsigned (gamestuff.level[gamestate.mapon].stats.total_inf);

    VW_UpdateScreen (screen.buffer);
    IN_Ack ();
}


/*
==================
=
= ShowMap
=
==================
*/

void ShowMap (void)
{
    int   oldangle;
    fixed oldx,oldy;

    oldangle = player->angle;
    oldx = player->x;
    oldy = player->y;

    player->angle = ANG90;
    player->x = player->y = ((fixed)(MAPSIZE / 2) << TILESHIFT) + TILECENTER;

    US_CenterWindow (20,11);

    US_CPrint ("CURRENT MAP\n\n ");

    vbuf = VW_LockSurface(screen.buffer);

    if (!vbuf)
        Quit ("Unable to lock surface: %s\n",SDL_GetError());

    ShowOverhead ((screen.basewidth / 2) - 32,py,32,0,OV_ACTORS | OV_SHOWALL | OV_KEYS | OV_PUSHWALLS);

    VW_UnlockSurface (screen.buffer);
    vbuf = NULL;

    VW_UpdateScreen (screen.buffer);
    IN_Ack ();

    player->angle = oldangle;
    player->x = oldx;
    player->y = oldy;
}


#ifdef IN_DEVELOPMENT
/*
================
=
= ShapeTest
=
================
*/

void ShapeTest (void)
{
    bool     done;
    ScanCode scan;
    int      i,j,k,x;
    int      v1,v2;
    int      sound;
    int      oldviewheight;
    uint32_t l,oldflags;
    byte     *addr;

    US_CenterWindow (20,16);
    VW_UpdateScreen (screen.buffer);

    for (i = 0, done = false; !done; )
    {
        US_ClearWindow ();
        sound = -1;

        US_Print (" Page #");
        US_PrintSigned (i);

        if (i < PMSpriteStart)
            US_Print (" (Wall)");
        else if (i < PMSoundStart)
            US_Print (" (Sprite)");
        else if (i == ChunksInFile - 1)
            US_Print (" (Sound Info)");
        else
            US_Print (" (Sound)");

        US_Print ("\n Address: ");
        addr = PM_GetPage(i);

        snprintf (str,sizeof(str),"0x%010x",(uintptr_t)addr);
        US_Print (str);

        if (addr)
        {
            if (i < PMSpriteStart)
            {
                //
                // draw the wall
                //
                vbuf = VW_LockSurface(screen.buffer);

                if (!vbuf)
                    Quit ("Unable to lock surface: %s\n",SDL_GetError());

                postx = (screen.width / 2) - ((TEXTURESIZE * screen.scale) / 2);
                postsource = addr;

                oldviewheight = viewheight;
                viewheight = screen.height;        // no clipping
                centery = viewheight / 2;

                oldflags = gamestate.flags;
                gamestate.flags &= ~GS_LIGHTING;   // no shading

                for (x = 0; x < TEXTURESIZE * screen.scale; x++, postx++)
                {
                    wallheight[postx] = 256 * screen.scale;

                    ScalePost ();

                    if ((x % screen.scale) == screen.scale - 1)
                        postsource += TEXTURESIZE;
                }

                VW_UnlockSurface (screen.buffer);
                vbuf = NULL;

                viewheight = oldviewheight;
                centery = viewheight / 2;

                if (oldflags & GS_LIGHTING)
                    gamestate.flags |= GS_LIGHTING;
            }
            else if (i < PMSoundStart)
            {
                //
                // draw the sprite
                //
                vbuf = VW_LockSurface(screen.buffer);

                if (!vbuf)
                    Quit ("Unable to lock surface: %s\n",SDL_GetError());

                oldviewheight = viewheight;
                viewheight = screen.height;
                centery = viewheight / 2;

                oldflags = gamestate.flags;
                gamestate.flags &= ~GS_LIGHTING;

                SimpleScaleShape (screen.width / 2,centery,64 * screen.scale,i - PMSpriteStart,NO_SHADING);

                VW_UnlockSurface (screen.buffer);
                vbuf = NULL;

                viewheight = oldviewheight;
                centery = viewheight / 2;

                if (oldflags & GS_LIGHTING)
                    gamestate.flags |= GS_LIGHTING;
            }
            else if (i == ChunksInFile - 1)
            {
                US_Print ("\n\n Number of sounds: ");
                US_PrintSigned (NumDigi);

                l = 0;

                for (j = 0; j < NumDigi; j++)
                    l += DigiList[j].length;

                US_Print ("\n Total bytes: ");
                US_PrintUnsigned (l);
                US_Print ("\n Total pages: ");
                US_PrintSigned (ChunksInFile - PMSoundStart - 1);
            }
            else
            {
                //
                // display sounds
                //
                sound = i - PMSoundStart;

                for (j = 0; j < NumDigi; j++)
                {
                    if (j == NumDigi - 1)
                        k = ChunksInFile - 1;    // don't let it overflow
                    else
                        k = DigiList[j + 1].startpage;

                    if (i >= PMSoundStart + DigiList[j].startpage && i < PMSoundStart + k)
                        break;
                }

                if (j < NumDigi)
                {
                    sound = j;

                    US_Print ("\n Sound #");
                    US_PrintSigned (sound);
                    US_Print ("\n Segment #");
                    US_PrintSigned (i - PMSoundStart - DigiList[j].startpage);
                }

                for (j = 0; j < pageLengths[i]; j += 32)
                {
                    v1 = addr[j];
                    v2 = (v1 - 128) / 4;

                    if (v2 < 0)
                        VW_Vlin (WindowY + WindowH - 32 + v2,WindowY + WindowH - 32,WindowX + 8 + (j / 32),BLACK);
                    else
                        VW_Vlin (WindowY + WindowH - 32,WindowY + WindowH - 32 + v2,WindowX + 8 + (j / 32),BLACK);
                }
            }
        }

        VW_UpdateScreen (screen.buffer);

        //
        // TODO: this should be changed to a WaitEvent loop
        // to improve control (eg. mouse wheel events)
        //
        IN_Ack ();

        scan = LastScan;

        IN_ClearKey (scan);

        switch (scan)
        {
            case sc_LeftArrow:
                if (i)
                    i--;
                break;

            case sc_RightArrow:
                if (++i >= ChunksInFile)
                    i--;
                break;

            case sc_W:  // Walls
                i = 0;
                break;

            case sc_S:  // Sprites
                i = PMSpriteStart;
                break;

            case sc_D: // Digitized
                i = PMSoundStart;
                break;

            case sc_I: // Digitized info
                i = ChunksInFile - 1;
                break;

            case sc_P:
                if (sound != -1)
                    SD_PlayDigitized (sound,8,8);
                break;

            case sc_Escape:
                done = true;
                break;
        }
    }

    SD_StopDigitized ();
    RedrawStatusAreas ();
}
#endif

/*
================
=
= DebugKeys
=
================
*/

bool DebugKeys (void)
{
    int level,i;

    if (Keyboard[sc_A])  // A = Show Actors on AutoMap
    {
        ExtraRadarFlags ^= OV_ACTORS;
        US_CenterWindow (24,3);

        if (ExtraRadarFlags & OV_ACTORS)
            US_PrintCentered ("AUTOMAP: Show Actors ON");
        else
            US_PrintCentered ("AUTOMAP: Show Actors OFF");

        VW_UpdateScreen (screen.buffer);
        IN_Ack ();

        return true;
    }
#ifdef NOTYET
    else if (Keyboard[sc_B])  // B = border color
    {
        US_CenterWindow (24,3);
        PrintY += 6;
        US_Print (" Border color (0-15):");
        VW_UpdateScreen (screen.buffer);

        if (US_LineInput(px,py,str,NULL,true,2,0))
        {
            level = atoi(str);

            if (level >= 0 && level <= 15)
                VW_ColorBorder (level);
        }

        return true;
    }
#endif
    else if (Keyboard[sc_K])  // K = Map Content totals
    {
        CountTotals ();
        return true;
    }
    else if (Keyboard[sc_C])  // C = count objects
    {
        CountObjects ();
        return true;
    }
    else if (Keyboard[sc_R])
    {
        ShowMap ();
        return true;
    }
    else if (Keyboard[sc_D])   // D = Dumb/Blind Objects (player invisible)
    {
        US_CenterWindow (19,3);
        PlayerInvisible ^= true;

        if (PlayerInvisible)
            US_PrintCentered ("Player Invisible");
        else
            US_PrintCentered ("Player visible");

        VW_UpdateScreen (screen.buffer);
        IN_Ack ();

        return true;
    }
    else if (Keyboard[sc_E])   // E = Win Mission
    {
        US_CenterWindow (19,3);
        US_PrintCentered ("Instant Wiener!");
        InstantWin = 1;
        playstate = ex_victorious;

        VW_UpdateScreen (screen.buffer);
        IN_Ack ();

        return true;
    }
    else if (Keyboard[sc_F])  // F = facing spot
    {
        US_CenterWindow (18,5);
        US_Print ("X:");
        US_PrintSigned (player->x);
        US_Print ("  ");
        US_PrintSigned (player->x >> TILESHIFT);
        US_Print ("\nY:");
        US_PrintSigned (player->y);
        US_Print ("  ");
        US_PrintSigned (player->y >> TILESHIFT);
        US_Print ("\nA:");
        US_PrintSigned (player->angle);
        US_Print ("\nD:");
        US_PrintUnsigned (player->dir);

        VW_UpdateScreen (screen.buffer);
        IN_Ack ();

        return true;
    }
    else if (Keyboard[sc_G])  // G = god mode
    {
        US_CenterWindow (12,2);

        if (godmode)
            US_PrintCentered ("God mode OFF");
        else
            US_PrintCentered ("God mode ON");

        VW_UpdateScreen (screen.buffer);
        IN_Ack ();

        godmode ^= true;

        return true;
    }
    else if (Keyboard[sc_H])  // H = hurt self
    {
        IN_ClearKeysDown ();
        TakeDamage (1,NULL);
    }
    else if (Keyboard[sc_I])   // I = item cheat
    {
        US_CenterWindow (12,3);
        US_PrintCentered ("Free items!");

        HealSelf (99);
        GiveToken (5);

        for (i = wp_autocharge; i <= wp_bfg_cannon; i++)
        {
            if (!(gamestate.weapons & (1 << i)))
            {
                GiveWeapon (i);
                break;
            }
        }

        gamestate.ammo += 50;

        if (gamestate.ammo > MAX_AMMO)
            gamestate.ammo = MAX_AMMO;

        DrawAmmo (true);
        DrawScore ();

        VW_UpdateScreen (screen.buffer);
        IN_Ack ();

        return true;
    }
    else if (Keyboard[sc_M])   // M = memory info
    {
        DebugMemory ();

        return true;
    }
#ifdef IN_DEVELOPMENT
    else if (Keyboard[sc_N])   // N = no clip
    {
        US_CenterWindow (18,3);

        gamestate.flags ^= GS_CLIP_WALLS;

        if (gamestate.flags & GS_CLIP_WALLS)
            US_PrintCentered ("NO clipping OFF");
        else
            US_PrintCentered ("No clipping ON");

        VW_UpdateScreen (screen.buffer);
        IN_Ack ();

        return true;
    }
    else if (Keyboard[sc_T])   // T = shape test
    {
        ShapeTest ();

        return true;
    }
#endif
    else if (Keyboard[sc_O])   // O = Show Push Walls
    {
        US_CenterWindow (24,3);

        ExtraRadarFlags ^= OV_PUSHWALLS;

        if (ExtraRadarFlags & OV_PUSHWALLS)
            US_PrintCentered ("AUTOMAP: Show PWalls ON");
        else
            US_PrintCentered ("AUTOMAP: Show PWalls OFF");

        VW_UpdateScreen (screen.buffer);
        IN_Ack ();

        return true;
    }
    else if (Keyboard[sc_U])   // Unlock All Floors
    {
        US_CenterWindow (24,3);
        US_PrintCentered ("Unlock All Floors!");
        VW_UpdateScreen (screen.buffer);
        IN_Ack ();

        for (i = 0; i < MAPS_PER_EPISODE; i++)
            gamestuff.level[i].locked = false;

        return true;
    }
    else if (Keyboard[sc_V])   // V = extra VBLs
    {
        US_CenterWindow (30,3);
        PrintY += 6;
        US_Print ("  Add how many extra VBLs(0-8):");
        VW_UpdateScreen (screen.buffer);

        if (US_LineInput (px,py,str,NULL,true,2,0))
        {
            level = atoi(str);

            if (level >= 0 && level <= 8)
                extravbls = level;
        }

        return true;
    }
    else if (Keyboard[sc_S])   // S = slow motion
    {
        US_CenterWindow (18,3);

        singlestep ^= true;

        if (singlestep)
            US_PrintCentered ("Slow motion ON");
        else
            US_PrintCentered ("Slow motion OFF");

        VW_UpdateScreen (screen.buffer);
        IN_Ack ();

        return true;
    }
    else if (Keyboard[sc_W])   // W = warp to level
    {
        US_CenterWindow(26,5);
        PrintY += 6;

        ForceLoadDefault = Keyboard[sc_LShift] || Keyboard[sc_RShift] || Keyboard[sc_CapsLock];

        if (ForceLoadDefault)
            US_Print ("         --- LOAD DEFAULT ---\n");

        US_Print ("  Current map: ");
        US_PrintSigned (gamestate.mapon);
        US_Print ("\n  Enter map number: ");
        VW_UpdateScreen (screen.buffer);

        if (US_LineInput (px,py,str,NULL,true,2,0))
        {
            level = atoi(str);

            if (level >= 0 && level <= MAX_WARP_LEVEL)
            {
                if (level != gamestate.mapon - 1)
                {
                    gamestate.lastmapon = gamestate.mapon;
                    gamestate.mapon = level - 1;
                    playstate = ex_warped;

                    if (ForceLoadDefault)
                        BONUS_QUEUE = BONUS_SHOWN = 0;
                }
            }
        }

        return true;
    }
    else if (Keyboard[sc_Home])  // Dec ceiling color
    {
#ifdef CEILING_FLOOR_COLORS
        if (gamestate.flags & GS_DRAW_CEILING)
#endif
        {
            if (--ceilingpic < 0)
                ceilingpic = NUM_TILES - 1;

            LoadPlanes ();
        }
#ifdef CEILING_FLOOR_COLORS
        else
        {
            ceilingcolor--;
            ceilingcolor &= 0xff;
        }
#endif
        VW_WaitVBL (8);

        return true;
    }
    else if (Keyboard[sc_PgUp])  // Inc ceiling color
    {
#ifdef CEILING_FLOOR_COLORS
        if (gamestate.flags & GS_DRAW_CEILING)
#endif
        {
            if (++ceilingpic >= NUM_TILES)
                ceilingpic = 0;

            LoadPlanes ();
        }
#ifdef CEILING_FLOOR_COLORS
        else
        {
            ceilingcolor++;
            ceilingcolor &= 0xff;
        }
#endif
        VW_WaitVBL (8);

        return true;
    }
    else if (Keyboard[sc_End])   // Dec floor color
    {
#ifdef CEILING_FLOOR_COLORS
        if (gamestate.flags & GS_DRAW_FLOOR)
#endif
        {
            if (--floorpic < 0)
                floorpic = NUM_TILES - 1;

            LoadPlanes ();
        }
#ifdef CEILING_FLOOR_COLORS
        else
        {
            floorcolor--;
            floorcolor &= 0xff;
        }
#endif
        VW_WaitVBL (8);

        return true;
    }
    else if (Keyboard[sc_PgDn])  // Inc bottom color
    {
#ifdef CEILING_FLOOR_COLORS
        if (gamestate.flags & GS_DRAW_FLOOR)
#endif
        {
            if (++floorpic >= NUM_TILES)
                floorpic = 0;

            LoadPlanes ();
        }
#ifdef CEILING_FLOOR_COLORS
        else
        {
            floorcolor++;
            floorcolor &= 0xff;
        }
#endif
        VW_WaitVBL (8);

        return true;
    }
#ifdef IN_DEVELOPMENT
    else if (Keyboard[sc_Y])
    {
        GivePoints (100000,false);

        return true;
    }
#endif
    if (gamestate.flags & GS_LIGHTING)   // Shading adjustments
    {
        if (Keyboard[sc_Equal] && normalshadediv < 12)
            normalshadediv++;
        else if (Keyboard[sc_Minus] && normalshadediv > 1)
            normalshadediv--;

        normalshade = (((viewwidth * 9) >> 4) - 3) / normalshadediv;

        if (Keyboard[sc_RBracket] && shademax < SHADEMAX)
            shademax++;
        else if (Keyboard[sc_LBracket] && shademax > 5)
            shademax--;

        VW_WaitVBL (8);    // don't go too fast
    }

    return false;
}
