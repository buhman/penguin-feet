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

%.level.o: %.level
	$(BUILD_BINARY_O)

%.dfreq.o: %.dfreq
	$(BUILD_BINARY_O)

%.character: %.data
	python pack/character.py $< $(basename $<).dim $@

%.palette: %.pal
	python pack/palette.py $< $@

%.level: %.data
	python pack/level.py $< $@

maze_tile_%.character: maze_tile_%.data
	python pack/character.py $< $(@D)/maze_tile.dim $@

footprint_%.character: footprint_%.data
	python pack/character.py $< $(@D)/footprint.dim $@

MAZE_TILE_OBJ = character/maze_tile.palette.o
MAZE_TILE_OBJ += character/maze_tile_CORNER_NE.character.o
MAZE_TILE_OBJ += character/maze_tile_CROSS_NESW.character.o
MAZE_TILE_OBJ += character/maze_tile_END_W.character.o
MAZE_TILE_OBJ += character/maze_tile_END_S.character.o
MAZE_TILE_OBJ += character/maze_tile_LINE_NS.character.o
MAZE_TILE_OBJ += character/maze_tile_LINE_WE.character.o
MAZE_TILE_OBJ += character/maze_tile_TEE_NES.character.o
MAZE_TILE_OBJ += character/maze_tile_TEE_WNE.character.o

PENGUIN_OBJ = character/penguin.palette.o
PENGUIN_OBJ += character/penguin.character.o

BACKGROUND_OBJ = character/background.palette.o
BACKGROUND_OBJ += character/background.character.o

LEVEL_OBJ = level/0.level.o
LEVEL_OBJ += level/1.level.o

FOOTPRINT_OBJ = character/footprint.palette.o
FOOTPRINT_OBJ += character/footprint_east_0.character.o
FOOTPRINT_OBJ += character/footprint_east_1.character.o
FOOTPRINT_OBJ += character/footprint_north_0.character.o
FOOTPRINT_OBJ += character/footprint_north_1.character.o

LOG_OBJ = character/log.palette.o
LOG_OBJ += character/log.character.o

MUSIC_OBJ = music/passacaglia_voice_0.dfreq.o
MUSIC_OBJ += music/passacaglia_voice_1.dfreq.o

OBJS = header.o load.o main.o palette.o tile.o copy16.o
OBJS += path_debug.o ucs.o min_heap.o
OBJS += level.o $(LEVEL_OBJ)
OBJS += penguin.o $(PENGUIN_OBJ)
OBJS += footprint.o $(FOOTPRINT_OBJ)
OBJS += log.o $(LOG_OBJ)
#OBJS += music.o $(MUSIC_OBJ)

animals.elf: $(OBJS) | animals.lds
	$(LD) --print-memory-usage -Map=$@.map -T animals.lds $^ -o $@

SNES_OBJS = header.o load.o main_gpio.o
snes.elf: $(SNES_OBJS) | animals.lds
	$(LD) -Map=$@.map -T animals.lds $^ -o $@

deploy: animals.gba

include common.mk
