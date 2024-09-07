add_executable(bin1 bin1.c)
add_executable(bin2 bin2.c)
add_executable(bin3 bin3.c ../src/metadata.c)

# use -Wl,-rpath 
set_target_properties(bin1 PROPERTIES BUILD_RPATH ${CMAKE_BINARY_DIR}/lib)
set_target_properties(bin2 PROPERTIES BUILD_RPATH ${CMAKE_BINARY_DIR}/lib)

# link libpreload.so to bin1 and bin2, but not to bin3
target_link_libraries(bin1 ${CMAKE_BINARY_DIR}/lib/libpreload.so)
target_link_libraries(bin2 ${CMAKE_BINARY_DIR}/lib/libpreload.so)


# Specify the include directories
target_include_directories(bin1 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(bin2 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(bin3 PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(CMAKE_BUILD_TYPE Debug)