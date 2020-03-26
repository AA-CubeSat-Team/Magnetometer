# Sensors

Each directory listed here is associated to different sensors. Currently there are only two sensors that are being tested, but maybe we will be testing more later on. One of the sensors that is being tested is the BMI055 which is an Inertial Measurement Unit (IMU) which has an accelerometer and gyroscope that can detect movement and rotation. Since the SOC-i is only looking at the angular rates, we will only look at the Gyroscope on the IMU. The second of the sensor is HMC5983, which is a magnetometer that measure the magnetic intensities. 

Note that both program is implemented in C so if you don't have a lot of experience with C, then please go through a tutorial before going through the code. Currently, the HMC5983 is more developed and the BMI055 is a work in progress. 

