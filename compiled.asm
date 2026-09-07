global _start
section .text

_start:
	push rbp
	mov rbp, rsp
	sub rsp, 72
	
	mov rax, 1
	mov [rbp - 8], rax
	.L0:
	mov rax, [rbp - 8]
	cmp rax, 5
	setl al
	movzx rax, al
	mov [rbp - 24], rax
	mov rax, [rbp - 24]
	cmp rax, 0
	je .L1
	mov rax, [rbp - 24]
	mov [rbp - 32], rax
	mov rax, [rbp - 8]
	add rax, 1
	mov [rbp - 40], rax
	mov rax, [rbp - 40]
	mov [rbp - 8], rax
	jmp .L0
	mov rax, 0
	mov [rbp - 16], rax
	.L1:
	
	mov rdi, [rbp - 8]
	mov rsp, rbp
	pop rbp
	mov rax, 60
	syscall