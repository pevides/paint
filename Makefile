TARGET = paint

CC = gcc

CFLAGS = -Wall -Wextra

SDL_CFLAGS = $(shell sdl2-config --cflags)
SDL_LIBS = $(shell sdl2-config --libs)

SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC) paint.h
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(SDL_CFLAGS) $(SDL_LIBS)

clean:
	rm -f $(TARGET) canvas.bmp
	rm -rf temp

run: $(TARGET)
	./$(TARGET)