global _start
section .text

_start:
	push rbp
	mov rbp, rsp
	sub rsp, 8
	
	mov rax, 1
	mov [rbp - 8], rax
	mov rsp, rbp
	pop rbp
	mov rax, 60
	syscall