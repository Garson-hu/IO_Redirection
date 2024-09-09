cmake_minimum_required(VERSION 3.10)

# 项目信息
project(pmem_io_redirection VERSION 1.0
        DESCRIPTION "PMEM IO Redirection using LD_PRELOAD"
        LANGUAGES C)

# 设置C标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# 子目录处理
add_subdirectory(src)
add_subdirectory(test)
