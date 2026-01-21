#include "func.h"

void CallFunction(LLVMValueRef cell_ptr, LLVMBuilderRef builder, LLVMTypeRef i8) {
	LLVMValueRef func_id = LLVMBuildLoad2(builder, i8, cell_ptr, "func_id");
}
