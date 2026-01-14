#ifndef ABSTRACT_SYNTAX_TREE_H
#define ABSTRACT_SYNTAX_TREE_H

#include <stdlib.h>

typedef enum {
	NODE_COMMAND, // + - < > . ,
	NODE_LOOP     // [ ... ]
} NodeType;

struct ASTNode;

typedef struct {
	struct ASTNode** children;
	size_t size;
	size_t capacity;
} NodeList;

typedef struct ASTNode {
	NodeType type;
	int token;
	NodeList loop_body;
} ASTNode;


ASTNode* create_node(NodeType type, int token) {
	ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
	node->type = type;
	node->token = token;
	node->loop_body.children = NULL;
	node->loop_body.size = 0;
	return node;
}

void add_to_loop(ASTNode* loop, ASTNode* child) {
	if (loop->loop_body.size >= loop->loop_body.capacity) {
		loop->loop_body.capacity = loop->loop_body.capacity == 0 ? 4 : loop->loop_body.capacity * 2;
		loop->loop_body.children = realloc(loop->loop_body.children, sizeof(ASTNode*) * loop->loop_body.capacity);
	}
	loop->loop_body.children[loop->loop_body.size++] = child;
}

#endif
