#include "9cc.h"

int main(int argc, char const *argv[]) {
  if (argc != 2) {
    error("引数が正しくありません\n");
    return 1;
  }

  // トークナイズしてパース
  user_input = argv[1];
  token = tokenize();
  Node *node = expr();

  codegen(node);
  return 0;
}
