#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

// 动态链接库路径
#define LIB_PATH "./libxxx.so"

// 函数指针
typedef int (*ADD_FUNC)(int, int);

int main()
{
	void *handle;
	char *error;
	ADD_FUNC add_func = NULL;

	// 打开动态链接库
	handle = dlopen(LIB_PATH, RTLD_LAZY);
	if (!handle)
	{
		fprintf(stderr, "%s\n", dlerror());
		exit(-1);
	}
	// 清除之前存在的错误
	dlerror();

	// 获取一个函数
	add_func = (ADD_FUNC)dlsym(handle, "add");
	if ((error = dlerror()) != NULL)
	{
		fprintf(stderr, "%s\n", error);
		exit(-1);
	}

	printf("add :%d\n", (*add_func)(2, 3));

	// 关闭动态链接库
	dlclose(handle);
	exit(0);
}