;.686
;.MODEL FLAT 
;.STACK         
;.DATA   
.CODE
;ecx - first arg
;edx - second arg
;eax - return value
min PROC
	mov rax, rcx
	cmp rcx, rdx
	cmovg rax, rdx ;if jd then do mov
	ret
min ENDP

PUBLIC min

END
