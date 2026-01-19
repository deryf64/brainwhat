#include "codegen.h"
#include "ast.h"
#include <llvm-c/Core.h>
#include <llvm-c/Target.h>
#include <stdio.h>

static LLVMValueRef get_or_create_putchar(LLVMModuleRef mod) {
	LLVMValueRef f = LLVMGetNamedFunction(mod, "putchar");
	if (f) return f;

	LLVMTypeRef param_types[] = { LLVMInt32Type() };
	LLVMTypeRef ft = LLVMFunctionType(LLVMInt32Type(), param_types, 1, 0);

	return LLVMAddFunction(mod, "putchar", ft);
}

static LLVMValueRef get_or_create_getchar(LLVMModuleRef mod) {
	LLVMValueRef f = LLVMGetNamedFunction(mod, "getchar");
	if (f) return f;

	LLVMTypeRef ft = LLVMFunctionType(LLVMInt32Type(), NULL, 0, 0);

	return LLVMAddFunction(mod, "getchar", ft);
}

void codegen(ASTNode* node, LLVMBuilderRef builder, LLVMModuleRef module) {
	if (!node) return;

	LLVMValueRef tape = LLVMGetNamedGlobal(module, "tape");
	LLVMValueRef ptr  = LLVMGetNamedGlobal(module, "ptr");

	LLVMTypeRef i8  = LLVMInt8Type();
	LLVMTypeRef i32 = LLVMInt32Type();

	if (node->type == NODE_COMMAND) {
		LLVMValueRef index = LLVMBuildLoad2(builder, i32, ptr, "ptr_load");

		LLVMValueRef indices[2] = { LLVMConstInt(i32, 0, 0), index };
		LLVMValueRef cell = LLVMBuildGEP2(builder, LLVMArrayType(i8, 30000), tape, indices, 2, "cell");

		LLVMValueRef val = LLVMBuildLoad2(builder, i8, cell, "cell_val");

		switch (node->token) {
			case '+': {
			LLVMValueRef inc = LLVMBuildAdd(builder, val, LLVMConstInt(i8, 1, 0), "inc");
			LLVMBuildStore(builder, inc, cell);
			break;
		}
		case '-': {
			LLVMValueRef dec = LLVMBuildSub(builder, val, LLVMConstInt(i8, 1, 0), "dec");
			LLVMBuildStore(builder, dec, cell);
			break;
		}
		case '>': {
			LLVMValueRef next = LLVMBuildAdd(builder, index, LLVMConstInt(i32, 1, 0), "ptr_inc");
			LLVMBuildStore(builder, next, ptr);
			break;
		}
		case '<': {
			LLVMValueRef prev = LLVMBuildSub(builder, index, LLVMConstInt(i32, 1, 0), "ptr_dec");
			LLVMBuildStore(builder, prev, ptr);
			break;
		}
		case '.': {
			LLVMValueRef putchar_fn = get_or_create_putchar(module);

			LLVMValueRef v32 = LLVMBuildZExt(builder, val, i32, "char_to_int");

			LLVMValueRef args[] = { v32 };
			LLVMTypeRef ft = LLVMFunctionType(i32, &i32, 1, 0);

			LLVMBuildCall2(builder, ft, putchar_fn, args, 1, "");
			break;
		}
		case ',': {
			LLVMValueRef getchar_fn = get_or_create_getchar(module);
			LLVMTypeRef ft = LLVMFunctionType(i32, NULL, 0, 0);

			LLVMValueRef input =
				LLVMBuildCall2(builder, ft, getchar_fn, NULL, 0, "input");

			LLVMValueRef input_i8 =
				LLVMBuildTrunc(builder, input, i8, "input_i8");

			LLVMBuildStore(builder, input_i8, cell);
			break;
		}
		default:
			break;
		}
	}
	else if (node->type == NODE_LOOP) {
		LLVMBasicBlockRef current_bb = LLVMGetInsertBlock(builder);
		LLVMValueRef fn = LLVMGetBasicBlockParent(current_bb);

		LLVMBasicBlockRef cond_bb  = LLVMAppendBasicBlock(fn, "loop_cond");
		LLVMBasicBlockRef body_bb  = LLVMAppendBasicBlock(fn, "loop_body");
		LLVMBasicBlockRef after_bb = LLVMAppendBasicBlock(fn, "loop_after");

		// Cond check
		LLVMBuildBr(builder, cond_bb);

		// Cond block
		LLVMPositionBuilderAtEnd(builder, cond_bb);

		LLVMValueRef index = LLVMBuildLoad2(builder, i32, ptr, "ptr_load");
		LLVMValueRef indices[2] = { LLVMConstInt(i32, 0, 0), index };
		LLVMValueRef cell = LLVMBuildGEP2(builder, LLVMArrayType(i8, 30000), tape, indices, 2, "cell");

		LLVMValueRef val = LLVMBuildLoad2(builder, i8, cell, "cell_val");
		LLVMValueRef is_nonzero = LLVMBuildICmp(builder, LLVMIntNE, val, LLVMConstInt(i8, 0, 0), "loop_cond");

		LLVMBuildCondBr(builder, is_nonzero, body_bb, after_bb);

		// Loop body
		LLVMPositionBuilderAtEnd(builder, body_bb);

		for (size_t i = 0; i < node->loop_body.size; i++) {
			codegen(node->loop_body.children[i], builder, module);
		}

		// Back to cond
		LLVMBuildBr(builder, cond_bb);

		// After loop bodu
		LLVMPositionBuilderAtEnd(builder, after_bb);
	}
}

void init_codegen(LLVMModuleRef* module, LLVMBuilderRef* builder) {
	LLVMInitializeNativeTarget();
	LLVMInitializeNativeAsmPrinter();

	*module = LLVMModuleCreateWithName("bf_module");
	*builder = LLVMCreateBuilder();

	LLVMTypeRef i8  = LLVMInt8Type();
	LLVMTypeRef i32 = LLVMInt32Type();

	// Mem tape [ 30000 x i8 ]
	LLVMTypeRef tape_ty = LLVMArrayType(i8, 30000);
	LLVMValueRef tape = LLVMAddGlobal(*module, tape_ty, "tape");
	LLVMSetInitializer(tape, LLVMConstNull(tape_ty));
	
	// Pointer: i32
	LLVMValueRef ptr = LLVMAddGlobal(*module, i32, "ptr");
	LLVMSetInitializer(ptr, LLVMConstInt(i32, 0, 0));
}
