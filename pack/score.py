from collections import defaultdict
import struct
import sys

from lxml import etree


frequencies = {
    "C2": 65.41,  # ~lowest GBA frequency
    "C#2": 69.30,
    "Db2": 69.30,
    "D2": 73.42,
    "D#2": 77.78,
    "Eb2": 77.78,
    "E2": 82.41,
    "Fb2": 82.41,
    "F2": 87.31,
    "F#2": 92.50,
    "Gb2": 92.50,
    "G2": 98.00,
    "G#2": 103.83,
    "Ab2": 103.83,
    "A2": 110.00,
    "A#2": 116.54,
    "Bb2": 116.54,
    "B2": 123.47,
    "Cb3": 123.47,
    "C3": 130.81,
    "C#3": 138.59,
    "Db3": 138.59,
    "D3": 146.83,
    "D#3": 155.56,
    "Eb3": 155.56,
    "E3": 164.81,
    "Fb3": 164.81,
    "F3": 174.61,
    "F#3": 185.00,
    "Gb3": 185.00,
    "G3": 196.00,
    "G#3": 207.65,
    "Ab3": 207.65,
    "A3": 220.00,
    "A#3": 233.08,
    "Bb3": 233.08,
    "B3": 246.94,
    "Cb4": 246.94,
    "C4": 261.63,
    "C#4": 277.18,
    "Db4": 277.18,
    "D4": 293.66,
    "D#4": 311.13,
    "Eb4": 311.13,
    "E4": 329.63,
    "Fb4": 329.63,
    "F4": 349.23,
    "F#4": 369.99,
    "Gb4": 369.99,
    "G4": 392.00,
    "G#4": 415.30,
    "Ab4": 415.30,
    "A4": 440.00,
    "A#4": 466.16,
    "Bb4": 466.16,
    "B4": 493.88,
    "Cb5": 493.88,
    "C5": 523.25,
    "C#5": 554.37,
    "Db5": 554.37,
    "D5": 587.33,
    "D#5": 622.25,
    "Eb5": 622.25,
    "E5": 659.25,
    "Fb5": 659.25,
    "F5": 698.46,
    "F#5": 739.99,
    "Gb5": 739.99,
    "G5": 783.99,
    "G#5": 830.61,
    "Ab5": 830.61,
    "A5": 880.00,
    "A#5": 932.33,
    "Bb5": 932.33,
    "B5": 987.77,
    "Cb6": 987.77,
    "C6": 1046.50,
    "C#6": 1108.73,
    "Db6": 1108.73,
    "D6": 1174.66,
    "D#6": 1244.51,
    "Eb6": 1244.51,
    "E6": 1318.51,
    "Fb6": 1318.51,
    "F6": 1396.91,
    "F#6": 1479.98,
    "Gb6": 1479.98,
    "G6": 1567.98,
    "G#6": 1661.22,
    "Ab6": 1661.22,
    "A6": 1760.00,
    "A#6": 1864.66,
    "Bb6": 1864.66,
    "B6": 1975.53,
    "Cb7": 1975.53,
    "C7": 2093.00,
    "C#7": 2217.46,
    "Db7": 2217.46,
    "D7": 2349.32,
    "D#7": 2489.02,
    "Eb7": 2489.02,
    "E7": 2637.02,
    "Fb7": 2637.02,
    "F7": 2793.83,
    "F#7": 2959.96,
    "Gb7": 2959.96,
    "G7": 3135.96,
    "G#7": 3322.44,
    "Ab7": 3322.44,
    "A7": 3520.00,
    "A#7": 3729.31,
    "Bb7": 3729.31,
    "B7": 3951.07,
    "Cb8": 3951.07,
    "C8": 4186.01,
    "C#8": 4434.92,
    "Db8": 4434.92,
    "D8": 4698.63,
    "D#8": 4978.03,
    "Eb8": 4978.03,
    "E8": 5274.04,
    "Fb8": 5274.04,
    "F8": 5587.65,
    "F#8": 5919.91,
    "Gb8": 5919.91,
    "G8": 6271.93,
    "G#8": 6644.88,
    "Ab8": 6644.88,
    "A8": 7040.00,
    "A#8": 7458.62,
    "Bb8": 7458.62,
    "B8": 7902.13,
}


def as_n(freq: float) -> int:
    assert int(freq) != 0
    n = (2048 * (freq - 64)) / freq
    if (n < 1 and n > 0):
        n = 1 # forcibly round 0 up to 1
    assert (round(n) > 0)
    return round(n)


def _key(octave, step, alter):
    sign = "b" if alter == -1 else "#" if alter == 1 else ""
    return "{step}{sign}{octave}".format(step=step, sign=sign, octave=octave)


def parse_note(measure_number, note, x_offset):
    voice = int(note.xpath('./voice/text()')[0])
    note_x = float(note.get("default-x", 0.0))

    #if not (note_x > x_offset[voice]):
    #    print("discard", (note_x))
    #    return
    #else:
    x_offset[voice] = note_x

    duration = int(note.xpath('./duration/text()')[0])
    assert duration < 256, (note_x)

    if note.xpath('./rest'):
        yield voice, duration, 0
    elif note.xpath('./unpitched'):
        yield voice, duration, 0xffff
    else:
        octave = int(note.xpath('./pitch/octave/text()')[0])
        step = note.xpath('./pitch/step/text()')[0]
        alter = note.xpath('./pitch/alter/text()') # 1 sharp -1 flat
        alter = int(next(iter(alter), 0))

        try:
            frequency = frequencies[_key(octave, step, alter)]
        except KeyError:
            print("measure", measure_number)
            raise

        n = as_n(frequency)
        assert n < (2 ** 11), (frequency, n)
        #print(voice, octave, step, alter, duration)

        yield voice, duration, n


def parse_measures(measures):
    measure_number = 0
    last_duration = -1

    for measure in measures:
        assert int(measure.get('number')) > measure_number
        measure_number = int(measure.get('number'))

        #attributes_path = measure.xpath('./attributes')
        #if attributes_path:
        #    assert len(attributes_path) == 1, attributes_path
        #    attributes, = attributes_path

        x_offset = defaultdict(lambda: -1.0)
        measure_duration = 0
        for note in measure.xpath('./note'):
            for voice, duration, frequency in parse_note(measure_number, note, x_offset):
                measure_duration += duration
                yield voice, (duration, frequency)

        # time signature changes require more thought
        if last_duration == -1:
            last_duration = measure_duration
        else:
            assert last_duration == measure_duration, (measure_duration, measure_number)


def parse_parts(root):
    parts = root.xpath('//part')
    for part in parts:
        part_id = part.get("id")
        assert part_id
        measures = part.xpath('./measure')
        for voice, d__f in parse_measures(measures):
            yield (part_id, voice), d__f


def main():
    with open(sys.argv[1], 'rb') as f:
        root = etree.fromstring(f.read())

    voices = defaultdict(list)
    for p__v, d__f in parse_parts(root):
        voices[p__v].append(d__f)

    prefix = sys.argv[2]
    for k, v in voices.items():
        fn = "{prefix}_part_{k_0}_voice_{k_1}.dfreq".format(prefix=prefix, k_0=k[0], k_1=k[1])
        print(fn)
        with open(fn, "wb") as f:
            for duration, frequency in v:
                f.write(struct.pack("<BBH", duration, 0, frequency))


main()
