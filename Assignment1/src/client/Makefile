# Compilation options

CC=gcc

# Directories
CURDIR=$(shell pwd)
SRCDIR=$(CURDIR)/../
OBJDIR=obj

HEADERDIR=$(SRCDIR)/head
COMMONDIR=$(SRCDIR)/common
COMMONOBJDIR=$(SRCDIR)/common/obj

# get library and header and common files 
LIBS=
HEADERS=$(wildcard $(HEADERDIR)/*.h)
COMMON=$(wildcard $(COMMONOBJDIR)/*.o)

# get client src and obj files
CLISRC=$(wildcard *.c)
CLIOBJ=$(patsubst %.c, $(OBJDIR)/%.o, $(CLISRC))

CFLAGS=-I$(HEADERDIR) -I$(COMMONDIR)

all: directories myclient

directories: ${OBJDIR} 

${OBJDIR}: 
	mkdir -p ${OBJDIR}

# make client binary
myclient:$(CLIOBJ) $(COMMON) 
	$(CC) -g -o $@ $^ $(CFLAGS) $(LIBS)

$(OBJDIR)/%.o: %.c $(HEADERS) 
	$(CC) -g -c -o $@ $< $(CFLAGS)

# clean or clobber 

clobber clean:
	rm -f $(OBJDIR)/*.o myclient 
