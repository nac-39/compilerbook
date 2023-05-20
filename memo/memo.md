## C とそれに対応するアセンブラ

test1.c の逆アセンブルの結果（./assembly/test1.s）が本に載ってるやつと一致しないんだけどこれはいいの？

```s
.intel_syntax noprefix
.globl main
main:
        mov rax, 42
        ret
```

## C 言語

- strtol: [https://marycore.jp/prog/c-lang/stdlib-strtol/](https://marycore.jp/prog/c-lang/stdlib-strtol/)

- enum, typedef: [https://yu-nix.com/archives/c-enum-typedef/](https://yu-nix.com/archives/c-enum-typedef/)

- struct: [https://www.cc.kyoto-su.ac.jp/~yamada/programming/struct.html](https://www.cc.kyoto-su.ac.jp/~yamada/programming/struct.html)

- calloc: 指定バイト分メモリ領域を n 個分確保する。
  [http://www9.plala.or.jp/sgwr-t/lib/calloc.html](http://www9.plala.or.jp/sgwr-t/lib/calloc.html)

- vfprintf: 
        フォーマットした文字列をファイルストリームに書き込む。置換文字列を配列として受け取る。ダイサン引数に置換文字列を渡す。
        vはvectorのvか。
        [http://note.onichannn.net/archives/661](http://note.onichannn.net/archives/661)