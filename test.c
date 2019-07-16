#include <stdio.h>
#include <stdbool.h>

#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>
#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/IRReader.h>

int main () {
  LLVMInitializeNativeTarget();

  char* triple = LLVMGetDefaultTargetTriple();

  char* error = NULL;
  LLVMTargetRef target;
  if (LLVMGetTargetFromTriple(triple, &target, &error)) {
    fprintf(stderr, "LLVMGetTargetFromTriple: %s\n", error);
    return 1;
  }
  LLVMDisposeMessage(error);

  LLVMModuleRef mod = LLVMModuleCreateWithName("main");

  LLVMTypeRef node = LLVMStructCreateNamed(LLVMGetGlobalContext(), "node");
  LLVMTypeRef fields[2] = {
    LLVMInt32Type(),
    LLVMPointerType(node, 0)
  };
  LLVMStructSetBody(node, fields, 2, 0);

  //LLVMTypeRef param_types[] = { LLVMInt32Type(), LLVMInt32Type() };
  //LLVMTypeRef main_ty = LLVMFunctionType(LLVMInt32Type(), NULL, 0, 0);
  LLVMTypeRef main_ty = LLVMFunctionType(node, NULL, 0, 0);
  LLVMValueRef main = LLVMAddFunction(mod, "main", main_ty);

  LLVMBasicBlockRef entry = LLVMAppendBasicBlock(main, "entry");
  LLVMBuilderRef builder = LLVMCreateBuilder();
  LLVMPositionBuilderAtEnd(builder, entry);

  LLVMValueRef vals[2] = {
    LLVMConstInt(LLVMInt32Type(), 5, false),
    LLVMConstPointerNull(node)
  };
  LLVMValueRef inst = LLVMConstNamedStruct(node, vals, 2);
  LLVMBuildRet(builder, inst);

  //LLVMBuildRet(builder, LLVMConstInt(LLVMInt32Type(), 0, false));

  LLVMDumpModule(mod);
}
