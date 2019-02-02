.Code

ReadMSR	Proc
	rdmsr	; MSR[ecx] --> edx:eax
	shl	rdx, 32
	or	rax, rdx
	ret
ReadMSR	EndP

DisableWP 	Proc
		cli
		mov	rax,cr0
		push	rax
		and	rax, 0FFFEFFFFh
		mov	cr0,rax
		pop	rax
		ret
DisableWP 	EndP

RestoreCR0 	Proc
		mov	cr0,rcx
		sti
		ret
RestoreCR0 	EndP

End