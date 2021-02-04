global _start
_start:
	mov eax, 1
	mov ebx, 42
	sub ebx, 29
	int 0x80

;如何编译
;nasm -f elf32 write.asm -o write.o
;ld -m elf_i386 write.o -o write.out
;echo $?
