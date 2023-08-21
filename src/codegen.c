#include "9cc.h"

void gen_lval(Node *node) {
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  if (node->kind != ND_LVAR)
    error("代入の左辺値が変数ではありません");
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void gen(Node *node) {
  LOGGER("%s:l%d  %s()", __FILE__, __LINE__, __func__);
  LOGGER("> %s", get_node_name(node->kind));
  if (node->kind == ND_RETURN) {
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  }
  if (node->kind == ND_IF) {
    gen(node->lhs);
    printf("  pop rax\n"); // スタックトップに結果が入っているはず
    printf("  cmp rax, 0\n");
    char buf[24];
    snprintf(buf, 24, "%d", label_index++);
    if (node->els) {
      printf("  je .Lelse%s\n", buf);
    } else {
      printf("  je .Lend%s\n", buf);
    }
    gen(node->rhs);
    if (node->els) {
      printf("  .Lelse%s:\n", buf);
      gen(node->els);
    }
    printf(".Lend%s:\n", buf);
    return;
  }
  if (node->kind == ND_WHILE) {
    char buf[24];
    snprintf(buf, 24, "%d", label_index++);
    printf(".Lbegin%s:\n", buf);
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%s\n", buf);
    gen(node->rhs);
    printf("  jmp .Lbegin%s\n", buf);
    printf(".Lend%s:\n", buf);
    return;
  }
  // 終端記号
  switch (node->kind) {
  case ND_LVAR:
    gen_lval(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  case ND_ASSIGN:
    gen_lval(node->lhs);
    gen(node->rhs);
    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
    return;
  case ND_NUM:
    printf("  push %d\n", node->val);
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
  case ND_ADD:
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    printf("  sub rax, rdi\n");
    break;
  case ND_MUL:
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
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

  printf("  push rax\n");
}
