#include <wiringPiSPI.h>
#include <math.h>
#include <stdio.h>

#define SPI_SPEED 8000000
#define SPI_CHANNEL 0
int main(){
  printf("Hello World\n");

  int status = wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED); 
  if(status == -1){
    printf("Got an error with setting up wiringPi SPI");
    return 1;
  }

  unsigned char buffer[520];
  buffer[0] = 0x80;
  printf("Sending 0x%x\n", (unsigned char)buffer[0]);
  
  int status2 =  wiringPiSPIDataRW(SPI_CHANNEL, buffer, 1);
  if(status2 == -1){
    printf("Error in reading spi");
      return 1;
  }
  printf("Got a response: 0x%x\n", (unsigned char)buffer[0]);
  
  return 0;
  
}


