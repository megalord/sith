sith: main.sith
	sith build main.sith > sith.c
	gcc sith.c -o sith

upgrade: sith
	cp ./sith ~/.local/bin/sith

hello-world: sith
	./sith build examples/hello-world.sith > examples/hello-world.c
	gcc examples/hello-world.c -o examples/hello-world
	./examples/hello-world

case: sith
	./sith build examples/case.sith > examples/case.c
	gcc examples/case.c -o examples/case
	./examples/case

conditionals: sith
	./sith build examples/conditionals.sith > examples/conditionals.c
	gcc examples/conditionals.c -o examples/conditionals
	./examples/conditionals hello world

loops: sith
	./sith build examples/loops.sith > examples/loops.c
	gcc examples/loops.c -o examples/loops
	./examples/loops

progn: sith
	./sith build examples/progn.sith > examples/progn.c
	gcc examples/progn.c -o examples/progn
	./examples/progn
