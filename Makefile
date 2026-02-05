CC      := cc
CFLAGS  := -Wall -Wextra -g
LIBS    := -lm
ARCH    := x86_64

BIN     := bin
TARGET  := $(BIN)/calculator
SOURCES := compiler.c main.c misc.c scanner.c
ifeq ($(ARCH), x86_64)
	SOURCES += gen_x64.c
endif
ifeq ($(ARCH), arm)
	SOURCES += gen_aarch64.c
endif

HEADERS := codegen.h common.h compiler.h misc.h scanner.h
OBJECTS := $(addprefix $(BIN)/, $(SOURCES:.c=.o))

$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(OBJECTS): $(BIN)/%.o: %.c $(HEADERS)
	@mkdir -p $(BIN)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN):
	mkdir -p $(BIN)

test: $(TARGET)
	(cd tests; ./runtests.sh)

clean:
	rm -f $(OBJECTS) $(TARGET)
