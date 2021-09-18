import sys
from itertools import islice, repeat
import struct


not_pathable = {
    0: False, # white
    1: True,  # green
    2: True,  # black
    3: True,  # grey
}

reserved_bits = 1

def graph_nbit(data: bytes, nbits: int) -> list[int]:
    graph = list(islice(repeat(0), 32 * nbits))

    assert len(data) == (32 * 32), len(data)

    for i, b in enumerate(data):
        assert b < (2 ** (nbits - reserved_bits))
        assert b in not_pathable, (i, b)
        np = int(not_pathable[b])
        nib = (np << 3) | (b & 0x7)
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
