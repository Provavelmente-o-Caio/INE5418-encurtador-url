CC      = gcc
CFLAGS  = -Wall -Wextra -Wpedantic -std=c11 -g
TARGET  = urlshort
LIB_OBJ = urlshort_client.o

.PHONY: all clean

all: $(TARGET)

# Compila a biblioteca (objeto)
urlshort_client.o: urlshort_client.c urlshort_client.h
	$(CC) $(CFLAGS) -c urlshort_client.c -o urlshort_client.o

# Linka o demo com a biblioteca
$(TARGET): main.c $(LIB_OBJ) urlshort_client.h
	$(CC) $(CFLAGS) main.c $(LIB_OBJ) -o $(TARGET)

clean:
	rm -f $(LIB_OBJ) $(TARGET)