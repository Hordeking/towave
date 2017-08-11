PROGRAMFLAGS=-DNDEBUG -DNEWMODE
GME_DIR=gme
GME_FILES=$(GME_DIR)/*.c $(GME_DIR)/*.cpp
GME_OBJ=*.o
GME_STATICLIB=gme.a
WAVE_WRITER_C=wave_writer.c
TOWAVE=towave.cpp
EXECUTABLE=towave
PLAYER=player/*.cpp
CC=g++ #i586-mingw32msvc-g++

.PHONY: clean cleaner veryclean

towave: *.cpp *.c gme.a
	@echo Building toWave.;
	@$(CC) -o towave $(WAVE_WRITER_C) $(PROGRAMFLAGS) $(TOWAVE) $(GME_STATICLIB)

#player:
#	$(CC) -o sdl-test $(GME_C) -DNDEBUG $(PLAYER)

#gme: $(addprefix $(GME_DIR)/,*.c *.cpp) | gme
#	cd gme;
#	$(CC) $(PROGRAMFLAGS) -c *.cpp *.c

#$(GME_DIR)/%.o: $(GME_DIR)/%.c $(GME_DIR)/%.cpp
#	cd $(GME_DIR)
#	$(CC) $(PROGRAMFLAGS) -c *.cpp *.c

engine: $(GME_DIR)/*.c $(GME_DIR)/*.cpp
	@$(MAKE) -C gme --no-print-directory
	
gme.a: $(GME_DIR)/*.c $(GME_DIR)/*.cpp
	@echo Building GME Engine.;
	@g++ $(PROGRAMFLAGS) -c $(GME_FILES);
	@ar rvs gme.a *.o > /dev/null && echo Created GME Static Library.;
	@rm -f $(GME_OBJ);
	
clean:
	rm -f gme/*.o demo/*.o player/*.o *.o
	
cleaner:
	rm -f gme/*.o demo/*.o player/*.o *.o *.a
	
veryclean:
	rm -f gme/*.o demo/*.o player/*.o *.o *.a towave
