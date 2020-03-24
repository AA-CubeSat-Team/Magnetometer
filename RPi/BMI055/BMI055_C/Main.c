#include "../lib/I2Cdrive.h"
#include "../lib/externalLib.h"
#include "BMI055.h"

int main(int argc, char **argv){
    // Open the I2C communication
    connect_i2c();

    connect_bmi055_I2C();
    
    int set_res = 0;
    set_res = bmi055_set_resolution(0x02);
    if (set_res == 0){
        printf("Resolution set successfully.\n");
    }

    float gyro_resolution = bmi055_get_resolution();
    printf("Gyro resolution is %f deg/s.\n", gyro_resolution);
    
    // Test sensor
    int sensor_ok = bmi055_run_bist();

    float ang_x = 0.0;
    float ang_y = 0.0;
    float ang_z = 0.0;

    int i = 0;
    while (i < 1000){
        ang_x = bmi055_get_raw_gyro_X(gyro_resolution);
        //printf("Angular rate along X: %.2f deg/s\n", ang_x);

        ang_y = bmi055_get_raw_gyro_Y(gyro_resolution);
        //printf("Angular rate along Y: %.2f deg/s\n", ang_y);

        ang_z = bmi055_get_raw_gyro_Z(gyro_resolution);
        printf("Angular rate along Z: %.2f deg/s\n", ang_z);

        usleep(30000);
    }

    // CLose the I2C communication
    disconnect_i2c();

    return 0;
}
