LIBSO=libadd-test.so

APP=test.out

all:${APP}

${LIBSO}:
	gcc -fpic -shared add.c -o ${LIBSO}
	cp ${LIBSO} /usr/lib64

test:${LIBSO}
	gcc  test.c -o ${APP} -I ./ ${LIBSO}

clean:
	rm -f ${APP} ${LIBSO}