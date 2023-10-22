#Bootloader for x86_64
This is the bootloader source code directory of platform x86_64,which consists of two parts:
**bootsec**:which is embedded into MBR and loaded at startup by BIOS into memory whose address is 0x07c00 and excuted first.After it's excuted,it loads kernel image from sector 2-62 (or 2-2047) into memory 0x10000 and jump at it.
