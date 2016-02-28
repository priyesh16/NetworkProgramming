# Directories

BINDIR=bin
SRCDIR=src
SERVDIR=$(SRCDIR)/proxy
MAKE=make
COPYFILES=$(BINDIR)/proxy $(BINDIR)/forbidden-sites

install: all directories $(COPYFILES) 

directories: ${BINDIR}

${BINDIR}:
	    mkdir -p ${BINDIR}

$(BINDIR)/proxy: src/proxy/proxy
$(BINDIR)/forbidden-sites: src/proxy/forbidden-sites

${BINDIR}/%:
	cp -f $< $@

# make all
all: proxy 

# make server binary and objs
proxy:
	$(MAKE) -C $(SERVDIR) all

# clean server binary and objs
cleanserver:
	$(MAKE) -C $(SERVDIR) clean 
	
# clean or clobber 

clobber clean: cleanserver cleanbin

cleanbin:
	rm -f $(BINDIR)/* 
