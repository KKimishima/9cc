//
// Created by kimishima on 2020/02/14.
//
#ifndef INC_9CC_MAIN_H
#define INC_9CC_MAIN_H

#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<stdbool.h>
#include<string.h>
#include<ctype.h>

// tokenize.c
// トークンの種類
typedef enum {
    TK_RESERVED, // 記号
    TK_NUM, // 整数トークン
    TK_EOF, // 入力の終わりのトークン
} TokenKind;

// トークンの型
typedef struct Token Token;
struct Token {
    TokenKind kind; // トークンの型
    Token *next; // 次の入力トークン
    int val; // kindがTK_NUMの場合、その数値
    char *str; // トークン文字列
    int len;    //トークンの長さ
};

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...);

// エラーの箇所を報告する
void error_at(char *loc, char *fmt, ...);

// 次のトークンが期待している記号のときに、トークンを1つ進める真を返す
// その以外の場合は偽を返す
bool consume(char *op);

// 次のトークンが期待している記号のときに、トークンを１つ進める。
// それ以外はエラーを報告する
void expect(char *op);

// 次のトークンが数値の場合、トークンを一つ読みその数値を返す。
// それ以外はエラーを報告する
int expect_number();

// 終端文字を判別
bool at_eof();

// 新しいトークンを作成してcurにつなげる
Token *new_token(TokenKind kind, Token *cur, char *str, int len);

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize();

// 入力プログラム
extern char *user_input;
// 現在着目しているトークン
extern Token *token;

// parse.c
// 抽象構文木のノード種類
typedef enum {
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_EQ,  // ==
    ND_NE,  // <
    ND_LT,  //<=
    ND_LE,  // <=
    ND_NUM  // 整数
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノード型
struct Node {
    NodeKind kind; // ノードの種類
    Node *lhs; // 左
    Node *rhs; // 右
    int val; //kindがND_NUMの場合ここを数値として使う
};

// expr = equality
Node *expr();

// codegen.c
// スタック操作をアセンブラに変換
void codegen(Node *node);

#endif //INC_9CC_MAIN_H
