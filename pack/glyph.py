import sys


stride = 496
with open(sys.argv[1], 'rb') as f:
    bizcat = f.read()
assert len(bizcat) == (stride * 16), len(bizcat)


buf = []


def repack_character(cx, cy):
    for y in range(16):
        n = 0
        for x in range(8):
            ix = (cy * 16 + y) * stride + (cx * 8 + x)
            #print("█" if bizcat[ix] else " ", end="")
            assert bizcat[ix] in {0, 1}
            n |= (bizcat[ix] << x)
        buf.append(n)


for cy in range(len(bizcat) // (stride * 16)):
    for cx in range(stride // 8):
        repack_character(cx, cy)


with open(sys.argv[2], 'wb') as f:
    f.write(bytes(buf))
