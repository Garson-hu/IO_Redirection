# 编译 bin1 和 bin2 测试程序
add_executable(bin1 bin1.c)
add_executable(bin2 bin2.c)
add_executable(bin3 bin3.c)

# 设置 RPATH，确保运行时可以找到共享库
set_target_properties(bin1 PROPERTIES BUILD_RPATH ${CMAKE_BINARY_DIR}/lib)
set_target_properties(bin2 PROPERTIES BUILD_RPATH ${CMAKE_BINARY_DIR}/lib)
set_target_properties(bin3 PROPERTIES BUILD_RPATH ${CMAKE_BINARY_DIR}/lib)

# 链接共享库 libpreload.so 到 bin1 和 bin2
target_link_libraries(bin1 ${CMAKE_BINARY_DIR}/lib/libpreload.so)
target_link_libraries(bin2 ${CMAKE_BINARY_DIR}/lib/libpreload.so)
target_link_libraries(bin3 ${CMAKE_BINARY_DIR}/lib/libpreload.so)
