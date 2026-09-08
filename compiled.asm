global _start
section .text

_start:
	push rbp
	mov rbp, rsp
	sub rsp, 168
	
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
	mov rax, 1
	mov [rbp - 48], rax
	.L2:
	mov rax, [rbp - 48]
	cmp rax, 5
	setl al
	movzx rax, al
	mov [rbp - 64], rax
	mov rax, [rbp - 64]
	cmp rax, 0
	je .L3
	mov rax, [rbp - 64]
	mov [rbp - 72], rax
	mov rax, [rbp - 8]
	add rax, 2
	mov [rbp - 80], rax
	mov rax, [rbp - 80]
	mov [rbp - 8], rax
	mov rax, [rbp - 48]
	add rax, 1
	mov [rbp - 88], rax
	mov rax, [rbp - 88]
	mov [rbp - 48], rax
	jmp .L2
	mov rax, 0
	mov [rbp - 56], rax
	.L3:
	.L4:
	mov rax, [rbp - 16]
	add rax, 1
	mov [rbp - 104], rax
	mov rax, [rbp - 104]
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