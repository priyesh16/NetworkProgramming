CC=gcc
IDIR=head
CFLAGS=-I$(IDIR) 
SRCDIR=src
OBJDIR=obj
BINDIR=bin

DEPS= head/hellomake.h
OBJ = hellomake.o hellofunc.o 
$(warning OBJ =  $(OBJ))
LIBS=
SRC=$(wildcard $(SRCDIR)/*.c)
OBJ=$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))
$(warning SOURCES =  $(SRC))
$(warning OBJECTS =  $(OBJ))

$(BINDIR)/hellomake:$(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(OBJDIR)/*.o *.o 
