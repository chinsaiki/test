
# Setting Default Options

Note that Valgrind also reads options from three places:
1. The file ~/.valgrindrc
2. The environment variable $VALGRIND_OPTS
3. The file ./.valgrindrc

Any tool-specific options put in $VALGRIND_OPTS or the .valgrindrc files should be prefixed with the tool name and a colon. For example, if you want Memcheck to always do leak checking, you can put the following entry in `~/.valgrindrc`:

```
--memcheck:leak-check=yes
```

