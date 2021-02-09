/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 06月 14日 星期五 09:05:33 CST. */
/* 
 Copyright (C) Rong Tao @Beijing

 Permission is granted to copy, distribute and/or modify this document
 under the terms of the GNU Free Documentation License, Version 1.3
 or any later version published by the Free Software Foundation;
 with no Invariant Sections, no Front-Cover Texts, and no Back-Cover
 Texts. A copy of the license is included in the section entitled ‘‘GNU
 Free Documentation License’’.
   2019年 03月 14日 星期四 19:24:34 CST. 
*/
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 13日 星期三 08:54:06 CST. */
/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 08日 星期五 08:10:18 CST. */
/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

/* Copyright (C) Rong Tao @Sylincom Beijing, 2019年 03月 07日 星期四 20:27:55 CST. */
/*

struct tm * getdate (const char *string)

The interface to getdate is the simplest possible for a function to parse a string and
return the value. string is the input string and the result is returned in a statically
allocated variable.
The details about how the string is processed are hidden from the user. In fact, they
can be outside the control of the program. Which formats are recognized is controlled
by the fle named by the environment variable DATEMSK. This fle should contain lines
of valid format strings which could be passed to strptime.
The getdate function reads these format strings one after the other and tries to
match the input string. The frst line which completely matches the input string is
used.
Elements not initialized through the format string retain the values present at the
time of the getdate function call.
The formats recognized by getdate are the same as for strptime. See above for an
explanation. There are only a few extensions to the strptime behavior:
• If the %Z format is given the broken-down time is based on the current time of
the timezone matched, not of the current timezone of the runtime environment.
Note: This is not implemented (currently). The problem is that timezone names
are not unique. If a fxed timezone is assumed for a given string (say EST meaning
US East Coast time), then uses for countries other than the USA will fail. So far
we have found no good solution to this.
• If only the weekday is specifed the selected day depends on the current date. If
the current weekday is greater or equal to the tm_wday value the current week’s
day is chosen, otherwise the day next week is chosen.
• A similar heuristic is used when only the month is given and not the year. If the
month is greater than or equal to the current month, then the current year is
used. Otherwise it wraps to next year. The frst day of the month is assumed if
one is not explicitly specifed.
• The current hour, minute, and second are used if the appropriate value is not set
through the format.
• If no date is given tomorrow’s date is used if the time is smaller than the current
time. Otherwise today’s date is taken.
It should be noted that the format in the template fle need not only contain format
elements. The following is a list of possible format strings (taken from the Unix
standard):
%m
%A %B %d, %Y %H:%M:%S
%A

%B
%m/%d/%y %I %p
%d,%m,%Y %H:%M
at %A the %dst of %B in %Y
run job at %I %p,%B %dnd
%A den %d. %B %Y %H.%M Uhr
As you can see, the template list can contain very specifc strings like run job at %I
%p,%B %dnd. Using the above list of templates and assuming the current time is Mon
Sep 22 12:19:47 EDT 1986 we can obtain the following results for the given input.
Input Match Result
Mon %a Mon Sep 22 12:19:47 EDT 1986
Sun %a Sun Sep 28 12:19:47 EDT 1986
Fri %a Fri Sep 26 12:19:47 EDT 1986
September %B Mon Sep 1 12:19:47 EDT 1986
January %B Thu Jan 1 12:19:47 EST 1987
December %B Mon Dec 1 12:19:47 EST 1986
Sep Mon %b %a Mon Sep 1 12:19:47 EDT 1986
Jan Fri %b %a Fri Jan 2 12:19:47 EST 1987
Dec Mon %b %a Mon Dec 1 12:19:47 EST 1986
Jan Wed 1989 %b %a %Y Wed Jan 4 12:19:47 EST 1989
Fri 9 %a %H Fri Sep 26 09:00:00 EDT 1986
Feb 10:30 %b %H:%S Sun Feb 1 10:00:30 EST 1987
10:30 %H:%M Tue Sep 23 10:30:00 EDT 1986
13:30 %H:%M Mon Sep 22 13:30:00 EDT 1986
The return value of the function is a pointer to a static variable of type struct tm,
or a null pointer if an error occurred. The result is only valid until the next getdate
call, making this function unusable in multi-threaded applications.
The errno variable is not changed. Error conditions are stored in the global variable
getdate_err. See the description above for a list of the possible error values.
Warning: The getdate function should never be used in SUID-programs. The reason
is obvious: using the DATEMSK environment variable you can get the function to open
any arbitrary fle and chances are high that with some bogus input (such as a binary
fle) the program will crash.


*/
#include <stdio.h>
#include <time.h>
#include <string.h>

struct tm * getdate (const char *string);
int getdate_r (const char *string, struct tm *tp);


int main()
{

    
    struct tm *t2 = getdate("Thursday Thu January Jan 19 Thu Jan  1 08:00:01 1970 %");
    
    // printf("tm_sec = %d:\n", t2->tm_sec);
    // printf("tm_min = %d:\n", t2->tm_min);
    // printf("tm_hour = %d:\n", t2->tm_hour);
    // printf("tm_mday = %d:\n", t2->tm_mday);
    // printf("tm_mon = %d:\n", t2->tm_mon);
    // printf("tm_year = %d:\n", t2->tm_year);
    // printf("tm_wday = %d:\n", t2->tm_wday);
    // printf("tm_yday = %d:\n", t2->tm_yday);
    // printf("tm_isdst = %d:\n", t2->tm_isdst);
    // printf("tm_gmtoff = %d:\n", t2->tm_gmtoff);
    // printf("tm_zone = %s:\n", t2->tm_zone);
    
    return 0;
}