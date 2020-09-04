cc=gcc
CFLAGS=-Wall -g -std=c99
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

all: example1 example2 example3 example4 example5 example6

#main.exe: $(OBJ)
#	$(cc) -o $@ $^ -lm 

example1: $(EX1)
	$(cc) $(CFLAGS) -o $@ $^ -lm

example2: $(EX2)
	$(cc) $(CFLAGS) -o $@ $^  -lm

example3: $(EX3)
	$(cc) $(CFLAGS) -o $@ $^ -lm

example4: $(EX4)
	$(cc) $(CFLAGS) -o $@ $^ -lm 

example5: $(EX5)
	$(cc) $(CFLAGS) -o $@ $^ -lm

example6: $(EX6)
	$(cc) $(CFLAGS) -o $@ $^ -lgsl -lm

clean :
	rm ./src/example1.o ./src/example2.o ./src/example3.o ./src/example4.o ./src/example5.o ./src/example6.o ./src/HMC5983.o ./lib/I2Cdrive.o example1 example2 example3 example4 example5 example6 
