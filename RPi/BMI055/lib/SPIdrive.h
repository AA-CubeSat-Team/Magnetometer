#ifndef SPI_DRIVE
#define SPI_DRIVE

int connect_spi();
int disconnect_spi();
int spi_connect_2_slave(int addr);
unsigned char spi_get_reg(unsigned char reg);
void spi_set_reg(unsigned char reg, unsigned char data);

#endif
