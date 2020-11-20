EXAMPLES

------------------------
Below are shown two example programs that demonstrate the use of
memfd_create() and the file sealing API.

The first program, t_memfd_create.c, creates a tmpfs(5) file using
memfd_create(), sets a size for the file, maps it into memory, and
optionally places some seals on the file.  The program accepts up to
three command-line arguments, of which the first two are required.
The first argument is the name to associate with the file, the second
argument is the size to be set for the file, and the optional third
argument is a string of characters that specify seals to be set on
file.

The second program, t_get_seals.c, can be used to open an existing
file that was created via memfd_create() and inspect the set of seals
that have been applied to that file.

The following shell session demonstrates the use of these programs.
First we create a tmpfs(5) file and set some seals on it:

   $ ./t_memfd_create my_memfd_file 4096 sw &
   [1] 11775
   PID: 11775; fd: 3; /proc/11775/fd/3

At this point, the t_memfd_create program continues to run in the
background.  From another program, we can obtain a file descriptor
for the file created by memfd_create() by opening the /proc/[pid]/fd
file that corresponds to the file descriptor opened by memfd_cre‐
ate().  Using that pathname, we inspect the content of the
/proc/[pid]/fd symbolic link, and use our t_get_seals program to view
the seals that have been placed on the file:

   $ readlink /proc/11775/fd/3
   /memfd:my_memfd_file (deleted)
   $ ./t_get_seals /proc/11775/fd/3
   Existing seals: WRITE SHRINK