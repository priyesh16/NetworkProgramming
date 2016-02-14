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

# get client src and obj files
CLISRC=$(wildcard *.c)
CLIOBJ=$(patsubst %.c, $(OBJDIR)/%.o, $(CLISRC))

CFLAGS=-I$(HEADERDIR) -I$(COMMONDIR)

all: directories client

directories: ${OBJDIR} 

${OBJDIR}: 
	mkdir -p ${OBJDIR}

# make client binary
client:$(CLIOBJ) $(COMMON) 
	$(CC) -Wall -g -pthread -o $@ $^ $(CFLAGS) $(LIBS)

$(OBJDIR)/%.o: %.c $(HEADERS) 
	$(CC) -Wall -g -pthread -c -o $@ $< $(CFLAGS)

# clean or clobber 

clobber clean:
	rm -f $(OBJDIR)/*.o client 
