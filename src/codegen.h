#ifndef CODEGEN_H
#define CODEGEN_H

#include <llvm-c/Core.h>
#include "ast.h"

void init_codegen(LLVMModuleRef *module, LLVMBuilderRef *builder);
void codegen(ASTNode* node, LLVMBuilderRef builder, LLVMModuleRef module);

#endif
