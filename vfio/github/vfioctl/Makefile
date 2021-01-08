all: vfioctl

clean: vfioctl
	rm vfioctl

install: vfioctl
	cp vfioctl /usr/bin/vfioctl
	ln -s /usr/bin/vfioctl /usr/bin/vfio-bind
	ln -s /usr/bin/vfioctl /usr/bin/vfio-unbind
	chown root:kvm /usr/bin/vfioctl
	chmod 4755 /usr/bin/vfioctl

vfioctl: vfioctl.c
	gcc -O3 -Wall -Wextra -Wstrict-prototypes vfioctl.c -o vfioctl
