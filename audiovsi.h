/////////////////////////////////////////////////
//
// MUSE Header for .VSI
// Created Tue Aug 23 16:07:48 1994
//
/////////////////////////////////////////////////

//
// Sound names & indexes
//
enum soundnames
{
    HITWALLSND,
    TERM_TYPESND,
    GETPISTOLSND,
    LIQUIDDIESND,
    MOVEGUN2SND,
    MOVEGUN1SND,
    NOWAYSND,
    SCOUT_ALERTSND,
    GURNEYSND,
    PLAYERDEATHSND,
    CONCESSIONSSND,
    ATKIONCANNONSND,
    GETKEYSND,
    WARPOUTSND,
    WARPINSND,
    ROBOT_SERVOSND,
    INFORMANTDEATHSND,
    GOLDSTERNHALTSND,
    OPENDOORSND,
    CLOSEDOORSND,
    GETDETONATORSND,
    HALTSND,
    RENTDEATH2SND,
    ATKAUTOCHARGESND,
    ATKCHARGEDSND,
    RADAR_POWERUPSND,
    ATKBURSTRIFLESND,
    VITAL_GONESND,
    SHOOTDOORSND,
    RENTDEATH1SND,
    GETBURSTRIFLESND,
    GETAMMOSND,
    SHOOTSND,
    HEALTH1SND,
    HEALTH2SND,
    BONUS1SND,
    BONUS2SND,
    BONUS3SND,
    GETIONCANNONSND,
    ESCPRESSEDSND,
    ELECAPPEARSND,
    EXTRA_MANSND,
    ELEV_BUTTONSND,
    INTERROGATESND,
    BONUS5SND,
    BONUS4SND,
    PUSHWALLSND,
    TERM_BEEPSND,
    ROLL_SCORESND,
    TURRETSND,
    EXPLODE1SND,
    __UNUSED_51__SND,
    SWATDIESND,
    GGUARDHALTSND,
    EXPLODE2SND,
    BLUEBOYHALTSND,
    PROGUARDDEATHSND,
    DOGBOYHALTSND,
    ENGINE_THRUSTSND,
    SCANHALTSND,
    GETCANNONSND,
    LCANHALTSND,
    PROHALTSND,
    GGUARDDEATHSND,
    BLUEBOYDEATHSND,
    GOLDSTERNLAUGHSND,
    SCIENTISTHALTSND,
    SCIENTISTDEATHSND,
    DOGBOYDEATHSND,
    H_BEATSND,
    SWATHALTSND,
    SCANDEATHSND,
    LCANDEATHSND,
    INFORMDEATH2SND,
    INFORMDEATH3SND,
    GURNEYDEATHSND,
    PRODEATH2SND,
    __UNUSED_77__SND,
    SWATDEATH2SND,
    LCANBREAKSND,
    SCANBREAKSND,
    HTECHDOOROPENSND,
    HTECHDOORCLOSESND,
    ELECARCDAMAGESND,
    PODHATCHSND,
    ELECTSHOTSND,
    ELECDIESND,
    ATKGRENADESND,
    CLAWATTACKSND,
    PUNCHATTACKSND,
    SPITATTACKSND,
    PODDEATHSND,
    PODHALTSND,
    __UNUSED_93__SND,
    SCIDEATH2SND,
    SCIDEATH3SND,
    GOTTOKENSND,
    SWITCHSND,
    STATS1SND,
    STATS2SND,

    LASTSOUND
};

//
// Base offsets
//
#define STARTPCSOUNDS       0
#define STARTADLIBSOUNDS    LASTSOUND
#define STARTDIGISOUNDS     (LASTSOUND * 2)
#define STARTMUSIC          (LASTSOUND * 3)

//
// Music names & indexes
//
enum musicnames
{
    CATACOMB_MUS,
    STICKS_MUS,
    APOGFNFM_MUS,
    PLOT_MUS,
    CIRCLES_MUS,
    LASTLAFF_MUS,
    TOHELL_MUS,
    FORTRESS_MUS,
    GIVING_MUS,
    HARTBEAT_MUS,
    LURKING_MUS,
    MAJMIN_MUS,
    VACCINAP_MUS,
    DARKNESS_MUS,
    MONASTRY_MUS,
    TOMBP_MUS,
    TIME_MUS,
    MOURNING_MUS,
    SERPENT_MUS,
    HISCORE_MUS,
    THEME_MUS,

    LASTMUSIC
};

#define NUMSOUNDS       LASTSOUND
#define NUMSNDCHUNKS    (STARTMUSIC + LASTMUSIC)
#define NUMDIGISNDS     (STARTMUSIC - STARTDIGISOUNDS)

/////////////////////////////////////////////////
//
// Thanks for playing with MUSE!
//
/////////////////////////////////////////////////
