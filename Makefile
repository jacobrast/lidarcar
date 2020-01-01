CC=gcc
DEPS=include
CFLAGS +=-I$(DEPS) -gdwarf-3
ODIR = build/obj
SDIR = src
TARGET = main
_OBJ = dict.o command.o cli.o stub.o test.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

include Makefile.datastructs

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $(ODIR)/$(@F) $< $(CFLAGS)

.DEFAULT $(TARGET): $(DATA_OBJ) $(OBJ)
	$(CC) -o $(TARGET) $(wildcard $(ODIR)/*.o) $(CFLAGS) -lm

.PHONY clean:
	rm $(TARGET)
	rm $(ODIR)/*.o
