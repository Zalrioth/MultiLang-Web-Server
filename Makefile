ifeq ($(shell uname),Darwin)
    LDFLAGS := -Wl,-dead_strip
else
    LDFLAGS := -Wl,--gc-sections -lpthread -ldl
endif

all: clean target/server
	target/server

target:
	mkdir -p $@

target/server: target/main.o target/debug/libserver.a
	dmd -of$@ src/cache.d src/module.d $^ -L--no-as-needed -L-lpthread -L-ldl

target/debug/libserver.a: src/lib.rs Cargo.toml
	cargo build

target/main.o: src/main.c | target
	$(CC) -o $@ -c $<

clean:
	rm -rf target