#ifndef LEXER_H
#define LEXER_H

#include <string.h>

enum Token {
	tok_eof = -1,
	
	tok_inc = '+',
	tok_dec = '-',
	tok_rgt = '>',
	tok_lft = '<',
	tok_ech = '.',
	tok_get = ',',
	tok_lpo = '[',
	tok_lpc = ']',

	tok_szr = 'Z', // Set Zero
	tok_fnc = '#', // Call function
	tok_mvr = '}', // Move data right
	tok_mvl = '{', // Move data left
};

typedef struct CharPosition {
	int col;
	int row;
} CharPosition;

CharPosition charPos = { 0, 0 };

void countChar(int ch) {
	if (ch == EOF || ch == '\r') return;
	if (ch == '\n') {
		charPos.col = 0;
		charPos.row++;
	} else {
		charPos.col++;
	}
}

static int gettok() {
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

extern int CurTok;

#endif
