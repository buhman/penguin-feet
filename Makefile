all: animals.gba

.PRECIOUS: %.character %.palette

define BUILD_BINARY_O
	$(OBJCOPY) \
		-I binary -O elf32-littlearm -B armv4t \
		--rename-section .data=.data.$(basename $@) \
		$< $@
endef

%.palette.o: %.palette
	$(BUILD_BINARY_O)

%.character.o: %.character
	$(BUILD_BINARY_O)

%.character: %.data
	python pack-character.py $< $(basename $<).dim $@

%.palette: %.pal
	python pack-palette.py $< $@

maze_tile_%.character: maze_tile_%.data
	python pack-character.py $< $(@D)/maze_tile.dim $@

MAZE_TILE_OBJ = image/maze_tile.palette.o
MAZE_TILE_OBJ += image/maze_tile_CORNER_NE.character.o
MAZE_TILE_OBJ += image/maze_tile_CROSS_NESW.character.o
MAZE_TILE_OBJ += image/maze_tile_END_W.character.o
MAZE_TILE_OBJ += image/maze_tile_END_S.character.o
MAZE_TILE_OBJ += image/maze_tile_LINE_NS.character.o
MAZE_TILE_OBJ += image/maze_tile_LINE_WE.character.o
MAZE_TILE_OBJ += image/maze_tile_TEE_NES.character.o
MAZE_TILE_OBJ += image/maze_tile_TEE_WNE.character.o

PENGUIN_OBJ = image/penguin.palette.o
PENGUIN_OBJ += image/penguin.character.o

BACKGROUND_OBJ = image/background.palette.o
BACKGROUND_OBJ += image/background.character.o

OBJS = header.o load.o main.o palette.o tile.o
OBJS += maze.o penguin.o background.o graph.o
OBJS += $(MAZE_TILE_OBJ)
OBJS += $(PENGUIN_OBJ)
OBJS += $(BACKGROUND_OBJ)

animals.elf: $(OBJS) | animals.lds
	$(LD) -Map=$@.map -T animals.lds $^ -o $@

SNES_OBJS = header.o load.o main_gpio.o
snes.elf: $(SNES_OBJS) | animals.lds
	$(LD) -Map=$@.map -T animals.lds $^ -o $@

deploy: snes.gba

include common.mk
