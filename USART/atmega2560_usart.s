	.file	"atmega2560_usart.c"
__SP_H__ = 0x3e
__SP_L__ = 0x3d
__SREG__ = 0x3f
__RAMPZ__ = 0x3b
__tmp_reg__ = 0
__zero_reg__ = 1
	.text
.global	usart0_init
	.type	usart0_init, @function
usart0_init:
	push r28
	push r29
	rcall .
	push __tmp_reg__
	in r28,__SP_L__
	in r29,__SP_H__
/* prologue: function */
/* frame size = 4 */
/* stack size = 6 */
.L__stack_usage = 6
	movw r18,r22
	movw r20,r24
	ldi r24,4
	1:
	lsl r18
	rol r19
	rol r20
	rol r21
	dec r24
	brne 1b
	ldi r22,0
	ldi r23,lo8(36)
	ldi r24,lo8(-12)
	ldi r25,0
	call __udivmodsi4
	subi r18,1
	sbc r19,__zero_reg__
	sts 197,r18
	sts 196,r18
	ldi r24,lo8(6)
	sts 194,r24
	ldi r24,lo8(24)
	sts 193,r24
/* epilogue start */
	pop __tmp_reg__
	pop __tmp_reg__
	pop __tmp_reg__
	pop __tmp_reg__
	pop r29
	pop r28
	ret
	.size	usart0_init, .-usart0_init
.global	usart0_rx_enable_interrupt
	.type	usart0_rx_enable_interrupt, @function
usart0_rx_enable_interrupt:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	ldi r30,lo8(-63)
	ldi r31,0
	ld r24,Z
	ori r24,lo8(-128)
	st Z,r24
/* epilogue start */
	ret
	.size	usart0_rx_enable_interrupt, .-usart0_rx_enable_interrupt
.global	usart0_rx_disable_interrupt
	.type	usart0_rx_disable_interrupt, @function
usart0_rx_disable_interrupt:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	ldi r30,lo8(-63)
	ldi r31,0
	ld r24,Z
	andi r24,lo8(127)
	st Z,r24
/* epilogue start */
	ret
	.size	usart0_rx_disable_interrupt, .-usart0_rx_disable_interrupt
.global	usart0_tx
	.type	usart0_tx, @function
usart0_tx:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
.L5:
	lds r25,192
	sbrs r25,5
	rjmp .L5
	sts 198,r24
/* epilogue start */
	ret
	.size	usart0_tx, .-usart0_tx
.global	usart0_tx_ready
	.type	usart0_tx_ready, @function
usart0_tx_ready:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	lds r24,192
	bst r24,5
	clr r24
	bld r24,0
/* epilogue start */
	ret
	.size	usart0_tx_ready, .-usart0_tx_ready
.global	usart0_getchar_nb
	.type	usart0_getchar_nb, @function
usart0_getchar_nb:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	movw r26,r24
	lds r25,rx_head
	lds r24,rx_tail
	cp r25,r24
	breq .L11
	lds r30,rx_tail
	ldi r31,0
	subi r30,lo8(-(rx_buf))
	sbci r31,hi8(-(rx_buf))
	ld r25,Z
	lds r24,rx_tail
	subi r24,lo8(-(1))
	andi r24,lo8(127)
	sts rx_tail,r24
	st X,r25
	ldi r24,lo8(1)
	ldi r25,0
	ret
.L11:
	ldi r25,0
	ldi r24,0
/* epilogue start */
	ret
	.size	usart0_getchar_nb, .-usart0_getchar_nb
.global	usart0_rx_blocking
	.type	usart0_rx_blocking, @function
usart0_rx_blocking:
	push r28
	push r29
	push __tmp_reg__
	in r28,__SP_L__
	in r29,__SP_H__
/* prologue: function */
/* frame size = 1 */
/* stack size = 3 */
.L__stack_usage = 3
.L13:
	movw r24,r28
	adiw r24,1
	call usart0_getchar_nb
	or r24,r25
	breq .L13
	ldd r24,Y+1
/* epilogue start */
	pop __tmp_reg__
	pop r29
	pop r28
	ret
	.size	usart0_rx_blocking, .-usart0_rx_blocking
.global	usart0_send_string
	.type	usart0_send_string, @function
usart0_send_string:
	push r28
	push r29
/* prologue: function */
/* frame size = 0 */
/* stack size = 2 */
.L__stack_usage = 2
	movw r28,r24
.L17:
	ld r24,Y+
	cpse r24,__zero_reg__
	rjmp .L18
/* epilogue start */
	pop r29
	pop r28
	ret
.L18:
	call usart0_tx
	rjmp .L17
	.size	usart0_send_string, .-usart0_send_string
.global	usart0_putchar_printf
	.type	usart0_putchar_printf, @function
usart0_putchar_printf:
	push r28
/* prologue: function */
/* frame size = 0 */
/* stack size = 1 */
.L__stack_usage = 1
	mov r28,r24
	cpi r24,lo8(10)
	brne .L20
	ldi r24,lo8(13)
	call usart0_tx
.L20:
	mov r24,r28
	call usart0_tx
	ldi r25,0
	ldi r24,0
/* epilogue start */
	pop r28
	ret
	.size	usart0_putchar_printf, .-usart0_putchar_printf
.global	__vector_25
	.type	__vector_25, @function
__vector_25:
	__gcc_isr 1
	in r18,__RAMPZ__
	push r18
	push r24
	push r25
	push r30
	push r31
/* prologue: Signal */
/* frame size = 0 */
/* stack size = 5...9 */
.L__stack_usage = 5 + __gcc_isr.n_pushed
	lds r18,198
	lds r24,rx_head
	subi r24,lo8(-(1))
	andi r24,lo8(127)
	lds r25,rx_tail
	cpse r25,r24
	rjmp .L22
	lds r25,rx_tail
	subi r25,lo8(-(1))
	andi r25,lo8(127)
	sts rx_tail,r25
.L22:
	lds r30,rx_head
	ldi r31,0
	subi r30,lo8(-(rx_buf))
	sbci r31,hi8(-(rx_buf))
	st Z,r18
	sts rx_head,r24
/* epilogue start */
	pop r31
	pop r30
	pop r25
	pop r24
	pop r18
	out __RAMPZ__,r18
	__gcc_isr 2
	reti
	__gcc_isr 0,r18
	.size	__vector_25, .-__vector_25
.global	usart0_rx_available
	.type	usart0_rx_available, @function
usart0_rx_available:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	lds r24,rx_head
	lds r25,rx_tail
	sub r24,r25
	andi r24,lo8(127)
/* epilogue start */
	ret
	.size	usart0_rx_available, .-usart0_rx_available
.global	usart1_init
	.type	usart1_init, @function
usart1_init:
	push r28
	push r29
	rcall .
	push __tmp_reg__
	in r28,__SP_L__
	in r29,__SP_H__
/* prologue: function */
/* frame size = 4 */
/* stack size = 6 */
.L__stack_usage = 6
	movw r18,r22
	movw r20,r24
	ldi r24,4
	1:
	lsl r18
	rol r19
	rol r20
	rol r21
	dec r24
	brne 1b
	ldi r22,0
	ldi r23,lo8(36)
	ldi r24,lo8(-12)
	ldi r25,0
	call __udivmodsi4
	subi r18,1
	sbc r19,__zero_reg__
	sts 205,r19
	sts 204,r18
	ldi r24,lo8(6)
	sts 202,r24
	ldi r24,lo8(24)
	sts 201,r24
/* epilogue start */
	pop __tmp_reg__
	pop __tmp_reg__
	pop __tmp_reg__
	pop __tmp_reg__
	pop r29
	pop r28
	ret
	.size	usart1_init, .-usart1_init
.global	usart1_tx
	.type	usart1_tx, @function
usart1_tx:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
.L26:
	lds r25,200
	sbrs r25,5
	rjmp .L26
	sts 206,r24
/* epilogue start */
	ret
	.size	usart1_tx, .-usart1_tx
	.local	rx_tail
	.comm	rx_tail,1,1
	.local	rx_head
	.comm	rx_head,1,1
	.local	rx_buf
	.comm	rx_buf,128,1
	.ident	"GCC: (Homebrew AVR GCC 9.4.0_1) 9.4.0"
.global __do_clear_bss
