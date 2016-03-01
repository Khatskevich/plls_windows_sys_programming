.686
.MODEL FLAT 
.STACK         
.DATA   
.CODE
;ecx - first arg
;edx - second arg
;eax - return value
?min@@YIHHH@Z PROC
	mov eax, ecx
	cmp ecx, edx
	cmovg eax, edx ;if jd then do mov
	ret
?min@@YIHHH@Z ENDP

PUBLIC ?min@@YIHHH@Z

END
