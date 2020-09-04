#ifndef EXTERNAL_LIB_H
#define EXTERNAL_LIB_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
//
#ifdef __linux__
	#include <linux/i2c-dev.h>
#else
	#include "linux/i2c-dev.h"
#endif
//
#include <math.h>
#include <signal.h>

#endif
