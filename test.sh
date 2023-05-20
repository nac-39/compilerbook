#!/bin/bash
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

echo OK