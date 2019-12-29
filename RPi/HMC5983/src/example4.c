#include "../lib/I2Cdrive.h"
#include "../lib/commonLib.h"
#include "HMC5983.h"

/*
This examples is to demonstrate the self test 
operation that is mentioned in the HMC5983 
document. 

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

/*
 A simple example of pointer reference
 */
unsigned char  update_setting(unsigned char *gain){
  unsigned char val = 0;
  if(*gain == 0xA0){
    *gain = 0xC0;
  }else if(*gain == 0xC0){
    *gain = 0xE0;
  }else{
    val = 1;
  }
  return val;
}

unsigned char check_positive_value(unsigned char gain, float x, float y, float z){
  int lower_bound = 243;
  int upper_bound = 575;
  if(gain == 0xA0){
    if(lower_bound < x && x < upper_bound &&
       lower_bound < y && y < upper_bound &&
       lower_bound < z && z < upper_bound){
      return 1;
    }
  }else if(gain == 0xC0){
    lower_bound = lower_bound * 330.0/390.0;
    upper_bound = upper_bound * 330.0/390.0;
    if(lower_bound < x && x < upper_bound &&
       lower_bound < y && y < upper_bound &&
       lower_bound < z && z < upper_bound){
      return 1;
    }
  }else if(gain == 0xE0){
    lower_bound = lower_bound * 230.0/390.0;
    upper_bound = upper_bound * 230.0/390.0;
    if(lower_bound < x && x < upper_bound &&
       lower_bound < y && y < upper_bound &&
       lower_bound < z && z < upper_bound){
      return 1;
    }
  }else{
    printf("Did nothing\n");
  }

  return 0;
  
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
    Set the data output rate to 8-average, 15 Hz, positive bias
   */
  hmc5983_set_cra(0x71);
  printf("\nCurrent cra is %d\n", hmc5983_get_cra());
  
  /*
    Set to continuous-measurement mode
   */
  hmc5983_set_mode(0x00);
  printf("\nCurrent mode is %d\n", hmc5983_get_mode());

  /*
    Set the magnetic field range to +- 4.7
   */
  unsigned char gain_setting = 0xA0;
  hmc5983_set_crb(gain_setting);

  float x_m = 0.0;
  float y_m = 0.0;
  float z_m = 0.0;
  // Putting an upper bound of 3 because we should only see
  // an iteration at most of 3. 
  for(int j =0; j < 3; j++){
    for(int i = 0; i < 5; i++){
      x_m = hmc5983_get_raw_magnetic_x(0);
      y_m = hmc5983_get_raw_magnetic_y(0);
      z_m = hmc5983_get_raw_magnetic_z(0);  
      
      /*
	Sleep for a second before getting next measurement
      */
      sleep(1);
    }
    printf("The magnetic field in X is %.2f\n", x_m);
    printf("The magnetic field in Y is %.2f\n", y_m);
    printf("The magnetic field in Z is %.2f\n\n", z_m);

    
    unsigned char pass = check_positive_value(gain_setting, x_m, y_m, z_m);
    if(pass){
      hmc5983_set_cra(0x70);
      break;
    }else{
      if(update_setting(&gain_setting)){
	printf("Failed positive self test.\n");
	hmc5983_set_cra(0x70);

	break;
      }
    }

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

