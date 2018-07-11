; ModuleID = 'examples/conditional'
source_filename = "examples/conditional"

@str = private unnamed_addr constant [5 x i8] c"true\00"
@str.1 = private unnamed_addr constant [6 x i8] c"false\00"
@str.2 = private unnamed_addr constant [6 x i8] c"false\00"

define i32 @main() {
entry:
  br i1 true, label %then, label %else

then:                                             ; preds = %entry
  br i1 true, label %then1, label %else2

else:                                             ; preds = %entry
  %puts5 = call i32 @puts(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @str.2, i32 0, i32 0))
  br label %if_cont

if_cont:                                          ; preds = %else, %if_cont3
  %iftmp6 = phi i32 [ %iftmp, %if_cont3 ], [ %puts5, %else ]
  ret i32 0

then1:                                            ; preds = %then
  %puts = call i32 @puts(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @str, i32 0, i32 0))
  br label %if_cont3

else2:                                            ; preds = %then
  %puts4 = call i32 @puts(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @str.1, i32 0, i32 0))
  br label %if_cont3

if_cont3:                                         ; preds = %else2, %then1
  %iftmp = phi i32 [ %puts, %then1 ], [ %puts4, %else2 ]
  br label %if_cont
}

declare i32 @puts(i8*)
