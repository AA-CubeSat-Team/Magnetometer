# Magnetometer with FreeRTOS

Interfacing with the magnetometer HMC5983 with FreeRTOS. There are currently
two directory. The directory with "_lpspi" is the project which communicates with 
the HMC5983 through SPI using FreeRTOS. The directory with "_sdcard" is the project
which stores data into SD card. Currently trying to store the magnetometer data into 
an SD card, which could be useful for calibrating the dataset in another device. 
