; ModuleID = 'examples/switch'
source_filename = "examples/switch"

@str = private unnamed_addr constant [5 x i8] c"even\00"
@str.1 = private unnamed_addr constant [4 x i8] c"odd\00"
@str.2 = private unnamed_addr constant [6 x i8] c"seven\00"

define i32 @main() {
entry:
  switch i32 0, label %else [
    i32 0, label %case_0
    i32 2, label %case_0
    i32 4, label %case_0
    i32 1, label %case_1
    i32 3, label %case_1
    i32 5, label %case_1
    i32 7, label %case_2
  ]

else:                                             ; preds = %entry
  unreachable

switch_cont:                                      ; preds = %case_2, %case_1, %case_0
  %switchtmp = phi i32 [ %puts, %case_0 ], [ %puts1, %case_1 ], [ %puts2, %case_2 ]
  ret i32 0

case_0:                                           ; preds = %entry, %entry, %entry
  %puts = call i32 @puts(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @str, i32 0, i32 0))
  br label %switch_cont

case_1:                                           ; preds = %entry, %entry, %entry
  %puts1 = call i32 @puts(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @str.1, i32 0, i32 0))
  br label %switch_cont

case_2:                                           ; preds = %entry
  %puts2 = call i32 @puts(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @str.2, i32 0, i32 0))
  br label %switch_cont
}

declare i32 @puts(i8*)
