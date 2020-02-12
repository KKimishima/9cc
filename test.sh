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

echo "test完了"