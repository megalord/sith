sith: main.c
	gcc main.c -o ~/.local/bin/sith

hello-world: sith
	sith build examples/hello-world.sith > examples/hello-world.c
	gcc examples/hello-world.c -o examples/hello-world
	./examples/hello-world
