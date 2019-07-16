; ModuleID = 'main'
source_filename = "main"

%Maybe_Ptr_Cons = type { i1, %Cons*, i1 }
%Cons = type { %cars, %Maybe_Ptr_Cons }
%cars = type { i1, %Cons*, %Strs* }
%Strs = type { i8*, i32 }
%FILE = type opaque
%BufferPtr_Cons = type { i32, i32, %Strs }
%StreamPtr_Cons = type { %FILE*, i32 }

@"/usr/local/lib/sith/data/bool_False_Bool_False" = global i1 false
@"/usr/local/lib/sith/data/bool_True_Bool_False" = global i1 true
@"/usr/local/lib/sith/data/maybe_None_Maybe_Ptr" = global %Maybe_Ptr_Cons { i1 true, %Cons* null, i1 false }

declare i32 @puts(i8*)

declare i32 @putchar(i32)

declare %FILE* @fopen(i8*, i8*)

declare i32 @fclose(%FILE*)

declare i1 @"/usr/local/lib/sith/data/bool_and_Bool_Bool"(i1, i1)

declare i1 @"/usr/local/lib/sith/data/bool_or_Bool_Bool"(i1, i1)

declare i1 @"/usr/local/lib/sith/data/bool_not_Bool"(i1)

define %Maybe_Ptr_Cons @"/usr/local/lib/sith/data/maybe_Just_Ptr"(%Cons*) {
entry:
  %Maybe = alloca %Maybe_Ptr_Cons
  %sum = getelementptr %Maybe_Ptr_Cons, %Maybe_Ptr_Cons* %Maybe, i32 0, i32 0
  store i1 false, i1* %sum
  %Just = getelementptr %Maybe_Ptr_Cons, %Maybe_Ptr_Cons* %Maybe, i32 0, i32 1
  store %Cons* %0, %Cons** %Just
  %None = getelementptr %Maybe_Ptr_Cons, %Maybe_Ptr_Cons* %Maybe, i32 0, i32 2
  store i1 false, i1* %None
  %load_sum_constructor_return = load %Maybe_Ptr_Cons, %Maybe_Ptr_Cons* %Maybe
  ret %Maybe_Ptr_Cons %load_sum_constructor_return
}

define %Strs @"/usr/local/lib/sith/data/str_Str_Ptr_I32"(i8*, i32) {
entry:
  %Str = alloca %Strs
  %data = getelementptr %Strs, %Strs* %Str, i32 0, i32 0
  store i8* %0, i8** %data
  %len = getelementptr %Strs, %Strs* %Str, i32 0, i32 1
  store i32 %1, i32* %len
  %load_product_constructor_return = load %Strs, %Strs* %Str
  ret %Strs %load_product_constructor_return
}

define i8* @data(%Strs) {
entry:
  %data = extractvalue %Strs %0, 0
  ret i8* %data
}

define i32 @len(%Strs) {
entry:
  %len = extractvalue %Strs %0, 1
  ret i32 %len
}

define i32 @"/usr/local/lib/sith/data/str_print_Str"(%Strs) {
entry:
  %data = call i8* @data(%Strs %0)
  %puts = call i32 @puts(i8* %data)
  ret i32 %puts
}

define %Cons @lexer_Cons_car_Maybe(%cars, %Maybe_Ptr_Cons) {
entry:
  %Cons = alloca %Cons
  %car = getelementptr %Cons, %Cons* %Cons, i32 0, i32 0
  store %cars %0, %cars* %car
  %cdr = getelementptr %Cons, %Cons* %Cons, i32 0, i32 1
  store %Maybe_Ptr_Cons %1, %Maybe_Ptr_Cons* %cdr
  %load_product_constructor_return = load %Cons, %Cons* %Cons
  ret %Cons %load_product_constructor_return
}

define %cars @car(%Cons) {
entry:
  %car = extractvalue %Cons %0, 0
  ret %cars %car
}

define %Maybe_Ptr_Cons @cdr(%Cons) {
entry:
  %cdr = extractvalue %Cons %0, 1
  ret %Maybe_Ptr_Cons %cdr
}

define %BufferPtr_Cons @lexer_Buffer_I32_I32_Str(i32, i32, %Strs) {
entry:
  %Buffer = alloca %BufferPtr_Cons
  %rem = getelementptr %BufferPtr_Cons, %BufferPtr_Cons* %Buffer, i32 0, i32 0
  store i32 %0, i32* %rem
  %size = getelementptr %BufferPtr_Cons, %BufferPtr_Cons* %Buffer, i32 0, i32 1
  store i32 %1, i32* %size
  %data = getelementptr %BufferPtr_Cons, %BufferPtr_Cons* %Buffer, i32 0, i32 2
  store %Strs %2, %Strs* %data
  %load_product_constructor_return = load %BufferPtr_Cons, %BufferPtr_Cons* %Buffer
  ret %BufferPtr_Cons %load_product_constructor_return
}

define i32 @rem(%BufferPtr_Cons) {
entry:
  %rem = extractvalue %BufferPtr_Cons %0, 0
  ret i32 %rem
}

define i32 @size(%BufferPtr_Cons) {
entry:
  %size = extractvalue %BufferPtr_Cons %0, 1
  ret i32 %size
}

define %Strs @data.1(%BufferPtr_Cons) {
entry:
  %data = extractvalue %BufferPtr_Cons %0, 2
  ret %Strs %data
}

define %StreamPtr_Cons @lexer_Stream_Ptr_I32(%FILE*, i32) {
entry:
  %Stream = alloca %StreamPtr_Cons
  %source = getelementptr %StreamPtr_Cons, %StreamPtr_Cons* %Stream, i32 0, i32 0
  store %FILE* %0, %FILE** %source
  %next = getelementptr %StreamPtr_Cons, %StreamPtr_Cons* %Stream, i32 0, i32 1
  store i32 %1, i32* %next
  %load_product_constructor_return = load %StreamPtr_Cons, %StreamPtr_Cons* %Stream
  ret %StreamPtr_Cons %load_product_constructor_return
}

define %FILE* @source(%StreamPtr_Cons) {
entry:
  %source = extractvalue %StreamPtr_Cons %0, 0
  ret %FILE* %source
}

define i32 @next(%StreamPtr_Cons) {
entry:
  %next = extractvalue %StreamPtr_Cons %0, 1
  ret i32 %next
}

define i32 @"lexer_filename->node_Str_Ptr"(%Strs, %Cons*) {
entry:
  %2 = alloca [4 x i8], i8 1
  store [4 x i8] c"cdr\00", [4 x i8]* %2
  %3 = bitcast [4 x i8]* %2 to i8*
  %load_global_var = load %Maybe_Ptr_Cons, %Maybe_Ptr_Cons* @"/usr/local/lib/sith/data/maybe_None_Maybe_Ptr"
  %cdr = getelementptr %Cons, %Cons* %1, i32 0, i32 1
  store %Maybe_Ptr_Cons %load_global_var, %Maybe_Ptr_Cons* %cdr
  ret i32 0
}
