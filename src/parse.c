#include "9cc.h"

// 次のトークンが期待している記号の時には、トークンを一つ読み進めて
// 　真を返す。それ以外の場合には偽を返す。
bool consume(char *op) {
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  return true;
};

// 次のトークンが期待しているトークンの時には、トークンを一つ読み進めて
// 　真を返す。それ以外の場合には偽を返す。
bool consume_token(TokenKind kind) {
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  LOGGER("actually: %s, expected: %s", get_token_name(token->kind),
         get_token_name(kind));
  if (token->kind != kind)
    return false;
  token = token->next;
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  return true;
};

// ローカル変数を呼んでトークンを一つ進める
Token *consume_ident() {
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  LOGGER("consume_ident(): %s", get_token_name(token->kind));
  LOGGER("consume_ident(): kind=%s, str=%s, len=%d, strlen=%d",
         get_token_name(token->kind), token->str, token->len,
         strlen(token->str));
  Token *old_token = token;
  if (token->kind != TK_IDENT || *token->str < '0' ||
      (*token->str > 9 && *token->str < 'a') || *token->str > 'z') {
    return NULL;
  } // 数字と文字以外のasciiコードを排除
  token = token->next;
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  return old_token;
}

// 次のトークンが期待している記号の時には、トークンを一つ読み進める。
// それ以外の場合にはエラーを報告する。
void expect(char *op) {
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  LOGGER("strlen=%d, token->len=%d, memcmp=%d, token->str=%s, op=%s",
         strlen(op), token->len, memcmp(token->str, op, token->len), token->str,
         op);
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    error_at(token->str, "'%s'ではありません。", op);
  token = token->next;
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
}

// 次のトークンが数値の場合、トークンを一つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number() {
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  LOGGER("expect_number: num=%d, kind=%s", token->val,
         get_token_name(token->kind));
  if (token->kind != TK_NUM)
    error_at(token->str, "数ではありません");
  int val = token->val;
  token = token->next;
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  return val;
}

bool at_eof() { return token->kind == TK_EOF; }

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  Token *tok = calloc(1, sizeof(Token));
  // 1つのTokenサイズのメモリを確保。*tokはそのメモリのアドレスを指す。
  // callocはallocとは違い、全てのメモリを０クリアする。
  // ので、初期状態ではtok->next==NULL?
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  LOGGER("new_token: kind=%s, str=%.*s, len=%d, val=%d",
         get_token_name(tok->kind), tok->len, tok->str, tok->len, tok->val);
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  return tok;
}

LVar *find_lvar(Token *tok) {
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  for (LVar *var = locals; var; var = var->next) {
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
  }
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  return NULL;
}

bool startswith(char *p, char *q) { return memcmp(p, q, strlen(q)) == 0; }

int is_alnum(char c) {
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  LOGGER("is_alnum(): %s", &c);
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') ||
         ('0' <= c && c <= '9') || (c == '_');
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize() {
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  char *p = user_input;
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    LOGGER("tokenizing: %s", p);

    // 空白文字をスキップ
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
      cur = new_token(TK_RETURN, cur, p, 6);
      p += 6;
      continue;
    }

    if (strncmp(p, "if", 2) == 0 && !is_alnum(p[2])) {
      cur = new_token(TK_IF, cur, p, 2);
      p += 2;
      continue;
    }

    if (strncmp(p, "else", 4) == 0 && !is_alnum(p[4])) {
      cur = new_token(TK_ELSE, cur, p, 4);
      p += 4;
      continue;
    }

    if (strncmp(p, "while", 5) == 0 && !is_alnum(p[5])) {
      cur = new_token(TK_WHILE, cur, p, 5);
      p += 5;
      continue;
    }

    if (strncmp(p, "for", 3) == 0 && !is_alnum(p[3])) {
      cur = new_token(TK_FOR, cur, p, 3);
      p += 3;
      continue;
    }

    // 記号(二個進める)
    if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") ||
        startswith(p, ">=")) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    // 記号(一つ進める)
    if (strchr("+-*/()<>;={}", *p)) {
      // strchr:
      // 検索対象が見つかればその場所のアドレスを、見つからなければNULLを返す
      cur = new_token(TK_RESERVED, cur, p, 1);
      p++;
      continue;
    }

    if ('a' <= *p && *p <= 'z') {
      char *old_p = p;
      int char_len = 0;
      while (is_alnum(*p)) {
        p++;
        char_len++;
      }
      cur = new_token(TK_IDENT, cur, old_p, char_len);
      cur->len = char_len;
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 0);
      char *q = p;
      cur->val = strtol(p, &p, 10); // str to long
      cur->len = p - q;
      continue;
    }

    error_at(p, "トークナイズできません。");
  }

  new_token(TK_EOF, cur, p, 0);
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  return head.next;
}

// あたらしいノードを作る
Node *new_node(NodeKind kind) {
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  return node;
}

// 新しい二分木を作る？
Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  Node *node = new_node(kind);
  node->lhs = lhs;
  node->rhs = rhs;
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  return node;
}

Node *new_num(int val) {
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  Node *node = new_node(ND_NUM);
  node->val = val;
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  return node;
}

// 文法一覧
// program    = stmt*
// stmt       = expr "; | "return" expr ";"
//            | "if" "(" expr ")" stmt ("else" stmt)?
//            | "while" "(" expr ")" stmt
//            | "for" "(" expr? ";" expr? ";" expr? ")" stmt
//            | "{" stmt* "}"
// expr       = assign
// assign     = equality ("=" assign)?
// equality   = relational ("==" relational | "!=" relational)*
// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
// add        = mul ("+" mul | "-" mul)*
// mul        = unary ("*" unary | "/" unary)*
// unary      = ("+" | "-")? primary
// primary    = num | ident | "(" expr ")"| ident ("(" ")")?

// program    = stmt*
void program() {
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  int i = 0;
  while (!at_eof()) {
    code[i++] = stmt();
    LOGGER("program(): node=%s", get_node_name(code[i - 1]->kind));
  }
  code[i] = NULL;
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
}

// stmt       = expr "; | "return" expr ";"
//            | "if" "(" expr ")" stmt ("else" stmt)?
//            | "while" "(" expr ")" stmt
//            | "for" "(" expr? ";" expr? ";" expr? ")" stmt
//            | "{" stmt* "}"
Node *stmt() {
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  LOGGER("Now tokenizing... %s", get_token_name(token->kind));
  Node *node;
  if (consume_token(TK_RETURN)) {
    node = new_node(ND_RETURN);
    node->lhs = expr();
    expect(";");
  } else if (consume_token(TK_IF)) {
    node = new_node(ND_IF);
    node->cond = expr();
    node->stmt = stmt();
    if (consume_token(TK_ELSE)) {
      node->els = stmt();
    } else {
      node->els = NULL;
    }
  } else if (consume_token(TK_WHILE)) {
    node = new_node(ND_WHILE);
    node->cond = expr();
    node->stmt = stmt();
  } else if (consume_token(TK_FOR)) {
    node = new_node(ND_FOR);
    consume("(");
    if (!consume(";")) {
      node->init = expr();
      expect(";");
    }
    if (!consume(";")) {
      node->cond = expr();
      expect(";");
    }
    if (!consume(")")) {
      node->inc = expr();
      expect(")");
    }
    node->stmt = stmt();
  } else if (consume("{")) { // ブロックを読み込む
    node = new_node(ND_BLOCK);
    Node *last_stmt = NULL;
    while (!consume("}")) {
      Node *tmp_stmt = stmt();
      if (node->stmts) {
        last_stmt->next = tmp_stmt;
      } else {
        node->stmts = tmp_stmt;
      }
      last_stmt = tmp_stmt;
    }
  } else {
    node = expr();
    expect(";");
  }

  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  LOGGER("next token is... %s", get_token_name(token->kind));
  return node;
}

// expr       = assign
Node *expr() { return assign(); }

// assign     = equality ("=" assign)?
Node *assign() {
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  Node *node = equality();
  if (consume("="))
    node = new_binary(ND_ASSIGN, node, assign());
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  return node;
}

// equality = relational ("==" relational | "!=" relational)*
Node *equality() {
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  Node *node = relational();
  for (;;) {
    if (consume("=="))
      node = new_binary(ND_EQ, node, relational());
    else if (consume("!="))
      node = new_binary(ND_NE, node, relational());
    else
      return node;
  }
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
}

// relational = add("<" add | "<=" add | ">" add | ">=" add)*
Node *relational() {
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
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
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
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
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
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
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  if (consume("+"))
    return unary();
  if (consume("-"))
    return new_binary(ND_SUB, new_num(0), unary());
  return primary();
}

// primary    = num | ident | "(" expr ")"
Node *primary() {
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  // 次のトークンが"*"なら、"(" expr ")"のはず
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  // ローカル変# 数があれば読む
  Token *tok = consume_ident();
  if (tok) {
    Node *node = calloc(1, sizeof(Node));
    // もし一つ後のトークンが(なら、identは関数名
    if (consume("(")) {
      node->kind = ND_FUNC;
      node->fname = calloc(1, sizeof(char));
      strncpy(node->fname, tok->str, tok->len);
      expect(")");
    } else {
      // そうでなければローカル変数
      node->kind = ND_LVAR;
      LVar *lvar = find_lvar(tok);
      if (lvar) {
        node->offset = lvar->offset;
      } else {
        lvar = calloc(1, sizeof(LVar));
        lvar->next = locals;
        lvar->name = tok->str;
        lvar->len = tok->len;
        lvar->offset = locals->offset + 8;
        node->offset = lvar->offset;
        locals = lvar;
      }
    }
    return node;
  }
  // そうでなければ数値のはず;
  return new_num(expect_number());
}
