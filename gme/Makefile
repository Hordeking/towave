PROGRAMFLAGS=-DNDEBUG -DNEWMODE
CC=g++ #i586-mingw32msvc-g++

.PHONY: clean

gme: *.o
	@echo GME Built.

%.o: %.c %.cpp
	@echo Building the Game Music Engine.;
	@$(CC) $(PROGRAMFLAGS) -c *.cpp *.c
	
clean:
	rm -f *.o
