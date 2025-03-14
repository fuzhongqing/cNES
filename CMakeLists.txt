cmake_minimum_required(VERSION 3.12)
project(cNES VERSION 0.1.0 LANGUAGES C CXX)

# 设置默认构建类型为Debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# 设置调试和发布模式的编译标志
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置SDL2查找路径
set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};/usr/local/lib/cmake/SDL2")

# 设置SDL2包含路径
include_directories("/usr/local/include")

# 查找SDL2包
find_package(SDL2 REQUIRED)

# 添加源代码目录
add_subdirectory(src)
