#!/bin/bash
make
assert() {
  expected="$1"
  input="$2"

  ./bin/9cc "$input" > ./assembly/tmp.s
  cc -o ./bin/tmp ./assembly/tmp.s
  ./bin/tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
# トークナイザー追加
assert 41 " 12 + 34 - 5 ;"
assert 10 "             5 + 5 - 0;"
# # 優先度ありの四則演算
assert 47 "5+6*7;"
assert 15 "5*(9-6);"
assert 4 "(3+5)/2;"
# 単項プラス（unary）追加
assert 10 "-10 + 20;"
# # 比較演算子追加
assert 0 '0==1;'
assert 1 '42==42;'
assert 1 '0!=1;'
assert 0 '42!=42;'

assert 1 '0<1;'
assert 0 '1<1;'
assert 0 '2<1;'
assert 1 '0<=1;'
assert 1 '1<=1;'
assert 0 '2<=1;'

assert 1 '1>0;'
assert 0 '1>1;'
assert 0 '1>2;'
assert 1 '1>=0;'
assert 1 '1>=1;'
assert 0 '1>=2;'

# ローカル変数（一文字)の導入
assert 1 'a=1; return a;'
assert 100 'a=100; return a;'
assert 1 'a=1; a;'
assert 3 'a=1;b=2;return a+b;'
assert 4 'a=1;b=0; return (a+a)*(a+a)+b;'
assert 1 'a=1;b=0; return a>b;'

# ローカル変数（複数文字）の導入
assert 2 'apple=2; return apple;'
assert 1 'apple=1; apple;'
assert 3 'apple=1;banana=2;return apple+banana;'
assert 0 'apple=1;banana=0; return (apple+apple)*banana;'
assert 1 'apple=1;banana=0; return apple>banana;'

# if文の追加
assert 1 'if(1) return 1;'
# assert 2 'if(1<2) return 3;'
assert 2 'if(1>2) return 3;return 2;'
echo OK
