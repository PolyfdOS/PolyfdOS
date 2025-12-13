OBJECTS = loader.o kmain.o io.o fb.o serial.o gdt.o gdt_s.o idt.o idt_s.o keyboard.o shell.o snake.o texteditor.o filesystem.o hardware.o bootsplash.o realistic.o realistic_asm_s.o realistic_demo.o sysfiles.o filemanager.o
CC = gcc
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
         -nostartfiles -nodefaultlibs -Wall -Wextra -Werror
LDFLAGS = -T link.ld -melf_i386
AS = nasm
ASFLAGS = -f elf

all: kernel.elf

kernel.elf: $(OBJECTS)
	ld $(LDFLAGS) $(OBJECTS) -o kernel.elf

polyfdos.iso: kernel.elf
	cp kernel.elf iso/boot/kernel.elf
	genisoimage -R \
	            -b boot/grub/stage2_eltorito \
	            -no-emul-boot \
	            -boot-load-size 4 \
	            -A os \
	            -input-charset utf8 \
	            -quiet \
	            -boot-info-table \
	            -o polyfdos.iso \
	            iso

run: polyfdos.iso
	qemu-system-i386 -cdrom polyfdos.iso

# Assembly files
loader.o: loader.s
	$(AS) $(ASFLAGS) loader.s -o loader.o

io.o: io.s
	$(AS) $(ASFLAGS) io.s -o io.o

gdt_s.o: gdt_asm.s
	$(AS) $(ASFLAGS) gdt_asm.s -o gdt_s.o

idt_s.o: idt_asm.s
	$(AS) $(ASFLAGS) idt_asm.s -o idt_s.o

# C files
kmain.o: kmain.c
	$(CC) $(CFLAGS) -c kmain.c -o kmain.o

fb.o: fb.c
	$(CC) $(CFLAGS) -c fb.c -o fb.o

serial.o: serial.c
	$(CC) $(CFLAGS) -c serial.c -o serial.o

gdt.o: gdt.c
	$(CC) $(CFLAGS) -c gdt.c -o gdt.o

idt.o: idt.c
	$(CC) $(CFLAGS) -c idt.c -o idt.o

keyboard.o: keyboard.c
	$(CC) $(CFLAGS) -c keyboard.c -o keyboard.o

shell.o: shell.c
	$(CC) $(CFLAGS) -c shell.c -o shell.o

snake.o: snake.c
	$(CC) $(CFLAGS) -c snake.c -o snake.o

texteditor.o: texteditor.c
	$(CC) $(CFLAGS) -c texteditor.c -o texteditor.o

filesystem.o: filesystem.c
	$(CC) $(CFLAGS) -c filesystem.c -o filesystem.o

hardware.o: hardware.c
	$(CC) $(CFLAGS) -c hardware.c -o hardware.o

bootsplash.o: bootsplash.c
	$(CC) $(CFLAGS) -c bootsplash.c -o bootsplash.o

realistic.o: realistic.c
	$(CC) $(CFLAGS) -c realistic.c -o realistic.o

realistic_asm_s.o: realistic_asm.s
	$(AS) $(ASFLAGS) realistic_asm.s -o realistic_asm_s.o

realistic_demo.o: realistic_demo.c
	$(CC) $(CFLAGS) -c realistic_demo.c -o realistic_demo.o

sysfiles.o: sysfiles.c
	$(CC) $(CFLAGS) -c sysfiles.c -o sysfiles.o

filemanager.o: filemanager.c
	$(CC) $(CFLAGS) -c filemanager.c -o filemanager.o

clean:
	rm -rf *.o kernel.elf polyfdos.iso