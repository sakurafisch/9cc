#include "9cc.h"

extern char *user_input;

// 正在处理的标记
extern Token *token;


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

		if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
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




Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
	Node *node = calloc(1, sizeof(Node));
	node->kind = kind;
	node->lhs  = lhs;
	node->rhs  = rhs;
	return node;
}

Node *new_node_num(int val) {
	Node *node = calloc(1, sizeof(Node));
	node->kind = ND_NUM;
	node->val = val;
	return node;
}

Node *expr() {
	Node *node = mul();

	while (true) {
		if (consume('+')) {
			node = new_node(ND_ADD, node, mul());
		} else if (consume('-')) {
			node = new_node(ND_SUB, node, mul());
		} else {
			return node;
		}
	}
}

Node *mul() {
	Node *node = term();

	while (true) {
		if (consume('*')) {
			node = new_node(ND_MUL, node, term());
		} else if (consume('/')) {
			node = new_node(ND_DIV, node, term());
		} else {
			return node;
		}
	}
}

Node *term() {
	// 下一个标记如果是 "("，则应该是 "(" expr ")"
	if (consume('(')) {
		Node *node = expr();
		expect(')');
		return node;
	}

	// 否则返回数值
	return new_node_num(expect_number());
}

