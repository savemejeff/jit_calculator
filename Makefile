CC      := cc
CFLAGS  := -Wall -Wextra
LIBS    := -lm

BIN     := bin
TARGET  := $(BIN)/calculator
SOURCES := $(wildcard *.c)
HEADERS := $(wildcard *.h)
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
