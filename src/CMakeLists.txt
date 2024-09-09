# 添加共享库 preload
add_library(preload SHARED preload.c)

# 链接到 dl 库以支持动态链接
target_link_libraries(preload dl)

# 设置共享库的输出路径
set_target_properties(preload PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
