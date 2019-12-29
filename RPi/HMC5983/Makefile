cc=gcc
CFLAGS=-Wall -g
DEPS = ./lib/I2Cdrive.h ./src/HMC5983.h ./lib/commonLib.h
OBJ = ./src/Main.o ./lib/I2Cdrive.o ./src/HMC5983.o 
EX1 = ./src/example1.o ./lib/I2Cdrive.o ./src/HMC5983.o 
EX2 = ./src/example2.o ./lib/I2Cdrive.o ./src/HMC5983.o 
EX3 = ./src/example3.o ./lib/I2Cdrive.o ./src/HMC5983.o 
EX4 = ./src/example4.o ./lib/I2Cdrive.o ./src/HMC5983.o 
EX5 = ./src/example5.o ./lib/I2Cdrive.o ./src/HMC5983.o 
EX6 = ./src/example6.o ./lib/I2Cdrive.o ./src/HMC5983.o 

%.o: %.c $(DEPS)
	$(cc) $(CFLAGS) -c -o $@ $< 

all: example1.exe example2.exe example3.exe example4.exe example5.exe example6.exe

#main.exe: $(OBJ)
#	$(cc) -o $@ $^ -lm 

example1.exe: $(EX1)
	$(cc) $(CFLAGS) -o $@ $^ -lm

example2.exe: $(EX2)
	$(cc) $(CFLAGS) -o $@ $^  -lm

example3.exe: $(EX3)
	$(cc) $(CFLAGS) -o $@ $^ -lm

example4.exe: $(EX4)
	$(cc) $(CFLAGS) -o $@ $^ -lm 

example5.exe: $(EX5)
	$(cc) $(CFLAGS) -o $@ $^ -lm

example6.exe: $(EX6)
	$(cc) $(CFLAGS) -o $@ $^ -lgsl -lm

clean :
	rm ./src/example1.o ./src/example2.o ./src/example3.o ./src/example4.o ./src/example5.o ./src/example6.o ./src/HMC5983.o ./lib/I2Cdrive.o example1.exe example2.exe example3.exe example4.exe example5.exe example6.exe 
