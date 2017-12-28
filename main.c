#include <stdio.h>
#include <time.h>
/*#include <malloc.h>*/
#include <stdlib.h>
#include <strings.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/Vendor.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <Xm/Xm.h>
#include <Xm/MainW.h>
#include <Xm/DrawingA.h>
#include <Xm/MessageB.h>
#include <Xm/ScrolledW.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/Text.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/CascadeB.h>
#include <Xm/Label.h>
#include <Xm/Scale.h>
#include <Xm/LabelG.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrollBar.h>



#ifndef TRUE
#define    TRUE    1
#define    FALSE   0
#endif

#ifndef DEBUG
#define DEBUG TRUE
#endif

#define OWFONT   "-Adobe-ITC Souvenir-Demi-R-Normal--14-*"
#define ABFONT   "-Adobe-New Century Schoolbook-Bold-R-Normal--*-140-*"
#define MONOFONT "-Adobe-Courier-Medium-R-Normal--*-120-*"


#define OWFONT_ALT   "-adobe-itc bookman-demi-r-normal--14-*"
#define ABFONT_ALT   "-adobe-new century schoolbook-bold-r-normal--*-140-*"
#define MONOFONT_ALT "-adobe-courier-medium-r-normal--*-120-*"

#define DEFAULT_FONT "9x15"

/*
 tetris pieces data structure
 
 0 T      |  *   | *   |     |  *  |
   | ***  | **   | *** | **  |
   |      | *    |  *  |  *  |
 
 1 L      |  *   | *** | **  |   * |
   |  *   | *    |  *  | *** |
   |  **  |      |  *  |     |
 
 2 p      |  *   | *   | **  |     |
   |  *   | ***  | *   | *** |
   | **   |      | *   |   * |
 
 3 s      |      |     |
   |  **  | *    |
   | **   | **   |
   |      |  *   |
 
 4 f      |      |     |
   | **   |  *   |
   |  **  | **   |
   |      | *    |
 
   |      |  *  |
 5 bar    | **** |  *  |
   |      |  *   |
   |      |  *   |
 
 6 box    |  **  |
   |  **  |
 */
#include "ticon.bm"
#include "ticonify.bm"
#include "tunit.bm"
#include "lunit.bm"
#include "punit.bm"
#include "sunit.bm"
#include "funit.bm"
#include "barunit.bm"
#include "boxunit.bm"
#include "fall.bm"
#include "filler.bm"
#include "gameover.bm"
#include "levelbtn.bm"
#include "help.bm"
#include "about.h"
#include "up.bm"
#include "here.bm"
#include "score.h"
#define T    0
#define L    1
#define P    2
#define S    3
#define F    4
#define BAR  5
#define BOX  6
#define MAXP 7
#define MHEIGHT  22
#define RECTSIZE 19
#define INITWIDTH  10
#define INITHEIGHT 20
#define INITLEVEL  0
#define INITPREP   0
#define MAXINTERVAL 500
#define WWIDTH   RECTSIZE*INITWIDTH
#define WHEIGHT  RECTSIZE*INITHEIGHT+MHEIGHT
#define MAXHEI 4
#define MAXWID 4
#define MAXHEI0 2
#define MAXWID0 4
#define MB1T   1
#define MB2T   2
#define MB3T   3
#define KB4T   4
#define KB5T   5
#define KB6T   6
#define KB0T   7
#define SCORE  0
#define NEXT   1
#define STAT   2
#define HELP   3
#define MAXWIN 4
#define SCOREUP 1<<0
#define LEVELUP 1<<1
#define LINEUP  1<<2
#define INCSCORE 4
#define REPEATSTART    250
#define REPEATINTERVAL 80
#define ONPIX  0
#define OFFPIX 1
#define NUMLBTN 10
#define LBTNCOL 5
#define LBTNROW 2
#define NUMHBTN 6
#define HBTNCOL 3
#define HBTNROW 2
#define BTNRECTSIZE 30
#define MAXPREPH 13
#define PMAP(x, y) pmap[pwidth*((y)+2)+(x)]
#define SCOREDISPLINE  5
#define SCORELINE  30
#define PTRWIDTH   20
#define RANKWIDTH  28
#define NAMEWIDTH  100
#define SCOREWIDTH 50
#define LEVELWIDTH 30
#define WEEKWIDTH  35
#define MONWIDTH   40
#define TIMEWIDTH  115
#define SCORERANK  PTRWIDTH
#define SCORENAME  SCORERANK + RANKWIDTH + 3
#define SCORESCORE SCORENAME + NAMEWIDTH + 3
#define SCORELEVEL SCORESCORE + SCOREWIDTH
#define SCOREWEEK  SCORELEVEL + LEVELWIDTH + 5
#define SCOREMONTH SCOREWEEK + WEEKWIDTH
#define SCORETIME  SCOREMONTH + MONWIDTH
#define SCOREEND   SCORETIME + TIMEWIDTH
#define RESIZENOTICE "Only 10x20 is official size\nYou will enter Training mode.\nAnd your score will not record."

static struct {
    Pixmap unipix;
    int    n;
} statdata[MAXP];

static struct {
    XImage  uniimage;
    char   *unitbits;
    int     pwidth[4];
    int     pheight[4];
    struct {
        signed char ppmap[MAXHEI][MAXWID];
        int         xdlt;
        int         ydlt;
    } map[4];
} pieces[MAXP] = {
    {
        /* XImage uniimage */
        {
            RECTSIZE, RECTSIZE, 0, XYBitmap, tunit_bits,
            LSBFirst, 8, LSBFirst, 8, 1, (RECTSIZE+7)/8,
        },
        tunit_bits,          /* char   *unitbits  */
        {3, 2, 3, 2},        /* int    pwidth[4]  */
        {2, 3, 2, 3},        /* int    pheight[4] */
        /* struct { ... } map[4] */
        {
            {
                /* char ppmap[MAXHEI][MAXWID] */
                {
                    {0, 1, 0, 0},
                    {1, 1, 1, 0},
                    {0, 0, 0, 0},
                    {0, 0, 0, 0},
                },
                0, 0,
            },
            {
                /* char ppmap[MAXHEI][MAXWID] */
                {
                    {0, 1, 0, 0},
                    {1, 1, 0, 0},
                    {0, 1, 0, 0},
                    {0, 0, 0, 0},
                },
                0, 1,
            },
            {
                /* char ppmap[MAXHEI][MAXWID] */
                {
                    {1, 1, 1, 0},
                    {0, 1, 0, 0},
                    {0, 0, 0, 0},
                    {0, 0, 0, 0},
                },
                1, -1,
            },
            {
                /* char ppmap[MAXHEI][MAXWID] */
                {
                    {1, 0, 0, 0},
                    {1, 1, 0, 0},
                    {1, 0, 0, 0},
                    {0, 0, 0, 0},
                },
                -1, 0,
            }
        }
    },
    {
        {RECTSIZE, RECTSIZE, 0, XYBitmap, lunit_bits,
            LSBFirst, 8, LSBFirst, 8, 1, (RECTSIZE+7)/8},
        lunit_bits,
        {3, 2, 3, 2},
        {2, 3, 2, 3},
        {
            {{
                {1, 1, 1, 0},
                {1, 0, 0, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
            }, 1, -1},
            {{
                {1, 0, 0, 0},
                {1, 0, 0, 0},
                {1, 1, 0, 0},
                {0, 0, 0, 0},
            }, -1, 0},
            {{
                {0, 0, 1, 0},
                {1, 1, 1, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
            }, 0, 0},
            {{
                {1, 1, 0, 0},
                {0, 1, 0, 0},
                {0, 1, 0, 0},
                {0, 0, 0, 0},
            }, 0, 1}
        }
    },
    {
        {RECTSIZE, RECTSIZE, 0, XYBitmap, punit_bits,
            LSBFirst, 8, LSBFirst, 8, 1, (RECTSIZE+7)/8},
        punit_bits,
        {3, 2, 3, 2},
        {2, 3, 2, 3},
        {
            {{
                {1, 0, 0, 0},
                {1, 1, 1, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
            }, 0, 0},
            {{
                {0, 1, 0, 0},
                {0, 1, 0, 0},
                {1, 1, 0, 0},
                {0, 0, 0, 0},
            }, 0, 1},
            {{
                {1, 1, 1, 0},
                {0, 0, 1, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
            }, 1, -1},
            {{
                {1, 1, 0, 0},
                {1, 0, 0, 0},
                {1, 0, 0, 0},
                {0, 0, 0, 0},
            }, -1, 0}
        }
    },
    {
        {RECTSIZE, RECTSIZE, 0, XYBitmap, sunit_bits,
            LSBFirst, 8, LSBFirst, 8, 1, (RECTSIZE+7)/8},
        sunit_bits,
        {3, 2, 3, 2},
        {2, 3, 2, 3},
        {
            {{
                {0, 1, 1, 0},
                {1, 1, 0, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
            }, 0, 0},
            {{
                {1, 0, 0, 0},
                {1, 1, 0, 0},
                {0, 1, 0, 0},
                {0, 0, 0, 0},
            }, 0, 0},
            {{
                {0, 1, 1, 0},
                {1, 1, 0, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
            }, 0, 0},
            {{
                {1, 0, 0, 0},
                {1, 1, 0, 0},
                {0, 1, 0, 0},
                {0, 0, 0, 0},
            }, 0, 0}
        }
    },
    {
        {RECTSIZE, RECTSIZE, 0, XYBitmap, funit_bits,
            LSBFirst, 8, LSBFirst, 8, 1, (RECTSIZE+7)/8},
        funit_bits,
        {3, 2, 3, 2},
        {2, 3, 2, 3},
        {
            {{
                {1, 1, 0, 0},
                {0, 1, 1, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
            }, 0, 0},
            {{
                {0, 1, 0, 0},
                {1, 1, 0, 0},
                {1, 0, 0, 0},
                {0, 0, 0, 0},
            }, 0, 0},
            {{
                {1, 1, 0, 0},
                {0, 1, 1, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
            }, 0, 0},
            {{
                {0, 1, 0, 0},
                {1, 1, 0, 0},
                {1, 0, 0, 0},
                {0, 0, 0, 0},
            }, 0, 0}
        }
    },
    {
        {RECTSIZE, RECTSIZE, 0, XYBitmap, barunit_bits,
            LSBFirst, 8, LSBFirst, 8, 1, (RECTSIZE+7)/8},
        barunit_bits,
        {4, 1, 4, 1},
        {1, 4, 1, 4},
        {
            {{
                {1, 1, 1, 1},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
            }, 1, -2},
            {{
                {1, 0, 0, 0},
                {1, 0, 0, 0},
                {1, 0, 0, 0},
                {1, 0, 0, 0},
            }, -1, 2},
            {{
                {1, 1, 1, 1},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
            }, 1, -2},
            {{
                {1, 0, 0, 0},
                {1, 0, 0, 0},
                {1, 0, 0, 0},
                {1, 0, 0, 0},
            }, -1, 2}
        }
    },
    {
        {RECTSIZE, RECTSIZE, 0, XYBitmap, boxunit_bits,
            LSBFirst, 8, LSBFirst, 8, 1, (RECTSIZE+7)/8},
        boxunit_bits,
        {2, 2, 2, 2},
        {2, 2, 2, 2},
        {
            {{
                {1, 1, 0, 0},
                {1, 1, 0, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
            }, 0, 0},
            {{
                {1, 1, 0, 0},
                {1, 1, 0, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
            }, 0, 0},
            {{
                {1, 1, 0, 0},
                {1, 1, 0, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
            }, 0, 0},
            {{
                {1, 1, 0, 0},
                {1, 1, 0, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
            }, 0, 0}
        }
    }
};


/*
 *  Global data
 */
long             interval, initinterval, setupinterval;
int              cpiece, npiece, crtopy, mapsize, nmember;
int              crx, cry, pwidth, pheight, crdir;
Dimension        width, height, tmpwidth, tmpheight;
signed char     *pmap, *savepmap;
int              cx[MAXP], cy[MAXP];
XtIntervalId     tid, reptid, ftid;


/*
 *  flag
 */
int    playing, over, wpause, sound, preinit, training;

/*
 *   Score
 */
int    sline, score, level, initlevel,
       preph, initprep, tmpinitlevel, tmpinitprep;

/*
 *  X/Motif
 */
XtAppContext     appContext;
Display         *dpy;
Window           work, next;
GC               gcc, rgcc, gcb, defwgc;
Pixel            defback;
Pixmap           copywork, filler, gameover, wipeffect, nextpix,
                 btntmp, levbtn[NUMLBTN][2], heibtn[NUMHBTN][2], helpix, up, here;
Pixmap           fall_pix;
XImage fallimage = {fall_width, fall_height, 0, XYBitmap, fall_bits,
                    LSBFirst, 8, LSBFirst, 8, 1, (fall_width+7)/8};

XImage btnimg    = {levelbtn_width, levelbtn_height, 0, XYBitmap, levelbtn_bits,
                    LSBFirst, 8, LSBFirst, 8, 1, (levelbtn_width+7)/8};
XImage fillim    = {RECTSIZE, RECTSIZE, 0, XYBitmap, filler_bits,
                    LSBFirst, 8, LSBFirst, 8, 1, (RECTSIZE+7)/8};
XImage goim      = {gameover_width, gameover_height, 0, XYBitmap, gameover_bits,
                    LSBFirst, 8, LSBFirst, 8, 1, (gameover_width+7)/8};
XImage helpimg   = {help_width, help_height, 0, XYBitmap, help_bits,
                    LSBFirst, 8, LSBFirst, 8, 1, (help_width+7)/8};
XImage upimg     = {up_width, up_height, 0, XYBitmap, up_bits,
                    LSBFirst, 8, LSBFirst, 8, 1, (up_width+7)/8},
       hereimg   = {here_width, here_height, 0, XYBitmap, here_bits,
                    LSBFirst, 8, LSBFirst, 8, 1, (here_width+7)/8};

/*  Widgets */
Widget    toplevel_widget, main_widget, menu_widget, work_adb, work_window,
          fall_button1, fall_button2,
          fpull_down, aboutbtn, moreabout, quitbtn, fentry, /* Pull down menu */
          startbtn, sizebtn, soundbtn, lsetupbtn,
          wpull_down, winbtn[MAXWIN], wentry,
          aboutd,     maboutd, mabouts, maboutb,            /* about dwtetris */
          sizepd, tlab, wscale, hscale, sokbtn, scanbtn, rsc,
                                                            /* size change dialog */
          scored, scl, scv, levl, levv, linl, linv,         /* Score display */
          nextd, nextlab, nextpwin,                         /* next */
          helpd,                                            /* help */
          statd, stlab, stpix[MAXP], stval[MAXP],           /* statistic */
          leveld, levell, levelm, level_btn[NUMLBTN],       /* Level setup */
          heightl, heightm, height_btn[NUMHBTN], lok, lcan,
          rankfail, rankd, rankl, rcomment,                 /* Ranking */
          ranksbox, rankdbox, herepic,
         *rrank, *rname, *rscore, *rlevel, *rtimew, *rtimem, *rtimeo, rdismbtn;

Cursor    fall_cursor,
          left_cursor, right_cursor,
          rotate_cursor, normal_cursor;

XKeyboardState   kbstate;
XFontStruct     *owindow_font, *about_font, *mono_font;
XmFontList       owfont, abfont, mnfont;


/*
 *   Event handlers
 */
static void Eventproc(Widget     w,
                      XtPointer  client_data,
                      XEvent    *event,
                      Boolean   *continue_to_dispatch_return);

static void Eventproc2(Widget     w,
                       XtPointer  client_data,
                       XEvent    *event,
                       Boolean   *continue_to_dispatch_return);

/*
 *   call backs
 */
static void quit_proc(void);

static void size_proc(void);

static void resize_proc(Widget               w,
                        int                  tag,
                        XmAnyCallbackStruct *reason);

static void sizechange_proc(Widget                 w,
                            int                    tag,
                            XmScaleCallbackStruct *scallback);

static void sizebtn_proc(Widget               w,
                         int                  tag,
                         XmAnyCallbackStruct *reason);

static void start_proc(
                       void);

static void expose_proc(Widget                       w,
                        caddr_t                      tag,
                        XmDrawingAreaCallbackStruct *exposevent);

static void exposenext_proc(Widget                       w,
                            caddr_t                      tag,
                            XmDrawingAreaCallbackStruct *exposevent);

static void fallbtn_proc(Widget               w,
                         int                  tag,
                         XmAnyCallbackStruct *reason);

static void soundsw_proc(
                         Widget                        w,
                         int                           tag,
                         XmToggleButtonCallbackStruct *reason);

static void window_proc(Widget                        w,
                        int                           tag,
                        XmToggleButtonCallbackStruct *reason);

static void level_proc(Widget             w,
                       int             tag,
                       XmToggleButtonCallbackStruct *reason);

static void height_proc(Widget                        w,
                        int                           tag,
                        XmToggleButtonCallbackStruct *reason);

static void levelbtn_proc(Widget               w,
                          int                  tag,
                          XmAnyCallbackStruct *reason);

static void levelsetup_proc(void);

static void about_proc(Widget               w,
                       int                  tag,
                       XmAnyCallbackStruct *reason);

static void scroll_proc(Widget                     w,
                        int                        tag,
                        XmScrollBarCallbackStruct *reason);

static void dispscore(struct score_rec     *allrec,
                      int                   cnt,
                      int                   rank,
                      char                  upped,
                      char                  first);

static void hidescore_proc(void);

static void hidemore_proc(void);

static void timer_proc(XtPointer     client_data,
                       XtIntervalId *id);

static void repeat_proc(XtPointer     client_data,
                        XtIntervalId *id);

static void wipe_proc(XtPointer     client_data,
                      XtIntervalId *id);

/*
 *   Functions
 */
static void init_tetris(void);

static void rotate(void);

static void left(void);

static void right(void);

static void fall(void);

static void expfall(void);

static void fixed(void);

static void gameover_proc(void);

static void gameover_pic(int ex,
                         int ey,
                         int ew,
                         int eh);

static void upscore(int mask);

static void scorefileupdate(void);

static void scorefail(void);

static char put_piece(int     x,
                      int     y,
                      int     p,
                      int     i);

static int    drop(void);

int scorefile(char   *name,
              int     score,
              int     level,
              void    (*dispproc)());


int main(int   argc,
         char *argv[])
{
    int     i, j, k, l, n, x, y;
    Arg     args[20];
    Widget  cpull_down, centry;
    static XtCallbackRec cb_expose[]     = {{expose_proc, NULL},     {NULL, NULL}},
                         cb_exposen[]    = {{exposenext_proc, NULL}, {NULL, NULL}},
                         cb_about[]      = {{about_proc, NULL},      {NULL, NULL}},
                         cb_quit[]       = {{quit_proc, NULL},       {NULL, NULL}},
                         cb_size[]       = {{size_proc, NULL},       {NULL, NULL}},
                         cb_fall[]       = {{fallbtn_proc, NULL},    {NULL, NULL}},
                         cb_sound[]      = {{soundsw_proc, NULL},    {NULL, NULL}},
                         cb_window[]     = {{window_proc, NULL},     {NULL, NULL}},
                         cb_start[]      = {{start_proc, NULL},      {NULL, NULL}},
                         cb_levelsetup[] = {{levelsetup_proc, NULL}, {NULL, NULL}},
                         cb_level[]      = {{level_proc, NULL},      {NULL, NULL}},
                         cb_height[]     = {{height_proc, NULL},     {NULL, NULL}},
                         cb_lsetupok[]   = {{levelbtn_proc, NULL},   {NULL, NULL}},
                         cb_scroll[]     = {{scroll_proc, NULL},     {NULL, NULL}},
                         cb_dismscore[]  = {{hidescore_proc, NULL}, {NULL, NULL}};
    
    Pixmap     ticon_pix, ticonify_pix;
    XGCValues  ogcv;
    
    /*
     *  Window Pulldown params
     */
    static char    *wbtnlab[MAXWIN] = {"Score", "Next", "Statistic", "Help"};
    static Boolean  wdef[MAXWIN]    = { TRUE, FALSE, FALSE, FALSE};
    static char    *wbtnacl[MAXWIN] = {"", "Ctrl/N", "Ctrl/T", ""};
    static char    *wbtnacc[MAXWIN] = {"", "Ctrl<KeyPress>n:", "Ctrl<KeyPress>t:", ""};
    
    toplevel_widget = XtAppInitialize(&appContext, "dxt",
                                      NULL, 0,
                                      &argc, argv, NULL,
                                      NULL, 0);
    
    dpy = XtDisplay(toplevel_widget);
    
#if DEBUG
    XSynchronize(dpy, TRUE);
#endif
    
    n = 0;
    ticon_pix = XCreateBitmapFromData(dpy,
                                      DefaultRootWindow(dpy),
                                      ticon_bits,
                                      ticon_width, ticon_height);
    ticonify_pix = XCreateBitmapFromData(dpy,
                                         DefaultRootWindow(dpy),
                                         ticonify_bits,
                                         ticonify_width, ticonify_height);
    XtSetArg(args[n], XtNiconPixmap,    ticon_pix);     n++;
    /*    XtSetArg(args[n], XtNiconifyPixmap, ticonify_pix);     n++;*/
    XtSetValues(toplevel_widget, args, n);
    
    /*
     *  Cursor
     */
    left_cursor   = XCreateFontCursor(dpy, XC_left_side);
    right_cursor  = XCreateFontCursor(dpy, XC_right_side);
    rotate_cursor = XCreateFontCursor(dpy, XC_exchange);
    normal_cursor = XCreateFontCursor(dpy, XC_question_arrow);
    fall_cursor   = XCreateFontCursor(dpy, XC_bottom_side);
    
    /*
     *  Main widget
     */
    n = 0;
    XtSetArg(args[n], XmNwidth,  0); n++;
    XtSetArg(args[n], XmNheight, 0); n++;
    main_widget = XmCreateMainWindow(toplevel_widget, "Tetris", args, n);
    
    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtManageChild(menu_widget = XmCreateMenuBar(main_widget,
                                                "Menu Bar", args, n));
    /*
     * File
     */
    n = 0;
    fpull_down = XmCreatePulldownMenu(menu_widget, "File", args, n);
    
    n = 0;
    cb_about[0].closure = (char *)0;
    XtSetArg(args[n], XmNactivateCallback, cb_about);       n++;
    XtSetArg(args[n], XmNlabelString,
             XmStringLtoRCreate("About..." , "ISO8859-1")); n++;
    XtManageChild(aboutbtn =
                  XmCreatePushButtonGadget(fpull_down, "about", args, n));
    
    n = 0;
    cb_about[0].closure = (char *)1;
    XtSetArg(args[n], XmNactivateCallback, cb_about);            n++;
    XtSetArg(args[n], XmNlabelString,
             XmStringLtoRCreate("More About..." , "ISO8859-1")); n++;
    XtManageChild(moreabout =
                  XmCreatePushButtonGadget(fpull_down, "Moreabout", args, n));
    
    n = 0;
    XtSetArg(args[n], XmNactivateCallback, cb_quit);       n++;
    XtSetArg(args[n], XmNlabelString,
             XmStringLtoRCreate("Quit" , "ISO8859-1"));    n++;
    XtSetArg(args[n], XmNacceleratorText,
             XmStringLtoRCreate("Ctrl/Q" , "ISO8859-1"));  n++;
    XtSetArg(args[n], XmNaccelerator, "Ctrl<KeyPress>q:"); n++;
    XtManageChild(quitbtn =
                  XmCreatePushButtonGadget(fpull_down, "Quit", args, n));
    
    n = 0;
    XtSetArg(args[n], XmNsubMenuId, fpull_down); n++;
    XtManageChild(fentry = XmCreateCascadeButton(menu_widget, "File",
                                                 args, n));
    
    /*
     * Control
     */
    n = 0;
    cpull_down = XmCreatePulldownMenu (menu_widget, "Control", args, n);
    
    n = 0;
    XtSetArg(args[n], XmNactivateCallback, cb_start);         n++;
    XtSetArg(args[n], XmNlabelString,
             XmStringLtoRCreate("Start Game" , "ISO8859-1")); n++;
    XtSetArg(args[n], XmNacceleratorText,
             XmStringLtoRCreate("Ctrl/S" , "ISO8859-1"));     n++;
    XtSetArg(args[n], XmNaccelerator, "Ctrl<KeyPress>s:");    n++;
    XtManageChild(startbtn = XmCreatePushButtonGadget(cpull_down,
                                                      "Start Game",
                                                      args, n));
    
    n = 0;
    XtSetArg(args[n], XmNvalueChangedCallback, cb_sound); n++;
    XtSetArg(args[n], XmNvisibleWhenOff,       FALSE);    n++;
    XtSetArg(args[n], XmNvalue,                TRUE);     n++;
    XtSetArg(args[n], XmNlabelString,
             XmStringLtoRCreate("Sound" , "ISO8859-1"));  n++;
    XtManageChild(soundbtn =
                  XmCreateToggleButtonGadget(cpull_down, "SoundSwitch",
                                             args, n));
    
    n = 0;
    XtSetArg(args[n], XmNactivateCallback, cb_levelsetup);     n++;
    XtSetArg(args[n], XmNlabelString,
             XmStringLtoRCreate("Level Setup" , "ISO8859-1")); n++;
    XtManageChild(lsetupbtn = XmCreatePushButtonGadget(cpull_down,
                                                       "LevelSetup",
                                                       args, n));
    
    n = 0;
    XtSetArg(args[n], XmNactivateCallback, cb_size);           n++;
    XtSetArg(args[n], XmNlabelString,
             XmStringLtoRCreate("Change Size" , "ISO8859-1")); n++;
    XtManageChild(sizebtn = XmCreatePushButtonGadget(cpull_down,
                                                     "Change size", args, n));
    
    n = 0;
    XtSetArg(args[n], XmNsubMenuId, cpull_down); n++;
    XtManageChild(centry = XmCreateCascadeButton (menu_widget, "Control",
                                                  args, n));
    
    /*
     * Windows
     */
    n = 0;
    wpull_down = XmCreatePulldownMenu(menu_widget, "Windows", args, n);
    
    for (i = 0; i < MAXWIN; i++) {
        n = 0;
        if (i == 0) {
            XtSetArg(args[n], XmNset, TRUE); n++;
        }
        cb_window[0].closure = (char *)i;
        XtSetArg(args[n], XmNvalueChangedCallback, cb_window);      n++;
        XtSetArg(args[n], XmNvisibleWhenOff,       FALSE);          n++;
        XtSetArg(args[n], XmNvalue,                wdef[i]);        n++;
        XtSetArg(args[n], XmNlabelString,
                 XmStringLtoRCreate(wbtnlab[i], "ISO8859-1"));      n++;
        if (strlen(wbtnacl[i]) > 0) {
            XtSetArg(args[n], XmNacceleratorText,
                     XmStringLtoRCreate(wbtnacl[i] , "ISO8859-1")); n++;
            XtSetArg(args[n], XmNaccelerator, wbtnacc[i]);          n++;
        }
        XtManageChild(winbtn[i] = XmCreateToggleButtonGadget(wpull_down,
                                                             wbtnlab[i],
                                                             args, n));
    }
    
    n = 0;
    XtSetArg(args[n], XmNsubMenuId, wpull_down); n++;
    XtManageChild(wentry = XmCreateCascadeButton (menu_widget, "Windows",
                                                  args, n));
    
    owindow_font = XLoadQueryFont(dpy, OWFONT);
    if (owindow_font == NULL) {
        owindow_font = XLoadQueryFont(dpy, OWFONT_ALT);
    }
    if (owindow_font == NULL) {
        owindow_font = XLoadQueryFont(dpy, DEFAULT_FONT);
    }
    if (owindow_font == NULL) {
        fprintf(stderr, "Cannot get 9x15 font???\n");
        exit(42);
    }
    owfont = XmFontListCreate(owindow_font, "ISO8859-1");
    
    /*
     *  Score
     */
    n = 0;
    XtSetArg(args[n], XmNx,                 30);        n++;
    XtSetArg(args[n], XmNy,                 50);        n++;
    XtSetArg(args[n], XmNunitType,          XmPIXELS);  n++;
    XtSetArg(args[n], XmNnoResize,          TRUE);      n++;
    XtSetArg(args[n], XmNallowOverlap,      FALSE);     n++;
    XtSetArg(args[n], XmNverticalSpacing,   10);        n++;
    XtSetArg(args[n], XmNhorizontalSpacing, 10);        n++;
    XtSetArg(args[n], XmNlabelFontList,     owfont);    n++;
    XtSetArg(args[n], XmNdialogTitle,
             XmStringLtoRCreate("Score", "ISO8859-1")); n++;
    scored = XmCreateFormDialog(toplevel_widget, "Score",
                                args, n);
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,     XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment,    XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNlabelString,
             XmStringLtoRCreate("Score:", "ISO8859-1")); n++;
    XtManageChild(scl=XmCreateLabelGadget(scored, "scl", args, n));
    
    n = 0;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,     scl);                      n++;
    XtSetArg(args[n], XmNbottomOffset,     0);                        n++;
    XtSetArg(args[n], XmNleftAttachment,   XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget,       scl);                      n++;
    XtSetArg(args[n], XmNleftOffset,       50);                       n++;
    XtSetArg(args[n], XmNrightOffset,      10);                       n++;
    XtSetArg(args[n], XmNlabelString,
             XmStringLtoRCreate("00000" , "ISO8859-1"));              n++;
    XtSetArg(args[n], XmNrecomputeSize,    FALSE);                    n++;
    XtManageChild(scv=XmCreateLabelGadget(scored, "scv", args,        n));
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,    XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget,        scl);             n++;
    XtSetArg(args[n], XmNleftAttachment,   XmATTACH_FORM);   n++;
    XtSetArg(args[n], XmNlabelString,
             XmStringLtoRCreate("Level:" , "ISO8859-1"));  n++;
    XtManageChild(levl=XmCreateLabelGadget(scored, "levl", args, n));
    
    n = 0;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,     levl);                     n++;
    XtSetArg(args[n], XmNbottomOffset,     0);                        n++;
    XtSetArg(args[n], XmNleftAttachment,   XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget,       levl);                     n++;
    XtSetArg(args[n], XmNleftOffset,       50);                       n++;
    XtSetArg(args[n], XmNrightOffset,      10);                       n++;
    XtSetArg(args[n], XmNlabelString,
             XmStringLtoRCreate("00000" , "ISO8859-1"));              n++;
    XtSetArg(args[n], XmNrecomputeSize,    FALSE);                    n++;
    XtManageChild(levv=XmCreateLabelGadget(scored, "levv", args, n));
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,    XmATTACH_WIDGET);          n++;
    XtSetArg(args[n], XmNtopWidget,        levl);                     n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);            n++;
    XtSetArg(args[n], XmNleftAttachment,   XmATTACH_FORM);            n++;
    XtSetArg(args[n], XmNlabelString,
             XmStringLtoRCreate("Lines:" , "ISO8859-1"));             n++;
    XtManageChild(linl=XmCreateLabelGadget(scored, "linl", args, n));
    
    n = 0;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,     linl);                     n++;
    XtSetArg(args[n], XmNbottomOffset,     0);                        n++;
    XtSetArg(args[n], XmNrightOffset,      10);                       n++;
    XtSetArg(args[n], XmNleftAttachment,   XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget,       linl);                     n++;
    XtSetArg(args[n], XmNleftOffset,       50);                       n++;
    XtSetArg(args[n], XmNlabelString,
             XmStringLtoRCreate("00000" , "ISO8859-1"));              n++;
    XtSetArg(args[n], XmNrecomputeSize,    FALSE);                    n++;
    XtManageChild(linv=XmCreateLabelGadget(scored, "linv", args, n));
    
    
    /*
     *  Level
     */
    n = 0;
    XtSetArg(args[n], XmNunitType,          XmPIXELS);                   n++;
    XtSetArg(args[n], XmNnoResize,          TRUE);                       n++;
    XtSetArg(args[n], XmNallowOverlap,      FALSE);                      n++;
    XtSetArg(args[n], XmNverticalSpacing,   10);                         n++;
    XtSetArg(args[n], XmNhorizontalSpacing, 10);                         n++;
    XtSetArg(args[n], XmNdefaultPosition,   TRUE);                       n++;
    XtSetArg(args[n], XmNdialogTitle,
             XmStringLtoRCreate("Level", "ISO8859-1"));                  n++;
    XtSetArg(args[n], XmNdialogStyle,       XmDIALOG_APPLICATION_MODAL); n++;
    XtSetArg(args[n], XmNlabelFontList,     owfont);                     n++;
    XtSetArg(args[n], XmNwidth,
             (BTNRECTSIZE + 4) * LBTNCOL + 6 + (LBTNCOL - 1) + 20);      n++;
    leveld = XmCreateFormDialog(main_widget, "Level", args, n);
    
    /*
     *  Level setup
     */
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,  XmATTACH_FORM);                n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);                n++;
    XtSetArg(args[n], XmNlabelString,
             XmStringLtoRCreate("Level" , "ISO8859-1"));                n++;
    XtManageChild(levell=XmCreateLabelGadget(leveld, "levellabel", args, n));
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,  XmATTACH_WIDGET);              n++;
    XtSetArg(args[n], XmNtopWidget,      levell);                       n++;
    XtSetArg(args[n], XmNtopOffset,      5);                            n++;
    XtSetArg(args[n], XmNwidth,
             (BTNRECTSIZE + 4) * LBTNCOL + 6 + (LBTNCOL - 1));          n++;
    XtSetArg(args[n], XmNheight,
             (BTNRECTSIZE + 4) * LBTNROW + 6 + (LBTNROW - 1));          n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);                n++;
    XtSetArg(args[n], XmNorientation,    XmHORIZONTAL);                 n++;
    XtSetArg(args[n], XmNnumColumns,     LBTNROW);                      n++;
    XtSetArg(args[n], XmNbackground,     BlackPixel(dpy, 0));           n++;
    levelm=XmCreateRadioBox(leveld, "levelmenu", args, n);
    
    btntmp = XCreatePixmap(dpy, DefaultRootWindow(dpy),
                           (unsigned int)(btnimg.width),
                           (unsigned int)(btnimg.height),
                           DefaultDepth(dpy, 0));
    XPutImage(dpy, btntmp, DefaultGC(dpy, 0),
              &btnimg, 0, 0, 0, 0, btnimg.width, btnimg.height);
    
    for (i = 0; i < LBTNROW; i++) {
        for (j = 0; j < LBTNCOL; j++) {
            levbtn[i * LBTNCOL + j][OFFPIX] =
            XCreatePixmap(dpy, DefaultRootWindow(dpy),
                          (unsigned int)BTNRECTSIZE,
                          (unsigned int)BTNRECTSIZE,
                          DefaultDepth(dpy, 0));
            XCopyArea(dpy, btntmp, levbtn[i * LBTNCOL + j][OFFPIX],
                      DefaultGC(dpy, 0), (i * LBTNCOL + j) * BTNRECTSIZE, 0,
                      BTNRECTSIZE, BTNRECTSIZE, 0, 0);
            levbtn[i * LBTNCOL + j][ONPIX] =
            XCreatePixmap(dpy, DefaultRootWindow(dpy),
                          (unsigned int)BTNRECTSIZE,
                          (unsigned int)BTNRECTSIZE,
                          DefaultDepth(dpy, 0));
            XCopyArea(dpy, btntmp, levbtn[i * LBTNCOL + j][ONPIX],
                      DefaultGC(dpy, 0), (i * LBTNCOL + j) * BTNRECTSIZE,
                      BTNRECTSIZE, BTNRECTSIZE, BTNRECTSIZE, 0, 0);
        }
    }
    
    for (i = 0; i < NUMLBTN; i++) {
        n = 0;
        XtSetArg(args[n], XmNlabelType,    XmPIXMAP);          n++;
        XtSetArg(args[n], XmNlabelPixmap,  levbtn[i][ONPIX]);  n++;
        XtSetArg(args[n], XmNselectPixmap, levbtn[i][OFFPIX]); n++;
        XtSetArg(args[n], XmNindicatorOn,  FALSE);             n++;
        XtSetArg(args[n], XmNspacing,      0);                 n++;
        XtSetArg(args[n], XmNmarginHeight, 0);                 n++;
        XtSetArg(args[n], XmNmarginWidth,  0);                 n++;
        XtSetArg(args[n], XmNmarginLeft,   0);                 n++;
        XtSetArg(args[n], XmNmarginRight,  0);                 n++;
        XtSetArg(args[n], XmNmarginBottom, 0);                 n++;
        XtSetArg(args[n], XmNmarginTop,    0);                 n++;
        cb_level[0].closure = (char *)i;
        XtSetArg(args[n], XmNvalueChangedCallback, cb_level);  n++;
        XtManageChild(level_btn[i] =
                      XmCreateToggleButton(levelm, "Lbtn", args, n));
    }
    
    /*
     *  Height setup
     */
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,  XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget,      levelm);          n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);   n++;
    XtSetArg(args[n], XmNlabelString,
             XmStringLtoRCreate("Height" , "ISO8859-1"));  n++;
    XtManageChild(heightl = XmCreateLabelGadget(leveld, "heightlabel",
                                                args, n));
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);      n++;
    XtSetArg(args[n], XmNtopWidget,     heightl);              n++;
    XtSetArg(args[n], XmNtopOffset,     5);                    n++;
    XtSetArg(args[n], XmNwidth,
             (BTNRECTSIZE + 4) * HBTNCOL + 6 + (HBTNCOL - 1)); n++;
    XtSetArg(args[n], XmNheight,
             (BTNRECTSIZE + 4) * HBTNROW + 6 + (HBTNROW - 1)); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNorientation,   XmHORIZONTAL);         n++;
    XtSetArg(args[n], XmNnumColumns,    HBTNROW);              n++;
    XtSetArg(args[n], XmNbackground,    BlackPixel(dpy, 0));   n++;
    heightm = XmCreateRadioBox(leveld, "heightmenu", args, n);
    
    for (i = 0; i < HBTNROW; i++) {
        for (j = 0; j < HBTNCOL; j++) {
            heibtn[i * HBTNCOL + j][OFFPIX] =
            XCreatePixmap(dpy, DefaultRootWindow(dpy),
                          (unsigned int)BTNRECTSIZE,
                          (unsigned int)BTNRECTSIZE, DefaultDepth(dpy, 0));
            XCopyArea(dpy, btntmp, heibtn[i * HBTNCOL + j][OFFPIX],
                      DefaultGC(dpy, 0), (i * HBTNCOL + j) * BTNRECTSIZE,
                      BTNRECTSIZE * 2, BTNRECTSIZE, BTNRECTSIZE, 0, 0);
            heibtn[i * HBTNCOL + j][ONPIX] =
            XCreatePixmap(dpy, DefaultRootWindow(dpy),
                          (unsigned int)BTNRECTSIZE,
                          (unsigned int)BTNRECTSIZE,
                          DefaultDepth(dpy, 0));
            XCopyArea(dpy, btntmp, heibtn[i * HBTNCOL + j][ONPIX],
                      DefaultGC(dpy, 0), (i * HBTNCOL + j) * BTNRECTSIZE,
                      BTNRECTSIZE * 3, BTNRECTSIZE, BTNRECTSIZE, 0, 0);
        }
    }
    
    XFreePixmap(dpy, btntmp);
    for(i = 0; i < NUMHBTN; i++) {
        n = 0;
        XtSetArg(args[n], XmNlabelType,    XmPIXMAP);          n++;
        XtSetArg(args[n], XmNlabelPixmap,  heibtn[i][ONPIX]);  n++;
        XtSetArg(args[n], XmNselectPixmap, heibtn[i][OFFPIX]); n++;
        XtSetArg(args[n], XmNindicatorOn,  FALSE);             n++;
        XtSetArg(args[n], XmNspacing,      0);                 n++;
        XtSetArg(args[n], XmNmarginHeight, 0);                 n++;
        XtSetArg(args[n], XmNmarginWidth,  0);                 n++;
        XtSetArg(args[n], XmNmarginLeft,   0);                 n++;
        XtSetArg(args[n], XmNmarginRight,  0);                 n++;
        XtSetArg(args[n], XmNmarginBottom, 0);                 n++;
        XtSetArg(args[n], XmNmarginTop,    0);                 n++;
        cb_height[0].closure = (char *)i;
        XtSetArg(args[n], XmNvalueChangedCallback, cb_height); n++;
        XtManageChild(height_btn[i] =
                      XmCreateToggleButton(heightm, "Hbtn", args, n));
    }
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,    XmATTACH_WIDGET);   n++;
    XtSetArg(args[n], XmNtopWidget,        heightm);           n++;
    XtSetArg(args[n], XmNleftAttachment,   XmATTACH_FORM);     n++;
    cb_lsetupok[0].closure = (char *)TRUE;
    XtSetArg(args[n], XmNactivateCallback, cb_lsetupok);       n++;
    XtSetArg(args[n], XmNlabelString,
             XmStringLtoRCreate("Start" , "ISO8859-1"));       n++;
    XtManageChild(lok = XmCreatePushButton(leveld, "Ok", args, n));
    
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,    XmATTACH_WIDGET);   n++;
    XtSetArg(args[n], XmNtopWidget,        heightm);           n++;
    XtSetArg(args[n], XmNleftAttachment,   XmATTACH_WIDGET);   n++;
    XtSetArg(args[n], XmNleftWidget,       lok);               n++;
    XtSetArg(args[n], XmNleftOffset,       5);                 n++;
    cb_lsetupok[0].closure = (char *)FALSE;
    XtSetArg(args[n], XmNactivateCallback, cb_lsetupok);       n++;
    XtSetArg(args[n], XmNlabelString,
             XmStringLtoRCreate("Cancel" , "ISO8859-1"));      n++;
    XtManageChild(lcan = XmCreatePushButton(leveld, "Cancel", args, n));
    
    XtManageChild(levelm);
    XtManageChild(heightm);
    XtSetArg(args[0], XmNdefaultButton, lok);
    XtSetValues(leveld, args, 1);
    
    /*
     *  Ranking
     */
    up = XCreatePixmap(dpy, DefaultRootWindow(dpy),
                       (unsigned int)up_width,
                       (unsigned int)up_height,
                       DefaultDepth(dpy, 0));
    here = XCreatePixmap(dpy, DefaultRootWindow(dpy),
                         (unsigned int)here_width,
                         (unsigned int)here_height,
                         DefaultDepth(dpy, 0));
    
    n = 0;
    XtSetArg(args[n], XmNlabelFontList,     owfont);                     n++;
    XtSetArg(args[n], XmNunitType,          XmPIXELS);                   n++;
    XtSetArg(args[n], XmNnoResize,          FALSE);                      n++;
    XtSetArg(args[n], XmNallowOverlap,      FALSE);                      n++;
    XtSetArg(args[n], XmNverticalSpacing,   10);                         n++;
    XtSetArg(args[n], XmNhorizontalSpacing, 10);                         n++;
    XtSetArg(args[n], XmNdialogTitle,
             XmStringLtoRCreate("Ranking", "ISO8859-1"));                n++;
    XtSetArg(args[n], XmNdialogStyle,       XmDIALOG_APPLICATION_MODAL); n++;
    XtSetArg(args[n], XmNdefaultPosition,   TRUE);                       n++;
    rankd = XmCreateFormDialog(toplevel_widget, "Ranking", args, n);
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,   XmATTACH_FORM);          n++;
    XtSetArg(args[n], XmNleftAttachment,  XmATTACH_FORM);          n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);          n++;
    XtSetArg(args[n], XmNlabelString,
             XmStringLtoRCreate("mtetris Ranking" , "ISO8859-1")); n++;
    XtManageChild(rankl = XmCreateLabelGadget(rankd, "rankl", args, n));
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,  XmATTACH_WIDGET);            n++;
    XtSetArg(args[n], XmNtopWidget,      rankl);                      n++;
    XtSetArg(args[n], XmNtopOffset,      5);                          n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);              n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);             n++;
    /*    XtSetArg(args[n], XmNwidth,          SCORETIME+30+17);         n++; */
    XtSetArg(args[n], XmNwidth,          SCORETIME+30+157);           n++;
    XtSetArg(args[n], XmNheight,         SCOREDISPLINE*SCORELINE+17); n++;
    XtSetArg(args[n], XmNscrollingPolicy, XmAUTOMATIC);               n++;
    ranksbox = XmCreateScrolledWindow(rankd, "ranksbox", args, n);
    
    n = 0;
    XtSetArg(args[n], XmNx,            0);              n++;
    XtSetArg(args[n], XmNy,            0);              n++;
    XtSetArg(args[n], XmNwidth,        SCOREEND * 1.5); n++;
    XtSetArg(args[n], XmNunitType,     XmPIXELS);       n++;
    XtSetArg(args[n], XmNnoResize,     FALSE);          n++;
    XtSetArg(args[n], XmNallowOverlap, FALSE);          n++;
    XtSetArg(args[n], XmNmarginHeight,   0);            n++;
    rankdbox = XmCreateBulletinBoard(ranksbox, "Rnakdbox", args, n);
    
    n = 0;
    XtSetArg(args[n], XmNx,           0);        n++;
    XtSetArg(args[n], XmNlabelType,   XmPIXMAP); n++;
    XtSetArg(args[n], XmNlabelPixmap, here);     n++;
    XtManageChild(herepic = XmCreateLabel(rankdbox, "RnakHere", args, n));
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,   XmATTACH_WIDGET);     n++;
    XtSetArg(args[n], XmNtopWidget,       ranksbox);            n++;
    XtSetArg(args[n], XmNleftAttachment,  XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNlabelString,
             XmStringLtoRCreate("Comment Here" , "ISO8859-1")); n++;
    XtManageChild(rcomment=XmCreateLabel(rankd, "rcomment", args, n));
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,     XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget,         rcomment);        n++;
    XtSetArg(args[n], XmNleftAttachment,    XmATTACH_FORM);   n++;
    XtSetArg(args[n], XmNlabelString,
             XmStringLtoRCreate("Dismiss" , "ISO8859-1"));    n++;
    XtSetArg(args[n], XmNborderWidth,      1); n++;
    XtSetArg(args[n], XmNactivateCallback, cb_dismscore);     n++;
    XtManageChild(rdismbtn = XmCreatePushButton(rankd, "fall", args, n));
    
    XtSetArg(args[0], XmNdefaultButton, rdismbtn);
    XtSetValues(rankd, args, 1);
    
    XtManageChild(rankdbox);
    XtManageChild(ranksbox);
    
    /*
     *  Optional windows
     *
     *  Next
     */
    nextpix = XCreatePixmap(dpy, DefaultRootWindow(dpy),
                            (unsigned int)(MAXWID0 * RECTSIZE),
                            (unsigned int)(MAXHEI0 * RECTSIZE),
                            DefaultDepth(dpy, 0));
    n = 0;
    XtSetArg(args[n], XmNx,                 30);                      n++;
    XtSetArg(args[n], XmNy,                 190);                     n++;
    XtSetArg(args[n], XmNwidth,             MAXWID0 * RECTSIZE + 20); n++;
    XtSetArg(args[n], XmNunitType,          XmPIXELS);                n++;
    XtSetArg(args[n], XmNnoResize,          TRUE);                    n++;
    XtSetArg(args[n], XmNallowOverlap,      FALSE);                   n++;
    XtSetArg(args[n], XmNverticalSpacing,   10);                      n++;
    XtSetArg(args[n], XmNhorizontalSpacing, 10);                      n++;
    XtSetArg(args[n], XmNlabelFontList,     owfont);                  n++;
    XtSetArg(args[n], XmNdialogTitle,
             XmStringLtoRCreate("Next", "ISO8859-1"));                n++;
    nextd = XmCreateFormDialog(toplevel_widget, "Next", args, n);
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,   XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment,  XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNlabelString,
             XmStringLtoRCreate("Next" , "ISO8859-1"));   n++;
    XtManageChild(nextlab=XmCreateLabelGadget(nextd, "nextlab", args, n));
    
    n = 0;
    XtSetArg(args[n], XmNwidth,            MAXWID0 * RECTSIZE); n++;
    XtSetArg(args[n], XmNheight,           MAXHEI0 * RECTSIZE); n++;
    XtSetArg(args[n], XmNborderWidth,      0);                  n++;
    XtSetArg(args[n], XmNtopAttachment,    XmATTACH_WIDGET);    n++;
    XtSetArg(args[n], XmNtopWidget,        nextlab);            n++;
    XtSetArg(args[n], XmNleftAttachment,   XmATTACH_FORM);      n++;
    XtSetArg(args[n], XmNrightAttachment,  XmATTACH_FORM);      n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);      n++;
    XtSetArg(args[n], XmNexposeCallback,   cb_exposen);         n++;
    XtManageChild(nextpwin = XmCreateDrawingArea(nextd, "nextw", args, n));
    
    /*
     *  Help
     */
    helpix = XCreatePixmap(dpy, DefaultRootWindow(dpy),
                           (unsigned int)help_width,
                           (unsigned int)help_height,
                           DefaultDepth(dpy, 0));
    XPutImage(dpy, helpix, DefaultGC(dpy, 0),
              &helpimg, 0, 0, 0, 0, help_width , help_height);
    
    n = 0;
    XtSetArg(args[n], XmNx,                170);         n++;
    XtSetArg(args[n], XmNy,                550);         n++;
    XtSetArg(args[n], XmNwidth,            help_width);  n++;
    XtSetArg(args[n], XmNheight,           help_height); n++;
    XtSetArg(args[n], XmNnoResize,         TRUE);        n++;
    XtSetArg(args[n], XmNbackgroundPixmap, helpix);      n++;
    XtSetArg(args[n], XmNdialogTitle,
             XmStringLtoRCreate("Help", "ISO8859-1"));   n++;
    helpd = XmCreateBulletinBoardDialog(toplevel_widget, "Help", args, n);
    
    /*
     * Statistic
     */
    n = 0;
    XtSetArg(args[n], XmNx,                 30);            n++;
    XtSetArg(args[n], XmNy,                 318);           n++;
    XtSetArg(args[n], XmNunitType,          XmPIXELS);      n++;
    XtSetArg(args[n], XmNnoResize,          TRUE);          n++;
    XtSetArg(args[n], XmNallowOverlap,      FALSE);         n++;
    XtSetArg(args[n], XmNverticalSpacing,   10);            n++;
    XtSetArg(args[n], XmNhorizontalSpacing, 10);            n++;
    XtSetArg(args[n], XmNlabelFontList,     owfont);        n++;
    XtSetArg(args[n], XmNdialogTitle,
             XmStringLtoRCreate("Statistic", "ISO8859-1")); n++;
    statd = XmCreateFormDialog(toplevel_widget, "Statistic",  args, n);
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,   XmATTACH_FORM);     n++;
    XtSetArg(args[n], XmNleftAttachment,  XmATTACH_FORM);    n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);    n++;
    XtSetArg(args[n], XmNlabelString,
             XmStringLtoRCreate("Statistic" , "ISO8859-1")); n++;
    XtManageChild(stlab=XmCreateLabelGadget(statd, "stlab", args, n));
    
    for (i = 0; i < MAXP; i++) {
        statdata[i].unipix = XCreatePixmap(dpy, DefaultRootWindow(dpy),
                                           (unsigned int)RECTSIZE,
                                           (unsigned int)RECTSIZE,
                                           DefaultDepth(dpy, 0));
        XPutImage(dpy, statdata[i].unipix, DefaultGC(dpy, 0),
                  &pieces[i].uniimage, 0, 0, 0, 0, RECTSIZE, RECTSIZE);
        
        n = 0;
        XtSetArg(args[n], XmNtopAttachment,  XmATTACH_WIDGET);          n++;
        XtSetArg(args[n], XmNtopWidget, (i == 0) ? stlab : stval[i-1]); n++;
        XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);            n++;
        XtSetArg(args[n], XmNleftOffset, RECTSIZE+20);                  n++;
        XtSetArg(args[n], XmNlabelString,
                 XmStringLtoRCreate("00000" , "ISO8859-1"));            n++;
        XtSetArg(args[n], XmNrecomputeSize,    FALSE);                  n++;
        XtManageChild(stval[i]=XmCreateLabelGadget(statd, "stv", args, n));
        
        n = 0;
        XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
        XtSetArg(args[n], XmNbottomWidget,     stval[i]);                 n++;
        XtSetArg(args[n], XmNbottomOffset,     0);                        n++;
        XtSetArg(args[n], XmNleftAttachment,   XmATTACH_FORM);            n++;
        XtSetArg(args[n], XmNlabelType,        XmPIXMAP);                 n++;
        XtSetArg(args[n], XmNlabelPixmap,      statdata[i].unipix);       n++;
        XtManageChild(stpix[i]=XmCreateLabel(statd, "stpix", args, n));
        
        if (i == MAXP - 1) {
            n = 0;
            XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
            XtSetValues(stval[i], args, n);
        }
    }
    
    /*
     *  Work attached dialog box
     */
    n = 0;
    XtSetArg(args[n], XmNunitType,     XmPIXELS);     n++;
    XtSetArg(args[n], XmNnoResize,     FALSE);        n++;
    XtSetArg(args[n], XmNallowOverlap, FALSE);        n++;
    XtSetArg(args[n], XmNresize,       XmRESIZE_ANY); n++;
    work_adb = XmCreateForm(main_widget, "WorkDB",  args, n);
    
    /*
     *   Work window
     */
    fall_pix = XCreatePixmap(dpy, DefaultRootWindow(dpy),
                             (unsigned int)(RECTSIZE*INITWIDTH),
                             (unsigned int)fall_height,
                             DefaultDepth(dpy, 0));
    
    for (i = 0; i < (RECTSIZE*INITWIDTH); i += RECTSIZE) {
        XPutImage(dpy, fall_pix, DefaultGC(dpy, 0), &fallimage,
                  0, 0, i, 0, RECTSIZE*INITWIDTH, fall_height);
    }
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,                XmATTACH_FORM);      n++;
    XtSetArg(args[n], XmNleftAttachment,               XmATTACH_FORM);      n++;
    XtSetArg(args[n], XmNheight,                       fall_height);        n++;
    XtSetArg(args[n], XmNwidth,                        RECTSIZE*INITWIDTH); n++;
    XtSetArg(args[n], XmNlabelType,                    XmPIXMAP);           n++;
    XtSetArg(args[n], XmNlabelPixmap,                  fall_pix);           n++;
    XtSetArg(args[n], XmNhighlightThickness,           0);                  n++;
    XtSetArg(args[n], XmNshadowThickness,              0);                  n++;
    XtSetArg(args[n], XmNdefaultButtonShadowThickness, 0);                  n++;
    XtSetArg(args[n], XmNshowAsDefault,                0);                  n++;
    XtSetArg(args[n], XmNborderWidth,                  0);                  n++;
    XtSetArg(args[n], XmNactivateCallback,             cb_fall);            n++;
    XtManageChild(fall_button1 = XmCreatePushButton(work_adb, "fall",
                                                    args, n));
    
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,  XmATTACH_WIDGET);     n++;
    XtSetArg(args[n], XmNtopWidget,      fall_button1);        n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);       n++;
    XtSetArg(args[n], XmNwidth,          RECTSIZE*INITWIDTH);  n++;
    XtSetArg(args[n], XmNheight,         RECTSIZE*INITHEIGHT); n++;
    XtSetArg(args[n], XmNexposeCallback, cb_expose);           n++;
    XtManageChild(work_window = XmCreateDrawingArea(work_adb, "Work",
                                                    args, n));
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,                XmATTACH_WIDGET);    n++;
    XtSetArg(args[n], XmNtopWidget,                    work_window);        n++;
    XtSetArg(args[n], XmNleftAttachment,               XmATTACH_FORM);      n++;
    XtSetArg(args[n], XmNheight,                       fall_height);        n++;
    XtSetArg(args[n], XmNwidth,                        RECTSIZE*INITWIDTH); n++;
    XtSetArg(args[n], XmNlabelType,                    XmPIXMAP);           n++;
    XtSetArg(args[n], XmNlabelPixmap,                  fall_pix);           n++;
    XtSetArg(args[n], XmNfillOnArm,                    TRUE);               n++;
    XtSetArg(args[n], XmNshowAsDefault,                0);                  n++;
    XtSetArg(args[n], XmNhighlightThickness,           0);                  n++;
    XtSetArg(args[n], XmNshadowThickness,              0);                  n++;
    XtSetArg(args[n], XmNhighlightThickness,           0);                  n++;
    XtSetArg(args[n], XmNdefaultButtonShadowThickness, 0);                  n++;
    XtSetArg(args[n], XmNshadowThickness,              0);                  n++;
    XtSetArg(args[n], XmNborderWidth,                  0);                  n++;
    XtSetArg(args[n], XmNactivateCallback,             cb_fall);            n++;
    XtManageChild(fall_button2 = XmCreatePushButton(work_adb, "fall",
                                                    args, n));
    
    XtManageChild(work_adb);
    
    n = 0;
    XtSetArg(args[n], XmNworkWindow, work_adb);    n++;
    XtSetArg(args[n], XmNmenuBar,    menu_widget); n++;
    XtSetValues(main_widget, args, n);
    
    XtManageChild(main_widget);
    XtMoveWidget(toplevel_widget, 170, 50);
    XtInstallAllAccelerators(main_widget, main_widget);
    XtRealizeWidget(toplevel_widget);
    
    n = 0;
    XtSetArg(args[n], XmNallowShellResize, FALSE); n++;
    XtSetValues(toplevel_widget, args, n);
    
    work = XtWindow(work_window);
    next = XtWindow(nextpwin);
    
    n = 0;
    XtSetArg(args[n], XmNbackground, &defback); n++;
    XtGetValues(work_window, args, n);
    
    XDefineCursor(dpy, XtWindow(fall_button1), fall_cursor);
    XDefineCursor(dpy, XtWindow(fall_button2), fall_cursor);
    
    /*
     *  Setup map
     */
    ogcv.background = WhitePixel(dpy, 0);
    ogcv.foreground = BlackPixel(dpy, 0);
    gcc = XCreateGC(dpy, work, GCForeground | GCBackground, &ogcv);
    ogcv.foreground = defback;
    ogcv.background = WhitePixel(dpy, 0);
    rgcc = XCreateGC(dpy, work, GCForeground | GCBackground, &ogcv);
    ogcv.foreground = BlackPixel(dpy, 0);
    ogcv.background = defback;
    defwgc = XCreateGC(dpy, work, GCForeground | GCBackground, &ogcv);
    ogcv.background = WhitePixel(dpy, 0);
    ogcv.foreground = BlackPixel(dpy, 0);
    ogcv.line_width = 1;
    gcb = XCreateGC(dpy, work,  GCForeground | GCBackground | GCLineWidth ,
                    &ogcv);
    copywork = XCreatePixmap(dpy, work,
                             (unsigned int)(MAXP*RECTSIZE),
                             (unsigned int)RECTSIZE,
                             DefaultDepth(dpy, 0));
    x = y = 0;
    
    for (i = 0; i < MAXP; i++) {
        XPutImage(dpy, copywork, gcc, &pieces[i].uniimage,
                  0, 0, (cx[i] = x), (cy[i] = y), RECTSIZE, RECTSIZE);
        x += RECTSIZE;
        for (j = 0; j <4; j++) {
            for (k = 0; k < MAXHEI; k++)
                for (l = 0; l < MAXWID; l++)
                    if (pieces[i].map[j].ppmap[k][l] != 0)
                        pieces[i].map[j].ppmap[k][l] = i+1;
        }
    }
    
    XPutImage(dpy, up, defwgc,
              &upimg, 0, 0, 0, 0, up_width, up_height);
    XPutImage(dpy, here, defwgc,
              &hereimg, 0, 0, 0, 0, here_width, here_height);
    gameover = XCreatePixmap(dpy, work,
                             (unsigned int)gameover_width,
                             (unsigned int)RECTSIZE,
                             DefaultDepth(dpy, 0));
    XPutImage(dpy, gameover, gcc, &goim,
              0, 0, 0, 0, gameover_width, RECTSIZE);
    XGetKeyboardControl(dpy, &kbstate);
    ftid = reptid = tid = 0;
    rankfail = aboutd = rsc = sizepd  = NULL;
    pwidth  = INITWIDTH;
    pheight = INITHEIGHT;
    width   = RECTSIZE * pwidth;
    height  = RECTSIZE * pheight;
    initinterval = MAXINTERVAL;
    initlevel = INITLEVEL;
    initprep  = INITPREP;
    preph     = 0;
    savepmap = pmap = NULL;
    sound = True;
    training = wpause = over = playing = False;
    mapsize = 0;
    wipeffect = filler = 0;
    cpiece = -2;
    preinit = True;
    
    n = 0;
    XtSetArg(args[n], XmNvalue, True); n++;
    XtSetValues(level_btn[initlevel], args, n);
    XtSetValues(height_btn[initprep], args, n);
    
    XtAppMainLoop(appContext);
    
    return (1);
}




static void init_tetris(void)
{
    time_t     tbuf;
    int        i, j, k, l, p;
    Arg        args[1];
    static int prep[NUMHBTN - 1] = {0, 4, 7, 10, 13};
    
    level   = initlevel;
    sline   = score = 0;
    width   = RECTSIZE * pwidth;
    height  = RECTSIZE * pheight;
    XClearWindow (dpy, work);
    XDrawRectangle (dpy, work, gcb, -1, -1, width+2, height+2);
    if (XtIsManaged(nextd)) {
        XFillRectangle (dpy, nextpix, rgcc, 0, 0, MAXWID0 * RECTSIZE,
                        MAXHEI0 * RECTSIZE);
        XClearWindow (dpy, next);
    }
    
    interval = setupinterval = initinterval - (initinterval / 12) * initlevel;
    cpiece = -2;
    time(&tbuf);
    srand((unsigned)tbuf);
    if (pmap == NULL) {
        if (wipeffect != 0) {
            XFreePixmap(dpy, wipeffect);
        }
        wipeffect = XCreatePixmap(dpy, work,
                                  (unsigned int)(pwidth * RECTSIZE),
                                  (unsigned int)(pheight * RECTSIZE),
                                  DefaultDepth(dpy, 0));
        if (filler != 0) {
            XFreePixmap(dpy, filler);
        }
        filler = XCreatePixmap(dpy, work,
                               (unsigned int)(pwidth * RECTSIZE),
                               (unsigned int)RECTSIZE,
                               DefaultDepth(dpy, 0));
        for (i = 0; i < pwidth; i ++) {
            XPutImage(dpy, filler, gcc, &fillim,
                      0, 0, i * RECTSIZE, 0, RECTSIZE, RECTSIZE);
        }
        pmap     = XtCalloc(pwidth, pheight+3);
        savepmap = XtCalloc(pwidth, MAXPREPH);
        mapsize = (pheight + 3) * pwidth;
    } else {
        memset((void *)pmap, 0, (size_t)mapsize);
    }
    
    for (i = 0; i < pwidth; i++) {
        PMAP(i, pheight) = MAXP;
    }
    
    switch (initprep) {
        case 0:
            preph = 0;
            break;
        case NUMHBTN - 1:
            memcpy((void *)(&PMAP(0, pheight - prep[preph])),
                   (void *)savepmap,
                   (size_t)(pwidth * prep[preph]));
            for (i = 0; i < prep[preph]; i++)
                for (j = 0; j < pwidth; j++)
                    if (PMAP(j, pheight - i - 1) != 0) {
                        p = PMAP(j, pheight - i - 1) - 1;
                        XCopyArea(dpy, copywork, work, gcc, cx[p], cy[p],
                                  RECTSIZE, RECTSIZE,
                                  j*RECTSIZE, (pheight - i - 1)*RECTSIZE);
                    }
            break;
        default:
            preph = initprep;
            for (i = 0; i < prep[preph]; i++)
                for (j = 0, k = rand() % pwidth; j < k; j++) {
                    while (PMAP((l = rand() % pwidth), pheight - i - 1) != 0) {
                    }
                    p = PMAP(l, pheight - i - 1) = (rand() % MAXP) + 1;
                    XCopyArea(dpy, copywork, work, gcc, cx[p - 1], cy[p - 1],
                              RECTSIZE, RECTSIZE,
                              l*RECTSIZE, (pheight - i - 1)*RECTSIZE);
                }
            memcpy((void *)savepmap,
                   (void *)(&PMAP(0, pheight - prep[preph])),
                   (size_t)(pwidth * prep[preph]));
            break;
    }
    
    crtopy = prep[preph];
    for (i = 0; i < MAXP; i++) {
        statdata[i].n = 0;
    }
    if (XtIsManaged(statd)) {
        for (i = 0; i < MAXP; i ++) {
            XtSetArg(args[0], XmNlabelString,
                     XmStringLtoRCreate("    0" , "ISO8859-1"));
            XtSetValues(stval[i], args, 1);
        }
    }
    
    XDefineCursor (dpy, work, normal_cursor);
    upscore(SCOREUP | LEVELUP | LINEUP);
    XtGrabKeyboard(work_window, False,
                   GrabModeAsync, GrabModeAsync, CurrentTime);
    XtAddEventHandler(work_adb,
                      EnterWindowMask | LeaveWindowMask,
                      False, Eventproc2, NULL);
    XtAddEventHandler(work_window,
                      KeyPressMask | KeyReleaseMask,
                      False, Eventproc, NULL);
    preinit = False;
}



static void timer_proc(XtPointer     client_data,
                       XtIntervalId *id)
{
    if (cpiece >= 0) {
        fall();
    } else {
        if (wpause || !(drop())) {
            tid = 0;
        }
    }
    tid = XtAppAddTimeOut(appContext, interval, timer_proc, 0);
}


static void quit_proc(void)
{
    exit(1);
}



static void start_proc(void)
{
    Arg     args[1];
    int     n;
    Boolean set;
    
    XtSetArg(args[0], XmNsensitive, FALSE);
    XtSetValues(startbtn, args, 1);
    XtSetValues(sizebtn, args, 1);
    XtSetValues(lsetupbtn, args, 1);
    /*
     n = 0;
     XtSetArg(args[n], XmNset, &set);    n++;
     XtGetValues(winbtn[0], args, n);
     
     if (set && !XtIsManaged(scored)) {
     XtManageChild(scored);
     } else if (!set && XtIsManaged(scored)) {
     XtUnmanageChild(scored);
     }
     */
    
    init_tetris();
    drop();
    playing = True;
    over    = False;
    if (tid == 0) {
        tid = XtAppAddTimeOut(appContext, interval, timer_proc, 0);
    }
    if (kbstate.global_auto_repeat) {
        XAutoRepeatOff(dpy);
    }
    XtAddEventHandler(work_window,
                      ButtonPressMask | ButtonReleaseMask,
                      False, Eventproc, NULL);
}



static int drop(void)
{
    int  i, j, savep;
    Arg  args[1];
    char val[6] = "000000";
    
    if (over) {
        return(False);
    }
    
    /*
     *   Select which piece
     */
    savep  = (cpiece < -1) ? -1 : npiece;
    cpiece = (cpiece < -1) ? rand() % MAXP : npiece;
    npiece = rand() % MAXP;
    
    if (XtIsManaged(nextd)) {
        if (savep != npiece) {
            XFillRectangle (dpy, nextpix, rgcc, 0, 0, MAXWID0 * RECTSIZE,
                            MAXHEI0 * RECTSIZE);
            for (i = 0; i < pieces[npiece].pheight[0]; i ++) {
                for (j = 0; j < pieces[npiece].pwidth[0]; j ++) {
                    if (pieces[npiece].map[0].ppmap[i][j] != 0) {
                        XCopyArea(dpy, copywork, nextpix, gcc,
                                  cx[npiece], cy[npiece], RECTSIZE, RECTSIZE,
                                  j * RECTSIZE, i * RECTSIZE);
                    }
                }
            }
            XClearWindow(dpy, next);
            XCopyArea(dpy, nextpix, next, gcc, 0, 0,
                      pieces[npiece].pwidth[0] * RECTSIZE,
                      pieces[npiece].pheight[0] * RECTSIZE,
                      (MAXWID0 - pieces[npiece].pwidth[0]) * RECTSIZE / 2,
                      (MAXHEI0 - pieces[npiece].pheight[0]) * RECTSIZE / 2);
        }
    }
    
    statdata[cpiece].n++;
    if (XtIsManaged(statd)) {
        sprintf (val, "%5d", statdata[cpiece].n);
        XtSetArg(args[0], XmNlabelString,
                 XmStringLtoRCreate(val , "ISO8859-1"));
        XtSetValues(stval[cpiece], args, 1);
    }
    
    /*
     *  Calc init position
     */
    crx = ((pwidth - pieces[cpiece].pwidth[0]) / 2) * RECTSIZE;
    cry = (1 - pieces[cpiece].pheight[0]) * RECTSIZE;
    crdir = 0;
    if (!put_piece(crx, cry, cpiece, crdir)) {
        gameover_proc();
        return(False);
    }
    return(True);
}


static char put_piece(int x,
                      int y,
                      int p,
                      int i)
{
    int pmx, pmy, k, l, xx, yy;
    
    /*
     *  Check room
     */
    for (pmy = y/RECTSIZE, k=0; pmy < 0; k++, pmy++) {
    }
    for (; k<pieces[p].pheight[i]; k++, pmy++) {
        for (pmx = x/RECTSIZE, l=0; l<pieces[p].pwidth[i]; l++, pmx++) {
            if (pieces[p].map[i].ppmap[k][l] != 0) {
                if (PMAP(pmx, pmy) > 0)
                    return False;
                PMAP(pmx, pmy) = -1;
            }
        }
    }
    
    /*
     *   Put block imamge
     */
    for (pmy = y/RECTSIZE, k=0, yy = y; pmy < 0; k++, pmy++, yy += RECTSIZE) {
    }
    for (; k<pieces[p].pheight[i]; k++, pmy++, yy += RECTSIZE) {
        for (pmx = x/RECTSIZE, l=0, xx = x; l<pieces[p].pwidth[i];
             l++, pmx++, xx += RECTSIZE) {
            if (pieces[p].map[i].ppmap[k][l] != 0) {
                XCopyArea(dpy, copywork, work, gcc, cx[p], cy[p],
                          RECTSIZE, RECTSIZE, xx, yy);
            }
        }
    }
    
    return True;
}




static void fall(void)
{
    static int pmx, pmy, k, l;
    static int x, y;
    
    if (cpiece < 0) {
        return;
    }
    
    /*
     *  Check bottom
     */
    pmy = cry/RECTSIZE;
    
    if (pmy + pieces[cpiece].pheight[crdir] >= pheight)    {
        fixed();
        cpiece = -1;
        return;
    }
    
    /*
     *  can fall ?
     */
    for (k = 0; k < pieces[cpiece].pheight[crdir]; k++, pmy++) {
        for (pmx = crx/RECTSIZE, l=0; l<pieces[cpiece].pwidth[crdir];
             l++, pmx++) {
            if (pieces[cpiece].map[crdir].ppmap[k][l] != 0) {
                if (PMAP(pmx, pmy+1) > 0) {
                    fixed();
                    cpiece = -1;
                    return;
                }
            }
        }
    }
    
    /*
     *  Clear block
     */
    for (k=0, y=cry; k<pieces[cpiece].pheight[crdir]; k++, y += RECTSIZE) {
        for (l=0, x=crx; l<pieces[cpiece].pwidth[crdir]; l++, x += RECTSIZE) {
            if (pieces[cpiece].map[crdir].ppmap[k][l] != 0) {
                if (k == 0 || pieces[cpiece].map[crdir].ppmap[k-1][l] == 0) {
                    XClearArea (dpy, work, x, y, RECTSIZE, RECTSIZE, False);
                }
            }
        }
    }
    
    /*
     *  Move next position
     */
    for (k = 0, y = cry + RECTSIZE; k < pieces[cpiece].pheight[crdir];
         k++, y += RECTSIZE) {
        for (l = 0, x = crx; l < pieces[cpiece].pwidth[crdir];
             l++, x += RECTSIZE) {
            if (pieces[cpiece].map[crdir].ppmap[k][l] != 0) {
                if (k == pieces[cpiece].pheight[crdir]-1
                    || pieces[cpiece].map[crdir].ppmap[k+1][l] == 0) {
                    XCopyArea(dpy, copywork, work, gcc,
                              cx[cpiece], cy[cpiece],
                              RECTSIZE, RECTSIZE, x, y);
                }
            }
        }
    }
    
    /*
     *  Update Map
     */
    for (pmy = cry/RECTSIZE, k = 0; k < pieces[cpiece].pheight[crdir];
         k++, pmy++) {
        for (pmx = crx/RECTSIZE, l=0; l<pieces[cpiece].pwidth[crdir];
             l++, pmx++) {
            if (pieces[cpiece].map[crdir].ppmap[k][l] != 0) {
                if (k == 0 || pieces[cpiece].map[crdir].ppmap[k-1][l] == 0)
                    PMAP(pmx, pmy) = 0;
                PMAP(pmx, pmy+1) = -1;
            }
        }
    }
    
    cry += RECTSIZE;
}




static void expfall(void)
{
    int pmx, pmy, k, l;
    int x, y, newy;
    
    if (cpiece < 0) {
        return;
    }
    
    /*
     *  Check bottom
     */
    pmy = cry/RECTSIZE;
    
    if (pmy + pieces[cpiece].pheight[crdir] >= pheight) {
        fixed();
        cpiece = -1;
        return;
    }
    
    /*
     *  can fall ?
     */
    for (k=0; k<pieces[cpiece].pheight[crdir]; k++, pmy++) {
        for (pmx = crx/RECTSIZE, l=0; l<pieces[cpiece].pwidth[crdir];
             l++, pmx++) {
            if (pieces[cpiece].map[crdir].ppmap[k][l] != 0) {
                if (PMAP(pmx, pmy+1) > 0) {
                    fixed();
                    cpiece = -1;
                    return;
                }
            }
        }
    }
    
    /*
     *  Find new location
     */
    newy = pheight - pieces[cpiece].pheight[crdir];
    for (pmx = crx/RECTSIZE, l=0; l<pieces[cpiece].pwidth[crdir];
         l++, pmx++) {
        for (pmy = cry/RECTSIZE + pieces[cpiece].pheight[crdir],
             k = pieces[cpiece].pheight[crdir] - 1; k >= 0 ; k--, pmy--) {
            if (pieces[cpiece].map[crdir].ppmap[k][l] != 0) {
                for (x = pmx, y = pmy + 1; y <= pheight; y++)
                    if (PMAP(x, y) > 0)    {
                        if (newy > y - k - 1)
                            newy = y - k - 1 ;
                        break;
                    }
                break;
            }
        }
    }
    
    /*
     *  Clear block
     */
    for (pmy = cry/RECTSIZE, k=0, y=cry; k<pieces[cpiece].pheight[crdir];
         pmy++, k++, y += RECTSIZE) {
        for (pmx = crx/RECTSIZE, l=0, x=crx; l<pieces[cpiece].pwidth[crdir];
             pmx++, l++, x += RECTSIZE) {
            if (pieces[cpiece].map[crdir].ppmap[k][l] != 0)    {
                XClearArea (dpy, work, x, y, RECTSIZE, RECTSIZE, False);
                PMAP(pmx, pmy) = 0;
            }
        }
    }
    
    /*
     *  Move next position
     */
    for (pmy = newy, k=0, y=newy*RECTSIZE; k<pieces[cpiece].pheight[crdir];
         pmy++, k++, y += RECTSIZE) {
        for (pmx = crx/RECTSIZE, l=0, x=crx; l<pieces[cpiece].pwidth[crdir];
             pmx++, l++, x += RECTSIZE) {
            if (pieces[cpiece].map[crdir].ppmap[k][l] != 0) {
                XCopyArea(dpy, copywork, work, gcc,
                          cx[cpiece], cy[cpiece],
                          RECTSIZE, RECTSIZE, x, y);
                PMAP(pmx, pmy) = -1;
            }
        }
    }
    
    score += newy - cry / RECTSIZE;
    upscore(SCOREUP);
    cry = newy * RECTSIZE;
    fixed();
    cpiece = -1;
}



static void left(void)
{
    static int pmx, pmy, k, l;
    static int x, y;
    
    if (cpiece < 0) {
        return;
    }
    
    /*
     * Check edge
     */
    if ((crx/RECTSIZE) == 0) {
        return;
    }
    
    /*
     *  can move ?
     */
    for (pmy = cry/RECTSIZE, k=0; k<pieces[cpiece].pheight[crdir];
         k++, pmy++) {
        for (pmx = crx/RECTSIZE, l=0; l<pieces[cpiece].pwidth[crdir];
             l++, pmx++) {
            if (pieces[cpiece].map[crdir].ppmap[k][l] != 0) {
                if (PMAP(pmx-1, pmy) > 0) {
                    return;
                } else {
                    break;
                }
            }
        }
    }
    
    /*
     *  Clear block
     */
    for (k=0, y=cry; k<pieces[cpiece].pheight[crdir]; k++, y += RECTSIZE) {
        for (l=0, x=crx; l<pieces[cpiece].pwidth[crdir]; l++, x += RECTSIZE) {
            if (pieces[cpiece].map[crdir].ppmap[k][l] != 0) {
                if (l == pieces[cpiece].pwidth[crdir] ||
                    pieces[cpiece].map[crdir].ppmap[k][l+1] == 0) {
                    XClearArea (dpy, work, x, y, RECTSIZE, RECTSIZE, False);
                }
            }
        }
    }
    
    /*
     *  Move next position
     */
    for (k=0, y=cry; k<pieces[cpiece].pheight[crdir]; k++, y += RECTSIZE) {
        for (l=0, x=crx-RECTSIZE; l<pieces[cpiece].pwidth[crdir];
             l++, x += RECTSIZE) {
            if (pieces[cpiece].map[crdir].ppmap[k][l] != 0) {
                if (l == 0 || pieces[cpiece].map[crdir].ppmap[k][l-1] == 0) {
                    XCopyArea(dpy, copywork, work, gcc,
                              cx[cpiece], cy[cpiece],
                              RECTSIZE, RECTSIZE, x, y);
                }
            }
        }
    }
    
    /*
     *   Update Map
     */
    for (pmy = cry/RECTSIZE, k=0; k<pieces[cpiece].pheight[crdir];
         k++, pmy++) {
        for (pmx = crx/RECTSIZE, l=0; l<pieces[cpiece].pwidth[crdir];
             l++, pmx++) {
            if (pieces[cpiece].map[crdir].ppmap[k][l] != 0) {
                if (l == pieces[cpiece].pwidth[crdir] ||
                    pieces[cpiece].map[crdir].ppmap[k][l+1] == 0) {
                    PMAP(pmx, pmy) = 0;
                }
                PMAP(pmx-1, pmy) = -1;
            }
        }
    }
    
    crx -= RECTSIZE;
}


static void right(void)
{
    static int pmx, pmy, k, l;
    static int x, y;
    
    if (cpiece < 0) {
        return;
    }
    
    /*
     *  Check edge
     */
    if (crx / RECTSIZE + pieces[cpiece].pwidth[crdir] >= pwidth) {
        return;
    }
    
    /*
     *  can move ?
     */
    for (pmy = cry/RECTSIZE, k=0; k<pieces[cpiece].pheight[crdir];
         k++, pmy++) {
        for (pmx = crx/RECTSIZE + pieces[cpiece].pwidth[crdir] - 1 ,
             l = pieces[cpiece].pwidth[crdir] - 1; l>=0; l--, pmx--) {
            if (pieces[cpiece].map[crdir].ppmap[k][l] != 0) {
                if (PMAP(pmx+1, pmy) > 0) {
                    return;
                } else {
                    break;
                }
            }
        }
    }
    
    /*
     *  Clear block
     */
    for (k=0, y=cry; k<pieces[cpiece].pheight[crdir]; k++, y += RECTSIZE) {
        for (l=0, x=crx; l<pieces[cpiece].pwidth[crdir]; l++, x += RECTSIZE) {
            if (pieces[cpiece].map[crdir].ppmap[k][l] != 0) {
                if (l == 0 || pieces[cpiece].map[crdir].ppmap[k][l-1] == 0) {
                    XClearArea (dpy, work, x, y, RECTSIZE, RECTSIZE, False);
                }
            }
        }
    }
    
    /*
     *  Move next position
     */
    for (k=0, y=cry; k<pieces[cpiece].pheight[crdir];k++, y += RECTSIZE) {
        for (l=0, x=crx+RECTSIZE; l<pieces[cpiece].pwidth[crdir];
             l++, x += RECTSIZE) {
            if (pieces[cpiece].map[crdir].ppmap[k][l] != 0) {
                if (l == pieces[cpiece].pwidth[crdir] - 1 ||
                    pieces[cpiece].map[crdir].ppmap[k][l+1] == 0) {
                    XCopyArea(dpy, copywork, work, gcc,
                              cx[cpiece], cy[cpiece],
                              RECTSIZE, RECTSIZE, x, y);
                }
            }
        }
    }
    
    /*
     *  Update Map
     */
    for (pmy = cry/RECTSIZE, k=0 ; k<pieces[cpiece].pheight[crdir];
         k++, pmy++) {
        for (pmx = crx/RECTSIZE, l=0 ; l<pieces[cpiece].pwidth[crdir];
             l++, pmx++) {
            if (pieces[cpiece].map[crdir].ppmap[k][l] != 0) {
                if (l == 0 || pieces[cpiece].map[crdir].ppmap[k][l-1] == 0) {
                    PMAP(pmx, pmy) = 0;
                }
                PMAP(pmx+1, pmy) = -1;
            }
        }
    }
    
    crx += RECTSIZE;
}



static void rotate(void)
{
    int k, l, pmy, pmx, newdir = (crdir + 1 > 3) ? 0 : crdir + 1;
    int nk, nl, newx, newy, x, y;
    
    if (cpiece < 0) {
        return;
    }
    
    /*
     *  Check edge
     */
    pmx = crx / RECTSIZE + pieces[cpiece].map[crdir].xdlt;
    pmy = cry / RECTSIZE + pieces[cpiece].map[crdir].ydlt;
    if (pmx < 0 || pmx + pieces[cpiece].pwidth[newdir] > pwidth ||
        pmy + pieces[cpiece].pheight[newdir] > pheight) {
        return;
    }
    
    /*
     *  Can rotate ?
     */
    newx = crx + pieces[cpiece].map[crdir].xdlt * RECTSIZE;
    newy = cry + pieces[cpiece].map[crdir].ydlt * RECTSIZE;
    for (pmy = newy/RECTSIZE, k=0; pmy < 0; k++, pmy++)    {
    }
    for (; k<pieces[cpiece].pheight[newdir]; k++, pmy++) {
        for (pmx = newx/RECTSIZE, l=0; l<pieces[cpiece].pwidth[newdir];
             l++, pmx++) {
            if (pieces[cpiece].map[newdir].ppmap[k][l] != 0) {
                if (PMAP(pmx, pmy) > 0) {
                    return;
                }
            }
        }
    }
    
    /*
     *  Clear block
     */
    for (k=0, nk = - pieces[cpiece].map[crdir].ydlt,
         y = cry; k<pieces[cpiece].pheight[crdir];
         nk++, k++, y += RECTSIZE) {
        for (l=0, nl = - pieces[cpiece].map[crdir].xdlt,
             x = crx; l<pieces[cpiece].pwidth[crdir];
             nl++, l++, x += RECTSIZE) {
            if (nk < 0 || nk >= pieces[cpiece].pheight[newdir] ||
                nl < 0 || nl >= pieces[cpiece].pwidth[newdir]) {
                if (pieces[cpiece].map[crdir].ppmap[k][l] != 0) {
                    XClearArea (dpy, work, x, y, RECTSIZE, RECTSIZE, False);
                }
            } else {
                if (pieces[cpiece].map[crdir].ppmap[k][l] != 0 &&
                    pieces[cpiece].map[newdir].ppmap[nk][nl] == 0) {
                    XClearArea (dpy, work, x, y, RECTSIZE, RECTSIZE, False);
                }
            }
        }
    }
    
    /*
     *  Move next position
     */
    for (k = pieces[cpiece].map[crdir].ydlt, nk = 0,
         y=newy; nk<pieces[cpiece].pheight[newdir];
         nk++, k++, y += RECTSIZE) {
        for (nl=0, l = pieces[cpiece].map[crdir].xdlt,
             x=newx; nl<pieces[cpiece].pwidth[newdir];
             nl++, l++, x += RECTSIZE) {
            if (k < 0 || k >= pieces[cpiece].pheight[crdir] ||
                l < 0 || l >= pieces[cpiece].pwidth[crdir]) {
                if (pieces[cpiece].map[newdir].ppmap[nk][nl] != 0) {
                    XCopyArea(dpy, copywork, work, gcc,
                              cx[cpiece], cy[cpiece],
                              RECTSIZE, RECTSIZE, x, y);
                }
            } else {
                if (pieces[cpiece].map[crdir].ppmap[k][l] == 0 &&
                    pieces[cpiece].map[newdir].ppmap[nk][nl] != 0) {
                    XCopyArea(dpy, copywork, work, gcc,
                              cx[cpiece], cy[cpiece],
                              RECTSIZE, RECTSIZE, x, y);
                }
            }
        }
    }
    
    /*
     *  Update Map
     */
    for (pmy = cry/RECTSIZE, k=0 ; k<pieces[cpiece].pheight[crdir];
         k++, pmy++) {
        for (pmx = crx/RECTSIZE, l=0 ; l<pieces[cpiece].pwidth[crdir];
             l++, pmx++) {
            if (pieces[cpiece].map[crdir].ppmap[k][l] != 0) {
                PMAP(pmx, pmy) = 0;
            }
        }
    }
    
    for (pmy = newy/RECTSIZE, k=0 ; k<pieces[cpiece].pheight[newdir];
         k++, pmy++) {
        for (pmx = newx/RECTSIZE, l=0 ; l<pieces[cpiece].pwidth[newdir];
             l++, pmx++) {
            if (pieces[cpiece].map[newdir].ppmap[k][l] != 0) {
                PMAP(pmx, pmy) = -1;
            }
        }
    }
    
    crx = newx;
    cry = newy;
    crdir = newdir;
}



static void expose_proc(Widget                       w,
                        caddr_t                      tag,
                        XmDrawingAreaCallbackStruct *exposevent)
{
    static int firstTime = True;
    int        i, j, p, ex, ey, ew, eh;
    Arg        args[10];
    int        n;
    
    if (((XExposeEvent *)(exposevent->event))->count > 0) {
        return;
    }
    
    {
        Dimension width, height, height2;
        Position  y;
        
        n = 0;
        XtSetArg(args[n], XmNy, &height2); n++;
        XtGetValues(work_adb, args, n);
        
        n = 0;
        XtSetArg(args[n], XmNy, &y);           n++;
        XtSetArg(args[n], XmNheight, &height); n++;
        XtGetValues(fall_button2, args, n);
        
        n = 0;
        XtSetArg(args[n], XmNwidth, &width); n++;
        XtGetValues(toplevel_widget, args, n);
        
        n = 0;
        XtSetArg(args[n], XmNheight,    y + height + height2); n++;
        XtSetArg(args[n], XmNminHeight, y + height + height2); n++;
        XtSetArg(args[n], XmNmaxHeight, y + height + height2); n++;
        XtSetValues(toplevel_widget, args, n);
        
        
        width = pwidth * RECTSIZE;
        n = 0;
        XtSetArg(args[n], XmNwidth, width);    n++;
        XtSetArg(args[n], XmNminWidth, width); n++;
        XtSetArg(args[n], XmNmaxWidth, width); n++;
        XtSetValues(toplevel_widget, args, n);
    }
    
    
    if (firstTime) {
        Boolean set;
        
        n = 0;
        XtSetArg(args[n], XmNset, &set); n++;
        XtGetValues(winbtn[0], args, n);
        
        if (set && !XtIsManaged(scored)) {
            XtManageChild(scored);
        } else if (!set && XtIsManaged(scored)) {
            XtUnmanageChild(scored);
        }
        
        levelsetup_proc();
        
        firstTime = False;
    }
    
    if (preinit) {
        return;
    }
    
    XDrawRectangle (dpy, work, gcb, -1, -1, width+2, height+2);
    
    if (((XExposeEvent *)(exposevent->event))->x > width) {
        return;
    }
    ex = ((XExposeEvent *)(exposevent->event))->x / RECTSIZE;
    if (((XExposeEvent *)(exposevent->event))->y > height) {
        return;
    }
    ey = ((XExposeEvent *)(exposevent->event))->y / RECTSIZE;
    ew = (((XExposeEvent *)(exposevent->event))->width +
          ((XExposeEvent *)(exposevent->event))->x >= width) ?
    pwidth - ((XExposeEvent *)(exposevent->event))->x / RECTSIZE :
    (((XExposeEvent *)(exposevent->event))->width + ((XExposeEvent *)(exposevent->event))->x) / RECTSIZE -
    ((XExposeEvent *)(exposevent->event))->x / RECTSIZE + 1;
    eh = (((XExposeEvent *)(exposevent->event))->height + ((XExposeEvent *)(exposevent->event))->y >= height) ?
    pheight - ((XExposeEvent *)(exposevent->event))->y / RECTSIZE :
    (((XExposeEvent *)(exposevent->event))->height + ((XExposeEvent *)(exposevent->event))->y) / RECTSIZE -
    ((XExposeEvent *)(exposevent->event))->y / RECTSIZE + 1;
    
    if (over){
        /*
         gameover_pic(ex, ey, ew, eh);
         */
        gameover_pic(0, 0, (int)width, (int)height);
    } else {
        if (ey + eh > crtopy) {
            for (i = ey; i < ey + eh ; i++) {
                for (j = ex; j < ex + ew; j++) {
                    if (PMAP(j, i) != 0) {
                        p = (PMAP(j, i) > 0) ? PMAP(j, i) -1 : cpiece;
                        XCopyArea(dpy, copywork, work, gcc, cx[p], cy[p],
                                  RECTSIZE, RECTSIZE, j*RECTSIZE, i*RECTSIZE);
                    }
                }
            }
        }
        if (cpiece >= 0) {
            for (i = 0; i < pieces[cpiece].pheight[crdir]; i++) {
                for (j = 0; j < pieces[cpiece].pwidth[crdir]; j++) {
                    if (pieces[cpiece].map[crdir].ppmap[i][j] != 0) {
                        XCopyArea(dpy, copywork, work, gcc,
                                  cx[cpiece], cy[cpiece],
                                  RECTSIZE, RECTSIZE,
                                  j*RECTSIZE + crx, i*RECTSIZE + cry);
                    }
                }
            }
        }
    }
}


static void exposenext_proc(Widget                       w,
                            caddr_t                      tag,
                            XmDrawingAreaCallbackStruct *exposevent)
{
    if (playing && npiece >= 0)    {
        XClearWindow (dpy, next);
        XCopyArea(dpy, nextpix, next, gcc, 0, 0,
                  pieces[npiece].pwidth[0] * RECTSIZE,
                  pieces[npiece].pheight[0] * RECTSIZE,
                  (MAXWID0 - pieces[npiece].pwidth[0]) * RECTSIZE / 2,
                  (MAXHEI0 - pieces[npiece].pheight[0]) * RECTSIZE / 2);
    } else {
        XClearWindow (dpy, next);
    }
}



static void fixed(void)
{
    int        k, l, pmx, pmy = cry/RECTSIZE;
    static int fy[MAXHEI];
    char       filled;
    
    if (cry < 0) {
        gameover_proc();
        return;
    }
    
    if (crtopy > pmy) {
        crtopy = pmy;
    }
    
    for (k = 0; k < pieces[cpiece].pheight[crdir]; k++, pmy++) {
        for (pmx = crx/RECTSIZE, l = 0; l < pieces[cpiece].pwidth[crdir];
             l++, pmx++) {
            if (pieces[cpiece].map[crdir].ppmap[k][l] != 0) {
                PMAP(pmx, pmy) = pieces[cpiece].map[crdir].ppmap[k][l];
            }
        }
    }
    
    score += (cry / height) * 20 + 4;
    upscore(SCOREUP);
    
    /*
     *  Check line
     */
    l = 0;
    for (pmy = cry/RECTSIZE, k = 0; k<pieces[cpiece].pheight[crdir];
         k++ , pmy++)    {
        filled = True;
        for (pmx = 0; pmx < pwidth; pmx++) {
            if (PMAP(pmx, pmy) == 0) {
                filled = False;
                break;
            }
        }
        if (filled) {
            PMAP(0, pmy) = -2;
            XCopyArea(dpy, filler, work, gcc, 0, 0,
                      width, RECTSIZE, 0, pmy * RECTSIZE);
            fy[l++] = pmy;
            if (sound) {
                XBell(dpy, 20);
            }
        }
    }
    if (l > 0) {
        if (tid != 0) {
            XtRemoveTimeOut(tid);
            tid = 0;
        }
        if (reptid != 0) {
            XtRemoveTimeOut(reptid);
            reptid = 0;
        }
        XtRemoveEventHandler(work_window,
                             ButtonPressMask | ButtonReleaseMask,
                             False, Eventproc, NULL);
        fy[l] = -1;
        ftid = XtAppAddTimeOut(appContext, 100, wipe_proc, fy);
        wpause = True;
    }
}


void wipe_proc(XtPointer     client_data,
               XtIntervalId *intervalid)
{
    Opaque fyp = (Opaque)client_data;

    int *fy = (int *) fyp;
    int i, j, k, p, ll, savetop;
    
    for (i = 0; ; i++) {
        if (fy[i] < 0) {
            break;
        }
    }
    
    i = ll = i - 1;
    
    /*
     *  Make current image on pixmap
     */
    XFillRectangle (dpy, wipeffect, rgcc, 0, 0, width, height);
    for (k = fy[i]-1; k >= crtopy; k--) {
        if (PMAP(0, k) == -2) {
            XCopyArea(dpy, filler, wipeffect, gcc, 0, 0,
                      width, RECTSIZE, 0, k * RECTSIZE);
        } else {
            for (j = 0; j < pwidth; j++) {
                if (PMAP(j, k) != 0) {
                    p = (PMAP(j, k) > 0) ? PMAP(j, k) -1 : cpiece;
                    XCopyArea(dpy, copywork, wipeffect, gcc, cx[p], cy[p],
                              RECTSIZE, RECTSIZE, j*RECTSIZE, k*RECTSIZE);
                }
            }
        }
    }
    
    /*
     *  Update map
     */
    memset((void *)(&PMAP(0, -1)), 0, (size_t)pwidth);
    k = 1;
    for (i = ll - 1, j = fy[ll]; j >= crtopy; j--) {
        while ((i >= 0) && (j - k == fy[i])) {
            k++;
            i--;
        }
        memcpy((void *)(&PMAP(0, j)), (void *)((j - k < -1) ? &PMAP(0, -1) :
                                               &PMAP(0, j-k)), (size_t)pwidth);
    }
    
    /*
     *  Visual effect
     */
    savetop = crtopy;
    p = 0;
#if DEGUB
#else
    XSynchronize(dpy, True);
#endif
    for (i = ll; i >= 0; i--) {
        for (j = 0; j < RECTSIZE; j++)
            XCopyArea(dpy, wipeffect, work, gcc, 0, savetop * RECTSIZE - 1,
                      width, (fy[i] + p - crtopy) * RECTSIZE + 1,
                      0, crtopy * RECTSIZE + j);
        crtopy ++; p ++;
    }
    
#if DEGUB
#else
    XSynchronize(dpy, False);
#endif
    
    sline += p;
    score += p * 10 + (p * p * 2);
    upscore(LINEUP | SCOREUP);
    if (tid == 0) {
        tid = XtAppAddTimeOut(appContext, interval, timer_proc, 0);
    }
    XtAddEventHandler(work_window,
                      ButtonPressMask | ButtonReleaseMask,
                      False, Eventproc, NULL);
    wpause = False;
}



/*
 *  Setup Procs
 *
 *  Level
 */
static void levelsetup_proc(void)
{
    tmpinitlevel = initlevel;
    tmpinitprep  = initprep;
    
    if (!XtIsManaged(leveld)) {
        XtManageChild(leveld);
    }
}


static void level_proc(Widget                        w,
                       int                           tag,
                       XmToggleButtonCallbackStruct *reason)
{
    if (reason->set) {
        tmpinitlevel = tag;
    }
}


static void height_proc(Widget                        w,
                        int                           tag,
                        XmToggleButtonCallbackStruct *reason)
{
    if (reason->set) {
        tmpinitprep = tag;
    }
}


static void levelbtn_proc(Widget               w,
                          int                  tag,
                          XmAnyCallbackStruct *reason)
{
    int     n;
    Arg     args[1];
    Boolean set;
    
    XtUnmanageChild(leveld);
    
    if (tag) {
        initlevel = tmpinitlevel;
        initprep  = tmpinitprep;
        
        XtSetArg(args[0], XmNsensitive, FALSE);
        XtSetValues(startbtn,  args, 1);
        XtSetValues(sizebtn,   args, 1);
        XtSetValues(lsetupbtn, args, 1);
        /*
         n = 0;
         XtSetArg(args[n], XmNset, &set);    n++;
         XtGetValues(winbtn[0], args, n);
         
         if (set && !XtIsManaged(scored)) {
         XtManageChild(scored);
         } else if (!set && XtIsManaged(scored)) {
         XtUnmanageChild(scored);
         }
         */
        init_tetris();
        drop();
        playing = True;
        over    = False;
        if (tid == 0) {
            tid = XtAppAddTimeOut(appContext, interval, timer_proc, 0);
        }
        if (kbstate.global_auto_repeat) {
            XAutoRepeatOff(dpy);
        }
        XtAddEventHandler(work_window,
                          ButtonPressMask | ButtonReleaseMask,
                          False, Eventproc, NULL);
    } else {
        init_tetris();
    }
}


/*
 *    Ranking
 */
static void scorefileupdate(void)
{
    /*      char username[L_cuserid];*/
    char username[256] = "Bob";
    
    if (!training) {
        /*    cuserid(username);*/
        if (!scorefile(username, score, level, dispscore)) {
            scorefail();
        }
    }
}


static void scroll_proc(Widget                     w,
                        int                        tag,
                        XmScrollBarCallbackStruct *reason)
{
    Arg args[1];
    
    switch (tag) {
        case 0:
            XtSetArg(args[0], XmNy, - reason->value * SCORELINE);
            XtSetValues(rankdbox, args, 1);
            break;
        case 1:
            XtSetArg(args[0], XmNx, - reason->value);
            XtSetValues(rankdbox, args, 1);
            break;
    }
}



static void dispscore(struct score_rec *allrec,
                      int               cnt,
                      int               rank,
                      char              upped,
                      char             first)
{
    int     n, i, newy, rankofs;
    Arg     args[10];
    char    cnvtmp[10], comment[100];
    struct {
        char timew[3];
        char wnull;
        char timem[3];
        char mnull;
        char timeo[16];
        char tnull[2];
    } timestrbuf;
    
    if (XtIsManaged(rankd)) {
        return;
    }
    
    rrank  = (Widget *)XtMalloc(sizeof(Widget) * cnt);
    rname  = (Widget *)XtMalloc(sizeof(Widget) * cnt);
    rscore = (Widget *)XtMalloc(sizeof(Widget) * cnt);
    rlevel = (Widget *)XtMalloc(sizeof(Widget) * cnt);
    rtimew = (Widget *)XtMalloc(sizeof(Widget) * cnt);
    rtimem = (Widget *)XtMalloc(sizeof(Widget) * cnt);
    rtimeo = (Widget *)XtMalloc(sizeof(Widget) * cnt);
    
    nmember = cnt;
    for (i = 0; i < cnt; i++) {
        sprintf(cnvtmp, "%d", i+1);
        
        n = 0;
        XtSetArg(args[n], XmNy,         i * SCORELINE);               n++;
        XtSetArg(args[n], XmNx,         SCORERANK);                   n++;
        XtSetArg(args[n], XmNalignment, XmALIGNMENT_END);             n++;
        XtSetArg(args[n], XmNwidth,     RANKWIDTH);                   n++;
        XtSetArg(args[n], XmNlabelString,
                 XmStringLtoRCreate(cnvtmp , "ISO8859-1"));           n++;
        XtManageChild(rrank[i]= XmCreateLabelGadget(rankdbox, "Rnak",
                                                    args, n));
        
        n = 0;
        XtSetArg(args[n], XmNy,          i * SCORELINE);              n++;
        XtSetArg(args[n], XmNx,          SCORENAME);                  n++;
        XtSetArg(args[n], XmNalignment,  XmALIGNMENT_BEGINNING);      n++;
        XtSetArg(args[n], XmNwidth,      NAMEWIDTH);                  n++;
        XtSetArg(args[n], XmNlabelString,
                 XmStringLtoRCreate(allrec[i].user , "ISO8859-1"));   n++;
        XtManageChild(rname[i]=
                      XmCreateLabel(rankdbox, "names", args, n));
        
        sprintf(cnvtmp, "%d", allrec[i].score);
        n = 0;
        XtSetArg(args[n], XmNy,          i * SCORELINE);              n++;
        XtSetArg(args[n], XmNx,          SCORESCORE);                 n++;
        XtSetArg(args[n], XmNalignment,  XmALIGNMENT_END);            n++;
        XtSetArg(args[n], XmNwidth,      SCOREWIDTH);                 n++;
        XtSetArg(args[n], XmNlabelString, 
                 XmStringLtoRCreate(cnvtmp , "ISO8859-1"));           n++;
        XtManageChild(rscore[i]=
                      XmCreateLabelGadget(rankdbox, "score", args, n));
        
        sprintf(cnvtmp, "/%d", allrec[i].level);
        n = 0;
        XtSetArg(args[n], XmNy,          i * SCORELINE);              n++;
        XtSetArg(args[n], XmNx,          SCORELEVEL);                 n++;
        XtSetArg(args[n], XmNalignment,  XmALIGNMENT_BEGINNING);      n++;
        XtSetArg(args[n], XmNwidth,      LEVELWIDTH);                 n++;
        XtSetArg(args[n], XmNlabelString, 
                 XmStringLtoRCreate(cnvtmp , "ISO8859-1"));           n++;
        XtManageChild(rlevel[i]=
                      XmCreateLabelGadget(rankdbox, "level", args, n));
        
        strcpy(&timestrbuf, ctime (&allrec[i].time));
        timestrbuf.tnull[0] = timestrbuf.mnull = timestrbuf.wnull = 0;
        
        n = 0;
        XtSetArg(args[n], XmNy,          i * SCORELINE);              n++;
        XtSetArg(args[n], XmNx,          SCOREWEEK);                  n++;
        XtSetArg(args[n], XmNalignment,  XmALIGNMENT_BEGINNING);      n++;
        XtSetArg(args[n], XmNlabelString, 
                 XmStringLtoRCreate(timestrbuf.timew , "ISO8859-1")); n++;
        XtManageChild(rtimew[i]=
                      XmCreateLabelGadget(rankdbox, "timew", args, n));
        
        n = 0;
        XtSetArg(args[n], XmNy,           i * SCORELINE);             n++;
        XtSetArg(args[n], XmNx,           SCOREMONTH);                n++;
        XtSetArg(args[n], XmNalignment,   XmALIGNMENT_BEGINNING);     n++;
        XtSetArg(args[n], XmNlabelString, 
                 XmStringLtoRCreate(timestrbuf.timem , "ISO8859-1")); n++;
        XtManageChild(rtimem[i] = XmCreateLabelGadget(rankdbox, "timem",
                                                      args, n));
        
        n = 0;
        XtSetArg(args[n], XmNy,           i * SCORELINE);             n++;
        XtSetArg(args[n], XmNx,           SCORETIME);                 n++;
        XtSetArg(args[n], XmNalignment,   XmALIGNMENT_BEGINNING);     n++;
        XtSetArg(args[n], XmNlabelString, 
                 XmStringLtoRCreate(timestrbuf.timeo , "ISO8859-1")); n++;
        XtManageChild(rtimeo[i] = XmCreateLabelGadget(rankdbox, "timeo",
                                                      args, n));
    }
    
    if ((newy = ((rank - (SCOREDISPLINE / 2)) * - SCORELINE)) > 0) {
        newy = 0;
    } else {
        if (cnt <= SCOREDISPLINE) {
            newy = 0;
        }
    }
    
    
    XtSetArg(args[0], XmNy, newy);
    XtSetArg(args[1], XmNx, 0);
    XtSetValues(rankdbox, args, 2);
    
    XtUnmanageChild(herepic);
    sprintf(cnvtmp, "%d", rank+1);
    rankofs = RANKWIDTH - XTextWidth(owindow_font, cnvtmp, strlen(cnvtmp)) - 3;
    XtSetArg(args[1], XmNx, rankofs);
    
    if (upped) {
        XtSetArg(args[0], XmNy,           SCORELINE * rank);
        XtSetArg(args[2], XmNlabelPixmap, up); n++;
        XtSetValues(herepic, args, 3);
        if (first) {
            XtSetArg(args[0], XmNlabelString, 
                     XmStringLtoRCreate("Welcome to mtetris.\nHere is your first score." , "ISO8859-1"));
        } else {
            if (rank == 0) {
                XtSetArg(args[0], XmNlabelString, 
                         XmStringLtoRCreate("YOU ARE TOP !" , "ISO8859-1"));
            } else {
                XtSetArg(args[0], XmNlabelString, 
                         XmStringLtoRCreate("Congratulations!\nYour rank upped." , "ISO8859-1"));
            }
        }
        XtSetValues(rcomment, args, 1);
    } else {
        XtSetArg(args[0], XmNy, SCORELINE * rank + 7);
        XtSetArg(args[2], XmNlabelPixmap, here); n++;
        XtSetValues(herepic, args, 3);
        sprintf(comment, 
                "Your best score is here.\nYour score is %d.\nTry again!", score);
        XtSetArg(args[0], XmNlabelString, 
                 XmStringLtoRCreate(comment , "ISO8859-1"));
        XtSetValues(rcomment, args, 1);
    }
    XtManageChild(herepic);
    
    if ((newy = rank - (SCOREDISPLINE / 2)) < 0) {
        newy = 0;
    } else {
        if (cnt <= SCOREDISPLINE) {
            newy = 0;
        }
    }
    
    XtManageChild (rankd);
}



static void hidescore_proc(
                           void)
{
    int i;
    
    XtUnmanageChild (rankd);
    
    for (i = 0; i < nmember; i ++) {
        XtDestroyWidget(rrank[i]);
        XtDestroyWidget(rname[i]);
        XtDestroyWidget(rscore[i]);
        XtDestroyWidget(rlevel[i]);
        XtDestroyWidget(rtimew[i]);
        XtDestroyWidget(rtimem[i]);
        XtDestroyWidget(rtimeo[i]);
    }
    
    XtFree((char *)rrank);
    XtFree((char *)rname);
    XtFree((char *)rscore);
    XtFree((char *)rlevel);
    XtFree((char *)rtimew);
    XtFree((char *)rtimem);
    XtFree((char *)rtimeo);
    XtGrabKeyboard(work_window, False,
                   GrabModeAsync, GrabModeAsync, CurrentTime);
}


static void scorefail(void)
{
    Arg args[5];
    int n;
    
    if (rankfail == NULL) {
        n = 0;
        XtSetArg(args[n], XmNdefaultPosition, TRUE);              n++;
        XtSetArg(args[n], XmNmessageString, 
                 XmStringLtoRCreate("Cannot access score file.",
                                    "ISO8859-1"));                n++;
        XtSetArg(args[n], XmNlabelFontList, owfont);              n++;
        XtSetArg(args[n], XmNdialogTitle, 
                 XmStringLtoRCreate("About . . .", "ISO8859-1")); n++;
        rankfail = XmCreateInformationDialog(toplevel_widget,
                                             "About...", args, n);
    }
    
    if (!XtIsManaged(rankfail)) {
        XtManageChild(rankfail);
    }
}



/*
 Size
 */
static void size_proc(void)
{
    Arg                  args[10];
    int                  n;
    static XtCallbackRec cb_val[] = {{sizechange_proc, NULL}, {NULL, NULL}},
                         cb_btn[] = {{sizebtn_proc, NULL},    {NULL, NULL}};
    
    if (sizepd == NULL)    {
        n = 0;
        XtSetArg(args[n], XmNwidth,           200);                 n++;
        XtSetArg(args[n], XmNheight,          200);                 n++;
        XtSetArg(args[n], XmNunitType,        XmPIXELS);            n++;
        XtSetArg(args[n], XmNnoResize,        TRUE);                n++;
        XtSetArg(args[n], XmNallowOverlap,    FALSE);               n++;
        XtSetArg(args[n], XmNdefaultPosition, TRUE);                n++;
        XtSetArg(args[n], XmNdialogTitle, 
                 XmStringLtoRCreate("Change Size", "ISO8859-1"));   n++;
        sizepd = XmCreateBulletinBoardDialog(toplevel_widget, "Change Size", 
                                             args, n);
        
        n = 0;
        XtSetArg(args[n], XmNx,           32); n++;
        XtSetArg(args[n], XmNy,           10); n++;
        XtSetArg(args[n], XmNlabelString, 
                 XmStringLtoRCreate(" Set new width\nand\nheight" ,
                                    "ISO8859-1")); n++;
        XtManageChild(tlab=XmCreateLabelGadget(sizepd, "sizlab", args, n));
        
        n = 0;
        XtSetArg(args[n], XmNshowValue,            TRUE);           n++;
        XtSetArg(args[n], XmNorientation,          XmHORIZONTAL);   n++;
        XtSetArg(args[n], XmNprocessingDirection,  XmMAX_ON_RIGHT); n++;
        XtSetArg(args[n], XmNx,                    10);             n++;
        XtSetArg(args[n], XmNy,                    83);             n++;
        XtSetArg(args[n], XmNwidth,                50);             n++;
        XtSetArg(args[n], XmNminimum,              10);             n++;
        XtSetArg(args[n], XmNmaximum,              20);             n++;
        XtSetArg(args[n], XmNvalue,                pwidth);         n++;
        cb_val[0].closure = (char *)0;
        XtSetArg(args[n], XmNvalueChangedCallback, cb_val);         n++;
        XtManageChild(wscale=XmCreateScale(sizepd, "Width", args, n));
        
        n = 0;
        XtSetArg(args[n], XmNshowValue,           TRUE);            n++;
        XtSetArg(args[n], XmNorientation,         XmHORIZONTAL);    n++;
        XtSetArg(args[n], XmNprocessingDirection, XmMAX_ON_RIGHT);  n++;
        XtSetArg(args[n], XmNx,                   90);              n++;
        XtSetArg(args[n], XmNy,                   83);              n++;
        XtSetArg(args[n], XmNwidth,               50);              n++;
        XtSetArg(args[n], XmNminimum,             pheight);         n++;
        XtSetArg(args[n], XmNmaximum,             40);              n++;
        XtSetArg(args[n], XmNvalue,               20);              n++;
        cb_val[0].closure = (char *)1;
        XtSetArg(args[n], XmNvalueChangedCallback, cb_val);         n++;
        XtManageChild(hscale=XmCreateScale(sizepd, "Height", args, n));
        
        n = 0;
        XtSetArg(args[n], XmNx,                10);                 n++;
        XtSetArg(args[n], XmNy,                157);                n++;
        cb_btn[0].closure = (char *)TRUE; 
        XtSetArg(args[n], XmNactivateCallback, cb_btn);             n++;
        XtSetArg(args[n], XmNlabelString, 
                 XmStringLtoRCreate("Ok" , "ISO8859-1"));           n++;
        XtManageChild(sokbtn = XmCreatePushButton(sizepd, "Ok", args, n));
        
        n = 0;
        XtSetArg(args[n], XmNx,                60);                 n++;
        XtSetArg(args[n], XmNy,                157);                n++;
        cb_btn[0].closure = (char *)FALSE;
        XtSetArg(args[n], XmNactivateCallback, cb_btn);             n++;
        XtSetArg(args[n], XmNlabelString, 
                 XmStringLtoRCreate("Cancel" , "ISO8859-1"));       n++;
        XtManageChild(scanbtn = XmCreatePushButton(sizepd, "Can", args, n));
        XtSetArg(args[0], XmNdefaultButton, sokbtn);
        XtSetValues(sizepd, args, 1);
        XtManageChild(sizepd);
    }
    
    if (!XtIsManaged(sizepd)) {
        n = 0;
        XtSetArg(args[n], XmNvalue, pwidth); n++;
        XtSetValues(wscale, args, n);
        
        n = 0;
        XtSetArg(args[n], XmNvalue, pheight); n++;
        XtSetValues(hscale, args, n);
        XtManageChild(sizepd);
    }
    tmpwidth  = pwidth;
    tmpheight = pheight;
}



static void sizechange_proc(Widget                 w,
                            int                    tag,
                            XmScaleCallbackStruct *scallback)
{
    if (tag == 0){
        tmpwidth = scallback->value;
    } else {
        tmpheight = scallback->value;
    }
}


static void sizebtn_proc(Widget               w,
                         int                  tag,
                         XmAnyCallbackStruct *reason)
{
    int n;
    Arg args[10];
    static XtCallbackRec
    cb_confok[]  = {{resize_proc, NULL}, {NULL, NULL}}, 
    cb_confcan[] = {{resize_proc, NULL}, {NULL, NULL}};
    
    if (XtIsManaged(sizepd)) {
        XtUnmanageChild(sizepd);
    }
    
    if (!tag) {
        return;
    }
    
    if (!training && (tmpwidth != INITWIDTH || tmpheight != INITHEIGHT)) {
        if (rsc == NULL) {
            n = 0;
            XtSetArg(args[n], XmNmessageString, 
                     XmStringLtoRCreate(RESIZENOTICE , "ISO8859-1"));        n++;
            XtSetArg(args[n], XmNokLabelString, 
                     XmStringLtoRCreate("Ok" , "ISO8859-1"));                n++;
            XtSetArg(args[n], XmNdefaultButtonType, XmDIALOG_CANCEL_BUTTON); n++;
            cb_confok[0].closure = (char *)TRUE;
            XtSetArg(args[n], XmNokCallback,      cb_confok);                n++;
            cb_confcan[0].closure = (char *)FALSE;
            XtSetArg(args[n], XmNcancelCallback,  cb_confcan);               n++;
            XtSetArg(args[n], XmNdefaultPosition, TRUE);                     n++;
            XtSetArg(args[n], XmNdialogTitle, 
                     XmStringLtoRCreate("Resize Caution", "ISO8859-1"));     n++;
            rsc = XmCreateWarningDialog(toplevel_widget, 
                                        "Resize Caution", args, n);
        }
        XtManageChild(rsc);
    } else {
        if (rsc == NULL) {
            return;
        }
        XtCallCallbacks(rsc, XmNokCallback, True);
    }
}



static void resize_proc(Widget               w,
                        int                  tag,
                        XmAnyCallbackStruct *reason)
{
    int       n, i;
    Dimension cw, ch;
    Arg       args[2];
    
    if (rsc != NULL && XtIsManaged(rsc)) {
        XtUnmanageChild(rsc);
    }
    
    if (tag) {
        if (pwidth != tmpwidth) {
            if (filler != 0) {
                XFreePixmap(dpy, filler);
            }
            filler = XCreatePixmap(dpy, work, 
                                   (unsigned int)(tmpwidth*RECTSIZE),
                                   (unsigned int)RECTSIZE,
                                   DefaultDepth(dpy, 0));
            for (i = 0; i < tmpwidth; i ++) {
                XPutImage(dpy, filler, gcc, &fillim, 
                          0, 0, i*RECTSIZE, 0, RECTSIZE, RECTSIZE);
            }
        }
        if (pwidth != tmpwidth || pheight != tmpheight) {
            if (wipeffect != 0) {
                XFreePixmap(dpy, wipeffect);
            }
            wipeffect = XCreatePixmap(dpy, work, 
                                      (unsigned int)(tmpwidth*RECTSIZE),
                                      (unsigned int)(tmpheight*RECTSIZE), 
                                      DefaultDepth(dpy, 0));
        }
        pwidth  = tmpwidth;
        pheight = tmpheight;
        width   = pwidth  * RECTSIZE;
        height  = pheight * RECTSIZE;
        if (pmap == NULL || mapsize != (pheight + 3) * pwidth) {
            if (pmap != NULL) {
                XtFree((char *)pmap);
            }
            pmap = XtCalloc(pwidth, pheight+3);
            if (savepmap != NULL) {
                XtFree((char *)savepmap);
            }
            savepmap = XtCalloc(pwidth, MAXPREPH);
            mapsize = (pheight + 3) * pwidth;
        } else {
            memset((void *)pmap, 0, (size_t)mapsize);
        }
        
        for (i = 0; i < pwidth; i++) {
            PMAP(i, pheight) = MAXP;
        }
        
        n = 0;
        XtSetArg(args[n], XmNwidth,  &cw); n++;
        XtSetArg(args[n], XmNheight, &ch); n++;
        XtGetValues(work_window, args, n);
        
        if (cw != width || ch != height) {
            n = 0;
            XtSetArg(args[n], XmNwidth, width); n++;
            XtSetValues(fall_button1, args, n);
            XtSetValues(fall_button2, args, n);
            
            fall_pix = XCreatePixmap(dpy, DefaultRootWindow(dpy),
                                     (unsigned int)width,
                                     (unsigned int)fall_height,
                                     DefaultDepth(dpy, 0));
            {
                int i;
                
                for (i = 0; i < width; i += RECTSIZE) {
                    XPutImage(dpy, fall_pix, DefaultGC(dpy, 0), &fallimage, 
                              0, 0, i, 0, RECTSIZE, fall_height);
                }
            }
            
            n = 0;
            XtSetArg(args[n], XmNlabelPixmap, fall_pix); n++;
            XtSetValues(fall_button1, args, n);
            XtSetValues(fall_button2, args, n);
            
            n = 0;
            XtSetArg(args[n], XmNwidth,  width);  n++;
            XtSetArg(args[n], XmNheight, height); n++;
            XtSetValues(work_window, args, n);
            
            XResizeWindow(dpy, work, width, height);
            XClearWindow(dpy, work);
            XDrawRectangle(dpy, work, gcb, -1, -1, width + 2, height + 2);
            
            {
                Dimension btnHeight;
                
                n = 0;
                XtSetArg(args[n], XmNheight, &btnHeight); n++;
                XtGetValues(fall_button1, args, n);
                
                n = 0;
                XtSetArg(args[n], XmNheight, height + (2 * btnHeight)); n++;
                XtSetValues(work_adb, args, n);
            }
            
            XEvent exposeEvent;
            memset(&exposeEvent, 0, sizeof(exposeEvent));
            exposeEvent.type           = Expose;
            exposeEvent.xexpose.window = work;
            XSendEvent(dpy, work, False, ExposureMask, &exposeEvent);
            XFlush(dpy);
        }
    }
    
    if (pwidth != INITWIDTH || pheight != INITHEIGHT) {
        training = True; 
    } else {
        training = False;
    }
}



/*
 *    About mtetris
 */
static void about_proc(Widget               w,
                       int                  tag,
                       XmAnyCallbackStruct *reason)
{
    Arg                  args[20];
    int                  n;
    static XtCallbackRec cb_hidemore[] = {{hidemore_proc, NULL}, {NULL, NULL}};
    
    switch (tag) {
        case 0:
            if (aboutd == NULL)    {
                about_font = XLoadQueryFont(dpy, ABFONT);
                if (about_font == NULL) {
                    about_font = XLoadQueryFont(dpy, DEFAULT_FONT);
                }            
                abfont = XmFontListCreate(about_font, "ISO8859-1");
                
                n = 0;
                XtSetArg(args[n], XmNdefaultPosition, TRUE);              n++;
                XtSetArg(args[n], XmNmessageString, 
                         XmStringLtoRCreate(ABOUTTXT , "ISO8859-1"));     n++;
                XtSetArg(args[n], XmNlabelFontList, abfont);              n++;
                XtSetArg(args[n], XmNdialogTitle, 
                         XmStringLtoRCreate("About . . .", "ISO8859-1")); n++;
                aboutd = XmCreateInformationDialog(toplevel_widget,
                                                   "About...", args, n);
            }
            if (!XtIsManaged(aboutd)) {
                XtManageChild(aboutd);
            }
            break;
        case 1:
            if (maboutd == NULL) {
                mono_font = XLoadQueryFont(dpy, MONOFONT);
                if (mono_font == NULL) {
                    mono_font = XLoadQueryFont(dpy, DEFAULT_FONT);
                }
                mnfont = XmFontListCreate(mono_font, "ISO8859-1");
                
                n = 0;
                XtSetArg(args[n], XmNverticalSpacing,   3);                    n++;
                XtSetArg(args[n], XmNhorizontalSpacing, 3);                    n++;
                XtSetArg(args[n], XmNnoResize,          FALSE);                n++;
                XtSetArg(args[n], XmNdefaultPosition,   TRUE);                 n++;
                XtSetArg(args[n], XmNdialogTitle, 
                         XmStringLtoRCreate("More About . . .", "ISO8859-1")); n++;
                maboutd = XmCreateFormDialog(toplevel_widget, 
                                             "More About...", args, n);
                
                n = 0;
                XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);      n++;
                XtSetArg(args[n], XmNtopAttachment,    XmATTACH_NONE);      n++;
                XtSetArg(args[n], XmNleftAttachment,   XmATTACH_FORM);      n++;
                XtSetArg(args[n], XmNrightAttachment,  XmATTACH_NONE);      n++;
                XtSetArg(args[n], XmNactivateCallback, cb_hidemore);        n++;
                XtSetArg(args[n], XmNlabelString, 
                         XmStringLtoRCreate("Acknowledged" , "ISO8859-1")); n++;
                XtManageChild(maboutb = XmCreatePushButton(maboutd, "Ack", args, n));
                
                
                
                n = 0;
                XtSetArg(args[n], XmNvalue,                 MOREABOUT);         n++;
                XtSetArg(args[n], XmNtopAttachment,         XmATTACH_FORM);     n++;
                XtSetArg(args[n], XmNleftAttachment,        XmATTACH_FORM);     n++;
                XtSetArg(args[n], XmNrightAttachment,       XmATTACH_FORM);     n++;
                XtSetArg(args[n], XmNbottomAttachment,      XmATTACH_WIDGET);   n++;
                XtSetArg(args[n], XmNbottomWidget,          maboutb);           n++;
                XtSetArg(args[n], XmNrows,                  20);                n++;
                XtSetArg(args[n], XmNcolumns,               65);                n++;
                XtSetArg(args[n], XmNborderWidth,           1);                 n++;
                XtSetArg(args[n], XmNcursorPositionVisible, TRUE);              n++;
                XtSetArg(args[n], XmNresizeHeight,          FALSE);             n++;
                XtSetArg(args[n], XmNresizeWidth,           FALSE);             n++;
                XtSetArg(args[n], XmNeditable,              FALSE);             n++;
                XtSetArg(args[n], XmNeditMode,              XmMULTI_LINE_EDIT); n++;
                XtSetArg(args[n], XmNscrollVertical,        TRUE);              n++;
                XtSetArg(args[n], XmNwordWrap,              TRUE);              n++;
                XtSetArg(args[n], XmNlabelFontList,         mnfont);            n++;
                XtSetArg(args[n], XmNscrollingPolicy,        XmAUTOMATIC);      n++;
                XtManageChild (mabouts = XmCreateScrolledText(maboutd, 
                                                              "MoreAboutText", args, n));
            }
            if (!XtIsManaged(maboutd)) {
                XtManageChild(maboutd);
            }
            break;
    }
}


static void hidemore_proc(void)
{
    if (XtIsManaged(maboutd)) {
        XtUnmanageChild(maboutd);
    }
}




static void soundsw_proc(Widget                        w,
                         int                           tag,
                         XmToggleButtonCallbackStruct *reason)
{
    sound = reason->set;
}



static void window_proc(Widget                        w,
                        int                           tag,
                        XmToggleButtonCallbackStruct *reason)
{
    int  i, j;
//    char     *val = "000000";
    char val[6] = "000000";
    Arg  args[1];
    
    switch (tag) {
        case SCORE:
            if (reason->set) {
                if (!XtIsManaged(scored))
                    XtManageChild(scored);
            } else {
                if (XtIsManaged(scored)) {
                    XtUnmanageChild(scored);
                }
            }
            break;
        case NEXT:
            if (reason->set) {
                if (!XtIsManaged(nextd)) {
                    XtManageChild(nextd);
                    next = XtWindow(nextpwin);
                    if (playing) {
                        XFillRectangle (dpy, nextpix, rgcc, 0, 0, MAXWID0 * RECTSIZE, 
                                        MAXHEI0 * RECTSIZE);
                        for (i = 0; i < pieces[npiece].pheight[0]; i ++)
                            for (j = 0; j < pieces[npiece].pwidth[0]; j ++)
                                if (pieces[npiece].map[0].ppmap[i][j] != 0)
                                    XCopyArea(dpy, copywork, nextpix, gcc, 
                                              cx[npiece], cy[npiece], RECTSIZE, RECTSIZE, 
                                              j * RECTSIZE, i * RECTSIZE);
                        XClearWindow (dpy, next);
                        XCopyArea(dpy, nextpix, next, gcc, 0, 0, 
                                  pieces[npiece].pwidth[0] * RECTSIZE, 
                                  pieces[npiece].pheight[0] * RECTSIZE, 
                                  (MAXWID0 - pieces[npiece].pwidth[0]) * RECTSIZE / 2, 
                                  (MAXHEI0 - pieces[npiece].pheight[0]) * RECTSIZE / 2);
                    } else {
                        XFillRectangle (dpy, nextpix, rgcc, 0, 0, 
                                        MAXWID0 * RECTSIZE, 
                                        MAXHEI0 * RECTSIZE);
                        XClearWindow (dpy, next);
                    }
                }
            } else {
                if (XtIsManaged(nextd))
                    XtUnmanageChild(nextd);
            }
            break;
        case STAT:
            if (reason->set) {
                if (!XtIsManaged(statd)) {
                    for (i = 0; i < MAXP; i ++)    {
                        sprintf (val, "%5d", statdata[i].n);
                        XtSetArg(args[0], XmNlabelString, 
                                 XmStringLtoRCreate(val , "ISO8859-1"));
                        XtSetValues(stval[i], args, 1);
                    }
                    XtManageChild(statd);
                }
            } else {
                if (XtIsManaged(statd))
                    XtUnmanageChild(statd);
            }
            break;
        case HELP:
            if (reason->set) {
                if (!XtIsManaged(helpd))
                    XtManageChild(helpd);
            } else {
                if (XtIsManaged(helpd))
                    XtUnmanageChild(helpd);
            }
            break;
    }
}



static void fallbtn_proc(Widget               w,
                         int                  tag,
                         XmAnyCallbackStruct *reason)
{
    if (playing) {
        expfall();
    }
}



static void Eventproc(Widget     w,
                      XtPointer  client_data,
                      XEvent    *event,
                      Boolean   *continue_to_dispatch_return)
{
    char *tag = (char *)client_data;
    Opaque  kbid;
    
    switch (event->type) {
        case ButtonPress:
            switch (event->xbutton.button) {
                case Button1:
                    XDefineCursor (dpy, work, left_cursor);
                    kbid = (char *)MB1T;
                    left();
                    break;
                case Button2:
                    XDefineCursor (dpy, work, rotate_cursor);
                    kbid = (char *)MB2T;
                    rotate();
                    break;
                case Button3:
                    XDefineCursor (dpy, work, right_cursor);
                    kbid = (char *)MB3T;
                    right();
                    break;
            }
            if (kbid != (char *) NULL) {
                if (reptid != 0) {
                    XtRemoveTimeOut(reptid);
                    reptid = 0;
                }
                reptid = XtAppAddTimeOut(appContext, REPEATSTART,
                                         repeat_proc, kbid);
            }
            break;
        case ButtonRelease:
            if (reptid != 0)    {
                XtRemoveTimeOut(reptid);
                reptid = 0;
            }
            XDefineCursor (dpy, work, normal_cursor);
            break;
        case KeyPress:
            kbid = NULL;
            switch (XLookupKeysym(event, 0)) {
                case XK_KP_4:
                case XK_Left:
                case XK_h:
                    if (playing && !wpause) {
                        kbid = (char *)KB4T;
                        left();
                    }
                    break;
                case XK_KP_5:    
                case XK_Up:
                case XK_j:
                    if (playing && !wpause) {
                        kbid = (char *)KB5T;
                        rotate();
                    }
                    break;
                case XK_KP_6:
                case XK_Right:
                case XK_k:
                    if (playing && !wpause) {
                        kbid = (char *)KB6T;
                        right();
                    }
                    break;
                case XK_KP_0:
                case XK_KP_2:
                case XK_Down:
                case XK_space:
                    if (playing && !wpause) {
                        expfall();
                        if (tid != 0) {
                            XtRemoveTimeOut(tid);
                            tid = 0;
                        }
                        timer_proc(NULL, NULL);
                    }
                    break;
                case XK_q:
                    if (event->xkey.state == ControlMask)
                        quit_proc();
                    break;
                case XK_s:
                    if (event->xkey.state == ControlMask)
                        if (!playing) start_proc();
                    break;
                case XK_n:
                    if (event->xkey.state == ControlMask)
                        XmToggleButtonSetState(winbtn[NEXT], 
                                               !XmToggleButtonGetState(winbtn[NEXT]), True);
                    break;
                case XK_t:
                    if (event->xkey.state == ControlMask)
                        XmToggleButtonSetState(winbtn[STAT], 
                                               !XmToggleButtonGetState(winbtn[STAT]), True);
                    break;
                default:
                    kbid = NULL;
            }
            if (kbid != NULL) {
                if (reptid != 0) {
                    XtRemoveTimeOut(reptid);
                    reptid = 0;
                }
                reptid = XtAppAddTimeOut(appContext, REPEATSTART, repeat_proc, kbid);
            }
            break;
        case KeyRelease:
            if (reptid != 0)    {
                XtRemoveTimeOut(reptid);
                reptid = 0;
            }
            break;
        default:
            break;
    }
}

static void Eventproc2(Widget     w,
                       XtPointer  client_data,
                       XEvent    *event,
                       Boolean   *continue_to_dispatch_return)
{
    char *tag = (char *)client_data;
    switch (event->type) {
        case EnterNotify:
            if (playing) {
                if (tid != 0) {
                    XtRemoveTimeOut(tid);
                    tid = 0;
                }
                tid = XtAppAddTimeOut(appContext, interval, timer_proc, 0);
            }
            XtGrabKeyboard(work_window, False, GrabModeAsync, 
                           GrabModeAsync, CurrentTime);
            if (kbstate.global_auto_repeat) {
                XAutoRepeatOff(dpy);
            }
            break;
        case LeaveNotify:
            XUngrabKeyboard(dpy, CurrentTime);
            if (kbstate.global_auto_repeat) {
                XAutoRepeatOn(dpy);
            }
            if (playing) {
                if (tid != 0) {
                    XtRemoveTimeOut(tid);
                    tid = 0;
                }
                if (reptid != 0) {
                    XtRemoveTimeOut(reptid);
                    reptid = 0;
                }
            }
            XDefineCursor (dpy, work, normal_cursor);
            break;
        default:
            break;
    }    
}


void repeat_proc(XtPointer     client_data,
                 XtIntervalId *intervalid)
{
    Opaque kbid = (Opaque)client_data;
    if (playing && !wpause && cpiece >= 0) {
        switch ((int) kbid) {
            case MB1T:
            case KB4T:
                left();
                break;
            case MB3T:
            case KB6T:
                right();
                break;
            default:
                break;
        }
        reptid = XtAppAddTimeOut(appContext, REPEATINTERVAL,
                                 repeat_proc, kbid);
    } else {
        reptid = 0;
    }
}



static void gameover_pic(int ex,
                         int ey, 
                         int ew, 
                         int eh)
{
    int i, x, y;
    
    for (i = ey; i < ey + eh; i++) {
        XCopyArea(dpy, filler, work, gcc, ex * RECTSIZE, 0, 
                  ew * RECTSIZE, RECTSIZE, ex * RECTSIZE, i*RECTSIZE);
    }
    x = ((pwidth - 8) / 2) * RECTSIZE;
    y = ((pheight - 1) / 2) * RECTSIZE;
    XCopyArea(dpy, gameover, work, gcc, 0, 0, 
              gameover_width, RECTSIZE, x, y);
}



static void upscore(int mask)
{
    Arg  args[1];
    int  tmplevel;
    char val[6] = "000000";
    
    tmplevel = initlevel + sline / 15;
    if (level != tmplevel) {
        level = tmplevel;
        mask |= LEVELUP;
    }
    
    interval = initinterval - (initinterval / 12) *
    (initlevel +(((sline) * 100) / 15) / 100) ;
    if (interval < 30) interval = 30;
    if ((mask & SCOREUP) != 0) {
        sprintf(val, "%5d", score);
        XtSetArg(args[0], XmNlabelString,
                 XmStringLtoRCreate(val , "ISO8859-1"));
        XtSetValues(scv, args, 1);
    }
    
    if ((mask & LEVELUP) != 0) {
        sprintf(val, "%5d", level);
        XtSetArg(args[0], XmNlabelString, 
                 XmStringLtoRCreate(val , "ISO8859-1"));
        XtSetValues(levv, args, 1);
    }
    if ((mask & LINEUP) != 0) {
        sprintf(val, "%5d", sline);
        XtSetArg(args[0], XmNlabelString, 
                 XmStringLtoRCreate(val , "ISO8859-1"));
        XtSetValues(linv, args, 1);
    }
}



static void gameover_proc(void)
{
    Arg args[1];
    
    if (tid != 0) {
        XtRemoveTimeOut(tid);
        tid = 0;
    }
    if (reptid != 0) {
        XtRemoveTimeOut(reptid);
        reptid = 0;
    }
    XtRemoveEventHandler(work_window, 
                         ButtonPressMask | ButtonReleaseMask, 
                         False, Eventproc, NULL);
    playing = False;
    over = True;
    if (XtIsManaged(nextd)) {
        XFillRectangle (dpy, nextpix, rgcc, 0, 0, MAXWID0 * RECTSIZE, 
                        MAXHEI0 * RECTSIZE);
        XClearWindow (dpy, next);
    }
    
    gameover_pic(0, 0, (int)pwidth, (int)pheight);
    XtSetArg(args[0], XmNsensitive, TRUE);
    XtSetValues(startbtn, args, 1);
    XtSetValues(sizebtn, args, 1);
    XtSetValues(lsetupbtn, args, 1);
    XUngrabKeyboard(dpy, CurrentTime);
    if (kbstate.global_auto_repeat) {
        XAutoRepeatOn(dpy);
    }
    scorefileupdate();
}
