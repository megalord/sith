; ModuleID = 'examples/variables'
source_filename = "examples/variables"

@str = private unnamed_addr constant [6 x i8] c"hello\00"
@str.1 = private unnamed_addr constant [6 x i8] c"world\00"

define i32 @main() {
entry:
  %puts = call i32 @puts(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @str, i32 0, i32 0))
  %puts1 = call i32 @puts(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @str.1, i32 0, i32 0))
  ret i32 0
}

declare i32 @puts(i8*)
