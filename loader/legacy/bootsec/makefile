AS = as
LD = ld
ASFLAGS =
LDFLAGS = --script=./ldscript -nostdlib --oformat=binary

SOURCE = bootsec.S

.PHONY:all
all:
	$(AS) $(SOURCE) -o bootsec.o $(ASFLAGS)
	$(LD) bootsec.o -o bootsec $(LDFLAGS)

.PHONY:clean
clean:
	rm -f bootsec.o bootsec
