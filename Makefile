.DEFAULT_GOAL := main
GIT_COMMIT = $(shell git rev-parse HEAD)
SRC_DIR = $(shell pwd)/src
SRC=$(wildcard $(SRC_DIR)/*.c)
OBJ=$(SRC:.c=.o)

main.o: $(SRC)
	echo "#define VERSION \"$(shell git name-rev --tags --name-only $(GIT_COMMIT)) - $(GIT_COMMIT)\"" > $(SRC_DIR)/version.h
	clang -c -g -I$(SRC_DIR) `llvm-config --cflags` $(SRC_DIR)/main.c

main: $(OBJ)
	clang++ `llvm-config --cxxflags --ldflags --libs core executionengine mcjit interpreter irreader analysis native bitwriter --system-libs` $^ -o main

hello-world: main
	./main build examples/hello-world.sith
	./examples/hello-world

conditional: main
	./main build examples/conditional.sith
	./examples/conditional
