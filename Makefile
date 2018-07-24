.DEFAULT_GOAL := main
GIT_COMMIT = $(shell git rev-parse HEAD)
SRC_DIR = src
EXAMPLE_DIR = examples
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:.c=.o)

main: $(OBJ)
	clang++ -g `llvm-config --cxxflags --ldflags --libs core executionengine mcjit interpreter irreader analysis native bitwriter --system-libs` $(OBJ) -o main

$(SRC_DIR)/main.o: $(SRC_DIR)/main.c
	echo "#define VERSION \"$(shell git name-rev --tags --name-only $(GIT_COMMIT)) - $(GIT_COMMIT)\"" > $(SRC_DIR)/version.h
	clang -c -g -I$(SRC_DIR) `llvm-config --cflags` $< -o $@

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	clang -c -g -I$(SRC_DIR) `llvm-config --cflags` $< -o $@

hello-world: main
	./main build examples/hello-world.sith
	./examples/hello-world

conditional: main
	./main build examples/conditional.sith
	./examples/conditional

switch: main
	./main build examples/switch.sith
	./examples/switch

variables: main
	./main build examples/variables.sith
	./examples/variables

ir: main
	for f in examples/*.sith; do \
		echo "./main buildir $$f"; \
		./main buildir $$f; \
	done
