#include "../lib/I2Cdrive.h"
#include "../lib/externalLib.h"
#include "BMI055.h"

void create_filename(char *filename, int freq, int pts){
  strcpy(filename, "./data/data_");
  time_t rawtime;
  struct tm *timeinfo;
  time(&rawtime);
  timeinfo = localtime( &rawtime);
  char timeValue[100];
  strcpy(timeValue, asctime(timeinfo));
  int lastIndex;
  for(int i = 0; i < 100; i++){
    if(timeValue[i] == ' '){
      timeValue[i] = '_';
    }else if(timeValue[i] == 0){
      lastIndex = i;
      break;
    }
  }
  timeValue[lastIndex-1] = '\0'; 

  int int_length = snprintf(NULL, 0, "%dHz_%dpts_", freq, pts);
  char* str = malloc( int_length + 1 );
  snprintf( str, int_length + 1, "%dHz_%dpts_", freq, pts);

  strcat(filename, str);
  strcat(filename,timeValue);
  strcat(filename, ".csv");
  free(str);
}

void calcTimeDiff(struct timespec t1, struct timespec t2, struct timespec *td)
{
    td->tv_nsec = t2.tv_nsec - t1.tv_nsec;
    td->tv_sec  = t2.tv_sec - t1.tv_sec;
    if (td->tv_sec > 0 && td->tv_nsec < 0)
    {
        td->tv_nsec += 1000000000;
        td->tv_sec--;
    }
    else if (td->tv_sec < 0 && td->tv_nsec > 0)
    {
        td->tv_nsec -= 1000000000;
        td->tv_sec++;
    }
}

int main(int argc, char **argv){

    int sampling_frequency = 50; //Hz
    int num_samples = 100000;
    if(argc == 3){
        //sampling_frequency = argv[1];
        sscanf(argv[1], "%d", &sampling_frequency); 
        //num_samples = argv[2];
        sscanf(argv[2], "%d", &num_samples); 
    }
    else{
        printf("No arguments provided.\n");
        printf("Using defaults: %d Hz, %d data points", sampling_frequency, num_samples);
    }
    
    // Approximate time for each call of clovk_gettime is 3000 microseconds
    int time_interval = ((int) (1000000.0/sampling_frequency)) - 3000; //In microseconds
    

    FILE *fp;
    
    char filename[100]; 
    create_filename(filename, sampling_frequency, num_samples);
    printf("Data is going to be save in file %s\n",filename);
    fp = fopen(filename, "w+");
    
    struct timespec startTime;
    struct timespec currTime;
    struct timespec diffTime;
    
    // Open the I2C communication
    connect_i2c();

    connect_bmi055_I2C();

    float gyro_resolution = bmi055_get_resolution();
    printf("Gyro resolution is %f deg/s.\n", gyro_resolution);

    float ang_x = 0.0;
    float ang_y = 0.0;
    float ang_z = 0.0;

    float ang_x_old = 0.0;
    float ang_y_old = 0.0;
    float ang_z_old = 0.0;

    fprintf(fp, "t, x, y, z\n");
    
    // Ignore the first 100 readings
    int i = -100;
    clock_gettime(CLOCK_REALTIME, &startTime); 
    while (i < num_samples){
        // Ignore first 100 samples. Set during initialization of i
        if(i>=0){
        ang_x = bmi055_get_raw_gyro_X(gyro_resolution);
        //printf("Angular rate along X: %.2f deg/s\n", ang_x);

        ang_y = bmi055_get_raw_gyro_Y(gyro_resolution);
        //printf("Angular rate along Y: %.2f deg/s\n", ang_y);

        ang_z = bmi055_get_raw_gyro_Z(gyro_resolution);
        //printf("Angular rate along Z: %.2f deg/s\n", ang_z);
        clock_gettime(CLOCK_REALTIME, &currTime);
        
        calcTimeDiff(startTime, currTime, &diffTime);
        //While printing the data, a 0 at the end indicates true data
        //1 indicates that the last data was used due to atleast one NaN.
        if(!isnan(ang_x) && !isnan(ang_y) && !isnan(ang_z)){
            fprintf(fp, "%d.%.9ld, %f,%f,%f,0\n", (int) diffTime.tv_sec, diffTime.tv_nsec, ang_x, ang_y, ang_z);
            ang_x_old = ang_x;
            ang_y_old = ang_y;
            ang_z_old = ang_z;
        }
        else{
            fprintf(fp, "%d.%.9ld, %f,%f,%f,1\n", (int) diffTime.tv_sec, diffTime.tv_nsec, ang_x_old, ang_y_old, ang_z_old);
        }
        
        if(i%100 == 0){
                printf("%d\n", i);
            }
        }
        i++;

        usleep(time_interval);
    }
    fclose(fp);

    // CLose the I2C communication
    disconnect_i2c();

    return 0;
}
