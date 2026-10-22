.include "macros.inc"

.set MSR_IR_DR, 0x30
.set MSR_TGPR_HI, 0x2

.macro TRK_INTERRUPT_ENTRY vector
	mtsprg 1, r2
	mtsprg 2, r3
	mtsprg 3, r4
	mfsrr0 r2
	mfsrr1 r4
	mfmsr r3
	ori r3, r3, MSR_IR_DR
	mtsrr1 r3
	lis r3, TRKInterruptHandler@h
	ori r3, r3, TRKInterruptHandler@l
	mtsrr0 r3
	li r3, \vector
	rfi
.endm

.macro TRK_INTERRUPT_VECTOR vector
.org \vector
	TRK_INTERRUPT_ENTRY \vector
.endm

.macro TRK_TLB_MISS_VECTOR vector
.org \vector
	mtsprg 1, r2
	mfcr r2
	mtsprg 2, r2
	mfmsr r2
	andis. r2, r2, MSR_TGPR_HI
	beq .LTLBMissSaved\@
	mfmsr r2
	xoris r2, r2, MSR_TGPR_HI
	sync
	mtmsr r2
	sync
	mtsprg 1, r2
.LTLBMissSaved\@:
	mfsprg r2, 2
	mtcrf 0xff, r2
	mfsprg r2, 1
	TRK_INTERRUPT_ENTRY \vector
.endm

.section .init, "ax"
.balign 4

.fn gTRKInterruptVectorTable, global
	.asciz "Metrowerks Target Resident Kernel for PowerPC"

.org 0x100
	b __TRK_reset

.org 0x200
	mtsprg 1, r2
	mfsrr0 r2
	icbi r0, r2
	mfdar r2
	dcbi r0, r2
	mfsprg r2, 1
	TRK_INTERRUPT_ENTRY 0x200

TRK_INTERRUPT_VECTOR 0x300
TRK_INTERRUPT_VECTOR 0x400
TRK_INTERRUPT_VECTOR 0x500
TRK_INTERRUPT_VECTOR 0x600
TRK_INTERRUPT_VECTOR 0x700
TRK_INTERRUPT_VECTOR 0x800
TRK_INTERRUPT_VECTOR 0x900
TRK_INTERRUPT_VECTOR 0xC00
TRK_INTERRUPT_VECTOR 0xD00
TRK_INTERRUPT_VECTOR 0xE00

.org 0xF00
	b .LPerformanceMonitor

TRK_INTERRUPT_VECTOR 0xF20

.LPerformanceMonitor:
	TRK_INTERRUPT_ENTRY 0xF00

TRK_TLB_MISS_VECTOR 0x1000
TRK_TLB_MISS_VECTOR 0x1100
TRK_TLB_MISS_VECTOR 0x1200

TRK_INTERRUPT_VECTOR 0x1300
TRK_INTERRUPT_VECTOR 0x1400
TRK_INTERRUPT_VECTOR 0x1600
TRK_INTERRUPT_VECTOR 0x1700
TRK_INTERRUPT_VECTOR 0x1C00
TRK_INTERRUPT_VECTOR 0x1D00
TRK_INTERRUPT_VECTOR 0x1E00
TRK_INTERRUPT_VECTOR 0x1F00

.endfn gTRKInterruptVectorTable

.global gTRKInterruptVectorTableEnd
gTRKInterruptVectorTableEnd:
