#	Nut bootloader
#	This file is embedded into MBR
#	which copies the 1-64 sectors to 0x10000-0x17bff in physical memory and directly jump to 0x10000
#
#		Written by Li Fei(C) 2016.11.8
#
#
#
#	The MBR is loaded at memory location 0x0000:0x7C00 with the following CPU registers
#	DL:
#	0x80 = first, 0x81 = second, ..., 0xFE
#	floppies / superfloppies: 0x00 = first, 0x01 = second, ..., 7E
#	values 0x7F and 0xFF are reserved for ROM / remote drives and must not be used on disk
#	DH:
#	DH bit 5 = 0: device supported through INT 13h
#	else don't care (should be zero)
#	DH is supported by some IBM BIOSes

.code16
_start:	
#	Set up stack at 0x0000:0xffff
	mov	$0x0000,%ax
	mov	%ax,%ss
	mov	$0xffff,%sp
	
	push	%dx
	
	call	set_video_mode
	call	clear_screen
	
	pop	%dx
	call	load_image
	
/*	mov	$0x0000,%di
	mov	%di,%ds
	lea	str,%si
	mov	$11,%cx
	call	print*/
	
	
/*	mov	$0x1000,%di
	mov	%di,%ds
	mov	$0x0000,%di
	mov	%ds:(%di),%ah
	cmp	$0x0a,%ah*/
	
/*	mov	%di,%si
	mov	$512,%cx
	call	hexdump*/
	
/*	mov	$0x0000,%di
	mov	%di,%ds
	*/
/* 	lea	validate_faild,%si 
	jne	error
	*/
/*	lea	msg,%si
	mov	$10,%cx
	call	print*/
	
 	ljmp	$0x1000,$0x0000
	hlt
dead:
	jmp	dead
#	Set up text video mode with 80 * 25 resolution
set_video_mode:
	mov	$0x00,%ah
	mov	$0x02,%al
	int	$0x10
	
	ret
	
clear_screen:
	mov	$0x06,%ah
	mov	$0x00,%al
	mov	$0x00,%ch
	mov	$0x00,%ch
	mov	$0x24,%dh
	mov	$0x79,%dl
	mov	$0x07,%bh
	int	$0x10
	
	ret
conv:.ascii "01234567890abcdef"
buf:.ascii "\0\0\0"
#	Dump hex
#	CX:Count
#	SI:Address
hexdump:

	push	%si
	push	%cx
	
hexdump_loop:

	pop	%cx
	pop	%si
	jcxz	hexdump_done
	
	lea	conv,%bp
	lea	buf,%di
	
	movb	(%si),%bl
	movb	$0x00,%bh
	and	$0x00f0,%bx
	shr	$4,%bx
	
	add	%bp,%bx
	movb	(%bx),%dl
	movb	%dl,(%di)
	
	movb	(%si),%bl
	movb	$0x00,%bh
	and	$0x000f,%bx
	
	add	%bp,%bx
	movb	(%bx),%dl
	inc	%di
	movb	%dl,(%di)
	
	inc	%si
	dec	%cx
	
	push	%si
	push	%cx
	
	lea	buf,%si
	call	print
	
hexdump_done:
	ret

#	Print message 
#	SI:Address
#	CX:Count
print:
	push	%cx
print_loop:
	pop	%cx
	jcxz	print_done
	dec	%cx
	push	%cx
	
#	cmpb	$0x00,(%si)
#	jz	print_done
	
#	read_cursor_column_number
	mov	$0x03,%ah
	mov	$0x00,%bh
	int	$0x10
	
#	increment_cursor_column_number
	mov	$0x02,%ah
	inc	%dl
	int	$0x10
	
	movb	(%si),%al
	inc	%si
	
	mov	$0x0A,%ah
# 	mov	$'A',%al 
	mov	$0x00,%bh
	mov	$0x0001,%cx
	int	$0x10
	
 	jmp	print_loop
print_done:	
	ret	
	
load_image:
	mov	$0x02,%ah
	mov	$62,%al
	mov	$0x00,%ch
	mov	$0x02,%cl
	mov	$0x00,%dh
	mov	$0x1000,%di
	mov	%di,%es
	mov	$0x0000,%bx
	int	$0x13
	
	lea	disk_error,%si
	jc	error
	
	ret
error:
	call	print
	hlt
disk_error:.ascii "Read disk error."
msg:.ascii "Helloworld."
