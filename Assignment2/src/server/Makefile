# Compilation options

CC=gcc

# Directories
CURDIR=$(shell pwd)
SRCDIR=$(CURDIR)/../
PARENTDIR=$(SRCDIR)/../
OBJDIR=obj

HEADERDIR=$(PARENTDIR)/head
LIBDIR=$(PARENTDIR)/lib
COMMONDIR=$(SRCDIR)/common
COMMONOBJDIR=$(SRCDIR)/common/obj

# get library and header and common files 
LIBS=$(wildcard $(LIBDIR)/*.a)
HEADERS=$(wildcard $(HEADERDIR)/*.h)
COMMON=$(wildcard $(COMMONOBJDIR)/*.o)

# get server src and obj files
SERVSRC=$(wildcard *.c)
SERVOBJ=$(patsubst %.c, $(OBJDIR)/%.o, $(SERVSRC))

CFLAGS=-I$(HEADERDIR) -I$(COMMONDIR)
all: directories server

directories: ${OBJDIR} 

${OBJDIR}: 
	mkdir -p ${OBJDIR}

# make server binary
server:$(SERVOBJ) $(COMMON) 
	$(CC) -Wall -g -o $@ $^ $(CFLAGS) $(LIBS)

$(OBJDIR)/%.o: %.c $(HEADERS) 
	$(CC) -Wall -g -c -o $@ $< $(CFLAGS)

# clean or clobber 

clobber clean:
	rm -f $(OBJDIR)/*.o server 
