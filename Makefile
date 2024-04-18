CC = gcc
CFLAGS = -std=c17 -Wall -Wextra -Wpedantic
LDFLAGS =
OBJFILES = pc.c eventbuf.c
TARGET = pc

.PHONY: clean debug

all: $(TARGET)

debug: CFLAGS += -g
debug: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f $(OBJFILES) $(TARGET)