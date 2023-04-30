## C とそれに対応するアセンブラ

test1.c の逆アセンブルの結果（./assembly/test1.s）が本に載ってるやつと一致しないんだけどこれはいいの？

```s
.intel_syntax noprefix
.globl main
main:
        mov rax, 42
        ret
```
