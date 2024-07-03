CC=gcc
CFLAGS=-fPIC -Wall
LDFLAGS=-shared
RM=rm -f
TARGET_LIB=libpmem_io_redirection.so

# 默认构建目标
all: $(TARGET_LIB) main

# 构建共享库
$(TARGET_LIB): pmem_io_redirection.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET_LIB) pmem_io_redirection.c -ldl

# 构建测试程序
test: main.c
	$(CC) -Wall -Wextra main.c -o main

# 清理构建文件
clean:
	$(RM) $(TARGET_LIB) main
