#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <bfd.h>

//调试的程序需要有自定义的断点

#if 0
#include <stdio.h>
    
#if __SIZEOF_POINTER__ > 4
#define RTOAX_BREAKPOINT_PTR ".quad"
#else
#define RTOAX_BREAKPOINT_PTR ".long"
#endif
#define RTOAX_BREAKPOINT \
    asm("0:"                              \
        ".pushsection embed-breakpoints;" \
        RTOAX_BREAKPOINT_PTR" 0b;"       \
        ".popsection;")

int main() {
    printf("Hello,\n");
    RTOAX_BREAKPOINT;
    printf("world!\n");
    RTOAX_BREAKPOINT;
    return 0;
}
#endif



#define die(msg, ...) \
    do { fprintf(stderr, msg, ## __VA_ARGS__); exit(1); } while (0)

#define errno_die(msg) \
    do { perror(msg); exit(1); } while(0)

#define bfd_die(msg) \
    do { bfd_perror(msg); exit(1); } while(0)


int main(int argc, char *argv[]) {
    if (argc < 2)
        die("Usage: %s <other gdb args> ... <program>\n", argv[0]);

    size_t i;

    // Get contents of the embed-breakpoints section

    bfd *abfd = bfd_openr(argv[argc-1], NULL);
    if (!abfd)
        bfd_die("bfd_openr");

    // We must call this to load the data!
    if (!bfd_check_format(abfd, bfd_object))
        bfd_die("bfd_check_format");

    asection *sec = bfd_get_section_by_name(abfd, "embed-breakpoints");
    if (!sec)
        bfd_die("bfd_get_section_by_name");

    size_t contents_size = sec->size;
    char *contents = malloc(contents_size);
    if (!contents)
        die("Could not allocate contents\n");

    if (!bfd_get_section_contents(abfd, sec, contents, 0, contents_size))
        bfd_die("bfd_get_section_contents");

    if (!bfd_close(abfd))
        bfd_die("bfd_close");


    for (i = 0; i < contents_size; i += sizeof(void*)) {
        fprintf(stderr, "break *%p\n", *( (void**) (contents+i) ));
    }
}
