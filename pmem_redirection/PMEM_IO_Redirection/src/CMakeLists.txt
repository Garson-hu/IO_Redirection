# create a library for this subdirectory
add_library(preload SHARED preload.c metadata.c)

target_link_libraries(preload dl)

# target_include_directories(preload PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(preload PROPERTIES COMPILE_FLAGS "-g")
set_target_properties(preload PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
