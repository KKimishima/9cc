//
// Created by kimishima on 2020/02/14.
//
#include "9cc.h"

// スタック操作をアセンブラに変換
void gen(Node *node) {
  // nodeが数値の場合はpushして終わり
  if (node->kind == ND_NUM) {
    printf(" push %d\n", node->val);
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
    case ND_EQ:
      printf("  cmp rax, rdi\n");
      printf("  sete al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_NE:
      printf("  cmp rax, rdi\n");
      printf("  setne al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LT:
      printf("  cmp rax, rdi\n");
      printf("  setl al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LE:
      printf("  cmp rax, rdi\n");
      printf("  setle al\n");
      printf("  movzb rax, al\n");
      break;
  }
  printf(" push rax\n");
}

void codegen(Node *node) {
  // アセンブリ前半
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // 抽象構文木を下りながらコード生成
  gen(node);

  // スタックトップに式全体の値が残っているので,RAXにロードして関数返り値とする
  printf(" pop rax\n");
  printf(" ret\n");
}