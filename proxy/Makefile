CC     = clang
CFLAGS = -Wall -Wextra -pthread -Isrc
SRC    = $(wildcard src/*.c)

proxy: $(SRC)
	$(CC) $(CFLAGS) -o proxy $(SRC)

clean:
	rm -f proxy
