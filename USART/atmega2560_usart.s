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
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	sts 196,r24
	sts 197,r25
	ldi r24,lo8(6)
	sts 194,r24
	ldi r30,lo8(-64)
	ldi r31,0
	ld r24,Z
	ori r24,lo8(2)
	st Z,r24
	ldi r24,lo8(24)
	sts 193,r24
/* epilogue start */
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
.global	usart0_rx_buffered
	.type	usart0_rx_buffered, @function
usart0_rx_buffered:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	movw r26,r24
	lds r25,rx0_head
	lds r24,rx0_tail
	cp r25,r24
	breq .L11
	lds r30,rx0_tail
	ldi r31,0
	subi r30,lo8(-(r0_buf))
	sbci r31,hi8(-(r0_buf))
	ld r25,Z
	lds r24,rx0_tail
	subi r24,lo8(-(1))
	andi r24,lo8(127)
	sts rx0_tail,r24
	st X,r25
	ldi r24,lo8(1)
	ret
.L11:
	ldi r24,0
/* epilogue start */
	ret
	.size	usart0_rx_buffered, .-usart0_rx_buffered
.global	usart0_rx
	.type	usart0_rx, @function
usart0_rx:
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
	call usart0_rx_buffered
	tst r24
	breq .L13
	ldd r24,Y+1
/* epilogue start */
	pop __tmp_reg__
	pop r29
	pop r28
	ret
	.size	usart0_rx, .-usart0_rx
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
	brne .L17
	ldi r24,lo8(13)
	call usart0_tx
.L17:
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
	lds r24,rx0_head
	subi r24,lo8(-(1))
	andi r24,lo8(127)
	lds r25,rx0_tail
	cpse r25,r24
	rjmp .L19
	lds r25,rx0_tail
	subi r25,lo8(-(1))
	andi r25,lo8(127)
	sts rx0_tail,r25
.L19:
	lds r30,rx0_head
	ldi r31,0
	subi r30,lo8(-(r0_buf))
	sbci r31,hi8(-(r0_buf))
	st Z,r18
	sts rx0_head,r24
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
	lds r24,rx0_head
	lds r25,rx0_tail
	sub r24,r25
	andi r24,lo8(127)
/* epilogue start */
	ret
	.size	usart0_rx_available, .-usart0_rx_available
.global	usart1_init
	.type	usart1_init, @function
usart1_init:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	sts 204,r24
	sts 205,r25
	ldi r24,lo8(6)
	sts 202,r24
	ldi r30,lo8(-64)
	ldi r31,0
	ld r24,Z
	ori r24,lo8(2)
	st Z,r24
	ldi r24,lo8(24)
	sts 201,r24
/* epilogue start */
	ret
	.size	usart1_init, .-usart1_init
.global	usart1_rx_enable_interrupt
	.type	usart1_rx_enable_interrupt, @function
usart1_rx_enable_interrupt:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	ldi r30,lo8(-55)
	ldi r31,0
	ld r24,Z
	ori r24,lo8(-128)
	st Z,r24
/* epilogue start */
	ret
	.size	usart1_rx_enable_interrupt, .-usart1_rx_enable_interrupt
.global	usart1_rx_disable_interrupt
	.type	usart1_rx_disable_interrupt, @function
usart1_rx_disable_interrupt:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	ldi r30,lo8(-55)
	ldi r31,0
	ld r24,Z
	andi r24,lo8(127)
	st Z,r24
/* epilogue start */
	ret
	.size	usart1_rx_disable_interrupt, .-usart1_rx_disable_interrupt
.global	usart1_rx_buffered
	.type	usart1_rx_buffered, @function
usart1_rx_buffered:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	movw r26,r24
	lds r25,rx1_head
	lds r24,rx1_tail
	cp r25,r24
	breq .L26
	lds r30,rx1_tail
	ldi r31,0
	subi r30,lo8(-(rx1_buf))
	sbci r31,hi8(-(rx1_buf))
	ld r25,Z
	lds r24,rx1_tail
	subi r24,lo8(-(1))
	andi r24,lo8(127)
	sts rx1_tail,r24
	st X,r25
	ldi r24,lo8(1)
	ldi r25,0
	ret
.L26:
	ldi r25,0
	ldi r24,0
/* epilogue start */
	ret
	.size	usart1_rx_buffered, .-usart1_rx_buffered
.global	usart1_rx
	.type	usart1_rx, @function
usart1_rx:
	push r28
	push r29
	push __tmp_reg__
	in r28,__SP_L__
	in r29,__SP_H__
/* prologue: function */
/* frame size = 1 */
/* stack size = 3 */
.L__stack_usage = 3
.L28:
	movw r24,r28
	adiw r24,1
	call usart1_rx_buffered
	or r24,r25
	breq .L28
	ldd r24,Y+1
/* epilogue start */
	pop __tmp_reg__
	pop r29
	pop r28
	ret
	.size	usart1_rx, .-usart1_rx
.global	usart1_rx_available
	.type	usart1_rx_available, @function
usart1_rx_available:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	jmp usart0_rx_available
	.size	usart1_rx_available, .-usart1_rx_available
.global	__vector_36
	.type	__vector_36, @function
__vector_36:
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
	lds r18,206
	lds r24,rx1_head
	subi r24,lo8(-(1))
	andi r24,lo8(127)
	lds r25,rx1_tail
	cpse r25,r24
	rjmp .L33
	lds r25,rx1_tail
	subi r25,lo8(-(1))
	andi r25,lo8(127)
	sts rx1_tail,r25
.L33:
	lds r30,rx1_head
	ldi r31,0
	subi r30,lo8(-(rx1_buf))
	sbci r31,hi8(-(rx1_buf))
	st Z,r18
	sts rx1_head,r24
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
	.size	__vector_36, .-__vector_36
.global	__vector_37
	.type	__vector_37, @function
__vector_37:
	__gcc_isr 1
	in r24,__RAMPZ__
	push r24
	push r25
	push r30
	push r31
/* prologue: Signal */
/* frame size = 0 */
/* stack size = 4...8 */
.L__stack_usage = 4 + __gcc_isr.n_pushed
	lds r25,tx1_tail
	lds r24,tx1_head
	cp r25,r24
	breq .L35
	lds r30,tx1_tail
	ldi r31,0
	subi r30,lo8(-(tx1_buf))
	sbci r31,hi8(-(tx1_buf))
	ld r24,Z
	sts 206,r24
	lds r24,tx1_tail
	subi r24,lo8(-(1))
	andi r24,lo8(63)
	sts tx1_tail,r24
.L34:
/* epilogue start */
	pop r31
	pop r30
	pop r25
	pop r24
	out __RAMPZ__,r24
	__gcc_isr 2
	reti
.L35:
	lds r24,201
	andi r24,lo8(-33)
	sts 201,r24
	rjmp .L34
	__gcc_isr 0,r24
	.size	__vector_37, .-__vector_37
.global	usart1_tx_ready
	.type	usart1_tx_ready, @function
usart1_tx_ready:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	lds r24,200
	bst r24,5
	clr r24
	bld r24,0
/* epilogue start */
	ret
	.size	usart1_tx_ready, .-usart1_tx_ready
.global	usart1_tx_buffered
	.type	usart1_tx_buffered, @function
usart1_tx_buffered:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	lds r25,tx1_head
	subi r25,lo8(-(1))
	andi r25,lo8(63)
	lds r18,tx1_tail
	cp r18,r25
	breq .L40
	lds r30,tx1_head
	ldi r31,0
	subi r30,lo8(-(tx1_buf))
	sbci r31,hi8(-(tx1_buf))
	st Z,r24
	sts tx1_head,r25
	lds r24,201
	ori r24,lo8(32)
	sts 201,r24
	ldi r24,0
	ret
.L40:
	ldi r24,lo8(-1)
/* epilogue start */
	ret
	.size	usart1_tx_buffered, .-usart1_tx_buffered
.global	usart1_tx
	.type	usart1_tx, @function
usart1_tx:
	push r28
/* prologue: function */
/* frame size = 0 */
/* stack size = 1 */
.L__stack_usage = 1
	mov r28,r24
.L42:
	mov r24,r28
	call usart1_tx_buffered
	cpse r24,__zero_reg__
	rjmp .L43
/* epilogue start */
	pop r28
	ret
.L43:
	ldi r24,lo8(199)
	ldi r25,hi8(199)
1:	sbiw r24,1
	brne 1b
	rjmp .
	nop
	rjmp .L42
	.size	usart1_tx, .-usart1_tx
.global	usart1_putchar_printf
	.type	usart1_putchar_printf, @function
usart1_putchar_printf:
	push r28
/* prologue: function */
/* frame size = 0 */
/* stack size = 1 */
.L__stack_usage = 1
	mov r28,r24
	cpi r24,lo8(10)
	brne .L45
	ldi r24,lo8(13)
	call usart1_tx
.L45:
	mov r24,r28
	call usart0_tx
	ldi r25,0
	ldi r24,0
/* epilogue start */
	pop r28
	ret
	.size	usart1_putchar_printf, .-usart1_putchar_printf
	.local	tx1_tail
	.comm	tx1_tail,1,1
	.local	tx1_head
	.comm	tx1_head,1,1
	.local	tx1_buf
	.comm	tx1_buf,64,1
	.local	rx1_tail
	.comm	rx1_tail,1,1
	.local	rx1_head
	.comm	rx1_head,1,1
	.local	rx1_buf
	.comm	rx1_buf,128,1
	.local	rx0_tail
	.comm	rx0_tail,1,1
	.local	rx0_head
	.comm	rx0_head,1,1
	.local	r0_buf
	.comm	r0_buf,128,1
	.ident	"GCC: (Homebrew AVR GCC 9.4.0_1) 9.4.0"
.global __do_clear_bss
