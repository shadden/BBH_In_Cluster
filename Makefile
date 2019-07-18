CC=gcc
CFLAGS= -std=c99 -O3 -fPIC -Wall
SOURCES=example_force.c plummer.c
LIB=libforces.so
REBOUNDDIR=/Users/shadden/Projects/00_Codes_And_Data/rebound/src
REBOUNDXDIR=/Users/shadden/Projects/00_Codes_And_Data/reboundx/src
OBJECTS=$(SOURCES:.c=.o)

all: $(SOURCES) libforces.so

libforces.so: $(OBJECTS)
	@echo ""
	@echo "Linking shared library $@ ..."
	$(CC) $(CFLAGS) -shared $(OBJECTS) -Wl,-rpath,. -Wl,-rpath,$(REBOUNDXDIR) -L $(REBOUNDXDIR) -lreboundx -lrebound  -o $@
	@echo ""
	@echo "The shared library $< has been created successfully."

%.o: %.c
	@echo "Compiling source file $< ..."
	$(CC) -c -o $@ $< $(CFLAGS) -I $(REBOUNDDIR) -I $(REBOUNDXDIR) 

clean:
	@echo "Removing object files *.o ..."
	@-rm -f *.o
	@echo "Removing shared library libforces.so ..."
	@-rm -f *.so
