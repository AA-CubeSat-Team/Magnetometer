#include "../lib/I2Cdrive.h"
#include "../lib/commonLib.h"
#include "HMC5983.h"
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_blas.h>
/*
  This example is to show how to calibrate the
  magnetometer using the sophisticated method. Refer to the README.md
 file to see which method we're using.
 */

#define CAL_SIZE 2000    // THe number of data points we record for calibration
#define MEAS_SIZE  1000   // THe number of data points we record after calibration

#define INCLUDE_BIAS 1   // Add more bias to see the effect of calibration

gsl_matrix *A1;
gsl_vector *b;
double F = 1.0;   // Default is 1. 

/**
   The deconstructor of the program. We always want to ensure that
   the dynamically stored pointers of A1 and b is freed. 
 **/
void __attribute__ ((destructor))  dtor() {
  printf("Running deconstructor\n");

  if(A1 != NULL){
    gsl_matrix_free(A1);
  }

  if(b != NULL){
    gsl_vector_free(b);
  }

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
  
  exit(0);
}

/**
   Write the X,Y,Z data into a CSV file
   Parameters:: filename: the file's name
                x: Array containing the x's data
		y: Array containing the y's data
		z: Array containing the z's data
		length: The length of an arrays 
 **/
int write_csv_file(char const *fileName, double* x, double* y, double* z, int length){
  FILE *f = fopen(fileName, "w");
  if (f == NULL) return -1;
  for(int i = 0; i < length; i++){
    fprintf(f, "%f,%f,%f\n", x[i], y[i], z[i]);
  }
  fclose(f);
  return 0;
}

/**
   Print the data in gsl_vector
   Parameters:: ve: Pointer to vector data
                n: The length of the vector
 **/
void print_vector(gsl_vector *ve, int n){
  for(int i = 0; i < n; i++){
    double val = gsl_vector_get(ve, i);
    printf("%.2f, ", val);
  }
  printf("\n");
}

// Print the context of the complex vector
void print_vector_complex(gsl_vector_complex *ve, int n ){
  for(int i = 0; i < n; i++){
    gsl_complex val = gsl_vector_complex_get(ve, i);
    printf("%.5f + %.5f i \n",GSL_REAL(val), GSL_IMAG(val));
  }
  printf("\n");
}

/**
   Print the data in gsl_matrix
   Parameters:: ma: The data of the matrix
                m: The # of columns in the matrix
		n: THe # of rows in the matrix
 **/
void print_matrix(gsl_matrix * ma, int m, int n){
  for(int i = 0; i < m; i++){
    for(int j = 0; j < n; j++){
      double val = gsl_matrix_get(ma, i, j);
      printf("%.2f, ", val);
      
    }
    printf("\n");
  }
  printf("\n");
}

void print_matrix_complex(gsl_matrix_complex *ma, int m, int n){
 for(int i = 0; i < m; i++){
    for(int j = 0; j < n; j++){
      gsl_complex val = gsl_matrix_complex_get(ma, i, j);
      printf("%.5f+%.5f i, ", GSL_REAL(val), GSL_IMAG(val));
      
    }
    printf("\n");
  }
  printf("\n");
}


/**
   Compute the square root of a square matrix
   Parameters:: M: The matrix
               n: The dimension of the matrix
 **/
gsl_matrix* computeSqrtMatrix(gsl_matrix * M, int n){
  gsl_matrix* Mcpy = gsl_matrix_calloc(n,n);
  gsl_matrix* res = gsl_matrix_calloc(n,n);

  gsl_matrix_memcpy(Mcpy, M);
  
  // Compute the eigenvalue and eigenvectors of the matrix
  gsl_vector_complex *eval = gsl_vector_complex_alloc (n);
  gsl_matrix_complex *evec = gsl_matrix_complex_alloc (n, n);

  gsl_eigen_nonsymmv_workspace * w =
    gsl_eigen_nonsymmv_alloc (n);

  gsl_eigen_nonsymmv (Mcpy, eval, evec, w);

  gsl_eigen_nonsymmv_free (w);

  gsl_eigen_nonsymmv_sort (eval, evec,
                           GSL_EIGEN_SORT_VAL_DESC);

  // See if there's any duplicate eigenvalues. If so, then
  // we need to use some other method to compute the square root
  // of a matrix
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

  // Compute M = P D P^-1 where D is a diagonal matrix with
  // eigenvalues, and P is the matrix containing the eigenvectors.
  // We are also square rooting the eigenvalue such that we can get
  // the square root of the matrix M. 
  // Is P and evec the same matrix?
  gsl_matrix* P = gsl_matrix_calloc(n,n);
  gsl_matrix* D = gsl_matrix_calloc(n,n);
  for(int i = 0; i < n; i++){
    gsl_matrix_set(D,i , i,   sqrt(gsl_vector_get(&eval_real.vector, i)));
    gsl_vector_complex_view evec_i = gsl_matrix_complex_column(evec, i);
    for(int j =0 ; j< n; j++){
      gsl_complex gval = gsl_vector_complex_get(&evec_i.vector, j);
      gsl_matrix_set(P, j,i, GSL_REAL(gval));
    }
  }

  //printf("Printing Square root of D matrix\n");
  //print_matrix(D, n, n);

  //printf("Printing P matrix\n");
  //print_matrix(P, n, n);

  // Get the matrix inversion of P
  gsl_matrix* Pinv = gsl_matrix_alloc(n,n);
  gsl_matrix* Pinv_pre = gsl_matrix_alloc(n,n);
  gsl_matrix_memcpy(Pinv_pre, P);
  gsl_permutation *p2 = gsl_permutation_alloc(n);
  int s;
  gsl_linalg_LU_decomp(Pinv_pre, p2, &s);
  gsl_linalg_LU_invert(Pinv_pre, p2, Pinv);

  //printf("Printinf Pinv\n");
  //print_matrix(Pinv, n, n);

  gsl_matrix_free(Pinv_pre);

  gsl_matrix* res_pre = gsl_matrix_alloc(n,n);

  // Compute the square root of the matrix M
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, D, Pinv, 0.0, res_pre);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, P, res_pre, 0.0, res);

  
  // Free the allocated memory
  gsl_matrix_free(Mcpy);
  gsl_matrix_free(res_pre);
  gsl_permutation_free(p2);
  gsl_matrix_free(D);
  gsl_matrix_free(P);
  gsl_matrix_free(Pinv);
  gsl_vector_complex_free(eval);
  gsl_matrix_complex_free(evec);

  return res;
}


// The calibration function. It will first find the ellipsoid
// that wll best fit the dataset, and then alter it to have a sphere figure
// Parameters:: x: the array of x data
//              y: The array of y data
//              z: The array of z data
//              N; The length of the array
//              k: tuning parameter, which effect how we fit ellipsoid
void calibrate(double *x ,double *y, double* z, int N, double k){
  double Cmatrix[] = {-1,  k/2.0-1,  k/2.0-1,  0,  0,  0,
                       k/2.0-1, -1,  k/2.0-1,  0,  0,  0,
                       k/2.0-1,  k/2.0-1, -1,  0,  0,  0,
                       0,  0,  0, -k,  0,  0,
                       0,  0,  0,  0, -k,  0,
                       0,  0,  0,  0,  0, -k};
  // A matrix view is a temporary matrix stored on stack
  gsl_matrix_view C = gsl_matrix_view_array (Cmatrix,6, 6);
  
  double tempS[100] = {0};
  
  gsl_matrix * D = gsl_matrix_calloc(10, N);
  gsl_matrix_view S = gsl_matrix_view_array(tempS, 10, 10);
  double max_x = -INFINITY;
  double min_x = INFINITY;
  for(int i = 0 ; i < N; i++){
    //printf("%.2f, %.2f, %.2f\n", x[i], y[i], z[i]);
    if(x[i] > max_x){
      max_x = x[i];
    }
    if(x[i] < min_x){
      min_x = x[i];
    }
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
  // Determine the radius of sphere, assuming the data isn't to skewed.
  F = (max_x - min_x)/2.0;
  gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, D, D, 0.0, &S.matrix);

  // Free gsl_matrix *D
  gsl_matrix_free(D);

  // 
  gsl_matrix_view S11 = gsl_matrix_submatrix(&S.matrix, 0, 0, 6, 6);
  gsl_matrix_view S22 = gsl_matrix_submatrix(&S.matrix, 6, 6, 4, 4);
  gsl_matrix_view S12 = gsl_matrix_submatrix(&S.matrix, 0, 6, 6, 4);
  gsl_matrix_view S21 = gsl_matrix_submatrix(&S.matrix, 6, 0, 4, 6);


  // Compute the inverse of S22
  int s2;
  gsl_permutation *p = gsl_permutation_alloc(4);
  gsl_linalg_LU_decomp(&S22.matrix, p, &s2);
  gsl_matrix * invS22 = gsl_matrix_alloc(4,4);
  gsl_linalg_LU_invert(&S22.matrix, p, invS22);
  gsl_permutation_free(p);

  //Pre step for cComputing the E matrix which is later used to compute eigh
  gsl_matrix * tempE = gsl_matrix_alloc(4, 6);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, invS22, &S21.matrix, 0.0, tempE);
  gsl_matrix * tempE2 = gsl_matrix_alloc(6,6);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, &S12.matrix, tempE, 0.0, tempE2);
  gsl_matrix *tempE3 = gsl_matrix_alloc(6,6);
  gsl_matrix_memcpy( tempE3 , &S11.matrix);
  gsl_matrix_sub(tempE3, tempE2);


  // Compute the inverse of C matrix
  gsl_matrix *invC = gsl_matrix_alloc(6,6);
  int s3;
  gsl_permutation* p2 = gsl_permutation_alloc(6);
  gsl_linalg_LU_decomp(&C.matrix, p2, &s3);
  gsl_linalg_LU_invert(&C.matrix, p2, invC);
  gsl_permutation_free(p2);
  
  // Compute the E matrix
  gsl_matrix *E = gsl_matrix_alloc(6,6);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, invC, tempE3, 0.0, E);

  // Compute the eigenvalue and eigenvector of E
  gsl_vector_complex *eval = gsl_vector_complex_alloc(6);
  gsl_matrix_complex *evec = gsl_matrix_complex_alloc (6, 6);
  gsl_eigen_nonsymmv_workspace * w = gsl_eigen_nonsymmv_alloc (6);
  gsl_matrix* Ecpy = gsl_matrix_alloc(6,6);
  gsl_matrix_memcpy(Ecpy, E);
  gsl_eigen_nonsymmv (Ecpy, eval, evec, w);
  gsl_eigen_nonsymmv_free (w);
  gsl_eigen_nonsymmv_sort (eval, evec,
                           GSL_EIGEN_SORT_VAL_DESC);
  gsl_matrix_free(Ecpy);

  // Get the eigenvector associated with largest eigenvalue
  gsl_vector_complex_view v1_pre
    = gsl_matrix_complex_column (evec, 0);
  gsl_vector_view v1 =  gsl_vector_complex_real(&v1_pre.vector);
  if(gsl_vector_get(&v1.vector, 0) < 0){
    gsl_vector_scale(&v1.vector, -1);
  }

  // Compute v2 
  double temp[4]={0};
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

  // Compute the matrix M and vector n
  double Marr[9] = {v10, v15, v14,
		    v15, v11, v13,
		    v14, v13, v12};
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
  gsl_permutation_free(p3);
  gsl_matrix_free(M2);

  // Compute the matrix b and inv of A
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

  
  printf("Printing A:\n");
  print_matrix(A1, 3, 3 );

  printf("Printing b:\n");
  print_vector(b, 3);
  

  // Free up the allocated memory
  gsl_vector_free(tempA1);
  
  gsl_matrix_free(invM);
  
  gsl_vector_complex_free(eval);
  gsl_matrix_complex_free(evec);
  
  gsl_matrix_free(invS22);
  gsl_matrix_free(tempE);
  gsl_matrix_free(tempE2);
  gsl_matrix_free(tempE3);
  gsl_matrix_free(invC);
  gsl_matrix_free(E);
}

// Unit test function to see if Square root matrix function
// works correctly.
void testSquareMatrix(){
  double arr[] = {33,24,48,57};
  gsl_matrix_view test2 = gsl_matrix_view_array(arr,2,2);
  print_matrix(&test2.matrix, 2,2);

  gsl_matrix* test3 = computeSqrtMatrix(&test2.matrix, 2);
  printf("The result of squar root:\n");
  print_matrix(test3, 2, 2);
  gsl_matrix_free(test3);
}

// Set of unit test
void runTest(){
  testSquareMatrix();
}

int main(){

  //runTest();
  //return 0;
  
  /*
    Catch SIGINT 
   */
  signal(SIGINT, handle_sigint);
  
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
      printf("Gathered %d dataset for calibration\n", i);
    x[i] = hmc5983_get_raw_magnetic_x(1);
    y[i] = hmc5983_get_raw_magnetic_y(1);
    z[i] = hmc5983_get_raw_magnetic_z(1);

    while(isnan(x[i]) || isnan(y[i]) || isnan(z[i])){
      x[i] = hmc5983_get_raw_magnetic_x(1);
      y[i] = hmc5983_get_raw_magnetic_y(1);
      z[i] = hmc5983_get_raw_magnetic_z(1);
    }

    if(INCLUDE_BIAS){
      x[i] = x[i]*1.2 + .3;
      y[i] = y[i]*1.5 + .6;
      z[i] = z[i]*.8 - .4;
    }
    
    //printf("The magnetic field in X is %.2f G\n", x[i]);
    //printf("The magnetic field in Y is %.2f G\n", y[i]);
    //printf("The magnetic field in Z is %.2f G\n\n", z[i]);
    
    /*
      Sleep for a second before getting next measurement
     */
    usleep(30000);
  }
  
  calibrate(x,y,z,CAL_SIZE, 5);
  sleep(2);


  /*
    Compute the correted dataset after calibration
   */
  double x2[CAL_SIZE];
  double y2[CAL_SIZE];
  double z2[CAL_SIZE];
  gsl_vector *data_i = gsl_vector_alloc(3);
  gsl_vector* data_cal_i = gsl_vector_alloc(3);
  for(int i = 0; i < CAL_SIZE; i++){
    if(i%100 == 0){
      printf("Calibrated %d data\n", i);
    }
    
    gsl_vector_set(data_i, 0, x[i]);
    gsl_vector_set(data_i, 1, y[i]);
    gsl_vector_set(data_i, 2, z[i]);

    
    gsl_vector_sub(data_i, b);
    gsl_blas_dgemv(CblasNoTrans, 1.0, A1, data_i, 0.0, data_cal_i);

    x2[i] = gsl_vector_get(data_cal_i, 0);
    y2[i] = gsl_vector_get(data_cal_i, 1);
    z2[i] = gsl_vector_get(data_cal_i, 2);
  }

  gsl_vector_free(data_i);
  gsl_vector_free(data_cal_i);

  // Save uncalibrated and calibrated data into csv files
  write_csv_file("data/uncalibrated.csv", x, y, z, CAL_SIZE);
  write_csv_file("data/calibrated.csv", x2, y2, z2, CAL_SIZE);
  
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
