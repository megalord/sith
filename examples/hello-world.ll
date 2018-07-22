; ModuleID = 'examples/hello-world'
source_filename = "examples/hello-world"

declare i32 @puts(i8*)

define i32 @hello(i8*) {
entry:
  %1 = alloca [6 x i8], i8 1
  store [6 x i8] c"hello\00", [6 x i8]* %1
  %2 = bitcast [6 x i8]* %1 to i8*
  %puts = call i32 @puts(i8* %2)
  %puts1 = call i32 @puts(i8* %0)
  ret i32 %puts1
}

define i32 @main() {
entry:
  %0 = alloca [6 x i8], i8 1
  store [6 x i8] c"world\00", [6 x i8]* %0
  %1 = bitcast [6 x i8]* %0 to i8*
  %hello = call i32 @hello(i8* %1)
  ret i32 0
}
