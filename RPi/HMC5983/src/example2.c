#include "../lib/I2Cdrive.h"
#include "../lib/commonLib.h"
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

  printf("\nSetting the operating mode to single-measurement mode\n");
  printf("and taking a single measurement\n");
  
  printf("\nGetting the current operating mode.\n");
  int modeStat =0;
  unsigned char pre_mode = hmc5983_get_mode();
  if(pre_mode == 0x01){
    printf("\nCurrent mode is already single-measurement mode\n");
    
  }else{
    printf("\nSetting the mode to single-measurement mode\n");
    modeStat = hmc5983_set_mode(0x01);
    if(modeStat){
      printf("\nError in setting mode. Make sure that the wires are connected\n");
      return 0;
    }
    printf("\nGetting the operating mode again to see if it changed.\n");
    unsigned char post_mode = hmc5983_get_mode();
    printf("\nCurent Operating mode is 0x%02x\n", post_mode);
  }
  
  printf("\nCurrent cra value is 0x%02x\n", hmc5983_get_cra());
  
  
  int s = hmc5983_get_status();
  printf("The current status is 0x%02x\n", s);
  
  sleep(1);
  
  printf("\nReading measurement data\n");
  float x = hmc5983_get_magnetic_x();
  float y = hmc5983_get_magnetic_y();
  float z = hmc5983_get_magnetic_z();

  printf("\nThe measurements are \n");
  printf("\tx: %f G\n", x);
  printf("\ty: %f G\n", y);
  printf("\tz: %f G\n", z);

  s = hmc5983_get_status();
  printf("The current status is %d\n", s);

  printf("\nAfter getting the measruement, the single mode changes to\n");
  printf("idle mode. So now checking if it did.\n");
  unsigned char after_mode = hmc5983_get_mode();
  printf("\nAfter measurement, mode is 0x%02x\n", after_mode);

  printf("\nSetting mode back to single-measurement mode\n");
  modeStat = hmc5983_set_mode(0x01);
  sleep(2);
  
  printf("\nReading another measurement data\n");
  x = hmc5983_get_magnetic_x();
  y = hmc5983_get_magnetic_y();
  z = hmc5983_get_magnetic_z();

  printf("\nThe measurements are \n");
  printf("\tx: %f G\n", x);
  printf("\ty: %f G\n", y);
  printf("\tz: %f G\n", z);
  
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
