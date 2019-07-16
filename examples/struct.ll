; ModuleID = 'main'
source_filename = "main"

%Maybe_Ptr_FILE = type { i1, %FILE*, i1 }
%FILE = type opaque
%Atome_Ptr_FILE = type { i32, i32 }

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

define %Atome_Ptr_FILE @"examples/struct_Atom_I32_I32"(i32, i32) {
entry:
  %Atom = alloca %Atome_Ptr_FILE
  %line = getelementptr %Atome_Ptr_FILE, %Atome_Ptr_FILE* %Atom, i32 0, i32 0
  store i32 %0, i32* %line
  %pos = getelementptr %Atome_Ptr_FILE, %Atome_Ptr_FILE* %Atom, i32 0, i32 1
  store i32 %1, i32* %pos
  %load_product_constructor_return = load %Atome_Ptr_FILE, %Atome_Ptr_FILE* %Atom
  ret %Atome_Ptr_FILE %load_product_constructor_return
}

define i32 @line(%Atome_Ptr_FILE) {
entry:
  %line = extractvalue %Atome_Ptr_FILE %0, 0
  ret i32 %line
}

define i32 @pos(%Atome_Ptr_FILE) {
entry:
  %pos = extractvalue %Atome_Ptr_FILE %0, 1
  ret i32 %pos
}

define i32 @"examples/struct_main"() {
entry:
  %Atom = call %Atome_Ptr_FILE @"examples/struct_Atom_I32_I32"(i32 72, i32 105)
  %line = call i32 @line(%Atome_Ptr_FILE %Atom)
  %putchar = call i32 @putchar(i32 %line)
  %pos = call i32 @pos(%Atome_Ptr_FILE %Atom)
  %putchar1 = call i32 @putchar(i32 %pos)
  ret i32 0
}
