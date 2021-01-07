

all:
	gcc adjtick.c -o adjtick.out
	gcc alarmtimer-suspend.c -lrt -o alarmtimer-suspend.out
	gcc change_skew.c -o change_skew.out
	gcc clocksource-switch.c -o clocksource-switch.out
	gcc freq-step.c -lm -o freq-step.out
	gcc inconsistency-check.c -o inconsistency-check.out
	gcc leap-a-day.c -lrt -o leap-a-day.out
	gcc leapcrash.c -o leapcrash.out
	gcc mqueue-lat.c -lrt -o mqueue-lat.out
	gcc nanosleep.c -o nanosleep.out
	gcc nsleep-lat.c -o nsleep-lat.out
	gcc posix_timers.c -lrt -o posix_timers.out
	gcc raw_skew.c -o raw_skew.out
	gcc rtcpie.c -o rtcpie.out
	gcc set-2038.c -o set-2038.out
	gcc set-tai.c -o set-tai.out
	gcc set-timer-lat.c -lrt -o set-timer-lat.out
	gcc set-tz.c -o set-tz.out
	gcc skew_consistency.c -o skew_consistency.out
	gcc threadtest.c -pthread -o threadtest.out
	gcc valid-adjtimex.c -o valid-adjtimex.out

clean:
	rm -f *.out
