# PMEM Redirection

Originally: ld-preload-practice

Goal: I/O Redirection of Workflows to Local and Disaggregate Persistent Memory

- Try running `test.sh` in each directory for some `LD_PRELOAD` magic

##  Installation

If you have root access, you might want to use your favorite package manager for these libraries/packages, with the exception of maybe installing `libpmem2` from source if it's not offered for your system.

This project was built on [NCSU ARC Cluster](https://arcb.csc.ncsu.edu/~mueller/cluster/arc/), which uses CentOS Linux 7 (Core) x86_64. You might need to add some needed dependancies that don't exist on your system. Try `cat fetch.txt` for more system information.

### [Memkind Library](https://github.com/memkind/memkind)
See [memkind#building-and-installing](https://github.com/memkind/memkind#building-and-installing).

Because I don't have root permissions, I am installing in `$HOME/.local`.
```
./autogen.sh
./configure --prefix=$HOME/.local
make
make install
```

You'd also want to add the following to your `.bashrc`:
```
# man memkind
export MANPATH=$MANPATH:$HOME/.local/share/man

# For some reason, this is needed in addition to `-L`
export LD_LIBRARY_PATH=$HOME/.local/lib

# TODO: maybe do the same for #include and ld's lib, instead of `-I` and `-L` respectively
```

You can choose to install it at a system level, or to another directory. If you install at a system level, you probably don't need to modify the scripts to account for this change. If you're curious to see where `.local` is hardcoded, you can run the following command:

```
grep -r '[.]local'
```

### [Persistent Memory Development Kit (PMDK)](https://pmem.io/pmdk/)

> The Persistent Memory Development Kit (PMDK) is a growing collection of libraries and tools. Tuned and validated on both Linux and Windows, the libraries build on the DAX (Direct Access) feature of those operating systems which allows applications to access persistent memory as memory-mapped files, as described in the SNIA NVM Programming Model.

See [Installing PMDK](https://docs.pmem.io/persistent-memory/getting-started-guide/installing-pmdk/). I went with [Installing PMDK using Linux Packages](https://docs.pmem.io/persistent-memory/getting-started-guide/installing-pmdk/installing-pmdk-using-linux-packages). If you need libpmem2, you should use [Installing PMDK from Source on Linux](https://docs.pmem.io/persistent-memory/getting-started-guide/installing-pmdk/compiling-pmdk-from-source).

## Debugging

If you have root access, you might want to use your favorite package manager for these too.

I was debating using [Homebrew](https://en.wikipedia.org/wiki/Homebrew_(package_manager)) since it can technically streamline installing from source, even without initally having root access, but is not officially supported (and [faced some issues](https://github.com/orgs/Homebrew/discussions?discussions_q=author%3Aosalbahr+CentOS)). I also just wanted to try out installing from source more anyways.

### [ltrace](https://en.wikipedia.org/wiki/Ltrace)

For debugging a ["realloc error"](https://github.com/osalbahr/ld-preload-practice/commit/f960fd33ded086b86eb7c23222dba63567f96b96), I wanted to have `ltrace` as follows. See their [INSTALL](https://gitlab.com/cespedes/ltrace/-/blob/main/INSTALL) for instructions.

```
git clone https://gitlab.com/cespedes/ltrace.git
cd ltrace
./autogen.sh
./configure --prefix=$HOME/.local
make
make install

# You can also use
./autogen.sh && ./configure --prefix=$HOME/.local && make && make install
```

### [Valgrind](https://en.wikipedia.org/wiki/Valgrind)

To further investigate the memory error, I wanted to have `valgrind`. See https://valgrind.org/downloads
```
curl -O https://sourceware.org/pub/valgrind/valgrind-3.21.0.tar.bz2
cd valgrind-3.21.0
./configure --prefix=$HOME/.local
make
make install

# You can also use
curl -O https://sourceware.org/pub/valgrind/valgrind-3.21.0.tar.bz2 && cd valgrind-3.21.0 && ./configure --prefix=$HOME/.local && make && make install
```

### [GEF (GDB Enhanced Features)](https://github.com/hugsy/gef)

Since `gdb` rpm's are outdated, I ended up needing to `spack install gdb && spack load gdb`. I initially installed `gdb` from source, but had glibc version issues and thought I'd use a tool that automates the whole process, [`spack`](https://github.com/spack/spack).
