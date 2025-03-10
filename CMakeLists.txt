cmake_minimum_required(VERSION 3.12)
project(cNES VERSION 0.1.0 LANGUAGES C CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加源代码目录
add_subdirectory(src)