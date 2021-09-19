#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

typedef enum {
	TK_RESERVED, // 整数
	TK_NUM,      // 整数标记
	TK_EOF,      // 代表输入结束的标记
} TokenKind;

typedef struct Token Token;

// 标记形态
struct Token {
	TokenKind kind;  // 标记的形态
	Token *next;     // 下一个输入标记
	int val;         // kind 为 TK_NUM 时的数值
	char *str;
};

// Input program
char *user_input;

// 正在处理的标记
Token *token;

// 处理错误的函数
// 取和 printf 相同的引数
void error(char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

// Reports an error location and exit.
void error_at(char *loc, char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);

	int pos = loc - user_input;
	fprintf(stderr, "%s\n", user_input);
	fprintf(stderr, "%*s", pos, "");
	fprintf(stderr, "^ ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

// 下一个标记为符合预期的符号时，读入一个标记并往下继续，
// 回传 true。否则回传 false。
bool consume(char op) {
	if (token->kind != TK_RESERVED || token->str[0] != op)
	    return false;
	token = token->next;
	return true;
}

// 下一个标记为符合预期的符号时，读入一个标记并往下继续。
// 否则警告为错误
void expect(char op) {
	if (token->kind != TK_RESERVED || token->str[0] != op) {
		error_at(token->str, "expected '%c'", op);
	}
	token = token->next;
}

// 下一个标记为数值时，读入一个标记并往下继续，回传该数值
// 否则警告为错误
int expect_number() {
	if (token->kind != TK_NUM) {
		// error("不是数值");
		error_at(token->str, "expected a number");
	}
	int val = token->val;
	token = token->next;
	return val;
}

bool at_eof() {
	return token->kind == TK_EOF;
}

// 建立一个新的标记，连接到 cur
Token *new_token(TokenKind kind, Token *cur, char *str) {
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	cur->next = tok;
	return tok;
}

// Tokenize `user_input` and returns new tokens
Token *tokensize() {
	char *p = user_input;
	Token head;
	head.next = NULL;
	Token *cur = &head;

	while (*p) {
		// 跳过空白字符串
		if (isspace(*p)) {
			p++;
			continue;
		}

		if (*p == '+' || *p == '-') {
			cur = new_token(TK_RESERVED, cur, p++);
			continue;
		}

		if (isdigit(*p)) {
			cur = new_token(TK_NUM, cur, p);
			cur->val = strtol(p, &p, 10);
			continue;
		}

		error_at(p, "expected a number");
	}

	new_token(TK_EOF, cur, p);
	return head.next;
}

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "引数数量错误\n");
		return 1;
	}

	// 标记解析
	user_input = argv[1];
	token = tokensize();

	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	// 确认算式必须以数开头
	// 输出最初的 mov 指令
	printf("  mov rax, %d\n", expect_number());

	// 一边消耗 `+ <数>` 或 `- <数>` 的标记，
	// 并输出组合语言指令
	while (!at_eof()) {
		if (consume('+')) {
			printf("  add rax, %d\n", expect_number());
			continue;
		}

		expect('-');
		printf("  sub rax, %d\n", expect_number());
	}

	printf("  ret\n");
	return 0;
}

