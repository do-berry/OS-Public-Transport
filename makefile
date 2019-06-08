CC=g++
DEFINES =
CFLAGS = -std=c++11 -O0 -g
LIB = -lpthread -lncurses

main: Passenger.o Bus.o Screen.o Lights.o main.o
	$(CC) -o main main.o Passenger.o Bus.o Screen.o Lights.o $(LIB)

main.o: main.cpp Passenger.h Bus.h Screen.h Lights.h
	$(CC) -c $(DEFINES) $(CFLAGS) main.cpp -o main.o

Passenger.o: Passenger.cpp Passenger.h
	$(CC) -c $(DEFINES) $(CFLAGS) Passenger.cpp -o Passenger.o

Bus.o: Bus.cpp Bus.h
	$(CC) -c $(DEFINES) $(CFLAGS) Bus.cpp -o Bus.o

Screen.o: Screen.cpp Screen.h
	$(CC) -c $(DEFINES) $(CFLAGS) Screen.cpp -o Screen.o

Lights.o: Lights.cpp Lights.h
	$(CC) -c $(DEFINES) $(CFLAGS) Lights.cpp -o Lights.o
