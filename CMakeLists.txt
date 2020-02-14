# CMakeのバージョンを設定
cmake_minimum_required(VERSION 3.15)
# プロジェクト名と使用する言語を設定
project(9cc C)

set(CMAKE_C_STANDARD 11)
# a.outという実行ファイルをmain.cppとhello.cppから作成
add_executable(9cc main.c 9cc.h parse.c tokenize.c codegen.c)

