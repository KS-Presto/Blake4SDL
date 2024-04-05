//--------------------------------------------------------------------------
//
//                             TEXT PRESENTER
//                    Programmed by Michael D. Maynard
//                  Copyright 1993, JAM Productions, Inc.
//                          All rights reserved.
//
// Active control codes:
//
//  ^ANnn   - define animation
//  ^AXnn   - alter X coordinate (nn = signed char)
//  ^AYnn   - alter Y coordinate (nn = signed char)
//  ^BCnn   - set background color
//  ^BE     - ring bell
//  ^BXn    - draw box around shape (0 = no box, 1 = box)
//  ^CE     - center text between 'left margin' and 'right margin'
//  ^DCnn   - dark color (dark-light for boxes)
//  ^DM     - use default margins
//  ^DSnn   - Display String Number 'nn'
//  ^EP     - end of page (waits for up/down arrow)
//  ^FCnn   - set font color
//  ^FNn    - set font number
//  ^HI     - hide cursor
//  ^HCnn   - highlight color
//  ^HF     - highlight color off
//  ^HO     - highlight color on
//  ^LCnn   - light color (highlight for boxes)
//  ^LJ     - left justify  --\ ^RJ doesn't handle imbedded control
//  ^LMnnn  - set left margin (if 'nnn' == "fff" uses current x)
//  ^MO     - wait for action from any input device
//  ^PA     - pause 1/2 second
//  ^PMnn   - Play Music Number 'nn'
//  ^PSnn   - Play Sound Number 'nn'
//  ^PXnnn  - move x to coordinate 'n'
//  ^PYnnn  - move y to coordinate 'n'
//  ^RMnnn  - set right margin (if 'nnn' == "fff" uses current x)
//  ^RJ     - right justify --/ codes properly. Use with caution.
//  ^RL     - restore cursor location.
//  ^RXn    - restore x location
//  ^RYn    - restore y location
//  ^SBn    - clear scaled background (0 = no clear, 1 = clear)
//  ^SCnn   - shadow color (shadow for text / pics)
//  ^SHnnn  - display shape 'n' at current x,y
//  ^SPn    - shadow pics (0 = no shadow, 1 = shadow)
//  ^STn    - shadow text (0 = no shadow, 1 = shadow)
//  ^SL     - save cursor location.
//  ^SXn    - save x location
//  ^SYn    - save y location
//  ^XX     - exit presenter
//  ^ZZ     - NOP ... This code is ignored.
//
// Other info:
//
// All 'n' values are hex numbers (0 - f), case insensitive.
// The number of N's listed is the number of digits REQUIRED by that control
// code. (IE: ^LMnnn MUST have 3 values! --> 003, 1a2, 01f, etc...)
//
// If a line consists only of control codes, the cursor is NOT advanced
// to the next line (the ending <CR><LF> is skipped). If just ONE non-control
// code is added, the number "8" for example, then the "8" is displayed
// and the cursor is advanced to the next line.
//
// ^CE must be on the same line as the text it should center!
//
// The text presenter now handles sprites, but they are NOT masked! Also,
// sprite animations will be difficult to implement unless all frames are
// of the same dimensions.
//
// You can comment-out a line by beginning the line with a semi-colon.
// Commented lines are ignored by the parser. Do not comment-out the last
// line of the file for this may cause harm to all things great and small!
//
//--------------------------------------------------------------------------

#include "3d_def.h"


//
// string array table is a quick, easy and expandable way to print
// any number of strings in a text file using the ^DS codes
//
// See Macro TP_INIT_DISPLAY_STR(num,str_ptr)      - JM_TP.h
// To init strings and handle range checking
//
char *piStringTable[PI_MAX_NUM_DISP_STRS];

//
// shape table provides a way for the presenter to access and
// display any shape
//
piShapeInfo piShapeTable[] =
{
    {SPR_GREEN_OOZE1,pis_scaled},   // 0 - Green Ooze

    {SPR_STAT_32,pis_scaled},       // 1 - Red Key
    {SPR_STAT_33,pis_scaled},       // 2 - Yellow Key
    {SPR_STAT_34,pis_scaled},       // 3 - Green Key
    {SPR_STAT_35,pis_scaled},       // 4 - Blue Key
    {SPR_STAT_36,pis_scaled},       // 5 - Gold Key
    {SPR_STAT_26,pis_scaled},       // 6 - Charge Unit
    {SPR_STAT_24,pis_scaled},       // 7 - PISTOL
    {SPR_STAT_27,pis_scaled},       // 8 - Auto-Burst Rifle
    {SPR_STAT_28,pis_scaled},       // 9 - Particle Charged ION
    {SPR_STAT_29,pis_scaled},       // 10 - First Aid
    {SPR_STAT_30,pis_scaled},       // 11 - Full Heal
    {SPR_STAT_48,pis_scaled},       // 12 - money bag
    {SPR_STAT_49,pis_scaled},       // 13 - loot
    {SPR_STAT_50,pis_scaled},       // 14 - gold
    {SPR_STAT_51,pis_scaled},       // 15 - bonus
    {SPR_STAT_42,pis_scaled},       // 16 - Chicken Leg
    {SPR_STAT_44,pis_scaled},       // 17 - Ham
    {SPR_STAT_40,pis_scaled},       // 18 - Full Water Bowl
    {SPR_STAT_0,pis_scaled},        // 19 - Water Puddle

    // ATTACKING ACTORS

    {SPR_RENT_W1_8,pis_scaled},     // 20 - rentacopobj, Walking
    {SPR_RENT_W2_8,pis_scaled},     // 21 - rentacopobj, Walking
    {SPR_RENT_W3_8,pis_scaled},     // 22 - rentacopobj, Walking
    {SPR_RENT_W4_8,pis_scaled},     // 23 - rentacopobj, Walking

    {SPR_TERROT_1,pis_scaled},      // 24 - hang_terrotobj, Rotating
    {SPR_TERROT_2,pis_scaled},      // 25 - hang_terrotobj, Rotating
    {SPR_TERROT_3,pis_scaled},      // 26 - hang_terrotobj, Rotating
    {SPR_TERROT_4,pis_scaled},      // 27 - hang_terrotobj, Rotating
    {SPR_TERROT_5,pis_scaled},      // 28 - hang_terrotobj, Rotating
    {SPR_TERROT_6,pis_scaled},      // 29 - hang_terrotobj, Rotating
    {SPR_TERROT_7,pis_scaled},      // 30 - hang_terrotobj, Rotating
    {SPR_TERROT_8,pis_scaled},      // 31 - hang_terrotobj, Rotating

    {SPR_STAT_77,pis_scaled},       // 32 - 1 Credit Food Unit Token
    {SPR_STAT_78,pis_scaled},       // 33 - 5 Credit Food Unit Token
    {SPR_STAT_79,pis_scaled},       // 34 - Auto-Charge Pistol

    {14*2,pis_scwall},              // 35 - Food Unit Wall
    {16*2,pis_scwall},              // 36 - Terminal Wall
    {20*2+1,pis_scwall},            // 37 - Teleport w/Pannel on it
    {44*2,pis_scwall},              // 38 - Barrier Switch Wall
    {31*2,pis_scwall},              // 39 - Warp Tile

    {BIGGOLDSTERNPIC,pis_pic},      // 40 - Big Goldstern
    {STARLOGOPIC,pis_pic},          // 41 - Star Logo
    {BLAKEWITHGUNPIC,pis_pic},      // 42 - Blake With Gun
    {STARINSTITUTEPIC,pis_pic},     // 43 - Star Institute
    {MEDALOFHONORPIC,pis_pic},      // 44 - Medal Of Honor
    {SMALLGOLDSTERNPIC,pis_pic},    // 45 - Small Goldstern
    {BLAKEWINPIC,pis_pic},          // 46 - Blake Win
    {AUTOMAPPIC,pis_pic},           // 47 - AutoMap Pic

    {SPR_ELEC_POST1,pis_scaled},    // 48 - Barrier Post 1
    {SPR_ELEC_POST2,pis_scaled},    // 49 - Barrier Post 2
    {SPR_ELEC_POST3,pis_scaled},    // 50 - Barrier Post 3

    {SHUTTLEEXPPIC,pis_pic},        // 51 - Shuttle Explode
    {PLANETSPIC,pis_pic},           // 52 - Planets Pic
    {SPR_CUBE1,pis_scaled},         // 53 - Rotating Cube

    {H_PPIC,pis_pic},               // 54 - M key
    {MOUSEPIC,pis_pic},             // 55 - Mouse Pad

    {SPR_BIO_W1_8,pis_scaled},      // 56 - gen_scientist, walking
    {SPR_BIO_W2_8,pis_scaled},      // 57 - gen_scientist, walking
    {SPR_BIO_W3_8,pis_scaled},      // 58 - gen_scientist, walking
    {SPR_BIO_W4_8,pis_scaled},      // 59 - gen_scientist, walking

    {SPR_POD_WALK1,pis_scaled},     // 60 - podalien - walking
    {SPR_POD_WALK2,pis_scaled},     // 61 - podalien - walking
    {SPR_POD_WALK3,pis_scaled},     // 62 - podalien - walking
    {SPR_POD_WALK4,pis_scaled},     // 63 - podalien - walking

    {SPR_ELEC_WALK1,pis_scaled},    // 64 - electro alien - walking
    {SPR_ELEC_WALK2,pis_scaled},    // 65 - electro alien - walking
    {SPR_ELEC_WALK3,pis_scaled},    // 66 - electro alien - walking
    {SPR_ELEC_WALK4,pis_scaled},    // 67 - electro alien - walking

    {SPR_ELECTRO_SPHERE_ROAM1,pis_scaled},    // 68 - electrosphere - Roaming
    {SPR_ELECTRO_SPHERE_ROAM2,pis_scaled},    // 69 - electrosphere - Roaming
    {SPR_ELECTRO_SPHERE_ROAM3,pis_scaled},    // 70 - electrosphere - Roaming

    {SPR_PRO_W1_8,pis_scaled},      // 71 - proguard - Walking
    {SPR_PRO_W2_8,pis_scaled},      // 72 - proguard - Walking
    {SPR_PRO_W3_8,pis_scaled},      // 73 - proguard - Walking
    {SPR_PRO_W4_8,pis_scaled},      // 74 - proguard - Walking

    {SPR_GENETIC_W1,pis_scaled},    // 75 - genetic_guard - Walking
    {SPR_GENETIC_W2,pis_scaled},    // 76 - genetic_guard - Walking
    {SPR_GENETIC_W3,pis_scaled},    // 77 - genetic_guard - Walking
    {SPR_GENETIC_W4,pis_scaled},    // 78 - genetic_guard - Walking

    {SPR_MUTHUM1_W1,pis_scaled},    // 79 - mutant_human1 - walking
    {SPR_MUTHUM1_W2,pis_scaled},    // 80 - mutant_human1 - walking
    {SPR_MUTHUM1_W3,pis_scaled},    // 81 - mutant_human1 - walking
    {SPR_MUTHUM1_W4,pis_scaled},    // 82 - mutant_human1 - walking

    {SPR_MUTHUM2_W1,pis_scaled},    // 83 - mutant_human2 - walking
    {SPR_MUTHUM2_W2,pis_scaled},    // 84 - mutant_human2 - walking
    {SPR_MUTHUM2_W3,pis_scaled},    // 85 - mutant_human2 - walking
    {SPR_MUTHUM2_W4,pis_scaled},    // 86 - mutant_human2 - walking

    {SPR_LCAN_ALIEN_READY,pis_scaled},    // 87 - Large canister for lg_Aln

    {SPR_LCAN_ALIEN_W1,pis_scaled},    // 88 - Large Canister Alien
    {SPR_LCAN_ALIEN_W2,pis_scaled},    // 89 - Large Canister Alien
    {SPR_LCAN_ALIEN_W3,pis_scaled},    // 90 - Large Canister Alien
    {SPR_LCAN_ALIEN_W4,pis_scaled},    // 91 - Large Canister Alien

    {SPR_SCAN_ALIEN_READY,pis_scaled}, // 92 - Small Cansiter

    {SPR_SCAN_ALIEN_W1,pis_scaled},    // 93 - Sm Can Alien - Walking
    {SPR_SCAN_ALIEN_W2,pis_scaled},    // 94 - Sm Can Alien - Walking
    {SPR_SCAN_ALIEN_W3,pis_scaled},    // 95 - Sm Can Alien - Walking
    {SPR_SCAN_ALIEN_W4,pis_scaled},    // 96 - Sm Can Alien - Walking

    {SPR_GURNEY_MUT_READY,pis_scaled}, // 97 - Gurney Mutant on gurney

    {SPR_GURNEY_MUT_W1,pis_scaled},    // 98  - Gurney Mutant - Walking
    {SPR_GURNEY_MUT_W2,pis_scaled},    // 99  - Gurney Mutant - Walking
    {SPR_GURNEY_MUT_W3,pis_scaled},    // 100 - Gurney Mutant - Walking
    {SPR_GURNEY_MUT_W4,pis_scaled},    // 101 - Gurney Mutant - Walking

    {SPR_LIQUID_S1,pis_scaled},        // 102 - liquid Alien - Shooting
    {SPR_LIQUID_S2,pis_scaled},        // 103 - liquid Alien - Shooting
    {SPR_LIQUID_S3,pis_scaled},        // 104 - liquid Alien - Shooting

    {SPR_SWAT_W1_8,pis_scaled},        // 105 - SWAT guard - Walking
    {SPR_SWAT_W2_8,pis_scaled},        // 106 - SWAT guard - Walking
    {SPR_SWAT_W3_8,pis_scaled},        // 107 - SWAT guard - Walking
    {SPR_SWAT_W4_8,pis_scaled},        // 108 - SWAT guard - Walking

    {SPR_GOLD_W1_8,pis_scaled},        // 109 - Dr Goldstern - Walking
    {SPR_GOLD_W2_8,pis_scaled},        // 110 - Dr Goldstern - Walking
    {SPR_GOLD_W3_8,pis_scaled},        // 111 - Dr Goldstern - Walking
    {SPR_GOLD_W4_8,pis_scaled},        // 112 - Dr Goldstern - Walking

    {JOYSTICKPIC,pis_pic},             // 113 - Joystick

    {SPR_GSCOUT_W1_8,pis_scaled},      // 114 - Volatile Transport - moving
    {SPR_GSCOUT_W2_8,pis_scaled},      // 115 - Volatile Transport - moving
    {SPR_GSCOUT_W3_8,pis_scaled},      // 116 - Volatile Transport - moving
    {SPR_GSCOUT_W4_8,pis_scaled},      // 117 - Volatile Transport - moving

    {SPR_FSCOUT_W1_8,pis_scaled},      // 118 - Floating Bomb - moving
    {SPR_FSCOUT_W2_8,pis_scaled},      // 119 - Floating Bomb - moving
    {SPR_FSCOUT_W3_8,pis_scaled},      // 120 - Floating Bomb - moving
    {SPR_FSCOUT_W4_8,pis_scaled},      // 121 - Floating Bomb - moving

    {SPR_CUBE1,pis_scaled},            // 122 - ** OPEN **

    {SPR_ELEC_SHOT1,pis_scaled},       // 123 - Electro Aln Shot - floating
    {SPR_ELEC_SHOT2,pis_scaled},       // 124 - Electro Aln Shot - floating

    {SPR_SECURITY_NORMAL,pis_scaled},     // 125 - Security_light - normal
    {SPR_SECURITY_ALERT,pis_scaled},      // 126 - Security_light - Alerted

    {SPR_LIQUID_SHOT_FLY_1,pis_scaled},   // 127 - Liquid Aln Sht - float'n
    {SPR_LIQUID_SHOT_FLY_2,pis_scaled},   // 128 - Liquid Aln Sht - float'n
    {SPR_LIQUID_SHOT_FLY_3,pis_scaled},   // 129 - Liquid Aln Sht - float'n

    {SPR_POD_EGG,pis_scaled},          // 130 - POD Alien Egg

    {SPR_DECO_ARC_1,pis_scaled},       // 131 - DecoArc
    {SPR_DECO_ARC_2,pis_scaled},       // 132 - DecoArc
    {SPR_DECO_ARC_3,pis_scaled},       // 133 - DecoArc

    {GRAVISPADPIC,pis_pic},            // 134 - Gravis Pad
    {H_MPIC,pis_pic},                  // 135 - M key

    {SPR_CANDY_BAR,pis_scaled},        // 136 - Candy bar
    {SPR_SANDWICH,pis_scaled},         // 137 - Sandwich

    {SPR_STAT_46,pis_scaled},          // 138 - Grenade Launcher
    {SPR_EXPLOSION_3,pis_scaled},      // 139 - General Explosion
    {SPR_BLACK_OOZE1,pis_scaled},      // 140 - Black Ooze

    {SPR_ELEC_ARC1,pis_scaled},        // 141 - Electric Arc Barrier 1
    {SPR_ELEC_ARC2,pis_scaled},        // 142 - Electric Arc Barrier 2
    {SPR_ELEC_ARC3,pis_scaled},        // 143 - Electric Arc Barrier 3

    {C_EPISODE1PIC,pis_pic},       // 144 - STAR Institute
    {C_EPISODE2PIC,pis_pic},       // 145 - Floating Fortress
    {C_EPISODE3PIC,pis_pic},       // 146 - Underground Network
    {C_EPISODE4PIC,pis_pic},       // 147 - STAR Port
    {C_EPISODE5PIC,pis_pic},       // 148 - Habitat II
    {C_EPISODE6PIC,pis_pic},       // 149 - Defense Center

    {SPR_BOSS1_W1,pis_scaled},     // 150 - Boss 1 - walking
    {SPR_BOSS1_W2,pis_scaled},     // 151 - Boss 1 - walking
    {SPR_BOSS1_W3,pis_scaled},     // 152 - Boss 1 - walking
    {SPR_BOSS1_W4,pis_scaled},     // 153 - Boss 1 - walking

    {SPR_BOSS2_W1,pis_scaled},     // 154 - Boss 2 - walking
    {SPR_BOSS2_W2,pis_scaled},     // 155 - Boss 2 - walking
    {SPR_BOSS2_W3,pis_scaled},     // 156 - Boss 2 - walking
    {SPR_BOSS2_W4,pis_scaled},     // 157 - Boss 2 - walking

    {SPR_BOSS3_W1,pis_scaled},     // 158 - Boss 3 - walking
    {SPR_BOSS3_W2,pis_scaled},     // 159 - Boss 3 - walking
    {SPR_BOSS3_W3,pis_scaled},     // 160 - Boss 3 - walking
    {SPR_BOSS3_W4,pis_scaled},     // 161 - Boss 3 - walking

    {SPR_BOSS4_W1,pis_scaled},     // 162 - Boss 4 - walking
    {SPR_BOSS4_W2,pis_scaled},     // 163 - Boss 4 - walking
    {SPR_BOSS4_W3,pis_scaled},     // 164 - Boss 4 - walking
    {SPR_BOSS4_W4,pis_scaled},     // 165 - Boss 4 - walking

    {SPR_BOSS5_W1,pis_scaled},     // 166 - Boss 5 - walking
    {SPR_BOSS5_W2,pis_scaled},     // 167 - Boss 5 - walking
    {SPR_BOSS5_W3,pis_scaled},     // 168 - Boss 5 - walking
    {SPR_BOSS5_W4,pis_scaled},     // 169 - Boss 5 - walking

    {SPR_BOSS6_W1,pis_scaled},     // 170 - Boss 6 - walking
    {SPR_BOSS6_W2,pis_scaled},     // 171 - Boss 6 - walking
    {SPR_BOSS6_W3,pis_scaled},     // 172 - Boss 6 - walking
    {SPR_BOSS6_W4,pis_scaled},     // 173 - Boss 6 - walking

    {H_ALTPIC,pis_pic},            // 174 - ALT key
    {H_CTRLPIC,pis_pic},           // 175 - CTRL key
    {H_SPACEPIC,pis_pic},          // 176 - SPACE key
    {H_PAUSEPIC,pis_pic},          // 177 - PAUSE key
    {H_ESCPIC,pis_pic},            // 178 - ESC key
    {H_LTARROWPIC,pis_pic},        // 179 - LEFT ARROW key
    {H_UPARROWPIC,pis_pic},        // 180 - UP ARROW key
    {H_DNARROWPIC,pis_pic},        // 181 - DOWN ARROW key
    {H_RTARROWPIC,pis_pic},        // 182 - RIGHT ARROW key
    {H_QPIC,pis_pic},              // 183 - Q key
    {H_WPIC,pis_pic},              // 184 - W key
    {H_EPIC,pis_pic},              // 185 - E key
    {H_IPIC,pis_pic},              // 186 - I key
    {H_HPIC,pis_pic},              // 187 - H key
    {H_1PIC,pis_pic},              // 188 - 1 key
    {H_2PIC,pis_pic},              // 189 - 2 key
    {H_3PIC,pis_pic},              // 190 - 3 key
    {H_4PIC,pis_pic},              // 191 - 4 key
    {H_5PIC,pis_pic},              // 192 - 5 key
    {H_F1PIC,pis_pic},             // 193 - F1 key
    {H_F2PIC,pis_pic},             // 194 - F2 key
    {H_F3PIC,pis_pic},             // 195 - F3 key
    {H_F4PIC,pis_pic},             // 196 - F4 key
    {H_F5PIC,pis_pic},             // 197 - F5 key
    {H_F6PIC,pis_pic},             // 198 - F6 key
    {H_F7PIC,pis_pic},             // 199 - F7 key
    {H_F8PIC,pis_pic},             // 200 - F8 key
    {H_F9PIC,pis_pic},             // 201 - F9 key
    {H_F10PIC,pis_pic},            // 202 - F10 key
    {H_ENTERPIC,pis_pic},          // 203 - ENTER key
    {H_TABPIC,pis_pic},            // 204 - TAB key
    {H_CPIC,pis_pic},              // 205 - C key
    {H_FPIC,pis_pic},              // 206 - F key
    {SPR_CRATE_1,pis_scaled},      // 207 - Crate
    {SPR_GSCOUT_W1_1,pis_scaled},  // 208 - VMT
    {SPR_FSCOUT_W1_1,pis_scaled},  // 209 - PerScan
    {APOGEE_LOGOPIC,pis_pic},      // 210 - Apogee Logo
    {VISAPIC,pis_pic},             // 211 - Visa Card
    {MCPIC,pis_pic},               // 212 - Master Card
    {FAXPIC, pis_pic},             // 213 - Fax Machine
    {H_LPIC, pis_pic},             // 214 - L key
    {H_SHIFTPIC, pis_pic},         // 215 - Shift key
    {SPR_DOORBOMB,pis_scaled},     // 216 - DoorBomb
    {SPR_AUTOMAPPER,pis_scaled},   // 217 - Radar Powerup.
    {STARPORTPIC,pis_pic},         // 218 - Star Port
    {BOSSPIC,pis_pic},             // 219 - AOG Boss
    {THREEPLANETSPIC,pis_pic},     // 220 - Three Planets
    {SOLARSYSTEMPIC,pis_pic},      // 221 - Solar System
    {AOGENDINGPIC,pis_pic},        // 222 - AOG Ending
    {GFLOGOSPIC,pis_pic},          // 223 - Goldfire 'n Logos
    {BLAKEHEADPIC,pis_pic},        // 224 - Blake's Big Head
    {PROJECTFOLDERPIC,pis_pic},    // 225 - Project Folder
    {SPR_GREEN2_OOZE1,pis_scaled}, // 226 - Green2 Ooze
    {SPR_BLACK2_OOZE1,pis_scaled}, // 227 - Black2 Ooze
    {SPR_STAT_34,pis_scaled},      // 228 - BFG Cannon AKA Anti-Plasma Cannon
    {H_PLUSPIC,pis_pic},           // 229 - PLUS key
    {SPR_DOORBOMB,pis_scaled},     // 230 - Plasma Detonator
    {SPR_DETONATOR_EXP3,pis_scaled},    // 231 - Plasma Detonator Explosion
    {SPR_MGOLD_WALK1,pis_scaled},       // 232 - Morphed Dr Goldstern - Walking
    {SPR_MGOLD_WALK2,pis_scaled},       // 233 - Morphed Dr Goldstern - Walking
    {SPR_MGOLD_WALK3,pis_scaled},       // 234 - Morphed Dr Goldstern - Walking
    {SPR_MGOLD_WALK4,pis_scaled},       // 235 - Morphed Dr Goldstern - Walking
    {H_MINUSPIC,pis_pic},           // 236 - MINUS key
    {H_6PIC,pis_pic},               // 237 - 6 KEY
    {H_TILDEPIC,pis_pic},           // 238 - TILDE KEY
    {AUTOMAP_MAG1PIC,pis_pic},      // 239 - AutoMap pic MAG1 (normal)
    {AUTOMAP_MAG2PIC,pis_pic},      // 240 - AutoMap pic MAG2
    {AUTOMAP_MAG4PIC,pis_pic},      // 241 - AutoMap pic MAG4
    {SPR_VPOST1,pis_scaled},        // 242 - VPost Barrier
    {SPR_VPOST2,pis_scaled},        // 243 - VPost Barrier
    {SPR_VPOST3,pis_scaled},        // 244 - VPost Barrier
    {SPR_VPOST4,pis_scaled},        // 245 - VPost Barrier
    {SPR_VPOST5,pis_scaled},        // 246 - VPost Barrier
    {SPR_VPOST6,pis_scaled},        // 247 - VPost Barrier
    {SPR_VPOST7,pis_scaled},        // 248 - VPost Barrier
    {SPR_VPOST8,pis_scaled},        // 249 - VPost Barrier
    {SPR_VSPIKE1,pis_scaled},       // 250 - VSpike Barrier
    {SPR_VSPIKE2,pis_scaled},       // 251 - VSpike Barrier
    {SPR_VSPIKE3,pis_scaled},       // 252 - VSpike Barrier
    {SPR_VSPIKE4,pis_scaled},       // 253 - VSpike Barrier
    {SPR_VSPIKE5,pis_scaled},       // 254 - VSpike Barrier
    {SPR_VSPIKE6,pis_scaled},       // 255 - VSpike Barrier
    {SPR_VSPIKE7,pis_scaled},       // 256 - VSpike Barrier
    {SPR_VSPIKE8,pis_scaled},       // 257 - VSpike Barrier
    {SPR_CUBE1,pis_scaled},         // 258 - Security Cube - Rotating
    {SPR_CUBE2,pis_scaled},         // 259 - Security Cube - Rotating
    {SPR_CUBE3,pis_scaled},         // 260 - Security Cube - Rotating
    {SPR_CUBE4,pis_scaled},         // 261 - Security Cube - Rotating
    {SPR_CUBE5,pis_scaled},         // 262 - Security Cube - Rotating
    {SPR_CUBE6,pis_scaled},         // 263 - Security Cube - Rotating
    {SPR_CUBE7,pis_scaled},         // 264 - Security Cube - Rotating
    {SPR_CUBE8,pis_scaled},         // 265 - Security Cube - Rotating
    {SPR_CUBE9,pis_scaled},         // 266 - Security Cube - Rotating
    {SPR_CUBE10,pis_scaled},        // 267 - Security Cube - Rotating

    {SPR_BOSS7_W1,pis_scaled},      // 268 - Final Boss 1 Walking
    {SPR_BOSS7_W2,pis_scaled},      // 269 - Final Boss 1 Walking
    {SPR_BOSS7_W3,pis_scaled},      // 270 - Final Boss 1 Walking
    {SPR_BOSS7_W4,pis_scaled},      // 271 - Final Boss 1 Walking

    {SPR_BOSS8_W1,pis_scaled},      // 272 - Final Boss 2 Walking
    {SPR_BOSS8_W2,pis_scaled},      // 273 - Final Boss 2 Walking
    {SPR_BOSS8_W3,pis_scaled},      // 274 - Final Boss 2 Walking
    {SPR_BOSS8_W4,pis_scaled},      // 275 - Final Boss 2 Walking

    {SPR_BOSS9_W1,pis_scaled},      // 276 - Final Boss 3 Walking
    {SPR_BOSS9_W2,pis_scaled},      // 277 - Final Boss 3 Walking
    {SPR_BOSS9_W3,pis_scaled},      // 278 - Final Boss 3 Walking
    {SPR_BOSS9_W4,pis_scaled},      // 279 - Final Boss 3 Walking

    {SPR_BOSS10_W1,pis_scaled},     // 280 - Final Boss 4 Walking
    {SPR_BOSS10_W2,pis_scaled},     // 281 - Final Boss 4 Walking
    {SPR_BOSS10_W3,pis_scaled},     // 282 - Final Boss 4 Walking
    {SPR_BOSS10_W4,pis_scaled},     // 283 - Final Boss 4 Walking
};

//
// anim table holds info about each different animation
//
piAnimInfo piAnimTable[] =
{
    {136,0,2,0,20,pia_shapetable,pid_cycle},    // 0 - OPEN
    {127,0,3,0,20,pia_shapetable,pid_cycle},    // 1 - podeggobj,
    {123,0,2,0,20,pia_shapetable,pid_cycle},    // 2 - electroshotobj,
    {126,0,1,0,20,pia_shapetable,pid_cycle},    // 3 - security_lightobj - Alerted

    {20,0,4,0,20,pia_shapetable,pid_cycle},     // 4 - rentacopobj,
    {24,0,8,0,20,pia_shapetable,pid_cycle},     // 5 - hang_terrotobj,
    {242,0,8,0,20,pia_shapetable,pid_rebound},  // 6 -   VPost
    {250,0,8,0,20,pia_shapetable,pid_rebound},  // 7 -  VSpike
    {258,0,10,0,10,pia_shapetable,pid_cycle},   // 8 -   Security Cube
    {56,0,4,0,20,pia_shapetable,pid_cycle},     // 9 - gen_scientistobj,
    {60,0,4,0,20,pia_shapetable,pid_cycle},     // 10 - podobj,
    {64,0,4,0,20,pia_shapetable,pid_cycle},     // 11 - electroobj,
    {68,0,3,0,20,pia_shapetable,pid_cycle},     // 12 - electrosphereobj,
    {71,0,4,0,20,pia_shapetable,pid_cycle},     // 13 -  proguardobj,
    {75,0,4,0,20,pia_shapetable,pid_cycle},     // 14 - genetic_guardobj,
    {79,0,4,0,20,pia_shapetable,pid_cycle},     // 15 - mutant_human1obj,
    {83,0,4,0,20,pia_shapetable,pid_cycle},     // 16 - mutant_human2obj,
    {87,0,1,0,20,pia_shapetable,pid_cycle},     // 17 - lcan_wait_alienobj,
    {88,0,4,0,20,pia_shapetable,pid_cycle},     // 18 - lcan_alienobj,
    {92,0,1,0,20,pia_shapetable,pid_cycle},     // 19 - scan_wait_alienobj,
    {93,0,4,0,20,pia_shapetable,pid_cycle},     // 20 - scan_alienobj,
    {97,0,1,0,20,pia_shapetable,pid_cycle},     // 21 -  gurney_waitobj,
    {98,0,4,0,20,pia_shapetable,pid_cycle},     // 22 - gurneyobj,
    {102,0,3,0,20,pia_shapetable,pid_cycle},    // 23 - liquidobj,
    {105,0,4,0,20,pia_shapetable,pid_cycle},    // 24 - swatobj,
    {109,0,4,0,20,pia_shapetable,pid_cycle},    // 25 - goldsternobj,
    {113,0,1,0,20,pia_shapetable,pid_cycle},    // 26 - OPEN

    {131,0,3,0,20,pia_shapetable,pid_cycle},    // 27 - flickerlightobj,
    {134,0,2,0,20,pia_shapetable,pid_cycle},    // 28 - playerspshotobj,

    {141,0,3,0,20,pia_shapetable,pid_cycle},    // 29 -  Electric Arc barrier

    {150,0,4,0,20,pia_shapetable,pid_cycle},    // 30 -  Boss 1
    {154,0,4,0,20,pia_shapetable,pid_cycle},    // 31 -  Boss 2
    {158,0,4,0,20,pia_shapetable,pid_cycle},    // 32 -  Boss 3
    {162,0,4,0,20,pia_shapetable,pid_cycle},    // 33 -  Boss 4
    {166,0,4,0,20,pia_shapetable,pid_cycle},    // 34 -  Boss 5
    {170,0,4,0,20,pia_shapetable,pid_cycle},    // 35 -  Boss 6

    {48,0,3,0,10,pia_shapetable,pid_cycle},     // 36 -  Barrier Post
    {141,0,3,0,10,pia_shapetable,pid_cycle},    // 37 -  Barrier Arc

    {208,0,8,0,10,pia_grabscript,pid_cycle},    // 38 -  VMT
    {209,0,8,0,8,pia_grabscript,pid_cycle},     // 39 -  PerScan

    {232,0,4,0,20,pia_shapetable,pid_cycle},    // 40 - Dr. Goldstern Morph Walking

    {268,0,4,0,20,pia_shapetable,pid_cycle},    // 41 - Final Boss #1 - Walking
    {272,0,4,0,20,pia_shapetable,pid_cycle},    // 42 - Final Boss #2 - Walking
    {276,0,4,0,20,pia_shapetable,pid_cycle},    // 43 - Final Boss #3 - Walking
    {280,0,4,0,20,pia_shapetable,pid_cycle},    // 44 - Final Boss #4 - Walking
};

enum
{
    jm_left,
    jm_right,
    jm_flush,
};

//
// anim list is created on the fly from the anim table
// this allows a single animation to be displayed in more than
// one place
//
piAnimInfo      piAnimList[TP_MAX_ANIMS];
byte            TPscan;

static int      old_fontnumber;
static int      length;

static int      justify_mode = jm_left;

static unsigned flags;

static int      bgcolor,ltcolor,dkcolor,shcolor;
static int      anim_bgcolor = -1;
static int      xl,yl,xh,yh;
static int      cur_x,cur_y,last_cur_x,last_cur_y;

static char     *first_ch;
static char     *scan_ch,temp;
static int      scan_x,numanims,stemp;

static fontstruct *font;

static PresenterInfo *pi;

static int      disp_str_num = -1;
static int      music_num;
static int      save_cx[TP_CURSOR_SAVES + 1];
static int      save_cy[TP_CURSOR_SAVES + 1];
static int      pagex[2],pagey[2];


/*
====================
=
= TP_Presenter
=
====================
*/

void TP_Presenter (PresenterInfo *pinfo)
{
    int oldf,oldc;

    pi = pinfo;

    bgcolor = pi->bgcolor;
    ltcolor = pi->ltcolor;
    dkcolor = pi->dkcolor;
    shcolor = pi->shcolor;
    xl = pi->xl + TP_MARGIN;
    yl = pi->yl + TP_MARGIN;
    xh = pi->xh - TP_MARGIN;
    yh = pi->yh - TP_MARGIN;

    music_num = -1;

    flags |= fl_clearscback;

    if ((pi->flags & TPF_USE_CURRENT) && pi->cur_x != -1 && pi->cur_y != -1)
    {
        if (pi->flags & TPF_SHOW_CURSOR)
        {
            cur_x = px;
            cur_y = py;
        }
        else
        {
            cur_x = pi->cur_x;
            cur_y = pi->cur_y;
        }
    }
    else
    {
        cur_x = xl;
        cur_y = yl;
    }

    first_ch = pi->script[0];
    pi->pagenum = numanims = 0;
    disp_str_num = -1;

    old_fontnumber = fontnumber;
    fontnumber = pi->fontnumber;

    TP_PurgeAllGfx ();
    TP_CachePage (first_ch);

    font = (fontstruct *)grsegs[STARTFONT + fontnumber];
    flags = fl_presenting | fl_startofline;

    if (*first_ch == TP_CONTROL_CHAR)
        TP_HandleCodes ();

    //
    // display info UNDER defined region
    //
    if (pi->infoline)
    {
        oldf = fontnumber;
        oldc = fontcolor;

        px = xl;
        py = yh + TP_MARGIN + 1;
        fontnumber = 2;
        fontcolor = 0x39;

        VW_Bar (xl - TP_MARGIN,py,xh - xl + 1 + (TP_MARGIN * 2),8,bgcolor);
        ShPrint (pi->infoline,shcolor,false);

        if (pi->flags & TPF_SHOW_PAGES)
        {
            px = 246;
            py = 190;
            ShPrint ("PAGE ",shcolor,false);
            pagex[0] = px;
            pagey[0] = py;
            ShPrint ("   OF ",shcolor,false);
            pagex[1] = px;
            pagey[1] = py;

            TP_PrintPageNumber ();
        }

        fontcolor = oldc;
        fontnumber = oldf;
    }

    font = (fontstruct *)grsegs[STARTFONT + fontnumber];

    if (!(pi->flags & TPF_USE_CURRENT))
        VW_Bar (xl - TP_MARGIN,yl - TP_MARGIN,xh - xl + 1 + (TP_MARGIN * 2),yh - yl + 1 + (TP_MARGIN * 2),bgcolor);

    if (pi->flags & TPF_SHOW_CURSOR)
    {
        px = cur_x;
        py = cur_y;

        TP_Print ("@",true);
    }

    //
    // debug stuff -- draws box AROUND text presenter's printable region
    //
    //VW_Hlin (xl - TP_MARGIN,xh + (TP_MARGIN * 2),yl - TP_MARGIN,255);
    //VW_Vlin (yl - TP_MARGIN,yh + (TP_MARGIN * 2),xh + TP_MARGIN,255);
    //VW_Hlin (xl - TP_MARGIN,xh + (TP_MARGIN * 2),yh + TP_MARGIN,255);
    //VW_Vlin (yl - TP_MARGIN,yh + (TP_MARGIN * 2),xl - TP_MARGIN,255);

    while (flags & fl_presenting)
    {
        if (*first_ch == TP_CONTROL_CHAR)
            TP_HandleCodes ();
        else
            TP_WrapText ();
    }

    fontnumber = old_fontnumber;
    pi->cur_x = cur_x;
    pi->cur_y = cur_y;

    if (pi->flags & TPF_SHOW_CURSOR)
    {
        cur_x = px = last_cur_x;
        cur_y = py = last_cur_y;
    }
    else
    {
        px = cur_x;
        py = cur_y;
    }

    pi->cur_x = cur_x;
    pi->cur_y = cur_y;
}


/*
====================
=
= TP_NewLine
=
====================
*/

void TP_NewLine (void)
{
    int oldc;
    int width,height;

    flags &= ~fl_center;

    //
    // skip SPACE at end of wrapped line
    //
    if (first_ch[0] == ' ' && first_ch[1] != ' ')
        first_ch++;

    //
    // skip end-of-line designators
    //
    if (first_ch[0] == TP_RETURN_CHAR)
    {
        if (first_ch[1] == '\n')
            first_ch += 2;
        else
            first_ch++;
    }

    //
    // TP_CONTROL_CHARs don't advance to next character line
    //
    if ((*scan_ch != TP_CONTROL_CHAR) && *scan_ch)
    {
        oldc = fontcolor;

        //
        // remove cursor
        //
        if (pi->flags & TPF_SHOW_CURSOR)
        {
            fontcolor = bgcolor;
            px = last_cur_x;
            py = last_cur_y;
            TP_Print ("@",true);
            fontcolor = oldc;
        }

        cur_x = xl;

        //
        // if next line will be printed out of defined region, scroll up
        //
        if ((pi->flags & TPF_SCROLL_REGION) && cur_y + (font->height * 2) > yh)
        {
            //
            // copy the rectangle below the first line and draw it
            // at the top of the rectangle, clearing whatever was on
            // the last line
            //
            width = xh - xl + 1;
            height = (yh - yl + 1) - font->height + is_shadowed;

            VW_ScreenToScreen (width,height,xl,yl,font->height + is_shadowed,0);

            VW_Bar (cur_x,cur_y,width + (TP_MARGIN * 2),yh - cur_y + 1,bgcolor);

            if (cur_y + font->height > yh)
                cur_y = yh - font->height + 1 - is_shadowed;
        }
        else
            cur_y += font->height + is_shadowed;

        //
        // display cursor
        //
        if (pi->flags & TPF_SHOW_CURSOR)
        {
            px = cur_x;
            py = cur_y;
            TP_Print ("@",true);
        }
    }
}


/*
====================
=
= TP_WrapText
=
====================
*/

void TP_WrapText (void)
{
    int  last_x;
    word w,h;
    char *last_ch;

    flags &= ~fl_startofline;

    stemp = TP_LineCommented(first_ch);

    if (stemp)
    {
        first_ch += stemp;
        return;
    }

    //
    // parse script until one of the following:
    //
    // 1) text extends beyond right margin
    // 2) NULL termination is reached
    // 3) TP_RETURN_CHAR is reached
    // 4) TP_CONTROL_CHAR is reached
    //
    scan_x = cur_x;
    scan_ch = first_ch;

    while (scan_x + font->width[(byte)*scan_ch] <= xh && *scan_ch && *scan_ch != TP_RETURN_CHAR && *scan_ch != TP_CONTROL_CHAR)
        scan_x += font->width[(byte)*scan_ch++];

    //
    // if 'text extends beyond right margin', scan backwards for a SPACE
    //
    if (scan_x + font->width[(byte)*scan_ch] > xh)
    {
        last_x = scan_x;
        last_ch = scan_ch;

        while (scan_ch != first_ch && *scan_ch != ' ' && *scan_ch != TP_RETURN_CHAR)
            scan_x -= font->width[(byte)*scan_ch--];

        if (scan_ch == first_ch)
        {
            if (cur_x != xl)
            {
                TP_NewLine ();

                return;
            }

            scan_ch = last_ch;
            scan_x = last_x;
        }
    }

    //
    // print current line
    //
    temp = *scan_ch;
    *scan_ch = '\0';

    if (justify_mode == jm_right && !(flags & fl_center))
    {
        VW_MeasureString (first_ch,&w,&h);

        cur_x = xh - w + 1;

        if (cur_x < xl)
            cur_x = xl;
    }

    px = cur_x;
    py = cur_y;

    length = scan_ch - first_ch + 1;

    if (*first_ch != TP_RETURN_CHAR)
    {
        if (pi->print_delay)
            TP_SlowPrint (first_ch,pi->print_delay);
        else
            TP_Print (first_ch,false);
    }

    *scan_ch = temp;
    first_ch = scan_ch;

    TP_NewLine ();
}


/*
====================
=
= TP_HandleCodes
=
====================
*/

void TP_HandleCodes (void)
{
    #define TP_MORE_TEXT "<MORE>"

    ControlInfo ci;
    piAnimInfo  *anim;
    piShapeInfo *shape;
    int         i,shapenum;
    int         code,length;
    int         pos,oldbgcolor;
    unsigned    temp;
    char        *s,*old_first_ch;
    char        c;

    if (first_ch[-2] == TP_RETURN_CHAR && first_ch[-1] == '\n')
        flags |= fl_startofline;

    while (*first_ch == TP_CONTROL_CHAR)
    {
        first_ch++;

        code = ReadShort(first_ch);

        first_ch += 2;

        switch (code)
        {
            //
            // CENTER TEXT
            //
            case TP_CNVT_CODE('C','E'):
                length = 0;
                s = first_ch;

                while (*s && (*s != TP_RETURN_CHAR))
                {
                    switch (*s)
                    {
                        case TP_CONTROL_CHAR:
                            s++;
                            //
                            // TODO: triple nested switch eww
                            //
                            code = ReadShort(s);

                            s += 2;

                            switch (code)
                            {
                                case TP_CNVT_CODE('S','X'):
                                case TP_CNVT_CODE('R','X'):
                                case TP_CNVT_CODE('S','Y'):
                                case TP_CNVT_CODE('R','Y'):
                                case TP_CNVT_CODE('F','N'):
                                case TP_CNVT_CODE('S','T'):
                                case TP_CNVT_CODE('B','X'):
                                case TP_CNVT_CODE('S','P'):
                                    s++;
                                    break;

                                case TP_CNVT_CODE('F','C'):
                                case TP_CNVT_CODE('B','C'):
                                case TP_CNVT_CODE('S','C'):
                                case TP_CNVT_CODE('L','C'):
                                case TP_CNVT_CODE('D','C'):
                                case TP_CNVT_CODE('A','X'):
                                case TP_CNVT_CODE('A','Y'):
                                case TP_CNVT_CODE('H','C'):
                                    s += 2;
                                    break;

                                case TP_CNVT_CODE('L','M'):
                                case TP_CNVT_CODE('R','M'):
                                case TP_CNVT_CODE('P','X'):
                                case TP_CNVT_CODE('P','Y'):
                                    s += 3;
                                    break;

                                case TP_CNVT_CODE('S','H'):
                                    shapenum = TP_VALUE(first_ch,3);
                                    s += 3;
                                    shape = &piShapeTable[shapenum];
                                    length += TP_BoxAroundShape(-1,-1,shape->shapenum,shape->shapetype);
                                    break;

                                case TP_CNVT_CODE('A','N'):
                                    shapenum = TP_VALUE(first_ch,2);
                                    s += 2;
                                    anim = &piAnimTable[shapenum];
                                    //
                                    // TODO: quad nested switch what in the hell
                                    //
                                    switch (anim->animtype)
                                    {
                                        case pia_shapetable:
                                            shape = &piShapeTable[anim->baseshape+anim->frame];
                                            length += TP_BoxAroundShape(-1,-1,shape->shapenum,shape->shapetype);
                                            break;

                                        case pia_grabscript:
                                            shape = &piShapeTable[anim->baseshape];
                                            length += TP_BoxAroundShape(-1,-1,shape->shapenum + anim->frame,shape->shapetype);
                                            break;
                                    }
                                    break;

                                case TP_CNVT_CODE('Z','Z'):
                                case TP_CNVT_CODE('D','M'):
                                case TP_CNVT_CODE('C','E'):
                                case TP_CNVT_CODE('E','P'):
                                case TP_CNVT_CODE('L','J'):
                                case TP_CNVT_CODE('R','J'):
                                case TP_CNVT_CODE('X','X'):
                                case TP_CNVT_CODE('S','L'):
                                case TP_CNVT_CODE('R','L'):
                                case TP_CNVT_CODE('B','E'):
                                case TP_CNVT_CODE('H','I'):
                                case TP_CNVT_CODE('P','A'):
                                case TP_CNVT_CODE('M','O'):
                                case TP_CNVT_CODE('H','O'):
                                case TP_CNVT_CODE('H','F'):
                                case TP_CNVT_CODE('S','B'):
                                    break;
                            }
                            break;

                        default:
                            length += font->width[(byte)*s++];
                            break;
                    }
                }

                //
                // make sure we don't go out of bounds
                //
                pos = ((xh - cur_x + 1) - length) / 2;

                if (pos < 0)
                    pos = -pos;

                cur_x += pos;

                if (cur_x >= screen.basewidth)
                    cur_x = screen.basewidth - 1;

                flags |= fl_center;

                if (pi->flags & TPF_SHOW_CURSOR)
                    TP_JumpCursor ();
                break;

            //
            // DRAW SHAPE
            //
            case TP_CNVT_CODE('S','H'):
                shapenum = TP_VALUE(first_ch,3);
                first_ch += 3;
                shape = &piShapeTable[shapenum];
                TP_DrawShape (cur_x,cur_y,shape->shapenum,shape->shapetype);
                break;

            //
            // CLEAR SCALED BACKGROUND
            //
            case TP_CNVT_CODE('S','B'):
                if (TP_VALUE(first_ch++,1))
                    flags |= fl_clearscback;
                else
                    flags &= ~fl_clearscback;
                break;

            //
            // HIGHLIGHT COLOR
            //
            case TP_CNVT_CODE('H','C'):
                pi->highlight_color = TP_VALUE(first_ch,2);
                first_ch += 2;
                break;

            //
            // HIGHLIGHT ON
            //
            case TP_CNVT_CODE('H','O'):
                pi->fontcolor = fontcolor;
                fontcolor = pi->highlight_color;
                break;

            //
            // HIGHLIGHT OFF
            //
            case TP_CNVT_CODE('H','F'):
                fontcolor = pi->fontcolor;
                break;

            //
            // ALTER X
            //
            case TP_CNVT_CODE('A','X'):
                c = TP_VALUE(first_ch,2);
                first_ch += 2;
                cur_x += c;
                break;

            //
            // ALTER Y
            //
            case TP_CNVT_CODE('A','Y'):
                c = TP_VALUE(first_ch,2);
                first_ch += 2;
                cur_y += c;
                break;

            //
            // INIT ANIMATION
            //
            case TP_CNVT_CODE('A','N'):
                shapenum = TP_VALUE(first_ch,2);
                first_ch += 2;
                memcpy (&piAnimList[numanims],&piAnimTable[shapenum],sizeof(piAnimInfo));
                anim = &piAnimList[numanims++];

                anim->y = cur_y;

                switch (anim->animtype)
                {
                    case pia_shapetable:
                        shape = &piShapeTable[anim->baseshape + anim->frame];
                        anim->x = TP_DrawShape(cur_x,cur_y,shape->shapenum,shape->shapetype);
                        break;

                    case pia_grabscript:
                        shape = &piShapeTable[anim->baseshape];
                        anim->x = TP_DrawShape(cur_x,cur_y,shape->shapenum + anim->frame,shape->shapetype);
                        break;
                }

                anim->diradd = 1;

                if (anim_bgcolor == -1)
                    anim_bgcolor = bgcolor;
                break;

            //
            // FONT COLOR
            //
            case TP_CNVT_CODE('F','C'):
                fontcolor = TP_VALUE(first_ch,2);
                first_ch += 2;
                break;

            //
            // SHADOW COLOR
            //
            case TP_CNVT_CODE('S','C'):
                shcolor = TP_VALUE(first_ch,2);
                first_ch += 2;
                break;

            //
            // LIGHT COLOR
            //
            case TP_CNVT_CODE('L','C'):
                ltcolor = TP_VALUE(first_ch,2);
                first_ch += 2;
                break;

            //
            // SAVE LOCATION
            //
            case TP_CNVT_CODE('S','L'):
                save_cx[TP_CURSOR_SAVES] = cur_x;
                save_cy[TP_CURSOR_SAVES] = cur_y;
                break;

            //
            // RESTORE LOCATION
            //
            case TP_CNVT_CODE('R','L'):
                cur_x = save_cx[TP_CURSOR_SAVES];
                cur_y = save_cy[TP_CURSOR_SAVES];

                if (pi->flags & TPF_SHOW_CURSOR)
                    TP_JumpCursor ();
                break;

            //
            // SAVE X LOCATION
            //
            case TP_CNVT_CODE('S','X'):
                temp = TP_VALUE(first_ch++,1);

                if (pi->flags & TPF_SHOW_CURSOR)
                    save_cx[temp] = last_cur_x;
                else
                    save_cx[temp] = cur_x;
                break;

            //
            // RESTORE X LOCATION
            //
            case TP_CNVT_CODE('R','X'):
                temp = TP_VALUE(first_ch++,1);
                cur_x = save_cx[temp];

                if (pi->flags & TPF_SHOW_CURSOR)
                    TP_JumpCursor ();
                break;

            //
            // SAVE Y LOCATION
            //
            case TP_CNVT_CODE('S','Y'):
                temp = TP_VALUE(first_ch++,1);

                if (pi->flags & TPF_SHOW_CURSOR)
                    save_cy[temp] = last_cur_y;
                else
                    save_cy[temp] = cur_y;
                break;

            //
            // RESTORE Y LOCATION
            //
            case TP_CNVT_CODE('R','Y'):
                temp = TP_VALUE(first_ch++,1);
                cur_y = save_cy[temp];

                if (pi->flags & TPF_SHOW_CURSOR)
                    TP_JumpCursor ();
                break;

            //
            // NOP
            //
            case TP_CNVT_CODE('Z','Z'):
                break;

            //
            // DARK COLOR
            //
            case TP_CNVT_CODE('D','C'):
                dkcolor = TP_VALUE(first_ch,2);  // ^ bgcolor;
                first_ch += 2;
                break;

            //
            // FONT NUMBER
            //
            case TP_CNVT_CODE('F','N'):
                fontnumber = TP_VALUE(first_ch++,1);
                font = (fontstruct *)grsegs[STARTFONT + fontnumber];
                break;

            //
            // BACKGROUND COLOR
            //
            case TP_CNVT_CODE('B','C'):
                bgcolor = TP_VALUE(first_ch,2);
                first_ch += 2;
                break;

            //
            // SHADOW TEXT
            //
            case TP_CNVT_CODE('S','T'):
                if (TP_VALUE(first_ch++,1))
                    flags |= fl_shadowtext;
                else
                    flags &= ~fl_shadowtext;
                break;

            //
            // SHADOW PIC
            //
            case TP_CNVT_CODE('S','P'):
                if (TP_VALUE(first_ch++,1))
                    flags |= fl_shadowpic;
                else
                    flags &= ~fl_shadowpic;
                break;

            //
            // BOX SHAPES
            //
            case TP_CNVT_CODE('B','X'):
                if (TP_VALUE(first_ch++,1))
                    flags |= fl_boxshape;
                else
                    flags &= ~fl_boxshape;
                break;

            //
            // LEFT MARGIN
            //
            case TP_CNVT_CODE('L','M'):
                shapenum = TP_VALUE(first_ch,3);
                first_ch += 3;

                if (shapenum == 0xfff)
                    xl = cur_x;
                else
                    xl = shapenum;

                if (cur_x < xl)
                    cur_x = xl;
                break;

            //
            // RIGHT MARGIN
            //
            case TP_CNVT_CODE('R','M'):
                shapenum = TP_VALUE(first_ch,3);
                first_ch += 3;

                if (shapenum == 0xfff)
                    xh = cur_x;
                else
                    xh = shapenum;
                break;

            //
            // DEFAULT MARGINS
            //
            case TP_CNVT_CODE('D','M'):
                xl = pi->xl + TP_MARGIN;
                yl = pi->yl + TP_MARGIN;
                xh = pi->xh - TP_MARGIN;
                yh = pi->yh - TP_MARGIN;
                break;

            //
            // SET X COORDINATE
            //
            case TP_CNVT_CODE('P','X'):
                cur_x = TP_VALUE(first_ch,3);
                first_ch += 3;

                if (pi->flags & TPF_SHOW_CURSOR)
                    TP_JumpCursor ();
                break;

            //
            // SET Y COORDINATE
            //
            case TP_CNVT_CODE('P','Y'):
                cur_y = TP_VALUE(first_ch,3);
                first_ch += 3;

                if (pi->flags & TPF_SHOW_CURSOR)
                    TP_JumpCursor ();
                break;

            //
            // LEFT JUSTIFY
            //
            case TP_CNVT_CODE('L','J'):
                justify_mode = jm_left;
                break;

            //
            // RIGHT JUSTIFY
            //
            case TP_CNVT_CODE('R','J'):
                justify_mode = jm_right;
                break;

            //
            // BELL
            //
            case TP_CNVT_CODE('B','E'):
                SD_PlaySound (TERM_BEEPSND);
                SD_WaitSoundDone ();
                break;

            //
            // HIDE CURSOR
            //
            case TP_CNVT_CODE('H','I'):
#if 0
                px = cur_x;
                py = cur_y;
                old_color = fontcolor;
                fontcolor = TERM_BCOLOR;
                VW_DrawString ("@");
                fontcolor = old_color;
#endif
                break;

            //
            // PAUSE
            //
            case TP_CNVT_CODE('P','A'):
                for (i = 0; i < 30; i++)
                {
                    VW_WaitVBL (1);
                    CycleColors ();
                }
                break;

            //
            // MORE
            //
            case TP_CNVT_CODE('M','O'):
                if (pi->print_delay)
                    TP_SlowPrint (TP_MORE_TEXT,pi->print_delay);
                else
                    TP_Print (TP_MORE_TEXT,false);

                LastScan = 0;    // TODO: IN_ClearKeysDown?

                do
                {
                    IN_ReadControl (&ci);
                } while (!ci.button0 && !ci.button1 && !ci.button2 && !ci.button3 && ci.dir == dir_None && !LastScan);

                cur_x = xl;
                VW_Bar (cur_x,cur_y,xh - xl + 1 + (TP_MARGIN * 2),font->height + is_shadowed,bgcolor);

                if (pi->flags & TPF_SHOW_CURSOR)
                    TP_JumpCursor ();

                if (LastScan == sc_Escape)
                    flags &= ~fl_presenting;

                TPscan = LastScan;
                break;

            //
            // DISPLAY STRING
            //
            case TP_CNVT_CODE('D','S'):
                disp_str_num = TP_VALUE(first_ch,2);

                if (disp_str_num >= PI_MAX_NUM_DISP_STRS)
                    Quit ("String number exceeds max array size!");

                old_first_ch = first_ch + 2;

                first_ch = piStringTable[disp_str_num];

                if (first_ch)
                {
                    while ((flags & fl_presenting) && *first_ch)
                    {
                        if (*first_ch == TP_CONTROL_CHAR)
                            TP_HandleCodes ();
                        else
                            TP_WrapText ();
                    }
                }

                first_ch = old_first_ch;
                break;

            //
            // PLAY MUSIC
            //
            case TP_CNVT_CODE('P','M'):
                temp = TP_VALUE(first_ch,2);

                if (temp < LASTMUSIC && temp != music_num)
                {
                    music_num = temp;
                    TP_CacheIn (ct_music,music_num);
                }

                first_ch += 2;
                break;

            //
            // PLAY SOUND
            //
            case TP_CNVT_CODE('P','S'):
                temp = TP_VALUE(first_ch,2);

                if (temp < LASTSOUND)
                {
                    TP_CacheIn (ct_scaled,0);
                    SD_PlaySound (temp);
                }

                first_ch += 2;
                break;

            //
            // END OF PAGE
            //
            case TP_CNVT_CODE('E','P'):
                VW_UpdateScreen (screen.buffer);

                if (screen.flags & SC_FADED)
                    VW_FadeIn ();

                if (anim_bgcolor != -1)
                {
                    oldbgcolor = bgcolor;
                    bgcolor = anim_bgcolor;
                }

                while (1)
                {
                    CycleColors ();
                    CalcTics ();

                    TP_AnimatePage (numanims);
                    VW_UpdateScreen (screen.buffer);
                    IN_ReadControl (&ci);

                    if (Keyboard[sc_PgUp])
                        ci.dir = dir_North;
                    else if (Keyboard[sc_PgDn])
                        ci.dir = dir_South;

                    if (pi->flags & TPF_CONTINUE && (ci.button0 || Keyboard[sc_Space] || Keyboard[sc_Enter]))
                    {
                        EscPressed = false;
                        flags &= ~fl_presenting;
                        break;
                    }

                    if (ci.button1 || Keyboard[sc_Escape])
                    {
                        EscPressed = true;
                        flags &= ~fl_presenting;
                        TPscan = sc_Escape;
                        break;
                    }
                    else
                    {
                        if ((ci.dir == dir_North || ci.dir == dir_West) && pi->pagenum)
                        {
                            if (flags & fl_upreleased)
                            {
                                pi->pagenum--;
                                flags &= ~fl_upreleased;
                                break;
                            }
                        }
                        else
                        {
                            flags |= fl_upreleased;

                            if ((ci.dir == dir_South || ci.dir == dir_East) && pi->pagenum < pi->numpages - 1)
                            {
                                if (flags & fl_dnreleased)
                                {
                                    pi->pagenum++;
                                    flags &= ~fl_dnreleased;
                                    break;
                                }
                            }
                            else
                                flags |= fl_dnreleased;
                        }
                    }
                }

                if (anim_bgcolor != -1)
                {
                    bgcolor = oldbgcolor;
                    anim_bgcolor = -1;
                }

                cur_x = xl;
                cur_y = yl;

                if (cur_y + font->height > yh)
                    cur_y = yh - font->height;

                first_ch = pi->script[pi->pagenum];

                numanims = 0;

                TP_PurgeAllGfx ();
                TP_CachePage (first_ch);

                if (*first_ch == TP_CONTROL_CHAR)
                {
                    TP_HandleCodes ();
                    flags &= ~fl_startofline;
                }

                VW_Bar (xl,yl,xh - xl + 1,yh - yl + 1,bgcolor);
                TP_PrintPageNumber ();
                //VW_Bar (xl - TP_MARGIN,yl - TP_MARGIN,xh - xl + 1 + (TP_MARGIN * 2),yh - yl + 1 + (TP_MARGIN * 2),bgcolor);
                break;

            //
            // EXIT PRESENTER
            //
            case TP_CNVT_CODE('X','X'):
                flags &= ~fl_presenting;
                VW_UpdateScreen (screen.buffer);
                break;
        }
    }

    if (first_ch[0] == TP_RETURN_CHAR && first_ch[1] == '\n' && (flags & fl_startofline))
        first_ch += 2;
}


/*
====================
=
= TP_PrintPageNumber
=
====================
*/

void TP_PrintPageNumber (void)
{
    char buffer[5];
    int  oldf,oldc;

    if (!(pi->flags & TPF_SHOW_PAGES))
        return;

    oldf = fontnumber;
    oldc = fontcolor;

    fontnumber = 2;
    fontcolor = 0x39;

    px = pagex[0];
    py = pagey[0];
    VW_Bar (px,py,12,7,0xe3);
    snprintf (buffer,sizeof(buffer),"%02d",pi->pagenum + 1);
    ShPrint (buffer,shcolor,false);

    px = pagex[1];

    if (px > -1)
    {
        py = pagey[1];

        snprintf (buffer,sizeof(buffer),"%02d",pi->numpages);
        ShPrint (buffer,shcolor,false);
        pagex[1] = -1;
    }

    fontnumber = oldf;
    fontcolor = oldc;
}


/*
====================
=
= TP_DrawShape
=
====================
*/

int TP_DrawShape (int x, int y, int shapenum, int shapetype)
{
    int  width;
    byte *addr;

    //
    // mask 'x coordinate' when displaying certain shapes
    //
    switch (shapetype)
    {
        case pis_pic:
            x = (x + 7) & 0xFFF8;
            break;
    }

    //
    // get width of shape (also draws a box/shadow if needed)
    //
    width = TP_BoxAroundShape(x,y,shapenum,shapetype);

    //
    // draw this shape
    //
    switch (shapetype)
    {
        case pis_scwall:
            TP_CacheIn (ct_scaled,0);
            addr = PM_GetPage(shapenum);
            vbuf = VW_LockSurface(screen.buffer);

            if (!vbuf)
                Quit ("Unable to lock surface: %s\n",SDL_GetError());

            postx = x;
            postsource = addr;

            for (x = 0; x < TEXTURESIZE; x++, postx++, postsource += TEXTURESIZE)
            {
                wallheight[postx] = 256;
                ScalePost ();
            }

            VW_UnlockSurface (screen.buffer);
            vbuf = NULL;
            break;

        case pis_scaled:
            TP_CacheIn (ct_scaled,0);

            if (flags & fl_clearscback)
                VW_Bar (x,y,64,64,bgcolor);

            MegaSimpleScaleShape (x + 32,y + 32,64,shapenum,NO_SHADING);
            break;

        case pis_pic:
            VW_DrawPic (x,y,shapenum);
            break;
    }

    //
    // advance current x position past shape and tell calling function where
    // (horizontally) this shape was drawn
    //
    cur_x += width;

    return x;
}


/*
====================
=
= TP_ResetAnims
=
====================
*/

void TP_ResetAnims (void)
{
    piAnimList[0].baseshape = -1;
}


/*
====================
=
= TP_AnimatePage
=
====================
*/

void TP_AnimatePage (int numanims)
{
    piAnimInfo  *anim;
    piShapeInfo *shape;

    anim = piAnimList;

    while (numanims--)
    {
        anim->delay += tics;

        if (anim->delay >= anim->maxdelay)
        {
            anim->delay = 0;
            anim->frame += anim->diradd;

            if (anim->frame == anim->maxframes || anim->frame < 0)
            {
                switch (anim->dirtype)
                {
                    case pid_cycle:
                        anim->frame = 0;
                        break;

                    case pid_rebound:
                        anim->diradd = -anim->diradd;
                        anim->frame += anim->diradd;
                        break;
                }
            }

            switch (anim->animtype)
            {
                case pia_shapetable:
                    shape = &piShapeTable[anim->baseshape + anim->frame];
                    TP_DrawShape (anim->x,anim->y,shape->shapenum,shape->shapetype);
                    break;

                case pia_grabscript:
                    shape = &piShapeTable[anim->baseshape];
                    TP_DrawShape (anim->x,anim->y,shape->shapenum + anim->frame,shape->shapetype);
                    break;
            }
        }

        anim++;
    }
}


/*
====================
=
= TP_BoxAroundShape
=
====================
*/

int TP_BoxAroundShape (int x1, int y1, int shapenum, int shapetype)
{
    int x2,y2;
    int width;

    switch (shapetype)
    {
        case pis_scwall:
        case pis_scaled:
            x2 = x1 + 63;
            y2 = y1 + 63;
            break;

        case pis_pic:
            width = pictable[shapenum - STARTPICS].width;
            x2 = x1 + width - 1;
            y2 = y1 + (pictable[shapenum - STARTPICS].height) - 1;
            break;
    }

    if (flags & fl_boxshape)
    {
        x1--;
        x2++;
        y1--;
        y2++;

        if (x1 >= 0 && y1 >= 0)
        {
            VW_Hlin (x1,x2,y1,ltcolor);
            VW_Hlin (x1,x2,y2,dkcolor);
            VW_Vlin (y1,y2,x1,ltcolor);
            VW_Vlin (y1,y2,x2,dkcolor);
        }
    }

    if (flags & fl_shadowpic)
    {
        x2++;
        y2++;

        if (x1 >= 0 && y1 >= 0)
        {
            VW_Hlin (x1 + 1,x2,y2,shcolor);
            VW_Vlin (y1 + 1,y2,x2,shcolor);
        }
    }

    return x2 - x1 + 1;
}


/*
====================
=
= TP_PurgeAllGfx
=
====================
*/

void TP_PurgeAllGfx (void)
{
    if (pi->flags & TPF_CACHE_NO_GFX)
        return;

    TP_ResetAnims ();
}


/*
====================
=
= TP_CachePage
=
====================
*/

void TP_CachePage (const char *script)
{
    int  code;
    bool end_of_page = false;
    int  numanims = 0;

    while (!end_of_page)
    {
        for (stemp = TP_LineCommented(script); stemp; stemp = TP_LineCommented(script))
            script += stemp;

        switch (*script++)
        {
            case TP_CONTROL_CHAR:
                code = ReadShort(script);

                script += 2;

                switch (code)
                {
                    case TP_CNVT_CODE('S','H'):
                        script += 3;
                        break;

                    case TP_CNVT_CODE('A','N'):
                        script += 2;

                        if (numanims++ == TP_MAX_ANIMS)
                            Quit ("Too many anims on one page!");

                    case TP_CNVT_CODE('X','X'):
                    case TP_CNVT_CODE('E','P'):
                        end_of_page = true;
                        break;
                }
                break;
        }
    }

    if (pi->flags & TPF_CACHE_NO_GFX)
        return;

    TP_CacheIn (ct_marks,0);
}


/*
====================
=
= TP_VALUE
=
====================
*/

unsigned TP_VALUE (const char *str, int num_nybbles)
{
    int      ch,nybble,shift;
    unsigned value = 0;

    for (nybble = 0; nybble < num_nybbles; nybble++)
    {
        shift = 4 * (num_nybbles - nybble - 1);

        ch = *str++;

        if (isxdigit(ch))
        {
            if (isalpha(ch))
                value |= (toupper(ch) - 'A' + 10) << shift;
            else
                value |= (ch - '0') << shift;
        }
    }

    return value;
}


/*
====================
=
= TP_JumpCursor
=
====================
*/

void TP_JumpCursor (void)
{
    int oldc = fontcolor;

    fontcolor = bgcolor;
    px = last_cur_x;
    py = last_cur_y;
    TP_Print ("@",true);

    px = cur_x = last_cur_x;
    py = cur_y = last_cur_y;
    fontcolor = oldc;
    TP_Print ("@",true);
}


/*
====================
=
= TP_Print
=
====================
*/

void TP_Print (const char *str, bool singlechar)
{
    char buf[2] = {0,0};

    LastScan = 0;    // TODO: IN_ClearKeysDown?

    last_cur_x = cur_x;
    last_cur_y = cur_y;

//
// the only test needed SHOULD be (flags & fl shadowtext),
// testing for '@' keeps the terminal's cursor from being
// shadowed - sorta' specific for AOG
//
// this should eventually be changed
//
    if ((flags & fl_shadowtext) && *str != '@')
    {
        if (fontcolor == bgcolor)
            ShPrint (str,bgcolor,singlechar);
        else
            ShPrint (str,shcolor,singlechar);
    }
    else if (singlechar)
    {
        buf[0] = *str;
        VW_DrawString (buf);
    }
    else
        VW_DrawString (str);

    cur_x = px;
    cur_y = py;

    if ((pi->flags & TPF_ABORTABLE) && LastScan)
        flags &= ~fl_presenting;
}


/*
====================
=
= TP_SlowPrint
=
====================
*/

bool TP_SlowPrint (const char *str, int delay)
{
    int     oldc;
    int     oldx,oldy;
    int32_t lasttimecount;
    bool    aborted = false;

    oldc = fontcolor;

    while (*str)
    {
        if (pi->flags & TPF_SHOW_CURSOR)
        {
            //
            // remove the cursor
            //
            fontcolor = bgcolor;
            px = oldx = last_cur_x;
            py = oldy = last_cur_y;
            TP_Print ("@",true);

            px = oldx;
            py = oldy;
            fontcolor = oldc;
        }

        //
        // if user aborted, print the whole string,
        // otherwise just print a character
        //
        if (aborted)
            TP_Print (str,false);
        else
            TP_Print (str++,true);

        //
        // print cursor
        //
        if (pi->flags & TPF_SHOW_CURSOR)
            TP_Print ("@",true);

        VW_UpdateScreen (screen.buffer);

        if (aborted)
            break;

        //
        // liven up the audio aspect!
        //
        if (pi->flags & TPF_TERM_SOUND)
        {
            if (*str != ' ')
                SD_PlaySound (TERM_TYPESND);
        }

        //
        // delay and check for abort if needed
        //
        if (!aborted)
        {
            LastScan = 0;    // TODO: IN_ClearKeysDown?
            lasttimecount = GetTimeCount();

            do
            {
                VW_WaitVBL (1);
                CycleColors ();

                if (pi->flags & TPF_ABORTABLE)
                {
                    if ((pi->flags & TPF_ABORTABLE) && LastScan)
                    {
                        aborted = true;
                        break;
                    }
                }

            } while (GetTimeCount() - lasttimecount < delay);
        }
    }

    if (aborted)
        flags &= ~fl_presenting;

    return aborted;
}


/*
====================
=
= TP_LoadScript
=
====================
*/

int32_t TP_LoadScript (const char *filename, PresenterInfo *pi, int idcache)
{
    int32_t size;
    char    *p;

    if (idcache)
    {
        pi->id_cache = idcache;
        pi->scriptstart = grsegs[idcache];

        p = strstr((char *)grsegs[idcache],int_xx);

        if (!p)
            Quit ("Can't find the %s doc terminator string!",int_xx);

        size = p - (char *)grsegs[idcache];
    }
    else
    {
        pi->id_cache = -1;

        size = IO_LoadFile(filename,&pi->scriptstart);

        if (!size)
            return 0;
    }

    pi->script[0] = pi->scriptstart;
    pi->script[0][size + 4] = 0;      // last byte is trashed!
    pi->flags |= TPF_CACHED_SCRIPT;

    TP_InitScript (pi);

    return size;
}


/*
====================
=
= TP_FreeScript
=
====================
*/

void TP_FreeScript (PresenterInfo *pi, int idcache)
{
    TP_PurgeAllGfx ();

    if (!idcache)
    {
        if (pi->flags & TPF_CACHED_SCRIPT)
            free (pi->scriptstart);
    }
}


/*
====================
=
= TP_InitScript
=
====================
*/

void TP_InitScript (PresenterInfo *pi)
{
    int  code;
    char *script;

    pi->numpages = 1;  // assume at least 1 page

    script = pi->script[0];

    while (*script)
    {
        for (stemp = TP_LineCommented(script); stemp; stemp = TP_LineCommented(script))
        {
            script += stemp;

            if (!*script)
            {
                pi->numpages--;
                return;
            }
        }

        switch (*script++)
        {
            case TP_CONTROL_CHAR:
                code = ReadShort(script);

                script += 2;

                if (code == TP_CNVT_CODE('E','P'))
                {
                    if (pi->numpages < TP_MAX_PAGES)
                        pi->script[pi->numpages++] = script;
                    else
                        Quit ("Too many pages in presenter!");
                }
                break;
#if 0
            case '\r':
                if (*script == '\n')
                {
                    *(script - 1) = TP_RETURN_CHAR;
                    *script++ = '*';   // this byte should always be skipped!
                }
                break;
#endif
        }
    }

    pi->numpages--;    // last page defined is not a real page
}


/*
====================
=
= TP_CacheIn
=
====================
*/

void TP_CacheIn (int type, int chunk)
{
    int first_ch_offset;
    int i,offset[TP_MAX_PAGES];

    first_ch_offset = first_ch - pi->script[0];

    switch (type)
    {
        case ct_scaled:
            if (flags & fl_pagemanager)
                break;

            flags |= fl_pagemanager;
            break;

        case ct_music:
        case ct_marks:
        case ct_chunk:
            if (flags & fl_pagemanager)
            {
                SD_StopDigitized ();
                flags &= ~fl_pagemanager;
            }

            switch (type)
            {
                case ct_chunk:
                case ct_marks:
                    break;

                case ct_music:
                    StartCPMusic (chunk);
                    break;
            }
            break;
    }

    font = (fontstruct *)grsegs[STARTFONT + fontnumber];

    //
    // re-assign script pointers if this is a cached script
    //
    if (pi->flags & TPF_CACHED_SCRIPT)
    {
        //
        // calculate offset of each page pointer
        //
        for (i = 1; i < pi->numpages; i++)
            offset[i] = pi->script[i] - pi->script[i - 1];

        //
        // re-assign all page pointers
        //
        if (pi->id_cache != -1)
            pi->scriptstart = grsegs[pi->id_cache];

        pi->script[0] = pi->scriptstart;

        for (i = 1; i < pi->numpages; i++)
            pi->script[i] = pi->script[i - 1] + offset[i];

        //
        // reset text pointer
        //
        first_ch = pi->script[0] + first_ch_offset;
    }
}


/*
====================
=
= TP_LineCommented
=
= TODO: this could use some safety checks
=
====================
*/

int TP_LineCommented (const char *string)
{
    const char *str = string;

    //
    // if a line starts with a semi-colon, the entire line is considered a
    // comment and is ignored
    //
    if (*string == ';' && *(string - 2) == TP_RETURN_CHAR)
    {
        while (*string != TP_RETURN_CHAR)
            string++;

        string += 2;
    }

    return (int)(string - str);
}
