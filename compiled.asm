global _start
section .text

_start:
	push rbp
	mov rbp, rsp
	sub rsp, 0
	
	mov rsp, rbp
	pop rbp
	mov rax, 60
	syscall