#ifndef _INTERPOLATION_H_
#define _INTERPOLATION_H_
#include "spline.h"

namespace spline {
		struct pair {
			double x,y;
		};

	double pow(const double &x, const int &p) {
		return p==0 ? 1: x*pow(x,p-1);
	}

	double spline3(double x, std::vector<pair> nodes, pair d2f) {
		int k=nodes.size()-1;
		double min=nodes[0].x;
		double max=nodes[k-1].x;
		int i;
		double c1, c2;

		c1 = d2f.x;
		c2 = d2f.y;

		std::vector<double> h;
		for (i=1; i<=k; ++i)
			h[i] = nodes[i]-xx[i-1];

		std::vector<double> ff;
		for (i=1; i<k; ++i)
			ff[i] = 3*(((fx[i-1]-fx[i])/h[i])-((fx[i]-fx[i+1])/h[i+1]));

		double a[3][100];
		for (i=1; i<=k; ++i) {a[0][i] = fx[i];}

		double ap[100], cp[100], bp[100];
		for (i=1; i<k; ++i) {
			ap[i] = h[i];
			cp[i] = 2*(h[i]+h[i+1]);
			bp[i] = h[i+1];
		}

		double kk[100], vv[100];
		kk[0] = 0;
		kk[k] = 0;
		vv[0] = c1/2;

		vv[k] = c2/2;
		for (i=1; i<k; ++i)
		{
			kk[i] = -bp[i]/(cp[i]+ap[i]*kk[i-1]);
			vv[i] = (ff[i]-ap[i]*vv[i-1])/(cp[i]+ap[i]*kk[i-1]);
		}

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

		int j = k-1;

		for (i=0; i<k; ++i)
			if ((x>=xx[i]) && (x<xx[i+1]))
				j = i;

		return a[0][j+1]+a[1][j+1]*pow(xx[j+1]-x,1)+a[2][j+1]*pow(xx[j+1]-x,2)+a[3][j+1]*pow(xx[j+1]-x,3);
	}

	std::vector<pair> gennodes(double (*f)(double), int nnodes, pair bound) {
		std::vector<pair> r;
		double h= (bound.y-bound.x)/nnodes;
		for(int i=0; i<nnodes; i++) {
			r[i].x=bound.x+i*h;
			r[i].y=f(r[i].x);
		}
		return r;
	}

	
}
#endif
