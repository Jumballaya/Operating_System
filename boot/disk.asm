;;;;;;;
; Disk
;;;;;;;

; load 'dh' sectors from drive 'dl' into ES:BX
disk_load:
	pusha

	; reading from disk requires setting specific values in all registers.
	; we will overwrite our input params from 'dx' and save it to the stack for later
	push dx

	mov ah, 0x02 ; ah <- int 0x13 function. 0x02 = 'read'
	mov al, dh   ; al <- number of sectors to read (0x01 ... 0x80)
	mov cl, 0x02 ; cl <- sector (0x01 ... 0x11)
							 ; 0x01 is out boot sector, 0x02 is the first 'available' sector
	mov ch, 0x00 ; ch <- cylinder (0x0 ... 0x3FF, upper 2 bits in 'cl')
							 ; dl <- drive number. Our caller sets it as a param and gets it from BIOS
							 ; 0 = floppy, 1 = floppy2, 0x80 = hdd, 0x81 = hdd2
	mov dh, 0x00 ; dh <- head number (0x0 ... 0xF)

	; [es:bx] <- pointer to buffer where the data will be stored
	; caller sets it up for us, and it is actually the standard location for int 13h
	int 0x13			; BIOS interupt
	jc disk_error

	pop dx
	cmp al, dh
	jne sectors_error
	popa
	ret

disk_error:
	mov bx, DISK_ERROR
	call print
	call print_nl
	mov dh, ah
	jmp disk_loop

sectors_error:
	mov bx, SECTORS_ERROR
	call print

disk_loop:
	jmp $

DISK_ERROR: db "Disk read error", 0
SECTORS_ERROR: db "Incorrect number of sectors read", 0
