/*
 * Simple function plotter
 *
 * The Hog, 2008
 */

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <pthread.h>

#include "icon.xbm"
#include "func.h"
#include "initial.h"

#define SHIFT	15
#define MULT	1.2
#define CDIAM	6

#define MIN(a,b) ( ((a)<(b)) ? (a) : (b) )
#define MAX(a,b) ( ((a)>(b)) ? (a) : (b) )

/* display */
Display *dpy;
Window win;
int scr;
GC cont;
int width, height;
char buffer[128];
int pos_x, pos_y;
void drawInfo();

/* color manipulation */
unsigned long black, white;
XColor green, blue, red;
void getColors();

/* plot */
extern MTYPE lb, rb;	// in func.c
MTYPE pixels_in_cell, ymin, ymax, f_h;
void draw(int, int);
MTYPE y_p2c(MTYPE);
MTYPE x_p2c(MTYPE);
MTYPE F(MTYPE, int);

/* modifiers */
MTYPE shift_y=0, shift_x=0, shift_pix=1;
void* dec(void*);
void* inc(void*);

MTYPE y_p2c(MTYPE y) {
	return -( (y-shift_y)/pixels_in_cell - ymax );

}
MTYPE x_p2c(MTYPE x) {
	return (x-shift_x)/pixels_in_cell + lb;
}

void* dec(void* what) {
	draw(0,0);
	if( (MTYPE*) what==&shift_y )
		shift_y-=SHIFT*MULT;
	if( (MTYPE*) what==&shift_x )
		shift_x-=SHIFT*MULT;
	if( (MTYPE*) what==&shift_pix )
		shift_pix/=MULT;
	draw(1,1);
	drawInfo();
	pthread_exit(NULL);
}

void* inc(void* what) {
	draw(0,0);
	if( (MTYPE*) what==&shift_y )
		shift_y+=SHIFT*MULT;
	if( (MTYPE*) what==&shift_x )
		shift_x+=SHIFT*MULT;
	if( (MTYPE*) what==&shift_pix ) {
		if( shift_pix<50 )
			shift_pix*=MULT;
	}
	draw(1,1);
	drawInfo();
	pthread_exit(NULL);
}

void* def(void *a) {
	draw(0,0);
	shift_x=0;
	shift_y=0;
	shift_pix=1;
	draw(1,1);
	drawInfo();
	pthread_exit(NULL);
}

void getColors() {
	Colormap cmap;
	Status rc;

	black = BlackPixel(dpy, scr);
	white = WhitePixel(dpy, scr);

	cmap = DefaultColormap(dpy, scr);
	rc = XAllocNamedColor(dpy, cmap, "green", &green, &green);
	if( !rc ) {
		fprintf(stderr, "XAllocNamedColor: failed to allocate color");
		green.pixel = black;
	}
	rc = XAllocNamedColor(dpy, cmap, "blue", &blue, &blue);
	if( !rc ) {
		fprintf(stderr, "XAllocNamedColor: failed to allocate color");
		blue.pixel = black;
	}
	rc = XAllocNamedColor(dpy, cmap, "red", &red, &red);
	if( !rc ) {
		fprintf(stderr, "XAllocNamedColor: failed to allocate color");
		red.pixel = black;
	}
}

/* converts y-coordinates of toPlot(x) from cells to pixels */
MTYPE F(MTYPE x, int type) {
	if( type==1 )
		return (-toPlot(x)+ymax)*pixels_in_cell + shift_y + height/2;
	else
		return (-f(x)+ymax)*pixels_in_cell + shift_y + height/2;
}

/* type:
 * 	2 - function parameters changed
 * 	1 - just modifiers changed
 * 	0 - all intact, redrawing previous
 */ 
void draw(int type, int color) {
	int i;
	MTYPE x;

	if(type==2) {
		f_h = f_max(&toPlot, lb, rb) - f_min(&toPlot, lb, rb);
		ymin = f_min(&toPlot, lb, rb);
		ymax = f_max(&toPlot, lb, rb);
	}
	if(type>=1) {
		pixels_in_cell = MIN( height/f_h, width/(rb-lb) );
		pixels_in_cell *= shift_pix;
	}
	if( color==0 )
		XSetForeground(dpy, cont, white);

	/* drawing spline */
	if(color!=0)
		XSetForeground(dpy, cont, red.pixel);
	for(x=lb; x<rb; x+=0.05)
		XDrawLine(dpy, win, cont, (x-lb)*pixels_in_cell + shift_x, F(x,1), (x+0.05-lb)*pixels_in_cell + shift_x, F(x+0.05,1));

	sprintf(buffer, "spline function");
	XDrawString(dpy, win, cont,
		20, 40,
		buffer, strlen(buffer));

	/* original function */
	if(color!=0)
		XSetForeground(dpy, cont, green.pixel);
	for(x=lb; x<rb; x+=0.05)
		XDrawLine(dpy, win, cont, (x-lb)*pixels_in_cell + shift_x, F(x,0), (x+0.05-lb)*pixels_in_cell + shift_x, F(x+0.05,0));

	sprintf(buffer, "initial function");
	XDrawString(dpy, win, cont,
		20, 20,
		buffer, strlen(buffer));

	/* points of intersection */
	if(color!=0)
		XSetForeground(dpy, cont, blue.pixel);
	for(i=0; i<=k; i++)
		XFillArc(dpy, win, cont, 
				(xx[i]-lb)*pixels_in_cell + shift_x - CDIAM/2, 
				(ymax - fx[i])*pixels_in_cell + shift_y + height/2 - CDIAM/2, 
				CDIAM, CDIAM, 
				0, 360*64);
	XFlush(dpy);
}


void drawInfo() {
	XSetForeground(dpy, cont, white);
	XFillRectangle(dpy, win, cont, 20, height-40, 170, 50);
	sprintf(buffer, "coord: [ %.3f ; %.3f ]", x_p2c(pos_x), y_p2c(pos_y));
	XSetForeground(dpy, cont, black);
		XDrawString(dpy, win, cont,
	     		20, height-20,
	       		buffer, strlen(buffer));
	if( x_p2c(pos_x)>=lb && x_p2c(pos_x)<=rb )
		sprintf(buffer, "delta: %f", fabs(toPlot(x_p2c(pos_x)) - f(x_p2c(pos_x))));
	else
		sprintf(buffer, "delta: undefined");
	XDrawString(dpy, win, cont,
			20, height-8,
			buffer, strlen(buffer));
}

int main() {
	XTextProperty title;
	XEvent event; 
	XSizeHints *hints;
	XWMHints *wm_hints;
	KeySym key;

	Pixmap pix;

	pthread_t thread;
	
	/* grepare toPlot function */
	spline_init();

	/* visual routines */
	int pressed_1[3] = {0, 0, 0};	// pressed state of button1: condition and coordinates

	char *caption = "Interpolation";
	char *dpy_str = getenv("DISPLAY");

	fprintf(stderr, "Connecting to %s\t ", dpy_str);
	dpy = XOpenDisplay(dpy_str);
	if( !dpy ) {
		fprintf(stderr, "Unable to connect\n");
		exit(EXIT_FAILURE);
	} else {
		fprintf(stderr, "OK\n");
	}

	scr = DefaultScreen(dpy);
	getColors();

	width = 2 * DisplayWidth(dpy, scr) / 3;
	height = 2 * DisplayHeight(dpy, scr) / 3;

	win = XCreateSimpleWindow(dpy,
			RootWindow(dpy, scr),
			0, 0,
			width, height,
			0,
			black, white
			);

	XStringListToTextProperty(&caption, 1, &title);
	XSetWMName(dpy, win, &title);
	XSetWMIconName(dpy, win, &title);


	cont = XCreateGC(dpy, win, 0, NULL);

	if( cont<0 ) {
		fprintf(stderr, "XCReateGC: unable to set GC");
		exit(EXIT_FAILURE);
	}

	/* making window unresizeable */
	hints = XAllocSizeHints();
	if( !hints ) {
		fprintf(stderr, "XAllocSizeHints: out of memory");
		exit(EXIT_FAILURE);
	}
	hints->flags = PMaxSize | PMinSize;
	hints->min_width = width;
	hints->min_height = height;
	hints->max_width = width;
	hints->max_height = height;
	XSetWMNormalHints(dpy, win, hints);
	XFree(hints);

	/* setting icon */
	pix = XCreateBitmapFromData(dpy, win, icon_bits, icon_width, icon_height);
	if( !pix ) {
		fprintf(stderr, "XCreateBitmapFromData: cannot create icon");
		exit(EXIT_FAILURE);
	}

	wm_hints = XAllocWMHints(); 
	if( !wm_hints ) {
		fprintf(stderr, "XAllocWMHints: out of memory");
		exit(EXIT_FAILURE);
	}
	wm_hints->flags = IconPixmapHint | StateHint | IconPositionHint;
	wm_hints->icon_pixmap = pix;
	wm_hints->initial_state = IconicState;
	wm_hints->icon_x = 0;
	wm_hints->icon_y = 0;

	XSetWMHints(dpy, win, wm_hints);
	XFree(wm_hints);
	

	/* making window visible */
	XMapWindow(dpy, win);
	draw(2,1);

	XSelectInput(dpy, win, ExposureMask | KeyPressMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask);

	while( 1 ) {
		XNextEvent(dpy, &event);
		switch( event.type ) {
			case Expose:
				if( event.xexpose.count>0 )
					break;
				draw(0,1);
				XFlush(dpy);
				break;
			case KeyPress:
				XLookupString((XKeyEvent*) &event, NULL, 0, &key, NULL);
				/* moving window */
				switch( key ) {
					case XK_Down:
						pthread_create(&thread, NULL, dec, (void*)&shift_y);
						break;
					case XK_Up:
						pthread_create(&thread, NULL, inc, (void*)&shift_y);
						break;
					case XK_Right:
						pthread_create(&thread, NULL, dec, (void*)&shift_x);
						break;
					case XK_Left:
						pthread_create(&thread, NULL, inc, (void*)&shift_x);
						break;
					case XK_plus:
						pthread_create(&thread, NULL, inc, (void*)&shift_pix);
						break;
					case XK_minus:
						pthread_create(&thread, NULL, dec, (void*)&shift_pix);
						break;
					case XK_equal:
						pthread_create(&thread, NULL, def, (void*)NULL);
						break;
					case XK_Escape:
						XCloseDisplay(dpy);
						exit(EXIT_SUCCESS);
					default:
						break;
				}
				break;
			case ButtonPress:
				switch( event.xbutton.button ) {
					case Button1:
						pressed_1[0]=1;
						pressed_1[1]=event.xmotion.x;
						pressed_1[2]=event.xmotion.y;
						break;
					default:
						break;
				}
				break;
			case ButtonRelease:
				switch( event.xbutton.button ) {
					case Button1:
						pressed_1[0]=0;
						break;
					default:
						break;
				}
				break;

			case MotionNotify:
				if( pressed_1[0] ) {
					/* if button_1 is pressed while moving */
					draw(1,0);
					shift_x-=pressed_1[1]-event.xmotion.x;
					shift_y-=pressed_1[2]-event.xmotion.y;
					pressed_1[1]=event.xmotion.x;
					pressed_1[2]=event.xmotion.y;
					draw(1,1);
				}
				pos_x=event.xmotion.x;
				pos_y=event.xmotion.y;
				drawInfo();

				break;
			default:
				break;
		} /* switch(event.type) */
	} /* endless loop */

	XCloseDisplay(dpy);
	return EXIT_SUCCESS;
}
