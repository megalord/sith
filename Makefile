sith: main.c
	gcc main.c -o ~/.local/bin/sith

hello-world: sith
	sith build examples/hello-world.sith
