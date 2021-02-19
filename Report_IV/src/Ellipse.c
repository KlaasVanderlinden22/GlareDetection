#include "stdlib.h"
#include "stdint.h"
#include "string.h"
#include "math.h"
#include "Ellipse.h"

void showellipse(Ellipse *E){
	printf("Ellipse %d found with\n",E->id); 
	printf("center at (%f,%f),\n",E->cx,E->cy); 
	printf("rotated by %f°,\n", E->theta);
	printf("and radii r1:%f, r2:%f\n", sqrt(1/E->l1), sqrt(1/E->l2));
	//printf("v1:\n");
	//showmat(E->v1);
	printf("\n");
}

void MVEE(Ellipse *E){
    Mat* P = E->pixels;
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

    
    double tolerance = 5e-2;
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

	//printf("m\n");
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

	
	err = ssdnorm(new_u, u);

	//printf("Error: %f\n",err);

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



    //showmat(center);
    //printf("A:\n");
    //showmat(A);


    double a = A->entries[0];
    double b = A->entries[1];
    double c = A->entries[2];
    double d = A->entries[3];
    double T = a + d;
    double D = a*d-b*c;
    double l1 = T/2 + sqrt((T*T)/4-D);
    double l2 = T/2 - sqrt((T*T)/4-D);
    
    Mat* v1 = newmat(2,1,0);
    set(v1,1,1,(l1-d)/c);
    set(v1,2,1,1);
    //showmat(v1);
    
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
	//theta = atan2(c,l1-d); in normal coordinates but y is flipped for images i.e. you start top left, not bottom left 
	theta = atan2((l1-d),c)*180/M_PI;
    }
    
    E->cx = center->entries[0];
    E->cy = center->entries[1];
    E->theta = theta;
    E->l1 = l1;
    E->l2 = l2;
    E->v1 = scalermultiply(v1,1/norm(v1));
    
    
    freemat(A);
    freemat(center);
}


void joinellipses(Ellipse* ellipses, size_t* ptrlength){
	int length = *ptrlength;
	// Minimum 2 ellipses for joining
	if (length > 1){
		
		// Array for new ellipses
		int max_nb_combos = floor(length/2);
		Ellipse new_ellipses[max_nb_combos];
		int counter_ne = 0;
		
		// Array of indices of old ellipses that need to be removed
		int remove_indices[length];
		int counter_ri = 0;
		
		//Flag
		int ellipsesjoined = 0;
		
		// Check every pair of ellipses
		for (int k = 0; k < length - 1; k++){
			for (int l = k+1; l < length; l++){
				Ellipse E1 = ellipses[k];
				Ellipse E2 = ellipses[l];
				// Conditions for belonging to same object
				if (round(E1.theta) == round(E2.theta)){
					double rico = round(atan2((E2.cy-E1.cy),-(E2.cx-E1.cx))*180/M_PI);
					if (rico == round(E1.theta)){
					
						// New ellipse based on combined points
						Mat* pixels12 = appendmat(E1.pixels, E2.pixels);
						
						Ellipse E12; 
						E12.id = E1.id*100+E2.id;
						E12.pixels = pixels12;
						MVEE(&E12);
						
						printf("Joined Ellipse %d and %d \n",E1.id,E2.id);
						//printf("with theta1:%f, theta2:%f,\n",E1.theta, E2.theta); 
						//printf("and rico:%f \n", rico);
						//showellipse(&E12);
						
						// Add ellipse to new ellipses
						new_ellipses[counter_ne] = E12;
						counter_ne++;
						
						// Add indices to remove
						remove_indices[counter_ri] = k;
						counter_ri++;
						remove_indices[counter_ri] = l;
						counter_ri++;
						
						// Set flag
						ellipsesjoined = 1;
					}	
				}	
			}
		}
		
		//Add new ellipse and remove old ones
		if (counter_ri > 0){
			for(int i = 0; i<counter_ri; i++){
				int pos = remove_indices[i];
				// Replace if possible
				if (counter_ne > 0){
					ellipses[pos] = new_ellipses[counter_ne-1];	
					counter_ne--;
				}
				// Remove 
				else{
					for(int j = pos; j<length-1; j++){
						ellipses[j] = ellipses[j+1];
					}
					length--;
				}
				
								
			}
		}
		
		if (ellipsesjoined == 0){
			printf("No ellipses joined.");
		}
		
		*ptrlength = length;
		
	}
	else{
		printf("Too few ellipses.\n");
	}
}

void overlap(EllipseCollideTest* test, Ellipse* ellipses, size_t length){
	initEllipseCollideTest(test, 10);
	
	if (length > 1){
		for (int k = 0; k < length - 1; k++){
			for (int l = k+1; l < length; l++){
				Ellipse E1 = ellipses[k];
				Ellipse E2 = ellipses[l];
				Mat* radius_vector1 = scalermultiply(E1.v1,sqrt(1/E1.l1));
				double wx1 = radius_vector1->entries[0];
				double wy1 = radius_vector1->entries[1];
				double hw1 = sqrt(1/E1.l2)/sqrt(1/E1.l1);
				Mat* radius_vector2 = scalermultiply(E2.v1,sqrt(1/E2.l1));
				double wx2 = radius_vector2->entries[0];
				double wy2 = radius_vector2->entries[1];
				double hw2 = sqrt(1/E2.l2)/sqrt(1/E2.l1);
				
				//printf("x0 = %f\n",E1.cx);
				//printf("y0 = %f\n",E1.cy);
				//printf("x1 = %f\n",E2.cx);
				//printf("y1 = %f\n",E2.cy);
				//printf("wx0 = %f\n",wx1);
				//printf("wy0 = %f\n",wy1);				
				//printf("wx1 = %f\n",wx2);
				//printf("wy1 = %f\n",wy2);				
				//printf("hw0 = %f\n",hw1);
				//printf("hw1 = %f\n",hw2);
				
				if (collide(test,E1.cx, E1.cy, wx1, wy1, hw1, E2.cx, E2.cx, wx2, wy2, hw2)) {
					printf("Collision between Ellipse %d and %d.\n", E1.id,E2.id);
				} else {
					printf("No Collision between Ellipse %d and %d.\n", E1.id,E2.id);
				}
			}
		}
	}
	else{
		printf("Too few ellipses.\n");
	}
}


/* Based on
 * Ellipse-circle collision detection
 * by Olli Niemitalo in 2012-08-06.
 */

void initEllipseCollideTest(EllipseCollideTest* test, int maxIterations){
	double innerPolygonCoef[maxIterations+1];
	double outerPolygonCoef[maxIterations+1];
	for (int t = 0; t <= maxIterations; t++) {
		int numNodes = 4 << t;
		innerPolygonCoef[t] = 0.5/cos(4*acos(0.0)/numNodes);
		outerPolygonCoef[t] = 0.5/(cos(2*acos(0.0)/numNodes)*cos(2*acos(0.0)/numNodes));
	}
	test->innerPolygonCoef = innerPolygonCoef;
	test->outerPolygonCoef = outerPolygonCoef;
	test->maxIterations = maxIterations;
}

bool iterate(EllipseCollideTest* test,double x, double y, double c0x, double c0y, double c2x, double c2y, double rr){
	int maxIterations = test->maxIterations;
	double *innerPolygonCoef = test->innerPolygonCoef;
	double *outerPolygonCoef = test->outerPolygonCoef;
	// Inside or meets triangle c0--?--c2
	for (int t = 1; t <= maxIterations; t++) {
		double c1x = (c0x + c2x)*innerPolygonCoef[t];
		double c1y = (c0y + c2y)*innerPolygonCoef[t];
		// Collision with triangles c3--c1--c2 and c4--c1--c2 is possible
		double tx = x - c1x; // t indicates a translated coordinate
		double ty = y - c1y;
		if (tx*tx + ty*ty <= rr) {
			// Collision with t1
			return true;
		}
		double t2x = c2x - c1x;
		double t2y = c2y - c1y;
		if (tx*t2x + ty*t2y >= 0 && tx*t2x + ty*t2y <= t2x*t2x + t2y*t2y &&
		  (ty*t2x - tx*t2y >= 0 || rr*(t2x*t2x + t2y*t2y) >= (ty*t2x - tx*t2y)*(ty*t2x - tx*t2y))) {
		  	// Collision with t1--t2
			return true;
		}
		double t0x = c0x - c1x;
		double t0y = c0y - c1y;
		if (tx*t0x + ty*t0y >= 0 && tx*t0x + ty*t0y <= t0x*t0x + t0y*t0y &&
		  (ty*t0x - tx*t0y <= 0 || rr*(t0x*t0x + t0y*t0y) >= (ty*t0x - tx*t0y)*(ty*t0x - tx*t0y))) {
		  	// Collision with t1--t0
			return true;
		}    
		double c3x = (c0x + c1x)*outerPolygonCoef[t];
		double c3y = (c0y + c1y)*outerPolygonCoef[t];
		if ((c3x-x)*(c3x-x) + (c3y-y)*(c3y-y) < rr) {
			// t3 is inside circle		
			c2x = c1x;
			c2y = c1y;
			continue;
		}
		double c4x = c1x - c3x + c1x;
		double c4y = c1y - c3y + c1y;
		if ((c4x-x)*(c4x-x) + (c4y-y)*(c4y-y) < rr) {
			// t4 is inside circle	
			c0x = c1x;
			c0y = c1y;
			continue;
		}
		double t3x = c3x - c1x;
		double t3y = c3y - c1y;
		// t2______t4
		//   --__   \                  
		//       --__\                /¨¨¨\
		//           t1 = (0, 0)     (  t  )
		//            |\              \___/
		//            | \               
		//            |  t3             
		//            | / 
		//            |/
		//           t0
		if (ty*t3x - tx*t3y <= 0 || rr*(t3x*t3x + t3y*t3y) > (ty*t3x - tx*t3y)*(ty*t3x - tx*t3y)) {
			if (tx*t3x + ty*t3y > 0) {
				if (fabs(tx*t3x + ty*t3y) <= t3x*t3x + t3y*t3y || (x-c3x)*(c0x-c3x) + (y-c3y)*(c0y-c3y) >= 0) {
					//Circle center is inside t0--t1--t3					
					c2x = c1x;
					c2y = c1y;
					continue;
				}
			} else if (-(tx*t3x + ty*t3y) <= t3x*t3x + t3y*t3y || (x-c4x)*(c2x-c4x) + (y-c4y)*(c2y-c4y) >= 0) {
				//Circle center is inside t1--t2--t4					
				c0x = c1x;
				c0y = c1y;
				continue;
			}	
		}
		// No collision possible
		return false;
	}
	return false; // Out of iterations so it is unsure if there was a collision. But have to return something.
}

// Test for collision between two ellipses, "0" and "1". Ellipse is at (x, y) with major or minor radius 
// vector (wx, wy) and the other major or minor radius perpendicular to that vector and hw times as long.
bool collide(EllipseCollideTest* test, double x0, double y0, double wx0, double wy0, double hw0,
	       double x1, double y1, double wx1, double wy1, double hw1){
	float rr = hw1*hw1*(wx1*wx1 + wy1*wy1)*(wx1*wx1 + wy1*wy1)*(wx1*wx1 + wy1*wy1);
	float x = hw1*wx1*(wy1*(y1 - y0) + wx1*(x1 - x0)) - wy1*(wx1*(y1 - y0) - wy1*(x1 - x0));
	float y = hw1*wy1*(wy1*(y1 - y0) + wx1*(x1 - x0)) + wx1*(wx1*(y1 - y0) - wy1*(x1 - x0));
	float temp = wx0;
	wx0 = hw1*wx1*(wy1*wy0 + wx1*wx0) - wy1*(wx1*wy0 - wy1*wx0);
	float temp2 = wy0;
	wy0 = hw1*wy1*(wy1*wy0 + wx1*temp) + wx1*(wx1*wy0 - wy1*temp);
	float hx0 = hw1*wx1*(wy1*(temp*hw0)-wx1*temp2*hw0)-wy1*(wx1*(temp*hw0)+wy1*temp2*hw0);
	float hy0 = hw1*wy1*(wy1*(temp*hw0)-wx1*temp2*hw0)+wx1*(wx1*(temp*hw0)+wy1*temp2*hw0);

	if (wx0*y - wy0*x < 0) {
		// Flip to upper half
		x = -x;
		y = -y;
	}
	
	//           ___-(hx0, hy0)--__              _----_
	//        _--                  \            /      \ 
	//       /                   (wx0, wy0)    | (x, y) |
	//      /        (0, 0)         /           \_    _/
	// (-wx0, -wy0)               _/              ---- radius = sqrt(rr)
	//       \__             ___--                              
	//          (-hx0, -hy0)-

		
	if ((wx0 - x)*(wx0 - x) + (wy0 - y)*(wy0 - y) <= rr) {
		//Collision with right point
		return true;
	} else if ((wx0 + x)*(wx0 + x) + (wy0 + y)*(wy0 + y) <= rr) {
		//Collision with left point
		return true;
	} else if ((hx0 - x)*(hx0 - x) + (hy0 - y)*(hy0 - y) <= rr) {
		//Collision with top point
		return true;
	} else if ((hx0 + x)*(hx0 + x) + (hy0 + y)*(hy0 + y) <= rr) {
		//Collision with bottom point
		return true;
	} else if (x*(hy0 - wy0) + y*(wx0 - hx0) <= hy0*wx0 - hx0*wy0 &&
	       y*(wx0 + hx0) - x*(wy0 + hy0) <= hy0*wx0 - hx0*wy0) {
		//Inside inscribed rectangle
		return true;
	} else if (x*(wx0-hx0) - y*(hy0-wy0) > hx0*(wx0-hx0) - hy0*(hy0-wy0)     
	       && x*(wx0-hx0) - y*(hy0-wy0) < wx0*(wx0-hx0) - wy0*(hy0-wy0)
	       && (x*(hy0-wy0) + y*(wx0-hx0) - hy0*wx0 + hx0*wy0)*(x*(hy0-wy0) + y*(wx0-hx0) - hy0*wx0 + hx0*wy0)
	       <= rr*((wx0-hx0)*(wx0-hx0) + (wy0-hy0)*(wy0-hy0))) {
	       //Collision with h0 -- w0
		return true;
	} else if (x*(wx0+hx0) + y*(wy0+hy0) > -wx0*(wx0+hx0) - wy0*(wy0+hy0)
	       && x*(wx0+hx0) + y*(wy0+hy0) < hx0*(wx0+hx0) + hy0*(wy0+hy0)
	       && (y*(wx0+hx0) - x*(wy0+hy0) - hy0*wx0 + hx0*wy0)*(y*(wx0+hx0) - x*(wy0+hy0) - hy0*wx0 + hx0*wy0)
	       <= rr*((wx0+hx0)*(wx0+hx0) + (wy0+hy0)*(wy0+hy0))) {
	       //Collision with h0 -- -w0
		return true;
	} else {
		if ((hx0-wx0 - x)*(hx0-wx0 - x) + (hy0-wy0 - y)*(hy0-wy0 - y) <= rr) {
			// Meets top left triangle
			return iterate(test,x, y, hx0, hy0, -wx0, -wy0, rr);
		} else if ((hx0+wx0 - x)*(hx0+wx0 - x) + (hy0+wy0 - y)*(hy0+wy0 - y) <= rr) {
			// Meets top right triangle
			return iterate(test,x, y, wx0, wy0, hx0, hy0, rr);
		} else if ((wx0-hx0 - x)*(wx0-hx0 - x) + (wy0-hy0 - y)*(wy0-hy0 - y) <= rr) {
			// Meets bottom right triangle
			return iterate(test,x, y, -hx0, -hy0, wx0, wy0, rr);
		} else if ((-wx0-hx0 - x)*(-wx0-hx0 - x) + (-wy0-hy0 - y)*(-wy0-hy0 - y) <= rr) {
			// Meets bottom left triangle
			return iterate(test,x, y, -wx0, -wy0, -hx0, -hy0, rr);
		} else if (wx0*y - wy0*x < wx0*hy0 - wy0*hx0 && fabs(hx0*y - hy0*x) < hy0*wx0 - hx0*wy0) {
			// Inside bounding box
			if (hx0*y - hy0*x > 0) {
				// Inside top left triangle
				return iterate(test,x, y, hx0, hy0, -wx0, -wy0, rr);
			}
			else{
				// Meets top right triangle		
				return iterate(test,x, y, wx0, wy0, hx0, hy0, rr);
			}
		} else if (wx0*x + wy0*y > wx0*(hx0-wx0) + wy0*(hy0-wy0) && wx0*x + wy0*y < wx0*(hx0+wx0) + wy0*(hy0+wy0)
			 && (wx0*y - wy0*x - hy0*wx0 + hx0*wy0)*(wx0*y - wy0*x - hy0*wx0 + hx0*wy0) < rr*(wx0*wx0 + wy0*wy0)) {
			// Reaching across top line
			if (wx0*x + wy0*y > wx0*hx0 + wy0*hy0) {
				// Meets top right triangle
				return iterate(test,x, y, wx0, wy0, hx0, hy0, rr);
			}
			else{
				// Meets top left triangle
				return iterate(test,x, y, hx0, hy0, -wx0, -wy0, rr);
			}
		} else {
			if (hx0*y - hy0*x < 0) {
				// Flip to left half
				x = -x;
				y = -y;
			}  
			if (hx0*x + hy0*y > -hx0*(wx0+hx0) - hy0*(wy0+hy0) && hx0*x + hy0*y < hx0*(hx0-wx0) + hy0*(hy0-wy0)
			    && (hx0*y - hy0*x - hy0*wx0 + hx0*wy0)*(hx0*y - hy0*x - hy0*wx0 + hx0*wy0) < rr*(hx0*hx0 + hy0*hy0)) {
				if (hx0*x + hy0*y > -hx0*wx0 - hy0*wy0) {      
					// Meets top left triangle
					return iterate(test,x, y, hx0, hy0, -wx0, -wy0, rr);
				} 
				else{
					// Meets bottom left triangle
					return iterate(test,x, y, -wx0, -wy0, -hx0, -hy0, rr);
				}
			}
			return false;
		}
	}
}



