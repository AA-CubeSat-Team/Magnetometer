#include "../Lib/I2Cdrive.h"
#include "../Lib/commonLib.h"
#include "HMC5983.h"


int main(){
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

  printf("\nGetting the current operating mode.\n");
  int modeStat =0;
  unsigned char pre_mode = hmc5983_get_mode();
  if(pre_mode == 0x01){
    printf("\nSetting the mode to idle mode\n");
    modeStat = hmc5983_set_mode(0x03);
    if(modeStat){
      printf("\nError in setting mode. Make sure that the wires are connected\n");
      return 0;
    }
  }else{
    printf("\nSetting the mode to single-measurement mode\n");
    modeStat = hmc5983_set_mode(0x01);
    if(modeStat){
      printf("\nError in setting mode. Make sure that the wires are connected\n");
      return 0;
    }
  }
  
  printf("\nGetting the operating mode again to see if it changed.\n");
  unsigned char mode = hmc5983_get_mode();
  
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
