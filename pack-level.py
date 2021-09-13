import sys
from itertools import islice, repeat
import struct


def graph_1bit(data: bytes) -> list[int]:
    graph = list(islice(repeat(0), 32))

    assert len(data) == (32 * 32), len(data)

    for i, b in enumerate(data):
        bit = (b != 0)
        graph[i // 32] |= (bit << (i % 32))

    return graph


with open(sys.argv[1], "rb") as f:
    data = f.read()

graph = graph_1bit(data)

with open(sys.argv[2], "wb") as f:
    for n in graph:
        f.write(struct.pack("<I", n))
