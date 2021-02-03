# uptime
Minimal uptime that doesn't require `/proc` and uses `sysinfo(2)`

## Example output
```
$ ./uptime 
Booted on Thursday, January 17 2019, at 10:09:30 AM
Up 1 weeks, 2 days, 6 hours, 2 minutes, and 37 seconds
```

## Motivation
I needed to use it on an old target system that didn't have `/proc`. It was not
worth the hassle of getting the GNU `uptime` dependencies working.

