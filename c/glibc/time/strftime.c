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

size_t strftime (char *s, size t size, const char *template, const [Function]
struct tm *brokentime)
This function is similar to the sprintf function (see Section 12.14 [Formatted Input],
page 266), but the conversion specifcations that can appear in the format template
template are specialized for printing components of the date and time brokentime
according to the locale currently specifed for time conversion (see Chapter 7 [Locales
and Internationalization], page 150).
Ordinary characters appearing in the template are copied to the output string s; this
can include multibyte character sequences. Conversion specifers are introduced by a
‘%’ character, followed by an optional ﬂag which can be one of the following. These
ﬂags are all GNU extensions. The frst three aﬀect only the output of numbers:
_ The number is padded with spaces.
- The number is not padded at all.
0 The number is padded with zeros even if the format specifes padding
with spaces.
^ The output uses uppercase characters, but only if this is possible (see
Section 4.2 [Case Conversion], page 67).
The default action is to pad the number with zeros to keep it a constant width.
Numbers that do not have a range indicated below are never padded, since there is
no natural width for them.
Following the ﬂag an optional specifcation of the width is possible. This is specifed
in decimal notation. If the natural size of the output is of the feld has less than the
specifed number of characters, the result is written right adjusted and space padded
to the given size.
An optional modifer can follow the optional ﬂag and width specifcation. The modi
fers, which were frst standardized by POSIX.2-1992 and by ISO C99, are:
E Use the locale’s alternate representation for date and time. This modifer
applies to the %c, %C, %x, %X, %y and %Y format specifers. In a Japanese
locale, for example, %Ex might yield a date format based on the Japanese
Emperors’ reigns.
O Use the locale’s alternate numeric symbols for numbers. This modifer
applies only to numeric format specifers.
If the format supports the modifer but no alternate representation is available, it is
ignored.
The conversion specifer ends with a format specifer taken from the following list.
The whole ‘%’ sequence is replaced in the output string as follows:
%a The abbreviated weekday name according to the current locale.
%A The full weekday name according to the current locale.
%b The abbreviated month name according to the current locale.
%B The full month name according to the current locale.
Using %B together with %d produces grammatically incorrect results for
some locales.

%c The preferred calendar time representation for the current locale.
%C The century of the year. This is equivalent to the greatest integer not
greater than the year divided by 100.
This format was frst standardized by POSIX.2-1992 and by ISO C99.
%d The day of the month as a decimal number (range 01 through 31).
%D The date using the format %m/%d/%y.
This format was frst standardized by POSIX.2-1992 and by ISO C99.
%e The day of the month like with %d, but padded with blank (range 1
through 31).
This format was frst standardized by POSIX.2-1992 and by ISO C99.
%F The date using the format %Y-%m-%d. This is the form specifed in the
ISO 8601 standard and is the preferred form for all uses.
This format was frst standardized by ISO C99 and by POSIX.1-2001.
%g The year corresponding to the ISO week number, but without the century
(range 00 through 99). This has the same format and value as %y, except
that if the ISO week number (see %V) belongs to the previous or next
year, that year is used instead.
This format was frst standardized by ISO C99 and by POSIX.1-2001.
%G The year corresponding to the ISO week number. This has the same
format and value as %Y, except that if the ISO week number (see %V)
belongs to the previous or next year, that year is used instead.
This format was frst standardized by ISO C99 and by POSIX.1-2001 but
was previously available as a GNU extension.
%h The abbreviated month name according to the current locale. The action
is the same as for %b.
This format was frst standardized by POSIX.2-1992 and by ISO C99.
%H The hour as a decimal number, using a 24-hour clock (range 00 through
23).
%I The hour as a decimal number, using a 12-hour clock (range 01 through
12).
%j The day of the year as a decimal number (range 001 through 366).
%k The hour as a decimal number, using a 24-hour clock like %H, but padded
with blank (range 0 through 23).
This format is a GNU extension.
%l The hour as a decimal number, using a 12-hour clock like %I, but padded
with blank (range 1 through 12).
This format is a GNU extension.
%m The month as a decimal number (range 01 through 12).
%M The minute as a decimal number (range 00 through 59).
%n A single ‘\n’ (newline) character.
This format was frst standardized by POSIX.2-1992 and by ISO C99.
%p Either ‘AM’ or ‘PM’, according to the given time value; or the corresponding
strings for the current locale. Noon is treated as ‘PM’ and midnight as
‘AM’. In most locales ‘AM’/‘PM’ format is not supported, in such cases "%p"
yields an empty string.
%P Either ‘am’ or ‘pm’, according to the given time value; or the corresponding
strings for the current locale, printed in lowercase characters. Noon is
treated as ‘pm’ and midnight as ‘am’. In most locales ‘AM’/‘PM’ format is
not supported, in such cases "%P" yields an empty string.
This format is a GNU extension.
%r The complete calendar time using the AM/PM format of the current
locale.
This format was frst standardized by POSIX.2-1992 and by ISO C99. In
the POSIX locale, this format is equivalent to %I:%M:%S %p.
%R The hour and minute in decimal numbers using the format %H:%M.
This format was frst standardized by ISO C99 and by POSIX.1-2001 but
was previously available as a GNU extension.
%s The number of seconds since the epoch, i.e., since 1970-01-01 00:00:00
UTC. Leap seconds are not counted unless leap second support is avail
able.
This format is a GNU extension.
%S The seconds as a decimal number (range 00 through 60).
%t A single ‘\t’ (tabulator) character.
This format was frst standardized by POSIX.2-1992 and by ISO C99.
%T The time of day using decimal numbers using the format %H:%M:%S.
This format was frst standardized by POSIX.2-1992 and by ISO C99.
%u The day of the week as a decimal number (range 1 through 7), Monday
being 1.
This format was frst standardized by POSIX.2-1992 and by ISO C99.
%U The week number of the current year as a decimal number (range 00
through 53), starting with the frst Sunday as the frst day of the frst
week. Days preceding the frst Sunday in the year are considered to be
in week 00.
%V The ISO 8601:1988 week number as a decimal number (range 01 through
53). ISO weeks start with Monday and end with Sunday. Week 01 of a
year is the frst week which has the majority of its days in that year; this
is equivalent to the week containing the year’s frst Thursday, and it is
also equivalent to the week containing January 4. Week 01 of a year can
contain days from the previous year. The week before week 01 of a year
is the last week (52 or 53) of the previous year even if it contains days
from the new year.
This format was frst standardized by POSIX.2-1992 and by ISO C99.
%w The day of the week as a decimal number (range 0 through 6), Sunday
being 0.
%W The week number of the current year as a decimal number (range 00
through 53), starting with the frst Monday as the frst day of the frst
week. All days preceding the frst Monday in the year are considered to
be in week 00.
%x The preferred date representation for the current locale.
%X The preferred time of day representation for the current locale.
%y The year without a century as a decimal number (range 00 through 99).
This is equivalent to the year modulo 100.
%Y The year as a decimal number, using the Gregorian calendar. Years before
the year 1 are numbered 0, -1, and so on.
%z RFC 822/ISO 8601:1988 style numeric time zone (e.g., -0600 or +0100),
or nothing if no time zone is determinable.
This format was frst standardized by ISO C99 and by POSIX.1-2001 but
was previously available as a GNU extension.
In the POSIX locale, a full RFC 822 timestamp is generated
by the format ‘"%a, %d %b %Y %H:%M:%S %z"’ (or the equivalent
‘"%a, %d %b %Y %T %z"’).
%Z The time zone abbreviation (empty if the time zone can’t be determined).
%% A literal ‘%’ character.




*/
#include <stdio.h>
#include <time.h>


size_t strftime (char *s, size_t size, const char *template, const struct tm *brokentime);

int main()
{
#if 1
    time_t time = 1;
    struct tm *t = localtime(&time);
    
    struct tm *t2;
    t2 = localtime_r(&time, t2);
    
    printf("tm_sec = %d:%d\n", t->tm_sec, t2->tm_sec);
    printf("tm_min = %d:%d\n", t->tm_min, t2->tm_min);
    printf("tm_hour = %d:%d\n", t->tm_hour, t2->tm_hour);
    printf("tm_mday = %d:%d\n", t->tm_mday, t2->tm_mday);
    printf("tm_mon = %d:%d\n", t->tm_mon, t2->tm_mon);
    printf("tm_year = %d:%d\n", t->tm_year, t2->tm_year);
    printf("tm_wday = %d:%d\n", t->tm_wday, t2->tm_wday);
    printf("tm_yday = %d:%d\n", t->tm_yday, t2->tm_yday);
    printf("tm_isdst = %d:%d\n", t->tm_isdst, t2->tm_isdst);
    printf("tm_gmtoff = %d:%d\n", t->tm_gmtoff, t2->tm_gmtoff);
    printf("tm_zone = %s:%s\n", t->tm_zone, t2->tm_zone);
    
    
    printf("----------------------\n");
    char str[1024];
    // strftime(str, 1024, 
            // "%A %a %B %b %C %c \n"\
            // "%D %d %E %e %F %f \n"\
            // "%G %g %H %h %I %i \n"\
            // "%J %j %K %k \n"\
            // "%L %l %M %m %N %n \n"\
            // "%O %o %P %p %Q %q \n"\
            // "%R %r %S %s %T %t \n"\
            // "%U %u %V %v %W %w \n"\
            // "%X %x %Y %y %Z %z \n",
            // t2);
    strftime(str, 1024, 
            "%A %a %B %b %C %c %%\n",
            t2);
    printf("%s\n", str);
    
    struct tm t3;
    strptime(str, 
            "%A %a %B %b %C %c %%\n",
            &t3);

            
    
    printf("tm_sec = %d:%d\n", t->tm_sec, t3.tm_sec);
    printf("tm_min = %d:%d\n", t->tm_min, t3.tm_min);
    printf("tm_hour = %d:%d\n", t->tm_hour, t3.tm_hour);
    printf("tm_mday = %d:%d\n", t->tm_mday, t3.tm_mday);
    printf("tm_mon = %d:%d\n", t->tm_mon, t3.tm_mon);
    printf("tm_year = %d:%d\n", t->tm_year, t3.tm_year);
    printf("tm_wday = %d:%d\n", t->tm_wday, t3.tm_wday);
    printf("tm_yday = %d:%d\n", t->tm_yday, t3.tm_yday);
    printf("tm_isdst = %d:%d\n", t->tm_isdst, t3.tm_isdst);
    printf("tm_gmtoff = %d:%d\n", t->tm_gmtoff, t3.tm_gmtoff);
    printf("tm_zone = %s:%s\n", t->tm_zone, t3.tm_zone);

#endif

    printf("----------------------\n");
    /* 将字符串转化为 tm 结构 */
    struct tm t4, *tp;
    tp = &t4;
    memset(&t4, 0, sizeof(t4));
    strptime("335-19:01:43", "%j-%H:%M:%S\n", &t4);
    
    printf("tm_sec = %d:%d\n", tp->tm_sec, t4.tm_sec);
    printf("tm_min = %d:%d\n", tp->tm_min, t4.tm_min);
    printf("tm_hour = %d:%d\n", tp->tm_hour, t4.tm_hour);
    printf("tm_mday = %d:%d\n", tp->tm_mday, t4.tm_mday);
    printf("tm_mon = %d:%d\n", tp->tm_mon, t4.tm_mon);
    printf("tm_year = %d:%d\n", tp->tm_year, t4.tm_year);
    printf("tm_wday = %d:%d\n", tp->tm_wday, t4.tm_wday);
    printf("tm_yday = %d:%d\n", tp->tm_yday, t4.tm_yday);
    printf("tm_isdst = %d:%d\n", tp->tm_isdst, t4.tm_isdst);
    printf("tm_gmtoff = %d:%d\n", tp->tm_gmtoff, t4.tm_gmtoff);
    printf("tm_zone = %s:%s\n", tp->tm_zone, t4.tm_zone);
    
    return 0;
}