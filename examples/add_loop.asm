; example program that calculates the sum of two integer vectors
; it showcases data declaration, function calls and some instructions

section .data
	x dd 1, 2, 3, 4
	y dd 5, 6, 7, 8
	z dd 0, 0, 0, 0


section .text
	; the execution starts here
	call main


	; you should always end your program with the halt instruction
	; or else you program will result in a segmentation falt
	hlt

main:
	; enter
	push ebp
	mov ebp, esp

	mov ecx, 0

	mov esi, x
	mov edi, y
	mov edx, z

	loop:
		mov eax, [esi + ecx * 4]
		add eax, [edi + ecx * 4]
		mov [edx + ecx * 4], eax

		add ecx, 1
		cmp ecx, 4
		jne loop

	; leave
	mov esp, ebp
	pop ebp

	ret
