cc=gcc
CFLAGS=-I.
DEPS = ./Lib/I2Cdrive.h ./src/HMC5983.h ./Lib/externalLib.h
OBJ = ./src/Main.o ./Lib/I2Cdrive.o ./src/HMC5983.o 
OBJ2 = ./src/example1.o ./Lib/I2Cdrive.o ./src/HMC5983.o
OBJ3 = ./src/example2.o ./Lib/I2Cdrive.o ./src/HMC5983.o 

%.o: %.c $(DEPS)
	$(cc) -c -o $@ $< $(CFLAGS)

all: main.exe example1.exe example2.exe

main.exe: $(OBJ)
	$(cc) -o $@ $^ $(CFLAGS)

example1.exe: $(OBJ2)
	$(cc) -o $@ $^ $(CFLAGS)

example2.exe: $(OBJ3)
	$(cc) -o $@ $^ $(CFLAGS)

clean :
	rm ./src/Main.o ./src/example1.o ./src/example2.o ./src/HMC5983.o ./Lib/I2Cdrive.o example1.exe example2.exe main.exe 
