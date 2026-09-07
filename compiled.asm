global _start
section .text

_start:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	
	mov rax, 2
	mov [rbp - 8], rax
	
	mov rdi, [rbp - 8]
	mov rsp, rbp
	pop rbp
	mov rax, 60
	syscall