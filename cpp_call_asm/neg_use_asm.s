.file "neg_use_asm.s"
.text
.global neg_use_asm
.type neg_use_asm @function

neg_use_asm:
	movl %edi, %eax
	negl %eax
	ret
