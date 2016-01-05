# env variables

export PATH:=./bin/:$(PATH)

# Compilation options

CC=gcc

# Directories

CFLAGS=-I$(HEADERDIR) 
SRCDIR=src
OBJDIR=obj
BINDIR=bin

HEADERDIR=$(SRCDIR)/head
SRCCOMMONDIR=$(SRCDIR)/common
SRCSERVDIR=$(SRCDIR)/server
SRCCLIDIR=$(SRCDIR)/client

OBJCOMMONDIR=$(SRCDIR)/common
OBJSERVDIR=$(OBJDIR)/server
OBJCLIDIR=$(OBJDIR)/client

# get library and header and common files 
LIBS=
HEADERS=$(wildcard $(HEADERDIR)/*.h)
COMMON=$(wildcard $(COMMONDIR)/*.c)

# get server src and obj files
SERVSRC=$(wildcard $(SRCSERVDIR)/*.c)
SERVOBJ=$(patsubst $(SRCSERVDIR)/%.c, $(OBJSERVDIR)/%.o, $(SERVSRC))

# get client src and obj files
CLISRC=$(wildcard $(SRCCLIDIR)/*.c)
CLIOBJ=$(patsubst $(SRCCLIDIR)/%.c, $(OBJCLIDIR)/%.o, $(CLISRC))

all : $(BINDIR)/myserver $(BINDIR)/myclient

# make server binary
server $(BINDIR)/myserver:$(SERVOBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

client $(OBJSERVDIR)/%.o: $(SRCSERVDIR)/%.c $(HEADERS) $(COMMON)
	$(CC) -c -o $@ $< $(CFLAGS)

# make client binary

$(BINDIR)/myclient:$(CLIOBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

$(OBJCLIDIR)/%.o: $(SRCCLIDIR)/%.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

# clean or clobber 

clobber clean:
	rm -f $(OBJSERVDIR)/*.o $(OBJCLIDIR)/*.o 
