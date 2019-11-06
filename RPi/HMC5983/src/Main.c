#include "../Lib/I2Cdrive.h"
#include "../Lib/commonLib.h"
#include "HMC5983.h"

// This is for the HMC5983 magnetometer.


/*
  Create a file that takes the current time
  as part of it's filename
 */
void create_filename(char *filename){
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
  strcat(filename,timeValue);
  strcat(filename, ".csv");
}

/*
   Plot some data
 */
void plot_data(char * filename, char* label, char* xlabel, char* ylabel,
	       int x, int y){
   // Plotting
  FILE * gnuplotPipe = popen("gnuplot -persistent", "w");

  fprintf(gnuplotPipe, "set title 'HMC5983 Calibrarted Value'\n");
  fprintf(gnuplotPipe, "set xlabel '%s'\n", xlabel);
  fprintf(gnuplotPipe, "show xlabel\n");
  fprintf(gnuplotPipe, "set ylabel '%s'\n", ylabel);
  fprintf(gnuplotPipe, "show ylabel\n");
  fprintf(gnuplotPipe, "set grid\n");
  fprintf(gnuplotPipe, "set datafile separator \",\"\n");
  //fprintf(gnuplotPipe, "plot '%s' using 0:1 title 'x', '%s' using 0:2 title 'y', '%s' using 0:3 title 'z'\n", filename, filename, filename);
  fprintf(gnuplotPipe, "plot '%s' using %d:%d title '%s'", filename, x, y, label);
  fprintf(gnuplotPipe, "exit\n");
  pclose(gnuplotPipe);

} 


/*
   Plot some data
 */
void plot_all_data(char * filename){
   // Plotting
  FILE * gnuplotPipe = popen("gnuplot -persistent", "w");

  fprintf(gnuplotPipe, "set title 'HMC5983 Calibrarted Value'\n");
  fprintf(gnuplotPipe, "set xlabel 'Mag (uT)'\n");
  fprintf(gnuplotPipe, "show xlabel\n");
  fprintf(gnuplotPipe, "set ylabel 'Mag (uT)'\n");
  fprintf(gnuplotPipe, "show ylabel\n");
  fprintf(gnuplotPipe, "set grid\n");
  fprintf(gnuplotPipe, "set datafile separator \",\"\n");
  fprintf(gnuplotPipe, "plot '%s' using 2:3 title 'xy', '%s' using 3:4 title 'yz','%s' using 2:4 title 'xz'\n", filename, filename, filename);
  //fprintf(gnuplotPipe, "plot '%s' using 2:3 title 'xy'\n", filename);
  fprintf(gnuplotPipe, "exit\n");
  pclose(gnuplotPipe);

} 


/*
  Main function 
 */
int main(int argc, char **argv){
  // Where we will be writing our data
  FILE *fp;
  
  char filename[100];
  create_filename(filename);

  printf("Data is going to be save in file %s\n",filename);
  fp = fopen(filename, "w+");  

  // Open the I2C bus
  int in_stat =   connect_i2c();
  if(in_stat){
    return 0;
  }else{
    disconnect_i2c();
    return 0;
  }

  // Connect to the HMC5983 chip
  connect_2_hmc5983_via_I2C();
  hmc5983_set_cra_reg(0x90);
  hmc5983_set_mode(0x00);
  //unsigned char m = hmc5983_get_mode();
  hmc5983_set_gain(0x40);
  sleep(1);  // Wait for HMC5983 to setup

  unsigned char gain_hex = hmc5983_get_gain();
  printf("The gain hex is 0x%x\n", gain_hex);
  
  if(argc == 2){
    int argVal = atoi(argv[1]);
    if(argVal == 1){
      // Get the previously saved calibrated biases
      hmc5983_get_calibrated_value_from_file();
    }else if(argVal == 2){
      hmc5983_get_calibrated_value_from_file();
      hmc5983_set_calibration_offset(0,0,0);
      hmc5983_set_calibration_scale(1.0, 1.0, 1.0);
    }
  }else{
  // Calibration step. Record data and get the biases.
    hmc5983_calibrate();
  }
  // Getting the "fixed" data
  float x_m =0.0;
  float y_m = 0.0;
  float z_m = 0.0;
  float temper = 0.0;

  int i = 0;
  while(i < 1000){
    if(i%100 == 0)
      printf("Iteration %d\n", i);
    x_m = hmc5983_get_magnetic_x();  
    //printf("The magnetic field in X is %f Gauss\n", x_m);
    x_m = x_m * 100.0;
    printf("The magnetic field in X is %.2f uT\n", x_m);
    
    y_m = hmc5983_get_magnetic_y();
    //printf("The magnetic field in Y is %f Gauss\n", y_m);
    y_m = y_m *100.0;
    printf("The magnetic field in Y is %.2f uT\n", y_m);
    
    z_m = hmc5983_get_magnetic_z();
    //printf("The magnetic field in Z is %f Gauss\n", z_m);
    z_m = z_m * 100.0;
    printf("The magnetic field in Z is %.2f uT\n\n", z_m);

    //temper = hmc5983_get_temp();
    //printf("The temperature is %f\n", temper);
    if(!isnan(x_m) && !isnan(y_m) && !isnan(z_m)){
      fprintf(fp, "%d, %f,%f,%f\n", i, x_m, y_m, z_m);
      i++;
    }

    //fprintf(gnuplotPipe, "%d %f, %d %f \n", i, x_m, i, y_m);
    usleep(30000);
    //sleep(1);
  }
  fclose(fp);
  
  //plot_data(filename, "x", "Iteration", "Magnetic Field (uT)", 1, 2);
  plot_all_data(filename);
  
  // CLose the I2C bus
  disconnect_i2c();
  
  return 0;
}
