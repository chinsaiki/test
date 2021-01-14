#include <unistd.h>
#include <syscall.h>

void my_sync() {
	syscall(SYS_sync);
}

int main() {
	my_sync();

	return 0;
}
