#include "9cc.h"

// グローバル変数
char *user_input;
Token *token;  // 現在注目しているトークン

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...) {
  va_list ap;  // ただのchar型のポインタ
  // 　可変超引数を一つの変数にまとめる
  va_start(ap, fmt);
  // まとめられた変数で処理する
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
};

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, "");  // posこの空白を出力
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    error("引数の個数が正しくありません");
    return 1;
  }

  user_input = argv[1];
  token = tokenize();
  Node *node = expr();

  // アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  gen(node);

  // スタックトップに敷き全体の値が残っていはずなので、
  // それをRAXにロードして関数からの返り血とする
  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}
