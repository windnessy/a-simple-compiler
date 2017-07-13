	.file	1 "test1.c"
	.section .mdebug.abi32
	.previous
	.nan	legacy
	.module	fp=xx
	.module	nooddspreg
	.abicalls

	.comm	a,4,4

	.comm	b,4,4
	.text
	.align	2
	.globl	program
	.set	nomips16
	.set	nomicromips
	.ent	program
	.type	program, @function
program:
	.frame	$fp,16,$31		# vars= 8, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-16
	sw	$fp,12($sp)
	move	$fp,$sp
	sw	$4,16($fp)
	sw	$5,20($fp)
	sw	$6,24($fp)
	sw	$0,0($fp)
	lw	$3,20($fp)
	lw	$2,24($fp)
	addu	$3,$3,$2
	lw	$2,16($fp)
	slt	$2,$3,$2
	beq	$2,$0,$L2
	nop

	lw	$3,20($fp)
	lw	$2,24($fp)
	mul	$2,$3,$2
	addiu	$3,$2,1
	lw	$2,16($fp)
	addu	$2,$3,$2
	sw	$2,4($fp)
	b	$L4
	nop

$L2:
	lw	$2,16($fp)
	sw	$2,4($fp)
	b	$L4
	nop

$L5:
	lw	$2,4($fp)
	sll	$2,$2,1
	sw	$2,0($fp)
$L4:
	lw	$2,0($fp)
	slt	$2,$2,101
	bne	$2,$0,$L5
	nop

	lw	$2,0($fp)
	move	$sp,$fp
	lw	$fp,12($sp)
	addiu	$sp,$sp,16
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	program
	.size	program, .-program
	.align	2
	.globl	demo
	.set	nomips16
	.set	nomicromips
	.ent	demo
	.type	demo, @function
demo:
	.frame	$fp,8,$31		# vars= 0, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-8
	sw	$fp,4($sp)
	move	$fp,$sp
	sw	$4,8($fp)
	lw	$2,8($fp)
	addiu	$2,$2,2
	sw	$2,8($fp)
	lw	$2,8($fp)
	sll	$2,$2,1
	move	$sp,$fp
	lw	$fp,4($sp)
	addiu	$sp,$sp,8
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	demo
	.size	demo, .-demo
	.align	2
	.globl	main
	.set	nomips16
	.set	nomicromips
	.ent	main
	.type	main, @function
main:
	.frame	$fp,48,$31		# vars= 16, regs= 2/0, args= 16, gp= 8
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-48
	sw	$31,44($sp)
	sw	$fp,40($sp)
	move	$fp,$sp
	li	$2,3			# 0x3
	sw	$2,28($fp)
	li	$2,4			# 0x4
	sw	$2,32($fp)
	li	$2,2			# 0x2
	sw	$2,36($fp)
	lw	$4,36($fp)
	.option	pic0
	jal	demo
	nop

	.option	pic2
	move	$6,$2
	lw	$5,32($fp)
	lw	$4,28($fp)
	.option	pic0
	jal	program
	nop

	.option	pic2
	sw	$2,28($fp)
	nop
	move	$sp,$fp
	lw	$31,44($sp)
	lw	$fp,40($sp)
	addiu	$sp,$sp,48
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	main
	.size	main, .-main
	.ident	"GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.1) 5.4.0 20160609"
