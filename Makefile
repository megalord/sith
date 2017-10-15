GIT_COMMIT = $(shell git rev-parse HEAD)

sith: main.h main.c lexer.c
	echo "#define VERSION \"$(shell git name-rev --tags --name-only $(GIT_COMMIT)) - $(GIT_COMMIT)\"" > version.h
	gcc -g -I. main.c -o main

hello-world: sith
	./main build examples/hello-world.sith
	./examples/hello-world
