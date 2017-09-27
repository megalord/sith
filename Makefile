GIT_COMMIT = $(shell git rev-parse HEAD)

sith: main.sith
	echo "#define VERSION \"$(shell git name-rev --tags --name-only $(GIT_COMMIT)) - $(GIT_COMMIT)\"" > version.h
	sith build main.sith

upgrade: sith
	cp ./main ~/.local/bin/sith

hello-world: sith
	./main build examples/hello-world.sith
	./examples/hello-world

case: sith
	./main build examples/case.sith
	./examples/case

conditionals: sith
	./main build examples/conditionals.sith
	./examples/conditionals hello world

include: sith
	./main build examples/include.sith
	./examples/include hello world

loops: sith
	./main build examples/loops.sith
	./examples/loops

progn: sith
	./main build examples/progn.sith
	./examples/progn

types: sith
	./main build examples/types.sith
	./examples/types

examples: hello-world case conditionals include loops progn types
	echo "done"
