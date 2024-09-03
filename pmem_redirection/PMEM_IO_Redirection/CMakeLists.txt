cmake_minimum_required(VERSION 3.26.0)

# set the project name and version
project(pmem_io_redirection VERSION 0.1
        DESCRIPTION "Used for PMEM IO Redirection Project"
        LANGUAGES C CXX)

# Some code (for now, all) the code are write by C
set(CMAKE_C_STANDARD 11)
set(C_STD_REQUIRED True)

# add executable variable
add_executable(pmem_io_redirection test/bin1.c test/bin2.c)

# add src directory
add_subdirectory(src)
add_subdirectory(test)

# link the target to subdirectory
target_link_libraries(pmem_io_redirection PUBLIC preload dl)
# target_link_libraries(pmem_io_redirection PUBLIC TEST)

# find the head files
target_include_directories(pmem_io_redirection PUBLIC 
                            "${PROJECT_BINARY_DIR}"
                            "${PROJECT_SOURCE_DIR}/src" )

