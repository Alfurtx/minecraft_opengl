##
# Minecraft OpenGL
#
# @file
# @version 0.1

UNAME_S = $(shell uname -s)

# TODO(fonsi): añadir flag -O3 cuando sea la build final

# NOTE(fonsi): definido CGLM_ALL_UNALIGNED porque habia ciertos problemas con el alineamiento en MACOS

CC = clang
CFLAGS = -std=c11 -g -Wall -Wextra -Wpedantic -Wstrict-aliasing -DCGLM_ALL_UNALIGNED
CFLAGS += -Wno-unused-parameter -Wno-switch -Wno-unused-function
LDFLAGS = -lm -lglfw libs/glad/src/glad.o

ifeq ($(UNAME_S), Darwin)
	LDFLAGS += -framework OpenGL -framework IOKit -framework CoreVideo -framework Cocoa
endif

ifeq ($(UNAME_S), Linux)
	LDFLAGS += -ldl -lpthread -lGL -lX11 -lXrandr -lXi
endif

SRC = $(wildcard src/*.c) $(wildcard src/**/*.c) $(wildcard src/**/**/*.c) $(wildcard src/**/**/**/*.c)
OBJ = $(SRC:.c=.o)
BIN = bin

.PHONY: all clean

all: dirs libs app

run:
	$(BIN)/app

app: $(OBJ)
	cd libs/glad && $(CC) -o src/glad.o -Iinclude -c src/glad.c # lib compile esta aqui porque por alguna razon el makefile no detecta libs como dependencia
	$(CC) -o $(BIN)/app $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

libs:
	cd libs/glad && $(CC) -o src/glad.o -Iinclude -c src/glad.c

dirs:
	mkdir -p ./$(BIN)

clean:
	rm -rf $(OBJ) $(TARGET)

debug:
	gdb --tui -q $(BIN)/app

leak:
	valgrind --leak-check=full --show-leak-kinds=all ./$(BIN)/app
