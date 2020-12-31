详情请查看文档

https://github.com/Rtoax/MoreThanDocs/tree/master/%E5%87%BD%E6%95%B0%E5%BA%93%E6%89%8B%E5%86%8C/valgrind
https://github.com/Rtoax/MoreThanDocs/tree/master/%E5%87%BD%E6%95%B0%E5%BA%93%E6%89%8B%E5%86%8C/valgrind/html
-----------------------------------------------

# Preparing your program

Compile your program with -g to include debugging information so that Memcheck's error messages include exact line numbers. Using -O0 is also a good idea, if you can tolerate the slowdown. With -O1 line numbers in error messages can be inaccurate, although generally speaking running Memcheck on code compiled at -O1 works fairly well, and the speed improvement compared to running -O0 is quite significant. Use of -O2 and above is not recommended as Memcheck occasionally reports uninitialised-value errors which don't really exist.
