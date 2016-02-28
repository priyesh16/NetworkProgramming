# Compilation options

CC=gcc

# Directories
CURDIR=$(shell pwd)
SRCDIR=$(CURDIR)/../
PARENTDIR=$(SRCDIR)/../
OBJDIR=obj

HEADERDIR=$(PARENTDIR)/head
LIBDIR=$(PARENTDIR)/lib

# get library and header and common files 
LIBS=$(wildcard $(LIBDIR)/*.a)
HEADERS=$(wildcard $(HEADERDIR)/*.h)

# get server src and obj files
SERVSRC=$(wildcard *.c)
SERVOBJ=$(patsubst %.c, $(OBJDIR)/%.o, $(SERVSRC))

CFLAGS=-I$(HEADERDIR) 
all: directories proxy 

directories: ${OBJDIR} 

${OBJDIR}: 
	mkdir -p ${OBJDIR}

# make server binary
proxy:$(SERVOBJ)
	$(CC) -pthread -Wall -g -o $@ $^ $(CFLAGS) $(LIBS)

$(OBJDIR)/%.o: %.c $(HEADERS) 
	$(CC) -pthread -Wall -g -c -o $@ $< $(CFLAGS)

# clean or clobber 

clobber clean:
	rm -f $(OBJDIR)/*.o proxy 
