import sys
from collections import defaultdict
from itertools import islice, repeat
import struct


not_pathable = defaultdict(lambda: True, {
    0: False, # white
    1: False, # yellow
})

trackable = defaultdict(lambda: False, {
    0: True,  # white
    3: True,  # teal
})

reserved_bits = 2

def graph_nbit(data: bytes, nbits: int) -> list[int]:
    graph = list(islice(repeat(0), 32 * nbits))

    assert len(data) == (32 * 32 * 2), len(data)

    for i, b in enumerate(data):
        if i % 2 != 0:
            assert(b == 0xff)
            continue
        i = i // 2

        #assert b < (2 ** (nbits - reserved_bits)), (b, (2 ** (nbits - reserved_bits)))
        np = int(not_pathable[b])
        tr = int(trackable[b])
        nib = (np << 3) | (tr << 2) | (b & 0x3)
        #print(i % 32, i // 32, (i * nbits) // 32, ((i * nbits) % 32))
        #print(" ", bin((nib << ((i * nbits) % 32))))
        graph[(i * nbits) // 32] |= (nib << ((i * nbits) % 32))

    return graph


with open(sys.argv[1], "rb") as f:
    data = f.read()

graph = graph_nbit(data, 4)

with open(sys.argv[2], "wb") as f:
    for n in graph:
        f.write(struct.pack("<I", n))
