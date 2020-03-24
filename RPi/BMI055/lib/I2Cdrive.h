#ifndef I2C_COMMAND_H
#define I2C_COMMAND_H

int connect_i2c();
int disconnect_i2c();
int i2c_connect_2_slave(int addr);
//int readBytes(int n);
int i2c_get_register_val(unsigned char reg);
int i2c_set_register_val(unsigned char reg, unsigned char data);
//int writeBytes(unsigned char* write_buffer, int n);	

unsigned char i2c_get_buffer_index(int i);

#endif
