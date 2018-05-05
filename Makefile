GIT_COMMIT = $(shell git rev-parse HEAD)

sith: main.c debug.c lexer.c jit.c
	echo "#define VERSION \"$(shell git name-rev --tags --name-only $(GIT_COMMIT)) - $(GIT_COMMIT)\"" > version.h
	gcc -g -I. main.c -o main -I/usr/local/share/libjit/include /usr/local/share/libjit/jit/.libs/libjit.a

hello-world: sith
	./main build examples/hello-world.sith

conditional: sith
	./main build examples/conditional.sith

main.o: main.c debug.c lexer.c module.c #jit.c
	clang -g `llvm-config --cflags` -c $<

llvm: main.o
	clang++ $< `llvm-config --cxxflags --ldflags --libs core executionengine mcjit interpreter analysis native bitwriter --system-libs` -o main-llvm
