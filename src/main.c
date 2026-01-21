#include <stdio.h>
#include <stdlib.h>
#include <llvm-c/Core.h>
#include <llvm-c/Target.h>

#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

LLVMValueRef create_main_function(LLVMModuleRef module, LLVMBuilderRef builder, ASTNode* root) {
	LLVMTypeRef ret_ty = LLVMInt32Type();
	LLVMTypeRef main_ty = LLVMFunctionType(ret_ty, NULL, 0, 0);
	LLVMValueRef main_fn = LLVMAddFunction(module, "main", main_ty);

	LLVMBasicBlockRef entry = LLVMAppendBasicBlock(main_fn, "entry");
	LLVMPositionBuilderAtEnd(builder, entry);

	for (size_t i = 0; i < root->loop_body.size; i++)
		codegen(root->loop_body.children[i], builder, module);

	LLVMBuildRet(builder, LLVMConstInt(ret_ty, 0, 0));

	return main_fn;
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <brainfuck_file.bf>\n", argv[0]);
		return 1;
	}

	if (freopen(argv[1], "r", stdin) == NULL) {
		perror("Cannot open input file");
		return 1;
	}

	LLVMModuleRef module = NULL;
	LLVMBuilderRef builder = NULL;

	init_codegen(&module, &builder);

	initBracketStack();
	ASTNode *root = parse_program();
	if (!isBracketStackEmpty()) {
		fprintf(stderr, "Unclosed loops:\n");
		while (!isBracketStackEmpty()) {
			CharPosition p = popBracketPosition();
			fprintf(stderr, " [ on row %d, col %d\n", p.row + 1, p.col + 1);
		}
		exit(1);
	}
	freeBracketStack();

	create_main_function(module, builder, root);

	free_ast(root);

	LLVMPrintModuleToFile(module, argv[2], NULL);

	// Сюда потом добавить верификацию, оптимизацию, генерацию объекта и т.д.
	// LLVMVerifyModule(module, LLVMPrintMessageAction, NULL);

	LLVMDisposeBuilder(builder);
	// LLVMDisposeModule(module);

	return 0;
}
