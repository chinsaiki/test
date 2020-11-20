#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <stdio.h>

int main()
{
	key_t key = ftok("/dev/shm/myshm1", 0);
	int shm_id = shmget(key, 0x400000, 0666);
	char *p = (char*)shmat(shm_id, NULL, 0);

	printf("%c %c %c %c .\n", p[0], p[1], p[2], p[3]);
	shmdt(p);

	return 0;
}
