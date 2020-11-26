; ----------------------------------------------------------------------------
; power.asm
;
; Command line application to compute x^y
; Syntax: power x y
; x and y are integers
; ----------------------------------------------------------------------------

	global	_main
	extern	_atoi
	extern	_printf

	section	.text
_main:
	push	ebx			; save the registers that must be saved
	push	esi
	push	edi

	mov	eax, [esp+16]		; argc (it's not at [esp+4] now :-))
	cmp	eax, 3			; must have exactly two arguments
	jne	error1

	mov	ebx, [esp+20]		; argv
	push	dword [ebx+4]		; argv[1]
	call	_atoi
	add	esp, 4
	mov	esi, eax		; x in esi
	push	dword [ebx+8]
	call	_atoi			; argv[2]
	add	esp, 4
	cmp	eax, 0
	jl	error2
	mov	edi, eax		; y in edi

	mov	eax, 1			; start with answer = 1
check:
	test	edi, edi		; we're counting y downto 0
	jz      gotit			; done
	imul	eax, esi		; multiply in another x
	dec	edi
	jmp	check
gotit:					; print report on success
	push    eax
	push    answer
	call    _printf
	add     esp, 8
	jmp	done
error1:					; print error message
	push	badArgumentCount
	call	_printf
	add	esp, 4
	jmp	done
error2:					; print error message
	push	negativeExponent
	call	_printf
	add	esp, 4
done:					; restore saved registers
	pop	edi
	pop	esi
	pop	ebx
	ret

answer:
	db      '%d', 10, 0
badArgumentCount:
	db	'Requires exactly two arguments', 10, 0
negativeExponent:
	db	'The exponent may not be negative', 10, 0