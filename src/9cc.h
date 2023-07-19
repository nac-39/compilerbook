#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOG_FILE "log/log.txt"
#define _DEBUG

#ifdef _DEBUG
#define LOGGER(fmt, ...) logger(fmt, ##__VA_ARGS__)
#else
#define LOGGER(s) /* do nothing */
#endif

#ifdef _DEBUG
// ログ用関数
void logger(char fmt[256], ...);
#endif

// トークンの種類
typedef enum {
  TK_RESERVED,  // 記号
  TK_IDENT,     // 識別子
  TK_NUM,       // 整数トークン
  TK_EOF,       // 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

// トークン型
struct Token {
  TokenKind kind;  // トークンの型
  Token *next;     // 次の入力トークン
  int val;         // kindがTK_NUMの場合、その数値
  char *str;       // トークン文字列
  int len;         // トークンの長さ
};

// 抽象構文木のノードの種類
typedef enum {
  ND_ADD,     // +
  ND_SUB,     // -
  ND_MUL,     // *
  ND_DIV,     // /
  ND_EQ,      // ==
  ND_NE,      // !=
  ND_LT,      // <
  ND_LE,      // <=
  ND_NUM,     // 整数
  ND_ASSIGN,  // =
  ND_LVAR,    // ローカル変数
} NodeKind;

// enumの名前を取得する関数
char *get_token_name(TokenKind kind);
char *get_node_name(NodeKind kind);

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
  NodeKind kind;  // ノードの方
  Node *lhs;      // 左辺
  Node *rhs;      // 右辺
  int val;        // kindがND_NUMの場合のみ使う
  int offset;  // kindがND_LVAR(ローカル変数)の場合のみ使う。ローカル変数のベースポインタからの　オフセットを表す。
};

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
void expect(char *op);
int expect_number();
bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
bool startswith(char *p, char *q);
Token *tokenize();

Node *new_node(NodeKind kind);
Node *new_binary(NodeKind kind, Node *lhs, Node *rhs);
Node *new_num(int val);
void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

void gen(Node *node);

// グローバル変数
extern char *user_input;

// 現在注目しているトークン
extern Token *token;

// プログラムを格納する配列
extern Node *code[100];

extern FILE *log_file;