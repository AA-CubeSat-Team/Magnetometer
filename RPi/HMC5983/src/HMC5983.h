#ifndef HMC5983_H
#define HMC5983_H

#define HMC5983Address  0x1E

#define HMC5983ConfRegA 0x00
#define HMC5983ConfRegB 0x01
#define HMC5983ModeReg  0x02

#define HMC5983Gain08   0x00
#define HMC5983Gain13   0x20
#define HMC5983Gain19   0x40
#define HMC5983Gain25   0x60
#define HMC5983Gain40   0x80
#define HMC5983Gain47   0xA0
#define HMC5983Gain56   0xC0
#define HMC5983Gain81   0xE0

#define HMC5983XMSB     0x03
#define HMC5983XLSB     0x04
#define HMC5983YMSB     0x07
#define HMC5983YLSB     0x08
#define HMC5983ZMSB     0x05
#define HMC5983ZLSB     0x06

#define HMC5983STATUS   0x09

#define HMC5983TempMSB  0x31
#define HMC5983TempLSB  0x32

struct hmc5983_Var3{
  float x;
  float y;
  float z;
};


int connect_2_hmc5983_via_I2C();
int hmc5983_set_cra(unsigned char craSetting);
unsigned char hmc5983_get_cra();
unsigned char hmc5983_get_mode();
int hmc5983_set_mode(unsigned char deviceMode);
unsigned char hmc5983_get_crb();
int hmc5983_set_crb(unsigned char gainSetting);
float hmc5983_get_raw_magnetic_x(unsigned char apply_gain);
float hmc5983_get_magnetic_x();
float hmc5983_get_raw_magnetic_y(unsigned char apply_gain);
float hmc5983_get_magnetic_y();
float hmc5983_get_raw_magnetic_z(unsigned char apply_gain);
float hmc5983_get_magnetic_z();
int hmc5983_get_status();
float hmc5983_get_temp();
void hmc5983_calibrate();
void hmc5983_get_calibrated_value_from_file();
void hmc5983_set_calibration_offset(float offset_x, float offset_y, float offset_z);
void hmc5983_set_calibration_scale(float magscale_x, float magscale_y, float magscale_z);


#endif
