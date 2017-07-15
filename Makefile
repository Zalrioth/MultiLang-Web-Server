LDFLAGS := -L--no-as-needed
LDLIBS := -L-lpthread -L-ldl

all: clean target/server
	target/server

target:
	mkdir -p $@

target/server: target/main.o target/debug/libserver.a
	dmd -of$@ src/init.d $^ $(LDFLAGS) $(LDLIBS)

target/debug/libserver.a: src/lib.rs Cargo.toml
	cargo build

target/main.o: src/main.c | target
	$(CC) -o $@ -c $< -Wall

clean:
	rm -rf target