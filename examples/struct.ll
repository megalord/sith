; ModuleID = 'main'
source_filename = "main"

%Atom_I32_I32 = type { i32, i32 }

declare i32 @puts(i8*)

declare i32 @putchar(i32)

define %Atom_I32_I32 @"examples/struct_Atom_I32_I32"(i32, i32) {
entry:
  %Atom = alloca %Atom_I32_I32
  %line = getelementptr %Atom_I32_I32, %Atom_I32_I32* %Atom, i32 0, i32 0
  store i32 %0, i32* %line
  %pos = getelementptr %Atom_I32_I32, %Atom_I32_I32* %Atom, i32 0, i32 1
  store i32 %1, i32* %pos
  %load_product_constructor_return = load %Atom_I32_I32, %Atom_I32_I32* %Atom
  ret %Atom_I32_I32 %load_product_constructor_return
}

define i32 @line(%Atom_I32_I32) {
entry:
  %line = extractvalue %Atom_I32_I32 %0, 0
  ret i32 %line
}

define i32 @pos(%Atom_I32_I32) {
entry:
  %pos = extractvalue %Atom_I32_I32 %0, 1
  ret i32 %pos
}

define i32 @"examples/struct_main"() {
entry:
  %Atom = call %Atom_I32_I32 @"examples/struct_Atom_I32_I32"(i32 72, i32 105)
  %line = call i32 @"examples/struct_line_Atom"(%Atom_I32_I32 %Atom)
  %putchar = call i32 @putchar(i32 %line)
  %pos = call i32 @"examples/struct_pos_Atom"(%Atom_I32_I32 %Atom)
  %putchar1 = call i32 @putchar(i32 %pos)
  ret i32 0
}

declare i32 @"examples/struct_line_Atom"(%Atom_I32_I32)

declare i32 @"examples/struct_pos_Atom"(%Atom_I32_I32)
