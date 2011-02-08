#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <assert.h>

#ifndef SCREEN_WIDTH
  #define SCREEN_WIDTH	1400
#endif
#ifndef SCREEN_HEIGHT
  #define SCREEN_HEIGHT	1050
#endif

Display *dpy;
Window win;
int scr;

int main() {
	char *str = "My program title";

	dpy=XOpenDisplay(NULL);
	assert(dpy);
	scr=DefaultScreen(dpy);

	win=XCreateSimpleWindow(dpy, RootWindow(dpy,scr), 
			0, 0, /* coordinates */
			SCREEN_WIDTH/2, SCREEN_HEIGHT/2, /* size */
			1, /* border width */
			WhitePixel(dpy,scr), /* foreground */
			BlackPixel(dpy,scr)  /* background */
			);
	XMapWindow(dpy, win);
	XMoveWindow(dpy, win, 0, 0);

	XTextProperty title;
	XStringListToTextProperty(&str, 1, &title);
	XSetWMName(dpy, win, &title);
	XSetWMIconName(dpy, win, &title);


	XFlush(dpy);
	getchar();
	return EXIT_SUCCESS;
}
