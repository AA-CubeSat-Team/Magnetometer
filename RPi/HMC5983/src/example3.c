#include "../Lib/I2Cdrive.h"
#include "../Lib/commonLib.h"
#include "HMC5983.h"

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
  
  printf("Starting program\n");
  
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
    Setting the Configuration Register to an invalid value. This will 
    cause the measurement data to not be updated. 
   */
  unsigned char invalid_cra = 0x09;
  printf("\nSetting the Configuration Register A to 0x%02x\n", invalid_cra);
  hmc5983_set_cra(invalid_cra);
  hmc5983_set_mode(0x01);
  hmc5983_set_crb(0x40);
  sleep(1);
  
  float x = hmc5983_get_magnetic_x();
  float y = hmc5983_get_magnetic_y();
  float z = hmc5983_get_magnetic_z();

  printf("The magnetic field in X is %.2f G\n", x);
  printf("The magnetic field in Y is %.2f G\n", y);
  printf("The magnetic field in Z is %.2f G\n", z);

  printf("\nMove the sensor around\n");
  printf("Waiting a few seconds, to see if value changes\n");
  sleep(5);
  hmc5983_set_mode(0x01);

  /*
    These measurement should be very similar to the previous
    data even if we move the sensor, because of the invalid value
    in Configuration Register A.
   */
  x = hmc5983_get_magnetic_x();
  y = hmc5983_get_magnetic_y();
  z = hmc5983_get_magnetic_z();

  printf("The magnetic field in X is %.2f G\n", x);
  printf("The magnetic field in Y is %.2f G\n", y);
  printf("The magnetic field in Z is %.2f G\n", z);

  /*
    Now setting a valid value in Configuration Register A. This
    should fix the problem we had before. 
   */
  unsigned char valid_cra = 0x10;
  printf("\nNow setting the Configuration Register A to 0x%02x\n", valid_cra);
  hmc5983_set_cra(valid_cra);
  hmc5983_set_mode(0x01);
  sleep(5);

  x = hmc5983_get_magnetic_x();
  y = hmc5983_get_magnetic_y();
  z = hmc5983_get_magnetic_z();

  printf("The magnetic field in X is %.2f G\n", x);
  printf("The magnetic field in Y is %.2f G\n", y);
  printf("The magnetic field in Z is %.2f G\n", z);

  
  printf("\nMove the sensor around\n");
  printf("Waiting a few seconds, to see if value changes\n");
  hmc5983_set_mode(0x01);
  sleep(5);

  x = hmc5983_get_magnetic_x();
  y = hmc5983_get_magnetic_y();
  z = hmc5983_get_magnetic_z();

  printf("The magnetic field in X is %.2f G\n", x);
  printf("The magnetic field in Y is %.2f G\n", y);
  printf("The magnetic field in Z is %.2f G\n", z);
  
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
