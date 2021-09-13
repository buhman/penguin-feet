import sys
import struct


class filename:
    data = sys.argv[1]
    data_dim = sys.argv[1] + ".dim"
    dest = sys.argv[2]


def decode_character_into():
    pass


class OAM:
    # 1024 characters in modes 0,1,2
    width = 0x20
    height = 0x20


character_size = 8 * 8


def buf_index(character, index, stride):
    x_offset = ((character * 8) % stride)
    y_offset = 8 * ((character * 8) // stride)
    y = index // 8
    x = index % 8
    return ((y_offset + y) * stride * 2 + (x_offset + x) * 2)


def pack_character(src: memoryview,
                   dest: memoryview,
                   character: int,
                   stride: int,
                   ) -> (int):
    index = 0
    nibble = 0

    while index < character_size:
        buf_ix = buf_index(character, index, stride)
        color = src[buf_ix+0]
        alpha = src[buf_ix+1]
        assert alpha in {0, 255}, (index, alpha)
        assert color < 15 and color >= 0
        out_color = 0 if alpha == 0 else color+1
        if (index % 2 == 0):
            nibble = out_color
        else:
            nibble |= out_color << 4
            # linear 1-dimensional addressing
            dest[index // 2] = nibble
        index += 1
    return index // 2


oam_valid_dimensions = {8, 16, 32, 64}


def decode_data():
    with open(filename.data, "rb") as f:
        data = f.read()
    with open(filename.data_dim, "rb") as f:
        data_dim = f.read()

    dimensions = tuple([int(i) for i in data_dim.strip().split()])
    assert len(dimensions) == 2
    size = dimensions[0] * dimensions[1]
    assert dimensions[0] in oam_valid_dimensions
    assert dimensions[1] in oam_valid_dimensions
    assert len(data) // 2 == size, (len(data) // 2, size)

    oam_vram = bytearray(0x8000) # 32k
    ptr = memoryview(oam_vram)

    stride = dimensions[1]
    offset = 0
    for character in range(size // (8 * 8)):
        assert offset < len(oam_vram)
        offset += pack_character(data, ptr[offset:], character, stride)

    with open(filename.dest, "wb") as f:
        f.write(oam_vram[:offset])

decode_data()
