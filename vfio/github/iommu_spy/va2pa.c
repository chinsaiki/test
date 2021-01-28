#include "common.h" // pagemap_get_entry

int main(int argc, char **argv) {
  pid_t pid;
  uintptr_t vaddr, paddr = 0;

  if (argc < 3) {
    printf("Usage: %s pid vaddr\n", argv[0]);
    return EXIT_FAILURE;
  }
  pid = strtoull(argv[1], NULL, 0);
  vaddr = strtoull(argv[2], NULL, 0);
  if (virt_to_phys_user(&paddr, pid, vaddr)) {
    fprintf(stderr, "error: virt_to_phys_user conversion failed\n");
    return EXIT_FAILURE;
  };
  printf("0x%jx\n", (uintmax_t)paddr);
  return EXIT_SUCCESS;
}
