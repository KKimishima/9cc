#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<stdbool.h>
#include<string.h>
#include<ctype.h>

typedef enum{
  TK_RESERVED, // 記号
  TK_NUM, // 整数トークン
  TK_EOF, // 入力の終わりのトークン
} TokenKind;

typedef struct Token Token;

struct Token{
  TokenKind kind; // トークンの型
  Token *next; // 次の入力トークン
  int val; // kindがTK_NUMの場合、その数値
  char *str; // トークン文字列
};

// 現在着目しているトークン
Token *token;

// 入力プログラム
char *user_input;

// エラーの箇所を報告する
void error_at(char *loc,char *fmt, ...){
  va_list ap;
  va_start(ap,fmt);

  int pos = loc - user_input;
  fprintf(stderr,"%s\n",user_input);
  fprintf(stderr,"%*s",pos, " "); //posの個数分空白を出力
  fprintf(stderr, "^");
  vfprintf(stderr,fmt,ap);
  fprintf(stderr,"\n");
  exit(1);
}

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...){
 va_list ap;
 va_start(ap,fmt);
 vfprintf(stderr,fmt,ap);
 fprintf(stderr,"\n");
 exit(1);
}


// 次のトークンが期待している記号のときに、トークンを1つ進める真を返す
// その以外の場合は偽を返す
bool consume(char op){
  if(token->kind != TK_RESERVED || token->str[0] != op)
    return false;
  token = token->next;
  return true;
}

// 次のトークンが期待している記号のときに、トークンを１つ進める。
// それ以外はエラーを報告する
void expect(char op){
  if(token->kind != TK_RESERVED || token->str[0] != op)
    error_at(token->str, "'%c'ではありません。",op);
  token = token->next;
}

// 次のトークンが数値の場合、トークンを一つ読みその数値を返す。
// それ以外はエラーを報告する
int expect_number(){
  if(token->kind != TK_NUM){
    error_at(token->str, "数値ではありません");
  }
  int val = token->val;
  token = token->next;
  return val;
}

// 終端文字を判別
bool at_eof(){
  return token->kind == TK_EOF;
}

// 新しいトークンを作成してcurにつなげる
Token *new_token(TokenKind kind, Token *cur, char *str){
  Token *tok = calloc(1,sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(){
  char *p = user_input;
  Token head;
  head.next = NULL;
  Token *cur = &head;
  while (*p){
    // 空文字をスキップ
    if(isspace(*p)){
      p++;
      continue;
    }

    // 記号
    if(*p == '+' || *p == '-'){
      cur = new_token(TK_RESERVED,cur,p++);
      continue;
    }

    // 数字
    if(isdigit(*p)){
      cur = new_token(TK_NUM,cur,p);
      cur->val = strtol(p,&p,10);
      continue;
    }

    error_at(p,"トークナイズできません");
  }

  new_token(TK_EOF,cur,p);
  return head.next;
}

int main(int argc, char const *argv[]){
  if(argc != 2){
    error("引数が正しくありません\n");
    return 1;
  }

  // トークナイズ
  user_input = argv[1];
  token = tokenize();

  // アセンブリ前半
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // 式の最小は数でなければならないのでチェック
  // 最初のmov命令を出力
  printf("  mov rax, %d\n", expect_number());

  // `+ <数>`あるいは`- <数>`というトークの並びを消費し、アセンブリを出力
  while (!at_eof()) {
    if(consume('+')){
      printf("  add rax, %d\n",expect_number());
      continue;
    }
    expect('-');
    printf("  sub rax, %d\n", expect_number());
 }
 printf("  ret\n");
 return 0;
}
