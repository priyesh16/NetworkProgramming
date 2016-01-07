# Directories

BINDIR=bin
SRCDIR=src
SERVDIR=$(SRCDIR)/server
CLIDIR=$(SRCDIR)/client
MAKE=make
COPYFILES=$(BINDIR)/myserver $(BINDIR)/myclient

install: all directories $(COPYFILES) 

directories: ${BINDIR}

${BINDIR}:
	    mkdir -p ${BINDIR}

$(BINDIR)/myserver: src/server/myserver  
$(BINDIR)/myclient: src/client/myclient

${BINDIR}/%:
	cp -f $< $@

# make all
all: server client

# make server binary and objs
server:
	$(MAKE) -C $(SERVDIR) all

# clean server binary and objs
cleanserver:
	$(MAKE) -C $(SERVDIR) clean 
	
# make client binary and objs
client:
	$(MAKE) -C $(CLIDIR) all 

# clear client binary and objs
cleanclient:
	$(MAKE) -C $(CLIDIR) clean 

# clean or clobber 

clobber clean: cleanserver cleanclient cleanbin

cleanbin:
	rm -f $(BINDIR)/* 
