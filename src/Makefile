#compiler options
CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lsqlite3

#path
TARGETDIR = ../bin/
OBJECTDIR = ../obj/

TARGET = $(addprefix $(TARGETDIR), book_meter)
SOURCES = book.c
OBJECTS = $(addprefix $(OBJECTDIR), $(SOURCES:.c=.o))

all: $(TARGETDIR) $(OBJECTDIR) $(TARGET)

$(OBJECTDIR)%.o : %.c
	        $(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

$(TARGET): $(OBJECTS)
	        $(CC) $^ $(LDFLAGS) $(INCLUDE) $(LIBRARY) -o $@

$(TARGETDIR):
	        mkdir ../bin

$(OBJECTDIR):
	        mkdir ../obj

clean:
	        rm -rf $(OBJECTDIR) $(TARGETDIR)
