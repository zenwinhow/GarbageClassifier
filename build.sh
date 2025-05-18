#!/bin/bash

# 创建 build 目录（如果不存在）
if [ ! -d "build" ]; then
    mkdir build
fi

# 进入 build 目录
cd build

# 生成 Makefile
cmake ..

# 构建（使用所有 CPU 核心）
make -j$(nproc)
