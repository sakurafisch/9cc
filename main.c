#include "9cc.h"

// Input program
char *user_input;

// 正在处理的标记
Token *token;

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "引数数量错误\n");
		return 1;
	}

	// 标记解析
	user_input = argv[1];
	token = tokensize();
	Node *node = expr();

	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	gen(node);

	printf("  pop rax\n");
	printf("  ret\n");
	return 0;
}

