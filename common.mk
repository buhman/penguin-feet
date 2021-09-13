ARCH ?= -mthumb-interwork -march=armv4t
override CFLAGS += -std=gnu2x -g -Og -Wall -Werror
CARCH ?= -mtune=arm7tdmi -ffreestanding -nostdlib
AFLAGS ?= -g

TARGET ?= arm-none-eabi-
CC = $(TARGET)gcc
AS = $(TARGET)as
LD = $(TARGET)ld
OBJCOPY = $(TARGET)objcopy
OBJDUMP = $(TARGET)objdump

%.o: %.S
	$(CC) $(ARCH) $(AFLAGS) -c $< -o $@

%.o: %.s
	$(AS) $(ARCH) $(AFLAGS) $< -o $@

%.o: %.c
	$(CC) $(ARCH) $(CARCH) $(CFLAGS) -c $< -o $@

%.elf: %.o
	$(LD) $< -o $@

%.gba: %.elf
	$(OBJCOPY) -O binary $< $@

dump:
	$(OBJDUMP) -D -j .text $(F)

clean:
	find -regextype posix-extended \
		-regex '.*\.(o|out|hex|elf|gba|palette|character)' \
		-exec rm -f {} \;

deploy:
	mount /dev/disk/by-label/GBA /mnt
	cp $< /mnt/$<
	umount /mnt
	sync

.SUFFIXES:
.INTERMEDIATE:
.PHONY: all clean dump
