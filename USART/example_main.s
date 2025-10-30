	.file	"example_main.c"
__SP_H__ = 0x3e
__SP_L__ = 0x3d
__SREG__ = 0x3f
__RAMPZ__ = 0x3b
__tmp_reg__ = 0
__zero_reg__ = 1
	.text
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"ATmega2560 USART0 test\r"
	.section	.text.startup,"ax",@progbits
.global	main
	.type	main, @function
main:
	push __tmp_reg__
	in r28,__SP_L__
	in r29,__SP_H__
/* prologue: function */
/* frame size = 1 */
/* stack size = 1 */
.L__stack_usage = 1
	ldi r22,0
	ldi r23,lo8(-31)
	ldi r24,0
	ldi r25,0
	call usart0_init
	ldi r24,lo8(usart0_stdout)
	ldi r25,hi8(usart0_stdout)
	sts __iob+2+1,r25
	sts __iob+2,r24
	call usart0_rx_enable_interrupt
/* #APP */
 ;  29 "example_main.c" 1
	sei
 ;  0 "" 2
/* #NOAPP */
	ldi r24,lo8(.LC0)
	ldi r25,hi8(.LC0)
	call puts
.L3:
	movw r24,r28
	adiw r24,1
	call usart0_getchar_nb
	or r24,r25
	breq .L3
	ldd r24,Y+1
	call usart0_tx
	ldd r24,Y+1
	call usart0_tx
	rjmp .L3
	.size	main, .-main
	.data
	.type	usart0_stdout, @object
	.size	usart0_stdout, 14
usart0_stdout:
	.zero	3
	.byte	2
	.zero	4
	.word	gs(usart0_putchar_printf)
	.word	0
	.word	0
	.ident	"GCC: (Homebrew AVR GCC 9.4.0_1) 9.4.0"
.global __do_copy_data
