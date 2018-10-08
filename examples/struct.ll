; ModuleID = 'examples/struct'
source_filename = "examples/struct"

%Atom = type { i32, i32 }

declare i32 @puts(i8*)

declare i32 @putchar(i32)

define i32 @main() {
entry:
  %Atom = call %Atom @Atom(i32 72, i32 105)
  %line = call i32 @line(%Atom %Atom)
  %putchar = call i32 @putchar(i32 %line)
  %pos = call i32 @pos(%Atom %Atom)
  %putchar1 = call i32 @putchar(i32 %pos)
  ret i32 0
}

define %Atom @Atom(i32, i32) {
entry:
  %Atom = alloca %Atom
  %line = getelementptr %Atom, %Atom* %Atom, i32 0, i32 0
  store i32 %0, i32* %line
  %pos = getelementptr %Atom, %Atom* %Atom, i32 0, i32 1
  store i32 %1, i32* %pos
  %load_product_constructor_return = load %Atom, %Atom* %Atom
  ret %Atom %load_product_constructor_return
}

define i32 @line(%Atom) {
entry:
  %line = extractvalue %Atom %0, 0
  ret i32 %line
}

define i32 @pos(%Atom) {
entry:
  %pos = extractvalue %Atom %0, 1
  ret i32 %pos
}
