CC = gcc
#DEBUG = true
DEBUG = false
LIBS = -lmpdclient
BIN = mpd-ralbum
OBJS = mpd-ralbum.o main.o 
PREFIX = /usr/local

ifeq ($(DEBUG), true)
	CFLAGS = -Wall -W -g -D __DEBUG
else
	CFLAGS = -Wall -W -O2 -g
endif

all: $(BIN)

clean :
	rm -rf $(OBJS) $(BIN)

install : 
	cp $(BIN) $(PREFIX)/bin

uninstall :
	rm $(PREFIX)/bin/$(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) $^ -o $(BIN) 

%.o : %.c %.h
	$(CC) $(CFLAGS) -c $<
