#include <bfd.h>
#include <stdio.h>


int main()
{
    bfd *abfd;

	bfd_init();

	abfd = bfd_openr("/bin/ls", NULL);

    bfd_close(abfd)

	return 0;
}
