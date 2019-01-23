; ModuleID = 'main'
source_filename = "main"

%Maybe_I32 = type { i1, i32, i1 }

@"/usr/local/lib/sith/data/bool_False_Bool_False" = global i1 false
@"/usr/local/lib/sith/data/bool_True_Bool_False" = global i1 true
@"/usr/local/lib/sith/data/maybe_None_Maybe_I32" = global %Maybe_I32 { i1 true, i32 0, i1 false }

declare i32 @puts(i8*)

declare i32 @putchar(i32)

declare i1 @"/usr/local/lib/sith/data/bool_and_Bool_Bool"(i1, i1)

declare i1 @"/usr/local/lib/sith/data/bool_or_Bool_Bool"(i1, i1)

declare i1 @"/usr/local/lib/sith/data/bool_not_Bool"(i1)

define %Maybe_I32 @"/usr/local/lib/sith/data/maybe_Just_I32"(i32) {
entry:
  %Maybe = alloca %Maybe_I32
  %sum = getelementptr %Maybe_I32, %Maybe_I32* %Maybe, i32 0, i32 0
  store i1 false, i1* %sum
  %Just = getelementptr %Maybe_I32, %Maybe_I32* %Maybe, i32 0, i32 1
  store i32 %0, i32* %Just
  %None = getelementptr %Maybe_I32, %Maybe_I32* %Maybe, i32 0, i32 2
  store i1 false, i1* %None
  %load_sum_constructor_return = load %Maybe_I32, %Maybe_I32* %Maybe
  ret %Maybe_I32 %load_sum_constructor_return
}

define i32 @"examples/match_main"() {
entry:
  %Just = call %Maybe_I32 @"/usr/local/lib/sith/data/maybe_Just_I32"(i32 74)
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
