import sys
import struct


class filename:
    src = sys.argv[1]
    dest = sys.argv[2]


def pack_color(src: memoryview,
               dest: memoryview,
               index: int
               ):
    r, g, b = [src[index * 3 + c] for c in [0, 1, 2]]
    color = ((b // 8) << 10) | ((g // 8) << 5) | ((r // 8) << 0)
    struct.pack_into('<H', dest, (index + 1) * 2, color)


def pack_palette():
    with open(filename.src, "rb") as f:
        data_pal = f.read()

    colors = len(data_pal) // 3
    assert colors < 16, colors

    palette_ram = bytearray(512)
    for color in range(colors):
        pack_color(data_pal, palette_ram, color)

    colors += 1 # allocate transparent color #0

    with open(filename.dest, "wb") as f:
        f.write(palette_ram[:colors * 2])


pack_palette()
