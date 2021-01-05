LIBSO=libadd-test.so

APP=test.out

all:${APP}

${LIBSO}:
	gcc -fpic -shared add.c -o ${LIBSO}
	cp ${LIBSO} /usr/lib64

${APP}:${LIBSO}
	gcc  test.c -o ${APP} -I ./ -L $(shell pwd) -ladd-test

clean:
	rm -f ${APP} ${LIBSO}
