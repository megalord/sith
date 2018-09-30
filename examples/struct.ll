; ModuleID = 'examples/struct'
source_filename = "examples/struct"

declare i32 @puts(i8*)

declare i32 @putchar(i32)

define i32 @main() {
entry:
  %Atom = call { i32, i32 } @Atom(i32 1, i32 7)
  %line = call i32 @line({ i32, i32 } %Atom)
  %putchar = call i32 @putchar(i32 %line)
  ret i32 0
}

declare { i32, i32 } @Atom(i32, i32)

declare i32 @line({ i32, i32 })

declare i32 @pos({ i32, i32 })
