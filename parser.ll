; ModuleID = 'src/parser.c'
source_filename = "src/parser.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.13.0"

%struct.module_cache_t = type { i32, i32, %struct.module_t* }
%struct.module_t = type { i8*, i32, %struct.module_t**, i32, %struct.type_t*, %struct.symbol_table_t, %struct.LLVMOpaqueModule* }
%struct.type_t = type { i8*, i32, i32, i32, %struct.type_t**, i8**, %struct.LLVMOpaqueType* }
%struct.LLVMOpaqueType = type opaque
%struct.symbol_table_t = type { %struct.symbol_table_t*, i32, i32, i8**, %struct.val_t* }
%struct.val_t = type { %struct.type_t*, %union.anon, %struct.LLVMOpaqueValue* }
%union.anon = type { %struct.anon.10 }
%struct.anon.10 = type { i32, %struct.val_t* }
%struct.LLVMOpaqueValue = type opaque
%struct.LLVMOpaqueModule = type opaque
%struct.expr_t = type { i32, %struct.type_t*, %union.anon.1 }
%union.anon.1 = type { %struct.anon.3 }
%struct.anon.3 = type { i8*, %struct.val_t*, i32, %struct.expr_t* }
%struct.__sFILE = type { i8*, i32, i32, i16, i16, %struct.__sbuf, i32, i8*, i32 (i8*)*, i32 (i8*, i8*, i32)*, i64 (i8*, i64, i32)*, i32 (i8*, i8*, i32)*, %struct.__sbuf, %struct.__sFILEX*, i32, [3 x i8], [1 x i8], %struct.__sbuf, i32, i64 }
%struct.__sFILEX = type opaque
%struct.__sbuf = type { i8*, i32 }
%struct.atom_t = type { i8*, i32, i32, i32 }
%struct.anon = type { i8* }
%struct.list_t = type { i32, %struct.node_t* }
%struct.node_t = type { i32, %union.anon.11, %struct.node_t* }
%union.anon.11 = type { %struct.atom_t* }
%struct.anon.4 = type { %struct.expr_t*, %struct.expr_t*, %struct.expr_t* }
%struct.anon.9 = type { i8*, %struct.val_t* }
%struct.anon.2 = type { %struct.val_t* }
%struct.anon.7 = type { i32, %struct.expr_t* }
%struct.anon.8 = type { i32, %struct.expr_t*, %struct.expr_t*, %struct.val_list_t* }
%struct.val_list_t = type { i32, %struct.val_t* }
%struct.anon.6 = type { i32, %struct.expr_t*, %struct.expr_t*, %struct.expr_t* }
%struct.anon.5 = type { %struct.symbol_table_t*, %struct.expr_t* }
%struct.anon.0 = type { %struct.expr_t* }

@.str = private unnamed_addr constant [21 x i8] c"/usr/local/lib/sith/\00", align 1
@stdlib = global i8* getelementptr inbounds ([21 x i8], [21 x i8]* @.str, i32 0, i32 0), align 8
@cache = global %struct.module_cache_t zeroinitializer, align 8
@.str.1 = private unnamed_addr constant [8 x i8] c"builtin\00", align 1
@MODULE_BUILTIN = global %struct.module_t { i8* getelementptr inbounds ([8 x i8], [8 x i8]* @.str.1, i32 0, i32 0), i32 0, %struct.module_t** null, i32 0, %struct.type_t* null, %struct.symbol_table_t zeroinitializer, %struct.LLVMOpaqueModule* null }, align 8
@i_expr = internal global i32 0, align 4
@__func__.expr_new = private unnamed_addr constant [9 x i8] c"expr_new\00", align 1
@.str.2 = private unnamed_addr constant [13 x i8] c"src/parser.c\00", align 1
@.str.3 = private unnamed_addr constant [19 x i8] c"i_expr < NUM_EXPRS\00", align 1
@exprs = internal global [9192 x %struct.expr_t] zeroinitializer, align 16
@__func__.expr_new_i = private unnamed_addr constant [11 x i8] c"expr_new_i\00", align 1
@.str.4 = private unnamed_addr constant [27 x i8] c"i_expr + i - 1 < NUM_EXPRS\00", align 1
@__stderrp = external global %struct.__sFILE*, align 8
@.str.5 = private unnamed_addr constant [25 x i8] c"ATOM_CHAR not supported\0A\00", align 1
@.str.6 = private unnamed_addr constant [31 x i8] c"ATOM_IDENTIFIER not supported\0A\00", align 1
@TYPE_I32 = common global %struct.type_t* null, align 8
@.str.7 = private unnamed_addr constant [2 x i8] c"0\00", align 1
@.str.8 = private unnamed_addr constant [31 x i8] c"unable to parse atom into int\0A\00", align 1
@TYPE_CSTR = common global %struct.type_t* null, align 8
@.str.9 = private unnamed_addr constant [5 x i8] c"Bool\00", align 1
@.str.10 = private unnamed_addr constant [35 x i8] c"if condition must be bool, got %s\0A\00", align 1
@.str.11 = private unnamed_addr constant [38 x i8] c"invalid let syntax: not enough forms\0A\00", align 1
@.str.12 = private unnamed_addr constant [49 x i8] c"invalid let syntax: var bindings must be a list\0A\00", align 1
@.str.13 = private unnamed_addr constant [21 x i8] c"invalid let binding\0A\00", align 1
@.str.14 = private unnamed_addr constant [27 x i8] c"invalid let variable name\0A\00", align 1
@.str.15 = private unnamed_addr constant [43 x i8] c"invalid case: expected list with length 2\0A\00", align 1
@.str.16 = private unnamed_addr constant [27 x i8] c"case can only match atoms\0A\00", align 1
@.str.17 = private unnamed_addr constant [43 x i8] c"invalid match: can only match on sum type\0A\00", align 1
@.str.18 = private unnamed_addr constant [50 x i8] c"invalid match: clause must be list with length 2\0A\00", align 1
@.str.19 = private unnamed_addr constant [55 x i8] c"invalid match: pattern must be var or funcall, got %d\0A\00", align 1
@.str.20 = private unnamed_addr constant [52 x i8] c"invalid match: pattern type does not fit condition\0A\00", align 1
@.str.21 = private unnamed_addr constant [29 x i8] c"symbol %s not found (%d:%d)\0A\00", align 1
@.str.22 = private unnamed_addr constant [22 x i8] c"%s is not a function\0A\00", align 1
@.str.23 = private unnamed_addr constant [32 x i8] c"signature of %s does not match\0A\00", align 1
@.str.24 = private unnamed_addr constant [18 x i8] c"var %s not found\0A\00", align 1
@.str.25 = private unnamed_addr constant [18 x i8] c"%s is a function\0A\00", align 1
@.str.26 = private unnamed_addr constant [36 x i8] c"cannot parse double list into expr\0A\00", align 1
@.str.27 = private unnamed_addr constant [3 x i8] c"if\00", align 1
@.str.28 = private unnamed_addr constant [4 x i8] c"let\00", align 1
@.str.29 = private unnamed_addr constant [6 x i8] c"progn\00", align 1
@.str.30 = private unnamed_addr constant [5 x i8] c"cond\00", align 1
@.str.31 = private unnamed_addr constant [6 x i8] c"match\00", align 1
@.str.32 = private unnamed_addr constant [18 x i8] c"invalid arg list\0A\00", align 1
@.str.33 = private unnamed_addr constant [32 x i8] c"no type signature found for %s\0A\00", align 1
@.str.34 = private unnamed_addr constant [45 x i8] c"%s definition does not match type signature\0A\00", align 1
@.str.35 = private unnamed_addr constant [43 x i8] c"Function parameter arguments must be atoms\00", align 1
@cwd = common global [1024 x i8] zeroinitializer, align 16
@.str.36 = private unnamed_addr constant [19 x i8] c"could not get cwd\0A\00", align 1
@.str.37 = private unnamed_addr constant [5 x i8] c"POLY\00", align 1
@TYPE_POLY = common global %struct.type_t* null, align 8
@.str.38 = private unnamed_addr constant [3 x i8] c"I8\00", align 1
@TYPE_I8 = common global %struct.type_t* null, align 8
@.str.39 = private unnamed_addr constant [4 x i8] c"I32\00", align 1
@.str.40 = private unnamed_addr constant [4 x i8] c"Ptr\00", align 1
@TYPE_PTR = common global %struct.type_t* null, align 8
@.str.41 = private unnamed_addr constant [26 x i8] c"root node must be a list\0A\00", align 1
@.str.42 = private unnamed_addr constant [8 x i8] c"include\00", align 1
@.str.43 = private unnamed_addr constant [8 x i8] c"deftype\00", align 1
@.str.44 = private unnamed_addr constant [2 x i8] c":\00", align 1
@.str.45 = private unnamed_addr constant [30 x i8] c"sub-root node must be a list\0A\00", align 1
@.str.46 = private unnamed_addr constant [19 x i8] c"invalid type name\0A\00", align 1
@.str.47 = private unnamed_addr constant [6 x i8] c"defun\00", align 1
@.str.48 = private unnamed_addr constant [18 x i8] c"failed in lexing\0A\00", align 1
@.str.49 = private unnamed_addr constant [19 x i8] c"failed in parsing\0A\00", align 1
@.str.50 = private unnamed_addr constant [6 x i8] c".sith\00", align 1
@INDENTATION = common global i32 0, align 4

; Function Attrs: noinline nounwind optnone ssp uwtable
define %struct.expr_t* @expr_new() #0 {
  %1 = load i32, i32* @i_expr, align 4
  %2 = icmp slt i32 %1, 9192
  %3 = xor i1 %2, true
  %4 = zext i1 %3 to i32
  %5 = sext i32 %4 to i64
  %6 = icmp ne i64 %5, 0
  br i1 %6, label %7, label %9

; <label>:7:                                      ; preds = %0
  call void @__assert_rtn(i8* getelementptr inbounds ([9 x i8], [9 x i8]* @__func__.expr_new, i32 0, i32 0), i8* getelementptr inbounds ([13 x i8], [13 x i8]* @.str.2, i32 0, i32 0), i32 34, i8* getelementptr inbounds ([19 x i8], [19 x i8]* @.str.3, i32 0, i32 0)) #7
  unreachable
                                                  ; No predecessors!
  br label %10

; <label>:9:                                      ; preds = %0
  br label %10

; <label>:10:                                     ; preds = %9, %8
  %11 = load i32, i32* @i_expr, align 4
  %12 = add nsw i32 %11, 1
  store i32 %12, i32* @i_expr, align 4
  %13 = sext i32 %11 to i64
  %14 = getelementptr inbounds %struct.expr_t, %struct.expr_t* getelementptr inbounds ([9192 x %struct.expr_t], [9192 x %struct.expr_t]* @exprs, i32 0, i32 0), i64 %13
  ret %struct.expr_t* %14
}

; Function Attrs: noreturn
declare void @__assert_rtn(i8*, i8*, i32, i8*) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define %struct.expr_t* @expr_new_i(i32) #0 {
  %2 = alloca i32, align 4
  %3 = alloca %struct.expr_t*, align 8
  store i32 %0, i32* %2, align 4
  %4 = load i32, i32* @i_expr, align 4
  %5 = load i32, i32* %2, align 4
  %6 = add nsw i32 %4, %5
  %7 = sub nsw i32 %6, 1
  %8 = icmp slt i32 %7, 9192
  %9 = xor i1 %8, true
  %10 = zext i1 %9 to i32
  %11 = sext i32 %10 to i64
  %12 = icmp ne i64 %11, 0
  br i1 %12, label %13, label %15

; <label>:13:                                     ; preds = %1
  call void @__assert_rtn(i8* getelementptr inbounds ([11 x i8], [11 x i8]* @__func__.expr_new_i, i32 0, i32 0), i8* getelementptr inbounds ([13 x i8], [13 x i8]* @.str.2, i32 0, i32 0), i32 39, i8* getelementptr inbounds ([27 x i8], [27 x i8]* @.str.4, i32 0, i32 0)) #7
  unreachable
                                                  ; No predecessors!
  br label %16

; <label>:15:                                     ; preds = %1
  br label %16

; <label>:16:                                     ; preds = %15, %14
  %17 = load i32, i32* @i_expr, align 4
  %18 = sext i32 %17 to i64
  %19 = getelementptr inbounds %struct.expr_t, %struct.expr_t* getelementptr inbounds ([9192 x %struct.expr_t], [9192 x %struct.expr_t]* @exprs, i32 0, i32 0), i64 %18
  store %struct.expr_t* %19, %struct.expr_t** %3, align 8
  %20 = load i32, i32* %2, align 4
  %21 = load i32, i32* @i_expr, align 4
  %22 = add nsw i32 %21, %20
  store i32 %22, i32* @i_expr, align 4
  %23 = load %struct.expr_t*, %struct.expr_t** %3, align 8
  ret %struct.expr_t* %23
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @parse_atom(%struct.module_t*, %struct.symbol_table_t*, %struct.atom_t*, %struct.val_t*) #0 {
  %5 = alloca i32, align 4
  %6 = alloca %struct.module_t*, align 8
  %7 = alloca %struct.symbol_table_t*, align 8
  %8 = alloca %struct.atom_t*, align 8
  %9 = alloca %struct.val_t*, align 8
  store %struct.module_t* %0, %struct.module_t** %6, align 8
  store %struct.symbol_table_t* %1, %struct.symbol_table_t** %7, align 8
  store %struct.atom_t* %2, %struct.atom_t** %8, align 8
  store %struct.val_t* %3, %struct.val_t** %9, align 8
  %10 = load %struct.atom_t*, %struct.atom_t** %8, align 8
  %11 = getelementptr inbounds %struct.atom_t, %struct.atom_t* %10, i32 0, i32 1
  %12 = load i32, i32* %11, align 8
  switch i32 %12, label %65 [
    i32 0, label %13
    i32 1, label %16
    i32 2, label %19
    i32 3, label %54
  ]

; <label>:13:                                     ; preds = %4
  %14 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %15 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %14, i8* getelementptr inbounds ([25 x i8], [25 x i8]* @.str.5, i32 0, i32 0))
  store i32 1, i32* %5, align 4
  br label %66

; <label>:16:                                     ; preds = %4
  %17 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %18 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %17, i8* getelementptr inbounds ([31 x i8], [31 x i8]* @.str.6, i32 0, i32 0))
  store i32 1, i32* %5, align 4
  br label %66

; <label>:19:                                     ; preds = %4
  %20 = load %struct.type_t*, %struct.type_t** @TYPE_I32, align 8
  %21 = load %struct.val_t*, %struct.val_t** %9, align 8
  %22 = getelementptr inbounds %struct.val_t, %struct.val_t* %21, i32 0, i32 0
  store %struct.type_t* %20, %struct.type_t** %22, align 8
  %23 = call i8* @malloc(i64 4) #8
  %24 = load %struct.val_t*, %struct.val_t** %9, align 8
  %25 = getelementptr inbounds %struct.val_t, %struct.val_t* %24, i32 0, i32 1
  %26 = bitcast %union.anon* %25 to %struct.anon*
  %27 = getelementptr inbounds %struct.anon, %struct.anon* %26, i32 0, i32 0
  store i8* %23, i8** %27, align 8
  %28 = load %struct.atom_t*, %struct.atom_t** %8, align 8
  %29 = getelementptr inbounds %struct.atom_t, %struct.atom_t* %28, i32 0, i32 0
  %30 = load i8*, i8** %29, align 8
  %31 = call i32 @atoi(i8* %30)
  %32 = load %struct.val_t*, %struct.val_t** %9, align 8
  %33 = getelementptr inbounds %struct.val_t, %struct.val_t* %32, i32 0, i32 1
  %34 = bitcast %union.anon* %33 to %struct.anon*
  %35 = getelementptr inbounds %struct.anon, %struct.anon* %34, i32 0, i32 0
  %36 = load i8*, i8** %35, align 8
  %37 = bitcast i8* %36 to i32*
  store i32 %31, i32* %37, align 4
  %38 = load %struct.val_t*, %struct.val_t** %9, align 8
  %39 = getelementptr inbounds %struct.val_t, %struct.val_t* %38, i32 0, i32 1
  %40 = bitcast %union.anon* %39 to %struct.anon*
  %41 = getelementptr inbounds %struct.anon, %struct.anon* %40, i32 0, i32 0
  %42 = load i8*, i8** %41, align 8
  %43 = icmp eq i8* %42, null
  br i1 %43, label %44, label %53

; <label>:44:                                     ; preds = %19
  %45 = load %struct.atom_t*, %struct.atom_t** %8, align 8
  %46 = getelementptr inbounds %struct.atom_t, %struct.atom_t* %45, i32 0, i32 0
  %47 = load i8*, i8** %46, align 8
  %48 = call i32 @strcmp(i8* %47, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.7, i32 0, i32 0))
  %49 = icmp ne i32 %48, 0
  br i1 %49, label %50, label %53

; <label>:50:                                     ; preds = %44
  %51 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %52 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %51, i8* getelementptr inbounds ([31 x i8], [31 x i8]* @.str.8, i32 0, i32 0))
  store i32 1, i32* %5, align 4
  br label %66

; <label>:53:                                     ; preds = %44, %19
  br label %65

; <label>:54:                                     ; preds = %4
  %55 = load %struct.type_t*, %struct.type_t** @TYPE_CSTR, align 8
  %56 = load %struct.val_t*, %struct.val_t** %9, align 8
  %57 = getelementptr inbounds %struct.val_t, %struct.val_t* %56, i32 0, i32 0
  store %struct.type_t* %55, %struct.type_t** %57, align 8
  %58 = load %struct.atom_t*, %struct.atom_t** %8, align 8
  %59 = getelementptr inbounds %struct.atom_t, %struct.atom_t* %58, i32 0, i32 0
  %60 = load i8*, i8** %59, align 8
  %61 = load %struct.val_t*, %struct.val_t** %9, align 8
  %62 = getelementptr inbounds %struct.val_t, %struct.val_t* %61, i32 0, i32 1
  %63 = bitcast %union.anon* %62 to %struct.anon*
  %64 = getelementptr inbounds %struct.anon, %struct.anon* %63, i32 0, i32 0
  store i8* %60, i8** %64, align 8
  br label %65

; <label>:65:                                     ; preds = %4, %54, %53
  store i32 0, i32* %5, align 4
  br label %66

; <label>:66:                                     ; preds = %65, %50, %16, %13
  %67 = load i32, i32* %5, align 4
  ret i32 %67
}

declare i32 @fprintf(%struct.__sFILE*, i8*, ...) #2

; Function Attrs: allocsize(0)
declare i8* @malloc(i64) #3

declare i32 @atoi(i8*) #2

declare i32 @strcmp(i8*, i8*) #2

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @parse_if(%struct.module_t*, %struct.symbol_table_t*, %struct.list_t*, %struct.expr_t*) #0 {
  %5 = alloca i32, align 4
  %6 = alloca %struct.module_t*, align 8
  %7 = alloca %struct.symbol_table_t*, align 8
  %8 = alloca %struct.list_t*, align 8
  %9 = alloca %struct.expr_t*, align 8
  %10 = alloca %struct.node_t*, align 8
  store %struct.module_t* %0, %struct.module_t** %6, align 8
  store %struct.symbol_table_t* %1, %struct.symbol_table_t** %7, align 8
  store %struct.list_t* %2, %struct.list_t** %8, align 8
  store %struct.expr_t* %3, %struct.expr_t** %9, align 8
  %11 = load %struct.list_t*, %struct.list_t** %8, align 8
  %12 = getelementptr inbounds %struct.list_t, %struct.list_t* %11, i32 0, i32 1
  %13 = load %struct.node_t*, %struct.node_t** %12, align 8
  %14 = getelementptr inbounds %struct.node_t, %struct.node_t* %13, i32 0, i32 2
  %15 = load %struct.node_t*, %struct.node_t** %14, align 8
  store %struct.node_t* %15, %struct.node_t** %10, align 8
  %16 = load %struct.module_t*, %struct.module_t** %6, align 8
  %17 = load %struct.symbol_table_t*, %struct.symbol_table_t** %7, align 8
  %18 = load %struct.node_t*, %struct.node_t** %10, align 8
  %19 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %20 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %19, i32 0, i32 2
  %21 = bitcast %union.anon.1* %20 to %struct.anon.4*
  %22 = getelementptr inbounds %struct.anon.4, %struct.anon.4* %21, i32 0, i32 0
  %23 = load %struct.expr_t*, %struct.expr_t** %22, align 8
  %24 = call i32 @parse_expr(%struct.module_t* %16, %struct.symbol_table_t* %17, %struct.node_t* %18, %struct.expr_t* %23)
  %25 = icmp ne i32 %24, 0
  br i1 %25, label %26, label %27

; <label>:26:                                     ; preds = %4
  store i32 1, i32* %5, align 4
  br label %82

; <label>:27:                                     ; preds = %4
  %28 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %29 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %28, i32 0, i32 2
  %30 = bitcast %union.anon.1* %29 to %struct.anon.4*
  %31 = getelementptr inbounds %struct.anon.4, %struct.anon.4* %30, i32 0, i32 0
  %32 = load %struct.expr_t*, %struct.expr_t** %31, align 8
  %33 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %32, i32 0, i32 1
  %34 = load %struct.type_t*, %struct.type_t** %33, align 8
  %35 = getelementptr inbounds %struct.type_t, %struct.type_t* %34, i32 0, i32 0
  %36 = load i8*, i8** %35, align 8
  %37 = call i32 @strcmp(i8* %36, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.9, i32 0, i32 0))
  %38 = icmp ne i32 %37, 0
  br i1 %38, label %39, label %51

; <label>:39:                                     ; preds = %27
  %40 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %41 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %42 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %41, i32 0, i32 2
  %43 = bitcast %union.anon.1* %42 to %struct.anon.4*
  %44 = getelementptr inbounds %struct.anon.4, %struct.anon.4* %43, i32 0, i32 0
  %45 = load %struct.expr_t*, %struct.expr_t** %44, align 8
  %46 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %45, i32 0, i32 1
  %47 = load %struct.type_t*, %struct.type_t** %46, align 8
  %48 = getelementptr inbounds %struct.type_t, %struct.type_t* %47, i32 0, i32 0
  %49 = load i8*, i8** %48, align 8
  %50 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %40, i8* getelementptr inbounds ([35 x i8], [35 x i8]* @.str.10, i32 0, i32 0), i8* %49)
  store i32 1, i32* %5, align 4
  br label %82

; <label>:51:                                     ; preds = %27
  %52 = load %struct.node_t*, %struct.node_t** %10, align 8
  %53 = getelementptr inbounds %struct.node_t, %struct.node_t* %52, i32 0, i32 2
  %54 = load %struct.node_t*, %struct.node_t** %53, align 8
  store %struct.node_t* %54, %struct.node_t** %10, align 8
  %55 = load %struct.module_t*, %struct.module_t** %6, align 8
  %56 = load %struct.symbol_table_t*, %struct.symbol_table_t** %7, align 8
  %57 = load %struct.node_t*, %struct.node_t** %10, align 8
  %58 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %59 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %58, i32 0, i32 2
  %60 = bitcast %union.anon.1* %59 to %struct.anon.4*
  %61 = getelementptr inbounds %struct.anon.4, %struct.anon.4* %60, i32 0, i32 1
  %62 = load %struct.expr_t*, %struct.expr_t** %61, align 8
  %63 = call i32 @parse_expr(%struct.module_t* %55, %struct.symbol_table_t* %56, %struct.node_t* %57, %struct.expr_t* %62)
  %64 = icmp ne i32 %63, 0
  br i1 %64, label %65, label %66

; <label>:65:                                     ; preds = %51
  store i32 1, i32* %5, align 4
  br label %82

; <label>:66:                                     ; preds = %51
  %67 = load %struct.node_t*, %struct.node_t** %10, align 8
  %68 = getelementptr inbounds %struct.node_t, %struct.node_t* %67, i32 0, i32 2
  %69 = load %struct.node_t*, %struct.node_t** %68, align 8
  store %struct.node_t* %69, %struct.node_t** %10, align 8
  %70 = load %struct.module_t*, %struct.module_t** %6, align 8
  %71 = load %struct.symbol_table_t*, %struct.symbol_table_t** %7, align 8
  %72 = load %struct.node_t*, %struct.node_t** %10, align 8
  %73 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %74 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %73, i32 0, i32 2
  %75 = bitcast %union.anon.1* %74 to %struct.anon.4*
  %76 = getelementptr inbounds %struct.anon.4, %struct.anon.4* %75, i32 0, i32 2
  %77 = load %struct.expr_t*, %struct.expr_t** %76, align 8
  %78 = call i32 @parse_expr(%struct.module_t* %70, %struct.symbol_table_t* %71, %struct.node_t* %72, %struct.expr_t* %77)
  %79 = icmp ne i32 %78, 0
  br i1 %79, label %80, label %81

; <label>:80:                                     ; preds = %66
  store i32 1, i32* %5, align 4
  br label %82

; <label>:81:                                     ; preds = %66
  store i32 0, i32* %5, align 4
  br label %82

; <label>:82:                                     ; preds = %81, %80, %65, %39, %26
  %83 = load i32, i32* %5, align 4
  ret i32 %83
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @parse_expr(%struct.module_t*, %struct.symbol_table_t*, %struct.node_t*, %struct.expr_t*) #0 {
  %5 = alloca i32, align 4
  %6 = alloca %struct.module_t*, align 8
  %7 = alloca %struct.symbol_table_t*, align 8
  %8 = alloca %struct.node_t*, align 8
  %9 = alloca %struct.expr_t*, align 8
  %10 = alloca i8*, align 8
  %11 = alloca %struct.expr_t*, align 8
  store %struct.module_t* %0, %struct.module_t** %6, align 8
  store %struct.symbol_table_t* %1, %struct.symbol_table_t** %7, align 8
  store %struct.node_t* %2, %struct.node_t** %8, align 8
  store %struct.expr_t* %3, %struct.expr_t** %9, align 8
  %12 = load %struct.node_t*, %struct.node_t** %8, align 8
  %13 = getelementptr inbounds %struct.node_t, %struct.node_t* %12, i32 0, i32 0
  %14 = load i32, i32* %13, align 8
  switch i32 %14, label %364 [
    i32 0, label %15
    i32 1, label %142
  ]

; <label>:15:                                     ; preds = %4
  %16 = load %struct.node_t*, %struct.node_t** %8, align 8
  %17 = getelementptr inbounds %struct.node_t, %struct.node_t* %16, i32 0, i32 1
  %18 = bitcast %union.anon.11* %17 to %struct.atom_t**
  %19 = load %struct.atom_t*, %struct.atom_t** %18, align 8
  %20 = getelementptr inbounds %struct.atom_t, %struct.atom_t* %19, i32 0, i32 1
  %21 = load i32, i32* %20, align 8
  %22 = icmp eq i32 %21, 1
  br i1 %22, label %23, label %109

; <label>:23:                                     ; preds = %15
  %24 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %25 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %24, i32 0, i32 0
  store i32 7, i32* %25, align 8
  %26 = load %struct.node_t*, %struct.node_t** %8, align 8
  %27 = getelementptr inbounds %struct.node_t, %struct.node_t* %26, i32 0, i32 1
  %28 = bitcast %union.anon.11* %27 to %struct.atom_t**
  %29 = load %struct.atom_t*, %struct.atom_t** %28, align 8
  %30 = getelementptr inbounds %struct.atom_t, %struct.atom_t* %29, i32 0, i32 0
  %31 = load i8*, i8** %30, align 8
  %32 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %33 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %32, i32 0, i32 2
  %34 = bitcast %union.anon.1* %33 to %struct.anon.9*
  %35 = getelementptr inbounds %struct.anon.9, %struct.anon.9* %34, i32 0, i32 0
  store i8* %31, i8** %35, align 8
  %36 = load %struct.symbol_table_t*, %struct.symbol_table_t** %7, align 8
  %37 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %38 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %37, i32 0, i32 2
  %39 = bitcast %union.anon.1* %38 to %struct.anon.9*
  %40 = getelementptr inbounds %struct.anon.9, %struct.anon.9* %39, i32 0, i32 0
  %41 = load i8*, i8** %40, align 8
  %42 = call %struct.val_t* @symbol_table_get(%struct.symbol_table_t* %36, i8* %41)
  %43 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %44 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %43, i32 0, i32 2
  %45 = bitcast %union.anon.1* %44 to %struct.anon.9*
  %46 = getelementptr inbounds %struct.anon.9, %struct.anon.9* %45, i32 0, i32 1
  store %struct.val_t* %42, %struct.val_t** %46, align 8
  %47 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %48 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %47, i32 0, i32 2
  %49 = bitcast %union.anon.1* %48 to %struct.anon.9*
  %50 = getelementptr inbounds %struct.anon.9, %struct.anon.9* %49, i32 0, i32 1
  %51 = load %struct.val_t*, %struct.val_t** %50, align 8
  %52 = icmp eq %struct.val_t* %51, null
  br i1 %52, label %53, label %65

; <label>:53:                                     ; preds = %23
  %54 = load %struct.module_t*, %struct.module_t** %6, align 8
  %55 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %56 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %55, i32 0, i32 2
  %57 = bitcast %union.anon.1* %56 to %struct.anon.9*
  %58 = getelementptr inbounds %struct.anon.9, %struct.anon.9* %57, i32 0, i32 0
  %59 = load i8*, i8** %58, align 8
  %60 = call %struct.val_t* @module_deps_symbol_find(%struct.module_t* %54, i8* %59)
  %61 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %62 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %61, i32 0, i32 2
  %63 = bitcast %union.anon.1* %62 to %struct.anon.9*
  %64 = getelementptr inbounds %struct.anon.9, %struct.anon.9* %63, i32 0, i32 1
  store %struct.val_t* %60, %struct.val_t** %64, align 8
  br label %65

; <label>:65:                                     ; preds = %53, %23
  %66 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %67 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %66, i32 0, i32 2
  %68 = bitcast %union.anon.1* %67 to %struct.anon.9*
  %69 = getelementptr inbounds %struct.anon.9, %struct.anon.9* %68, i32 0, i32 1
  %70 = load %struct.val_t*, %struct.val_t** %69, align 8
  %71 = icmp eq %struct.val_t* %70, null
  br i1 %71, label %72, label %80

; <label>:72:                                     ; preds = %65
  %73 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %74 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %75 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %74, i32 0, i32 2
  %76 = bitcast %union.anon.1* %75 to %struct.anon.9*
  %77 = getelementptr inbounds %struct.anon.9, %struct.anon.9* %76, i32 0, i32 0
  %78 = load i8*, i8** %77, align 8
  %79 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %73, i8* getelementptr inbounds ([18 x i8], [18 x i8]* @.str.24, i32 0, i32 0), i8* %78)
  store i32 1, i32* %5, align 4
  br label %364

; <label>:80:                                     ; preds = %65
  %81 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %82 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %81, i32 0, i32 2
  %83 = bitcast %union.anon.1* %82 to %struct.anon.9*
  %84 = getelementptr inbounds %struct.anon.9, %struct.anon.9* %83, i32 0, i32 1
  %85 = load %struct.val_t*, %struct.val_t** %84, align 8
  %86 = getelementptr inbounds %struct.val_t, %struct.val_t* %85, i32 0, i32 0
  %87 = load %struct.type_t*, %struct.type_t** %86, align 8
  %88 = getelementptr inbounds %struct.type_t, %struct.type_t* %87, i32 0, i32 1
  %89 = load i32, i32* %88, align 8
  %90 = icmp eq i32 %89, 2
  br i1 %90, label %91, label %99

; <label>:91:                                     ; preds = %80
  %92 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %93 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %94 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %93, i32 0, i32 2
  %95 = bitcast %union.anon.1* %94 to %struct.anon.9*
  %96 = getelementptr inbounds %struct.anon.9, %struct.anon.9* %95, i32 0, i32 0
  %97 = load i8*, i8** %96, align 8
  %98 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %92, i8* getelementptr inbounds ([18 x i8], [18 x i8]* @.str.25, i32 0, i32 0), i8* %97)
  store i32 1, i32* %5, align 4
  br label %364

; <label>:99:                                     ; preds = %80
  %100 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %101 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %100, i32 0, i32 2
  %102 = bitcast %union.anon.1* %101 to %struct.anon.9*
  %103 = getelementptr inbounds %struct.anon.9, %struct.anon.9* %102, i32 0, i32 1
  %104 = load %struct.val_t*, %struct.val_t** %103, align 8
  %105 = getelementptr inbounds %struct.val_t, %struct.val_t* %104, i32 0, i32 0
  %106 = load %struct.type_t*, %struct.type_t** %105, align 8
  %107 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %108 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %107, i32 0, i32 1
  store %struct.type_t* %106, %struct.type_t** %108, align 8
  store i32 0, i32* %5, align 4
  br label %364

; <label>:109:                                    ; preds = %15
  %110 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %111 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %110, i32 0, i32 0
  store i32 0, i32* %111, align 8
  %112 = call i8* @malloc(i64 32) #8
  %113 = bitcast i8* %112 to %struct.val_t*
  %114 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %115 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %114, i32 0, i32 2
  %116 = bitcast %union.anon.1* %115 to %struct.anon.2*
  %117 = getelementptr inbounds %struct.anon.2, %struct.anon.2* %116, i32 0, i32 0
  store %struct.val_t* %113, %struct.val_t** %117, align 8
  %118 = load %struct.module_t*, %struct.module_t** %6, align 8
  %119 = load %struct.symbol_table_t*, %struct.symbol_table_t** %7, align 8
  %120 = load %struct.node_t*, %struct.node_t** %8, align 8
  %121 = getelementptr inbounds %struct.node_t, %struct.node_t* %120, i32 0, i32 1
  %122 = bitcast %union.anon.11* %121 to %struct.atom_t**
  %123 = load %struct.atom_t*, %struct.atom_t** %122, align 8
  %124 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %125 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %124, i32 0, i32 2
  %126 = bitcast %union.anon.1* %125 to %struct.anon.2*
  %127 = getelementptr inbounds %struct.anon.2, %struct.anon.2* %126, i32 0, i32 0
  %128 = load %struct.val_t*, %struct.val_t** %127, align 8
  %129 = call i32 @parse_atom(%struct.module_t* %118, %struct.symbol_table_t* %119, %struct.atom_t* %123, %struct.val_t* %128)
  %130 = icmp ne i32 %129, 0
  br i1 %130, label %131, label %132

; <label>:131:                                    ; preds = %109
  store i32 1, i32* %5, align 4
  br label %364

; <label>:132:                                    ; preds = %109
  %133 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %134 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %133, i32 0, i32 2
  %135 = bitcast %union.anon.1* %134 to %struct.anon.2*
  %136 = getelementptr inbounds %struct.anon.2, %struct.anon.2* %135, i32 0, i32 0
  %137 = load %struct.val_t*, %struct.val_t** %136, align 8
  %138 = getelementptr inbounds %struct.val_t, %struct.val_t* %137, i32 0, i32 0
  %139 = load %struct.type_t*, %struct.type_t** %138, align 8
  %140 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %141 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %140, i32 0, i32 1
  store %struct.type_t* %139, %struct.type_t** %141, align 8
  store i32 0, i32* %5, align 4
  br label %364

; <label>:142:                                    ; preds = %4
  %143 = load %struct.node_t*, %struct.node_t** %8, align 8
  %144 = getelementptr inbounds %struct.node_t, %struct.node_t* %143, i32 0, i32 1
  %145 = bitcast %union.anon.11* %144 to %struct.list_t**
  %146 = load %struct.list_t*, %struct.list_t** %145, align 8
  %147 = getelementptr inbounds %struct.list_t, %struct.list_t* %146, i32 0, i32 1
  %148 = load %struct.node_t*, %struct.node_t** %147, align 8
  %149 = getelementptr inbounds %struct.node_t, %struct.node_t* %148, i32 0, i32 0
  %150 = load i32, i32* %149, align 8
  %151 = icmp eq i32 %150, 1
  br i1 %151, label %152, label %155

; <label>:152:                                    ; preds = %142
  %153 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %154 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %153, i8* getelementptr inbounds ([36 x i8], [36 x i8]* @.str.26, i32 0, i32 0))
  store i32 1, i32* %5, align 4
  br label %364

; <label>:155:                                    ; preds = %142
  %156 = load %struct.node_t*, %struct.node_t** %8, align 8
  %157 = getelementptr inbounds %struct.node_t, %struct.node_t* %156, i32 0, i32 1
  %158 = bitcast %union.anon.11* %157 to %struct.list_t**
  %159 = load %struct.list_t*, %struct.list_t** %158, align 8
  %160 = getelementptr inbounds %struct.list_t, %struct.list_t* %159, i32 0, i32 1
  %161 = load %struct.node_t*, %struct.node_t** %160, align 8
  %162 = getelementptr inbounds %struct.node_t, %struct.node_t* %161, i32 0, i32 1
  %163 = bitcast %union.anon.11* %162 to %struct.atom_t**
  %164 = load %struct.atom_t*, %struct.atom_t** %163, align 8
  %165 = getelementptr inbounds %struct.atom_t, %struct.atom_t* %164, i32 0, i32 0
  %166 = load i8*, i8** %165, align 8
  store i8* %166, i8** %10, align 8
  %167 = load i8*, i8** %10, align 8
  %168 = call i32 @strcmp(i8* %167, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.27, i32 0, i32 0))
  %169 = icmp eq i32 %168, 0
  br i1 %169, label %170, label %199

; <label>:170:                                    ; preds = %155
  %171 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %172 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %171, i32 0, i32 0
  store i32 2, i32* %172, align 8
  %173 = call %struct.expr_t* @expr_new_i(i32 3)
  store %struct.expr_t* %173, %struct.expr_t** %11, align 8
  %174 = load %struct.expr_t*, %struct.expr_t** %11, align 8
  %175 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %176 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %175, i32 0, i32 2
  %177 = bitcast %union.anon.1* %176 to %struct.anon.4*
  %178 = getelementptr inbounds %struct.anon.4, %struct.anon.4* %177, i32 0, i32 0
  store %struct.expr_t* %174, %struct.expr_t** %178, align 8
  %179 = load %struct.expr_t*, %struct.expr_t** %11, align 8
  %180 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %179, i64 1
  %181 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %182 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %181, i32 0, i32 2
  %183 = bitcast %union.anon.1* %182 to %struct.anon.4*
  %184 = getelementptr inbounds %struct.anon.4, %struct.anon.4* %183, i32 0, i32 1
  store %struct.expr_t* %180, %struct.expr_t** %184, align 8
  %185 = load %struct.expr_t*, %struct.expr_t** %11, align 8
  %186 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %185, i64 2
  %187 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %188 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %187, i32 0, i32 2
  %189 = bitcast %union.anon.1* %188 to %struct.anon.4*
  %190 = getelementptr inbounds %struct.anon.4, %struct.anon.4* %189, i32 0, i32 2
  store %struct.expr_t* %186, %struct.expr_t** %190, align 8
  %191 = load %struct.module_t*, %struct.module_t** %6, align 8
  %192 = load %struct.symbol_table_t*, %struct.symbol_table_t** %7, align 8
  %193 = load %struct.node_t*, %struct.node_t** %8, align 8
  %194 = getelementptr inbounds %struct.node_t, %struct.node_t* %193, i32 0, i32 1
  %195 = bitcast %union.anon.11* %194 to %struct.list_t**
  %196 = load %struct.list_t*, %struct.list_t** %195, align 8
  %197 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %198 = call i32 @parse_if(%struct.module_t* %191, %struct.symbol_table_t* %192, %struct.list_t* %196, %struct.expr_t* %197)
  store i32 %198, i32* %5, align 4
  br label %364

; <label>:199:                                    ; preds = %155
  %200 = load i8*, i8** %10, align 8
  %201 = call i32 @strcmp(i8* %200, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.28, i32 0, i32 0))
  %202 = icmp eq i32 %201, 0
  br i1 %202, label %203, label %214

; <label>:203:                                    ; preds = %199
  %204 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %205 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %204, i32 0, i32 0
  store i32 3, i32* %205, align 8
  %206 = load %struct.module_t*, %struct.module_t** %6, align 8
  %207 = load %struct.symbol_table_t*, %struct.symbol_table_t** %7, align 8
  %208 = load %struct.node_t*, %struct.node_t** %8, align 8
  %209 = getelementptr inbounds %struct.node_t, %struct.node_t* %208, i32 0, i32 1
  %210 = bitcast %union.anon.11* %209 to %struct.list_t**
  %211 = load %struct.list_t*, %struct.list_t** %210, align 8
  %212 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %213 = call i32 @parse_let(%struct.module_t* %206, %struct.symbol_table_t* %207, %struct.list_t* %211, %struct.expr_t* %212)
  store i32 %213, i32* %5, align 4
  br label %364

; <label>:214:                                    ; preds = %199
  %215 = load i8*, i8** %10, align 8
  %216 = call i32 @strcmp(i8* %215, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.29, i32 0, i32 0))
  %217 = icmp eq i32 %216, 0
  br i1 %217, label %218, label %244

; <label>:218:                                    ; preds = %214
  %219 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %220 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %219, i32 0, i32 0
  store i32 5, i32* %220, align 8
  %221 = load %struct.node_t*, %struct.node_t** %8, align 8
  %222 = getelementptr inbounds %struct.node_t, %struct.node_t* %221, i32 0, i32 1
  %223 = bitcast %union.anon.11* %222 to %struct.list_t**
  %224 = load %struct.list_t*, %struct.list_t** %223, align 8
  %225 = getelementptr inbounds %struct.list_t, %struct.list_t* %224, i32 0, i32 0
  %226 = load i32, i32* %225, align 8
  %227 = sub nsw i32 %226, 1
  %228 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %229 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %228, i32 0, i32 2
  %230 = bitcast %union.anon.1* %229 to %struct.anon.7*
  %231 = getelementptr inbounds %struct.anon.7, %struct.anon.7* %230, i32 0, i32 0
  store i32 %227, i32* %231, align 8
  %232 = load %struct.module_t*, %struct.module_t** %6, align 8
  %233 = load %struct.symbol_table_t*, %struct.symbol_table_t** %7, align 8
  %234 = load %struct.node_t*, %struct.node_t** %8, align 8
  %235 = getelementptr inbounds %struct.node_t, %struct.node_t* %234, i32 0, i32 1
  %236 = bitcast %union.anon.11* %235 to %struct.list_t**
  %237 = load %struct.list_t*, %struct.list_t** %236, align 8
  %238 = getelementptr inbounds %struct.list_t, %struct.list_t* %237, i32 0, i32 1
  %239 = load %struct.node_t*, %struct.node_t** %238, align 8
  %240 = getelementptr inbounds %struct.node_t, %struct.node_t* %239, i32 0, i32 2
  %241 = load %struct.node_t*, %struct.node_t** %240, align 8
  %242 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %243 = call i32 @parse_progn(%struct.module_t* %232, %struct.symbol_table_t* %233, %struct.node_t* %241, %struct.expr_t* %242)
  store i32 %243, i32* %5, align 4
  br label %364

; <label>:244:                                    ; preds = %214
  %245 = load i8*, i8** %10, align 8
  %246 = call i32 @strcmp(i8* %245, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.30, i32 0, i32 0))
  %247 = icmp eq i32 %246, 0
  br i1 %247, label %248, label %298

; <label>:248:                                    ; preds = %244
  %249 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %250 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %249, i32 0, i32 0
  store i32 6, i32* %250, align 8
  %251 = call %struct.expr_t* @expr_new()
  %252 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %253 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %252, i32 0, i32 2
  %254 = bitcast %union.anon.1* %253 to %struct.anon.8*
  %255 = getelementptr inbounds %struct.anon.8, %struct.anon.8* %254, i32 0, i32 1
  store %struct.expr_t* %251, %struct.expr_t** %255, align 8
  %256 = load %struct.node_t*, %struct.node_t** %8, align 8
  %257 = getelementptr inbounds %struct.node_t, %struct.node_t* %256, i32 0, i32 1
  %258 = bitcast %union.anon.11* %257 to %struct.list_t**
  %259 = load %struct.list_t*, %struct.list_t** %258, align 8
  %260 = getelementptr inbounds %struct.list_t, %struct.list_t* %259, i32 0, i32 0
  %261 = load i32, i32* %260, align 8
  %262 = sub nsw i32 %261, 2
  %263 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %264 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %263, i32 0, i32 2
  %265 = bitcast %union.anon.1* %264 to %struct.anon.8*
  %266 = getelementptr inbounds %struct.anon.8, %struct.anon.8* %265, i32 0, i32 0
  store i32 %262, i32* %266, align 8
  %267 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %268 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %267, i32 0, i32 2
  %269 = bitcast %union.anon.1* %268 to %struct.anon.8*
  %270 = getelementptr inbounds %struct.anon.8, %struct.anon.8* %269, i32 0, i32 0
  %271 = load i32, i32* %270, align 8
  %272 = call %struct.expr_t* @expr_new_i(i32 %271)
  %273 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %274 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %273, i32 0, i32 2
  %275 = bitcast %union.anon.1* %274 to %struct.anon.8*
  %276 = getelementptr inbounds %struct.anon.8, %struct.anon.8* %275, i32 0, i32 2
  store %struct.expr_t* %272, %struct.expr_t** %276, align 8
  %277 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %278 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %277, i32 0, i32 2
  %279 = bitcast %union.anon.1* %278 to %struct.anon.8*
  %280 = getelementptr inbounds %struct.anon.8, %struct.anon.8* %279, i32 0, i32 0
  %281 = load i32, i32* %280, align 8
  %282 = sext i32 %281 to i64
  %283 = mul i64 16, %282
  %284 = call i8* @malloc(i64 %283) #8
  %285 = bitcast i8* %284 to %struct.val_list_t*
  %286 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %287 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %286, i32 0, i32 2
  %288 = bitcast %union.anon.1* %287 to %struct.anon.8*
  %289 = getelementptr inbounds %struct.anon.8, %struct.anon.8* %288, i32 0, i32 3
  store %struct.val_list_t* %285, %struct.val_list_t** %289, align 8
  %290 = load %struct.module_t*, %struct.module_t** %6, align 8
  %291 = load %struct.symbol_table_t*, %struct.symbol_table_t** %7, align 8
  %292 = load %struct.node_t*, %struct.node_t** %8, align 8
  %293 = getelementptr inbounds %struct.node_t, %struct.node_t* %292, i32 0, i32 1
  %294 = bitcast %union.anon.11* %293 to %struct.list_t**
  %295 = load %struct.list_t*, %struct.list_t** %294, align 8
  %296 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %297 = call i32 @parse_switch(%struct.module_t* %290, %struct.symbol_table_t* %291, %struct.list_t* %295, %struct.expr_t* %296)
  store i32 %297, i32* %5, align 4
  br label %364

; <label>:298:                                    ; preds = %244
  %299 = load i8*, i8** %10, align 8
  %300 = call i32 @strcmp(i8* %299, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.31, i32 0, i32 0))
  %301 = icmp eq i32 %300, 0
  br i1 %301, label %302, label %353

; <label>:302:                                    ; preds = %298
  %303 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %304 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %303, i32 0, i32 0
  store i32 4, i32* %304, align 8
  %305 = call %struct.expr_t* @expr_new()
  %306 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %307 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %306, i32 0, i32 2
  %308 = bitcast %union.anon.1* %307 to %struct.anon.6*
  %309 = getelementptr inbounds %struct.anon.6, %struct.anon.6* %308, i32 0, i32 1
  store %struct.expr_t* %305, %struct.expr_t** %309, align 8
  %310 = load %struct.node_t*, %struct.node_t** %8, align 8
  %311 = getelementptr inbounds %struct.node_t, %struct.node_t* %310, i32 0, i32 1
  %312 = bitcast %union.anon.11* %311 to %struct.list_t**
  %313 = load %struct.list_t*, %struct.list_t** %312, align 8
  %314 = getelementptr inbounds %struct.list_t, %struct.list_t* %313, i32 0, i32 0
  %315 = load i32, i32* %314, align 8
  %316 = sub nsw i32 %315, 2
  %317 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %318 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %317, i32 0, i32 2
  %319 = bitcast %union.anon.1* %318 to %struct.anon.6*
  %320 = getelementptr inbounds %struct.anon.6, %struct.anon.6* %319, i32 0, i32 0
  store i32 %316, i32* %320, align 8
  %321 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %322 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %321, i32 0, i32 2
  %323 = bitcast %union.anon.1* %322 to %struct.anon.6*
  %324 = getelementptr inbounds %struct.anon.6, %struct.anon.6* %323, i32 0, i32 0
  %325 = load i32, i32* %324, align 8
  %326 = call %struct.expr_t* @expr_new_i(i32 %325)
  %327 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %328 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %327, i32 0, i32 2
  %329 = bitcast %union.anon.1* %328 to %struct.anon.6*
  %330 = getelementptr inbounds %struct.anon.6, %struct.anon.6* %329, i32 0, i32 2
  store %struct.expr_t* %326, %struct.expr_t** %330, align 8
  %331 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %332 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %331, i32 0, i32 2
  %333 = bitcast %union.anon.1* %332 to %struct.anon.6*
  %334 = getelementptr inbounds %struct.anon.6, %struct.anon.6* %333, i32 0, i32 0
  %335 = load i32, i32* %334, align 8
  %336 = call %struct.expr_t* @expr_new_i(i32 %335)
  %337 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %338 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %337, i32 0, i32 2
  %339 = bitcast %union.anon.1* %338 to %struct.anon.6*
  %340 = getelementptr inbounds %struct.anon.6, %struct.anon.6* %339, i32 0, i32 3
  store %struct.expr_t* %336, %struct.expr_t** %340, align 8
  %341 = load %struct.module_t*, %struct.module_t** %6, align 8
  %342 = load %struct.symbol_table_t*, %struct.symbol_table_t** %7, align 8
  %343 = load %struct.node_t*, %struct.node_t** %8, align 8
  %344 = getelementptr inbounds %struct.node_t, %struct.node_t* %343, i32 0, i32 1
  %345 = bitcast %union.anon.11* %344 to %struct.list_t**
  %346 = load %struct.list_t*, %struct.list_t** %345, align 8
  %347 = getelementptr inbounds %struct.list_t, %struct.list_t* %346, i32 0, i32 1
  %348 = load %struct.node_t*, %struct.node_t** %347, align 8
  %349 = getelementptr inbounds %struct.node_t, %struct.node_t* %348, i32 0, i32 2
  %350 = load %struct.node_t*, %struct.node_t** %349, align 8
  %351 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %352 = call i32 @parse_match(%struct.module_t* %341, %struct.symbol_table_t* %342, %struct.node_t* %350, %struct.expr_t* %351)
  store i32 %352, i32* %5, align 4
  br label %364

; <label>:353:                                    ; preds = %298
  %354 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %355 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %354, i32 0, i32 0
  store i32 1, i32* %355, align 8
  %356 = load %struct.module_t*, %struct.module_t** %6, align 8
  %357 = load %struct.symbol_table_t*, %struct.symbol_table_t** %7, align 8
  %358 = load %struct.node_t*, %struct.node_t** %8, align 8
  %359 = getelementptr inbounds %struct.node_t, %struct.node_t* %358, i32 0, i32 1
  %360 = bitcast %union.anon.11* %359 to %struct.list_t**
  %361 = load %struct.list_t*, %struct.list_t** %360, align 8
  %362 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %363 = call i32 @parse_funcall(%struct.module_t* %356, %struct.symbol_table_t* %357, %struct.list_t* %361, %struct.expr_t* %362)
  store i32 %363, i32* %5, align 4
  br label %364

; <label>:364:                                    ; preds = %72, %91, %99, %131, %132, %152, %170, %203, %218, %248, %302, %353, %4
  %365 = load i32, i32* %5, align 4
  ret i32 %365
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @parse_let(%struct.module_t*, %struct.symbol_table_t*, %struct.list_t*, %struct.expr_t*) #0 {
  %5 = alloca i32, align 4
  %6 = alloca %struct.module_t*, align 8
  %7 = alloca %struct.symbol_table_t*, align 8
  %8 = alloca %struct.list_t*, align 8
  %9 = alloca %struct.expr_t*, align 8
  %10 = alloca %struct.node_t*, align 8
  %11 = alloca %struct.symbol_table_t*, align 8
  %12 = alloca %struct.node_t*, align 8
  %13 = alloca %struct.node_t*, align 8
  %14 = alloca %struct.expr_t*, align 8
  %15 = alloca i32, align 4
  store %struct.module_t* %0, %struct.module_t** %6, align 8
  store %struct.symbol_table_t* %1, %struct.symbol_table_t** %7, align 8
  store %struct.list_t* %2, %struct.list_t** %8, align 8
  store %struct.expr_t* %3, %struct.expr_t** %9, align 8
  %16 = load %struct.list_t*, %struct.list_t** %8, align 8
  %17 = getelementptr inbounds %struct.list_t, %struct.list_t* %16, i32 0, i32 0
  %18 = load i32, i32* %17, align 8
  %19 = icmp slt i32 %18, 3
  br i1 %19, label %20, label %23

; <label>:20:                                     ; preds = %4
  %21 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %22 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %21, i8* getelementptr inbounds ([38 x i8], [38 x i8]* @.str.11, i32 0, i32 0))
  store i32 1, i32* %5, align 4
  br label %194

; <label>:23:                                     ; preds = %4
  %24 = load %struct.list_t*, %struct.list_t** %8, align 8
  %25 = getelementptr inbounds %struct.list_t, %struct.list_t* %24, i32 0, i32 1
  %26 = load %struct.node_t*, %struct.node_t** %25, align 8
  %27 = getelementptr inbounds %struct.node_t, %struct.node_t* %26, i32 0, i32 2
  %28 = load %struct.node_t*, %struct.node_t** %27, align 8
  store %struct.node_t* %28, %struct.node_t** %10, align 8
  %29 = load %struct.node_t*, %struct.node_t** %10, align 8
  %30 = getelementptr inbounds %struct.node_t, %struct.node_t* %29, i32 0, i32 0
  %31 = load i32, i32* %30, align 8
  %32 = icmp ne i32 %31, 1
  br i1 %32, label %33, label %36

; <label>:33:                                     ; preds = %23
  %34 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %35 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %34, i8* getelementptr inbounds ([49 x i8], [49 x i8]* @.str.12, i32 0, i32 0))
  store i32 1, i32* %5, align 4
  br label %194

; <label>:36:                                     ; preds = %23
  %37 = load %struct.symbol_table_t*, %struct.symbol_table_t** %7, align 8
  %38 = load %struct.node_t*, %struct.node_t** %10, align 8
  %39 = getelementptr inbounds %struct.node_t, %struct.node_t* %38, i32 0, i32 1
  %40 = bitcast %union.anon.11* %39 to %struct.list_t**
  %41 = load %struct.list_t*, %struct.list_t** %40, align 8
  %42 = getelementptr inbounds %struct.list_t, %struct.list_t* %41, i32 0, i32 0
  %43 = load i32, i32* %42, align 8
  %44 = call %struct.symbol_table_t* @symbol_table_new(%struct.symbol_table_t* %37, i32 %43)
  store %struct.symbol_table_t* %44, %struct.symbol_table_t** %11, align 8
  %45 = load %struct.symbol_table_t*, %struct.symbol_table_t** %11, align 8
  %46 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %47 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %46, i32 0, i32 2
  %48 = bitcast %union.anon.1* %47 to %struct.anon.5*
  %49 = getelementptr inbounds %struct.anon.5, %struct.anon.5* %48, i32 0, i32 0
  store %struct.symbol_table_t* %45, %struct.symbol_table_t** %49, align 8
  %50 = load %struct.node_t*, %struct.node_t** %10, align 8
  %51 = getelementptr inbounds %struct.node_t, %struct.node_t* %50, i32 0, i32 1
  %52 = bitcast %union.anon.11* %51 to %struct.list_t**
  %53 = load %struct.list_t*, %struct.list_t** %52, align 8
  %54 = getelementptr inbounds %struct.list_t, %struct.list_t* %53, i32 0, i32 1
  %55 = load %struct.node_t*, %struct.node_t** %54, align 8
  store %struct.node_t* %55, %struct.node_t** %12, align 8
  store i32 0, i32* %15, align 4
  br label %56

; <label>:56:                                     ; preds = %132, %36
  %57 = load i32, i32* %15, align 4
  %58 = load %struct.symbol_table_t*, %struct.symbol_table_t** %11, align 8
  %59 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %58, i32 0, i32 1
  %60 = load i32, i32* %59, align 8
  %61 = icmp slt i32 %57, %60
  br i1 %61, label %62, label %135

; <label>:62:                                     ; preds = %56
  %63 = load %struct.node_t*, %struct.node_t** %12, align 8
  %64 = getelementptr inbounds %struct.node_t, %struct.node_t* %63, i32 0, i32 0
  %65 = load i32, i32* %64, align 8
  %66 = icmp ne i32 %65, 1
  br i1 %66, label %67, label %70

; <label>:67:                                     ; preds = %62
  %68 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %69 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %68, i8* getelementptr inbounds ([21 x i8], [21 x i8]* @.str.13, i32 0, i32 0))
  store i32 1, i32* %5, align 4
  br label %194

; <label>:70:                                     ; preds = %62
  %71 = load %struct.node_t*, %struct.node_t** %12, align 8
  %72 = getelementptr inbounds %struct.node_t, %struct.node_t* %71, i32 0, i32 1
  %73 = bitcast %union.anon.11* %72 to %struct.list_t**
  %74 = load %struct.list_t*, %struct.list_t** %73, align 8
  %75 = getelementptr inbounds %struct.list_t, %struct.list_t* %74, i32 0, i32 1
  %76 = load %struct.node_t*, %struct.node_t** %75, align 8
  store %struct.node_t* %76, %struct.node_t** %13, align 8
  %77 = load %struct.node_t*, %struct.node_t** %13, align 8
  %78 = getelementptr inbounds %struct.node_t, %struct.node_t* %77, i32 0, i32 0
  %79 = load i32, i32* %78, align 8
  %80 = icmp ne i32 %79, 0
  br i1 %80, label %81, label %84

; <label>:81:                                     ; preds = %70
  %82 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %83 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %82, i8* getelementptr inbounds ([27 x i8], [27 x i8]* @.str.14, i32 0, i32 0))
  store i32 1, i32* %5, align 4
  br label %194

; <label>:84:                                     ; preds = %70
  %85 = load %struct.node_t*, %struct.node_t** %13, align 8
  %86 = getelementptr inbounds %struct.node_t, %struct.node_t* %85, i32 0, i32 1
  %87 = bitcast %union.anon.11* %86 to %struct.atom_t**
  %88 = load %struct.atom_t*, %struct.atom_t** %87, align 8
  %89 = getelementptr inbounds %struct.atom_t, %struct.atom_t* %88, i32 0, i32 0
  %90 = load i8*, i8** %89, align 8
  %91 = load %struct.symbol_table_t*, %struct.symbol_table_t** %11, align 8
  %92 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %91, i32 0, i32 3
  %93 = load i8**, i8*** %92, align 8
  %94 = load i32, i32* %15, align 4
  %95 = sext i32 %94 to i64
  %96 = getelementptr inbounds i8*, i8** %93, i64 %95
  store i8* %90, i8** %96, align 8
  %97 = load %struct.node_t*, %struct.node_t** %13, align 8
  %98 = getelementptr inbounds %struct.node_t, %struct.node_t* %97, i32 0, i32 2
  %99 = load %struct.node_t*, %struct.node_t** %98, align 8
  store %struct.node_t* %99, %struct.node_t** %13, align 8
  %100 = call %struct.expr_t* @expr_new()
  store %struct.expr_t* %100, %struct.expr_t** %14, align 8
  %101 = load %struct.module_t*, %struct.module_t** %6, align 8
  %102 = load %struct.symbol_table_t*, %struct.symbol_table_t** %7, align 8
  %103 = load %struct.node_t*, %struct.node_t** %13, align 8
  %104 = load %struct.expr_t*, %struct.expr_t** %14, align 8
  %105 = call i32 @parse_expr(%struct.module_t* %101, %struct.symbol_table_t* %102, %struct.node_t* %103, %struct.expr_t* %104)
  %106 = icmp ne i32 %105, 0
  br i1 %106, label %107, label %108

; <label>:107:                                    ; preds = %84
  store i32 1, i32* %5, align 4
  br label %194

; <label>:108:                                    ; preds = %84
  %109 = load %struct.expr_t*, %struct.expr_t** %14, align 8
  %110 = load %struct.symbol_table_t*, %struct.symbol_table_t** %11, align 8
  %111 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %110, i32 0, i32 4
  %112 = load %struct.val_t*, %struct.val_t** %111, align 8
  %113 = load i32, i32* %15, align 4
  %114 = sext i32 %113 to i64
  %115 = getelementptr inbounds %struct.val_t, %struct.val_t* %112, i64 %114
  %116 = getelementptr inbounds %struct.val_t, %struct.val_t* %115, i32 0, i32 1
  %117 = bitcast %union.anon* %116 to %struct.anon.0*
  %118 = getelementptr inbounds %struct.anon.0, %struct.anon.0* %117, i32 0, i32 0
  store %struct.expr_t* %109, %struct.expr_t** %118, align 8
  %119 = load %struct.expr_t*, %struct.expr_t** %14, align 8
  %120 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %119, i32 0, i32 1
  %121 = load %struct.type_t*, %struct.type_t** %120, align 8
  %122 = load %struct.symbol_table_t*, %struct.symbol_table_t** %11, align 8
  %123 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %122, i32 0, i32 4
  %124 = load %struct.val_t*, %struct.val_t** %123, align 8
  %125 = load i32, i32* %15, align 4
  %126 = sext i32 %125 to i64
  %127 = getelementptr inbounds %struct.val_t, %struct.val_t* %124, i64 %126
  %128 = getelementptr inbounds %struct.val_t, %struct.val_t* %127, i32 0, i32 0
  store %struct.type_t* %121, %struct.type_t** %128, align 8
  %129 = load %struct.node_t*, %struct.node_t** %12, align 8
  %130 = getelementptr inbounds %struct.node_t, %struct.node_t* %129, i32 0, i32 2
  %131 = load %struct.node_t*, %struct.node_t** %130, align 8
  store %struct.node_t* %131, %struct.node_t** %12, align 8
  br label %132

; <label>:132:                                    ; preds = %108
  %133 = load i32, i32* %15, align 4
  %134 = add nsw i32 %133, 1
  store i32 %134, i32* %15, align 4
  br label %56

; <label>:135:                                    ; preds = %56
  %136 = call %struct.expr_t* @expr_new()
  %137 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %138 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %137, i32 0, i32 2
  %139 = bitcast %union.anon.1* %138 to %struct.anon.5*
  %140 = getelementptr inbounds %struct.anon.5, %struct.anon.5* %139, i32 0, i32 1
  store %struct.expr_t* %136, %struct.expr_t** %140, align 8
  %141 = load %struct.list_t*, %struct.list_t** %8, align 8
  %142 = getelementptr inbounds %struct.list_t, %struct.list_t* %141, i32 0, i32 0
  %143 = load i32, i32* %142, align 8
  %144 = icmp eq i32 %143, 3
  br i1 %144, label %145, label %160

; <label>:145:                                    ; preds = %135
  %146 = load %struct.module_t*, %struct.module_t** %6, align 8
  %147 = load %struct.symbol_table_t*, %struct.symbol_table_t** %11, align 8
  %148 = load %struct.node_t*, %struct.node_t** %10, align 8
  %149 = getelementptr inbounds %struct.node_t, %struct.node_t* %148, i32 0, i32 2
  %150 = load %struct.node_t*, %struct.node_t** %149, align 8
  %151 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %152 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %151, i32 0, i32 2
  %153 = bitcast %union.anon.1* %152 to %struct.anon.5*
  %154 = getelementptr inbounds %struct.anon.5, %struct.anon.5* %153, i32 0, i32 1
  %155 = load %struct.expr_t*, %struct.expr_t** %154, align 8
  %156 = call i32 @parse_expr(%struct.module_t* %146, %struct.symbol_table_t* %147, %struct.node_t* %150, %struct.expr_t* %155)
  %157 = icmp ne i32 %156, 0
  br i1 %157, label %158, label %159

; <label>:158:                                    ; preds = %145
  store i32 1, i32* %5, align 4
  br label %194

; <label>:159:                                    ; preds = %145
  br label %193

; <label>:160:                                    ; preds = %135
  %161 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %162 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %161, i32 0, i32 2
  %163 = bitcast %union.anon.1* %162 to %struct.anon.5*
  %164 = getelementptr inbounds %struct.anon.5, %struct.anon.5* %163, i32 0, i32 1
  %165 = load %struct.expr_t*, %struct.expr_t** %164, align 8
  %166 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %165, i32 0, i32 0
  store i32 5, i32* %166, align 8
  %167 = load %struct.list_t*, %struct.list_t** %8, align 8
  %168 = getelementptr inbounds %struct.list_t, %struct.list_t* %167, i32 0, i32 0
  %169 = load i32, i32* %168, align 8
  %170 = sub nsw i32 %169, 2
  %171 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %172 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %171, i32 0, i32 2
  %173 = bitcast %union.anon.1* %172 to %struct.anon.5*
  %174 = getelementptr inbounds %struct.anon.5, %struct.anon.5* %173, i32 0, i32 1
  %175 = load %struct.expr_t*, %struct.expr_t** %174, align 8
  %176 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %175, i32 0, i32 2
  %177 = bitcast %union.anon.1* %176 to %struct.anon.7*
  %178 = getelementptr inbounds %struct.anon.7, %struct.anon.7* %177, i32 0, i32 0
  store i32 %170, i32* %178, align 8
  %179 = load %struct.module_t*, %struct.module_t** %6, align 8
  %180 = load %struct.symbol_table_t*, %struct.symbol_table_t** %11, align 8
  %181 = load %struct.node_t*, %struct.node_t** %10, align 8
  %182 = getelementptr inbounds %struct.node_t, %struct.node_t* %181, i32 0, i32 2
  %183 = load %struct.node_t*, %struct.node_t** %182, align 8
  %184 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %185 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %184, i32 0, i32 2
  %186 = bitcast %union.anon.1* %185 to %struct.anon.5*
  %187 = getelementptr inbounds %struct.anon.5, %struct.anon.5* %186, i32 0, i32 1
  %188 = load %struct.expr_t*, %struct.expr_t** %187, align 8
  %189 = call i32 @parse_progn(%struct.module_t* %179, %struct.symbol_table_t* %180, %struct.node_t* %183, %struct.expr_t* %188)
  %190 = icmp ne i32 %189, 0
  br i1 %190, label %191, label %192

; <label>:191:                                    ; preds = %160
  store i32 1, i32* %5, align 4
  br label %194

; <label>:192:                                    ; preds = %160
  br label %193

; <label>:193:                                    ; preds = %192, %159
  store i32 0, i32* %5, align 4
  br label %194

; <label>:194:                                    ; preds = %193, %191, %158, %107, %81, %67, %33, %20
  %195 = load i32, i32* %5, align 4
  ret i32 %195
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define %struct.symbol_table_t* @symbol_table_new(%struct.symbol_table_t*, i32) #0 {
  %3 = alloca %struct.symbol_table_t*, align 8
  %4 = alloca i32, align 4
  %5 = alloca %struct.symbol_table_t*, align 8
  store %struct.symbol_table_t* %0, %struct.symbol_table_t** %3, align 8
  store i32 %1, i32* %4, align 4
  %6 = call i8* @malloc(i64 32) #8
  %7 = bitcast i8* %6 to %struct.symbol_table_t*
  store %struct.symbol_table_t* %7, %struct.symbol_table_t** %5, align 8
  %8 = load %struct.symbol_table_t*, %struct.symbol_table_t** %3, align 8
  %9 = load %struct.symbol_table_t*, %struct.symbol_table_t** %5, align 8
  %10 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %9, i32 0, i32 0
  store %struct.symbol_table_t* %8, %struct.symbol_table_t** %10, align 8
  %11 = load i32, i32* %4, align 4
  %12 = load %struct.symbol_table_t*, %struct.symbol_table_t** %5, align 8
  %13 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %12, i32 0, i32 1
  store i32 %11, i32* %13, align 8
  %14 = load i32, i32* %4, align 4
  %15 = load %struct.symbol_table_t*, %struct.symbol_table_t** %5, align 8
  %16 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %15, i32 0, i32 2
  store i32 %14, i32* %16, align 4
  %17 = load i32, i32* %4, align 4
  %18 = sext i32 %17 to i64
  %19 = mul i64 8, %18
  %20 = call i8* @malloc(i64 %19) #8
  %21 = bitcast i8* %20 to i8**
  %22 = load %struct.symbol_table_t*, %struct.symbol_table_t** %5, align 8
  %23 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %22, i32 0, i32 3
  store i8** %21, i8*** %23, align 8
  %24 = load i32, i32* %4, align 4
  %25 = sext i32 %24 to i64
  %26 = mul i64 32, %25
  %27 = call i8* @malloc(i64 %26) #8
  %28 = bitcast i8* %27 to %struct.val_t*
  %29 = load %struct.symbol_table_t*, %struct.symbol_table_t** %5, align 8
  %30 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %29, i32 0, i32 4
  store %struct.val_t* %28, %struct.val_t** %30, align 8
  %31 = load %struct.symbol_table_t*, %struct.symbol_table_t** %5, align 8
  ret %struct.symbol_table_t* %31
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @parse_progn(%struct.module_t*, %struct.symbol_table_t*, %struct.node_t*, %struct.expr_t*) #0 {
  %5 = alloca i32, align 4
  %6 = alloca %struct.module_t*, align 8
  %7 = alloca %struct.symbol_table_t*, align 8
  %8 = alloca %struct.node_t*, align 8
  %9 = alloca %struct.expr_t*, align 8
  %10 = alloca %struct.expr_t*, align 8
  store %struct.module_t* %0, %struct.module_t** %6, align 8
  store %struct.symbol_table_t* %1, %struct.symbol_table_t** %7, align 8
  store %struct.node_t* %2, %struct.node_t** %8, align 8
  store %struct.expr_t* %3, %struct.expr_t** %9, align 8
  %11 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %12 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %11, i32 0, i32 2
  %13 = bitcast %union.anon.1* %12 to %struct.anon.7*
  %14 = getelementptr inbounds %struct.anon.7, %struct.anon.7* %13, i32 0, i32 0
  %15 = load i32, i32* %14, align 8
  %16 = call %struct.expr_t* @expr_new_i(i32 %15)
  %17 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %18 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %17, i32 0, i32 2
  %19 = bitcast %union.anon.1* %18 to %struct.anon.7*
  %20 = getelementptr inbounds %struct.anon.7, %struct.anon.7* %19, i32 0, i32 1
  store %struct.expr_t* %16, %struct.expr_t** %20, align 8
  %21 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %22 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %21, i32 0, i32 2
  %23 = bitcast %union.anon.1* %22 to %struct.anon.7*
  %24 = getelementptr inbounds %struct.anon.7, %struct.anon.7* %23, i32 0, i32 1
  %25 = load %struct.expr_t*, %struct.expr_t** %24, align 8
  store %struct.expr_t* %25, %struct.expr_t** %10, align 8
  br label %26

; <label>:26:                                     ; preds = %53, %4
  %27 = load %struct.expr_t*, %struct.expr_t** %10, align 8
  %28 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %29 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %28, i32 0, i32 2
  %30 = bitcast %union.anon.1* %29 to %struct.anon.7*
  %31 = getelementptr inbounds %struct.anon.7, %struct.anon.7* %30, i32 0, i32 1
  %32 = load %struct.expr_t*, %struct.expr_t** %31, align 8
  %33 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %34 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %33, i32 0, i32 2
  %35 = bitcast %union.anon.1* %34 to %struct.anon.7*
  %36 = getelementptr inbounds %struct.anon.7, %struct.anon.7* %35, i32 0, i32 0
  %37 = load i32, i32* %36, align 8
  %38 = sext i32 %37 to i64
  %39 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %32, i64 %38
  %40 = icmp ult %struct.expr_t* %27, %39
  br i1 %40, label %41, label %56

; <label>:41:                                     ; preds = %26
  %42 = load %struct.module_t*, %struct.module_t** %6, align 8
  %43 = load %struct.symbol_table_t*, %struct.symbol_table_t** %7, align 8
  %44 = load %struct.node_t*, %struct.node_t** %8, align 8
  %45 = load %struct.expr_t*, %struct.expr_t** %10, align 8
  %46 = call i32 @parse_expr(%struct.module_t* %42, %struct.symbol_table_t* %43, %struct.node_t* %44, %struct.expr_t* %45)
  %47 = icmp ne i32 %46, 0
  br i1 %47, label %48, label %49

; <label>:48:                                     ; preds = %41
  store i32 1, i32* %5, align 4
  br label %62

; <label>:49:                                     ; preds = %41
  %50 = load %struct.node_t*, %struct.node_t** %8, align 8
  %51 = getelementptr inbounds %struct.node_t, %struct.node_t* %50, i32 0, i32 2
  %52 = load %struct.node_t*, %struct.node_t** %51, align 8
  store %struct.node_t* %52, %struct.node_t** %8, align 8
  br label %53

; <label>:53:                                     ; preds = %49
  %54 = load %struct.expr_t*, %struct.expr_t** %10, align 8
  %55 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %54, i32 1
  store %struct.expr_t* %55, %struct.expr_t** %10, align 8
  br label %26

; <label>:56:                                     ; preds = %26
  %57 = load %struct.expr_t*, %struct.expr_t** %10, align 8
  %58 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %57, i32 0, i32 1
  %59 = load %struct.type_t*, %struct.type_t** %58, align 8
  %60 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %61 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %60, i32 0, i32 1
  store %struct.type_t* %59, %struct.type_t** %61, align 8
  store i32 0, i32* %5, align 4
  br label %62

; <label>:62:                                     ; preds = %56, %48
  %63 = load i32, i32* %5, align 4
  ret i32 %63
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @parse_switch(%struct.module_t*, %struct.symbol_table_t*, %struct.list_t*, %struct.expr_t*) #0 {
  %5 = alloca i32, align 4
  %6 = alloca %struct.module_t*, align 8
  %7 = alloca %struct.symbol_table_t*, align 8
  %8 = alloca %struct.list_t*, align 8
  %9 = alloca %struct.expr_t*, align 8
  %10 = alloca %struct.node_t*, align 8
  %11 = alloca %struct.node_t*, align 8
  %12 = alloca %struct.expr_t*, align 8
  %13 = alloca %struct.val_list_t*, align 8
  %14 = alloca i32, align 4
  %15 = alloca %struct.node_t*, align 8
  %16 = alloca i32, align 4
  store %struct.module_t* %0, %struct.module_t** %6, align 8
  store %struct.symbol_table_t* %1, %struct.symbol_table_t** %7, align 8
  store %struct.list_t* %2, %struct.list_t** %8, align 8
  store %struct.expr_t* %3, %struct.expr_t** %9, align 8
  %17 = load %struct.list_t*, %struct.list_t** %8, align 8
  %18 = getelementptr inbounds %struct.list_t, %struct.list_t* %17, i32 0, i32 1
  %19 = load %struct.node_t*, %struct.node_t** %18, align 8
  %20 = getelementptr inbounds %struct.node_t, %struct.node_t* %19, i32 0, i32 2
  %21 = load %struct.node_t*, %struct.node_t** %20, align 8
  store %struct.node_t* %21, %struct.node_t** %10, align 8
  %22 = load %struct.module_t*, %struct.module_t** %6, align 8
  %23 = load %struct.symbol_table_t*, %struct.symbol_table_t** %7, align 8
  %24 = load %struct.node_t*, %struct.node_t** %10, align 8
  %25 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %26 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %25, i32 0, i32 2
  %27 = bitcast %union.anon.1* %26 to %struct.anon.8*
  %28 = getelementptr inbounds %struct.anon.8, %struct.anon.8* %27, i32 0, i32 1
  %29 = load %struct.expr_t*, %struct.expr_t** %28, align 8
  %30 = call i32 @parse_expr(%struct.module_t* %22, %struct.symbol_table_t* %23, %struct.node_t* %24, %struct.expr_t* %29)
  %31 = icmp ne i32 %30, 0
  br i1 %31, label %32, label %33

; <label>:32:                                     ; preds = %4
  store i32 1, i32* %5, align 4
  br label %182

; <label>:33:                                     ; preds = %4
  %34 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %35 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %34, i32 0, i32 2
  %36 = bitcast %union.anon.1* %35 to %struct.anon.8*
  %37 = getelementptr inbounds %struct.anon.8, %struct.anon.8* %36, i32 0, i32 2
  %38 = load %struct.expr_t*, %struct.expr_t** %37, align 8
  store %struct.expr_t* %38, %struct.expr_t** %12, align 8
  %39 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %40 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %39, i32 0, i32 2
  %41 = bitcast %union.anon.1* %40 to %struct.anon.8*
  %42 = getelementptr inbounds %struct.anon.8, %struct.anon.8* %41, i32 0, i32 3
  %43 = load %struct.val_list_t*, %struct.val_list_t** %42, align 8
  store %struct.val_list_t* %43, %struct.val_list_t** %13, align 8
  store i32 0, i32* %14, align 4
  br label %44

; <label>:44:                                     ; preds = %178, %33
  %45 = load i32, i32* %14, align 4
  %46 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %47 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %46, i32 0, i32 2
  %48 = bitcast %union.anon.1* %47 to %struct.anon.8*
  %49 = getelementptr inbounds %struct.anon.8, %struct.anon.8* %48, i32 0, i32 0
  %50 = load i32, i32* %49, align 8
  %51 = icmp slt i32 %45, %50
  br i1 %51, label %52, label %181

; <label>:52:                                     ; preds = %44
  %53 = load %struct.node_t*, %struct.node_t** %10, align 8
  %54 = getelementptr inbounds %struct.node_t, %struct.node_t* %53, i32 0, i32 2
  %55 = load %struct.node_t*, %struct.node_t** %54, align 8
  store %struct.node_t* %55, %struct.node_t** %10, align 8
  %56 = load %struct.node_t*, %struct.node_t** %10, align 8
  %57 = getelementptr inbounds %struct.node_t, %struct.node_t* %56, i32 0, i32 0
  %58 = load i32, i32* %57, align 8
  %59 = icmp ne i32 %58, 1
  br i1 %59, label %68, label %60

; <label>:60:                                     ; preds = %52
  %61 = load %struct.node_t*, %struct.node_t** %10, align 8
  %62 = getelementptr inbounds %struct.node_t, %struct.node_t* %61, i32 0, i32 1
  %63 = bitcast %union.anon.11* %62 to %struct.list_t**
  %64 = load %struct.list_t*, %struct.list_t** %63, align 8
  %65 = getelementptr inbounds %struct.list_t, %struct.list_t* %64, i32 0, i32 0
  %66 = load i32, i32* %65, align 8
  %67 = icmp ne i32 %66, 2
  br i1 %67, label %68, label %71

; <label>:68:                                     ; preds = %60, %52
  %69 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %70 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %69, i8* getelementptr inbounds ([43 x i8], [43 x i8]* @.str.15, i32 0, i32 0))
  store i32 1, i32* %5, align 4
  br label %182

; <label>:71:                                     ; preds = %60
  %72 = load %struct.node_t*, %struct.node_t** %10, align 8
  %73 = getelementptr inbounds %struct.node_t, %struct.node_t* %72, i32 0, i32 1
  %74 = bitcast %union.anon.11* %73 to %struct.list_t**
  %75 = load %struct.list_t*, %struct.list_t** %74, align 8
  %76 = getelementptr inbounds %struct.list_t, %struct.list_t* %75, i32 0, i32 1
  %77 = load %struct.node_t*, %struct.node_t** %76, align 8
  store %struct.node_t* %77, %struct.node_t** %11, align 8
  %78 = load %struct.node_t*, %struct.node_t** %11, align 8
  %79 = getelementptr inbounds %struct.node_t, %struct.node_t* %78, i32 0, i32 0
  %80 = load i32, i32* %79, align 8
  switch i32 %80, label %163 [
    i32 0, label %81
    i32 1, label %101
  ]

; <label>:81:                                     ; preds = %71
  %82 = load %struct.val_list_t*, %struct.val_list_t** %13, align 8
  %83 = getelementptr inbounds %struct.val_list_t, %struct.val_list_t* %82, i32 0, i32 0
  store i32 1, i32* %83, align 8
  %84 = call i8* @malloc(i64 32) #8
  %85 = bitcast i8* %84 to %struct.val_t*
  %86 = load %struct.val_list_t*, %struct.val_list_t** %13, align 8
  %87 = getelementptr inbounds %struct.val_list_t, %struct.val_list_t* %86, i32 0, i32 1
  store %struct.val_t* %85, %struct.val_t** %87, align 8
  %88 = load %struct.module_t*, %struct.module_t** %6, align 8
  %89 = load %struct.symbol_table_t*, %struct.symbol_table_t** %7, align 8
  %90 = load %struct.node_t*, %struct.node_t** %11, align 8
  %91 = getelementptr inbounds %struct.node_t, %struct.node_t* %90, i32 0, i32 1
  %92 = bitcast %union.anon.11* %91 to %struct.atom_t**
  %93 = load %struct.atom_t*, %struct.atom_t** %92, align 8
  %94 = load %struct.val_list_t*, %struct.val_list_t** %13, align 8
  %95 = getelementptr inbounds %struct.val_list_t, %struct.val_list_t* %94, i32 0, i32 1
  %96 = load %struct.val_t*, %struct.val_t** %95, align 8
  %97 = call i32 @parse_atom(%struct.module_t* %88, %struct.symbol_table_t* %89, %struct.atom_t* %93, %struct.val_t* %96)
  %98 = icmp ne i32 %97, 0
  br i1 %98, label %99, label %100

; <label>:99:                                     ; preds = %81
  store i32 1, i32* %5, align 4
  br label %182

; <label>:100:                                    ; preds = %81
  br label %163

; <label>:101:                                    ; preds = %71
  %102 = load %struct.node_t*, %struct.node_t** %11, align 8
  %103 = getelementptr inbounds %struct.node_t, %struct.node_t* %102, i32 0, i32 1
  %104 = bitcast %union.anon.11* %103 to %struct.list_t**
  %105 = load %struct.list_t*, %struct.list_t** %104, align 8
  %106 = getelementptr inbounds %struct.list_t, %struct.list_t* %105, i32 0, i32 0
  %107 = load i32, i32* %106, align 8
  %108 = load %struct.val_list_t*, %struct.val_list_t** %13, align 8
  %109 = getelementptr inbounds %struct.val_list_t, %struct.val_list_t* %108, i32 0, i32 0
  store i32 %107, i32* %109, align 8
  %110 = load %struct.val_list_t*, %struct.val_list_t** %13, align 8
  %111 = getelementptr inbounds %struct.val_list_t, %struct.val_list_t* %110, i32 0, i32 0
  %112 = load i32, i32* %111, align 8
  %113 = sext i32 %112 to i64
  %114 = mul i64 32, %113
  %115 = call i8* @malloc(i64 %114) #8
  %116 = bitcast i8* %115 to %struct.val_t*
  %117 = load %struct.val_list_t*, %struct.val_list_t** %13, align 8
  %118 = getelementptr inbounds %struct.val_list_t, %struct.val_list_t* %117, i32 0, i32 1
  store %struct.val_t* %116, %struct.val_t** %118, align 8
  %119 = load %struct.node_t*, %struct.node_t** %11, align 8
  %120 = getelementptr inbounds %struct.node_t, %struct.node_t* %119, i32 0, i32 1
  %121 = bitcast %union.anon.11* %120 to %struct.list_t**
  %122 = load %struct.list_t*, %struct.list_t** %121, align 8
  %123 = getelementptr inbounds %struct.list_t, %struct.list_t* %122, i32 0, i32 1
  %124 = load %struct.node_t*, %struct.node_t** %123, align 8
  store %struct.node_t* %124, %struct.node_t** %15, align 8
  store i32 0, i32* %16, align 4
  br label %125

; <label>:125:                                    ; preds = %159, %101
  %126 = load i32, i32* %16, align 4
  %127 = load %struct.val_list_t*, %struct.val_list_t** %13, align 8
  %128 = getelementptr inbounds %struct.val_list_t, %struct.val_list_t* %127, i32 0, i32 0
  %129 = load i32, i32* %128, align 8
  %130 = icmp slt i32 %126, %129
  br i1 %130, label %131, label %162

; <label>:131:                                    ; preds = %125
  %132 = load %struct.node_t*, %struct.node_t** %15, align 8
  %133 = getelementptr inbounds %struct.node_t, %struct.node_t* %132, i32 0, i32 0
  %134 = load i32, i32* %133, align 8
  %135 = icmp ne i32 %134, 0
  br i1 %135, label %136, label %139

; <label>:136:                                    ; preds = %131
  %137 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %138 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %137, i8* getelementptr inbounds ([27 x i8], [27 x i8]* @.str.16, i32 0, i32 0))
  store i32 1, i32* %5, align 4
  br label %182

; <label>:139:                                    ; preds = %131
  %140 = load %struct.module_t*, %struct.module_t** %6, align 8
  %141 = load %struct.symbol_table_t*, %struct.symbol_table_t** %7, align 8
  %142 = load %struct.node_t*, %struct.node_t** %15, align 8
  %143 = getelementptr inbounds %struct.node_t, %struct.node_t* %142, i32 0, i32 1
  %144 = bitcast %union.anon.11* %143 to %struct.atom_t**
  %145 = load %struct.atom_t*, %struct.atom_t** %144, align 8
  %146 = load %struct.val_list_t*, %struct.val_list_t** %13, align 8
  %147 = getelementptr inbounds %struct.val_list_t, %struct.val_list_t* %146, i32 0, i32 1
  %148 = load %struct.val_t*, %struct.val_t** %147, align 8
  %149 = load i32, i32* %16, align 4
  %150 = sext i32 %149 to i64
  %151 = getelementptr inbounds %struct.val_t, %struct.val_t* %148, i64 %150
  %152 = call i32 @parse_atom(%struct.module_t* %140, %struct.symbol_table_t* %141, %struct.atom_t* %145, %struct.val_t* %151)
  %153 = icmp ne i32 %152, 0
  br i1 %153, label %154, label %155

; <label>:154:                                    ; preds = %139
  store i32 1, i32* %5, align 4
  br label %182

; <label>:155:                                    ; preds = %139
  %156 = load %struct.node_t*, %struct.node_t** %15, align 8
  %157 = getelementptr inbounds %struct.node_t, %struct.node_t* %156, i32 0, i32 2
  %158 = load %struct.node_t*, %struct.node_t** %157, align 8
  store %struct.node_t* %158, %struct.node_t** %15, align 8
  br label %159

; <label>:159:                                    ; preds = %155
  %160 = load i32, i32* %16, align 4
  %161 = add nsw i32 %160, 1
  store i32 %161, i32* %16, align 4
  br label %125

; <label>:162:                                    ; preds = %125
  br label %163

; <label>:163:                                    ; preds = %71, %162, %100
  %164 = load %struct.module_t*, %struct.module_t** %6, align 8
  %165 = load %struct.symbol_table_t*, %struct.symbol_table_t** %7, align 8
  %166 = load %struct.node_t*, %struct.node_t** %11, align 8
  %167 = getelementptr inbounds %struct.node_t, %struct.node_t* %166, i32 0, i32 2
  %168 = load %struct.node_t*, %struct.node_t** %167, align 8
  %169 = load %struct.expr_t*, %struct.expr_t** %12, align 8
  %170 = call i32 @parse_expr(%struct.module_t* %164, %struct.symbol_table_t* %165, %struct.node_t* %168, %struct.expr_t* %169)
  %171 = icmp ne i32 %170, 0
  br i1 %171, label %172, label %173

; <label>:172:                                    ; preds = %163
  store i32 1, i32* %5, align 4
  br label %182

; <label>:173:                                    ; preds = %163
  %174 = load %struct.expr_t*, %struct.expr_t** %12, align 8
  %175 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %174, i32 1
  store %struct.expr_t* %175, %struct.expr_t** %12, align 8
  %176 = load %struct.val_list_t*, %struct.val_list_t** %13, align 8
  %177 = getelementptr inbounds %struct.val_list_t, %struct.val_list_t* %176, i32 1
  store %struct.val_list_t* %177, %struct.val_list_t** %13, align 8
  br label %178

; <label>:178:                                    ; preds = %173
  %179 = load i32, i32* %14, align 4
  %180 = add nsw i32 %179, 1
  store i32 %180, i32* %14, align 4
  br label %44

; <label>:181:                                    ; preds = %44
  store i32 0, i32* %5, align 4
  br label %182

; <label>:182:                                    ; preds = %181, %172, %154, %136, %99, %68, %32
  %183 = load i32, i32* %5, align 4
  ret i32 %183
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @parse_match(%struct.module_t*, %struct.symbol_table_t*, %struct.node_t*, %struct.expr_t*) #0 {
  %5 = alloca i32, align 4
  %6 = alloca %struct.module_t*, align 8
  %7 = alloca %struct.symbol_table_t*, align 8
  %8 = alloca %struct.node_t*, align 8
  %9 = alloca %struct.expr_t*, align 8
  %10 = alloca %struct.node_t*, align 8
  %11 = alloca %struct.expr_t*, align 8
  %12 = alloca %struct.expr_t*, align 8
  %13 = alloca i32, align 4
  store %struct.module_t* %0, %struct.module_t** %6, align 8
  store %struct.symbol_table_t* %1, %struct.symbol_table_t** %7, align 8
  store %struct.node_t* %2, %struct.node_t** %8, align 8
  store %struct.expr_t* %3, %struct.expr_t** %9, align 8
  %14 = load %struct.module_t*, %struct.module_t** %6, align 8
  %15 = load %struct.symbol_table_t*, %struct.symbol_table_t** %7, align 8
  %16 = load %struct.node_t*, %struct.node_t** %8, align 8
  %17 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %18 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %17, i32 0, i32 2
  %19 = bitcast %union.anon.1* %18 to %struct.anon.6*
  %20 = getelementptr inbounds %struct.anon.6, %struct.anon.6* %19, i32 0, i32 1
  %21 = load %struct.expr_t*, %struct.expr_t** %20, align 8
  %22 = call i32 @parse_expr(%struct.module_t* %14, %struct.symbol_table_t* %15, %struct.node_t* %16, %struct.expr_t* %21)
  %23 = icmp ne i32 %22, 0
  br i1 %23, label %24, label %25

; <label>:24:                                     ; preds = %4
  store i32 1, i32* %5, align 4
  br label %179

; <label>:25:                                     ; preds = %4
  %26 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %27 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %26, i32 0, i32 2
  %28 = bitcast %union.anon.1* %27 to %struct.anon.6*
  %29 = getelementptr inbounds %struct.anon.6, %struct.anon.6* %28, i32 0, i32 1
  %30 = load %struct.expr_t*, %struct.expr_t** %29, align 8
  %31 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %30, i32 0, i32 1
  %32 = load %struct.type_t*, %struct.type_t** %31, align 8
  %33 = getelementptr inbounds %struct.type_t, %struct.type_t* %32, i32 0, i32 1
  %34 = load i32, i32* %33, align 8
  %35 = icmp ne i32 %34, 5
  br i1 %35, label %36, label %39

; <label>:36:                                     ; preds = %25
  %37 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %38 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %37, i8* getelementptr inbounds ([43 x i8], [43 x i8]* @.str.17, i32 0, i32 0))
  store i32 1, i32* %5, align 4
  br label %179

; <label>:39:                                     ; preds = %25
  %40 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %41 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %40, i32 0, i32 2
  %42 = bitcast %union.anon.1* %41 to %struct.anon.6*
  %43 = getelementptr inbounds %struct.anon.6, %struct.anon.6* %42, i32 0, i32 2
  %44 = load %struct.expr_t*, %struct.expr_t** %43, align 8
  store %struct.expr_t* %44, %struct.expr_t** %11, align 8
  %45 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %46 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %45, i32 0, i32 2
  %47 = bitcast %union.anon.1* %46 to %struct.anon.6*
  %48 = getelementptr inbounds %struct.anon.6, %struct.anon.6* %47, i32 0, i32 3
  %49 = load %struct.expr_t*, %struct.expr_t** %48, align 8
  store %struct.expr_t* %49, %struct.expr_t** %12, align 8
  store i32 0, i32* %13, align 4
  br label %50

; <label>:50:                                     ; preds = %175, %39
  %51 = load i32, i32* %13, align 4
  %52 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %53 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %52, i32 0, i32 2
  %54 = bitcast %union.anon.1* %53 to %struct.anon.6*
  %55 = getelementptr inbounds %struct.anon.6, %struct.anon.6* %54, i32 0, i32 0
  %56 = load i32, i32* %55, align 8
  %57 = icmp slt i32 %51, %56
  br i1 %57, label %58, label %178

; <label>:58:                                     ; preds = %50
  %59 = load %struct.node_t*, %struct.node_t** %8, align 8
  %60 = getelementptr inbounds %struct.node_t, %struct.node_t* %59, i32 0, i32 2
  %61 = load %struct.node_t*, %struct.node_t** %60, align 8
  store %struct.node_t* %61, %struct.node_t** %8, align 8
  %62 = load %struct.node_t*, %struct.node_t** %8, align 8
  %63 = getelementptr inbounds %struct.node_t, %struct.node_t* %62, i32 0, i32 0
  %64 = load i32, i32* %63, align 8
  %65 = icmp ne i32 %64, 1
  br i1 %65, label %74, label %66

; <label>:66:                                     ; preds = %58
  %67 = load %struct.node_t*, %struct.node_t** %8, align 8
  %68 = getelementptr inbounds %struct.node_t, %struct.node_t* %67, i32 0, i32 1
  %69 = bitcast %union.anon.11* %68 to %struct.list_t**
  %70 = load %struct.list_t*, %struct.list_t** %69, align 8
  %71 = getelementptr inbounds %struct.list_t, %struct.list_t* %70, i32 0, i32 0
  %72 = load i32, i32* %71, align 8
  %73 = icmp ne i32 %72, 2
  br i1 %73, label %74, label %77

; <label>:74:                                     ; preds = %66, %58
  %75 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %76 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %75, i8* getelementptr inbounds ([50 x i8], [50 x i8]* @.str.18, i32 0, i32 0))
  store i32 1, i32* %5, align 4
  br label %179

; <label>:77:                                     ; preds = %66
  %78 = load %struct.node_t*, %struct.node_t** %8, align 8
  %79 = getelementptr inbounds %struct.node_t, %struct.node_t* %78, i32 0, i32 1
  %80 = bitcast %union.anon.11* %79 to %struct.list_t**
  %81 = load %struct.list_t*, %struct.list_t** %80, align 8
  %82 = getelementptr inbounds %struct.list_t, %struct.list_t* %81, i32 0, i32 1
  %83 = load %struct.node_t*, %struct.node_t** %82, align 8
  store %struct.node_t* %83, %struct.node_t** %10, align 8
  %84 = load %struct.module_t*, %struct.module_t** %6, align 8
  %85 = load %struct.symbol_table_t*, %struct.symbol_table_t** %7, align 8
  %86 = load %struct.node_t*, %struct.node_t** %10, align 8
  %87 = load %struct.expr_t*, %struct.expr_t** %11, align 8
  %88 = call i32 @parse_expr(%struct.module_t* %84, %struct.symbol_table_t* %85, %struct.node_t* %86, %struct.expr_t* %87)
  %89 = icmp ne i32 %88, 0
  br i1 %89, label %90, label %91

; <label>:90:                                     ; preds = %77
  store i32 1, i32* %5, align 4
  br label %179

; <label>:91:                                     ; preds = %77
  %92 = load %struct.expr_t*, %struct.expr_t** %11, align 8
  %93 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %92, i32 0, i32 0
  %94 = load i32, i32* %93, align 8
  %95 = icmp ne i32 %94, 7
  br i1 %95, label %96, label %107

; <label>:96:                                     ; preds = %91
  %97 = load %struct.expr_t*, %struct.expr_t** %11, align 8
  %98 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %97, i32 0, i32 0
  %99 = load i32, i32* %98, align 8
  %100 = icmp ne i32 %99, 1
  br i1 %100, label %101, label %107

; <label>:101:                                    ; preds = %96
  %102 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %103 = load %struct.expr_t*, %struct.expr_t** %11, align 8
  %104 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %103, i32 0, i32 0
  %105 = load i32, i32* %104, align 8
  %106 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %102, i8* getelementptr inbounds ([55 x i8], [55 x i8]* @.str.19, i32 0, i32 0), i32 %105)
  store i32 1, i32* %5, align 4
  br label %179

; <label>:107:                                    ; preds = %96, %91
  %108 = load %struct.expr_t*, %struct.expr_t** %11, align 8
  %109 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %108, i32 0, i32 1
  %110 = load %struct.type_t*, %struct.type_t** %109, align 8
  %111 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %112 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %111, i32 0, i32 2
  %113 = bitcast %union.anon.1* %112 to %struct.anon.6*
  %114 = getelementptr inbounds %struct.anon.6, %struct.anon.6* %113, i32 0, i32 1
  %115 = load %struct.expr_t*, %struct.expr_t** %114, align 8
  %116 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %115, i32 0, i32 1
  %117 = load %struct.type_t*, %struct.type_t** %116, align 8
  %118 = icmp ne %struct.type_t* %110, %117
  br i1 %118, label %119, label %122

; <label>:119:                                    ; preds = %107
  %120 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %121 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %120, i8* getelementptr inbounds ([52 x i8], [52 x i8]* @.str.20, i32 0, i32 0))
  store i32 1, i32* %5, align 4
  br label %179

; <label>:122:                                    ; preds = %107
  %123 = load %struct.node_t*, %struct.node_t** %10, align 8
  %124 = getelementptr inbounds %struct.node_t, %struct.node_t* %123, i32 0, i32 2
  %125 = load %struct.node_t*, %struct.node_t** %124, align 8
  store %struct.node_t* %125, %struct.node_t** %10, align 8
  %126 = load %struct.expr_t*, %struct.expr_t** %11, align 8
  %127 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %126, i32 0, i32 0
  %128 = load i32, i32* %127, align 8
  %129 = icmp eq i32 %128, 1
  br i1 %129, label %130, label %161

; <label>:130:                                    ; preds = %122
  %131 = load %struct.expr_t*, %struct.expr_t** %12, align 8
  %132 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %131, i32 0, i32 0
  store i32 3, i32* %132, align 8
  %133 = call %struct.expr_t* @expr_new()
  %134 = load %struct.expr_t*, %struct.expr_t** %12, align 8
  %135 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %134, i32 0, i32 2
  %136 = bitcast %union.anon.1* %135 to %struct.anon.5*
  %137 = getelementptr inbounds %struct.anon.5, %struct.anon.5* %136, i32 0, i32 1
  store %struct.expr_t* %133, %struct.expr_t** %137, align 8
  %138 = load %struct.symbol_table_t*, %struct.symbol_table_t** %7, align 8
  %139 = load %struct.expr_t*, %struct.expr_t** %11, align 8
  %140 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %139, i32 0, i32 2
  %141 = bitcast %union.anon.1* %140 to %struct.anon.3*
  %142 = getelementptr inbounds %struct.anon.3, %struct.anon.3* %141, i32 0, i32 2
  %143 = load i32, i32* %142, align 8
  %144 = call %struct.symbol_table_t* @symbol_table_new(%struct.symbol_table_t* %138, i32 %143)
  %145 = load %struct.expr_t*, %struct.expr_t** %12, align 8
  %146 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %145, i32 0, i32 2
  %147 = bitcast %union.anon.1* %146 to %struct.anon.5*
  %148 = getelementptr inbounds %struct.anon.5, %struct.anon.5* %147, i32 0, i32 0
  store %struct.symbol_table_t* %144, %struct.symbol_table_t** %148, align 8
  %149 = load %struct.module_t*, %struct.module_t** %6, align 8
  %150 = load %struct.symbol_table_t*, %struct.symbol_table_t** %7, align 8
  %151 = load %struct.node_t*, %struct.node_t** %10, align 8
  %152 = load %struct.expr_t*, %struct.expr_t** %12, align 8
  %153 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %152, i32 0, i32 2
  %154 = bitcast %union.anon.1* %153 to %struct.anon.5*
  %155 = getelementptr inbounds %struct.anon.5, %struct.anon.5* %154, i32 0, i32 1
  %156 = load %struct.expr_t*, %struct.expr_t** %155, align 8
  %157 = call i32 @parse_expr(%struct.module_t* %149, %struct.symbol_table_t* %150, %struct.node_t* %151, %struct.expr_t* %156)
  %158 = icmp ne i32 %157, 0
  br i1 %158, label %159, label %160

; <label>:159:                                    ; preds = %130
  store i32 1, i32* %5, align 4
  br label %179

; <label>:160:                                    ; preds = %130
  br label %170

; <label>:161:                                    ; preds = %122
  %162 = load %struct.module_t*, %struct.module_t** %6, align 8
  %163 = load %struct.symbol_table_t*, %struct.symbol_table_t** %7, align 8
  %164 = load %struct.node_t*, %struct.node_t** %10, align 8
  %165 = load %struct.expr_t*, %struct.expr_t** %12, align 8
  %166 = call i32 @parse_expr(%struct.module_t* %162, %struct.symbol_table_t* %163, %struct.node_t* %164, %struct.expr_t* %165)
  %167 = icmp ne i32 %166, 0
  br i1 %167, label %168, label %169

; <label>:168:                                    ; preds = %161
  store i32 1, i32* %5, align 4
  br label %179

; <label>:169:                                    ; preds = %161
  br label %170

; <label>:170:                                    ; preds = %169, %160
  %171 = load %struct.expr_t*, %struct.expr_t** %11, align 8
  %172 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %171, i32 1
  store %struct.expr_t* %172, %struct.expr_t** %11, align 8
  %173 = load %struct.expr_t*, %struct.expr_t** %12, align 8
  %174 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %173, i32 1
  store %struct.expr_t* %174, %struct.expr_t** %12, align 8
  br label %175

; <label>:175:                                    ; preds = %170
  %176 = load i32, i32* %13, align 4
  %177 = add nsw i32 %176, 1
  store i32 %177, i32* %13, align 4
  br label %50

; <label>:178:                                    ; preds = %50
  store i32 0, i32* %5, align 4
  br label %179

; <label>:179:                                    ; preds = %178, %168, %159, %119, %101, %90, %74, %36, %24
  %180 = load i32, i32* %5, align 4
  ret i32 %180
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @parse_funcall(%struct.module_t*, %struct.symbol_table_t*, %struct.list_t*, %struct.expr_t*) #0 {
  %5 = alloca i32, align 4
  %6 = alloca %struct.module_t*, align 8
  %7 = alloca %struct.symbol_table_t*, align 8
  %8 = alloca %struct.list_t*, align 8
  %9 = alloca %struct.expr_t*, align 8
  %10 = alloca %struct.expr_t*, align 8
  %11 = alloca %struct.node_t*, align 8
  store %struct.module_t* %0, %struct.module_t** %6, align 8
  store %struct.symbol_table_t* %1, %struct.symbol_table_t** %7, align 8
  store %struct.list_t* %2, %struct.list_t** %8, align 8
  store %struct.expr_t* %3, %struct.expr_t** %9, align 8
  %12 = load %struct.list_t*, %struct.list_t** %8, align 8
  %13 = getelementptr inbounds %struct.list_t, %struct.list_t* %12, i32 0, i32 1
  %14 = load %struct.node_t*, %struct.node_t** %13, align 8
  %15 = getelementptr inbounds %struct.node_t, %struct.node_t* %14, i32 0, i32 1
  %16 = bitcast %union.anon.11* %15 to %struct.atom_t**
  %17 = load %struct.atom_t*, %struct.atom_t** %16, align 8
  %18 = getelementptr inbounds %struct.atom_t, %struct.atom_t* %17, i32 0, i32 0
  %19 = load i8*, i8** %18, align 8
  %20 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %21 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %20, i32 0, i32 2
  %22 = bitcast %union.anon.1* %21 to %struct.anon.3*
  %23 = getelementptr inbounds %struct.anon.3, %struct.anon.3* %22, i32 0, i32 0
  store i8* %19, i8** %23, align 8
  %24 = load %struct.symbol_table_t*, %struct.symbol_table_t** %7, align 8
  %25 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %26 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %25, i32 0, i32 2
  %27 = bitcast %union.anon.1* %26 to %struct.anon.3*
  %28 = getelementptr inbounds %struct.anon.3, %struct.anon.3* %27, i32 0, i32 0
  %29 = load i8*, i8** %28, align 8
  %30 = call %struct.val_t* @symbol_table_get(%struct.symbol_table_t* %24, i8* %29)
  %31 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %32 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %31, i32 0, i32 2
  %33 = bitcast %union.anon.1* %32 to %struct.anon.3*
  %34 = getelementptr inbounds %struct.anon.3, %struct.anon.3* %33, i32 0, i32 1
  store %struct.val_t* %30, %struct.val_t** %34, align 8
  %35 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %36 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %35, i32 0, i32 2
  %37 = bitcast %union.anon.1* %36 to %struct.anon.3*
  %38 = getelementptr inbounds %struct.anon.3, %struct.anon.3* %37, i32 0, i32 1
  %39 = load %struct.val_t*, %struct.val_t** %38, align 8
  %40 = icmp eq %struct.val_t* %39, null
  br i1 %40, label %41, label %53

; <label>:41:                                     ; preds = %4
  %42 = load %struct.module_t*, %struct.module_t** %6, align 8
  %43 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %44 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %43, i32 0, i32 2
  %45 = bitcast %union.anon.1* %44 to %struct.anon.3*
  %46 = getelementptr inbounds %struct.anon.3, %struct.anon.3* %45, i32 0, i32 0
  %47 = load i8*, i8** %46, align 8
  %48 = call %struct.val_t* @module_deps_symbol_find(%struct.module_t* %42, i8* %47)
  %49 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %50 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %49, i32 0, i32 2
  %51 = bitcast %union.anon.1* %50 to %struct.anon.3*
  %52 = getelementptr inbounds %struct.anon.3, %struct.anon.3* %51, i32 0, i32 1
  store %struct.val_t* %48, %struct.val_t** %52, align 8
  br label %53

; <label>:53:                                     ; preds = %41, %4
  %54 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %55 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %54, i32 0, i32 2
  %56 = bitcast %union.anon.1* %55 to %struct.anon.3*
  %57 = getelementptr inbounds %struct.anon.3, %struct.anon.3* %56, i32 0, i32 1
  %58 = load %struct.val_t*, %struct.val_t** %57, align 8
  %59 = icmp eq %struct.val_t* %58, null
  br i1 %59, label %60, label %84

; <label>:60:                                     ; preds = %53
  %61 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %62 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %63 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %62, i32 0, i32 2
  %64 = bitcast %union.anon.1* %63 to %struct.anon.3*
  %65 = getelementptr inbounds %struct.anon.3, %struct.anon.3* %64, i32 0, i32 0
  %66 = load i8*, i8** %65, align 8
  %67 = load %struct.list_t*, %struct.list_t** %8, align 8
  %68 = getelementptr inbounds %struct.list_t, %struct.list_t* %67, i32 0, i32 1
  %69 = load %struct.node_t*, %struct.node_t** %68, align 8
  %70 = getelementptr inbounds %struct.node_t, %struct.node_t* %69, i32 0, i32 1
  %71 = bitcast %union.anon.11* %70 to %struct.atom_t**
  %72 = load %struct.atom_t*, %struct.atom_t** %71, align 8
  %73 = getelementptr inbounds %struct.atom_t, %struct.atom_t* %72, i32 0, i32 2
  %74 = load i32, i32* %73, align 4
  %75 = load %struct.list_t*, %struct.list_t** %8, align 8
  %76 = getelementptr inbounds %struct.list_t, %struct.list_t* %75, i32 0, i32 1
  %77 = load %struct.node_t*, %struct.node_t** %76, align 8
  %78 = getelementptr inbounds %struct.node_t, %struct.node_t* %77, i32 0, i32 1
  %79 = bitcast %union.anon.11* %78 to %struct.atom_t**
  %80 = load %struct.atom_t*, %struct.atom_t** %79, align 8
  %81 = getelementptr inbounds %struct.atom_t, %struct.atom_t* %80, i32 0, i32 3
  %82 = load i32, i32* %81, align 8
  %83 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %61, i8* getelementptr inbounds ([29 x i8], [29 x i8]* @.str.21, i32 0, i32 0), i8* %66, i32 %74, i32 %82)
  store i32 1, i32* %5, align 4
  br label %212

; <label>:84:                                     ; preds = %53
  %85 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %86 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %85, i32 0, i32 2
  %87 = bitcast %union.anon.1* %86 to %struct.anon.3*
  %88 = getelementptr inbounds %struct.anon.3, %struct.anon.3* %87, i32 0, i32 1
  %89 = load %struct.val_t*, %struct.val_t** %88, align 8
  %90 = getelementptr inbounds %struct.val_t, %struct.val_t* %89, i32 0, i32 0
  %91 = load %struct.type_t*, %struct.type_t** %90, align 8
  %92 = getelementptr inbounds %struct.type_t, %struct.type_t* %91, i32 0, i32 1
  %93 = load i32, i32* %92, align 8
  %94 = icmp ne i32 %93, 2
  br i1 %94, label %95, label %103

; <label>:95:                                     ; preds = %84
  %96 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %97 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %98 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %97, i32 0, i32 2
  %99 = bitcast %union.anon.1* %98 to %struct.anon.3*
  %100 = getelementptr inbounds %struct.anon.3, %struct.anon.3* %99, i32 0, i32 0
  %101 = load i8*, i8** %100, align 8
  %102 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %96, i8* getelementptr inbounds ([22 x i8], [22 x i8]* @.str.22, i32 0, i32 0), i8* %101)
  store i32 1, i32* %5, align 4
  br label %212

; <label>:103:                                    ; preds = %84
  %104 = load %struct.list_t*, %struct.list_t** %8, align 8
  %105 = getelementptr inbounds %struct.list_t, %struct.list_t* %104, i32 0, i32 0
  %106 = load i32, i32* %105, align 8
  %107 = sub nsw i32 %106, 1
  %108 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %109 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %108, i32 0, i32 2
  %110 = bitcast %union.anon.1* %109 to %struct.anon.3*
  %111 = getelementptr inbounds %struct.anon.3, %struct.anon.3* %110, i32 0, i32 2
  store i32 %107, i32* %111, align 8
  %112 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %113 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %112, i32 0, i32 2
  %114 = bitcast %union.anon.1* %113 to %struct.anon.3*
  %115 = getelementptr inbounds %struct.anon.3, %struct.anon.3* %114, i32 0, i32 2
  %116 = load i32, i32* %115, align 8
  %117 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %118 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %117, i32 0, i32 2
  %119 = bitcast %union.anon.1* %118 to %struct.anon.3*
  %120 = getelementptr inbounds %struct.anon.3, %struct.anon.3* %119, i32 0, i32 1
  %121 = load %struct.val_t*, %struct.val_t** %120, align 8
  %122 = getelementptr inbounds %struct.val_t, %struct.val_t* %121, i32 0, i32 0
  %123 = load %struct.type_t*, %struct.type_t** %122, align 8
  %124 = getelementptr inbounds %struct.type_t, %struct.type_t* %123, i32 0, i32 3
  %125 = load i32, i32* %124, align 8
  %126 = sub nsw i32 %125, 1
  %127 = icmp ne i32 %116, %126
  br i1 %127, label %128, label %136

; <label>:128:                                    ; preds = %103
  %129 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %130 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %131 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %130, i32 0, i32 2
  %132 = bitcast %union.anon.1* %131 to %struct.anon.3*
  %133 = getelementptr inbounds %struct.anon.3, %struct.anon.3* %132, i32 0, i32 0
  %134 = load i8*, i8** %133, align 8
  %135 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %129, i8* getelementptr inbounds ([32 x i8], [32 x i8]* @.str.23, i32 0, i32 0), i8* %134)
  store i32 1, i32* %5, align 4
  br label %212

; <label>:136:                                    ; preds = %103
  %137 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %138 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %137, i32 0, i32 2
  %139 = bitcast %union.anon.1* %138 to %struct.anon.3*
  %140 = getelementptr inbounds %struct.anon.3, %struct.anon.3* %139, i32 0, i32 2
  %141 = load i32, i32* %140, align 8
  %142 = call %struct.expr_t* @expr_new_i(i32 %141)
  %143 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %144 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %143, i32 0, i32 2
  %145 = bitcast %union.anon.1* %144 to %struct.anon.3*
  %146 = getelementptr inbounds %struct.anon.3, %struct.anon.3* %145, i32 0, i32 3
  store %struct.expr_t* %142, %struct.expr_t** %146, align 8
  %147 = load %struct.list_t*, %struct.list_t** %8, align 8
  %148 = getelementptr inbounds %struct.list_t, %struct.list_t* %147, i32 0, i32 1
  %149 = load %struct.node_t*, %struct.node_t** %148, align 8
  %150 = getelementptr inbounds %struct.node_t, %struct.node_t* %149, i32 0, i32 2
  %151 = load %struct.node_t*, %struct.node_t** %150, align 8
  store %struct.node_t* %151, %struct.node_t** %11, align 8
  %152 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %153 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %152, i32 0, i32 2
  %154 = bitcast %union.anon.1* %153 to %struct.anon.3*
  %155 = getelementptr inbounds %struct.anon.3, %struct.anon.3* %154, i32 0, i32 3
  %156 = load %struct.expr_t*, %struct.expr_t** %155, align 8
  store %struct.expr_t* %156, %struct.expr_t** %10, align 8
  br label %157

; <label>:157:                                    ; preds = %184, %136
  %158 = load %struct.expr_t*, %struct.expr_t** %10, align 8
  %159 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %160 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %159, i32 0, i32 2
  %161 = bitcast %union.anon.1* %160 to %struct.anon.3*
  %162 = getelementptr inbounds %struct.anon.3, %struct.anon.3* %161, i32 0, i32 3
  %163 = load %struct.expr_t*, %struct.expr_t** %162, align 8
  %164 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %165 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %164, i32 0, i32 2
  %166 = bitcast %union.anon.1* %165 to %struct.anon.3*
  %167 = getelementptr inbounds %struct.anon.3, %struct.anon.3* %166, i32 0, i32 2
  %168 = load i32, i32* %167, align 8
  %169 = sext i32 %168 to i64
  %170 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %163, i64 %169
  %171 = icmp ult %struct.expr_t* %158, %170
  br i1 %171, label %172, label %187

; <label>:172:                                    ; preds = %157
  %173 = load %struct.module_t*, %struct.module_t** %6, align 8
  %174 = load %struct.symbol_table_t*, %struct.symbol_table_t** %7, align 8
  %175 = load %struct.node_t*, %struct.node_t** %11, align 8
  %176 = load %struct.expr_t*, %struct.expr_t** %10, align 8
  %177 = call i32 @parse_expr(%struct.module_t* %173, %struct.symbol_table_t* %174, %struct.node_t* %175, %struct.expr_t* %176)
  %178 = icmp ne i32 %177, 0
  br i1 %178, label %179, label %180

; <label>:179:                                    ; preds = %172
  store i32 1, i32* %5, align 4
  br label %212

; <label>:180:                                    ; preds = %172
  %181 = load %struct.node_t*, %struct.node_t** %11, align 8
  %182 = getelementptr inbounds %struct.node_t, %struct.node_t* %181, i32 0, i32 2
  %183 = load %struct.node_t*, %struct.node_t** %182, align 8
  store %struct.node_t* %183, %struct.node_t** %11, align 8
  br label %184

; <label>:184:                                    ; preds = %180
  %185 = load %struct.expr_t*, %struct.expr_t** %10, align 8
  %186 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %185, i32 1
  store %struct.expr_t* %186, %struct.expr_t** %10, align 8
  br label %157

; <label>:187:                                    ; preds = %157
  %188 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %189 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %188, i32 0, i32 2
  %190 = bitcast %union.anon.1* %189 to %struct.anon.3*
  %191 = getelementptr inbounds %struct.anon.3, %struct.anon.3* %190, i32 0, i32 1
  %192 = load %struct.val_t*, %struct.val_t** %191, align 8
  %193 = getelementptr inbounds %struct.val_t, %struct.val_t* %192, i32 0, i32 0
  %194 = load %struct.type_t*, %struct.type_t** %193, align 8
  %195 = getelementptr inbounds %struct.type_t, %struct.type_t* %194, i32 0, i32 4
  %196 = load %struct.type_t**, %struct.type_t*** %195, align 8
  %197 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %198 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %197, i32 0, i32 2
  %199 = bitcast %union.anon.1* %198 to %struct.anon.3*
  %200 = getelementptr inbounds %struct.anon.3, %struct.anon.3* %199, i32 0, i32 1
  %201 = load %struct.val_t*, %struct.val_t** %200, align 8
  %202 = getelementptr inbounds %struct.val_t, %struct.val_t* %201, i32 0, i32 0
  %203 = load %struct.type_t*, %struct.type_t** %202, align 8
  %204 = getelementptr inbounds %struct.type_t, %struct.type_t* %203, i32 0, i32 3
  %205 = load i32, i32* %204, align 8
  %206 = sub nsw i32 %205, 1
  %207 = sext i32 %206 to i64
  %208 = getelementptr inbounds %struct.type_t*, %struct.type_t** %196, i64 %207
  %209 = load %struct.type_t*, %struct.type_t** %208, align 8
  %210 = load %struct.expr_t*, %struct.expr_t** %9, align 8
  %211 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %210, i32 0, i32 1
  store %struct.type_t* %209, %struct.type_t** %211, align 8
  store i32 0, i32* %5, align 4
  br label %212

; <label>:212:                                    ; preds = %187, %179, %128, %95, %60
  %213 = load i32, i32* %5, align 4
  ret i32 %213
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define %struct.val_t* @symbol_table_get(%struct.symbol_table_t*, i8*) #0 {
  %3 = alloca %struct.val_t*, align 8
  %4 = alloca %struct.symbol_table_t*, align 8
  %5 = alloca i8*, align 8
  %6 = alloca i32, align 4
  store %struct.symbol_table_t* %0, %struct.symbol_table_t** %4, align 8
  store i8* %1, i8** %5, align 8
  store i32 0, i32* %6, align 4
  br label %7

; <label>:7:                                      ; preds = %32, %2
  %8 = load i32, i32* %6, align 4
  %9 = load %struct.symbol_table_t*, %struct.symbol_table_t** %4, align 8
  %10 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %9, i32 0, i32 1
  %11 = load i32, i32* %10, align 8
  %12 = icmp slt i32 %8, %11
  br i1 %12, label %13, label %35

; <label>:13:                                     ; preds = %7
  %14 = load %struct.symbol_table_t*, %struct.symbol_table_t** %4, align 8
  %15 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %14, i32 0, i32 3
  %16 = load i8**, i8*** %15, align 8
  %17 = load i32, i32* %6, align 4
  %18 = sext i32 %17 to i64
  %19 = getelementptr inbounds i8*, i8** %16, i64 %18
  %20 = load i8*, i8** %19, align 8
  %21 = load i8*, i8** %5, align 8
  %22 = call i32 @strcmp(i8* %20, i8* %21)
  %23 = icmp eq i32 %22, 0
  br i1 %23, label %24, label %31

; <label>:24:                                     ; preds = %13
  %25 = load %struct.symbol_table_t*, %struct.symbol_table_t** %4, align 8
  %26 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %25, i32 0, i32 4
  %27 = load %struct.val_t*, %struct.val_t** %26, align 8
  %28 = load i32, i32* %6, align 4
  %29 = sext i32 %28 to i64
  %30 = getelementptr inbounds %struct.val_t, %struct.val_t* %27, i64 %29
  store %struct.val_t* %30, %struct.val_t** %3, align 8
  br label %47

; <label>:31:                                     ; preds = %13
  br label %32

; <label>:32:                                     ; preds = %31
  %33 = load i32, i32* %6, align 4
  %34 = add nsw i32 %33, 1
  store i32 %34, i32* %6, align 4
  br label %7

; <label>:35:                                     ; preds = %7
  %36 = load %struct.symbol_table_t*, %struct.symbol_table_t** %4, align 8
  %37 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %36, i32 0, i32 0
  %38 = load %struct.symbol_table_t*, %struct.symbol_table_t** %37, align 8
  %39 = icmp ne %struct.symbol_table_t* %38, null
  br i1 %39, label %40, label %46

; <label>:40:                                     ; preds = %35
  %41 = load %struct.symbol_table_t*, %struct.symbol_table_t** %4, align 8
  %42 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %41, i32 0, i32 0
  %43 = load %struct.symbol_table_t*, %struct.symbol_table_t** %42, align 8
  %44 = load i8*, i8** %5, align 8
  %45 = call %struct.val_t* @symbol_table_get(%struct.symbol_table_t* %43, i8* %44)
  store %struct.val_t* %45, %struct.val_t** %3, align 8
  br label %47

; <label>:46:                                     ; preds = %35
  store %struct.val_t* null, %struct.val_t** %3, align 8
  br label %47

; <label>:47:                                     ; preds = %46, %40, %24
  %48 = load %struct.val_t*, %struct.val_t** %3, align 8
  ret %struct.val_t* %48
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define %struct.val_t* @module_deps_symbol_find(%struct.module_t*, i8*) #0 {
  %3 = alloca %struct.val_t*, align 8
  %4 = alloca %struct.module_t*, align 8
  %5 = alloca i8*, align 8
  %6 = alloca %struct.val_t*, align 8
  %7 = alloca i32, align 4
  store %struct.module_t* %0, %struct.module_t** %4, align 8
  store i8* %1, i8** %5, align 8
  store i32 0, i32* %7, align 4
  br label %8

; <label>:8:                                      ; preds = %30, %2
  %9 = load i32, i32* %7, align 4
  %10 = load %struct.module_t*, %struct.module_t** %4, align 8
  %11 = getelementptr inbounds %struct.module_t, %struct.module_t* %10, i32 0, i32 1
  %12 = load i32, i32* %11, align 8
  %13 = icmp slt i32 %9, %12
  br i1 %13, label %14, label %33

; <label>:14:                                     ; preds = %8
  %15 = load %struct.module_t*, %struct.module_t** %4, align 8
  %16 = getelementptr inbounds %struct.module_t, %struct.module_t* %15, i32 0, i32 2
  %17 = load %struct.module_t**, %struct.module_t*** %16, align 8
  %18 = load i32, i32* %7, align 4
  %19 = sext i32 %18 to i64
  %20 = getelementptr inbounds %struct.module_t*, %struct.module_t** %17, i64 %19
  %21 = load %struct.module_t*, %struct.module_t** %20, align 8
  %22 = getelementptr inbounds %struct.module_t, %struct.module_t* %21, i32 0, i32 5
  %23 = load i8*, i8** %5, align 8
  %24 = call %struct.val_t* @symbol_table_get(%struct.symbol_table_t* %22, i8* %23)
  store %struct.val_t* %24, %struct.val_t** %6, align 8
  %25 = load %struct.val_t*, %struct.val_t** %6, align 8
  %26 = icmp ne %struct.val_t* %25, null
  br i1 %26, label %27, label %29

; <label>:27:                                     ; preds = %14
  %28 = load %struct.val_t*, %struct.val_t** %6, align 8
  store %struct.val_t* %28, %struct.val_t** %3, align 8
  br label %34

; <label>:29:                                     ; preds = %14
  br label %30

; <label>:30:                                     ; preds = %29
  %31 = load i32, i32* %7, align 4
  %32 = add nsw i32 %31, 1
  store i32 %32, i32* %7, align 4
  br label %8

; <label>:33:                                     ; preds = %8
  store %struct.val_t* null, %struct.val_t** %3, align 8
  br label %34

; <label>:34:                                     ; preds = %33, %27
  %35 = load %struct.val_t*, %struct.val_t** %3, align 8
  ret %struct.val_t* %35
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @parse_defun(%struct.module_t*, %struct.symbol_table_t*, %struct.node_t*) #0 {
  %4 = alloca i32, align 4
  %5 = alloca %struct.module_t*, align 8
  %6 = alloca %struct.symbol_table_t*, align 8
  %7 = alloca %struct.node_t*, align 8
  %8 = alloca %struct.node_t*, align 8
  %9 = alloca i8*, align 8
  %10 = alloca %struct.val_t*, align 8
  %11 = alloca %struct.type_t*, align 8
  %12 = alloca %struct.expr_t*, align 8
  %13 = alloca %struct.node_t*, align 8
  %14 = alloca i32, align 4
  store %struct.module_t* %0, %struct.module_t** %5, align 8
  store %struct.symbol_table_t* %1, %struct.symbol_table_t** %6, align 8
  store %struct.node_t* %2, %struct.node_t** %7, align 8
  %15 = load %struct.node_t*, %struct.node_t** %7, align 8
  %16 = getelementptr inbounds %struct.node_t, %struct.node_t* %15, i32 0, i32 1
  %17 = bitcast %union.anon.11* %16 to %struct.list_t**
  %18 = load %struct.list_t*, %struct.list_t** %17, align 8
  %19 = getelementptr inbounds %struct.list_t, %struct.list_t* %18, i32 0, i32 1
  %20 = load %struct.node_t*, %struct.node_t** %19, align 8
  %21 = getelementptr inbounds %struct.node_t, %struct.node_t* %20, i32 0, i32 2
  %22 = load %struct.node_t*, %struct.node_t** %21, align 8
  store %struct.node_t* %22, %struct.node_t** %8, align 8
  %23 = load %struct.node_t*, %struct.node_t** %8, align 8
  %24 = getelementptr inbounds %struct.node_t, %struct.node_t* %23, i32 0, i32 0
  %25 = load i32, i32* %24, align 8
  %26 = icmp ne i32 %25, 1
  br i1 %26, label %37, label %27

; <label>:27:                                     ; preds = %3
  %28 = load %struct.node_t*, %struct.node_t** %8, align 8
  %29 = getelementptr inbounds %struct.node_t, %struct.node_t* %28, i32 0, i32 1
  %30 = bitcast %union.anon.11* %29 to %struct.list_t**
  %31 = load %struct.list_t*, %struct.list_t** %30, align 8
  %32 = getelementptr inbounds %struct.list_t, %struct.list_t* %31, i32 0, i32 1
  %33 = load %struct.node_t*, %struct.node_t** %32, align 8
  %34 = getelementptr inbounds %struct.node_t, %struct.node_t* %33, i32 0, i32 0
  %35 = load i32, i32* %34, align 8
  %36 = icmp ne i32 %35, 0
  br i1 %36, label %37, label %40

; <label>:37:                                     ; preds = %27, %3
  %38 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %39 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %38, i8* getelementptr inbounds ([18 x i8], [18 x i8]* @.str.32, i32 0, i32 0))
  store i32 1, i32* %4, align 4
  br label %252

; <label>:40:                                     ; preds = %27
  %41 = load %struct.node_t*, %struct.node_t** %8, align 8
  %42 = getelementptr inbounds %struct.node_t, %struct.node_t* %41, i32 0, i32 1
  %43 = bitcast %union.anon.11* %42 to %struct.list_t**
  %44 = load %struct.list_t*, %struct.list_t** %43, align 8
  %45 = getelementptr inbounds %struct.list_t, %struct.list_t* %44, i32 0, i32 1
  %46 = load %struct.node_t*, %struct.node_t** %45, align 8
  %47 = getelementptr inbounds %struct.node_t, %struct.node_t* %46, i32 0, i32 1
  %48 = bitcast %union.anon.11* %47 to %struct.atom_t**
  %49 = load %struct.atom_t*, %struct.atom_t** %48, align 8
  %50 = getelementptr inbounds %struct.atom_t, %struct.atom_t* %49, i32 0, i32 0
  %51 = load i8*, i8** %50, align 8
  store i8* %51, i8** %9, align 8
  %52 = load %struct.symbol_table_t*, %struct.symbol_table_t** %6, align 8
  %53 = load i8*, i8** %9, align 8
  %54 = call %struct.val_t* @symbol_table_get(%struct.symbol_table_t* %52, i8* %53)
  store %struct.val_t* %54, %struct.val_t** %10, align 8
  %55 = load %struct.val_t*, %struct.val_t** %10, align 8
  %56 = icmp eq %struct.val_t* %55, null
  br i1 %56, label %57, label %61

; <label>:57:                                     ; preds = %40
  %58 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %59 = load i8*, i8** %9, align 8
  %60 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %58, i8* getelementptr inbounds ([32 x i8], [32 x i8]* @.str.33, i32 0, i32 0), i8* %59)
  store i32 1, i32* %4, align 4
  br label %252

; <label>:61:                                     ; preds = %40
  %62 = load %struct.val_t*, %struct.val_t** %10, align 8
  %63 = getelementptr inbounds %struct.val_t, %struct.val_t* %62, i32 0, i32 0
  %64 = load %struct.type_t*, %struct.type_t** %63, align 8
  store %struct.type_t* %64, %struct.type_t** %11, align 8
  %65 = load %struct.type_t*, %struct.type_t** %11, align 8
  %66 = getelementptr inbounds %struct.type_t, %struct.type_t* %65, i32 0, i32 1
  %67 = load i32, i32* %66, align 8
  %68 = icmp ne i32 %67, 2
  br i1 %68, label %69, label %73

; <label>:69:                                     ; preds = %61
  %70 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %71 = load i8*, i8** %9, align 8
  %72 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %70, i8* getelementptr inbounds ([22 x i8], [22 x i8]* @.str.22, i32 0, i32 0), i8* %71)
  store i32 1, i32* %4, align 4
  br label %252

; <label>:73:                                     ; preds = %61
  %74 = load %struct.type_t*, %struct.type_t** %11, align 8
  %75 = getelementptr inbounds %struct.type_t, %struct.type_t* %74, i32 0, i32 3
  %76 = load i32, i32* %75, align 8
  %77 = load %struct.node_t*, %struct.node_t** %8, align 8
  %78 = getelementptr inbounds %struct.node_t, %struct.node_t* %77, i32 0, i32 1
  %79 = bitcast %union.anon.11* %78 to %struct.list_t**
  %80 = load %struct.list_t*, %struct.list_t** %79, align 8
  %81 = getelementptr inbounds %struct.list_t, %struct.list_t* %80, i32 0, i32 0
  %82 = load i32, i32* %81, align 8
  %83 = icmp ne i32 %76, %82
  br i1 %83, label %84, label %88

; <label>:84:                                     ; preds = %73
  %85 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %86 = load i8*, i8** %9, align 8
  %87 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %85, i8* getelementptr inbounds ([45 x i8], [45 x i8]* @.str.34, i32 0, i32 0), i8* %86)
  store i32 1, i32* %4, align 4
  br label %252

; <label>:88:                                     ; preds = %73
  %89 = call %struct.expr_t* @expr_new()
  store %struct.expr_t* %89, %struct.expr_t** %12, align 8
  %90 = load %struct.expr_t*, %struct.expr_t** %12, align 8
  %91 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %90, i32 0, i32 0
  store i32 3, i32* %91, align 8
  %92 = call %struct.expr_t* @expr_new()
  %93 = load %struct.expr_t*, %struct.expr_t** %12, align 8
  %94 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %93, i32 0, i32 2
  %95 = bitcast %union.anon.1* %94 to %struct.anon.5*
  %96 = getelementptr inbounds %struct.anon.5, %struct.anon.5* %95, i32 0, i32 1
  store %struct.expr_t* %92, %struct.expr_t** %96, align 8
  %97 = load %struct.symbol_table_t*, %struct.symbol_table_t** %6, align 8
  %98 = load %struct.type_t*, %struct.type_t** %11, align 8
  %99 = getelementptr inbounds %struct.type_t, %struct.type_t* %98, i32 0, i32 3
  %100 = load i32, i32* %99, align 8
  %101 = sub nsw i32 %100, 1
  %102 = call %struct.symbol_table_t* @symbol_table_new(%struct.symbol_table_t* %97, i32 %101)
  %103 = load %struct.expr_t*, %struct.expr_t** %12, align 8
  %104 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %103, i32 0, i32 2
  %105 = bitcast %union.anon.1* %104 to %struct.anon.5*
  %106 = getelementptr inbounds %struct.anon.5, %struct.anon.5* %105, i32 0, i32 0
  store %struct.symbol_table_t* %102, %struct.symbol_table_t** %106, align 8
  %107 = load %struct.expr_t*, %struct.expr_t** %12, align 8
  %108 = load %struct.val_t*, %struct.val_t** %10, align 8
  %109 = getelementptr inbounds %struct.val_t, %struct.val_t* %108, i32 0, i32 1
  %110 = bitcast %union.anon* %109 to %struct.anon.0*
  %111 = getelementptr inbounds %struct.anon.0, %struct.anon.0* %110, i32 0, i32 0
  store %struct.expr_t* %107, %struct.expr_t** %111, align 8
  %112 = load %struct.node_t*, %struct.node_t** %8, align 8
  %113 = getelementptr inbounds %struct.node_t, %struct.node_t* %112, i32 0, i32 1
  %114 = bitcast %union.anon.11* %113 to %struct.list_t**
  %115 = load %struct.list_t*, %struct.list_t** %114, align 8
  %116 = getelementptr inbounds %struct.list_t, %struct.list_t* %115, i32 0, i32 1
  %117 = load %struct.node_t*, %struct.node_t** %116, align 8
  %118 = getelementptr inbounds %struct.node_t, %struct.node_t* %117, i32 0, i32 2
  %119 = load %struct.node_t*, %struct.node_t** %118, align 8
  store %struct.node_t* %119, %struct.node_t** %13, align 8
  store i32 0, i32* %14, align 4
  br label %120

; <label>:120:                                    ; preds = %181, %88
  %121 = load i32, i32* %14, align 4
  %122 = load %struct.type_t*, %struct.type_t** %11, align 8
  %123 = getelementptr inbounds %struct.type_t, %struct.type_t* %122, i32 0, i32 3
  %124 = load i32, i32* %123, align 8
  %125 = sub nsw i32 %124, 1
  %126 = icmp slt i32 %121, %125
  br i1 %126, label %127, label %184

; <label>:127:                                    ; preds = %120
  %128 = load %struct.node_t*, %struct.node_t** %13, align 8
  %129 = getelementptr inbounds %struct.node_t, %struct.node_t* %128, i32 0, i32 0
  %130 = load i32, i32* %129, align 8
  %131 = icmp ne i32 %130, 0
  br i1 %131, label %140, label %132

; <label>:132:                                    ; preds = %127
  %133 = load %struct.node_t*, %struct.node_t** %13, align 8
  %134 = getelementptr inbounds %struct.node_t, %struct.node_t* %133, i32 0, i32 1
  %135 = bitcast %union.anon.11* %134 to %struct.atom_t**
  %136 = load %struct.atom_t*, %struct.atom_t** %135, align 8
  %137 = getelementptr inbounds %struct.atom_t, %struct.atom_t* %136, i32 0, i32 1
  %138 = load i32, i32* %137, align 8
  %139 = icmp ne i32 %138, 1
  br i1 %139, label %140, label %143

; <label>:140:                                    ; preds = %132, %127
  %141 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %142 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %141, i8* getelementptr inbounds ([43 x i8], [43 x i8]* @.str.35, i32 0, i32 0))
  store i32 1, i32* %4, align 4
  br label %252

; <label>:143:                                    ; preds = %132
  %144 = load %struct.node_t*, %struct.node_t** %13, align 8
  %145 = getelementptr inbounds %struct.node_t, %struct.node_t* %144, i32 0, i32 1
  %146 = bitcast %union.anon.11* %145 to %struct.atom_t**
  %147 = load %struct.atom_t*, %struct.atom_t** %146, align 8
  %148 = getelementptr inbounds %struct.atom_t, %struct.atom_t* %147, i32 0, i32 0
  %149 = load i8*, i8** %148, align 8
  %150 = load %struct.expr_t*, %struct.expr_t** %12, align 8
  %151 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %150, i32 0, i32 2
  %152 = bitcast %union.anon.1* %151 to %struct.anon.5*
  %153 = getelementptr inbounds %struct.anon.5, %struct.anon.5* %152, i32 0, i32 0
  %154 = load %struct.symbol_table_t*, %struct.symbol_table_t** %153, align 8
  %155 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %154, i32 0, i32 3
  %156 = load i8**, i8*** %155, align 8
  %157 = load i32, i32* %14, align 4
  %158 = sext i32 %157 to i64
  %159 = getelementptr inbounds i8*, i8** %156, i64 %158
  store i8* %149, i8** %159, align 8
  %160 = load %struct.type_t*, %struct.type_t** %11, align 8
  %161 = getelementptr inbounds %struct.type_t, %struct.type_t* %160, i32 0, i32 4
  %162 = load %struct.type_t**, %struct.type_t*** %161, align 8
  %163 = load i32, i32* %14, align 4
  %164 = sext i32 %163 to i64
  %165 = getelementptr inbounds %struct.type_t*, %struct.type_t** %162, i64 %164
  %166 = load %struct.type_t*, %struct.type_t** %165, align 8
  %167 = load %struct.expr_t*, %struct.expr_t** %12, align 8
  %168 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %167, i32 0, i32 2
  %169 = bitcast %union.anon.1* %168 to %struct.anon.5*
  %170 = getelementptr inbounds %struct.anon.5, %struct.anon.5* %169, i32 0, i32 0
  %171 = load %struct.symbol_table_t*, %struct.symbol_table_t** %170, align 8
  %172 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %171, i32 0, i32 4
  %173 = load %struct.val_t*, %struct.val_t** %172, align 8
  %174 = load i32, i32* %14, align 4
  %175 = sext i32 %174 to i64
  %176 = getelementptr inbounds %struct.val_t, %struct.val_t* %173, i64 %175
  %177 = getelementptr inbounds %struct.val_t, %struct.val_t* %176, i32 0, i32 0
  store %struct.type_t* %166, %struct.type_t** %177, align 8
  %178 = load %struct.node_t*, %struct.node_t** %13, align 8
  %179 = getelementptr inbounds %struct.node_t, %struct.node_t* %178, i32 0, i32 2
  %180 = load %struct.node_t*, %struct.node_t** %179, align 8
  store %struct.node_t* %180, %struct.node_t** %13, align 8
  br label %181

; <label>:181:                                    ; preds = %143
  %182 = load i32, i32* %14, align 4
  %183 = add nsw i32 %182, 1
  store i32 %183, i32* %14, align 4
  br label %120

; <label>:184:                                    ; preds = %120
  %185 = load %struct.node_t*, %struct.node_t** %7, align 8
  %186 = getelementptr inbounds %struct.node_t, %struct.node_t* %185, i32 0, i32 1
  %187 = bitcast %union.anon.11* %186 to %struct.list_t**
  %188 = load %struct.list_t*, %struct.list_t** %187, align 8
  %189 = getelementptr inbounds %struct.list_t, %struct.list_t* %188, i32 0, i32 0
  %190 = load i32, i32* %189, align 8
  %191 = icmp eq i32 %190, 3
  br i1 %191, label %192, label %211

; <label>:192:                                    ; preds = %184
  %193 = load %struct.module_t*, %struct.module_t** %5, align 8
  %194 = load %struct.expr_t*, %struct.expr_t** %12, align 8
  %195 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %194, i32 0, i32 2
  %196 = bitcast %union.anon.1* %195 to %struct.anon.5*
  %197 = getelementptr inbounds %struct.anon.5, %struct.anon.5* %196, i32 0, i32 0
  %198 = load %struct.symbol_table_t*, %struct.symbol_table_t** %197, align 8
  %199 = load %struct.node_t*, %struct.node_t** %8, align 8
  %200 = getelementptr inbounds %struct.node_t, %struct.node_t* %199, i32 0, i32 2
  %201 = load %struct.node_t*, %struct.node_t** %200, align 8
  %202 = load %struct.expr_t*, %struct.expr_t** %12, align 8
  %203 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %202, i32 0, i32 2
  %204 = bitcast %union.anon.1* %203 to %struct.anon.5*
  %205 = getelementptr inbounds %struct.anon.5, %struct.anon.5* %204, i32 0, i32 1
  %206 = load %struct.expr_t*, %struct.expr_t** %205, align 8
  %207 = call i32 @parse_expr(%struct.module_t* %193, %struct.symbol_table_t* %198, %struct.node_t* %201, %struct.expr_t* %206)
  %208 = icmp ne i32 %207, 0
  br i1 %208, label %209, label %210

; <label>:209:                                    ; preds = %192
  store i32 1, i32* %4, align 4
  br label %252

; <label>:210:                                    ; preds = %192
  br label %251

; <label>:211:                                    ; preds = %184
  %212 = load %struct.expr_t*, %struct.expr_t** %12, align 8
  %213 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %212, i32 0, i32 2
  %214 = bitcast %union.anon.1* %213 to %struct.anon.5*
  %215 = getelementptr inbounds %struct.anon.5, %struct.anon.5* %214, i32 0, i32 1
  %216 = load %struct.expr_t*, %struct.expr_t** %215, align 8
  %217 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %216, i32 0, i32 0
  store i32 5, i32* %217, align 8
  %218 = load %struct.node_t*, %struct.node_t** %7, align 8
  %219 = getelementptr inbounds %struct.node_t, %struct.node_t* %218, i32 0, i32 1
  %220 = bitcast %union.anon.11* %219 to %struct.list_t**
  %221 = load %struct.list_t*, %struct.list_t** %220, align 8
  %222 = getelementptr inbounds %struct.list_t, %struct.list_t* %221, i32 0, i32 0
  %223 = load i32, i32* %222, align 8
  %224 = sub nsw i32 %223, 2
  %225 = load %struct.expr_t*, %struct.expr_t** %12, align 8
  %226 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %225, i32 0, i32 2
  %227 = bitcast %union.anon.1* %226 to %struct.anon.5*
  %228 = getelementptr inbounds %struct.anon.5, %struct.anon.5* %227, i32 0, i32 1
  %229 = load %struct.expr_t*, %struct.expr_t** %228, align 8
  %230 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %229, i32 0, i32 2
  %231 = bitcast %union.anon.1* %230 to %struct.anon.7*
  %232 = getelementptr inbounds %struct.anon.7, %struct.anon.7* %231, i32 0, i32 0
  store i32 %224, i32* %232, align 8
  %233 = load %struct.module_t*, %struct.module_t** %5, align 8
  %234 = load %struct.expr_t*, %struct.expr_t** %12, align 8
  %235 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %234, i32 0, i32 2
  %236 = bitcast %union.anon.1* %235 to %struct.anon.5*
  %237 = getelementptr inbounds %struct.anon.5, %struct.anon.5* %236, i32 0, i32 0
  %238 = load %struct.symbol_table_t*, %struct.symbol_table_t** %237, align 8
  %239 = load %struct.node_t*, %struct.node_t** %8, align 8
  %240 = getelementptr inbounds %struct.node_t, %struct.node_t* %239, i32 0, i32 2
  %241 = load %struct.node_t*, %struct.node_t** %240, align 8
  %242 = load %struct.expr_t*, %struct.expr_t** %12, align 8
  %243 = getelementptr inbounds %struct.expr_t, %struct.expr_t* %242, i32 0, i32 2
  %244 = bitcast %union.anon.1* %243 to %struct.anon.5*
  %245 = getelementptr inbounds %struct.anon.5, %struct.anon.5* %244, i32 0, i32 1
  %246 = load %struct.expr_t*, %struct.expr_t** %245, align 8
  %247 = call i32 @parse_progn(%struct.module_t* %233, %struct.symbol_table_t* %238, %struct.node_t* %241, %struct.expr_t* %246)
  %248 = icmp ne i32 %247, 0
  br i1 %248, label %249, label %250

; <label>:249:                                    ; preds = %211
  store i32 1, i32* %4, align 4
  br label %252

; <label>:250:                                    ; preds = %211
  br label %251

; <label>:251:                                    ; preds = %250, %210
  store i32 0, i32* %4, align 4
  br label %252

; <label>:252:                                    ; preds = %251, %249, %209, %140, %84, %69, %57, %37
  %253 = load i32, i32* %4, align 4
  ret i32 %253
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define %struct.val_t* @symbol_table_add(%struct.symbol_table_t*, i8*, %struct.val_t*) #0 {
  %4 = alloca %struct.symbol_table_t*, align 8
  %5 = alloca i8*, align 8
  %6 = alloca %struct.val_t*, align 8
  %7 = alloca i8**, align 8
  %8 = alloca %struct.val_t*, align 8
  %9 = alloca %struct.val_t*, align 8
  store %struct.symbol_table_t* %0, %struct.symbol_table_t** %4, align 8
  store i8* %1, i8** %5, align 8
  store %struct.val_t* %2, %struct.val_t** %6, align 8
  %10 = load %struct.symbol_table_t*, %struct.symbol_table_t** %4, align 8
  %11 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %10, i32 0, i32 1
  %12 = load i32, i32* %11, align 8
  %13 = load %struct.symbol_table_t*, %struct.symbol_table_t** %4, align 8
  %14 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %13, i32 0, i32 2
  %15 = load i32, i32* %14, align 4
  %16 = icmp eq i32 %12, %15
  br i1 %16, label %17, label %82

; <label>:17:                                     ; preds = %3
  %18 = load %struct.symbol_table_t*, %struct.symbol_table_t** %4, align 8
  %19 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %18, i32 0, i32 2
  %20 = load i32, i32* %19, align 4
  %21 = mul nsw i32 %20, 2
  %22 = load %struct.symbol_table_t*, %struct.symbol_table_t** %4, align 8
  %23 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %22, i32 0, i32 2
  store i32 %21, i32* %23, align 4
  %24 = load %struct.symbol_table_t*, %struct.symbol_table_t** %4, align 8
  %25 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %24, i32 0, i32 2
  %26 = load i32, i32* %25, align 4
  %27 = sext i32 %26 to i64
  %28 = mul i64 8, %27
  %29 = call i8* @malloc(i64 %28) #8
  %30 = bitcast i8* %29 to i8**
  store i8** %30, i8*** %7, align 8
  %31 = load %struct.symbol_table_t*, %struct.symbol_table_t** %4, align 8
  %32 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %31, i32 0, i32 2
  %33 = load i32, i32* %32, align 4
  %34 = sext i32 %33 to i64
  %35 = mul i64 32, %34
  %36 = call i8* @malloc(i64 %35) #8
  %37 = bitcast i8* %36 to %struct.val_t*
  store %struct.val_t* %37, %struct.val_t** %8, align 8
  %38 = load i8**, i8*** %7, align 8
  %39 = bitcast i8** %38 to i8*
  %40 = load %struct.symbol_table_t*, %struct.symbol_table_t** %4, align 8
  %41 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %40, i32 0, i32 3
  %42 = load i8**, i8*** %41, align 8
  %43 = bitcast i8** %42 to i8*
  %44 = load %struct.symbol_table_t*, %struct.symbol_table_t** %4, align 8
  %45 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %44, i32 0, i32 1
  %46 = load i32, i32* %45, align 8
  %47 = sext i32 %46 to i64
  %48 = mul i64 8, %47
  %49 = load i8**, i8*** %7, align 8
  %50 = bitcast i8** %49 to i8*
  %51 = call i64 @llvm.objectsize.i64.p0i8(i8* %50, i1 false, i1 true)
  %52 = call i8* @__memcpy_chk(i8* %39, i8* %43, i64 %48, i64 %51) #9
  %53 = load %struct.val_t*, %struct.val_t** %8, align 8
  %54 = bitcast %struct.val_t* %53 to i8*
  %55 = load %struct.symbol_table_t*, %struct.symbol_table_t** %4, align 8
  %56 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %55, i32 0, i32 4
  %57 = load %struct.val_t*, %struct.val_t** %56, align 8
  %58 = bitcast %struct.val_t* %57 to i8*
  %59 = load %struct.symbol_table_t*, %struct.symbol_table_t** %4, align 8
  %60 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %59, i32 0, i32 1
  %61 = load i32, i32* %60, align 8
  %62 = sext i32 %61 to i64
  %63 = mul i64 32, %62
  %64 = load %struct.val_t*, %struct.val_t** %8, align 8
  %65 = bitcast %struct.val_t* %64 to i8*
  %66 = call i64 @llvm.objectsize.i64.p0i8(i8* %65, i1 false, i1 true)
  %67 = call i8* @__memcpy_chk(i8* %54, i8* %58, i64 %63, i64 %66) #9
  %68 = load %struct.symbol_table_t*, %struct.symbol_table_t** %4, align 8
  %69 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %68, i32 0, i32 3
  %70 = load i8**, i8*** %69, align 8
  %71 = bitcast i8** %70 to i8*
  call void @free(i8* %71)
  %72 = load %struct.symbol_table_t*, %struct.symbol_table_t** %4, align 8
  %73 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %72, i32 0, i32 4
  %74 = load %struct.val_t*, %struct.val_t** %73, align 8
  %75 = bitcast %struct.val_t* %74 to i8*
  call void @free(i8* %75)
  %76 = load i8**, i8*** %7, align 8
  %77 = load %struct.symbol_table_t*, %struct.symbol_table_t** %4, align 8
  %78 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %77, i32 0, i32 3
  store i8** %76, i8*** %78, align 8
  %79 = load %struct.val_t*, %struct.val_t** %8, align 8
  %80 = load %struct.symbol_table_t*, %struct.symbol_table_t** %4, align 8
  %81 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %80, i32 0, i32 4
  store %struct.val_t* %79, %struct.val_t** %81, align 8
  br label %82

; <label>:82:                                     ; preds = %17, %3
  %83 = load %struct.symbol_table_t*, %struct.symbol_table_t** %4, align 8
  %84 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %83, i32 0, i32 4
  %85 = load %struct.val_t*, %struct.val_t** %84, align 8
  %86 = load %struct.symbol_table_t*, %struct.symbol_table_t** %4, align 8
  %87 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %86, i32 0, i32 1
  %88 = load i32, i32* %87, align 8
  %89 = sext i32 %88 to i64
  %90 = getelementptr inbounds %struct.val_t, %struct.val_t* %85, i64 %89
  store %struct.val_t* %90, %struct.val_t** %9, align 8
  %91 = load i8*, i8** %5, align 8
  %92 = load %struct.symbol_table_t*, %struct.symbol_table_t** %4, align 8
  %93 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %92, i32 0, i32 3
  %94 = load i8**, i8*** %93, align 8
  %95 = load %struct.symbol_table_t*, %struct.symbol_table_t** %4, align 8
  %96 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %95, i32 0, i32 1
  %97 = load i32, i32* %96, align 8
  %98 = sext i32 %97 to i64
  %99 = getelementptr inbounds i8*, i8** %94, i64 %98
  store i8* %91, i8** %99, align 8
  %100 = load %struct.symbol_table_t*, %struct.symbol_table_t** %4, align 8
  %101 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %100, i32 0, i32 4
  %102 = load %struct.val_t*, %struct.val_t** %101, align 8
  %103 = load %struct.symbol_table_t*, %struct.symbol_table_t** %4, align 8
  %104 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %103, i32 0, i32 1
  %105 = load i32, i32* %104, align 8
  %106 = sext i32 %105 to i64
  %107 = getelementptr inbounds %struct.val_t, %struct.val_t* %102, i64 %106
  %108 = load %struct.val_t*, %struct.val_t** %6, align 8
  %109 = bitcast %struct.val_t* %107 to i8*
  %110 = bitcast %struct.val_t* %108 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %109, i8* %110, i64 32, i32 8, i1 false)
  %111 = load %struct.symbol_table_t*, %struct.symbol_table_t** %4, align 8
  %112 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %111, i32 0, i32 1
  %113 = load i32, i32* %112, align 8
  %114 = add nsw i32 %113, 1
  store i32 %114, i32* %112, align 8
  %115 = load %struct.val_t*, %struct.val_t** %9, align 8
  ret %struct.val_t* %115
}

; Function Attrs: nounwind
declare i8* @__memcpy_chk(i8*, i8*, i64, i64) #4

; Function Attrs: nounwind readnone speculatable
declare i64 @llvm.objectsize.i64.p0i8(i8*, i1, i1) #5

declare void @free(i8*) #2

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i32, i1) #6

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @module_cache_init() #0 {
  %1 = alloca i32, align 4
  %2 = alloca %struct.type_t, align 8
  %3 = alloca %struct.type_t, align 8
  %4 = alloca %struct.type_t, align 8
  %5 = alloca %struct.type_t, align 8
  %6 = alloca %struct.type_t, align 8
  %7 = alloca %struct.type_t, align 8
  %8 = call i8* @getcwd(i8* getelementptr inbounds ([1024 x i8], [1024 x i8]* @cwd, i32 0, i32 0), i64 1024)
  %9 = icmp eq i8* %8, null
  br i1 %9, label %10, label %13

; <label>:10:                                     ; preds = %0
  %11 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %12 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %11, i8* getelementptr inbounds ([19 x i8], [19 x i8]* @.str.36, i32 0, i32 0))
  store i32 1, i32* %1, align 4
  br label %106

; <label>:13:                                     ; preds = %0
  store i32 0, i32* getelementptr inbounds (%struct.module_cache_t, %struct.module_cache_t* @cache, i32 0, i32 0), align 8
  store i32 10, i32* getelementptr inbounds (%struct.module_cache_t, %struct.module_cache_t* @cache, i32 0, i32 1), align 4
  %14 = load i32, i32* getelementptr inbounds (%struct.module_cache_t, %struct.module_cache_t* @cache, i32 0, i32 1), align 4
  %15 = sext i32 %14 to i64
  %16 = mul i64 %15, 80
  %17 = call i8* @malloc(i64 %16) #8
  %18 = bitcast i8* %17 to %struct.module_t*
  store %struct.module_t* %18, %struct.module_t** getelementptr inbounds (%struct.module_cache_t, %struct.module_cache_t* @cache, i32 0, i32 2), align 8
  store i32 5, i32* getelementptr inbounds (%struct.module_t, %struct.module_t* @MODULE_BUILTIN, i32 0, i32 3), align 8
  %19 = load i32, i32* getelementptr inbounds (%struct.module_t, %struct.module_t* @MODULE_BUILTIN, i32 0, i32 3), align 8
  %20 = call %struct.type_t* @type_new_i(i32 %19)
  store %struct.type_t* %20, %struct.type_t** getelementptr inbounds (%struct.module_t, %struct.module_t* @MODULE_BUILTIN, i32 0, i32 4), align 8
  %21 = bitcast %struct.type_t* %3 to i8*
  call void @llvm.memset.p0i8.i64(i8* %21, i8 0, i64 48, i32 8, i1 false)
  %22 = getelementptr inbounds %struct.type_t, %struct.type_t* %3, i32 0, i32 0
  store i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.37, i32 0, i32 0), i8** %22, align 8
  %23 = getelementptr inbounds %struct.type_t, %struct.type_t* %3, i32 0, i32 1
  store i32 1, i32* %23, align 8
  %24 = bitcast %struct.type_t* %2 to i8*
  %25 = bitcast %struct.type_t* %3 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %24, i8* %25, i64 48, i32 8, i1 false)
  %26 = load %struct.type_t*, %struct.type_t** getelementptr inbounds (%struct.module_t, %struct.module_t* @MODULE_BUILTIN, i32 0, i32 4), align 8
  %27 = bitcast %struct.type_t* %26 to i8*
  %28 = bitcast %struct.type_t* %2 to i8*
  %29 = load %struct.type_t*, %struct.type_t** getelementptr inbounds (%struct.module_t, %struct.module_t* @MODULE_BUILTIN, i32 0, i32 4), align 8
  %30 = bitcast %struct.type_t* %29 to i8*
  %31 = call i64 @llvm.objectsize.i64.p0i8(i8* %30, i1 false, i1 true)
  %32 = call i8* @__memcpy_chk(i8* %27, i8* %28, i64 48, i64 %31) #9
  %33 = load %struct.type_t*, %struct.type_t** getelementptr inbounds (%struct.module_t, %struct.module_t* @MODULE_BUILTIN, i32 0, i32 4), align 8
  store %struct.type_t* %33, %struct.type_t** @TYPE_POLY, align 8
  %34 = bitcast %struct.type_t* %4 to i8*
  call void @llvm.memset.p0i8.i64(i8* %34, i8 0, i64 48, i32 8, i1 false)
  %35 = getelementptr inbounds %struct.type_t, %struct.type_t* %4, i32 0, i32 0
  store i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.38, i32 0, i32 0), i8** %35, align 8
  %36 = getelementptr inbounds %struct.type_t, %struct.type_t* %4, i32 0, i32 1
  store i32 1, i32* %36, align 8
  %37 = bitcast %struct.type_t* %2 to i8*
  %38 = bitcast %struct.type_t* %4 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %37, i8* %38, i64 48, i32 8, i1 false)
  %39 = load %struct.type_t*, %struct.type_t** getelementptr inbounds (%struct.module_t, %struct.module_t* @MODULE_BUILTIN, i32 0, i32 4), align 8
  %40 = getelementptr inbounds %struct.type_t, %struct.type_t* %39, i64 1
  %41 = bitcast %struct.type_t* %40 to i8*
  %42 = bitcast %struct.type_t* %2 to i8*
  %43 = load %struct.type_t*, %struct.type_t** getelementptr inbounds (%struct.module_t, %struct.module_t* @MODULE_BUILTIN, i32 0, i32 4), align 8
  %44 = getelementptr inbounds %struct.type_t, %struct.type_t* %43, i64 1
  %45 = bitcast %struct.type_t* %44 to i8*
  %46 = call i64 @llvm.objectsize.i64.p0i8(i8* %45, i1 false, i1 true)
  %47 = call i8* @__memcpy_chk(i8* %41, i8* %42, i64 48, i64 %46) #9
  %48 = load %struct.type_t*, %struct.type_t** getelementptr inbounds (%struct.module_t, %struct.module_t* @MODULE_BUILTIN, i32 0, i32 4), align 8
  %49 = getelementptr inbounds %struct.type_t, %struct.type_t* %48, i64 1
  store %struct.type_t* %49, %struct.type_t** @TYPE_I8, align 8
  %50 = bitcast %struct.type_t* %5 to i8*
  call void @llvm.memset.p0i8.i64(i8* %50, i8 0, i64 48, i32 8, i1 false)
  %51 = getelementptr inbounds %struct.type_t, %struct.type_t* %5, i32 0, i32 0
  store i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.39, i32 0, i32 0), i8** %51, align 8
  %52 = getelementptr inbounds %struct.type_t, %struct.type_t* %5, i32 0, i32 1
  store i32 1, i32* %52, align 8
  %53 = bitcast %struct.type_t* %2 to i8*
  %54 = bitcast %struct.type_t* %5 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %53, i8* %54, i64 48, i32 8, i1 false)
  %55 = load %struct.type_t*, %struct.type_t** getelementptr inbounds (%struct.module_t, %struct.module_t* @MODULE_BUILTIN, i32 0, i32 4), align 8
  %56 = getelementptr inbounds %struct.type_t, %struct.type_t* %55, i64 2
  %57 = bitcast %struct.type_t* %56 to i8*
  %58 = bitcast %struct.type_t* %2 to i8*
  %59 = load %struct.type_t*, %struct.type_t** getelementptr inbounds (%struct.module_t, %struct.module_t* @MODULE_BUILTIN, i32 0, i32 4), align 8
  %60 = getelementptr inbounds %struct.type_t, %struct.type_t* %59, i64 2
  %61 = bitcast %struct.type_t* %60 to i8*
  %62 = call i64 @llvm.objectsize.i64.p0i8(i8* %61, i1 false, i1 true)
  %63 = call i8* @__memcpy_chk(i8* %57, i8* %58, i64 48, i64 %62) #9
  %64 = load %struct.type_t*, %struct.type_t** getelementptr inbounds (%struct.module_t, %struct.module_t* @MODULE_BUILTIN, i32 0, i32 4), align 8
  %65 = getelementptr inbounds %struct.type_t, %struct.type_t* %64, i64 2
  store %struct.type_t* %65, %struct.type_t** @TYPE_I32, align 8
  %66 = getelementptr inbounds %struct.type_t, %struct.type_t* %6, i32 0, i32 0
  store i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.40, i32 0, i32 0), i8** %66, align 8
  %67 = getelementptr inbounds %struct.type_t, %struct.type_t* %6, i32 0, i32 1
  store i32 3, i32* %67, align 8
  %68 = getelementptr inbounds %struct.type_t, %struct.type_t* %6, i32 0, i32 2
  store i32 1, i32* %68, align 4
  %69 = getelementptr inbounds %struct.type_t, %struct.type_t* %6, i32 0, i32 3
  store i32 1, i32* %69, align 8
  %70 = getelementptr inbounds %struct.type_t, %struct.type_t* %6, i32 0, i32 4
  store %struct.type_t** @TYPE_POLY, %struct.type_t*** %70, align 8
  %71 = getelementptr inbounds %struct.type_t, %struct.type_t* %6, i32 0, i32 5
  store i8** null, i8*** %71, align 8
  %72 = getelementptr inbounds %struct.type_t, %struct.type_t* %6, i32 0, i32 6
  store %struct.LLVMOpaqueType* null, %struct.LLVMOpaqueType** %72, align 8
  %73 = bitcast %struct.type_t* %2 to i8*
  %74 = bitcast %struct.type_t* %6 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %73, i8* %74, i64 48, i32 8, i1 false)
  %75 = load %struct.type_t*, %struct.type_t** getelementptr inbounds (%struct.module_t, %struct.module_t* @MODULE_BUILTIN, i32 0, i32 4), align 8
  %76 = getelementptr inbounds %struct.type_t, %struct.type_t* %75, i64 3
  %77 = bitcast %struct.type_t* %76 to i8*
  %78 = bitcast %struct.type_t* %2 to i8*
  %79 = load %struct.type_t*, %struct.type_t** getelementptr inbounds (%struct.module_t, %struct.module_t* @MODULE_BUILTIN, i32 0, i32 4), align 8
  %80 = getelementptr inbounds %struct.type_t, %struct.type_t* %79, i64 3
  %81 = bitcast %struct.type_t* %80 to i8*
  %82 = call i64 @llvm.objectsize.i64.p0i8(i8* %81, i1 false, i1 true)
  %83 = call i8* @__memcpy_chk(i8* %77, i8* %78, i64 48, i64 %82) #9
  %84 = load %struct.type_t*, %struct.type_t** getelementptr inbounds (%struct.module_t, %struct.module_t* @MODULE_BUILTIN, i32 0, i32 4), align 8
  %85 = getelementptr inbounds %struct.type_t, %struct.type_t* %84, i64 3
  store %struct.type_t* %85, %struct.type_t** @TYPE_PTR, align 8
  %86 = getelementptr inbounds %struct.type_t, %struct.type_t* %7, i32 0, i32 0
  store i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.40, i32 0, i32 0), i8** %86, align 8
  %87 = getelementptr inbounds %struct.type_t, %struct.type_t* %7, i32 0, i32 1
  store i32 3, i32* %87, align 8
  %88 = getelementptr inbounds %struct.type_t, %struct.type_t* %7, i32 0, i32 2
  store i32 0, i32* %88, align 4
  %89 = getelementptr inbounds %struct.type_t, %struct.type_t* %7, i32 0, i32 3
  store i32 1, i32* %89, align 8
  %90 = getelementptr inbounds %struct.type_t, %struct.type_t* %7, i32 0, i32 4
  store %struct.type_t** @TYPE_I8, %struct.type_t*** %90, align 8
  %91 = getelementptr inbounds %struct.type_t, %struct.type_t* %7, i32 0, i32 5
  store i8** null, i8*** %91, align 8
  %92 = getelementptr inbounds %struct.type_t, %struct.type_t* %7, i32 0, i32 6
  store %struct.LLVMOpaqueType* null, %struct.LLVMOpaqueType** %92, align 8
  %93 = bitcast %struct.type_t* %2 to i8*
  %94 = bitcast %struct.type_t* %7 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %93, i8* %94, i64 48, i32 8, i1 false)
  %95 = load %struct.type_t*, %struct.type_t** getelementptr inbounds (%struct.module_t, %struct.module_t* @MODULE_BUILTIN, i32 0, i32 4), align 8
  %96 = getelementptr inbounds %struct.type_t, %struct.type_t* %95, i64 4
  %97 = bitcast %struct.type_t* %96 to i8*
  %98 = bitcast %struct.type_t* %2 to i8*
  %99 = load %struct.type_t*, %struct.type_t** getelementptr inbounds (%struct.module_t, %struct.module_t* @MODULE_BUILTIN, i32 0, i32 4), align 8
  %100 = getelementptr inbounds %struct.type_t, %struct.type_t* %99, i64 4
  %101 = bitcast %struct.type_t* %100 to i8*
  %102 = call i64 @llvm.objectsize.i64.p0i8(i8* %101, i1 false, i1 true)
  %103 = call i8* @__memcpy_chk(i8* %97, i8* %98, i64 48, i64 %102) #9
  %104 = load %struct.type_t*, %struct.type_t** getelementptr inbounds (%struct.module_t, %struct.module_t* @MODULE_BUILTIN, i32 0, i32 4), align 8
  %105 = getelementptr inbounds %struct.type_t, %struct.type_t* %104, i64 4
  store %struct.type_t* %105, %struct.type_t** @TYPE_CSTR, align 8
  store i32 0, i32* %1, align 4
  br label %106

; <label>:106:                                    ; preds = %13, %10
  %107 = load i32, i32* %1, align 4
  ret i32 %107
}

declare i8* @getcwd(i8*, i64) #2

declare %struct.type_t* @type_new_i(i32) #2

; Function Attrs: argmemonly nounwind
declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i32, i1) #6

; Function Attrs: noinline nounwind optnone ssp uwtable
define %struct.module_t* @module_cache_next() #0 {
  %1 = alloca %struct.module_t*, align 8
  %2 = load i32, i32* getelementptr inbounds (%struct.module_cache_t, %struct.module_cache_t* @cache, i32 0, i32 0), align 8
  %3 = load i32, i32* getelementptr inbounds (%struct.module_cache_t, %struct.module_cache_t* @cache, i32 0, i32 1), align 4
  %4 = sub nsw i32 %3, 1
  %5 = icmp eq i32 %2, %4
  br i1 %5, label %6, label %27

; <label>:6:                                      ; preds = %0
  %7 = load i32, i32* getelementptr inbounds (%struct.module_cache_t, %struct.module_cache_t* @cache, i32 0, i32 1), align 4
  %8 = mul nsw i32 %7, 2
  store i32 %8, i32* getelementptr inbounds (%struct.module_cache_t, %struct.module_cache_t* @cache, i32 0, i32 1), align 4
  %9 = load i32, i32* getelementptr inbounds (%struct.module_cache_t, %struct.module_cache_t* @cache, i32 0, i32 1), align 4
  %10 = sext i32 %9 to i64
  %11 = mul i64 %10, 80
  %12 = call i8* @malloc(i64 %11) #8
  %13 = bitcast i8* %12 to %struct.module_t*
  store %struct.module_t* %13, %struct.module_t** %1, align 8
  %14 = load %struct.module_t*, %struct.module_t** %1, align 8
  %15 = bitcast %struct.module_t* %14 to i8*
  %16 = load %struct.module_t*, %struct.module_t** getelementptr inbounds (%struct.module_cache_t, %struct.module_cache_t* @cache, i32 0, i32 2), align 8
  %17 = bitcast %struct.module_t* %16 to i8*
  %18 = load i32, i32* getelementptr inbounds (%struct.module_cache_t, %struct.module_cache_t* @cache, i32 0, i32 0), align 8
  %19 = sext i32 %18 to i64
  %20 = load %struct.module_t*, %struct.module_t** %1, align 8
  %21 = bitcast %struct.module_t* %20 to i8*
  %22 = call i64 @llvm.objectsize.i64.p0i8(i8* %21, i1 false, i1 true)
  %23 = call i8* @__memcpy_chk(i8* %15, i8* %17, i64 %19, i64 %22) #9
  %24 = load %struct.module_t*, %struct.module_t** getelementptr inbounds (%struct.module_cache_t, %struct.module_cache_t* @cache, i32 0, i32 2), align 8
  %25 = bitcast %struct.module_t* %24 to i8*
  call void @free(i8* %25)
  %26 = load %struct.module_t*, %struct.module_t** %1, align 8
  store %struct.module_t* %26, %struct.module_t** getelementptr inbounds (%struct.module_cache_t, %struct.module_cache_t* @cache, i32 0, i32 2), align 8
  br label %27

; <label>:27:                                     ; preds = %6, %0
  %28 = load %struct.module_t*, %struct.module_t** getelementptr inbounds (%struct.module_cache_t, %struct.module_cache_t* @cache, i32 0, i32 2), align 8
  %29 = load i32, i32* getelementptr inbounds (%struct.module_cache_t, %struct.module_cache_t* @cache, i32 0, i32 0), align 8
  %30 = add nsw i32 %29, 1
  store i32 %30, i32* getelementptr inbounds (%struct.module_cache_t, %struct.module_cache_t* @cache, i32 0, i32 0), align 8
  %31 = sext i32 %29 to i64
  %32 = getelementptr inbounds %struct.module_t, %struct.module_t* %28, i64 %31
  ret %struct.module_t* %32
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define %struct.module_t* @module_cache_find(i8*) #0 {
  %2 = alloca %struct.module_t*, align 8
  %3 = alloca i8*, align 8
  %4 = alloca %struct.module_t*, align 8
  store i8* %0, i8** %3, align 8
  %5 = load %struct.module_t*, %struct.module_t** getelementptr inbounds (%struct.module_cache_t, %struct.module_cache_t* @cache, i32 0, i32 2), align 8
  store %struct.module_t* %5, %struct.module_t** %4, align 8
  br label %6

; <label>:6:                                      ; preds = %23, %1
  %7 = load %struct.module_t*, %struct.module_t** %4, align 8
  %8 = load %struct.module_t*, %struct.module_t** getelementptr inbounds (%struct.module_cache_t, %struct.module_cache_t* @cache, i32 0, i32 2), align 8
  %9 = load i32, i32* getelementptr inbounds (%struct.module_cache_t, %struct.module_cache_t* @cache, i32 0, i32 0), align 8
  %10 = sext i32 %9 to i64
  %11 = getelementptr inbounds %struct.module_t, %struct.module_t* %8, i64 %10
  %12 = icmp ult %struct.module_t* %7, %11
  br i1 %12, label %13, label %26

; <label>:13:                                     ; preds = %6
  %14 = load %struct.module_t*, %struct.module_t** %4, align 8
  %15 = getelementptr inbounds %struct.module_t, %struct.module_t* %14, i32 0, i32 0
  %16 = load i8*, i8** %15, align 8
  %17 = load i8*, i8** %3, align 8
  %18 = call i32 @strcmp(i8* %16, i8* %17)
  %19 = icmp eq i32 %18, 0
  br i1 %19, label %20, label %22

; <label>:20:                                     ; preds = %13
  %21 = load %struct.module_t*, %struct.module_t** %4, align 8
  store %struct.module_t* %21, %struct.module_t** %2, align 8
  br label %27

; <label>:22:                                     ; preds = %13
  br label %23

; <label>:23:                                     ; preds = %22
  %24 = load %struct.module_t*, %struct.module_t** %4, align 8
  %25 = getelementptr inbounds %struct.module_t, %struct.module_t* %24, i32 1
  store %struct.module_t* %25, %struct.module_t** %4, align 8
  br label %6

; <label>:26:                                     ; preds = %6
  store %struct.module_t* null, %struct.module_t** %2, align 8
  br label %27

; <label>:27:                                     ; preds = %26, %20
  %28 = load %struct.module_t*, %struct.module_t** %2, align 8
  ret %struct.module_t* %28
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @module_setup(%struct.module_t*, %struct.node_t*) #0 {
  %3 = alloca i32, align 4
  %4 = alloca %struct.module_t*, align 8
  %5 = alloca %struct.node_t*, align 8
  %6 = alloca %struct.node_t*, align 8
  %7 = alloca i8*, align 8
  %8 = alloca i32, align 4
  store %struct.module_t* %0, %struct.module_t** %4, align 8
  store %struct.node_t* %1, %struct.node_t** %5, align 8
  %9 = load %struct.module_t*, %struct.module_t** %4, align 8
  %10 = getelementptr inbounds %struct.module_t, %struct.module_t* %9, i32 0, i32 1
  store i32 1, i32* %10, align 8
  %11 = load %struct.module_t*, %struct.module_t** %4, align 8
  %12 = getelementptr inbounds %struct.module_t, %struct.module_t* %11, i32 0, i32 3
  store i32 0, i32* %12, align 8
  %13 = load %struct.module_t*, %struct.module_t** %4, align 8
  %14 = getelementptr inbounds %struct.module_t, %struct.module_t* %13, i32 0, i32 5
  %15 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %14, i32 0, i32 1
  store i32 0, i32* %15, align 8
  %16 = load %struct.module_t*, %struct.module_t** %4, align 8
  %17 = getelementptr inbounds %struct.module_t, %struct.module_t* %16, i32 0, i32 5
  %18 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %17, i32 0, i32 2
  store i32 0, i32* %18, align 4
  %19 = load %struct.module_t*, %struct.module_t** %4, align 8
  %20 = getelementptr inbounds %struct.module_t, %struct.module_t* %19, i32 0, i32 5
  %21 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %20, i32 0, i32 0
  store %struct.symbol_table_t* null, %struct.symbol_table_t** %21, align 8
  %22 = load %struct.node_t*, %struct.node_t** %5, align 8
  %23 = getelementptr inbounds %struct.node_t, %struct.node_t* %22, i32 0, i32 0
  %24 = load i32, i32* %23, align 8
  %25 = icmp ne i32 %24, 1
  br i1 %25, label %26, label %29

; <label>:26:                                     ; preds = %2
  %27 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %28 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %27, i8* getelementptr inbounds ([26 x i8], [26 x i8]* @.str.41, i32 0, i32 0))
  store i32 1, i32* %3, align 4
  br label %168

; <label>:29:                                     ; preds = %2
  %30 = load %struct.node_t*, %struct.node_t** %5, align 8
  %31 = getelementptr inbounds %struct.node_t, %struct.node_t* %30, i32 0, i32 1
  %32 = bitcast %union.anon.11* %31 to %struct.list_t**
  %33 = load %struct.list_t*, %struct.list_t** %32, align 8
  %34 = getelementptr inbounds %struct.list_t, %struct.list_t* %33, i32 0, i32 1
  %35 = load %struct.node_t*, %struct.node_t** %34, align 8
  store %struct.node_t* %35, %struct.node_t** %6, align 8
  store i32 0, i32* %8, align 4
  br label %36

; <label>:36:                                     ; preds = %113, %29
  %37 = load i32, i32* %8, align 4
  %38 = load %struct.node_t*, %struct.node_t** %5, align 8
  %39 = getelementptr inbounds %struct.node_t, %struct.node_t* %38, i32 0, i32 1
  %40 = bitcast %union.anon.11* %39 to %struct.list_t**
  %41 = load %struct.list_t*, %struct.list_t** %40, align 8
  %42 = getelementptr inbounds %struct.list_t, %struct.list_t* %41, i32 0, i32 0
  %43 = load i32, i32* %42, align 8
  %44 = icmp slt i32 %37, %43
  br i1 %44, label %45, label %116

; <label>:45:                                     ; preds = %36
  %46 = load %struct.node_t*, %struct.node_t** %6, align 8
  %47 = getelementptr inbounds %struct.node_t, %struct.node_t* %46, i32 0, i32 0
  %48 = load i32, i32* %47, align 8
  %49 = icmp eq i32 %48, 1
  br i1 %49, label %50, label %109

; <label>:50:                                     ; preds = %45
  %51 = load %struct.node_t*, %struct.node_t** %6, align 8
  %52 = getelementptr inbounds %struct.node_t, %struct.node_t* %51, i32 0, i32 1
  %53 = bitcast %union.anon.11* %52 to %struct.list_t**
  %54 = load %struct.list_t*, %struct.list_t** %53, align 8
  %55 = getelementptr inbounds %struct.list_t, %struct.list_t* %54, i32 0, i32 1
  %56 = load %struct.node_t*, %struct.node_t** %55, align 8
  %57 = getelementptr inbounds %struct.node_t, %struct.node_t* %56, i32 0, i32 0
  %58 = load i32, i32* %57, align 8
  %59 = icmp eq i32 %58, 0
  br i1 %59, label %60, label %109

; <label>:60:                                     ; preds = %50
  %61 = load %struct.node_t*, %struct.node_t** %6, align 8
  %62 = getelementptr inbounds %struct.node_t, %struct.node_t* %61, i32 0, i32 1
  %63 = bitcast %union.anon.11* %62 to %struct.list_t**
  %64 = load %struct.list_t*, %struct.list_t** %63, align 8
  %65 = getelementptr inbounds %struct.list_t, %struct.list_t* %64, i32 0, i32 1
  %66 = load %struct.node_t*, %struct.node_t** %65, align 8
  %67 = getelementptr inbounds %struct.node_t, %struct.node_t* %66, i32 0, i32 1
  %68 = bitcast %union.anon.11* %67 to %struct.atom_t**
  %69 = load %struct.atom_t*, %struct.atom_t** %68, align 8
  %70 = getelementptr inbounds %struct.atom_t, %struct.atom_t* %69, i32 0, i32 0
  %71 = load i8*, i8** %70, align 8
  store i8* %71, i8** %7, align 8
  %72 = load i8*, i8** %7, align 8
  %73 = call i32 @strcmp(i8* %72, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @.str.42, i32 0, i32 0))
  %74 = icmp eq i32 %73, 0
  br i1 %74, label %75, label %82

; <label>:75:                                     ; preds = %60
  %76 = load %struct.module_t*, %struct.module_t** %4, align 8
  %77 = getelementptr inbounds %struct.module_t, %struct.module_t* %76, i32 0, i32 1
  %78 = load i32, i32* %77, align 8
  %79 = add nsw i32 %78, 1
  %80 = load %struct.module_t*, %struct.module_t** %4, align 8
  %81 = getelementptr inbounds %struct.module_t, %struct.module_t* %80, i32 0, i32 1
  store i32 %79, i32* %81, align 8
  br label %108

; <label>:82:                                     ; preds = %60
  %83 = load i8*, i8** %7, align 8
  %84 = call i32 @strcmp(i8* %83, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @.str.43, i32 0, i32 0))
  %85 = icmp eq i32 %84, 0
  br i1 %85, label %86, label %93

; <label>:86:                                     ; preds = %82
  %87 = load %struct.module_t*, %struct.module_t** %4, align 8
  %88 = getelementptr inbounds %struct.module_t, %struct.module_t* %87, i32 0, i32 3
  %89 = load i32, i32* %88, align 8
  %90 = add nsw i32 %89, 1
  %91 = load %struct.module_t*, %struct.module_t** %4, align 8
  %92 = getelementptr inbounds %struct.module_t, %struct.module_t* %91, i32 0, i32 3
  store i32 %90, i32* %92, align 8
  br label %107

; <label>:93:                                     ; preds = %82
  %94 = load i8*, i8** %7, align 8
  %95 = call i32 @strcmp(i8* %94, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.44, i32 0, i32 0))
  %96 = icmp eq i32 %95, 0
  br i1 %96, label %97, label %106

; <label>:97:                                     ; preds = %93
  %98 = load %struct.module_t*, %struct.module_t** %4, align 8
  %99 = getelementptr inbounds %struct.module_t, %struct.module_t* %98, i32 0, i32 5
  %100 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %99, i32 0, i32 1
  %101 = load i32, i32* %100, align 8
  %102 = add nsw i32 %101, 1
  %103 = load %struct.module_t*, %struct.module_t** %4, align 8
  %104 = getelementptr inbounds %struct.module_t, %struct.module_t* %103, i32 0, i32 5
  %105 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %104, i32 0, i32 1
  store i32 %102, i32* %105, align 8
  br label %106

; <label>:106:                                    ; preds = %97, %93
  br label %107

; <label>:107:                                    ; preds = %106, %86
  br label %108

; <label>:108:                                    ; preds = %107, %75
  br label %109

; <label>:109:                                    ; preds = %108, %50, %45
  %110 = load %struct.node_t*, %struct.node_t** %6, align 8
  %111 = getelementptr inbounds %struct.node_t, %struct.node_t* %110, i32 0, i32 2
  %112 = load %struct.node_t*, %struct.node_t** %111, align 8
  store %struct.node_t* %112, %struct.node_t** %6, align 8
  br label %113

; <label>:113:                                    ; preds = %109
  %114 = load i32, i32* %8, align 4
  %115 = add nsw i32 %114, 1
  store i32 %115, i32* %8, align 4
  br label %36

; <label>:116:                                    ; preds = %36
  %117 = load %struct.module_t*, %struct.module_t** %4, align 8
  %118 = getelementptr inbounds %struct.module_t, %struct.module_t* %117, i32 0, i32 1
  %119 = load i32, i32* %118, align 8
  %120 = sext i32 %119 to i64
  %121 = mul i64 %120, 8
  %122 = call i8* @malloc(i64 %121) #8
  %123 = bitcast i8* %122 to %struct.module_t**
  %124 = load %struct.module_t*, %struct.module_t** %4, align 8
  %125 = getelementptr inbounds %struct.module_t, %struct.module_t* %124, i32 0, i32 2
  store %struct.module_t** %123, %struct.module_t*** %125, align 8
  %126 = load %struct.module_t*, %struct.module_t** %4, align 8
  %127 = getelementptr inbounds %struct.module_t, %struct.module_t* %126, i32 0, i32 2
  %128 = load %struct.module_t**, %struct.module_t*** %127, align 8
  %129 = getelementptr inbounds %struct.module_t*, %struct.module_t** %128, i64 0
  store %struct.module_t* @MODULE_BUILTIN, %struct.module_t** %129, align 8
  %130 = load %struct.module_t*, %struct.module_t** %4, align 8
  %131 = getelementptr inbounds %struct.module_t, %struct.module_t* %130, i32 0, i32 3
  %132 = load i32, i32* %131, align 8
  %133 = sext i32 %132 to i64
  %134 = mul i64 %133, 48
  %135 = call i8* @malloc(i64 %134) #8
  %136 = bitcast i8* %135 to %struct.type_t*
  %137 = load %struct.module_t*, %struct.module_t** %4, align 8
  %138 = getelementptr inbounds %struct.module_t, %struct.module_t* %137, i32 0, i32 4
  store %struct.type_t* %136, %struct.type_t** %138, align 8
  %139 = load %struct.module_t*, %struct.module_t** %4, align 8
  %140 = getelementptr inbounds %struct.module_t, %struct.module_t* %139, i32 0, i32 5
  %141 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %140, i32 0, i32 1
  %142 = load i32, i32* %141, align 8
  %143 = load %struct.module_t*, %struct.module_t** %4, align 8
  %144 = getelementptr inbounds %struct.module_t, %struct.module_t* %143, i32 0, i32 5
  %145 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %144, i32 0, i32 2
  store i32 %142, i32* %145, align 4
  %146 = load %struct.module_t*, %struct.module_t** %4, align 8
  %147 = getelementptr inbounds %struct.module_t, %struct.module_t* %146, i32 0, i32 5
  %148 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %147, i32 0, i32 2
  %149 = load i32, i32* %148, align 4
  %150 = sext i32 %149 to i64
  %151 = mul i64 %150, 8
  %152 = call i8* @malloc(i64 %151) #8
  %153 = bitcast i8* %152 to i8**
  %154 = load %struct.module_t*, %struct.module_t** %4, align 8
  %155 = getelementptr inbounds %struct.module_t, %struct.module_t* %154, i32 0, i32 5
  %156 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %155, i32 0, i32 3
  store i8** %153, i8*** %156, align 8
  %157 = load %struct.module_t*, %struct.module_t** %4, align 8
  %158 = getelementptr inbounds %struct.module_t, %struct.module_t* %157, i32 0, i32 5
  %159 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %158, i32 0, i32 2
  %160 = load i32, i32* %159, align 4
  %161 = sext i32 %160 to i64
  %162 = mul i64 %161, 32
  %163 = call i8* @malloc(i64 %162) #8
  %164 = bitcast i8* %163 to %struct.val_t*
  %165 = load %struct.module_t*, %struct.module_t** %4, align 8
  %166 = getelementptr inbounds %struct.module_t, %struct.module_t* %165, i32 0, i32 5
  %167 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %166, i32 0, i32 4
  store %struct.val_t* %164, %struct.val_t** %167, align 8
  store i32 0, i32* %3, align 4
  br label %168

; <label>:168:                                    ; preds = %116, %26
  %169 = load i32, i32* %3, align 4
  ret i32 %169
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @module_parse_node(%struct.node_t*, %struct.module_t*) #0 {
  %3 = alloca i32, align 4
  %4 = alloca %struct.node_t*, align 8
  %5 = alloca %struct.module_t*, align 8
  %6 = alloca %struct.node_t*, align 8
  %7 = alloca %struct.module_t**, align 8
  %8 = alloca %struct.val_t*, align 8
  %9 = alloca i32, align 4
  %10 = alloca i32, align 4
  %11 = alloca i8*, align 8
  %12 = alloca i32, align 4
  %13 = alloca %struct.node_t*, align 8
  %14 = alloca i32, align 4
  %15 = alloca i32, align 4
  store %struct.node_t* %0, %struct.node_t** %4, align 8
  store %struct.module_t* %1, %struct.module_t** %5, align 8
  %16 = load %struct.module_t*, %struct.module_t** %5, align 8
  %17 = load %struct.node_t*, %struct.node_t** %4, align 8
  %18 = call i32 @module_setup(%struct.module_t* %16, %struct.node_t* %17)
  %19 = icmp ne i32 %18, 0
  br i1 %19, label %20, label %21

; <label>:20:                                     ; preds = %2
  store i32 1, i32* %3, align 4
  br label %330

; <label>:21:                                     ; preds = %2
  %22 = load %struct.node_t*, %struct.node_t** %4, align 8
  %23 = getelementptr inbounds %struct.node_t, %struct.node_t* %22, i32 0, i32 1
  %24 = bitcast %union.anon.11* %23 to %struct.list_t**
  %25 = load %struct.list_t*, %struct.list_t** %24, align 8
  %26 = getelementptr inbounds %struct.list_t, %struct.list_t* %25, i32 0, i32 1
  %27 = load %struct.node_t*, %struct.node_t** %26, align 8
  store %struct.node_t* %27, %struct.node_t** %6, align 8
  %28 = load %struct.module_t*, %struct.module_t** %5, align 8
  %29 = getelementptr inbounds %struct.module_t, %struct.module_t* %28, i32 0, i32 2
  %30 = load %struct.module_t**, %struct.module_t*** %29, align 8
  %31 = getelementptr inbounds %struct.module_t*, %struct.module_t** %30, i64 1
  store %struct.module_t** %31, %struct.module_t*** %7, align 8
  %32 = load %struct.module_t*, %struct.module_t** %5, align 8
  %33 = getelementptr inbounds %struct.module_t, %struct.module_t* %32, i32 0, i32 5
  %34 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %33, i32 0, i32 4
  %35 = load %struct.val_t*, %struct.val_t** %34, align 8
  store %struct.val_t* %35, %struct.val_t** %8, align 8
  store i32 0, i32* %9, align 4
  store i32 0, i32* %10, align 4
  store i32 0, i32* %12, align 4
  br label %36

; <label>:36:                                     ; preds = %249, %21
  %37 = load i32, i32* %12, align 4
  %38 = load %struct.node_t*, %struct.node_t** %4, align 8
  %39 = getelementptr inbounds %struct.node_t, %struct.node_t* %38, i32 0, i32 1
  %40 = bitcast %union.anon.11* %39 to %struct.list_t**
  %41 = load %struct.list_t*, %struct.list_t** %40, align 8
  %42 = getelementptr inbounds %struct.list_t, %struct.list_t* %41, i32 0, i32 0
  %43 = load i32, i32* %42, align 8
  %44 = icmp slt i32 %37, %43
  br i1 %44, label %45, label %252

; <label>:45:                                     ; preds = %36
  %46 = load %struct.node_t*, %struct.node_t** %6, align 8
  %47 = getelementptr inbounds %struct.node_t, %struct.node_t* %46, i32 0, i32 0
  %48 = load i32, i32* %47, align 8
  %49 = icmp ne i32 %48, 1
  br i1 %49, label %50, label %53

; <label>:50:                                     ; preds = %45
  %51 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %52 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %51, i8* getelementptr inbounds ([30 x i8], [30 x i8]* @.str.45, i32 0, i32 0))
  store i32 1, i32* %3, align 4
  br label %330

; <label>:53:                                     ; preds = %45
  %54 = load %struct.node_t*, %struct.node_t** %6, align 8
  %55 = getelementptr inbounds %struct.node_t, %struct.node_t* %54, i32 0, i32 1
  %56 = bitcast %union.anon.11* %55 to %struct.list_t**
  %57 = load %struct.list_t*, %struct.list_t** %56, align 8
  %58 = getelementptr inbounds %struct.list_t, %struct.list_t* %57, i32 0, i32 1
  %59 = load %struct.node_t*, %struct.node_t** %58, align 8
  %60 = getelementptr inbounds %struct.node_t, %struct.node_t* %59, i32 0, i32 0
  %61 = load i32, i32* %60, align 8
  %62 = icmp eq i32 %61, 0
  br i1 %62, label %63, label %245

; <label>:63:                                     ; preds = %53
  %64 = load %struct.node_t*, %struct.node_t** %6, align 8
  %65 = getelementptr inbounds %struct.node_t, %struct.node_t* %64, i32 0, i32 1
  %66 = bitcast %union.anon.11* %65 to %struct.list_t**
  %67 = load %struct.list_t*, %struct.list_t** %66, align 8
  %68 = getelementptr inbounds %struct.list_t, %struct.list_t* %67, i32 0, i32 1
  %69 = load %struct.node_t*, %struct.node_t** %68, align 8
  %70 = getelementptr inbounds %struct.node_t, %struct.node_t* %69, i32 0, i32 1
  %71 = bitcast %union.anon.11* %70 to %struct.atom_t**
  %72 = load %struct.atom_t*, %struct.atom_t** %71, align 8
  %73 = getelementptr inbounds %struct.atom_t, %struct.atom_t* %72, i32 0, i32 0
  %74 = load i8*, i8** %73, align 8
  store i8* %74, i8** %11, align 8
  %75 = load i8*, i8** %11, align 8
  %76 = call i32 @strcmp(i8* %75, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @.str.42, i32 0, i32 0))
  %77 = icmp eq i32 %76, 0
  br i1 %77, label %78, label %101

; <label>:78:                                     ; preds = %63
  %79 = load %struct.node_t*, %struct.node_t** %6, align 8
  %80 = getelementptr inbounds %struct.node_t, %struct.node_t* %79, i32 0, i32 1
  %81 = bitcast %union.anon.11* %80 to %struct.list_t**
  %82 = load %struct.list_t*, %struct.list_t** %81, align 8
  %83 = getelementptr inbounds %struct.list_t, %struct.list_t* %82, i32 0, i32 1
  %84 = load %struct.node_t*, %struct.node_t** %83, align 8
  %85 = getelementptr inbounds %struct.node_t, %struct.node_t* %84, i32 0, i32 2
  %86 = load %struct.node_t*, %struct.node_t** %85, align 8
  %87 = getelementptr inbounds %struct.node_t, %struct.node_t* %86, i32 0, i32 1
  %88 = bitcast %union.anon.11* %87 to %struct.atom_t**
  %89 = load %struct.atom_t*, %struct.atom_t** %88, align 8
  %90 = getelementptr inbounds %struct.atom_t, %struct.atom_t* %89, i32 0, i32 0
  %91 = load i8*, i8** %90, align 8
  %92 = call %struct.module_t* @module_load(i8* %91)
  %93 = load %struct.module_t**, %struct.module_t*** %7, align 8
  store %struct.module_t* %92, %struct.module_t** %93, align 8
  %94 = load %struct.module_t**, %struct.module_t*** %7, align 8
  %95 = load %struct.module_t*, %struct.module_t** %94, align 8
  %96 = icmp eq %struct.module_t* %95, null
  br i1 %96, label %97, label %98

; <label>:97:                                     ; preds = %78
  store i32 1, i32* %3, align 4
  br label %330

; <label>:98:                                     ; preds = %78
  %99 = load %struct.module_t**, %struct.module_t*** %7, align 8
  %100 = getelementptr inbounds %struct.module_t*, %struct.module_t** %99, i32 1
  store %struct.module_t** %100, %struct.module_t*** %7, align 8
  br label %244

; <label>:101:                                    ; preds = %63
  %102 = load i8*, i8** %11, align 8
  %103 = call i32 @strcmp(i8* %102, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @.str.43, i32 0, i32 0))
  %104 = icmp eq i32 %103, 0
  br i1 %104, label %105, label %178

; <label>:105:                                    ; preds = %101
  %106 = load %struct.node_t*, %struct.node_t** %6, align 8
  %107 = getelementptr inbounds %struct.node_t, %struct.node_t* %106, i32 0, i32 1
  %108 = bitcast %union.anon.11* %107 to %struct.list_t**
  %109 = load %struct.list_t*, %struct.list_t** %108, align 8
  %110 = getelementptr inbounds %struct.list_t, %struct.list_t* %109, i32 0, i32 1
  %111 = load %struct.node_t*, %struct.node_t** %110, align 8
  %112 = getelementptr inbounds %struct.node_t, %struct.node_t* %111, i32 0, i32 2
  %113 = load %struct.node_t*, %struct.node_t** %112, align 8
  store %struct.node_t* %113, %struct.node_t** %13, align 8
  %114 = load %struct.node_t*, %struct.node_t** %13, align 8
  %115 = getelementptr inbounds %struct.node_t, %struct.node_t* %114, i32 0, i32 0
  %116 = load i32, i32* %115, align 8
  %117 = icmp ne i32 %116, 0
  br i1 %117, label %118, label %131

; <label>:118:                                    ; preds = %105
  %119 = load %struct.node_t*, %struct.node_t** %13, align 8
  %120 = getelementptr inbounds %struct.node_t, %struct.node_t* %119, i32 0, i32 1
  %121 = bitcast %union.anon.11* %120 to %struct.list_t**
  %122 = load %struct.list_t*, %struct.list_t** %121, align 8
  %123 = getelementptr inbounds %struct.list_t, %struct.list_t* %122, i32 0, i32 1
  %124 = load %struct.node_t*, %struct.node_t** %123, align 8
  %125 = getelementptr inbounds %struct.node_t, %struct.node_t* %124, i32 0, i32 0
  %126 = load i32, i32* %125, align 8
  %127 = icmp ne i32 %126, 0
  br i1 %127, label %128, label %131

; <label>:128:                                    ; preds = %118
  %129 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %130 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %129, i8* getelementptr inbounds ([19 x i8], [19 x i8]* @.str.46, i32 0, i32 0))
  store i32 1, i32* %3, align 4
  br label %330

; <label>:131:                                    ; preds = %118, %105
  %132 = load %struct.node_t*, %struct.node_t** %13, align 8
  %133 = getelementptr inbounds %struct.node_t, %struct.node_t* %132, i32 0, i32 0
  %134 = load i32, i32* %133, align 8
  %135 = icmp eq i32 %134, 0
  br i1 %135, label %136, label %143

; <label>:136:                                    ; preds = %131
  %137 = load %struct.node_t*, %struct.node_t** %13, align 8
  %138 = getelementptr inbounds %struct.node_t, %struct.node_t* %137, i32 0, i32 1
  %139 = bitcast %union.anon.11* %138 to %struct.atom_t**
  %140 = load %struct.atom_t*, %struct.atom_t** %139, align 8
  %141 = getelementptr inbounds %struct.atom_t, %struct.atom_t* %140, i32 0, i32 0
  %142 = load i8*, i8** %141, align 8
  br label %155

; <label>:143:                                    ; preds = %131
  %144 = load %struct.node_t*, %struct.node_t** %13, align 8
  %145 = getelementptr inbounds %struct.node_t, %struct.node_t* %144, i32 0, i32 1
  %146 = bitcast %union.anon.11* %145 to %struct.list_t**
  %147 = load %struct.list_t*, %struct.list_t** %146, align 8
  %148 = getelementptr inbounds %struct.list_t, %struct.list_t* %147, i32 0, i32 1
  %149 = load %struct.node_t*, %struct.node_t** %148, align 8
  %150 = getelementptr inbounds %struct.node_t, %struct.node_t* %149, i32 0, i32 1
  %151 = bitcast %union.anon.11* %150 to %struct.atom_t**
  %152 = load %struct.atom_t*, %struct.atom_t** %151, align 8
  %153 = getelementptr inbounds %struct.atom_t, %struct.atom_t* %152, i32 0, i32 0
  %154 = load i8*, i8** %153, align 8
  br label %155

; <label>:155:                                    ; preds = %143, %136
  %156 = phi i8* [ %142, %136 ], [ %154, %143 ]
  %157 = load %struct.module_t*, %struct.module_t** %5, align 8
  %158 = getelementptr inbounds %struct.module_t, %struct.module_t* %157, i32 0, i32 4
  %159 = load %struct.type_t*, %struct.type_t** %158, align 8
  %160 = load i32, i32* %10, align 4
  %161 = sext i32 %160 to i64
  %162 = getelementptr inbounds %struct.type_t, %struct.type_t* %159, i64 %161
  %163 = getelementptr inbounds %struct.type_t, %struct.type_t* %162, i32 0, i32 0
  store i8* %156, i8** %163, align 8
  %164 = load %struct.module_t*, %struct.module_t** %5, align 8
  %165 = load %struct.node_t*, %struct.node_t** %13, align 8
  %166 = load %struct.module_t*, %struct.module_t** %5, align 8
  %167 = getelementptr inbounds %struct.module_t, %struct.module_t* %166, i32 0, i32 4
  %168 = load %struct.type_t*, %struct.type_t** %167, align 8
  %169 = load i32, i32* %10, align 4
  %170 = sext i32 %169 to i64
  %171 = getelementptr inbounds %struct.type_t, %struct.type_t* %168, i64 %170
  %172 = call i32 @parse_type(%struct.module_t* %164, %struct.node_t* %165, %struct.type_t* %171)
  %173 = icmp ne i32 %172, 0
  br i1 %173, label %174, label %175

; <label>:174:                                    ; preds = %155
  store i32 1, i32* %3, align 4
  br label %330

; <label>:175:                                    ; preds = %155
  %176 = load i32, i32* %10, align 4
  %177 = add nsw i32 %176, 1
  store i32 %177, i32* %10, align 4
  br label %243

; <label>:178:                                    ; preds = %101
  %179 = load i8*, i8** %11, align 8
  %180 = call i32 @strcmp(i8* %179, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.44, i32 0, i32 0))
  %181 = icmp eq i32 %180, 0
  br i1 %181, label %182, label %242

; <label>:182:                                    ; preds = %178
  %183 = load %struct.node_t*, %struct.node_t** %6, align 8
  %184 = getelementptr inbounds %struct.node_t, %struct.node_t* %183, i32 0, i32 1
  %185 = bitcast %union.anon.11* %184 to %struct.list_t**
  %186 = load %struct.list_t*, %struct.list_t** %185, align 8
  %187 = getelementptr inbounds %struct.list_t, %struct.list_t* %186, i32 0, i32 1
  %188 = load %struct.node_t*, %struct.node_t** %187, align 8
  %189 = getelementptr inbounds %struct.node_t, %struct.node_t* %188, i32 0, i32 2
  %190 = load %struct.node_t*, %struct.node_t** %189, align 8
  %191 = getelementptr inbounds %struct.node_t, %struct.node_t* %190, i32 0, i32 1
  %192 = bitcast %union.anon.11* %191 to %struct.atom_t**
  %193 = load %struct.atom_t*, %struct.atom_t** %192, align 8
  %194 = getelementptr inbounds %struct.atom_t, %struct.atom_t* %193, i32 0, i32 0
  %195 = load i8*, i8** %194, align 8
  %196 = load %struct.module_t*, %struct.module_t** %5, align 8
  %197 = getelementptr inbounds %struct.module_t, %struct.module_t* %196, i32 0, i32 5
  %198 = getelementptr inbounds %struct.symbol_table_t, %struct.symbol_table_t* %197, i32 0, i32 3
  %199 = load i8**, i8*** %198, align 8
  %200 = load i32, i32* %9, align 4
  %201 = sext i32 %200 to i64
  %202 = getelementptr inbounds i8*, i8** %199, i64 %201
  store i8* %195, i8** %202, align 8
  %203 = call %struct.type_t* (...) @type_new()
  %204 = load %struct.val_t*, %struct.val_t** %8, align 8
  %205 = getelementptr inbounds %struct.val_t, %struct.val_t* %204, i32 0, i32 0
  store %struct.type_t* %203, %struct.type_t** %205, align 8
  %206 = load %struct.val_t*, %struct.val_t** %8, align 8
  %207 = getelementptr inbounds %struct.val_t, %struct.val_t* %206, i32 0, i32 0
  %208 = load %struct.type_t*, %struct.type_t** %207, align 8
  %209 = getelementptr inbounds %struct.type_t, %struct.type_t* %208, i32 0, i32 0
  store i8* null, i8** %209, align 8
  %210 = load %struct.module_t*, %struct.module_t** %5, align 8
  %211 = load %struct.node_t*, %struct.node_t** %6, align 8
  %212 = getelementptr inbounds %struct.node_t, %struct.node_t* %211, i32 0, i32 1
  %213 = bitcast %union.anon.11* %212 to %struct.list_t**
  %214 = load %struct.list_t*, %struct.list_t** %213, align 8
  %215 = getelementptr inbounds %struct.list_t, %struct.list_t* %214, i32 0, i32 1
  %216 = load %struct.node_t*, %struct.node_t** %215, align 8
  %217 = getelementptr inbounds %struct.node_t, %struct.node_t* %216, i32 0, i32 2
  %218 = load %struct.node_t*, %struct.node_t** %217, align 8
  %219 = load %struct.val_t*, %struct.val_t** %8, align 8
  %220 = getelementptr inbounds %struct.val_t, %struct.val_t* %219, i32 0, i32 0
  %221 = load %struct.type_t*, %struct.type_t** %220, align 8
  %222 = call i32 @parse_type(%struct.module_t* %210, %struct.node_t* %218, %struct.type_t* %221)
  %223 = icmp ne i32 %222, 0
  br i1 %223, label %224, label %225

; <label>:224:                                    ; preds = %182
  store i32 1, i32* %3, align 4
  br label %330

; <label>:225:                                    ; preds = %182
  %226 = load %struct.val_t*, %struct.val_t** %8, align 8
  %227 = getelementptr inbounds %struct.val_t, %struct.val_t* %226, i32 0, i32 0
  %228 = load %struct.type_t*, %struct.type_t** %227, align 8
  %229 = getelementptr inbounds %struct.type_t, %struct.type_t* %228, i32 0, i32 1
  %230 = load i32, i32* %229, align 8
  %231 = icmp eq i32 %230, 2
  br i1 %231, label %232, label %237

; <label>:232:                                    ; preds = %225
  %233 = load %struct.val_t*, %struct.val_t** %8, align 8
  %234 = getelementptr inbounds %struct.val_t, %struct.val_t* %233, i32 0, i32 1
  %235 = bitcast %union.anon* %234 to %struct.anon.0*
  %236 = getelementptr inbounds %struct.anon.0, %struct.anon.0* %235, i32 0, i32 0
  store %struct.expr_t* null, %struct.expr_t** %236, align 8
  br label %237

; <label>:237:                                    ; preds = %232, %225
  %238 = load %struct.val_t*, %struct.val_t** %8, align 8
  %239 = getelementptr inbounds %struct.val_t, %struct.val_t* %238, i32 1
  store %struct.val_t* %239, %struct.val_t** %8, align 8
  %240 = load i32, i32* %9, align 4
  %241 = add nsw i32 %240, 1
  store i32 %241, i32* %9, align 4
  br label %242

; <label>:242:                                    ; preds = %237, %178
  br label %243

; <label>:243:                                    ; preds = %242, %175
  br label %244

; <label>:244:                                    ; preds = %243, %98
  br label %245

; <label>:245:                                    ; preds = %244, %53
  %246 = load %struct.node_t*, %struct.node_t** %6, align 8
  %247 = getelementptr inbounds %struct.node_t, %struct.node_t* %246, i32 0, i32 2
  %248 = load %struct.node_t*, %struct.node_t** %247, align 8
  store %struct.node_t* %248, %struct.node_t** %6, align 8
  br label %249

; <label>:249:                                    ; preds = %245
  %250 = load i32, i32* %12, align 4
  %251 = add nsw i32 %250, 1
  store i32 %251, i32* %12, align 4
  br label %36

; <label>:252:                                    ; preds = %36
  store i32 0, i32* %14, align 4
  br label %253

; <label>:253:                                    ; preds = %268, %252
  %254 = load i32, i32* %14, align 4
  %255 = load %struct.module_t*, %struct.module_t** %5, align 8
  %256 = getelementptr inbounds %struct.module_t, %struct.module_t* %255, i32 0, i32 3
  %257 = load i32, i32* %256, align 8
  %258 = icmp slt i32 %254, %257
  br i1 %258, label %259, label %271

; <label>:259:                                    ; preds = %253
  %260 = load %struct.module_t*, %struct.module_t** %5, align 8
  %261 = load %struct.module_t*, %struct.module_t** %5, align 8
  %262 = getelementptr inbounds %struct.module_t, %struct.module_t* %261, i32 0, i32 4
  %263 = load %struct.type_t*, %struct.type_t** %262, align 8
  %264 = load i32, i32* %14, align 4
  %265 = sext i32 %264 to i64
  %266 = getelementptr inbounds %struct.type_t, %struct.type_t* %263, i64 %265
  %267 = call i32 @type_add_constructors(%struct.module_t* %260, %struct.type_t* %266)
  br label %268

; <label>:268:                                    ; preds = %259
  %269 = load i32, i32* %14, align 4
  %270 = add nsw i32 %269, 1
  store i32 %270, i32* %14, align 4
  br label %253

; <label>:271:                                    ; preds = %253
  %272 = load %struct.node_t*, %struct.node_t** %4, align 8
  %273 = getelementptr inbounds %struct.node_t, %struct.node_t* %272, i32 0, i32 1
  %274 = bitcast %union.anon.11* %273 to %struct.list_t**
  %275 = load %struct.list_t*, %struct.list_t** %274, align 8
  %276 = getelementptr inbounds %struct.list_t, %struct.list_t* %275, i32 0, i32 1
  %277 = load %struct.node_t*, %struct.node_t** %276, align 8
  store %struct.node_t* %277, %struct.node_t** %6, align 8
  store i32 0, i32* %15, align 4
  br label %278

; <label>:278:                                    ; preds = %326, %271
  %279 = load i32, i32* %15, align 4
  %280 = load %struct.node_t*, %struct.node_t** %4, align 8
  %281 = getelementptr inbounds %struct.node_t, %struct.node_t* %280, i32 0, i32 1
  %282 = bitcast %union.anon.11* %281 to %struct.list_t**
  %283 = load %struct.list_t*, %struct.list_t** %282, align 8
  %284 = getelementptr inbounds %struct.list_t, %struct.list_t* %283, i32 0, i32 0
  %285 = load i32, i32* %284, align 8
  %286 = icmp slt i32 %279, %285
  br i1 %286, label %287, label %329

; <label>:287:                                    ; preds = %278
  %288 = load %struct.node_t*, %struct.node_t** %6, align 8
  %289 = getelementptr inbounds %struct.node_t, %struct.node_t* %288, i32 0, i32 1
  %290 = bitcast %union.anon.11* %289 to %struct.list_t**
  %291 = load %struct.list_t*, %struct.list_t** %290, align 8
  %292 = getelementptr inbounds %struct.list_t, %struct.list_t* %291, i32 0, i32 1
  %293 = load %struct.node_t*, %struct.node_t** %292, align 8
  %294 = getelementptr inbounds %struct.node_t, %struct.node_t* %293, i32 0, i32 0
  %295 = load i32, i32* %294, align 8
  %296 = icmp eq i32 %295, 0
  br i1 %296, label %297, label %322

; <label>:297:                                    ; preds = %287
  %298 = load %struct.node_t*, %struct.node_t** %6, align 8
  %299 = getelementptr inbounds %struct.node_t, %struct.node_t* %298, i32 0, i32 1
  %300 = bitcast %union.anon.11* %299 to %struct.list_t**
  %301 = load %struct.list_t*, %struct.list_t** %300, align 8
  %302 = getelementptr inbounds %struct.list_t, %struct.list_t* %301, i32 0, i32 1
  %303 = load %struct.node_t*, %struct.node_t** %302, align 8
  %304 = getelementptr inbounds %struct.node_t, %struct.node_t* %303, i32 0, i32 1
  %305 = bitcast %union.anon.11* %304 to %struct.atom_t**
  %306 = load %struct.atom_t*, %struct.atom_t** %305, align 8
  %307 = getelementptr inbounds %struct.atom_t, %struct.atom_t* %306, i32 0, i32 0
  %308 = load i8*, i8** %307, align 8
  store i8* %308, i8** %11, align 8
  %309 = load i8*, i8** %11, align 8
  %310 = call i32 @strcmp(i8* %309, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.47, i32 0, i32 0))
  %311 = icmp eq i32 %310, 0
  br i1 %311, label %312, label %321

; <label>:312:                                    ; preds = %297
  %313 = load %struct.module_t*, %struct.module_t** %5, align 8
  %314 = load %struct.module_t*, %struct.module_t** %5, align 8
  %315 = getelementptr inbounds %struct.module_t, %struct.module_t* %314, i32 0, i32 5
  %316 = load %struct.node_t*, %struct.node_t** %6, align 8
  %317 = call i32 @parse_defun(%struct.module_t* %313, %struct.symbol_table_t* %315, %struct.node_t* %316)
  %318 = icmp ne i32 %317, 0
  br i1 %318, label %319, label %320

; <label>:319:                                    ; preds = %312
  store i32 1, i32* %3, align 4
  br label %330

; <label>:320:                                    ; preds = %312
  br label %321

; <label>:321:                                    ; preds = %320, %297
  br label %322

; <label>:322:                                    ; preds = %321, %287
  %323 = load %struct.node_t*, %struct.node_t** %6, align 8
  %324 = getelementptr inbounds %struct.node_t, %struct.node_t* %323, i32 0, i32 2
  %325 = load %struct.node_t*, %struct.node_t** %324, align 8
  store %struct.node_t* %325, %struct.node_t** %6, align 8
  br label %326

; <label>:326:                                    ; preds = %322
  %327 = load i32, i32* %15, align 4
  %328 = add nsw i32 %327, 1
  store i32 %328, i32* %15, align 4
  br label %278

; <label>:329:                                    ; preds = %278
  store i32 0, i32* %3, align 4
  br label %330

; <label>:330:                                    ; preds = %329, %319, %224, %174, %128, %97, %50, %20
  %331 = load i32, i32* %3, align 4
  ret i32 %331
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define %struct.module_t* @module_load(i8*) #0 {
  %2 = alloca %struct.module_t*, align 8
  %3 = alloca i8*, align 8
  %4 = alloca [2048 x i8], align 16
  %5 = alloca %struct.module_t*, align 8
  store i8* %0, i8** %3, align 8
  %6 = bitcast [2048 x i8]* %4 to i8*
  call void @llvm.memset.p0i8.i64(i8* %6, i8 0, i64 2048, i32 16, i1 false)
  %7 = load i8*, i8** %3, align 8
  %8 = getelementptr inbounds i8, i8* %7, i64 0
  %9 = load i8, i8* %8, align 1
  %10 = sext i8 %9 to i32
  switch i32 %10, label %28 [
    i32 47, label %11
    i32 46, label %17
  ]

; <label>:11:                                     ; preds = %1
  %12 = getelementptr inbounds [2048 x i8], [2048 x i8]* %4, i32 0, i32 0
  %13 = load i8*, i8** %3, align 8
  %14 = load i8*, i8** %3, align 8
  %15 = call i64 @strlen(i8* %14)
  %16 = call i8* @__strncpy_chk(i8* %12, i8* %13, i64 %15, i64 2048) #9
  br label %39

; <label>:17:                                     ; preds = %1
  %18 = getelementptr inbounds [2048 x i8], [2048 x i8]* %4, i32 0, i32 0
  %19 = call i64 @strlen(i8* getelementptr inbounds ([1024 x i8], [1024 x i8]* @cwd, i32 0, i32 0))
  %20 = call i8* @__strncpy_chk(i8* %18, i8* getelementptr inbounds ([1024 x i8], [1024 x i8]* @cwd, i32 0, i32 0), i64 %19, i64 2048) #9
  %21 = getelementptr inbounds [2048 x i8], [2048 x i8]* %4, i32 0, i32 0
  %22 = load i8*, i8** %3, align 8
  %23 = getelementptr inbounds i8, i8* %22, i64 1
  %24 = load i8*, i8** %3, align 8
  %25 = call i64 @strlen(i8* %24)
  %26 = sub i64 %25, 1
  %27 = call i8* @__strncat_chk(i8* %21, i8* %23, i64 %26, i64 2048) #9
  br label %39

; <label>:28:                                     ; preds = %1
  %29 = getelementptr inbounds [2048 x i8], [2048 x i8]* %4, i32 0, i32 0
  %30 = load i8*, i8** @stdlib, align 8
  %31 = load i8*, i8** @stdlib, align 8
  %32 = call i64 @strlen(i8* %31)
  %33 = call i8* @__strncat_chk(i8* %29, i8* %30, i64 %32, i64 2048) #9
  %34 = getelementptr inbounds [2048 x i8], [2048 x i8]* %4, i32 0, i32 0
  %35 = load i8*, i8** %3, align 8
  %36 = load i8*, i8** %3, align 8
  %37 = call i64 @strlen(i8* %36)
  %38 = call i8* @__strncat_chk(i8* %34, i8* %35, i64 %37, i64 2048) #9
  br label %39

; <label>:39:                                     ; preds = %28, %17, %11
  %40 = getelementptr inbounds [2048 x i8], [2048 x i8]* %4, i32 0, i32 0
  %41 = call %struct.module_t* @module_cache_find(i8* %40)
  store %struct.module_t* %41, %struct.module_t** %5, align 8
  %42 = load %struct.module_t*, %struct.module_t** %5, align 8
  %43 = icmp ne %struct.module_t* %42, null
  br i1 %43, label %44, label %46

; <label>:44:                                     ; preds = %39
  %45 = load %struct.module_t*, %struct.module_t** %5, align 8
  store %struct.module_t* %45, %struct.module_t** %2, align 8
  br label %72

; <label>:46:                                     ; preds = %39
  %47 = call %struct.module_t* @module_cache_next()
  store %struct.module_t* %47, %struct.module_t** %5, align 8
  %48 = getelementptr inbounds [2048 x i8], [2048 x i8]* %4, i32 0, i32 0
  %49 = call i64 @strlen(i8* %48)
  %50 = add i64 %49, 1
  %51 = call i8* @malloc(i64 %50) #8
  %52 = load %struct.module_t*, %struct.module_t** %5, align 8
  %53 = getelementptr inbounds %struct.module_t, %struct.module_t* %52, i32 0, i32 0
  store i8* %51, i8** %53, align 8
  %54 = load %struct.module_t*, %struct.module_t** %5, align 8
  %55 = getelementptr inbounds %struct.module_t, %struct.module_t* %54, i32 0, i32 0
  %56 = load i8*, i8** %55, align 8
  %57 = getelementptr inbounds [2048 x i8], [2048 x i8]* %4, i32 0, i32 0
  %58 = load %struct.module_t*, %struct.module_t** %5, align 8
  %59 = getelementptr inbounds %struct.module_t, %struct.module_t* %58, i32 0, i32 0
  %60 = load i8*, i8** %59, align 8
  %61 = call i64 @llvm.objectsize.i64.p0i8(i8* %60, i1 false, i1 true)
  %62 = call i8* @__strcpy_chk(i8* %56, i8* %57, i64 %61) #9
  %63 = getelementptr inbounds [2048 x i8], [2048 x i8]* %4, i32 0, i32 0
  %64 = call i8* @__strncat_chk(i8* %63, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.50, i32 0, i32 0), i64 5, i64 2048) #9
  %65 = getelementptr inbounds [2048 x i8], [2048 x i8]* %4, i32 0, i32 0
  %66 = load %struct.module_t*, %struct.module_t** %5, align 8
  %67 = call i32 @module_parse_file(i8* %65, %struct.module_t* %66)
  %68 = icmp ne i32 %67, 0
  br i1 %68, label %69, label %70

; <label>:69:                                     ; preds = %46
  store %struct.module_t* null, %struct.module_t** %2, align 8
  br label %72

; <label>:70:                                     ; preds = %46
  %71 = load %struct.module_t*, %struct.module_t** %5, align 8
  store %struct.module_t* %71, %struct.module_t** %2, align 8
  br label %72

; <label>:72:                                     ; preds = %70, %69, %44
  %73 = load %struct.module_t*, %struct.module_t** %2, align 8
  ret %struct.module_t* %73
}

declare i32 @parse_type(%struct.module_t*, %struct.node_t*, %struct.type_t*) #2

declare %struct.type_t* @type_new(...) #2

declare i32 @type_add_constructors(%struct.module_t*, %struct.type_t*) #2

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @module_parse_file(i8*, %struct.module_t*) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i8*, align 8
  %5 = alloca %struct.module_t*, align 8
  %6 = alloca %struct.node_t, align 8
  store i8* %0, i8** %4, align 8
  store %struct.module_t* %1, %struct.module_t** %5, align 8
  %7 = load i8*, i8** %4, align 8
  %8 = call i32 @node_from_file(i8* %7, %struct.node_t* %6)
  %9 = icmp ne i32 %8, 0
  br i1 %9, label %10, label %13

; <label>:10:                                     ; preds = %2
  %11 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %12 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %11, i8* getelementptr inbounds ([18 x i8], [18 x i8]* @.str.48, i32 0, i32 0))
  store i32 1, i32* %3, align 4
  br label %21

; <label>:13:                                     ; preds = %2
  %14 = load %struct.module_t*, %struct.module_t** %5, align 8
  %15 = call i32 @module_parse_node(%struct.node_t* %6, %struct.module_t* %14)
  %16 = icmp ne i32 %15, 0
  br i1 %16, label %17, label %20

; <label>:17:                                     ; preds = %13
  %18 = load %struct.__sFILE*, %struct.__sFILE** @__stderrp, align 8
  %19 = call i32 (%struct.__sFILE*, i8*, ...) @fprintf(%struct.__sFILE* %18, i8* getelementptr inbounds ([19 x i8], [19 x i8]* @.str.49, i32 0, i32 0))
  store i32 1, i32* %3, align 4
  br label %21

; <label>:20:                                     ; preds = %13
  store i32 0, i32* %3, align 4
  br label %21

; <label>:21:                                     ; preds = %20, %17, %10
  %22 = load i32, i32* %3, align 4
  ret i32 %22
}

declare i32 @node_from_file(i8*, %struct.node_t*) #2

; Function Attrs: nounwind
declare i8* @__strncpy_chk(i8*, i8*, i64, i64) #4

declare i64 @strlen(i8*) #2

; Function Attrs: nounwind
declare i8* @__strncat_chk(i8*, i8*, i64, i64) #4

; Function Attrs: nounwind
declare i8* @__strcpy_chk(i8*, i8*, i64) #4

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { noreturn "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="true" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { allocsize(0) "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { nounwind readnone speculatable }
attributes #6 = { argmemonly nounwind }
attributes #7 = { noreturn }
attributes #8 = { allocsize(0) }
attributes #9 = { nounwind }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{!"Apple LLVM version 10.0.0 (clang-1000.10.44.2)"}
