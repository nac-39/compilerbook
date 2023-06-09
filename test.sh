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

assert 0 0
assert 42 42
assert 21 "5+20-4"
# トークナイザー追加
assert 41 " 12 + 34 - 5 "
assert 10 "             5 + 5 - 0"
# 優先度ありの四則演算
assert 47 "5+6*7"
assert 15 "5*(9-6)"
assert 4 "(3+5)/2"
# 単項プラス（unary）追加
assert 10 "-10 + 20"
# 比較演算子追加
assert 0 '0==1'
assert 1 '42==42'
assert 1 '0!=1'
assert 0 '42!=42'

assert 1 '0<1'
assert 0 '1<1'
assert 0 '2<1'
assert 1 '0<=1'
assert 1 '1<=1'
assert 0 '2<=1'

assert 1 '1>0'
assert 0 '1>1'
assert 0 '1>2'
assert 1 '1>=0'
assert 1 '1>=1'
assert 0 '1>=2'

echo OK
