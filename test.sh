#!/bin/bash

echo "test実行"
try() {
    exp="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    gcc -o tmp tmp.s
    ./tmp
    act="$?"

    if [ "$act" = "$exp" ]; then
        echo "テスト成功: $input => $act"
    else
        echo "テスト失敗: $input => $exp を期待しいましたが、 $act になっています"
    fi
}

try 1 1
try 21 "5+20-4"
try 41 " 12 + 34 - 5 "
try 47 '5+6*7'
try 15 '5*(9-6)'
try 4 '(3+5)/2'
try 10 '-10+20'
try 10 '- -10'
try 10 '- - +10'

try 0 '0==1'
try 1 '42==42'
try 1 '0!=1'
try 0 '42!=42'

try 1 '0<1'
try 0 '1<1'
try 0 '2<1'
try 1 '0<=1'
try 1 '1<=1'
try 0 '2<=1'

try 1 '1>0'
try 0 '1>1'
try 0 '1>2'
try 1 '1>=0'
try 1 '1>=1'
try 0 '1>=2'

try 11 '(1>0) + 10'

echo "test完了"