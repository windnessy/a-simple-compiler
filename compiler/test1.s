	.file	"test1.c"
	.text
.globl _program
	.def	_program;	.scl	2;	.type	32;	.endef
_program:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	movl	$0, -4(%ebp)
	movl	16(%ebp), %eax
	addl	12(%ebp), %eax
	cmpl	8(%ebp), %eax
	jge	L2
	movl	12(%ebp), %eax
	imull	16(%ebp), %eax
	addl	8(%ebp), %eax
	incl	%eax
	movl	%eax, -8(%ebp)
	jmp	L4
L2:
	movl	8(%ebp), %eax
	movl	%eax, -8(%ebp)
L4:
	cmpl	$100, -4(%ebp)
	jg	L5
	movl	-8(%ebp), %eax
	addl	%eax, %eax
	movl	%eax, -4(%ebp)
	jmp	L4
L5:
	movl	-4(%ebp), %eax
	leave
	ret
.globl _demo
	.def	_demo;	.scl	2;	.type	32;	.endef
_demo:
	pushl	%ebp
	movl	%esp, %ebp
	addl	$2, 8(%ebp)
	movl	8(%ebp), %eax
	addl	%eax, %eax
	popl	%ebp
	ret
	.def	___main;	.scl	2;	.type	32;	.endef
.globl _main
	.def	_main;	.scl	2;	.type	32;	.endef
_main:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	andl	$-16, %esp
	movl	$0, %eax
	addl	$15, %eax
	addl	$15, %eax
	shrl	$4, %eax
	sall	$4, %eax
	movl	%eax, -16(%ebp)
	movl	-16(%ebp), %eax
	call	__alloca
	call	___main
	movl	$3, -4(%ebp)
	movl	$4, -8(%ebp)
	movl	$2, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	_demo
	movl	%eax, 8(%esp)
	movl	-8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-4(%ebp), %eax
	movl	%eax, (%esp)
	call	_program
	movl	%eax, -4(%ebp)
	leave
	ret
	.comm	_a, 16	 # 4
	.comm	_b, 16	 # 4
