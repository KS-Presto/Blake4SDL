// JM_TP.H

#ifndef _JM_TP_H_
#define _JM_TP_H_


#define PI_MAX_NUM_DISP_STRS    1   // num display str ptrs allocated for text presenter

#define TP_CASE_SENSITIVE           // ctrl codes are case sensitive

#define TP_640x200              0   // is presenter in 640 x 200 mode?

#define TP_RETURN_CHAR          '\r'
#define TP_CONTROL_CHAR         '^'

#define TP_CURSOR_SAVES         8   // MAX different points to save

#define TP_CNVT_CODE(c1,c2)     ((c1) | (c2 << 8))

#define TP_MAX_ANIMS            10
#define TP_MAX_PAGES            41

#define TP_MARGIN               1   // distance between xl/xh/yl/yh points and text

#define is_shadowed             ((flags & fl_shadowtext) == fl_shadowtext)

#define TP_INIT_DISPLAY_STR(num,str_ptr)   \
{ \
    if ((num) < PI_MAX_NUM_DISP_STRS)   \
        piStringTable[(num)] = (str_ptr);     \
    else                                   \
        Quit ("String number exceeds max array size"); \
}


//
// global static flags
//
#define fl_center       0x0001
#define fl_uncachefont  0x0002
#define fl_boxshape     0x0004
#define fl_shadowtext   0x0008
#define fl_presenting   0x0010
#define fl_startofline  0x0020
#define fl_upreleased   0x0040
#define fl_dnreleased   0x0080
#define fl_pagemanager  0x0100
#define fl_hidecursor   0x0200
#define fl_shadowpic    0x0400
#define fl_clearscback  0x0800


//
// PresenterInfo structure flags
//
#define TPF_CACHED_SCRIPT   0x0001
#define TPF_CACHE_NO_GFX    0x0002
#define TPF_CONTINUE        0x0004
#define TPF_USE_CURRENT     0x0008
#define TPF_SHOW_CURSOR     0x0010
#define TPF_SCROLL_REGION   0x0020
#define TPF_SHOW_PAGES      0x0040
#define TPF_TERM_SOUND      0x0080
#define TPF_ABORTABLE       0x0100


typedef struct
{
    uint16_t flags,gflags;
    char     *script[TP_MAX_PAGES];
    void     *scriptstart;
    int16_t  numpages,pagenum;
    int16_t  xl,yl,xh,yh;
    byte     fontnumber;
    byte     bgcolor,ltcolor,dkcolor,shcolor;
    int16_t  cur_x,cur_y;
    int8_t   print_delay;
    byte     highlight_color,fontcolor;
    int16_t  id_cache;
    char     *infoline;
} PresenterInfo;


enum tpCacheTypes
{
    ct_scaled,
    ct_chunk,
    ct_marks,
    ct_music,
};

enum pisTypes
{
    pis_pic,
    pis_sprite,
    pis_scaled,
    pis_scwall,
    pis_latchpic,
#if TP_640x200
    pis_pic2x,
    pis_sprite2x,
#endif
};


typedef struct
{
    int16_t shapenum;
    byte    shapetype;
} piShapeInfo;


enum piaTypes
{
    pia_grabscript,
    pia_shapetable,
};

enum pidTypes
{
    pid_cycle,
    pid_rebound,
};


typedef struct
{
    int16_t baseshape;
    int8_t  frame,maxframes;
    int16_t delay,maxdelay;
    byte    animtype;
    byte    dirtype;
    int16_t x,y;
    int8_t  diradd;
} piAnimInfo;


extern  piShapeInfo piShapeTable[];
extern  piAnimInfo  piAnimTable[];
extern  piAnimInfo  piAnimList[TP_MAX_ANIMS];
extern  char        *piStringTable[PI_MAX_NUM_DISP_STRS];  // TODO: const?
extern  byte        TPscan;


void        TP_Presenter (PresenterInfo *pi);
void        TP_WrapText (void);
void        TP_HandleCodes (void);
int         TP_DrawShape (int x, int y, int shapenum, int type);
unsigned    TP_VALUE (char *ptr, int num_nybbles);
int32_t     TP_LoadScript (char *filename, PresenterInfo *pi, int idcache);
void        TP_FreeScript (PresenterInfo *pi, int idcache);
void        TP_InitScript (PresenterInfo *pi);
void        TP_AnimatePage (int numanims);
int         TP_BoxAroundShape (int x1, int y1, int shapenum, int shapetype);
void        TP_JumpCursor (void);
void        TP_Print (char *str, bool singlechar);
bool        TP_SlowPrint (char *str, int delay);
void        TP_PurgeAllGfx (void);
void        TP_CachePage (char *script);
void        TP_CacheIn (int type, int chunk);
int         TP_LineCommented (char *s);
void        TP_PrintPageNumber (void);

#endif
