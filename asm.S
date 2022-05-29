; x86
; EAX = current cell value
; ESP = current cell address

; +
inc eax

; -
dec eax

; >
mov [esp],eax
add esp,4
mov eax,[esp]

; <
mov [esp],eax
sub esp,4
mov eax,[esp]

; [
test eax,eax
jz 

; ]
test eax,eax
jnz 

; .
call putc

; ,
call getc


@ arm
@ r0 = current cell value
@ sp = current cell address

@ +
add r0,r0,#1

@ -
sub r0,r0,#1

@ >
stmia sp,{r0}
ldr r0,[sp]

@ <
stmda sp,{r0}
ldr r0,[sp]

@ [
tst r0,r0
beq

@ ]
tst r0,r0
bne

@ .
call putc

@ ,
call getc

