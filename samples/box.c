/*
   Simple Xlib application drawing a box in a window.
   */
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

const int SCREEN_WIDTH=1400;
const int SCREEN_HEIGHT=1050;


Display *dpy;	
Window win;
int scr;

void draw_function();
void draw_grid(const int);

int main(int argc, char *argv[]) {

	printf("Initialized...");
	fflush(stdout);

	int black, white;
	XEvent e;
	assert(dpy=XOpenDisplay(NULL));
	scr=DefaultScreen(dpy);

	black=BlackPixel(dpy, scr);
	white=WhitePixel(dpy, scr);

	assert(win=XCreateSimpleWindow(dpy, RootWindow(dpy, scr), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, white, white));

	XSelectInput(dpy, win, ExposureMask | KeyPressMask);

	XMapWindow(dpy, win);
	XMoveWindow(dpy, win, 0, 0);

	while(1) {
		XNextEvent(dpy, &e);
		/* redraw if moved*/
		if(e.type==Expose)
			draw_function(dpy, win, scr);
	}

	XCloseDisplay(dpy);
	return 0;
}

void draw_function() {
//	XFillRectangle(dpy, win, DefaultGC(dpy, scr), 20, 20, 10, 10);
//	XDrawString(dpy, win, DefaultGC(dpy, scr), 50, 50, "Hello, World!",strlen("Hello, World!"));
	draw_grid(50);
}

void draw_grid(const int width) {
	GC gc=XCreateGC(dpy, win, 0, NULL);
	int x;
	for(x=width; x<SCREEN_WIDTH-width; x+=width)
		XDrawLine(dpy, win, gc, x, 0, x, SCREEN_HEIGHT-width);
	for(x=width; x<=SCREEN_HEIGHT-width; x+=width)
		XDrawLine(dpy, win, gc, 0, x, SCREEN_WIDTH-width, x);
	XFlush(dpy);
}
