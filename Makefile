CC      := cc
CFLAGS  := -Wall -Wextra -g
LIBS    := -lm
ARCH    := native

ifeq ($(ARCH), x86_64)
	CFLAGS += -target x86_64-apple-darwin
endif

BIN     := bin
TARGET  := $(BIN)/calculator
SOURCES := compiler.c main.c misc.c scanner.c
SOURCES += gen_x64.c
HEADERS := codegen.h common.h compiler.h misc.h scanner.h
OBJECTS := $(addprefix $(BIN)/, $(SOURCES:.c=.o))

$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(OBJECTS): $(BIN)/%.o: %.c $(HEADERS) $(BIN)
	@mkdir -p $(BIN)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN):
	mkdir -p $(BIN)

test: $(TARGET)
	(cd tests; ./runtests.sh)

clean:
	rm -f $(OBJECTS) $(TARGET)
