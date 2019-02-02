.686p
.Model Flat,StdCall
Option CaseMap :None

.Code
ReadMSR	Proc	uses ecx MSRIndex
	mov	ecx,MSRIndex
	rdmsr
	ret
ReadMSR	EndP

DisableWP 	Proc
		cli
		mov	eax,cr0
		push	eax
		and	eax, 0FFFEFFFFh
		mov	cr0,eax
		pop	eax
		ret
DisableWP 	EndP

RestoreCR0 	Proc	OldCR0
		mov	eax,OldCR0
		mov	cr0,eax
		sti
		ret
RestoreCR0 	EndP

End