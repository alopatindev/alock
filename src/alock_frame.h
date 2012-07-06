
#ifndef _ALOCK_FRAME_H_
#define _ALOCK_FRAME_H_

/* ---------------------------------------------------------------- *\

  file    : alock_frame.h
  author  : m. gumz <akira at fluxbox dot org>
  copyr   : copyright (c) 2008 by m. gumz

  license : see LICENSE

  start   : Sa 04 Okt 2008 12:45:46 CEST

\* ---------------------------------------------------------------- */
/* ---------------------------------------------------------------- *\

  about : functions to render a colored frame to the screen
          mostly for visual feedback purposes :)

\* ---------------------------------------------------------------- */

/* ---------------------------------------------------------------- *\
\* ---------------------------------------------------------------- */

struct aXInfo;
struct aFrame;

struct aFrame* alock_create_frame(struct aXInfo* xi, int x, int y, int width, int height, int line_width);
void alock_draw_frame(struct aFrame* frame, const char* color_name);
void alock_show_frame(struct aFrame* frame);
void alock_hide_frame(struct aFrame* frame);
void alock_free_frame(struct aFrame* frame);

void alock_draw_box(struct aFrame* frame, int num);

/* ---------------------------------------------------------------- *\
\* ---------------------------------------------------------------- */
#endif // _ALOCK_FRAME_H_

