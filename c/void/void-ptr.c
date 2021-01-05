#include <stdio.h>

#define PAGE_MAPPING_ANON 0x1

struct anon_vma{
	struct anon_vma *root;
	void * rwsem;
	unsigned long refcount;
	unsigned degree;
	struct anon_vma *parent;
	struct {}rb_root;
};

int main()
{
	char str[] = {"1234567890"};
	struct anon_vma *av = (struct anon_vma *)str;

	printf("%p:%p\n", av, av + PAGE_MAPPING_ANON);
	printf("%p:%p\n", av, (void*)av + PAGE_MAPPING_ANON);
	printf("%p:%p\n", av, (char*)av + PAGE_MAPPING_ANON);

	printf("%s:%s\n", str, (char*)((void*)str + 0x1));
}
