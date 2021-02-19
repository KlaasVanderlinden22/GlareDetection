#include "stdlib.h"
#include "stdint.h"
#include "MatLib.h"

typedef struct Ellipse_ {
	double cx;
	double cy;
	double theta;
	double l1;
	double l2;

} Ellipse;

void MVEE(Mat *P, Ellipse *E);

