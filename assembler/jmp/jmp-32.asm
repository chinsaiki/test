; Copyright (C) Rong Tao @Sylincom Beijing, 2021年 02月 05日 星期五 08:47:08 CST. 
global _start 
section .text

_start:
	mov ebx, 42 ; exit status os 42
	mov eax, 1	; sys_exit
	jmp skip	; jump to skip
	mov ebx, 13	; exit status is 13
skip:
	int 0x80	; irq
