; ModuleID = 'examples/match'
source_filename = "examples/match"

@False = external global i1
@True = external global i1

declare i32 @puts(i8*)

declare i32 @putchar(i32)

declare i1 @and(i1, i1)

declare i1 @or(i1, i1)

declare i1 @not(i1)

declare i1 @eq(i8, i8)

define i32 @main() {
entry:
  %load_match_cond = load i1, i1* @False
  switch i1 %load_match_cond, label %else [
    i1 true, label %pat_0
    i1 false, label %pat_1
  ]

else:                                             ; preds = %entry
  unreachable

match_cont:                                       ; preds = %pat_1, %pat_0
  %matchtmp = phi i32 [ %puts, %pat_0 ], [ %puts1, %pat_1 ]
  ret i32 0

pat_0:                                            ; preds = %entry
  %0 = alloca [5 x i8], i8 1
  store [5 x i8] c"true\00", [5 x i8]* %0
  %1 = bitcast [5 x i8]* %0 to i8*
  %puts = call i32 @puts(i8* %1)
  br label %match_cont

pat_1:                                            ; preds = %entry
  %2 = alloca [6 x i8], i8 1
  store [6 x i8] c"false\00", [6 x i8]* %2
  %3 = bitcast [6 x i8]* %2 to i8*
  %puts1 = call i32 @puts(i8* %3)
  br label %match_cont
}
