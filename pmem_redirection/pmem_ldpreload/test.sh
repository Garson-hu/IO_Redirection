#!/bin/bash

# 设定你的共享库路径
LIB_PATH="./libredirect.so"
TEST_FILE="/data/redirect_files/testfile"
REDIRECT_PATH="$TEST_FILE"

# 创建测试文件的目录（如果不存在）
mkdir -p $(dirname $TEST_FILE)

# 导出环境变量
export LD_PRELOAD=$LIB_PATH
export REDIRECT_PATH

# 运行测试程序
./main

# 清除LD_PRELOAD设置
unset LD_PRELOAD
unset REDIRECT_PATH
