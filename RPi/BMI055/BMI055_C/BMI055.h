#ifndef BMI055_H
#define BMI055_H

int connect_bmi055_I2C();
// int setupBMI();
float bmi055_get_resolution();
int bmi055_set_resolution(unsigned char res);
// int end_bmi();
float bmi055_get_raw_gyro_X(float resolution);
float bmi055_get_raw_gyro_Y(float resolution);
float bmi055_get_raw_gyro_Z(float resolution);
// int bmi055_get_gyro_X();
// int bmi055_get_gyro_Y();
// int bmi055_get_gyro_Z();
int bmi055_run_bist();

#endif
