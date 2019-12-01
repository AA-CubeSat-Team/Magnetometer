cc=gcc
CFLAGS=-I.
DEPS = ./Lib/I2Cdrive.h ./src/HMC5983.h ./Lib/externalLib.h
OBJ = ./src/Main.o ./Lib/I2Cdrive.o ./src/HMC5983.o 
OBJ2 = ./src/example1.o ./Lib/I2Cdrive.o ./src/HMC5983.o
OBJ3 = ./src/example2.o ./Lib/I2Cdrive.o ./src/HMC5983.o 
OBJ4 = ./src/example3.o ./Lib/I2Cdrive.o ./src/HMC5983.o
OBJ5 = ./src/example4.o ./Lib/I2Cdrive.o ./src/HMC5983.o
OBJ6 = ./src/example5.o ./Lib/I2Cdrive.o ./src/HMC5983.o
OBJ7 = ./src/example6.o ./Lib/I2Cdrive.o ./src/HMC5983.o

QUICKHULL = ./src/quickhull.c 

%.o: %.c $(DEPS)
	$(cc) -c -o $@ $< $(CFLAGS)

all: main.exe example1.exe example2.exe example3.exe example4.exe example5.exe example6.exe

main.exe: $(OBJ)
	$(cc) -o $@ $^ $(CFLAGS)

example1.exe: $(OBJ2)
	$(cc) -o $@ $^ $(CFLAGS)

example2.exe: $(OBJ3)
	$(cc) -o $@ $^ $(CFLAGS)

example3.exe: $(OBJ4)
	$(cc) -o $@ $^ $(CFLAGS)

example4.exe: $(OBJ5)
	$(cc) -o $@ $^ $(CFLAGS)

example5.exe: $(OBJ6)
	$(cc) -o $@ $^ $(CFLAGS)

example6.exe: $(OBJ7)
	$(cc) -o $@ $^ $(CFLAGS)

quickhull.exe: $(QUICKHULL)
	$(cc) -o $@ $^ $(CFLAGS)

clean :
	rm ./src/Main.o ./src/example1.o ./src/example2.o ./src/example3.o ./src/HMC5983.o ./Lib/I2Cdrive.o example1.exe example2.exe example3.exe example4.exe example5.exe example6.exe quickhull.exe main.exe 
