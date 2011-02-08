/* maximal possible number of points */
#define PMAX	100
#define MTYPE	double
typedef MTYPE (*function)(MTYPE);

/* node x-coordinates */
extern MTYPE xx[PMAX];

/* node y-coordinates */
extern MTYPE fx[PMAX];

/* number of nodes */
extern int k;

/* for measuring maximal and minimal values of functions */
const extern MTYPE step;

/* initial function */
MTYPE f(MTYPE);

/* main spline function */
void spline_init();
/* final spline function. for plotting */
MTYPE toPlot(MTYPE);

/* additional routines */
MTYPE f_max(function f, MTYPE, MTYPE);
MTYPE f_min(function f, MTYPE, MTYPE);
MTYPE f_height(function f, MTYPE, MTYPE);
