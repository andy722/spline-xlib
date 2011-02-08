#include "func.h"
#include "initial.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

const MTYPE step = 0.1;
MTYPE lb = left, rb = right;

/* points of interpolation */
MTYPE xx[PMAX];
MTYPE fx[PMAX];

//static MTYPE ff[PMAX];
static MTYPE *ff;
static MTYPE a[3][PMAX];

//static MTYPE ap[PMAX], bp[PMAX], cp[PMAX];
//static MTYPE kk[PMAX], vv[PMAX];
static MTYPE *ap, *bp, *cp;
static MTYPE *kk, *vv;

int k=NPOINTS;
static MTYPE h[PMAX];
//MTYPE *h;

void printResults();

/* maximal value of function on given interval */
MTYPE f_max(function _f, MTYPE minx, MTYPE maxx) {
	register MTYPE res = 0, x, t;

	for(x = minx; x<maxx; x+=step)
		res = ( (t=_f(x)) > res) ? t : res;
	return res;
}

/* minimal value of function on given interval */
MTYPE f_min(function _f, MTYPE minx, MTYPE maxx) {
	register MTYPE res = 0, x, t;

	for(x = minx; x<maxx; x+=step)
		res = ( (t=_f(x)) < res) ? t : res;
	return res;
}

MTYPE f_height(function _f, MTYPE minx, MTYPE maxx) {
	return f_max(_f, minx, maxx) - f_min(_f, minx, maxx);
}

MTYPE f_error(MTYPE x) {
	return fabs(toPlot(x) - f(x));
}

/* Main spline function.
 * Should be called after spline_init() */
MTYPE toPlot(const MTYPE x) {
	int i;
	int j = k-1;

	for (i=0; i<k; i++)
        	if ((x>=xx[i]) && (x<xx[i+1]))
                	j = i;

	return a[0][j+1]+a[1][j+1]*pow(xx[j+1]-x,1)+a[2][j+1]*pow(xx[j+1]-x,2)+a[3][j+1]*pow(xx[j+1]-x,3);
}

/* Generates x_points */
void genxx() {
	int i;
	for(i=0; i<=k; i++)
		xx[i] = lb + i*(rb-lb)/k;
}

/* Initializes spline function.
 * Fills array "a" with coefficients */
void spline_init() {
	int i;
	MTYPE c1, c2;

//	h = (MTYPE *) malloc( (k+3)*sizeof(MTYPE) );
	
	ap = (MTYPE *) malloc( (k+1)*sizeof(MTYPE) );
	bp = (MTYPE *) malloc( (k+1)*sizeof(MTYPE) );
	cp = (MTYPE *) malloc( (k+1)*sizeof(MTYPE) );
	kk = (MTYPE *) malloc( (k+1)*sizeof(MTYPE) );
	vv = (MTYPE *) malloc( (k+1)*sizeof(MTYPE) );
	ff = (MTYPE *) malloc( (k+1)*sizeof(MTYPE) );


	/* boundary conditions */
	c1 = _c1;
	c2 = _c2;

	genxx();

	for (i=0; i<=k; i++)
		fx[i] = f(lb + i*(rb-lb)/k);

	for (i=1; i<=k; i++)
		h[i] = xx[i]-xx[i-1];
	for (i=1; i<k; i++)
                ff[i] = 3*(((fx[i-1]-fx[i])/h[i])-((fx[i]-fx[i+1])/h[i+1]));	
	
	for (i=1; i<=k; i++)
		a[0][i] = fx[i];

	for (i=1; i<k; i++) {
        	ap[i] = h[i];
        	cp[i] = 2*(h[i]+h[i+1]);
        	bp[i] = h[i+1];
	}


	/* forward */
	kk[0] = 0;
	kk[k] = 0;
	vv[0] = c1/2;

	vv[k] = c2/2;
	for (i=1; i<k; ++i)
	{
		kk[i] = -bp[i]/(cp[i]+ap[i]*kk[i-1]);
		vv[i] = (ff[i]-ap[i]*vv[i-1])/(cp[i]+ap[i]*kk[i-1]);
	}

	/* backwards */
	a[2][k] = c2/2;
	a[2][0] = c1/2;
	for (i=k-1; i>=1; i-=1)
	{
		a[2][i] = a[2][i+1]*kk[i]+vv[i];
	}


	for (i=1; i<=k; ++i)
		a[1][i] = (-h[i]*(a[2][i-1]+2*a[2][i]))/3 + (fx[i-1]-fx[i])/h[i];


	for (i=1; i<=k; ++i)
		a[3][i] = (a[2][i-1]-a[2][i])/(3*h[i]);
	printResults();
}

void printResults() {
	int i;
	printf("Spline interpolation.\n\n");
	printf("%i nodes given:\n     x\t\tf(x)\n", k+1);
	for(i=0; i<=k; i++) {
		printf("%*i) %.3f\t%.3f\n",3,i+1,xx[i],fx[i]);
	}
	printf("Approximate maximal error: %.3f\n\n",f_height(&f_error, lb, rb));
}
