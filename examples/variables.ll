; ModuleID = 'main'
source_filename = "main"

%Maybe_Ptr_FILE = type { i1, %FILE*, i1 }
%FILE = type opaque

@"/usr/local/lib/sith/data/bool_False_Bool_False" = global i1 false
@"/usr/local/lib/sith/data/bool_True_Bool_False" = global i1 true
@"/usr/local/lib/sith/data/maybe_None_Maybe_Ptr" = global %Maybe_Ptr_FILE { i1 true, %FILE* null, i1 false }

declare i1 @"/usr/local/lib/sith/data/bool_and_Bool_Bool"(i1, i1)

declare i1 @"/usr/local/lib/sith/data/bool_or_Bool_Bool"(i1, i1)

declare i1 @"/usr/local/lib/sith/data/bool_not_Bool"(i1)

define %Maybe_Ptr_FILE @"/usr/local/lib/sith/data/maybe_Just_Ptr"(%FILE*) {
entry:
  %Maybe = alloca %Maybe_Ptr_FILE
  %sum = getelementptr %Maybe_Ptr_FILE, %Maybe_Ptr_FILE* %Maybe, i32 0, i32 0
  store i1 false, i1* %sum
  %Just = getelementptr %Maybe_Ptr_FILE, %Maybe_Ptr_FILE* %Maybe, i32 0, i32 1
  store %FILE* %0, %FILE** %Just
  %None = getelementptr %Maybe_Ptr_FILE, %Maybe_Ptr_FILE* %Maybe, i32 0, i32 2
  store i1 false, i1* %None
  %load_sum_constructor_return = load %Maybe_Ptr_FILE, %Maybe_Ptr_FILE* %Maybe
  ret %Maybe_Ptr_FILE %load_sum_constructor_return
}

declare i32 @puts(i8*)

declare i32 @putchar(i32)

declare %Maybe_Ptr_FILE @fopen(i8*, i8*)

declare i8 @fgetc(%FILE*)

declare i32 @fclose(%FILE*)

define i32 @"examples/variables_main"() {
entry:
  %0 = alloca [6 x i8], i8 1
  store [6 x i8] c"hello\00", [6 x i8]* %0
  %1 = bitcast [6 x i8]* %0 to i8*
  %puts = call i32 @puts(i8* %1)
  %2 = alloca [6 x i8], i8 1
  store [6 x i8] c"world\00", [6 x i8]* %2
  %3 = bitcast [6 x i8]* %2 to i8*
  %puts1 = call i32 @puts(i8* %3)
  ret i32 0
}
