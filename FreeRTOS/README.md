# FreeRTOS

In this directory, there is an example for using FreeRTOS with the sensor. Currently, this is still a work in progress. The LPSPI example is currently the most developed. The SPI example is an example where the iMXRT1050 board is communicating with the magnetometer sensor HMC5983 through SPI. Note that you'll have to manually enable SPI on the HMC5983 by bridiging some ports. The SPI is also a good example of how task and queuing is being handled. 

Ignore the SD card directory, planning to remove it later. 
