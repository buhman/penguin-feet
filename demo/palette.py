import colorsys
import struct
import sys

with open(sys.argv[1], "wb") as f:
    for i in range(0, 256):
        r, g, b = colorsys.hsv_to_rgb(i / 255.0 * (2/3), 1, 1)

        color = (0
            | (round(b * 31) << 10)
            | (round(g * 31) << 5)
            | (round(r * 31) << 0)
        )

        f.write(struct.pack("<H", color))
