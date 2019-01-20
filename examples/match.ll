; ModuleID = 'examples/match'
source_filename = "examples/match"

%Maybe_I32 = type { i1, i32, i1 }

@False = external global i1
@True = external global i1
@None = external global %Maybe_I32

declare i32 @puts(i8*)

declare i32 @putchar(i32)

declare %Maybe_I32 @Just_I32(i32)

define i32 @main() {
entry:
  %Just = call %Maybe_I32 @Just_I32(i32 74)
  %sum_idx = extractvalue %Maybe_I32 %Just, 0
  switch i1 %sum_idx, label %else [
    i1 false, label %pat_0
    i1 true, label %pat_1
  ]

else:                                             ; preds = %entry
  unreachable

match_cont:                                       ; preds = %pat_1, %pat_0
  %matchtmp = phi i32 [ %putchar, %pat_0 ], [ %puts, %pat_1 ]
  ret i32 0

pat_0:                                            ; preds = %entry
  %0 = extractvalue %Maybe_I32 %Just, 1
  %putchar = call i32 @putchar(i32 %0)
  br label %match_cont

pat_1:                                            ; preds = %entry
  %1 = alloca [5 x i8], i8 1
  store [5 x i8] c"none\00", [5 x i8]* %1
  %2 = bitcast [5 x i8]* %1 to i8*
  %puts = call i32 @puts(i8* %2)
  br label %match_cont
}
