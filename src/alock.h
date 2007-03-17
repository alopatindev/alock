
#ifndef _ALOCK_H_
#define _ALOCK_H_

/* ---------------------------------------------------------------- *\

  file    : alock.h
  author  : m. gumz <akira at fluxbox dot org>
  copyr   : copyright (c) 2005 - 2007 by m. gumz

  license : see LICENSE

  start   : Sa 30 Apr 2005 11:51:52 CEST

\* ---------------------------------------------------------------- */
/* ---------------------------------------------------------------- *\

  about :

\* ---------------------------------------------------------------- */

#include <X11/Xlib.h>
#include <stdio.h>

/* ---------------------------------------------------------------- *\
\* ---------------------------------------------------------------- */
#ifdef DEBUG
#    define DBGMSG fprintf(stderr, "%s : %d\n", __FUNCTION__, __LINE__); fflush(stderr)
#else
#    define DBGMSG
#endif /* DEBUG */
/*------------------------------------------------------------------*\
\*------------------------------------------------------------------*/

struct aXInfo {

    Display*  display;

    int       nr_screens;

    Window*   root;
    Colormap* colormap;

    Window*   window;
    Cursor*   cursor;
};

struct aAuth {
    const char* name;
    int (*init)(const char* args);
    int (*auth)(const char* pass);
    int (*deinit)();
};

struct aCursor {
    const char* name;
    int (*init)(const char* args, struct aXInfo* xinfo);
    int (*deinit)(struct aXInfo* xinfo);
};

struct aBackground {
    const char* name;
    int (*init)(const char* args, struct aXInfo* xinfo);
    int (*deinit)(struct aXInfo* xinfo);
};

struct aOpts {
    struct aAuth* auth;
    struct aCursor* cursor;
    struct aBackground* background;
};


/*------------------------------------------------------------------*\
\*------------------------------------------------------------------*/
void alock_string2lower(char* string);
int alock_native_byte_order(void);
int alock_alloc_color(const struct aXInfo* xinfo, const int scr,
        const char* color_name,
        const char* fallback_name,
        XColor* result);
int alock_check_xrender(const struct aXInfo* xinfo);
int alock_shade_pixmap(const struct aXInfo* xinfo,
        int scr,
        const Pixmap src_pm,
        Pixmap dst_pm,
        unsigned char shade,
        int src_x, int src_y,
        int dst_x, int dst_y,
        unsigned int width,
        unsigned int height);

#endif /* _ALOCK_H_ */

