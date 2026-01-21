#include "parser.h"

int getNextToken(void) {
	return CurTok = gettok();
}

ASTNode *parse_expression(void) {
	ASTNode *node = NULL;
	CharPosition token_start_pos = charPos;

	switch (CurTok) {
		case tok_inc: case tok_dec:
		case tok_rgt: case tok_lft:
		case tok_ech: case tok_get:
		case tok_mvr: case tok_mvl:
		case tok_szr: case tok_fnc:
			node = create_node(NODE_COMMAND, CurTok);
			int current = CurTok;
			int count = 1;
			getNextToken();

			while (CurTok == current) {
				count++;
				getNextToken();
			}

			if ((current == '+' && CurTok == '-') ||
			    (current == '-' && CurTok == '+') ||
			    (current == '>' && CurTok == '<') ||
			    (current == '<' && CurTok == '>') ||
			    (current == '}' && CurTok == '{') ||
			    (current == '{' && CurTok == '}')) {
				getNextToken();
				return NULL;
			}

			node->token = current;
			node->count = count;

			return node;
		case tok_lpo:
			pushBracketPosition(token_start_pos);
			getNextToken();
			node = create_node(NODE_LOOP, 0);

			while (CurTok != tok_lpc && CurTok != tok_eof) {
				ASTNode *child = parse_expression();
				if (child) {
					add_child(&node->loop_body, child);
				}
			}

			if (CurTok == tok_lpc) {
				getNextToken();
				popBracketPosition();
			} else {
				CharPosition open_pos = openBracketStack.positions[openBracketStack.size - 1];

				fprintf(stderr, "\e[1;31mError:\e[0m expected ']' (unclosed loop) at line %d, col %d\n",
						open_pos.row + 1, open_pos.col + 1);
				exit(1);
			}

			// [+] [-]
			if (node->loop_body.size == 1) {
				ASTNode *child = node->loop_body.children[0];
				if (child->type == NODE_COMMAND) {
					if (child->token == '+' || child->token == '-') {
						free_ast(child);
						free(node->loop_body.children);
						node->type = NODE_COMMAND;
						node->token = 'Z';
						node->loop_body.children = NULL;
						node->loop_body.size = 0;
						node->loop_body.capacity = 0;
					}
				}
			}

			return node;
		case tok_lpc:
			fprintf(stderr, "\e[1;31mError:\e[0m stray closing bracket ']' at line %d, col %d\n",
					token_start_pos.row + 1, token_start_pos.col + 1);
			exit(1);

		default:
			getNextToken();
			return NULL;
	}
}

ASTNode *parse_program(void) {
	ASTNode *root = create_node(NODE_LOOP, 0);
	getNextToken();

	while (CurTok != tok_eof) {
		ASTNode *node = parse_expression();
		if (node) {
			add_child(&root->loop_body, node);
		}
	}
	return root;
}

