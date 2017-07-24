CC=gcc
LDFLAGS := -L--no-as-needed
LDLIBS := -L-lpthread -L-ldl
CFILES := $(wildcard src/*/*.c)
OBJS := $(CFILES:%.c=%)

all: clean target target/server
	target/server

target:
	mkdir -p $@

target/server: target/debug/libserver.a $(OBJS)
	dmd -of$@ src/init/init.d $(wildcard target/*.o) $< $(LDFLAGS) $(LDLIBS)

$(OBJS):
	$(CC) -o target/$(notdir $@).o -c $@.c -Wall

target/debug/libserver.a: src/lib.rs Cargo.toml
	cargo build

clean:
	rm -rf target