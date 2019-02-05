; ModuleID = 'main'
source_filename = "main"

%FILE = type opaque

declare i32 @puts(i8*)

declare i32 @putchar(i32)

declare %FILE* @fopen(i8*, i8*)

declare i32 @fclose(%FILE*)

define i32 @"examples/hello-world_hello_Ptr"(i8*) {
entry:
  %1 = alloca [6 x i8], i8 1
  store [6 x i8] c"hello\00", [6 x i8]* %1
  %2 = bitcast [6 x i8]* %1 to i8*
  %puts = call i32 @puts(i8* %2)
  %puts1 = call i32 @puts(i8* %0)
  ret i32 %puts1
}

define i32 @"examples/hello-world_main"() {
entry:
  %0 = alloca [6 x i8], i8 1
  store [6 x i8] c"world\00", [6 x i8]* %0
  %1 = bitcast [6 x i8]* %0 to i8*
  %hello = call i32 @"examples/hello-world_hello_Ptr"(i8* %1)
  ret i32 0
}
