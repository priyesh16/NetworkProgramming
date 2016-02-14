# Directories

BINDIR=bin
SRCDIR=src
SERVDIR=$(SRCDIR)/server
CLIDIR=$(SRCDIR)/client
COMMONDIR=$(SRCDIR)/common
MAKE=make
COPYFILES=$(BINDIR)/myserver $(BINDIR)/myclient $(BINDIR)/server-info.text

install: all directories $(COPYFILES) 

directories: ${BINDIR}

${BINDIR}:
	    mkdir -p ${BINDIR}

$(BINDIR)/myserver: src/server/server  
$(BINDIR)/myclient: src/client/client
$(BINDIR)/server-info.text: src/client/server-info.text

${BINDIR}/%:
	cp -f $< $@

# make all
all: common server client

# make common objs
common:
	$(MAKE) -C $(COMMONDIR) all
	
# clear common objs
cleancommon:
	$(MAKE) -C $(COMMONDIR) clean 

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

clobber clean: cleancommon cleanserver cleanclient cleanbin

cleanbin:
	rm -f $(BINDIR)/* 
