;; ata_lba.s -- ATA LBA-mode i/o

;; Copyright (c) 2017 Eetu "Razbit" Pesonen
;;
;; This file is part of RazOS.
;;
;; RazOS is free software: you can redistribute it and/or modify
;; it under the terms of the GNU General Public License version 2
;; as published by the Free Software Foundation.
;;
;; RazOS is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with RazOS. If not, see <http://www.gnu.org/licenses/>.

;; Technical Information on the ports:
;;      Port    Read/Write   Misc
;;     ------  ------------ -------------------------------------------------
;;       1f0       r/w       data register
;;       1f1       r         error register
;;       1f2       r/w       sector count
;;       1f3       r/w       sector number
;;       1f4       r/w       cylinder low
;;       1f5       r/w       cylinder high
;;       1f6       r/w       drive/head
;;                              bit 7 = 1
;;                              bit 6      lba mode
;;                              bit 5 = 1
;;                              bit 4      drive select
;;                              bit 3-0    head select/lba high
;;       1f7       r         status register
;;                              bit 7 = 1  controller is executing a command
;;                              bit 6 = 1  drive is ready
;;                              bit 5 = 1  write fault
;;                              bit 4 = 1  seek complete
;;                              bit 3 = 1  sector buffer requires servicing
;;                              bit 2 = 1  disk data read corrected
;;                              bit 1 = 1  index - set to 1 each revolution
;;                              bit 0 = 1  previous command ended in an error
;;       1f7       w         command register
;;                            commands:
;;                              50h format track
;;                              20h read sectors with retry
;;                              21h read sectors without retry
;;                              22h read long with retry
;;                              23h read long without retry
;;                              30h write sectors with retry
;;                              31h write sectors without retry
;;                              32h write long with retry
;;                              33h write long without retry


;; Declared in ata_lba.h
;; void ata_lba_read(uint32_t lba, uint8_t n_sect, void* buf);
[GLOBAL ata_lba_read]

;; void ata_lba_write(uint32_t lba, uint8_t n_sect, void* buf);
[GLOBAL ata_lba_write]

ata_lba_read:
    push edi
    push ebx

    mov edi, [esp+20]           ; buffer address
    mov ecx, [esp+16]           ; number of sectors to read
    and ecx, 0x000000ff         ; only 8 bits are meaningful
    mov eax, [esp+12]           ; LBA of the wanted sector
    mov ebx, eax                ; save LBA to ebx

    mov dx, 0x01f6              ; drive/head port
    shr eax, 24                 ; get bits 24..27 of lba to al
    or al, 0xe0                 ; set drive to lba mode
    out dx, al

    mov dx, 0x01f2              ; sector count port
    mov al, cl                  ; number of sectors from ecx
    out dx, al

    mov dx, 0x01f3              ; bits 0..7 of lba
    mov eax, ebx                ; LBA from ebx
    out dx, al

    mov dx, 0x01f4              ; bits 8..15 of lba
    mov eax, ebx
    shr eax, 8
    out dx, al

    mov dx, 0x01f5              ; bits 16..23 of lba
    mov eax, ebx
    shr eax, 16
    out dx, al

    mov dx, 0x01f7              ; control register
    mov al, 0x20                ; read with retry
    out dx, al

.loop:
    in al, dx                   ; read status register
    test al, 0x08               ; sector buffer requires servicing
    jz .loop

    mov eax, 256                ; 256 words = 1 sector
    xor bx, bx
    mov bl, cl                  ; sector count from ecx
    mul bx                      ; eax = number of words to read
    mov ecx, eax                ; ecx is insw counter

    mov edx, 0x01f0             ; data port
    rep insw                    ; read to [edi]

    pop ebx
    pop edi

    ret

ata_lba_write:
    push esi
    push ebx

    mov esi, [esp+20]           ; buffer address
    mov ecx, [esp+16]           ; number of sectors to read
    and ecx, 0x000000ff         ; only 8 bits are meaningful
    mov eax, [esp+12]           ; LBA of the wanted sector
    mov ebx, eax                ; save LBA to ebx

    mov dx, 0x01f6              ; drive/head port
    shr eax, 24                 ; get bits 24..27 of lba to al
    or al, 0xe0                 ; set drive to lba mode
    out dx, al

    mov dx, 0x01f2              ; sector count port
    mov al, cl                  ; number of sectors from ecx
    out dx, al

    mov dx, 0x01f3              ; bits 0..7 of lba
    mov eax, ebx                ; LBA from ebx
    out dx, al

    mov dx, 0x01f4              ; bits 8..15 of lba
    mov eax, ebx
    shr eax, 8
    out dx, al

    mov dx, 0x01f5              ; bits 16..23 of lba
    mov eax, ebx
    shr eax, 16
    out dx, al

    mov dx, 0x01f7              ; control register
    mov al, 0x30                ; write with retry
    out dx, al

.loop:
    in al, dx                   ; read status register
    test al, 0x08               ; sector buffer requires servicing
    jz .loop

    mov eax, 256                ; 256 words = 1 sector
    xor bx, bx
    mov bl, cl                  ; sector count from ecx
    mul bx                      ; eax = number of words to read
    mov ecx, eax                ; ecx is outsw counter

    mov edx, 0x01f0             ; data port
    rep outsw                   ; write from [esi]

    pop ebx
    pop esi

    ret
