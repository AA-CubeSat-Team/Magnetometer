#include "../Lib/I2Cdrive.h"
#include "../Lib/commonLib.h"
#include "HMC5983.h"
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_blas.h>
/*
  This example is to show how to calibrate the
  magnetometer using the sophisticated method.
 */

#define CAL_SIZE 100

gsl_matrix *A1;
gsl_vector *b;
double F = 1.0;   // Default is 1. 

void __attribute__ ((destructor))  dtor() {
  if(A1 != NULL){
    gsl_matrix_free(A1);
  }

  if(b != NULL){
    gsl_vector_free(b);
  }

  printf("Running deconstructor\n");
}

/*
  Handler for SIGINT, caused by
  Ctrl-C at keyboard
 */
void handle_sigint(int sig){
    /*
    Disconnect from the I2C bus
   */
  printf("\nDisconnecting from I2C Bus\n");
  int disStat = disconnect_i2c();
  if(disStat){
    printf("\nMake sure that no wires were disconnected\n");
  }

  if(A1 != NULL){
    gsl_matrix_free(A1);
  }
  if(b != NULL){
    gsl_vector_free(b);
  }
  
  exit(0);
}

int print_matrix(gsl_matrix * ma, int m, int n){
  for(int i = 0; i < m; i++){
    for(int j = 0; j < n; j++){
      double val = gsl_matrix_get(ma, i, j);
      printf("%.2f, ", val);
      
    }
    printf("\n");
  }
  printf("\n");
}


gsl_matrix* computeSqrtMatrix(gsl_matrix * M, int n){
  gsl_matrix* res = gsl_matrix_alloc(n,n);
  
  gsl_vector_complex *eval = gsl_vector_complex_alloc (n);
  gsl_matrix_complex *evec = gsl_matrix_complex_alloc (n, n);

  gsl_eigen_nonsymmv_workspace * w =
    gsl_eigen_nonsymmv_alloc (n);

  gsl_eigen_nonsymmv (M, eval, evec, w);

  gsl_eigen_nonsymmv_free (w);

  gsl_eigen_nonsymmv_sort (eval, evec,
                           GSL_EIGEN_SORT_VAL_DESC);

  gsl_vector_view eval_real = gsl_vector_complex_real(eval);
  for(int i = 0; i < n-1; i++){
    double eig1 = gsl_vector_get(&eval_real.vector, i);
    double eig2 = gsl_vector_get(&eval_real.vector ,i+1);
    if(eig1 == eig2){
	printf("There are duplicate eigenvalues");
	gsl_matrix_free(res);
	gsl_vector_complex_free(eval);
	gsl_matrix_complex_free(evec);
	return NULL;
      }

  }
  gsl_matrix* P = gsl_matrix_calloc(n,n);
  gsl_matrix* D = gsl_matrix_calloc(n,n);
  for(int i = 0; i < n; i++){
    gsl_matrix_set(D,i , i,   sqrt(gsl_vector_get(&eval_real.vector, i)));
    gsl_vector_complex_view evec_i = gsl_matrix_complex_column(evec, i);
    for(int j =0 ; j< n; j++){
      gsl_complex gval = gsl_vector_complex_get(&evec_i.vector, j);
      gsl_matrix_set(P, i,j, GSL_REAL(gval));
    }
  }

  gsl_matrix* Pinv = gsl_matrix_alloc(n,n);
  gsl_matrix_memcpy(Pinv, P);

  gsl_permutation *p2 = gsl_permutation_alloc(n);
  int s;
  gsl_linalg_LU_decomp(Pinv, p2, &s);
  gsl_linalg_LU_invert(Pinv, p2, Pinv);

  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, D, Pinv, 0.0, res);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, P, res, 0.0, res);
  
  gsl_permutation_free(p2);
  gsl_matrix_free(D);
  gsl_matrix_free(P);
  gsl_matrix_free(Pinv);
  gsl_vector_complex_free(eval);
  gsl_matrix_complex_free(evec);

  return res;
}

int ellipsoid_fit(double *x ,double *y, double* z, int N){
  double Cmatrix[] = {-1,  1,  1,  0,  0,  0,
                       1, -1,  1,  0,  0,  0,
                       1,  1, -1,  0,  0,  0,
                       0,  0,  0, -4,  0,  0,
                       0,  0,  0,  0, -4,  0,
                       0,  0,  0,  0,  0, -4};
  // A matrix view is a temporary matrix stored on stack
  gsl_matrix_view Cinv = gsl_matrix_view_array (Cmatrix,6, 6);
  
  double tempS[100] = {0};
  
  gsl_matrix * D = gsl_matrix_alloc(10, N);
  gsl_matrix_view S = gsl_matrix_view_array(tempS, 10, 10);

  for(int i = 0 ; i < N; i++){
    gsl_matrix_set(D, 0, i, gsl_pow_2(x[i]));
    gsl_matrix_set(D, 1, i, gsl_pow_2(y[i]));
    gsl_matrix_set(D, 2, i, gsl_pow_2(z[i]));

    gsl_matrix_set(D, 3, i, 2*y[i]*z[i]);
    gsl_matrix_set(D, 4, i, 2*x[i]*z[i]);
    gsl_matrix_set(D, 5, i, 2*x[i]*y[i]);

    gsl_matrix_set(D, 6, i, 2*x[i]);
    gsl_matrix_set(D, 7, i, 2*y[i]);
    gsl_matrix_set(D, 8, i, 2*z[i]);
    gsl_matrix_set(D, 9, i, 1);
  }

  //gsl_vector_fprintf(stdout, &x2.vector, "%g");

  //print_matrix(D, 10, 100);

  gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, D, D, 0.0, &S.matrix);
  print_matrix(&S.matrix, 10, 10);

  gsl_matrix_view S11 = gsl_matrix_submatrix(&S.matrix, 0, 0, 6, 6);
  gsl_matrix_view S22 = gsl_matrix_submatrix(&S.matrix, 6, 6, 4, 4);
  gsl_matrix_view S12 = gsl_matrix_submatrix(&S.matrix, 0, 6, 6, 4);
  gsl_matrix_view S21 = gsl_matrix_submatrix(&S.matrix, 6, 0, 4, 6);
  //printf("Testing S11\n");
  //print_matrix(&S11.matrix, 6, 6);

  //printf("texting S22\n");
  //print_matrix(&S22.matrix, 4, 4);

  //printf("Testing S12\n");
  //print_matrix(&S12.matrix, 6, 4);
  
  int s2;
  gsl_permutation *p = gsl_permutation_alloc(4);
  gsl_linalg_LU_decomp(&S22.matrix, p, &s2);
  //print_matrix(&m.matrix, 6,6);

  gsl_matrix * invm = gsl_matrix_alloc(4,4);
  gsl_linalg_LU_invert(&S22.matrix, p, invm);
  //print_matrix(invm, 6,6);

  gsl_matrix * tempE = gsl_matrix_alloc(4, 6);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, invm, &S21.matrix, 0.0, tempE);
  gsl_matrix * tempE2 = gsl_matrix_alloc(6,6);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, &S12.matrix, tempE, 0.0, tempE2);

  gsl_matrix *tempE3 = gsl_matrix_alloc(6,6);
  gsl_matrix_memcpy( tempE3 , &S11.matrix);
  gsl_matrix_sub(tempE3, tempE2);

  gsl_matrix *invm2 = gsl_matrix_alloc(6,6);
  int s3;
  gsl_permutation* p2 = gsl_permutation_alloc(6);
  gsl_linalg_LU_decomp(&Cinv.matrix, p2, &s3);
  gsl_linalg_LU_invert(&Cinv.matrix, p2, invm2);

  gsl_matrix *E = gsl_matrix_alloc(6,6);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, invm2, tempE3, 0.0, E);

  gsl_vector_complex *eval = gsl_vector_complex_alloc(6);
   gsl_matrix_complex *evec = gsl_matrix_complex_alloc (6, 6);

  gsl_eigen_nonsymmv_workspace * w = gsl_eigen_nonsymmv_alloc (6);

  gsl_eigen_nonsymmv (E, eval, evec, w);

  gsl_eigen_nonsymmv_free (w);

  gsl_eigen_nonsymmv_sort (eval, evec,
                           GSL_EIGEN_SORT_VAL_DESC);

  gsl_vector_complex_view v1_pre
    = gsl_matrix_complex_column (evec, 0);
  gsl_vector_view v1 =  gsl_vector_complex_real(&v1_pre.vector);
  if(gsl_vector_get(&v1.vector, 0) < 0){
    gsl_vector_scale(&v1.vector, -1);
  }

  printf("here\n");
  double temp[6];
  gsl_vector_view v2 = gsl_vector_view_array(temp, 4);
  gsl_blas_dgemv(CblasNoTrans, -1.0, tempE, &v1.vector, 0, &v2.vector);


  double v10 = gsl_vector_get(&v1.vector, 0);
  double v11 = gsl_vector_get(&v1.vector, 1);
  double v12 = gsl_vector_get(&v1.vector, 2);
  double v13 = gsl_vector_get(&v1.vector, 3);
  double v14 = gsl_vector_get(&v1.vector, 4);
  double v15 = gsl_vector_get(&v1.vector, 5);

  double v20 = gsl_vector_get(&v2.vector, 0);
  double v21 = gsl_vector_get(&v2.vector, 1);
  double v22 = gsl_vector_get(&v2.vector, 2);
  double d = gsl_vector_get(&v2.vector, 3);
  
  double Marr[9] = {v10, v13, v14, v13, v11, v15, v14, v15, v12};
  double narr[3] = {v20, v21, v22};

  gsl_matrix_view M = gsl_matrix_view_array(Marr, 3, 3);
  gsl_vector_view n2 = gsl_vector_view_array(narr, 3);

  gsl_matrix * M2 = gsl_matrix_alloc(3,3);
  gsl_matrix_memcpy(M2, &M.matrix);
  
  gsl_matrix *invM = gsl_matrix_alloc(3,3);
  int s4;
  gsl_permutation* p3 = gsl_permutation_alloc(3);
  gsl_linalg_LU_decomp(M2, p3, &s4);
  gsl_linalg_LU_invert(M2, p3, invM);

  b = gsl_vector_alloc(3);
  gsl_blas_dgemv(CblasNoTrans, -1.0, invM, &n2.vector, 0, b);


  
  gsl_vector *tempA1 = gsl_vector_alloc(3);
  gsl_vector_memcpy(tempA1, b);
  gsl_vector_scale(tempA1, -1);

  double valA1;
  gsl_blas_ddot(&n2.vector, tempA1, &valA1);
  valA1 = valA1 - d;
  valA1 = F/sqrt(valA1);

  A1 = computeSqrtMatrix(&M.matrix, 3);
  gsl_matrix_scale(A1, valA1);


  gsl_vector_free(tempA1);
  gsl_matrix_free(M2);
  
  gsl_matrix_free(invM);
  gsl_permutation_free(p3);
  
  gsl_vector_complex_free(eval);
  gsl_matrix_complex_free(evec);
  
  gsl_permutation_free(p);
  gsl_matrix_free(D);
  gsl_matrix_free(invm);
  gsl_matrix_free(tempE);
  gsl_matrix_free(tempE2);
  gsl_matrix_free(tempE3);
  gsl_matrix_free(invm2);
  gsl_permutation_free(p2);
  gsl_matrix_free(E);
}

int main(){
  /*
    Catch SIGINT 
   */
  signal(SIGINT, handle_sigint);

  /*
  double x[100];
  double y[100];
  double z[100];
  int upper = 10;
  int lower = -10;
  for(int i = 0; i < 100; i++){
    x[i] = (rand()% (upper - lower + 1))+lower;
    y[i] = (rand()% (upper - lower + 1))+lower;
    z[i] = (rand()% (upper - lower + 1))+lower;
   }
   
  ellipsoid_fit(x, y, z, 100);
  return 0;
  */
  
  /*
    Open up the I2C bus. This will need 
    to be called first before accessing any
    I2C tools.
   */
  int inStat = connect_i2c();
  if(inStat){
    printf("\nUnable to open the I2C bus.\n");
    printf("Make sure the i2c-tool is installed and you have root access\n");
    return 0;
  }
  /*
    Setting a connection to HMC5983 via I2C. 
    If you want to connect to a different slave chip
    via I2C, then run the following command

    i2c_connect_2_slave(SLAVE_ADDRESS);

   */
  printf("\nConnecting to HMC5983 via I2c\n");
  int stat2 = connect_2_hmc5983_via_I2C();
  if(stat2){
    printf("\nMake sure that the you set up the hardware correctly.\n");
    printf("run 'i2c-detect -y 1' in terminal and make sure \n");
    printf("you can see the slave address in it. \n");
    return 0;
  }


  /*
    Set the data output rate to 30 Hz
   */
  hmc5983_set_cra(0x14);
  printf("\nCurrent cra is %d\n", hmc5983_get_cra());
  
  /*
    Set to continuous-measurement mode
   */
  hmc5983_set_mode(0x00);
  printf("\nCurrent mode is %d\n", hmc5983_get_mode());
  
  
  /*
    Set the magnetic field range to +- 1.9
   */
  hmc5983_set_crb(0x40);

  
  double x[CAL_SIZE];
  double y[CAL_SIZE];
  double z[CAL_SIZE];
  
  printf("Randomly rotate the magnetometer around\n");
  printf("to gather data for calibration\n\n");
  
  /*
    Getting the calibration setting
   */
  for(int i = 0; i < CAL_SIZE; i++){
    if(i%100 == 0)
      printf("Gathered %d for calibration\n", i);
    x[i] = hmc5983_get_magnetic_x();
    y[i] = hmc5983_get_magnetic_y();
    z[i] = hmc5983_get_magnetic_z();
    
    //printf("The magnetic field in X is %.2f G\n", x_m);
    //printf("The magnetic field in Y is %.2f G\n", y_m);
    //printf("The magnetic field in Z is %.2f G\n\n", z_m);

    /*
      Sleep for a second before getting next measurement
     */
    usleep(30000);
  }
  
  ellipsoid_fit(x,y,z,CAL_SIZE);
  sleep(5);


  double x_m;
  double y_m;
  double z_m;
  gsl_vector* data_i = gsl_vector_alloc(3);
  for(int i = 0; i < 30; i++){
    x_m = hmc5983_get_magnetic_x();
    y_m = hmc5983_get_magnetic_y();
    z_m = hmc5983_get_magnetic_z();

    gsl_vector_set(data_i, 0, x_m);
    gsl_vector_set(data_i, 1, y_m);
    gsl_vector_set(data_i, 2, z_m);

    gsl_vector_sub(data_i, b);
    gsl_blas_dgemv(CblasNoTrans, 1.0, A1, data_i, 0.0, data_i);

    printf("The magnetic field in X is %.2f G\n", gsl_vector_get(data_i, 0));
    printf("The magnetic field in Y is %.2f G\n", gsl_vector_get(data_i, 1));
    printf("The magnetic field in Z is %.2f G\n\n", gsl_vector_get(data_i, 2));
    sleep(1);
  }

  gsl_vector_free(data_i);
  
  /*
    Disconnect from the I2C bus
   */
  printf("\nDisconnecting from I2C Bus\n");
  int disStat = disconnect_i2c();
  if(disStat){
    printf("\nMake sure that no wires were disconnected\n");
    return 0;
  }
  printf("\nProgram Finished\n");
}
