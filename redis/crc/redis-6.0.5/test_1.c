
/* Test main */
#include <stdio.h>
#include <inttypes.h> //PRIx64

#define UNUSED(x) (void)(x)
int crc64Test(int argc, char *argv[]) {
    UNUSED(argc);
    UNUSED(argv);
    crc64_init();
    printf("[calcula]: e9c6d914c4b8d9ca == %016" PRIx64 "\n",
           (uint64_t)_crc64(0, "123456789", 9));
    printf("[64speed]: e9c6d914c4b8d9ca == %016" PRIx64 "\n",
           (uint64_t)crc64(0, "123456789", 9));
    char li[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed "
                "do eiusmod tempor incididunt ut labore et dolore magna "
                "aliqua. Ut enim ad minim veniam, quis nostrud exercitation "
                "ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis "
                "aute irure dolor in reprehenderit in voluptate velit esse "
                "cillum dolore eu fugiat nulla pariatur. Excepteur sint "
                "occaecat cupidatat non proident, sunt in culpa qui officia "
                "deserunt mollit anim id est laborum. ";
    printf("[calcula]: c7794709e69683b3 == %016" PRIx64 "\n",
           (uint64_t)_crc64(0, li, sizeof(li)));
    printf("[64speed]: c7794709e69683b3 == %016" PRIx64 "\n",
           (uint64_t)crc64(0, li, sizeof(li)));
    return 0;
}


int main(int argc, char *argv[]) {
    return crc64Test(argc, argv);
}

