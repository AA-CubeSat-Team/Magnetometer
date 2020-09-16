#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

// Calibration of magnetometer using a simple method

/* Assign a unique ID to this sensor at the same time */
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);

void displaySensorDetails(void)
{
  sensor_t sensor;
  mag.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" uT");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" uT");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" uT");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void setup(void)
{
#ifndef ESP8266
  while (!Serial);     // will pause Zero, Leonardo, etc until serial console opens
#endif
  Serial.begin(9600);
  Serial.println("Magnetometer Test"); Serial.println("");

  /* Enable auto-gain */
  mag.enableAutoRange(true);

  /* Initialise the sensor */
  if(!mag.begin())
  {
    /* There was a problem detecting the LSM303 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }

  /* Display some basic information on this sensor */
  displaySensorDetails();
}

void loop(void){


  float high_x = -INFINITY;
  float high_y = -INFINITY;
  float high_z = -INFINITY;

  float low_x = INFINITY;
  float low_y = INFINITY;
  float low_z = INFINITY;

  int i =0;

// Gets data from 10 measurements in order to determine offset and scale for x, y, and z directions

    while (i<10) {
      Serial.println(i);
      i = i +1;
      sensors_event_t event;
      mag.getEvent(&event);

      float x_m = event.magnetic.x;
      float y_m = event.magnetic.y;
      float z_m = event.magnetic.z;

       /* Display the results (magnetic vector values are in micro-Tesla (uT)) */
      Serial.print("X: "); Serial.print(x_m); Serial.print("  ");
      Serial.print("Y: "); Serial.print(y_m); Serial.print("  ");
      Serial.print("Z: "); Serial.print(z_m); Serial.print("  ");Serial.println("uT");

      if(low_x > x_m){
        low_x = x_m;
      }else if(high_x <= x_m){
        high_x = x_m;
      }

      if(low_y > y_m){
        low_y = y_m;
      }else if(high_y <= y_m){
        high_y = y_m;
      }

      if(low_z > z_m){
        low_z = z_m;
      }else if(high_z <= z_m){
        high_z = z_m;
      }
       
    delay(500);   
  }

// Calculates offset and scale in x, y, and z directions

  float offset_x = (high_x + low_x)/2;
  float offset_y = (high_y + low_y)/2;
  float offset_z = (high_z + low_z)/2;

  float avg_delta_x = (high_x - low_x)/2;
  float avg_delta_y = (high_y - low_y)/2;
  float avg_delta_z = (high_z - low_z)/2;

  float avg_delta = (avg_delta_x + avg_delta_y + avg_delta_z)/3;


  float scale_x = avg_delta/ avg_delta_x;
  float scale_y = avg_delta/ avg_delta_y;
  float scale_z = avg_delta/ avg_delta_z;

  Serial.print("The x offset is "); Serial.print(offset_x); Serial.print(" and x scale is "); Serial.println(scale_x);
  Serial.print("The y offset is "); Serial.print(offset_y); Serial.print(" and y scale is "); Serial.println(scale_y);
  Serial.print("The z offset is "); Serial.print(offset_z); Serial.print(" and z scale is "); Serial.println(scale_z);

  Serial.println("Calibrated Readings");
  Serial.println("*****************");
  Serial.println(" ");
  
// Gets the calibrated measurements and prints them out

  int j =0;
  while (j <1000) {
    j = j+1;

    sensors_event_t event;
    mag.getEvent(&event);

    float x_m = event.magnetic.x;
    float y_m = event.magnetic.y;
    float z_m = event.magnetic.z;

    x_m = (x_m - offset_x) * scale_x;
    y_m = (y_m - offset_y) * scale_y;
    z_m = (z_m - offset_z) * scale_z;

    Serial.print("The magnetic field in X is "); Serial.println(x_m); 
    Serial.print("The magnetic field in Y is "); Serial.println(y_m);
    Serial.print("The magnetic field in Z is "); Serial.println(z_m);
    Serial.println();
    
  delay(500);  
  }

  delay(500);
}
