##
# Minecraft OpenGL
#
# @file
# @version 0.1

CC = cc
DEBUG_FLAGS = -Wall -Wextra -O3 -g -Wno-unused-parameter -Wno-switch
OPENGL_FLAGS = glfw GL X11 pthread Xrandr Xi dl m
TARGET = app

SRCDIR = src
INCDIR = src/include
OBJDIR = build

CFILES = $(wildcard $(SRCDIR)/*.c)
CFILES_NODIR = $(notdir $(CFILES))
OFILES = $(CFILES_NODIR:%.c=$(OBJDIR)/%.o)

INCFLAGS = $(addprefix -I, $(INCDIR)) $(addprefix -l, $(OPENGL_FLAGS)) $(addprefix -I, $(LIBDIR)/$(INCDIR))
CFLAGS = $(DEBUG_FLAGS) $(INCFLAGS)

VPATH = $(SRCDIR) $(LIBDIR)/$(SRCDIR)

.PHONY = build clean

all: $(TARGET)
	@./$(TARGET)

$(TARGET): $(OFILES)
	$(CC) -o $@ $^ $(CFLAGS)

$(OBJDIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

mac:
	$(CC) -o $(TARGET) $(CFILES) -Isrc/include -Wall -Wextra -O3 -g -framework OpenGL -framework IOKit -framework CoreVideo -framework Cocoa -lglfw
	@./$(TARGET)

clean:
	@rm -rf $(OBJDIR)/*
	@rm $(TARGET)

# end
