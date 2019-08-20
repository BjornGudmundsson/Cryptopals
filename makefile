CC = g++
CFLAGS = -g -Wall

default: crypto

crypto: main.o b64.o encoding.o frequency.o vigenere.o modes.o breaks.o parsing.o
	$(CC) $(CFLAGS) -o crypto main.o b64.o math.o frequency.o vigenere.o modes.o breaks.o parsing.o -lcryptopp


main.o: main.cpp headers/b64.hpp headers/math.hpp headers/frequency.hpp headers/modes.hpp headers/breaks.hpp headers/parsing.hpp
	$(CC) $(CFLAGS) -c main.cpp

b64.o: src/b64.cpp headers/b64.hpp
	$(CC) $(CFLAGS) -c src/b64.cpp

encoding.o: src/math.cpp headers/math.hpp
	$(CC) $(CFLAGS) -c src/math.cpp
frequency.o: src/frequency.cpp headers/frequency.hpp
	$(CC) $(CFLAGS) -c src/frequency.cpp

vigenere.o: src/vigenere.cpp headers/vigenere.hpp
	$(CC) $(CFLAGS) -c src/vigenere.cpp

modes.o: src/modes.cpp headers/modes.hpp
	$(CC) $(CFLAGS) -c src/modes.cpp

breaks.o: src/breaks.cpp headers/breaks.hpp
	$(CC) $(CFLAGS) -c src/breaks.cpp

parsing.o: src/parsing.cpp headers/parsing.hpp 
	$(CC) $(CFLAGS) -c src/parsing.cpp

clean:
	$(RM) *.o ./crypto main *.out