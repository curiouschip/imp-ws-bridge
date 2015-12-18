ALL_SRC := $(shell find . -name "*.c" -or -name "*.h")
TOPLEVEL_SRC := main.c mongoose.c

build/imp-bridge: build $(ALL_SRC)
	gcc -lpthread -o $@ $(TOPLEVEL_SRC)

build:
	mkdir -p build

clean:
	rm -rf build
