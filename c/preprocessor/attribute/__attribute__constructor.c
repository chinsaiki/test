#include <stdio.h>

#define debug() printf("[%s:%s %d]\n", __FILE__, __func__, __LINE__)

void __attribute__((constructor)) _crypto_init(){
	debug();
}
void __attribute__((destructor)) __crypto_exit(){
	debug();
}

int main(){
	debug();
}
