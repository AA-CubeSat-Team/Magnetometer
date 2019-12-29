#include "../lib/I2Cdrive.h"
#include "../lib/commonLib.h"
#include "HMC5983.h"

/*
  This example is to show how to calibrate the
  magnetometer using a naive method.
 */

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
  float low_val[3] = {INFINITY, INFINITY, INFINITY};
  float high_val[3] = {-INFINITY, -INFINITY, -INFINITY};

  printf("Randomly rotate the magnetometer around\n");
  printf("to gather data for calibration\n\n");
  
  /*
    Getting the calibration setting
   */
  for(int i = 0; i < 1000; i++){
    if(i%100 == 0)
      printf("Gathered %d for calibration\n", i);
    x_m = hmc5983_get_magnetic_x();
    y_m = hmc5983_get_magnetic_y();
    z_m = hmc5983_get_magnetic_z();
    
    //printf("The magnetic field in X is %.2f G\n", x_m);
    //printf("The magnetic field in Y is %.2f G\n", y_m);
    //printf("The magnetic field in Z is %.2f G\n\n", z_m);

    if(low_val[0] > x_m){
      low_val[0] = x_m;
    }else if(high_val[0] <= x_m){
      high_val[0] = x_m;
    }

    if(low_val[1] > y_m){
      low_val[1] = y_m;
    }else if(high_val[1] <= y_m){
      high_val[1] = y_m;
    }

    if(low_val[2] > z_m){
      low_val[2] = z_m;
    }else if(high_val[2] <= z_m){
      high_val[2] = z_m;
    }
    
    /*
      Sleep for a second before getting next measurement
     */
    usleep(30000);
  }
  
  // Hard-iron offset 
  float offset_x = (high_val[0] - low_val[0])/2;
  float offset_y = (high_val[1] - low_val[1])/2;
  float offset_z = (high_val[2] - low_val[2])/2;

  // Soft-iron offset
  float avg_delta_xyz[3] = {(high_val[0] - low_val[0])/2, (high_val[1] - low_val[1])/2, (high_val[2] - low_val[2])/2};
  float avg_delta = (avg_delta_xyz[0] + avg_delta_xyz[1] + avg_delta_xyz[2])/3;

  float scale_x = avg_delta/ avg_delta_xyz[0];
  float scale_y = avg_delta/ avg_delta_xyz[1];
  float scale_z = avg_delta/ avg_delta_xyz[2];

  printf("The x offset is %.2f and x scale is %.2f\n", offset_x, scale_x);
  printf("The y offset is %.2f and x scale is %.2f\n", offset_y, scale_y);
  printf("The z offset is %.2f and x scale is %.2f\n\n", offset_z, scale_z);
  sleep(5);


  for(int i = 0; i < 30; i++){
    x_m = hmc5983_get_magnetic_x();
    y_m = hmc5983_get_magnetic_y();
    z_m = hmc5983_get_magnetic_z();

    // Calibrating the data
    x_m = (x_m - offset_x) * scale_x;
    y_m = (y_m - offset_y) * scale_y;
    z_m = (z_m - offset_z) * scale_z;

    printf("The magnetic field in X is %.2f G\n", x_m);
    printf("The magnetic field in Y is %.2f G\n", y_m);
    printf("The magnetic field in Z is %.2f G\n\n", z_m);
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
