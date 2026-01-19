#ifndef AST_H
#define AST_H

#include <stdlib.h>
#include <stdio.h>

typedef enum {
	NODE_COMMAND, // + - < > . ,
	NODE_LOOP     // [ ... ]
} NodeType;

typedef struct ASTNode ASTNode;

typedef struct {
	ASTNode **children;
	size_t size;
	size_t capacity;
} NodeList;

struct ASTNode {
	NodeType type;
	int token;
	NodeList loop_body;
};

ASTNode *create_node(NodeType type, int token);
void add_child(NodeList* list, ASTNode* child);
void free_ast(ASTNode* node);

#endif
