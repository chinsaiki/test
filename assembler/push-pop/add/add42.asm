; Copyright (C) Rong Tao @Sylincom Beijing, 2021年 02月 05日 星期五 09:09:09 CST. 
global add42
add42:
	push ebp
	mov ebp, esp
	mov eax, [ebp+8]
	add eax, 42
	mov esp, ebp
	pop ebp
	ret
