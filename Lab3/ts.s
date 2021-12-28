.text
.code 32

.global reset
.global vectors_start, vectors_end
.global lock, unlock
reset:	

reset_handler:

  LDR sp, =svc_stack
  BL copy_vectors

  MSR cpsr, #0x12
  LDR sp, =irq_stack

  MSR cpsr, #0x13

  BL main
  B .

.align 4
irq_handler:
  sub	lr, lr, #4
  stmfd	sp!, {r0-r12, lr}
  bl	IRQ_handler  
  ldmfd	sp!, {r0-r12, pc}^

lock:
	mrs r0, cpsr
	orr r0, r0, #0x80  // set I bit to 1
	msr cpsr, r0
	mov pc, lr
unlock:
	mrs r0, cpsr
	bic r0, r0, #0x80  // clr I bit to 0
	msr cpsr, r0
	mov pc, lr

vectors_start:

  LDR PC, reset_handler_addr
  LDR PC, undef_handler_addr
  LDR PC, swi_handler_addr
  LDR PC, prefetch_abort_handler_addr
  LDR PC, data_abort_handler_addr
  B .
  LDR PC, irq_handler_addr
  LDR PC, fiq_handler_addr

reset_handler_addr:          .word reset_handler
undef_handler_addr:          .word undef_handler
swi_handler_addr:            .word swi_handler
prefetch_abort_handler_addr: .word prefetch_abort_handler
data_abort_handler_addr:     .word data_abort_handler
irq_handler_addr:            .word irq_handler
fiq_handler_addr:            .word fiq_handler

vectors_end:
	
.end
