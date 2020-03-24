#include "I2Cdrive.h"
#include "externalLib.h"

/**
This c program is a test bed for trying out 
I2C connection using Linux's command. This is 
the base code for I2C communication.
 **/

#define BUFFER_SIZE 60

int file_i2c;   // The i2c location in the device
unsigned char i2c_buffer[BUFFER_SIZE] = {0};  // buffer to store data from slave chip
int i2c_buffer_index = 0;

/*
  Open the I2C Bus on the device running
  a Linux OS.

     Returns:: 1: if opening failed
               0: if opening succeed 
*/
int connect_i2c(){
  char *filename = (char*) "/dev/i2c-1";    // The path to i2c
  // Are we able to get read/write acces to the specified file?
  if ((file_i2c = open(filename,O_RDWR )) <0){ 
    printf("Failed opening I2C bus\n");
    return 1 ;
  }
  return 0;
}

/*
  Close the I2C Bus on the device running
  a Linux OS.

     Returns:: 1: if closing the i2c connection failed
               0: if closing the i2c connection succeed 
*/
int disconnect_i2c(){
  if (close(file_i2c)){
    printf("Failed closing I2C bus\n");
    return 1;
  }
  return 0;
}

/*
 Connect to a specific slave chip
 using it's I2C address

    Parameters:: addr: the slave chip address
    Return:: 1: connection failed
             0: connection succeed
*/
int i2c_connect_2_slave(int addr){
  if(ioctl(file_i2c, I2C_SLAVE, addr)< 0){
    printf("Failed connecting to slave chip at \n address %h\n", addr);
    return 1;
  }
  return 0;
}

/*
 Read n Bytes of data from the I2C bus

    Parameters:: n: the number of Bytes 
    Returns:: 1: if reading failed
              0: if reading succeed 
*/
int i2c_read_bytes(int n){
  int i = 0;
  if(i2c_buffer_index+n > BUFFER_SIZE){
    i2c_buffer_index = 0;
  }
  while (read(file_i2c, &i2c_buffer[i2c_buffer_index], n) < 0 && i < 10){
    printf("Failed reading I2C bus\n");
    i += 1;
  }
  if( i == 10){
    return 1;
  }
  i2c_buffer_index = (i2c_buffer_index+1)%BUFFER_SIZE;
  return 0;
}

/*
 Write n Bytes of data from the I2C bus

    Parameters:: write_buffer: the data we want to send 
                 n: the number of Bytes 
    Returns:: 1: if reading failed
              0: if reading succeed
*/
int i2c_write_bytes(unsigned char* write_buffer, int n ){
  //printf("%x\n", &write_buffer);
  int i = 0;
  while(write(file_i2c, write_buffer, n) != n && i < 10){
    printf("Failed writing to I2C bus\n");
    i += 1;
  }

  if (i == 10)
    return 1;
  return 0;
}


/*
 Get the value at the slave chip register

    Parameters:: reg: the register you want to read in the slave chip
 */
int i2c_get_register_val(unsigned char reg){
  int write_status = i2c_write_bytes(&reg, 1);
  if(write_status){
    printf("Failed to write at regsiter %0x02x\n", reg);
    return -1;
  }
  //printf("Before the read buffer at %d is %02x\n", i2c_buffer_index, i2c_buffer[i2c_buffer_index]);
  int read_status = i2c_read_bytes(1);
  //printf("After the read buffer at %d is %02x\n", i2c_buffer_index-1, i2c_buffer[i2c_buffer_index-1]);

  
  if(read_status){
    printf("Failed to read at register %0x02x\n", reg);
    return -1;
  }

  int val = i2c_buffer_index-1;
  return val;
}

/*
 Set the value of the register in the slave chip
 
    Paramters:: reg: the register in the chip
                data: the data you want to set the register at
 */
int i2c_set_register_val(unsigned char reg, unsigned char data){
  unsigned char write_buffer[2] = {reg, data};
  int write_status = i2c_write_bytes(write_buffer, 2);
  return write_status;
}


unsigned char i2c_get_buffer_index(int i){
  unsigned char val = i2c_buffer[i];
  return val;
}
				      
