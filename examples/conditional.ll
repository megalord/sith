; ModuleID = 'examples/conditional'
source_filename = "examples/conditional"

declare i8 @and(i8, i8)

declare i8 @or(i8, i8)

declare i8 @not(i8)

declare i8 @eq(i8, i8)

declare i32 @puts(i8*)

define i32 @main() {
entry:
  br i1 true, label %then, label %else

then:                                             ; preds = %entry
  br i1 true, label %then1, label %else2

else:                                             ; preds = %entry
  %0 = alloca [6 x i8], i8 1
  store [6 x i8] c"false\00", [6 x i8]* %0
  %1 = bitcast [6 x i8]* %0 to i8*
  %puts5 = call i32 @puts(i8* %1)
  br label %if_cont

if_cont:                                          ; preds = %else, %if_cont3
  %iftmp6 = phi i32 [ %iftmp, %if_cont3 ], [ %puts5, %else ]
  ret i32 0

then1:                                            ; preds = %then
  %2 = alloca [5 x i8], i8 1
  store [5 x i8] c"true\00", [5 x i8]* %2
  %3 = bitcast [5 x i8]* %2 to i8*
  %puts = call i32 @puts(i8* %3)
  br label %if_cont3

else2:                                            ; preds = %then
  %4 = alloca [6 x i8], i8 1
  store [6 x i8] c"false\00", [6 x i8]* %4
  %5 = bitcast [6 x i8]* %4 to i8*
  %puts4 = call i32 @puts(i8* %5)
  br label %if_cont3

if_cont3:                                         ; preds = %else2, %then1
  %iftmp = phi i32 [ %puts, %then1 ], [ %puts4, %else2 ]
  br label %if_cont
}
