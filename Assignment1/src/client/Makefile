# Compilation options

CC=gcc

# Directories
CURDIR=$(shell pwd)
SRCDIR=$(CURDIR)/../
OBJDIR=obj

HEADERDIR=$(SRCDIR)/head
COMMONOBJDIR=$(SRCDIR)/common/obj

# get library and header and common files 
LIBS=
HEADERS=$(wildcard $(HEADERDIR)/*.h)
COMMON=$(wildcard $(COMMONOBJDIR)/*.o)

# get client src and obj files
CLISRC=$(wildcard *.c)
CLIOBJ=$(patsubst %.c, $(OBJDIR)/%.o, $(CLISRC))

CFLAGS=-I$(HEADERDIR) 

all: directories myclient

directories: ${OBJDIR} 

${OBJDIR}: 
	mkdir -p ${OBJDIR}

# make client binary
myclient:$(CLIOBJ) $(COMMON) 
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

$(OBJDIR)/%.o: %.c $(HEADERS) 
	$(CC) -c -o $@ $< $(CFLAGS)

# clean or clobber 

clobber clean:
	rm -f $(OBJDIR)/*.o myclient 
