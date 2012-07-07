/* ---------------------------------------------------------------- *\

  file    : alock_frame.c
  author  : m. gumz <akira at fluxbox dot org>
  copyr   : copyright (c) 2008 by m. gumz

  license : see LICENSE

  start   : Sa 04 Okt 2008 12:47:35 CEST

\* ---------------------------------------------------------------- */
/* ---------------------------------------------------------------- *\

  about :

\* ---------------------------------------------------------------- */

/* ---------------------------------------------------------------- *\
  includes
\* ---------------------------------------------------------------- */
#include "alock_frame.h"
#include "alock.h"

#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/Xatom.h>
#include <stdlib.h>

/* ---------------------------------------------------------------- *\
\* ---------------------------------------------------------------- */

#define XKB_CTRLS_MASK (XkbAllControlsMask & ~(XkbInternalModsMask | XkbIgnoreLockModsMask))

struct aSide {
    Window win;
    int width;
    int height;
    GC gc;
};

struct aFrame {
    int visible;
    struct aSide top;
    struct aSide left;
    struct aSide right;
    struct aSide bottom;
    XColor color;
    struct aXInfo* xi;
};

/* ---------------------------------------------------------------- *\
\* ---------------------------------------------------------------- */

struct aFrame* alock_create_frame(struct aXInfo* xi, int x, int y, int width, int height, int line_width) {

    Display* dpy = xi->display;
    struct aFrame* frame;
    struct aSide* side;
    int i;
    XSetWindowAttributes xswa;
    xswa.override_redirect = True;
    xswa.colormap = xi->colormap[0];

    frame = (struct aFrame*)calloc(1, sizeof(struct aFrame));

    if (frame == 0)
        return 0;

    frame->xi = xi;
    /*

       ascii - kungfoo

       p1 ------------------------------------------------- p2
       |                        top                          |
       p3 --------------------------------------------------p4
       p4 --- p5                                     p6 --- p7
       |   l   |                                     |   r   |
       |   e   |                                     |   i   |
       |   f   |                                     |   g   |
       |   t   |                                     |   h   |
       |       |                                     |   t   |
       p8 --- p9                                     pa --- pb
       pc ------------------------------------------------- pd
       |                      bottom                         |
       pe ------------------------------------------------- pf

    */

    frame->top.width = width;
    frame->top.height = line_width;
    frame->top.win = XCreateWindow(dpy, xi->root[0],
                x, y, frame->top.width, frame->top.height,
                0, CopyFromParent, InputOutput, CopyFromParent, CWOverrideRedirect|CWColormap, &xswa);
    frame->bottom.width = width;
    frame->bottom.height = line_width;
    frame->bottom.win = XCreateWindow(dpy, xi->root[0],
                x, y + height - line_width, frame->bottom.width, frame->bottom.height,
                0, CopyFromParent, InputOutput, CopyFromParent, CWOverrideRedirect|CWColormap, &xswa);

    frame->left.width = line_width;
    frame->left.height = height - line_width - 1;
    frame->left.win = XCreateWindow(dpy, xi->root[0],
                x, y + line_width, frame->left.width, frame->left.height,
                0, CopyFromParent, InputOutput, CopyFromParent, CWOverrideRedirect|CWColormap, &xswa);
    frame->right.width = line_width;
    frame->right.height = height - line_width - 1;
    frame->right.win = XCreateWindow(dpy, xi->root[0],
                x + width - line_width, y + line_width, frame->right.width, frame->right.height,
                0, CopyFromParent, InputOutput, CopyFromParent, CWOverrideRedirect|CWColormap, &xswa);


//    alock_show_frame(frame);

    side = (struct aSide*)&frame->top;
    for (i = 0; i < 4; i++) {
        side[i].gc = XCreateGC(dpy, side[i].win, 0, 0);
    }

    return frame;
}

void alock_free_frame(struct aFrame* frame) {
    struct aSide* side = (struct aSide*)&frame->top;
    struct aXInfo* xi = frame->xi;
    int i;

    for (i = 0; i < 4; i++) {
        XFreeGC(xi->display, side[i].gc);
        XDestroyWindow(xi->display, side[i].win);
    }

    free(frame);
}

void alock_draw_frame(struct aFrame* frame, const char* color_name) {

    struct aSide* side = (struct aSide*)&frame->top;
    struct aXInfo* xi = frame->xi;
    Display* dpy = xi->display;
    XGCValues gcvals;
    XColor tmp;
    int i;

    if (!frame->visible) {
        alock_show_frame(frame);
    }

    XAllocNamedColor(dpy, xi->colormap[0], color_name, &frame->color, &tmp);
    gcvals.foreground = frame->color.pixel;

    for (i = 0; i < 3; i++) {
        XChangeGC(dpy, side[i].gc, GCForeground, &gcvals);
        XFillRectangle(dpy, side[i].win, side[i].gc, 0, 0, side[i].width, side[i].height);
    }
}

int get_gr_num(Display *dpy, XkbDescPtr kb) {
    int rv;

    if (XkbGetControls(dpy, XKB_CTRLS_MASK, kb) != Success)
    printf("XkbGetControls() failed\n");
    rv = kb->ctrls->num_groups;
    XkbFreeControls(kb, XKB_CTRLS_MASK, 0);
    return rv;
}

void get_gr_names(Display *dpy, XkbDescPtr kb, int ngroups, char **groups) {
    char *name = NULL;
    int i;

    if (XkbGetNames(dpy, XkbGroupNamesMask, kb) != Success)
        printf("XkbGetNames() failed\n");
  
    for (i = 0; i < ngroups; i++) {
        if (kb->names->groups[i]) {
            if ((name = XGetAtomName(dpy, kb->names->groups[i])))
                snprintf(groups[i], 4, name);
            else
                printf("XGetAtomName() failed\n");
        }
    }
    XkbFreeNames(kb, XkbGroupNamesMask, 0);
}

void get_active_gr(Display *dpy, int *active) {
    XkbStateRec state;

    if (XkbGetState(dpy, XkbUseCoreKbd, &state) != Success)
        printf("XkbGetState() failed\n");
    *active = state.group;        
}

char** alock_prepare_kb_layouts(struct aFrame* frame) {
    Display* dpy = frame->xi->display;
    XkbDescPtr kb = XkbAllocKeyboard();

    int ngroups = 0;
    char** groups;
    int i;

    ngroups = get_gr_num(dpy, kb);

    groups = malloc(sizeof(char*)*ngroups);
    for (i = 0; i < ngroups; i++)
	    groups[i] = malloc(256); 

    get_gr_names(dpy, kb, ngroups, groups);
    XkbSelectEvents(dpy, XkbUseCoreKbd, XkbAllEventsMask, XkbAllEventsMask);

    return groups;
}

void alock_draw_kb_layout(struct aFrame* frame, char** groups) {
    struct aXInfo* xi = frame->xi;
    struct aSide* side = (struct aSide*)&frame->top;
    Display* dpy = xi->display;
    XGCValues gcvals;
    XColor tmp;
    //XkbEvent ev;
    int active = 0;
    static int old = -1;
    char color_name[5];

    get_active_gr(dpy, &active);
    if(active != old) {
        /*puts(groups[active]);
        fflush(stdout);*/

        if (strncmp(groups[active], "Eng", 4) != 0)
            strncpy(color_name, "red", 5);
        else
            strncpy(color_name, "blue", 5);

        old = active;
    }
	//XNextEvent(dpy, &ev.core);
    

    XAllocNamedColor(dpy, xi->colormap[0], color_name, &frame->color, &tmp);
    gcvals.foreground = frame->color.pixel;

    XChangeGC(dpy, side[3].gc, GCForeground, &gcvals);
    XFillRectangle(dpy, side[3].win, side[3].gc,
                   side[3].width - side[3].width / 35, 0,
                   side[3].width / 35, side[3].height);
}

void alock_draw_box(struct aFrame* frame, int num) {
    struct aSide* side = (struct aSide*)&frame->top;
    struct aXInfo* xi = frame->xi;
    Display* dpy = xi->display;
    XGCValues gcvals;
    XColor tmp;
    int i;

    if (!frame->visible) {
        alock_show_frame(frame);
    }


    XAllocNamedColor(dpy, xi->colormap[0], "black", &frame->color, &tmp);
    gcvals.foreground = frame->color.pixel;

    XChangeGC(dpy, side[3].gc, GCForeground, &gcvals);
    XFillRectangle(dpy, side[3].win, side[3].gc,
                   0, 0, side[3].width, side[3].height);


    XAllocNamedColor(dpy, xi->colormap[0], "green", &frame->color, &tmp);
    gcvals.foreground = frame->color.pixel;

    XChangeGC(dpy, side[3].gc, GCForeground, &gcvals);
    XFillRectangle(dpy, side[3].win, side[3].gc,
                   0, 0, side[3].width / 35 * num, side[3].height);
}

void alock_show_frame(struct aFrame* frame) {

    int i;
    struct aSide* side = (struct aSide*)&frame->top;

    if (frame->visible)
        return;

    for (i = 0; i < 4; i++) {
        XMapWindow(frame->xi->display, side[i].win);
        XRaiseWindow(frame->xi->display, side[i].win);
    }

    frame->visible = 1;
}

void alock_hide_frame(struct aFrame* frame) {

    int i;
    struct aSide* side = (struct aSide*)&frame->top;

    if (!frame->visible)
        return;

    for (i = 0; i < 4; i++) {
        XUnmapWindow(frame->xi->display, side[i].win);
    }

    frame->visible = 0;
}

/* ---------------------------------------------------------------- *\
\* ---------------------------------------------------------------- */


