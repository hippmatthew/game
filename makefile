CC = clang
CFLAGS = -I.

SRCS = $(wildcard src/*.c)
BINS = $(patsubst src/%.c, bin/%.o, $(SRCS))

TARGET = play

build: $(TARGET)

$(TARGET): $(BINS)
	$(CC) -o $@ $(CFLAGS) $(BINS)

bin/%.o: src/%.c
	$(CC) -o $@ $(CFLAGS) -c $^

clean:
	rm -f bin/* $(TARGET)