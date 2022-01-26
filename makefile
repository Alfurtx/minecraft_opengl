##
# Minecraft OpenGL
#
# @file
# @version 0.1

UNAME_S = $(shell uname -s)

CC = clang
CFLAGS = -std=c11 -O3 -g -Wall -Wextra -Wpedantic -Wstrict-aliasing
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

all: dirs libs app
	$(BIN)/app

app: $(OBJ)
	$(CC) -o $(BIN)/app $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

dirs:
	mkdir -p ./$(BIN)

libs:
	cd libs/glad && $(CC) -o src/glad.o -Iinclude -c src/glad.c

clean:
	rm -rf $(OBJ) $(TARGET)


# end
