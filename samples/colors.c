/*
 * Simple example of using Xlib: colors, rectangles
 *
 * by The Hog, 2008
 */
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>

Window win;
Display *dpy;
int scr;
XColor green, blue;
Colormap scr_cmap;

const int width = 400;
const int step = 20;

int main(int argc, char *argv[]) {
	Status rc;
	GC cont;

	dpy = XOpenDisplay(NULL);
	if( !dpy ) {
		fprintf(stderr, "Failed to open display");
		return EXIT_FAILURE;
	}
	scr = DefaultScreen(dpy);
	scr_cmap = DefaultColormap(dpy, scr);

	rc = XAllocNamedColor(dpy, scr_cmap, "green", &green, &green);
	if( !rc ) {
		fprintf(stderr, "XAllocNamedColor: failed to allocate color");
		return EXIT_FAILURE;
	}
	rc = XAllocNamedColor(dpy, scr_cmap, "blue", &blue, &blue);
	if ( !rc ) {
		fprintf(stderr, "XAllocNamedColor: failed to allocate color");
		return EXIT_FAILURE;
	}

	win = XCreateSimpleWindow(dpy, RootWindow(dpy, scr), 0, 0, width, width, 0, blue.pixel, blue.pixel);
	XMapWindow(dpy, win);

	cont = XCreateGC(dpy, win, 0, NULL);
	XSetFillStyle(dpy, cont, FillSolid);

	int s;
	int t = 0, q = 1;
	while(1) {
		s = 0;
		while( s<width/2 ) {
			XSetForeground(dpy, cont, t ? 
					(( (s% (2*step) )==0) ? green.pixel: blue.pixel) : 
					(( (s% (2*step) )==0) ? blue.pixel: green.pixel) );
			if(q)
				XFillRectangle(dpy, win, cont, s, s, width - 2*s, width - 2*s);
			else
				XFillRectangle(dpy, win, cont, width/2 - s, width/2 - s, 2*s, 2*s);

			s+=step;
			XFlush(dpy);
//			sleep(1);
			if( s!=step )
				napms(500);
		}
		t=!t;
		q=!q;

	}
	XCloseDisplay(dpy);
	return EXIT_SUCCESS;
}

