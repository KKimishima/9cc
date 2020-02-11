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

echo "test完了"