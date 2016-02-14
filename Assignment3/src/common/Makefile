# Compilation options

CC=gcc

# Directories
CURDIR=$(shell pwd)
SRCDIR=$(CURDIR)/../
OBJDIR=obj

HEADERDIR=$(PARENTDIR)/head
COMMONOBJDIR=$(SRCDIR)/common/obj

# get library and header and common files 
LIBS=
HEADERS=$(wildcard $(HEADERDIR)/*.h)

COMMONSRC=$(wildcard *.c)
COMMONOBJ=$(patsubst %.c, $(OBJDIR)/%.o, $(COMMONSRC))


CFLAGS=-I$(HEADERDIR) -I $(CURDIR)

all: directories $(COMMONOBJ)

directories: ${OBJDIR} 

${OBJDIR}: 
	mkdir -p ${OBJDIR}

# make common objs 
$(OBJDIR)/%.o: %.c $(HEADERS) 
	$(CC) -g -c -o $@ $< $(CFLAGS)

# clean or clobber 

clobber clean:
	rm -f $(OBJDIR)/*.o 
