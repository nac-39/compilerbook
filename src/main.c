#include "9cc.h"

// グローバル変数
char *user_input;
Token *token;  // 現在注目しているトークン
Node *code[100];

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
  LOGGER("start!!");
  if (argc != 2) {
    error("引数の個数が正しくありません");
    return 1;
  }

  user_input = argv[1];
  token = tokenize();
  program();
  // アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // プロローグ
  // 変数26個分の領域を確保する
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n");

  // 先頭の式から順にコード生成
  for (int i = 0; code[i]; i++) {
    gen(code[i]);
    // 式の評価結果としてスタックに一つの値が残っているはずなので、
    // スタックが溢れないようにポップしておく
    printf("  pop rax\n");
  }

  // エピローグ
  // スタックトップに敷き全体の値が残っていはずなので、
  // それをRAXにロードして関数からの返り血とする
  printf("  mov rsp, rbp\n");
  printf("  pop rax\n");
  printf("  ret\n");
  LOGGER("end!!");
  return 0;
}
