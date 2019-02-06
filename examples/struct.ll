; ModuleID = 'main'
source_filename = "main"

%FILE = type opaque
%Atom = type { i32, i32 }

declare i32 @puts(i8*)

declare i32 @putchar(i32)

declare %FILE* @fopen(i8*, i8*)

declare i32 @fclose(%FILE*)

define %Atom @"examples/struct_Atom_I32_I32"(i32, i32) {
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

define i32 @"examples/struct_main"() {
entry:
  %Atom = call %Atom @"examples/struct_Atom_I32_I32"(i32 72, i32 105)
  %line = call i32 @line(%Atom %Atom)
  %putchar = call i32 @putchar(i32 %line)
  %pos = call i32 @pos(%Atom %Atom)
  %putchar1 = call i32 @putchar(i32 %pos)
  ret i32 0
}
