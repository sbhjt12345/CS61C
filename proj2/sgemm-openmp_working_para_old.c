#include <nmmintrin.h>
#include <stdio.h>
  void transpose(int n,int m, float* dst, float* src){
  int i,j;  //n is row and m is column of original
  for (i=0;i<n;i++){
    for (j=0;j<m;j++){
        dst[j+i*m]=src[i+j*n];
    }

  }

}
void sgemm( int m_a, int n_a, float *A, float *B, float *C ) {
  int Blockk=36;
  int Blockj=36;
  float* Apad=malloc(m_a*n_a*sizeof(float));
  transpose(m_a,n_a,Apad,A);
  A=Apad;
  float* Bpad=malloc(m_a*n_a*sizeof(float));
  transpose(m_a,n_a,Bpad,B);
  B=Bpad;            
 
  int KB=n_a/Blockk;
  int JB=m_a/Blockj;
  
#pragma omp parallel
  {
   __m128 a1, c1, a2, c2, a3, c3;
  __m128 b1, b2, b3, b4;
 
  int ii=0;
  int jj=0;
  int kk=0;

  int kkma=0;
#pragma omp for
 //   for (int w=0;w<JB;w++){
for( int j = 0; j <m_a/*Blockj*/; j++ ) {
   
        jj=j/*+w*Blockj*/;   
  int  jma=jj*m_a;
 for (int g=0;g<KB;g++){
  for( int k = 0; k </*n_a*/ Blockk; k+=4 ) {

            kk=k+g*Blockk;
      //   ii=i+l*Blocki;
  //     for (int u=0;u<JB;u++){
   //       jj=j+u*Blockj;
	           kkma=kk*m_a;

   //    b1 = _mm_load1_ps(B+jj+kkma);

      b1 = _mm_load1_ps(B+jj+kkma);
      //printf("b2");
     b2 = _mm_load1_ps(B+jj+kkma+1*m_a);
      b3 = _mm_load1_ps(B+jj+kkma+2*m_a);
      b4 = _mm_load1_ps(B+jj+kkma+3*m_a);
      
   for( int i = 0; i < m_a; i+=12 ) {
a1 = _mm_loadu_ps(A+i+kkma);
 c1 = _mm_loadu_ps(C+i+jma);
 c1 = _mm_add_ps(_mm_mul_ps(a1, b1), c1);
 
 a2 = _mm_loadu_ps(A+i+4+kkma);
 c2 = _mm_loadu_ps(C+i+4+jma);
 c2 = _mm_add_ps(_mm_mul_ps(a2, b1), c2);

 a3 = _mm_loadu_ps(A+i+8+kkma);	
 c3 = _mm_loadu_ps(C+i+8+jma);
 c3 = _mm_add_ps(_mm_mul_ps(a3, b1), c3);
 
 //  kkma += m_a;

   //printf("i");
   a1 = _mm_loadu_ps(A+i+kkma+m_a);
   c1 = _mm_add_ps(_mm_mul_ps(a1, b2), c1);

   a2 = _mm_loadu_ps(A+i+4+kkma+m_a);
   c2 = _mm_add_ps(_mm_mul_ps(a2, b2), c2);

   a3 = _mm_loadu_ps(A+i+8+kkma+m_a);	
   c3 = _mm_add_ps(_mm_mul_ps(a3, b2), c3);
  

 //kkma+= m_a;

 a1 = _mm_loadu_ps(A+i+kkma+2*m_a);
 c1 = _mm_add_ps(_mm_mul_ps(a1, b3), c1);

 a2 = _mm_loadu_ps(A+i+4+kkma+2*m_a);
 c2 = _mm_add_ps(_mm_mul_ps(a2, b3), c2);

 a3 = _mm_loadu_ps(A+i+8+kkma+2*m_a);	
 c3 = _mm_add_ps(_mm_mul_ps(a3, b3), c3);


// kkma+= m_a;

 a1 = _mm_loadu_ps(A+i+kkma+3*m_a);
 c1 = _mm_add_ps(_mm_mul_ps(a1, b4), c1);
 _mm_storeu_ps(C+i+jma, c1);

 a2 = _mm_loadu_ps(A+i+4+kkma+3*m_a);
 c2 = _mm_add_ps(_mm_mul_ps(a2, b4), c2);
 _mm_storeu_ps(C+i+4+jma, c2);  

 a3 = _mm_loadu_ps(A+i+8+kkma+3*m_a);	
 c3 = _mm_add_ps(_mm_mul_ps(a3, b4), c3);
 _mm_storeu_ps(C+i+8+jma, c3);
 
 
 _mm_storeu_ps(C+i+jma, c1);     
 _mm_storeu_ps(C+i+4+jma, c2);   
 _mm_storeu_ps(C+i+8+jma, c3);   
 
    

 /////////////
     
    // C[i+j*m_a] += A[i+kk*m_a] * B[j+kk*m_a];
      }
    }
  }       //   }        
        }
  }//openmp
}






























/*typedef struct rect block;
struct rect{
int startR;
int startC;
int endR;
int endC;
};

void sgemm1( int m_a, int n_a, float *A, float *B, float *C, block* blockA,\
    block* blockB) {
 int startAR=blockA->startR;
 int startAC=blockA->startC;
 int AsizeR=blockA->endR-startAR;
 int AsizeC=blockA->endC-startAC;

 int startBR=blockB->startR;
 int startBC=blockB->startC;
 int BsizeR=blockB->endR-startBR;
 int BsizeC=blockB->endC-startBC;
 if (AsizeR!=BsizeC||AsizeC!=BsizeR)
   fprintf(stderr, "The DIM is not right for A\\B\n");

for( int j = 0;j  < AsizeR; j++ ) {
   int jj=j+startBC;
   for( int k = 0; k < AsizeC; k++ ) {
  int kA=k+startAC;
  int kB=k+startBR;
 
   
   for( int i = 0; i < AsizeR; i++ ) {
     int ii=i+startAR;
     fprintf(stderr,"***\n");
     fprintf(stderr,"A[ii:startAR]:%d:%d:%dx used\n",ii,kA,(int)A[ii+kA*m_a]);
 
     fprintf(stderr,"B[jj:startAR]:%d:%d:%dxis used\n",jj,kB,(int)B[jj+kB*m_a]);
	
       fprintf(stderr,"C[ii:jj]:%d:%d:%dxis used\n",ii,jj,(int)C[ii+jj*m_a]);
   
       float temp=  A[ii+kA*m_a] * B[jj+kB*n_a];
       fprintf(stderr,"temp: %d\n",(int)temp);
       C[ii+jj*m_a] +=temp;
   
      fprintf(stderr,"C[ii:jj]:%d:%d:%dis used\n",ii,jj,(int)C[ii+jj*m_a]);
      fprintf(stderr,"**\n");
   }
    }
  }
}


 
void sgemm(int m_a,int n_a,float* A, float * B, float * C){
    block blockA;
    blockA.startR=0;
    blockA.startC=0;
    blockA.endR=m_a/2;
    blockA.endC=n_a;

     block blockB;
    blockB.startR=0;
    blockB.startC=0;
    blockB.endR=n_a;
    blockB.endC=m_a/2;

    sgemm1(m_a,n_a,A,B,C,&blockA,&blockB);
   fprintf(stderr,"****** 1st finish********\n");
    //////////////////////////////////
//        block blockA;
    blockA.startR=0;
    blockA.startC=0;
    blockA.endR=m_a/2;
    blockA.endC=n_a;

//     block blockB;
    blockB.startR=0;
    blockB.startC=m_a/2;
    blockB.endR=n_a;
    blockB.endC=m_a;

    sgemm1(m_a,n_a,A,B,C,&blockA,&blockB); 
    
   fprintf(stderr,"****** 2nd finish********\n"); 
   ///////////////////////////////////
 //      block blockA;
    blockA.startR=m_a/2;
    blockA.startC=0;
    blockA.endR=m_a;
    blockA.endC=n_a;

 //    block blockB;
    blockB.startR=0;
    blockB.startC=0;
    blockB.endR=n_a;
    blockB.endC=m_a/2;

    sgemm1(m_a,n_a,A,B,C,&blockA,&blockB); 



 
   fprintf(stderr,"****** 3rd finish********\n"); 

   /////////////////////////////////

  //     block blockA;
    blockA.startR=m_a/2;
    blockA.startC=0;
    blockA.endR=m_a;
    blockA.endC=n_a;

  //   block blockB;
    blockB.startR=0;
    blockB.startC=m_a/2;
    blockB.endR=n_a;
    blockB.endC=m_a;

    sgemm1(m_a,n_a,A,B,C,&blockA,&blockB); 
   
   fprintf(stderr,"****** 4th finish********\n"); 

    


} */                                      
