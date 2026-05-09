CC      := $(or $(shell which clang 2>/dev/null), $(shell which gcc 2>/dev/null))
ifeq ($(CC),)
  $(error Nenhum compilador encontrado: instale clang ou gcc)
endif
CFLAGS = -Wall -Wextra -pthread -Isrc
SRC    = $(wildcard src/*.c)

proxy: $(SRC)
	$(CC) $(CFLAGS) -o proxy $(SRC)

clean:
	rm -f proxy