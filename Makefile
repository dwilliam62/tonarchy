CC = gcc
CFLAGS = -std=c23 -Wall -Wextra -O2
LDFLAGS =
STATIC_LDFLAGS = -static

TARGET = tonarchy
SRC = tonarchy.c

.PHONY: all clean install static

all: $(TARGET)

static: $(TARGET)-static

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

$(TARGET)-static: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)-static $(STATIC_LDFLAGS)

clean:
	rm -f $(TARGET) $(TARGET)-static

install: $(TARGET)
	install -Dm755 $(TARGET) /usr/local/bin/$(TARGET)
	cp -r packages /usr/share/tonarchy/
	cp -r configs /usr/share/tonarchy/
