#ifndef FUNCS_H
#define FUNCS_H

#include <llvm-c/Core.h>

void CallFunction(LLVMValueRef cell_ptr, LLVMBuilderRef builder, LLVMTypeRef i8);
// Добавить функции:
//	sqrt 32 64
//	add, dif, mp, div 8 16 32 64
//	


#endif
