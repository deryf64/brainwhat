CC = clang
CFLAGS = -O2 -Wall -Wextra $(shell llvm-config --cflags)
LDFLAGS = $(shell llvm-config --ldflags --libs core native)
BIN = bin/bf-llvm
SRC = src/main.c src/ast.c src/codegen.c

.PHONY: all clean run

all: $(BIN)

$(BIN): $(SRC)
	@mkdir -p bin build
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) -o $(BIN)

run: $(BIN)
	$(BIN) code.bf build/out.ll
	llc build/out.ll -filetype=obj -o build/out.o
	$(CC) build/out.o -o bin/bf.out
	./bin/bf.out

clean:
	rm -rf bin build

