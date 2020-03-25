# Sensors for AACT

## What's in this repository?

This repository contains the work related to the sensors for SOC-i. Feel free to clone and edit the code. 
There are currently three directory you could choose to follow. 
* The first is the Arduino, but there is actually no Arduino code for it. It actually contains a simulink model that will communicate with the Arduino to gather sensor data. It should be easy as uploaded the model, connecting your computer to an Arduino, and running the model. Currently, it's being abandoned since it doesn't seem necessary but we might come back to it. 
* The second is FreeRTOS which is very complicated. There is a lot going on in FreeRTOS and it will be easy to get lost in, but it is the one that will be used in the final product of SOC-i. There's a lot to learn about FreeRTOS, and a good resource to learn about it is [here](https://www.freertos.org/wp-content/uploads/2018/07/161204_Mastering_the_FreeRTOS_Real_Time_Kernel-A_Hands-On_Tutorial_Guide.pdf). If you don't have a lot of FreeRTOS experience, then it's advised to go through the tutorial first. This directory will only contain the FreeRTOS code for the sensors, but since SOC-i will be running FreeRTOS maybe look into other repository for other FreeRTOS code. There's still alot of work that's needed to be done with FreeRTOS is general... so good luck. 
* The third directory is the Raspberry Pi which is further developed than compared to the other directories. This is because to test out the sensors, we used a MCU that we're more familiar with. If you don't have a lot of programming experience, then it's advised to look through the code for Raspberry Pi first. There's some example for some of the sensors whcih could help you with developing code for the sensors. This is a good directory to look through first. 

## Cloning

To clone this repository type the following command:

```
git clone https://github.com/AA-CubeSat-Team/Sensor.git
```
