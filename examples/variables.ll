; ModuleID = 'examples/variables'
source_filename = "examples/variables"

declare i32 @puts(i8*)

declare i32 @putchar(i32)

define i32 @main() {
entry:
  %0 = alloca [6 x i8], i8 1
  store [6 x i8] c"hello\00", [6 x i8]* %0
  %1 = bitcast [6 x i8]* %0 to i8*
  %puts = call i32 @puts(i8* %1)
  %2 = alloca [6 x i8], i8 1
  store [6 x i8] c"world\00", [6 x i8]* %2
  %3 = bitcast [6 x i8]* %2 to i8*
  %puts1 = call i32 @puts(i8* %3)
  ret i32 0
}
