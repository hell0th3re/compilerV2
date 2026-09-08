global _start
section .text

_start:
	push rbp
	mov rbp, rsp
	sub rsp, 96
	
	mov rax, 1
	mov [rbp - 8], rax
	mov rax, 0
	mov [rbp - 16], rax
	.L0:
	mov rax, [rbp - 16]
	cmp rax, 5
	setl al
	movzx rax, al
	mov [rbp - 32], rax
	mov rax, [rbp - 32]
	cmp rax, 0
	je .L1
	mov rax, [rbp - 32]
	mov [rbp - 40], rax
	mov rax, 2
	mov [rbp - 48], rax
	.L2:
	mov rax, [rbp - 16]
	add rax, 1
	mov [rbp - 64], rax
	mov rax, [rbp - 64]
	mov [rbp - 16], rax
	jmp .L0
	mov rax, 0
	mov [rbp - 24], rax
	.L1:
	
	mov rdi, [rbp - 8]
	mov rsp, rbp
	pop rbp
	mov rax, 60
	syscall