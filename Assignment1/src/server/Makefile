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

# get server src and obj files
SERVSRC=$(wildcard *.c)
SERVOBJ=$(patsubst %.c, $(OBJDIR)/%.o, $(SERVSRC))

CFLAGS=-I$(HEADERDIR) -I$(COMMONDIR)
all: directories myserver

directories: ${OBJDIR} 

${OBJDIR}: 
	mkdir -p ${OBJDIR}

# make server binary
myserver:$(SERVOBJ) $(COMMON) 
	$(CC) -g -o $@ $^ $(CFLAGS) $(LIBS)

$(OBJDIR)/%.o: %.c $(HEADERS) 
	$(CC) -g -c -o $@ $< $(CFLAGS)

# clean or clobber 

clobber clean:
	rm -f $(OBJDIR)/*.o myserver 
