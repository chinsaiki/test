
//get directory entries
int getdents(unsigned int fd, struct linux_dirent *dirp,
            unsigned int count);

//Note: There is no glibc wrapper for this system call; see NOTES.

