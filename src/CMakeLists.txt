# 添加所有源文件
set(SOURCES
    main.cpp
)

# 创建可执行文件
add_executable(cnes ${SOURCES})

# 链接SDL2库
target_link_libraries(cnes PRIVATE SDL2::SDL2)