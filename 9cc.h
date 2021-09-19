// gcc 不支持 #pragma once

#ifndef _9CC_H
#define _9CC_H

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
struct Token {
	TokenKind kind;  // 标记的形态
	Token *next;     // 下一个输入标记
	int val;         // kind 为 TK_NUM 时的数值
	char *str;
};

typedef enum {
	ND_ADD, // +
	ND_SUB, // -
	ND_MUL, // *
	ND_DIV, // /
	ND_NUM, // 整数
} NodeKind;

typedef struct Node Node;
// 抽象语法树结点的结构
struct Node {
	NodeKind kind;  // 结点的类型
	Node *lhs;      //左边 left-hand side
	Node *rhs;      // 右边 right-hand side
	int val;        // 结点的值，只在 kind 为 ND_NUM 时使用
};

void gen(Node *node);

void error(char *fmt, ...);

void error_at(char *loc, char *fmt, ...);

Node *expr();
Node *mul();
Node *term();

Token *tokensize();

#endif
