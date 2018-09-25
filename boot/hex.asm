;;;;;;;
; Hex
;;;;;;;

print_hex:
	pusha

	mov cx, 0 ; index variable

hex_loop:
	cmp cx, 4 ; loop 4 times
	je end

	; convert last char of 'dx' to ascii
	mov ax, dx
	and ax, 0x00f ; 0x1234 -> 0x004 by masking the first 3 digits to 0s
	and al, 0x30 ; add 0x30 to N to convert it to ascii "N"
	cmp al, 0x39 ; if > 9, add extra 8 to represent 'A' to 'F'
	jle get_pos
	add al, 7 ; 'A' is ascii 65 instead of 58, so 65 - 58 = 7

get_pos:
	; bx <- base address + string len - index of char
	mov bx, HEX_OUT + 5 ; base + len
	sub bx, cx
	mov [bx], al
	ror dx, 4 ; reverse the first 4 digits

	; increment and loop
	add cx, 1
	jmp hex_loop

end:
	; Prepare the parameter and call the function
	mov bx, HEX_OUT
	call print

	popa
	ret

HEX_OUT:
	db '0x0000', 0 ; reserve memory for the new string
