#include "stdlib.h"
#include "stdbool.h"
#include "stdio.h"
#include "math.h"
#include "MatLib.h"


void showmat(Mat* A){
	if(A->row>0&&A->col>0){
		int k=0;
		printf("[");
		for(int i=1;i<=A->row;i++){
			for (int j=1;j<=A->col;j++){
				if(j<A->col){
					printf("%f\t",A->entries[k++]);
				}else{
					printf("%f",A->entries[k++]);
				}
			}
			if(i<A->row){
				printf("\n");
			}else{
				printf("]\n");
			}
		}	
		printf("\n");
	}else{
		printf("[]");
	}
}


Mat* newmat(int r,int c,double d){
	Mat* M=(Mat*)malloc(sizeof(Mat));			
	M->row=r;M->col=c;
	M->entries=(double*)malloc(sizeof(double)*r*c);
	int k=0;
	for(int i=1;i<=M->row;i++){
		for(int j=1;j<=M->col;j++){
			M->entries[k++]=d;
		}
	}
	return M;
}

void freemat(Mat* A){
	free(A->entries);
	free(A);
}

Mat* eye(int n){
	Mat* I=newmat(n,n,0);
	for(int i=1;i<=n;i++){
		I->entries[(i-1)*n+i-1]=1;
	}
	return I;
}
Mat* zeros(int r,int c){
	Mat* Z=newmat(r,c,0);	
	return Z;
}
Mat* ones(int r,int c){
	Mat* O=newmat(r,c,1);	
	return O;
}
Mat* randm(int r,int c,double l,double u){
	Mat* R=newmat(r,c,1);	
	int k=0;
	for(int i=1;i<=r;i++){
		for(int j=1;j<=c;j++){
			double r=((double)rand())/((double)RAND_MAX);
			R->entries[k++]=l+(u-l)*r;
		}
	}
	return R;
}
double get(Mat* M,int r,int c){
	double d=M->entries[(r-1)*M->col+c-1];
	return d;
}
void set(Mat* M,int r,int c,double d){
	M->entries[(r-1)*M->col+c-1]=d;
}

Mat* scalermultiply(Mat* M,double c){	
	Mat* B=newmat(M->row,M->col,0);
	int k=0;
	for(int i=0;i<M->row;i++){
		for(int j=0;j<M->col;j++){
			B->entries[k]=M->entries[k]*c;
			k+=1;
		}
	}
	return B;
}
Mat* sum(Mat* A,Mat* B){
	int r=A->row;
	int c=A->col;
	Mat* C=newmat(r,c,0);
	int k=0;	
	for(int i=0;i<r;i++){
		for(int j=0;j<c;j++){
			C->entries[k]=A->entries[k]+B->entries[k];
			k+=1;
		}
	}
	return C;
}
Mat* minus(Mat* A,Mat* B){
	int r=A->row;
	int c=A->col;
	Mat* C=newmat(r,c,0);
	int k=0;	
	for(int i=0;i<r;i++){
		for(int j=0;j<c;j++){
			C->entries[k]=A->entries[k]-B->entries[k];
			k+=1;
		}
	}
	return C;
}
Mat* submat(Mat* A,int r1,int r2,int c1,int c2){
	Mat* B=newmat(r2-r1+1,c2-c1+1,0);
	int k=0;
	for(int i=r1;i<=r2;i++){
		for(int j=c1;j<=c2;j++){
			B->entries[k++]=A->entries[(i-1)*A->col+j-1];
		}
	}
	return B;
}
void submat2(Mat* A,Mat* B,int r1,int r2,int c1,int c2){	
	int k=0;
	for(int i=r1;i<=r2;i++){
		for(int j=c1;j<=c2;j++){
			B->entries[k++]=A->entries[(i-1)*A->col+j-1];
		}
	}	
}

Mat* multiply(Mat* A,Mat* B){
	int r1=A->row;
	int r2=B->row;
	int c1=A->col;
	int c2=B->col;
	if (r1==1&&c1==1){
		Mat* C=scalermultiply(B,A->entries[0]);
		return C;
	}else if (r2==1&&c2==1){
		Mat* C=scalermultiply(A,B->entries[0]);
		return C;
	}
	Mat* C=newmat(r1,c2,0);
	for(int i=1;i<=r1;i++){
		for(int j=1;j<=c2;j++){
			double de=0;
			for(int k=1;k<=r2;k++){
				de+=A->entries[(i-1)*A->col+k-1]*B->entries[(k-1)*B->col+j-1];
			}
			C->entries[(i-1)*C->col+j-1]=de;
		}
	}
	return C;
}



Mat* removerow(Mat* A,int r){
	Mat* B=newmat(A->row-1,A->col,0);
	int k=0;
	for(int i=1;i<=A->row;i++){
		for(int j=1;j<=A->col;j++){
			if(i!=r){
				B->entries[k]=A->entries[(i-1)*A->col+j-1];
				k+=1;
			}
		}
	}
	return B;
}
Mat* removecol(Mat* A,int c){
	Mat* B=newmat(A->row,A->col-1,0);
	int k=0;
	for(int i=1;i<=A->row;i++){
		for(int j=1;j<=A->col;j++){
			if(j!=c){
				B->entries[k]=A->entries[(i-1)*A->col+j-1];
				k+=1;
			}
		}
	}
	return B;
}
void removerow2(Mat* A,Mat* B,int r){	
	int k=0;
	for(int i=1;i<=A->row;i++){
		for(int j=1;j<=A->col;j++){
			if(i!=r){
				B->entries[k++]=A->entries[(i-1)*A->col+j-1];				
			}
		}
	}	
}
void removecol2(Mat* A,Mat* B,int c){	
	int k=0;
	for(int i=1;i<=A->row;i++){
		for(int j=1;j<=A->col;j++){
			if(j!=c){
				B->entries[k++]=A->entries[(i-1)*A->col+j-1];				
			}
		}
	}	
}
Mat* transpose(Mat* A){
	Mat* B=newmat(A->col,A->row,0);
	int k=0;
	for(int i=1;i<=A->col;i++){
		for(int j=1;j<=A->row;j++){
			B->entries[k]=A->entries[(j-1)*A->col+i-1];
			k+=1;
		}
	}
	return B;
}
double det(Mat* M){
	int r=M->row;
	int c=M->col;
	if(r==1&&c==1){
		double d=M->entries[0];
		return d;
	}
	Mat* M1=removerow(M,1);
	Mat* M2=newmat(M->row-1,M->col-1,0);
	double d=0, si=+1;
	for(int j=1;j<=M->col;j++){
		double c=M->entries[j-1];		
		removecol2(M1,M2,j);				
		d+=si*det(M2)*c;
		si*=-1;
	}
	freemat(M1);
	freemat(M2);
	return d;
}

double trace(Mat* A){
	double d=0;
	for(int i=1;i<=A->row;i++){
		d+=A->entries[(i-1)*A->row+i-1];
	}
	return d;
}


Mat* adjoint(Mat* A){
	Mat* B=newmat(A->row,A->col,0);	
	Mat* A1=newmat(A->row-1,A->col,0);
	Mat* A2=newmat(A->row-1,A->col-1,0);
	for(int i=1;i<=A->row;i++){
		removerow2(A,A1,i);
		for(int j=1;j<=A->col;j++){			
			removecol2(A1,A2,j);
			double si=pow(-1,(double)(i+j));
			B->entries[(i-1)*B->col+j-1]=det(A2)*si;			
		}		
	}
	Mat* C=transpose(B);
	freemat(A1);
	freemat(A2);
	freemat(B);
	return C;
}

Mat* inverse(Mat* A){
	Mat* B=adjoint(A);
	double de=det(A);
	Mat* C=scalermultiply(B,1/de);
	freemat(B);
	return C;
}

Mat* copyvalue(Mat* A){
	Mat* B=newmat(A->row,A->col,0);
	int k=0;
	for(int i=1;i<=A->row;i++){
		for(int j=1;j<=A->col;j++){
			B->entries[k]=A->entries[k];
			k++;
		}
	}
	return B;
}
Mat* triinverse(Mat* A){
	Mat* B=newmat(A->row,A->col,0);
	for(int i=1;i<=B->row;i++){
		for(int j=i;j<=B->col;j++){
			if(i==j){
				B->entries[(i-1)*B->col+j-1]=1/A->entries[(i-1)*A->col+j-1];
			}else{
				B->entries[(i-1)*B->col+j-1]=-A->entries[(i-1)*A->col+j-1]/A->entries[(j-1)*A->col+j-1];
			}
		}
	}
	return B;
}
Mat* rowechelon(Mat* A){	
	if(A->row==1){
		for(int j=1;j<=A->col;j++){
			if(A->entries[j-1]!=0){
				Mat* B=scalermultiply(A,1/A->entries[j-1]);
				return B;
			}
		}
		Mat* B=newmat(1,A->col,0);
		return B;
	}	
	Mat* B=copyvalue(A);
	int ind1=B->col+1;
	int ind2=1;
	for(int i=1;i<=B->row;i++){		
		for(int j=1;j<=B->col;j++){
			if(B->entries[(i-1)*B->col+j-1]!=0&&j<ind1){
				ind1=j;
				ind2=i;				
				break;
			}
		}		
	}
	if(ind2>1){
		for(int j=1;j<=B->col;j++){
			double temp=B->entries[j-1];
			B->entries[j-1]=B->entries[(ind2-1)*B->col+j-1];
			B->entries[(ind2-1)*B->col+j-1]=temp;
		}
	}	
	if(B->entries[0]!=0){	
		double coeff=B->entries[0];
		for(int j=1;j<=B->col;j++){
			B->entries[j-1]/=coeff;
		}
		for(int i=2;i<=B->row;i++){	
			coeff=B->entries[(i-1)*B->col];
			for(int j=1;j<=B->col;j++){
				B->entries[(i-1)*B->col+j-1]-=coeff*B->entries[j-1];
			}				
		}
	}else{
		double coeff=0;
		for(int j=1;j<=B->col;j++){
			if(B->entries[j-1]!=0&&coeff==0){
				coeff=B->entries[j-1];
				B->entries[j-1]=1;
			}else if (B->entries[j-1]!=0){
				B->entries[j-1]/=coeff;
			}
		}
	}	
	Mat* B1=removerow(B,1);
	Mat* B2=removecol(B1,1);	
	Mat* Be=rowechelon(B2);
	for(int i=1;i<=Be->row;i++){
		for(int j=1;j<=Be->col;j++){
			B->entries[i*B->col+j]=Be->entries[(i-1)*Be->col+j-1];
		}
	}
	freemat(B1);
	freemat(B2);	
	freemat(Be);
	return B;
}
Mat* hconcat(Mat* A,Mat* B){
	Mat* C=newmat(A->row,A->col+B->col,0);
	int k=0;
	for(int i=1;i<=A->row;i++){
		for(int j=1;j<=A->col;j++){
			C->entries[k]=A->entries[(i-1)*A->col+j-1];
			k++;
		}
		for(int j=1;j<=B->col;j++){
			C->entries[k]=B->entries[(i-1)*B->col+j-1];
			k++;
		}
	}
	return C;
}
Mat* vconcat(Mat* A,Mat* B){
	Mat* C=newmat(A->row+B->row,A->col,0);
	int k=0;
	for(int i=1;i<=A->row;i++){
		for(int j=1;j<=A->col;j++){
			C->entries[k]=A->entries[(i-1)*A->col+j-1];
			k++;
		}		
	}
	for(int i=1;i<=B->row;i++){
		for(int j=1;j<=B->col;j++){
			C->entries[k]=B->entries[(i-1)*B->col+j-1];
			k++;
		}		
	}
	return C;
}
double norm(Mat* A){
	double d=0;
	int k=0;
	for(int i=1;i<=A->row;i++){
		for(int j=1;j<=A->col;j++){
			d+=A->entries[k]*A->entries[k];
			k++;
		}		
	}
	d=sqrt(d);
	return d;
}
Mat* null(Mat *A){
	Mat* RM=rowechelon(A);
	int k=RM->row;
	for(int i=RM->row;i>=1;i--){
		bool flag=false;
		for(int j=1;j<=RM->col;j++){
			if(RM->entries[(i-1)*RM->col+j-1]!=0){
				flag=true;
				break;
			}
		}		
		if(flag){
			k=i;
			break;
		}
	}
	Mat* RRM=submat(RM,1,k,1,RM->col);
	freemat(RM);	
	int nn=RRM->col-RRM->row;
	if(nn==0){
		Mat* N=newmat(0,0,0);
		return N;
	}
	Mat* R1=submat(RRM,1,RRM->row,1,RRM->row);
	Mat* R2=submat(RRM,1,RRM->row,1+RRM->row,RRM->col);				
	freemat(RRM);
	Mat* I=eye(nn);
	Mat* T1=multiply(R2,I);	
	freemat(R2);
	Mat* R3=scalermultiply(T1,-1);
	freemat(T1);
	Mat* T2=triinverse(R1);
	freemat(R1);
	Mat* X=multiply(T2,R3);	
	freemat(T2);
	freemat(R3);	
	Mat* N=vconcat(X,I);
	freemat(I);
	freemat(X);
	for(int j=1;j<=N->col;j++){
		double de=0;
		for(int i=1;i<=N->row;i++){
			de+=N->entries[(i-1)*N->col+j-1]*N->entries[(i-1)*N->col+j-1];
		}
		de=sqrt(de);
		for(int i=1;i<=N->row;i++){
			N->entries[(i-1)*N->col+j-1]/=de;
		}

	}	
	return N;
}

MatList* lu(Mat* A){
	if(A->row==1){
		MatList* ml=(MatList*)malloc(sizeof(MatList));
		ml->mat=newmat(1,1,A->entries[0]);	
		ml->next=(MatList*)malloc(sizeof(MatList));
		ml->next->mat=newmat(1,1,1);
		return ml;
	}
	double a=A->entries[0];
	double c=0;
	if(a!=0){
		c=1/a;
	}
	Mat* w=submat(A,1,1,2,A->col);
	Mat* v=submat(A,2,A->row,1,1);
	Mat* Ab=submat(A,2,A->row,2,A->col);	
	Mat* T1=multiply(v,w);
	Mat* T2=scalermultiply(T1,-c);
	Mat* T3=sum(Ab,T2);
	MatList* mlb=lu(T3);
	freemat(T1);
	freemat(T2);
	freemat(T3);
	freemat(Ab);
	Mat* L=newmat(A->row,A->col,0);
	Mat* U=newmat(A->row,A->col,0);
	int k=0;
	for(int i=1;i<=A->row;i++){
		for(int j=1;j<=A->col;j++){
			if(i==1&&j==1){
				L->entries[k]=1;
				U->entries[k]=a;
				k++;
			}else if(i==1&&j>1){
				U->entries[k]=w->entries[j-2];
				k++;
			}else if(i>1&&j==1){
				L->entries[k]=c*v->entries[i-2];
				k++;
			}else{
				L->entries[k]=mlb->mat->entries[(i-2)*mlb->mat->col+j-2];
				U->entries[k]=mlb->next->mat->entries[(i-2)*mlb->next->mat->col+j-2];
				k++;
			}
		}
	}
	MatList* ml=(MatList*)malloc(sizeof(MatList));
	ml->mat=L;
	ml->next=(MatList*)malloc(sizeof(MatList));;
	ml->next->mat=U;
	freemat(w);
	freemat(v);	
	free(mlb);
	return ml;
}

double innermultiply(Mat* a,Mat* b){
	double d=0;
	int n=a->row;
	if(a->col>n){
		n=a->col;
	}
	for(int i=1;i<=n;i++){
		d+=a->entries[i-1]*b->entries[i-1];
	}
	return d;
}

MatList* qr(Mat* A){
	int r=A->row;
	int c=A->col;
	Mat* Q=newmat(r,r,0);
	Mat* R=newmat(r,c,0);	
	Mat* ek=newmat(r,1,0);
	Mat* uj=newmat(r,1,0);
	Mat* aj=newmat(r,1,0);
	for(int j=1;j<=r;j++){
		submat2(A,aj,1,r,j,j);
		for(int k=1;k<=r;k++){
			uj->entries[k-1]=aj->entries[k-1];
		}		
		for(int k=1;k<=j-1;k++){	
			submat2(Q,ek,1,r,k,k);
			double proj=innermultiply(aj,ek);			
			for(int l=1;l<=ek->row;l++){
				ek->entries[l-1]*=proj;

			}			
			uj=minus(uj,ek);
		}		
		double nuj=norm(uj);		
		for(int i=1;i<=r;i++){
			Q->entries[(i-1)*r+j-1]=uj->entries[i-1]/nuj;
		}
		for(int j1=j;j1<=c;j1++){
			R->entries[(j-1)*c+j1-1]=innermultiply(uj,submat(A,1,r,j1,j1))/nuj;			
		}				
	}	
	MatList* ml=(MatList*)malloc(sizeof(MatList));
	ml->mat=Q;
	ml->next=(MatList*)malloc(sizeof(MatList));;
	ml->next->mat=R;
	freemat(ek);
	freemat(uj);
	freemat(aj);
	return ml;
}

// New functions

Mat* diag(Mat* A){
    int n;
    Mat* C;
    if((A->row ==1) || (A->col == 1)){
        n = A->row;
	if(n == 1){
	    n = A->col;
	}
	C = newmat(n,n,0);
	for(int i=1;i<=n;i++){
            C->entries[(i-1)*n+i-1]=A->entries[i-1];
	}
    }
    else{
	C = newmat(A->row,1,0);
	for(int i=1;i<=A->row;i++){
		C->entries[i-1]=A->entries[(i-1)*A->row+i-1];
	}
    }
    return C;
}

Mat* addrow(Mat* A,int r, Mat* newrow){
	Mat* B = newmat(A->row+1,A->col,1);
	int k=0;
	int rowAdded=0;
	for(int i=1;i<=B->row;i++){
		for(int j=1;j<=B->col;j++){
			if(i!=r){
				B->entries[k]=A->entries[(i-rowAdded-1)*A->col+j-1];
				k+=1;
			}
			else {
				B->entries[k]=newrow->entries[j-1];
				k+=1;
				rowAdded = 1;
			}
		}
	}
	return B;
}

void find_max(Mat* A, double *maximum, int* max_location_x, int* max_location_y){
	double d = A->entries[0];
	*maximum = d;
        *max_location_x = 1;
	*max_location_y = 1;	
	for(int i=1;i<=A->row;i++){
		for(int j=1;j<=A->col;j++){
			if(d<A->entries[(i-1)*A->col+j-1]){
				d = A->entries[(i-1)*A->col+j-1];
				*maximum = d;
				*max_location_x = i;
				*max_location_y = j;
			}
		}
	}
}

double ssdnorm(Mat* A, Mat* B){
        double d=0;
        int k=0;
        for(int i=1;i<=A->row;i++){
                for(int j=1;j<=A->col;j++){
		
                        d+=pow(A->entries[k]-B->entries[k],2);
                        k++;
                }
        }
        d=sqrt(d);
        return d;
}

