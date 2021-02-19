// *
// * Adapted from https://www.codeproject.com/Articles/5283245/Matrix-Library-in-C
// *


#pragma once

#include "stdlib.h"
#include "stdbool.h"
#include "stdio.h"


typedef struct {
   int row;
   int col;
   double* entries;
} Mat;

typedef struct MatList MatList;

struct MatList{
        Mat* mat;
        MatList* next;
};

void showmat(Mat* A);
Mat* newmat(int r,int c,double d);
void freemat(Mat* A);
Mat* eye(int n);
Mat* zeros(int r,int c);
Mat* ones(int r,int c);
Mat* randm(int r,int c,double l,double u);
double get(Mat* M,int r,int c);
void set(Mat* M,int r,int c,double d);
Mat* scalermultiply(Mat* M,double c);
Mat* sum(Mat* A,Mat* B);
Mat* minus(Mat* A,Mat* B);
Mat* submat(Mat* A,int r1,int r2,int c1,int c2);
void submat2(Mat* A,Mat* B,int r1,int r2,int c1,int c2);
Mat* multiply(Mat* A,Mat* B);
Mat* removerow(Mat* A,int r);
Mat* removecol(Mat* A,int c);
void removerow2(Mat* A,Mat* B,int r);
void removecol2(Mat* A,Mat* B,int c);
Mat* transpose(Mat* A);
double det(Mat* M);
double trace(Mat* A);
Mat* adjoint(Mat* A);
Mat* inverse(Mat* A);
Mat* copyvalue(Mat* A);
Mat* triinverse(Mat* A);
Mat* rowechelon(Mat* A);
Mat* hconcat(Mat* A,Mat* B);
Mat* vconcat(Mat* A,Mat* B);
double norm(Mat *A);
Mat* null(Mat *A);
MatList* lu(Mat* A);
double innermultiply(Mat* a,Mat* b);
MatList* qr(Mat* A);
Mat* diag(Mat* A);
Mat* addrow(Mat* A,int r, Mat* newrow);
Mat* appendmat(Mat* A, Mat* B);
void find_max(Mat* A, double *maximum, int* max_location_x, int* max_location_y);
double ssdnorm(Mat* A, Mat *B);


