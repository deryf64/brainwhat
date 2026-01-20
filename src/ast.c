#include "ast.h"

ASTNode* create_node(NodeType type, int token) {
	ASTNode *node = calloc(1, sizeof(ASTNode));
	if (!node) return NULL;
	node->type = type;
	node->count = 1;
	node->token = token;
	node->loop_body.children = NULL;
	node->loop_body.size = 0;
	node->loop_body.capacity = 0;
	return node;
}

void add_child(NodeList *list, ASTNode *child) {
	if (!child) return;

	if (list->size >= list->capacity) {
		size_t new_cap = list->capacity ? list->capacity * 8 : 8;
		ASTNode **new_ptr = realloc(list->children, new_cap * sizeof(ASTNode*));
		if (!new_ptr) {
			fprintf(stderr, "realloc failed\n");
			exit(1);
		}
		list->children = new_ptr;
		list->capacity = new_cap;
	}
	list->children[list->size++] = child;
}

void free_ast(ASTNode *node) {
	if (!node) return;
	if (node->type == NODE_LOOP) {
		for (size_t i = 0; i < node->loop_body.size; i++)
			free_ast(node->loop_body.children[i]);
		free(node->loop_body.children);
	}
	free(node);
}
