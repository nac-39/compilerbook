#include "9cc.h"

// 次のトークンが期待している記号の時には、トークンを一つ読み進めて
// 　真を返す。それ以外の場合には偽を返す。
bool consume(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
};

// ローカル変数を呼んでトークンを一つ進める
Token *consume_ident() {
  Token *old_token = token;
  if (token->kind != TK_IDENT || strlen(token->str) != token->len ||
      *token->str < '0' || (*token->str > 9 && *token->str < 'a') ||
      *token->str > 'z')  // 数字と文字以外のasciiコードを排除
    return NULL;
  token = token->next;
  return new_token(TK_IDENT, old_token, old_token->str, old_token->len);
}

// 次のトークンが期待している記号の時には、トークンを一つ読み進める。
// それ以外の場合にはエラーを報告する。
void expect(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    error_at(token->str, "'%s'ではありません。", op);
  token = token->next;
}

// 次のトークンが数値の場合、トークンを一つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number() {
  if (token->kind != TK_NUM) error_at(token->str, "数ではありません");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() { return token->kind == TK_EOF; }

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  // 1つのTokenサイズのメモリを確保。*tokはそのメモリのアドレスを指す。
  // callocはallocとは違い、全てのメモリを０クリアする。
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

bool startswith(char *p, char *q) { return memcmp(p, q, strlen(q)) == 0; }

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize() {
  char *p = user_input;
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    // 空白文字をスキップ
    if (isspace(*p)) {
      p++;
      continue;
    }

    // 記号
    if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") ||
        startswith(p, ">=")) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    if (strchr("+-*/()<>;", *p)) {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    if ('a' <= *p && *p <= 'z') {
      cur = new_token(TK_IDENT, cur, p++, 1);
      cur->len = 1;
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 0);
      char *q = p;
      cur->val = strtol(p, &p, 10);
      cur->len = p - q;
      continue;
    }
    error_at(p, "トークナイズできません。");
  }

  new_token(TK_EOF, cur, p, 0);
  return head.next;
}

// あたらしいノードを作る
Node *new_node(NodeKind kind) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  return node;
}

// 新しい二分木を作る？
Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = new_node(kind);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_num(int val) {
  Node *node = new_node(ND_NUM);
  node->val = val;
  return node;
}

// 文法一覧
// program    = stmt*
// stmt       = expr ";"
// expr       = assign
// assign     = equality ("=" assign)?
// equality   = relational ("==" relational | "!=" relational)*
// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
// add        = mul ("+" mul | "-" mul)*
// mul        = unary ("*" unary | "/" unary)*
// unary      = ("+" | "-")? primary
// primary    = num | ident | "(" expr ")"


// program    = stmt*
void program() {
  int i = 0;
  while (!at_eof()) code[i++] = stmt();
  code[i] = NULL;
}

// stmt       = expr ";"
Node *stmt() {
  Node *node = expr();
  expect(";");
  return node;
}

// expr       = assign
Node *expr() {
  Node *node = assign();
  return node;
}

// assign     = equality ("=" assign)?
Node *assign() {
  Node *node = equality();
  if (consume("=")) node = new_binary(ND_ASSIGN, node, assign());
}

// equality = relational ("==" relational | "!=" relational)*
Node *equality() {
  Node *node = relational();
  for (;;) {
    if (consume("=="))
      node = new_binary(ND_EQ, node, relational());
    else if (consume("!="))
      node = new_binary(ND_NE, node, relational());
    else
      return node;
  }
}

// relational = add("<" add | "<=" add | ">" add | ">=" add)*
Node *relational() {
  Node *node = add();
  for (;;) {
    if (consume("<"))
      node = new_binary(ND_LT, node, add());
    else if (consume("<="))
      node = new_binary(ND_LE, node, add());
    else if (consume(">"))
      node = new_binary(ND_LT, add(), node);
    else if (consume(">="))
      node = new_binary(ND_LE, add(), node);
    else
      return node;
  }
}

// add = mul ("+ mul | "- mul)*
Node *add() {
  Node *node = mul();
  for (;;) {
    if (consume("+"))
      node = new_binary(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_binary(ND_SUB, node, mul());
    else
      return node;
  }
}

// mul = unary ("*" unary | "/" unary)*
Node *mul() {
  Node *node = unary();
  for (;;) {
    if (consume("*"))
      node = new_binary(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_binary(ND_DIV, node, unary());
    else
      return node;
  }
}

// unary = ("+" | "-")? unary | primary
Node *unary() {
  if (consume("+")) return unary();
  if (consume("-")) return new_binary(ND_SUB, new_num(0), unary());
  return primary();
}

// primary    = num | ident | "(" expr ")"
Node *primary() {
  // 次のトークンが"*"なら、"(" expr ")"のはず
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  // ローカル変数があれば読む
  Token *tok = consume_ident();
  if (tok) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    node->offset = (tok->str[0] - 'a' + 1) * 8;
    error("consume_ident!!");
    return node;
  }
  // そうでなければ数値のはず;
  return new_num(expect_number());
}
