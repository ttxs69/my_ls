# my_ls

My solution of [missing-semester: lecture 2](https://missing.csail.mit.edu/2020/shell-tools/)

## Complile

```shell
mkdir build && cd build
cmake ..
cmake --build .
```

## Usage

```shell
./my_ls <dir>
```

## example

```shell
$./my_ls .
```

output:

```shell
drwxr-xr-x@  9 sarace  staff   288B May 04 15:54 ..
drwxr-xr-x@ 11 sarace  staff   352B May 04 15:53 .
-rwxr-xr-x   1 sarace  staff 124.0K May 04 15:52 my_ls
-rw-r--r--@  1 sarace  staff 226.3K May 04 15:52 .ninja_deps
-rw-r--r--@  1 sarace  staff   4.2K May 04 15:52 .ninja_log
drwxr-xr-x@ 11 sarace  staff   352B May 04 10:01 CMakeFiles
-rw-r--r--@  1 sarace  staff  14.1K May 04 10:01 CMakeCache.txt
-rw-r--r--   1 sarace  staff  21.7K May 04 10:01 build.ninja
-rw-r--r--   1 sarace  staff   471B May 04 10:01 compile_commands.json
drwxr-x---   3 sarace  staff    96B May 04 09:51 .cache
-rw-r--r--@  1 sarace  staff   1.5K Apr 20 15:52 cmake_install.cmake
```
