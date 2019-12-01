#include "../Lib/I2Cdrive.h"
#include "../Lib/commonLib.h"
#include "HMC5983.h"

/*
 The more sophisticated method of calibration.
 */


int findSide(float x1, float y1, float x2, float y2, float x3, float y3){
  int val = ((y3 - y1)*(x2 - x1) - (y2 - y1) * (x3 - x1));
  if (val > 0){
    return 1;
  }
  if (val < 0){
    return -1;
  }
  return 0;
}

int lineDist(float x1, float y1, float x2, float y2, float x3, float y3){
  return abs((y3 - y1)*(x2 - x1) - (y2 - y1) * (x3 - x1));
}


void quickhull(int n,float* x, float* y, float x_max, float y_max, float x_min, float y_min, int side, int* n_h, float* x_h, float* y_h){
  int ind = -1;
  int max_dist = 0;

  for(int i = 0; i < n;i++){
    int temp = lineDist(x_max, y_max, x_min, y_min, x[i], y[i]);
    if(findSide(x_max, y_max, x_min, y_min, x[i], y[i]) == side && temp > max_dist){
      ind = i;
      max_dist = temp;
    }
  }

  if(ind == -1){
    x_h[*n_h] = x_max;
    y_h[*n_h] = y_max;
    *n_h = *n_h + 1;
    x_h[*n_h] = x_min;
    y_h[*n_h] = y_min;
    *n_h = *n_h + 1;
    return;
  }

  quickhull(n, x, y,  x[ind], y[ind], x_max, y_max, -findSide(x[ind], y[ind], x_max, y_max, x_min, y_min), n_h, x_h, y_h);
  quickhull(n, x, y,  x[ind], y[ind], x_min, y_min, -findSide(x[ind], y[ind], x_min, y_min, x_max, y_max), n_h, x_h, y_h); 
			     
  
}


/*
  A Quick Hull algorithm to get the outer points of a set
 */
void quickhull_pre(int n, float* x, float* y, int* n_h, float* x_h, float* y_h){
  int max_x = 0;
  int min_x = 0;
  for(int i =0; i < n; i++){
    if(max_x < x[i]){
      max_x = i;
    }
    if(min_x > x[i]){
      min_x = i;
    }
  }
  quickhull(n, x, y, x[min_x], y[min_x], x[max_x], y[max_x], 1, n_h, x_h, y_h);
  quickhull(n, x, y, x[min_x], y[min_x], x[max_x], y[max_x], -1, n_h, x_h, y_h);
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

int main(){
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

  /*
    Get 10 measurement and print them out
   */
  float x_m = 0.0;
  float y_m = 0.0;
  float z_m = 0.0;
  for(int i = 0; i < 10; i++){
    x_m = hmc5983_get_magnetic_x();
    y_m = hmc5983_get_magnetic_y();
    z_m = hmc5983_get_magnetic_z();  
    printf("The magnetic field in X is %.2f G\n", x_m);
    printf("The magnetic field in Y is %.2f G\n", y_m);
    printf("The magnetic field in Z is %.2f G\n\n", z_m);

    /*
      Sleep for a second before getting next measurement
     */
    sleep(1);
  }

  
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
