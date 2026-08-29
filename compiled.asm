global _start
section .text

_start:
	push rbp
	mov rbp, rsp
	sub rsp, 112
	
	mov rax, 1
	mov [rbp - 8], rax
	mov rax, [rbp - 8]
	cmp rax, 2
	setg al
	movzx rax, al
	mov [rbp - 16], rax
	mov rax, [rbp - 16]
	cmp rax, 0
	je .L0
	mov rax, [rbp - 16]
	mov [rbp - 24], rax
	mov rax, 10
	mov [rbp - 8], rax
	jmp .L1
	mov rax, 0
	mov [rbp - 32], rax
	.L0:
	mov rax, [rbp - 8]
	cmp rax, 3
	setg al
	movzx rax, al
	mov [rbp - 40], rax
	mov rax, [rbp - 40]
	cmp rax, 0
	je .L2
	mov rax, [rbp - 40]
	mov [rbp - 48], rax
	mov rax, 5
	mov [rbp - 8], rax
	jmp .L3
	mov rax, 0
	mov [rbp - 56], rax
	.L2:
	mov rax, 0
	mov [rbp - 8], rax
	.L3:
	.L1:
	mov rsp, rbp
	pop rbp
	mov rax, 60
	syscall