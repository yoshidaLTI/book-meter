#compiler options
CC = gcc
CFLAGS = -Wall 
LDFLAGS = -lsqlite3

#path
TARGETDIR = ../bin/
OBJECTDIR = ../obj/

TARGET = $(addprefix $(TARGETDIR), book_meter)
SOURCES = book.c add_book.c list_book.c print_help.c update_progress.c
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
