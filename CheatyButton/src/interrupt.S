/* #include "main.h" */

buffer_data: .skip 1024
buffer_pos:  .word 0

.global TIMER0_IRQHandler2
TIMER0_IRQHandler2:
	push	{r7}
	add	r7, sp, #0
	movw	r3, #0

	movt	r3, #0

	ldr	r2, [r3, #0]
	movt    r3,  #0x4000 ; Load 0x40004000 (LPC_TIM0->IR) into r3
	movw	r3, #0x4000
	ldr	r3, [r3, #0]     ; Read LPC_TIM0->IR from memory
	mov	r1, r3
	movw	r3, #0

	movt	r3, #0

	strt	r1, [r3, r2, lsl #2]
	mov	r3, #buffer_pos
	movw	r2, #255
	str	r2, [r3, #0]
	movw	r3, #0

	movt	r3, #0

	ldr	r3, [r3, #0]
	addw	r2, r3, #1
	movw	r3, #0

	movt	r3, #0

	str	r2, [r3, #0]
	movw	r3, #0

	movt	r3, #0

	ldr	r3, [r3, #0]
	movw	r2, r3, lsl
	movw	r2, r2, lsr

	movw	r3, #0
	movt	r3, #0

	str	r2, [r3, #0]
	mov	sp, r7
	pop	{r7}
	bx	lr
	nop
