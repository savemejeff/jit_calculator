CC      := cc
CFLAGS  := -Wall -Wextra
LIBS    := -lm

TARGET  := calculator
SOURCES := $(wildcard *.c)
HEADERS := $(wildcard *.h)
OBJECTS := $(SOURCES:.c=.o)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^ $(LIBS)

$(OBJECTS): %.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJECTS) $(TARGET)
