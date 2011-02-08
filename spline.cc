/*
   Simple Xlib application drawing a box in a window.
   */
#include <X11/Xlib.h>
#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include "spline.h"
#include <vector>

Display *dpy;
Window win;
int scr;

void draw_function();
void draw_grid(const int);
double spline_f(const double &x);

int main(int argc, char *argv[]) {

	std::cout << "Initialized...";	

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
			draw_function();
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
		XDrawLine(dpy, win, gc, width, x, SCREEN_WIDTH-width, x);
	XFlush(dpy);
}

void plot(/*function_stack &fstack, double xmin, double xmax, TGraph *F*/) {
//        double ymin=-1,ymax=1;
        double ymin=0,ymax=0;
        double t;
        fp f;
        for(double x=xmin;x<xmax;x+=0.1) {
                for(int i=0; i<fstack.size(); i++) {
                t=f(x);
                if(t<ymin)
                        ymin=t;
                if(t>ymax)
                        ymax=t;
                }
        }

        double yrange=fabs(ymax-ymin);
        double xrange=fabs(xmax-xmin);
        double y_cell_size=F->Image1->Height/yrange;
        double x_cell_size=F->Image1->Width/xrange;
//      double kost=(F->Image1->Height/y_cell_size)/ymax;
        // grid
        F->Image1->Canvas->Pen->Color=clGray;

        for(int y=ymax*y_cell_size;y<F->Image1->Height;y+=y_cell_size) {
                F->Image1->Canvas->MoveTo(0,y);
                F->Image1->Canvas->LineTo(F->Image1->Width,y);
        }
        for(int y=ymax*y_cell_size;y>0;y-=y_cell_size) {
                F->Image1->Canvas->MoveTo(0,y);
                F->Image1->Canvas->LineTo(F->Image1->Width,y);
        }
        for(int x=0;x<F->Image1->Width;x+=x_cell_size) {
                F->Image1->Canvas->MoveTo(x,0);
                F->Image1->Canvas->LineTo(x,F->Image1->Height);
        }

        F->Image1->Canvas->Pen->Color=clBlack;
        F->Image1->Canvas->Pen->Width=2;
        F->Image1->Canvas->MoveTo(0,ymax*y_cell_size);
        F->Image1->Canvas->LineTo(F->Image1->Width,ymax*y_cell_size);
        F->Image1->Canvas->Pen->Width=1;
        // end grid

        F->Image1->Canvas->Pen->Color=clBlack;
        for(int i=0; i<fstack.size(); i++) {
                f=fstack.get(i);
                F->Image1->Canvas->Pen->Color=fstack.get_color(i);
                F->Image1->Canvas->MoveTo( (xmin-xmin)*x_cell_size , (-f(xmin)+ymax)*y_cell_size);
                for(double x=xmin;x<xmax;x+=2/x_cell_size) {
                        F->Image1->Canvas->LineTo( (x-xmin)*x_cell_size , (-f(x)+ymax)*y_cell_size);
//                        F->Image1->Canvas->LineTo( (x-xmin)*x_cell_size , (-f(x)*kost+ymax)*y_cell_size );
		}
	}
}
