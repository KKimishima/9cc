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

// 抽象構文木のノード種類
typedef enum{
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
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
    if(strchr("+-*/()", *p)){
      cur = new_token(TK_RESERVED,cur,p++);
      continue;
    }

    // 数字
    if(isdigit(*p)){
      cur = new_token(TK_NUM,cur,p);
      cur->val = strtol(p,&p,10);
      continue;
    }

    error_at(p,"トークナイズで文字です");
  }

  new_token(TK_EOF,cur,p);
  return head.next;
}

Node *new_node(NodeKind kind,Node *lhs,Node *rhs){
  Node *node = calloc(1,sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val){
  Node *node = calloc(1,sizeof(Node));
  node->val = val;
  node->kind = ND_NUM;
  return node;
}

Node *expr();
Node *mul();
Node *primary();

// expr = mul ("+" mul | "-" mul)*生成規則
Node *expr(){
  Node *node = mul();
  for (;;){
    if(consume('+')){
      node = new_node(ND_ADD,node,mul());
    }else if (consume('-')){
      node = new_node(ND_SUB,node,mul());
    }else{
      return node;
    }
  }
}

// mul = primary ("*" primary | "/" primary)*生成規則
Node *mul(){
  Node *node = primary();
  for (;;){
    if(consume('*')){
      node = new_node(ND_MUL,node,primary());
    }else if(consume('/')){
      node = new_node(ND_DIV,node,primary());
    }else{
     return node;   
    }
  }
}

// primary = "(" expr ")" | num 生成規則
Node *primary(){
  // 次のトークンが"("であるならば,"(" expr ")" であるはず
  if(consume('(')){
    Node *node = expr();
    expect(')');
    return node;
  }
  // そうでなければ数値のはず
  return new_node_num(expect_number());
}

// スタック操作をアセンブラに変換
void gen(Node *node){
  // nodeが数値の場合はpushして終わり
  if(node->kind == ND_NUM){
    printf(" push %d\n",node->val);
    return;
  }

  // 数値ではない場合は、再帰で左右のノードを詰めて行く  
  gen(node->lhs);
  gen(node->rhs);

  printf(" pop rdi\n");
  printf(" pop rax\n");

  switch (node->kind) {
  case ND_ADD:
    printf(" add  rax, rdi\n");
    break;
  case ND_SUB:
    printf(" sub rax, rdi\n");
    break;
  case ND_MUL:
    printf(" imul rax, rdi\n");
    break;
  case ND_DIV:
    printf(" cqo\n");
    printf(" idiv rdi\n");
    break;
  }
  printf(" push rax\n");
}

int main(int argc, char const *argv[]){
  if(argc != 2){
    error("引数が正しくありません\n");
    return 1;
  }

  // トークナイズしてパース
  user_input = argv[1];
  token = tokenize();
  Node *node = expr();

  // アセンブリ前半
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // 抽象構文木を下りながらコード生成
  gen(node);

  // スタックトップに式全体の値が残っているので,RAXにロードして関数返り値とする
  printf(" pop rax\n");
  printf(" ret\n");
  return 0;
}
