cc=gcc
CFLAGS=-I.
DEPS = ../lib/I2Cdrive.h BMI055.h ../lib/externalLib.h
OBJ = Main.o ../lib/I2Cdrive.o BMI055.o
OBJ2 = ../lib/I2Cdrive.o BMI055.o

%.o: %.c $(DEPS)
	$(cc) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(cc) -o $@ $^ $(CFLAGS)

clean:
	rm ./BMI055.o ./main_savedata.o
