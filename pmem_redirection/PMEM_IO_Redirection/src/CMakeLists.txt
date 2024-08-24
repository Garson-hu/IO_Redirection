# create a library for this subdirectory
add_library(SRC pmem_io_redirection.c)

# if the current mode is redirect or not
option(IO_REDIRECT "if this file shoule be redirected. YES: ON, NO: OFF" ON)


if(IO_REDIRECT)
    target_compile_definitions(SRC PRIVATE "IO_REDIRECT")
endif()

