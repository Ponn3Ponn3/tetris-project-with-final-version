  # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  #
  # Please keep the two macros PUSH and POP unchanged
  #
  
 .global delay
 .global time2string
 .global hexasc

.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm

.data
.align 2
.text
  #
  # Please add your own code below this line
hexasc: 
	andi $a0, $a0, 0xf # Only the 4 least significant bits of a0, all other bits are ignored
	addi $v0, $0, 0x30 # v0 = 0x30 (48, decimal for char '0')
	addi $t0, $0, 0x9 # t0 = 0x9 (9, will be used for evnetual branch)
	
	ble $a0, $t0, L1 # branch if a0 <= 0x9
	nop 
	addi $v0, $v0, 0x7 # v0 += 0x7
	
L1:
	add $v0, $a0, $v0 # v0 += a0
	jr $ra
	nop 
	
delay:
	PUSH $s0
	PUSH $ra
	addi $s0, $0, 0 #i = 0
	addi $t1, $0, 32760
	sll $t1, $t1, 7
	move $t0, $a0
	
while: 
	ble $t0, $0, done # branch if ms <= 0
	nop
	addi $t0, $t0, -1 # ms = ms - 1
	
for:
 	beq $s0, $t1, done2 # branch if i = 4711
 	nop
 	addi $s0, $s0, 1 # i++
 	j for
 	
 done2:
 	j while
 	
 done: 
 	POP $ra
 	POP $s0
 	jr $ra
 	nop
 	
 time2string: 
	PUSH $s0
	PUSH $s1
	PUSH $ra
	
	add $s0, $0, $a0 # address of timstr
	add $s1, $0, $a1 # time info (0x5957)
	
	andi $t0, $s1, 0xf000 # checks 4 msb, ignore rest
	srl $a0, $t0, 12 # shift msb to lsb
	jal hexasc # call hexasc
	nop
	sb $v0, 0($s0) #store the 4 bits
	
	andi $t1, $s1, 0x0f00 # mask 4 bits, ignore rest
	srl $a0, $t1, 8 # make the bits lsb
	jal hexasc 
	nop
	sb $v0, 1($s0) # srote bits in address pointed by a0
	
	li $t3, 0x3a # colon in ascii
	sb $t3, 2($s0)
	
	andi $t2, $s1, 0x00f0 # mask bits
	srl $a0, $t2, 4 # make bits lsb
	jal hexasc
	nop
	sb $v0, 3($s0)
	
	move $a0, $s1
	jal hexasc
	nop
	sb $v0, 4($s0)
	
	
	li $t4, 0x00 #  null in ascii
	sb $t4, 5($s0)
	
	POP $ra
	POP $s1
	POP $s0

	
	jr $ra
	nop
.global enable_interrupt
enable_interrupt:
	ei
	jr $ra
	nop
  #

