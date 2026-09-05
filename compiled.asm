global _start
section .text

_start:
	push rbp
	mov rbp, rsp
	sub rsp, 64
	
	mov rax, 2
	cmp rax, 2
	setg al
	movzx rax, al
	mov [rbp - 8], rax
	mov rax, [rbp - 8]
	cmp rax, 0
	je .L0
	mov rax, [rbp - 8]
	mov [rbp - 16], rax
	mov rax, 5
	mov [rbp - 24], rax
	jmp .L1
	mov rax, 0
	mov [rbp - 32], rax
	.L0:
	mov rax, 10
	mov [rbp - 24], rax
	.L1:
	
	mov rdi, [rbp - 24]
	mov rsp, rbp
	pop rbp
	mov rax, 60
	syscall