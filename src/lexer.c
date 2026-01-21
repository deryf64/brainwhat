#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

CharPosition charPos = { 0, 0 };
PositionStack openBracketStack = { 0 };
int CurTok = tok_eof;

void initBracketStack(void) {
	openBracketStack.positions = NULL;
	openBracketStack.size = 0;
	openBracketStack.capacity = 0;
}

void pushBracketPosition(CharPosition pos) {
	if (openBracketStack.size >= openBracketStack.capacity) {
		size_t new_cap = openBracketStack.capacity ? openBracketStack.capacity * 2 : 16;
		CharPosition *new_mem = realloc(openBracketStack.positions, new_cap * sizeof(CharPosition));
		if (!new_mem) {
			fprintf(stderr, "Cannot allocate memeory for bracket stack\n");
			exit(1);
		}
		openBracketStack.positions = new_mem;
		openBracketStack.capacity = new_cap;
	}
	openBracketStack.positions[openBracketStack.size++] = pos;
}

CharPosition popBracketPosition(void) {
	if (openBracketStack.size == 0) {
		fprintf(stderr, "Internal error: stack underflow\n");
		exit(1);
	}
	return openBracketStack.positions[--openBracketStack.size];
}

int isBracketStackEmpty(void) {
	return openBracketStack.size == 0;
}

void freeBracketStack(void) {
	free(openBracketStack.positions);
	openBracketStack.positions = NULL;
	openBracketStack.size = 0;
	openBracketStack.capacity = 0;
}

void countChar(int ch) {
	if (ch == EOF || ch == '\r') return;
	if (ch == '\n') {
		charPos.col = 0;
		charPos.row++;
	} else {
		charPos.col++;
	}
}

int gettok(void) {
	static int LastChar = ' ';

	for (;;) {
		if (LastChar == ' ') {
			LastChar = getchar();
			countChar(LastChar);
			continue;
		}

		if (LastChar == '%') {
			do {
				LastChar = getchar();
				countChar(LastChar);
			} while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');
			continue;
		}
		
		if (LastChar == EOF) {
			return tok_eof;
		}

		int ThisChar = LastChar;
		countChar(ThisChar);
		LastChar = getchar();

		if (strchr("+-><.,[]{}Z#", ThisChar)) {
			return ThisChar;
		}
	}
}

