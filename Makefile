ifeq ($(shell uname),Darwin)
    LDFLAGS := -Wl,-dead_strip -ldl
else
    LDFLAGS := -Wl,--gc-sections -lpthread -ldl
endif

all: clean target/server
	./main

target:
	mkdir -p $@

target/server: target/glue.o target/debug/libserver.a
	dmd src/main.d $^ -L--no-as-needed -L-lpthread -L-ldl
	

target/debug/libserver.a: src/lib.rs Cargo.toml
	cargo build

target/glue.o: src/glue.c | target
	$(CC) -o $@ -c $<

clean:
	rm -rf target