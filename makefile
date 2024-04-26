ROOTNAME=main
CC=gcc
CFLAGS= -g -I.
DEPS = sensorLib.h globals.h sensorThread.h
LIBS = -l pigpio -l pthread -lm

OBJ = $(ROOTNAME).o sensorLib.o sensorThread.o 
#TESTOPTIONS = --leak-check=full

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ROOTNAME): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
	
clean:
	rm *.o $(ROOTNAME)
	
run: $(ROOTNAME)
		./$(ROOTNAME) $(TESTOPTIONS)
		
vrun: $(ROOTNAME)
		valgrind ./$(ROOTNAME) $(TESTOPTIONS)
		
