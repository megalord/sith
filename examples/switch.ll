; ModuleID = 'main'
source_filename = "main"

%FILE = type opaque

declare i32 @puts(i8*)

declare i32 @putchar(i32)

declare %FILE* @fopen(i8*, i8*)

declare i32 @fclose(%FILE*)

define i32 @"examples/switch_main"() {
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
  %0 = alloca [5 x i8], i8 1
  store [5 x i8] c"even\00", [5 x i8]* %0
  %1 = bitcast [5 x i8]* %0 to i8*
  %puts = call i32 @puts(i8* %1)
  br label %switch_cont

case_1:                                           ; preds = %entry, %entry, %entry
  %2 = alloca [4 x i8], i8 1
  store [4 x i8] c"odd\00", [4 x i8]* %2
  %3 = bitcast [4 x i8]* %2 to i8*
  %puts1 = call i32 @puts(i8* %3)
  br label %switch_cont

case_2:                                           ; preds = %entry
  %4 = alloca [6 x i8], i8 1
  store [6 x i8] c"seven\00", [6 x i8]* %4
  %5 = bitcast [6 x i8]* %4 to i8*
  %puts2 = call i32 @puts(i8* %5)
  br label %switch_cont
}
