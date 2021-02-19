#include "stdlib.h"
#include "stdint.h"
#include "math.h"
#include "Ellipse.h"


void MVEE(Mat *P, Ellipse *E){
    //Dimension of points
    double dim = P->row;
    // Number of points
    double N = P->col;
    // Add row of ones
    Mat* Q = addrow(P, 3, ones(1,N));

//    printf("Q\n");
//    showmat(Q);
    //Initialize
    int count = 1;
    double err = 1;
    // u is an Nx1 vector where each element is 1/N
    Mat* u = scalermultiply(ones(N,1), (1/N));
//    printf("u\n");
//    showmat(u);

    
    double tolerance = 1e-1;
//    printf("Tolerance is %f.\n",tolerance);

    //Khachiyan Algorithm
    int i,j;
    double maximum, step_size;


    Mat* T1;
    Mat* T2;
    Mat* T3;


    Mat* Q_transpose = transpose(Q);
//    printf("Q_t\n");
//    showmat(Q_transpose);
    Mat* X;
    Mat* X_inverse;
    Mat* M;
    Mat* m;
    Mat* new_u;

    while((err > tolerance)){
	// Matrix multiplication
	// X = Q * diag(u) *Q'
	T1 = diag(u);
	//printf("diag(u)\n");
	//showmat(T1);
	T2 = multiply(Q,T1);
	//printf("T2\n");
	//showmat(T2);
	X = multiply(T2,Q_transpose);
	//printf("X\n");
	//showmat(X);

	// M = Q' * inv(X) * Q);
	// m = diag(M)
	X_inverse = inverse(X);
	T3 = multiply(Q_transpose,X_inverse);
	M = multiply(T3,Q);
	m = diag(M);

//	printf("m\n");
	//showmat(m);

	//Find max value and its location
	find_max(m, &maximum, &i, &j);
 	
//	printf("Max: %f, row: %d, col:%d\n",maximum,i,j);
	
	//Calculate step size for the ascent
	step_size = (maximum - dim -1)/((dim+1)*(maximum-1));
	
//	printf("Step size: %f\n",step_size);

	//Calculate new u
	new_u = scalermultiply(u, (1-step_size));

//	printf("new_u\n");
//	showmat(new_u);

	// Increment the jth element of new_u by step_size
	set(new_u, i, j, new_u->entries[(i-1)*new_u->col+(j-1)]+step_size); 
	
//	printf("new_u\n");
  //      showmat(new_u);

	//
	
	err = ssdnorm(new_u, u);

//	printf("Error: %f\n",err);

	// Increment count
	count++;
	u =copyvalue(new_u);
	
	//printf("u\n");
        //showmat(u);

    }

    // Put elements of vector u into the diagonal 
    Mat* U = diag(u);

    // Compute the A-Matrix
    // A = (1/d) * inv(P*U*P'-(P*u)*(P*u)';
    Mat* T4 = multiply(P,U);
    Mat* T5 = transpose(P);
    Mat* PUP = multiply(T4,T5);
    
    Mat* T6 = multiply(P,u);
    Mat* T7 = transpose(T6);
    Mat* PuPu = multiply(T6,T7);
    
    Mat* PUP_PuPu = minus(PUP,PuPu);
    Mat* T8 = inverse(PUP_PuPu);
    Mat* A = scalermultiply(T8,(1/dim));
    Mat* center = multiply(P,u);

    //Free memory
    freemat(T1);
    freemat(T2);
    freemat(T3);
    freemat(T4);
    freemat(T5);
    freemat(T6);
    freemat(T7);
    freemat(T8);
   

    freemat(Q_transpose);
    freemat(X);
    freemat(X_inverse);
    freemat(M);
    freemat(m);
    freemat(new_u);
    freemat(u);
    freemat(PUP);
    freemat(PuPu);
    freemat(PUP_PuPu);


//    showmat(center);
//    showmat(A);


    double a = A->entries[0];
    double b = A->entries[1];
    double c = A->entries[2];
    double d = A->entries[3];
    double T = a + d;
    double D = a*d-b*c;
    double l1 = T/2 + sqrt((T*T)/4-D);
    double l2 = T/2 - sqrt((T*T)/4-D);
    double theta;
    if((b==0)&&(c==0)){
	if(a>=d){
            theta = 0;
	}
	else{
	    theta = M_PI/2;
	}
    }
    else{
	theta = atan2(-b,a-l1);
    }
    
    
    E->cx = center->entries[0];
    E->cy = center->entries[1];
    E->theta = theta;
    E->l1 = l1;
    E->l2 = l2;
}
