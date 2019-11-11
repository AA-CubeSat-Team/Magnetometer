# HMC5983 with Raspberry Pi

Interfacing with the magnetometer HMC5983 with Raspberry Pi using I2C bus.    

##### Table of Contents
- [Hardware Setup](#hardware-setup)
- [Software Setup](#software-setup)
- [Makefile](#makefile)
- [Calibration](#calibration)
- [Brief overview of code](#brief-overview-of-code)
     * [Opening and Closing the I2C bus](#opening-and-closing-the-i2c-bus)
     * [Connect to HMC5983](#connect-to-hmc5983)
     * [Data Output Rate](#data-output-rate)
     * [Gain](#gain)
     * [Operating Mode](#operating-mode)
     * [Identification Register](#identification-register)
     * [Getting Measurement Reading](#getting-measurement-reading)
     * [Calibrating HMC5983](#calibrating-hmc5983)
- [Example](#example)
     * [Example 1](#example-1)
     * [Example 2](#example-2)
- [Useful Links](#useful-links)



## Hardware Setup

The image below shows the RPi gpio layout

![RPi gpio layout](https://github.com/hima1234/AACT_magnetometer/blob/master/RPi/HMC5983/image/gpio-numbers-pi2.png)

The following lines needs to be connected from the RPi to the HMC5983:

```
RPi         HMC5983

3.3V  <----> Vin
Ground<----> GND
GPIO3 <----> SCL
GPIO2 <----> SDA
```

[Back to Table of Contents](#table-of-contents)

## Software Setup

For the RPi to communicate to the HMC5983 through I2C, we used a Linux tool i2c-tool. To test if the i2c-tool is already installed, run the following code on the Linux terminal:

```
i2cdetect -y 1
```

and you should see something familiar being printed out

```
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
40: -- -- -- -- -- -- -- -- -- -- -- 1e -- -- -- --
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
70: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

```

If there's a hexadecimal value being printed out, then the i2c-tool was able to detect a slave chip on the I2C bus. The I2C slave address of the HMC5983 is 0x1E, so if you run the command above and see 0x1E then you were successfully able to connect to the HMC5983 throguh I2C. 

If Linux says that you don't have i2cdetect, then install 'i2c-tool'using the following commands:

```
sudo apt-get update
sudo apt-get install i2c-tools
```

[Back to Table of Contents](#table-of-contents)

## Makefile

Normally to compile a C program, you will have to write out the command in the terminal using gcc. For instance let's say you have a simple C file called "hello.c". Then you could write out the following command to build an executable file "hello.exe":

```
 gcc -o hello.exe hello.c
```

and then to run the executable file, you just need to write 

```
./hello.exe
```

in the terminal.

Writing out the gcc command isn't difficult if you're only compiling from a single file using standard C library. However, you'll need to write it out every time you want to recompile which can become bothersome, and if you have to link other files then the gcc command will becomes more complex. In order to make the compiling process easier, there's a "Makefile" which can help create the gcc command. Therefore, whenever you make change to the C program, you can recompile the program by typing in the terminal:

```
make
```

which will compile based on how you define Makefile. To clean any file that was made from Makefile, then run the command in the terminal:

```
make clean
```

A usefule link to learn more about Makefile is [here](http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/).

[Back to Table of Contents](#table-of-contents)

## Calibration

All magnetometer are affected by two different kind of bias: Hard-iron bias and Soft-iron bias. Hard-iron bias causes an offset to the magnetic intensity and are created by objects that produce a magnetic field. Such objects include a magnets, speaker, electricity in a wire, etc. Soft-iron bias will cause a distortion to the magnetic intensity and are created by ferromagnetic materials such as iron and nickel. To correct the error, we used a method mentioned [here](https://appelsiini.net/2018/calibrate-magnetometer/). There's a more sophisticated method of calibration by ellipsoid fitting which is mentioned [here](https://teslabs.com/articles/magnetometer-calibration/). 

To calibrate the error, we do need to record some measurements and the magnetometer must rotate around the x,y, and z axis multiple time. 

[Back to Table of Contents](#table-of-contents)

## Brief overview of code

In this section, sample codes are provided in how to communicate with the HMC5983 through the I2C bus. 

### Opening and Closing the I2C bus

In order to send read/write register to the slave chip, you'll need to open the I2C bus and after you're done, you'll need to close the I2C bus. The code snipet below shows how to open and close the I2C bus

```C
#include "../Lib/I2Cdrive.h"
#include "../Lib/externalLib.h"
#include "HMC5983.h"

int main(){
  // Open the I2C communication
  connect_i2c();
    
  // Close the I2C communication
  disconnect_i2c();
}
```

[Back to Table of Contents](#table-of-contents)

### Connect to HMC5983

To communicate with the HMC5983, you'll need to first set set the connection betweene the HMC5983 and I2C bus. The code snipets below show how to make that connection:

```C
  connect_2_hmc5983_via_I2C();
```
[Back to Table of Contents](#table-of-contents)

### Data Output Rate

HMC5983 is capable of changing the output rate of the requested data. To change the data output rate on the HMC5983, you'll need to write to the Configuration Register A on the chip, which has an address of 0x00. A function called `hmc5983_set_cra(reg_val)` is able to send `reg_val` to the Configuration Register A. The table below are some of the possible value with the associated data output rate

| reg_val   | Data rate (Hz)     | 
| :--------:|:-------------:| 
| 0x00      | .75 |
| 0x04      | 1.5 |
| 0x08      | 3   |
| 0x0C      | 7.5 |
| 0x10      | 15  |
| 0x14      | 30  |
| 0x18      | 75  |
| 0x1C      | 220 |

So if you want to set the data output rate to 15 Hz, then call the following function:

```C
hmc5983_set_cra(0x10);
```

If you want to know the current data output rate, the call the following function:

```C
unsigned char cra_val = hmc5983_get_cra();
```

Note that Configuration Register A is also responsible for setting the measurement mode, and adjusting this will add some bias to the data. I'm assuming that we will always be in normal measurement mode, but if you want to add some bias to the data, then please refer to the [datasheet](http://www.farnell.com/datasheets/1509871.pdf)

[Back to Table of Contents](#table-of-contents)

### Gain

HMC5983 is also capable of adjusting the gain setting, where you can adjust the measurement reading. The lower the gain, the higher the resolution. To change the gain setting on the chip, use the function `hmc5983_set_crb(reg_val)`. The table below shows the possible `reg_val` with the associated gain settings.


| reg_val   | Recommended Sensor Field (Gauss)   |  Digital Resolution (mG/LSB) |
| :--------:|:-------------:| :------------: |
| 0x00      | +-.88  | .73  |
| 0x20      | +-1.3  | .92  |
| 0x40      | +-1.9  | 1.22 |
| 0x60      | +-2.5  | 1.52 |
| 0x80      | +-4.0  | 2.27 |
| 0xA0      | +-4.7  | 2.56 |
| 0xC0      | +-5.6  | 3.03 |
| 0xE0      | +-8.1 | 4.35  |

So if you think you're in a sensor field range of +- 4.7 Guass, then run the following function:

```C
hmc5983_set_crb(0xA0);
```

To see the current gain setting, run the following function:

```C
unsigned char mode = hmc5983_get_crb();
```

[Back to Table of Contents](#table-of-contents)

### Operating Mode

There are 3 different operating mode on HMC5983: continuous-measurement mode, single-measurement mode, idle mode. The continuous-measurement mode will continuously perform measurement and gets ready for the data to be read by the RPi, even if the RPi hasn't requested any measurement. In single-measurement mode, HMC5983 will get a single measurement and then set's the operating mode to idle mode. In idle mode, no measurement are made. 

To set the operating mode of the chip, use the function `hmc5983_set_mode(reg_val)`. The table below shows the possible `reg_val` for the operating mode.

| reg_val | Operating Mode |
|:-------:|:--------:|
| 0x00 | Continuous-Measurement Mode |
| 0x01 | Single-Measurement Mode |
| 0x02 | Idle Mode |
|0x03 | Idle Mode |

So if you want to run the operating mode to be in continuous-measurement mode, then run the following function:

```C
hmc5983_set_mode(0x00);
```

If you want to get the current operating mode setting on the chip, then call the following function:

```C
unsigned char op_mode = hmc5983_get_mode();
```

[Back to Table of Contents](#table-of-contents)

### Identification Register

There are 3 different identification register that are used to identify the chip. The reading value is set, so if there's a discrepency then there's an issue with the chip. Identification register A should have a value of 0x48. Identification register B should have a value of 0x34. Identification register C should have a value of 0x33. 

This hasn't been implemented yet and need to be tested. 

 [Back to Table of Contents](#table-of-contents)

### Calibrating HMC5983

To calibrate HMC5983, call the following function:

```C
hmc5983_calibrate();
```

 [Back to Table of Contents](#table-of-contents)

### Getting Measurement Reading

Now we finally get to the point where we want to read the measurement data. HMC5983 is able to produce 3 measurement data in the X, Y, and Z. To read the magnetic intensity around the sensor, run the following function:

```C
flaot x = hmc5983_get_magnetic_x();  
flaot y = hmc5983_get_magnetic_y();  
flaot z = hmc5983_get_magnetic_z();   

```

**Note that the measurement are in Gauss.** To change the value in to micro-Tesla, multiply the value by 100. 
 
[Back to Table of Contents](#table-of-contents)
 
## Examples

There are some examples so that you can get used to the HMC5983. 

### Example 1

This is a very simple example of trying to open the I2C bus, connect to HMC5983, change the mode to continuous, gather some data, and then disconnect. To make and run the executable, type in the following command in the terminal:

```
make example1.exe
./example1.exe
```

[Back to Table of Contents](#table-of-contents)

### Example 2

This example shows how to change the operating mode on the chip. If the mode is on any other mode than single-measurement mode, then change it to single-measurement mode. If it's already in single-measurement mode, then do nothing. After getting the first data set, we get another. To make and run the executable, type in the following command in the terminal:

```
make example2.exe
./example2.exe
```

[Back to Table of Contents](#table-of-contents)

### Example 3

Going to implement the easy calibration method

### Example 4

Going to implement the more sophisticated calibration method

## Useful Links

* [C tutorial](https://www.tutorialspoint.com/cprogramming/index.htm)
* [Intro to I2C](https://learn.sparkfun.com/tutorials/i2c/all)
* [Makefile tutorial](http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/)
* [Another good makefile tutorial with gcc/g++](https://www3.ntu.edu.sg/home/ehchua/programming/cpp/gcc_make.html)
* [HMC5983 Datasheet](http://www.farnell.com/datasheets/1509871.pdf)
* [Magnetometer calibration](https://appelsiini.net/2018/calibrate-magnetometer/)
* [A more sophisticated calibration method](https://teslabs.com/articles/magnetometer-calibration/)

[Back to Table of Contents](#table-of-contents)
