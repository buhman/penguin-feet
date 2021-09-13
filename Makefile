OBJS = header.o load.o main.o palette.o tile.o

animals.elf: $(OBJS) | animals.lds
	$(LD) -Map=$@.map -T animals.lds $^ -o $@

include common.mk
