#include "stdlib.h"
#include "stdint.h"
#include "MatLib.h"

typedef struct Ellipse_ {
	double cx;
	double cy;
	double theta;
	double l1;
	double l2;
	Mat* v1;
	int id;
	Mat* pixels;

} Ellipse;

typedef struct EllipseCollideTest_ {
	int maxIterations;
	double *innerPolygonCoef;
	double *outerPolygonCoef;

} EllipseCollideTest;

void showellipse(Ellipse *E);
void MVEE(Ellipse *E);
void joinellipses(Ellipse* ellipses, size_t* length);
void overlap(EllipseCollideTest* test, Ellipse* ellipses, size_t length);

void initEllipseCollideTest(EllipseCollideTest* test, int maxIterations);
bool iterate(EllipseCollideTest* test,double x, double y, double c0x, double c0y, double c2x, double c2y, double rr);
bool collide(EllipseCollideTest* test, double x0, double y0, double wx0, double wy0, double hw0,
	       			 double x1, double y1, double wx1, double wy1, double hw1);
