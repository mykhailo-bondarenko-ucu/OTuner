from matplotlib import pyplot as plt
import numpy as np

NOTES = (
    (16.35, "C0"),(17.32, "C#0"),(18.35, "D0"),(19.45, "D#0"),(20.60, "E0"),(21.83, "F0"),(23.12, "F#0"),(24.50, "G0"),(25.96, "G#0"),(27.50, "A0"),(29.14, "A#0"),(30.87, "B0"),
    (32.70, "C1"),(34.65, "C#1"),(36.71, "D1"),(38.89, "D#1"),(41.20, "E1"),(43.65, "F1"),(46.25, "F#1"),(49.00, "G1"),(51.91, "G#1"),(55.00, "A1"),(58.27, "A#1"),(61.74, "B1"),
    (65.41, "C2"),(69.30, "C#2"),(73.42, "D2"),(77.78, "D#2"),(82.41, "E2"),(87.31, "F2"),(92.50, "F#2"),(98.00, "G2"),(103.83, "G#2"),(110.00, "A2"),(116.54, "A#2"),(123.47, "B2"),
    (130.81, "C3"),(138.59, "C#3"),(146.83, "D3"),(155.56, "D#3"),(164.81, "E3"),(174.61, "F3"),(185.00, "F#3"),(196.00, "G3"),(207.65, "G#3"),(220.00, "A3"),(233.08, "A#3"),(246.94, "B3"),
    (261.63, "C4"),(277.18, "C#4"),(293.66, "D4"),(311.13, "D#4"),(329.63, "E4"),(349.23, "F4"),(369.99, "F#4"),(392.00, "G4"),(415.30, "G#4"),(440.00, "A4"),(466.16, "A#4"),(493.88, "B4"),
    (523.25, "C5"),(554.37, "C#5"),(587.33, "D5"),(622.25, "D#5"),(659.25, "E5"),(698.46, "F5"),(739.99, "F#5"),(783.99, "G5"),(830.61, "G#5"),(880.00, "A5"),(932.33, "A#5"),(987.77, "B5"),
    (1046.50, "C6"),(1108.73, "C#6"),(1174.66, "D6"),(1244.51, "D#6"),(1318.51, "E6"),(1396.91, "F6"),(1479.98, "F#6"),(1567.98, "G6"),(1661.22, "G#6"),(1760.00, "A6"),(1864.66, "A#6"),(1975.53, "B6"),
    (2093.00, "C7"),(2217.46, "C#7"),(2349.32, "D7"),(2489.02, "D#7"),(2637.02, "E7"),(2793.83, "F7"),(2959.96, "F#7"),(3135.96, "G7"),(3322.44, "G#7"),(3520.00, "A7"),(3729.31, "A#7"),(3951.07, "B7"),
    (4186.01, "C8"),(4434.92, "C#8"),(4698.63, "D8"),(4978.03, "D#8"),(5274.04, "E8"),(5587.65, "F8"),(5919.91, "F#8"),(6271.93, "G8"),(6644.88, "G#8"),(7040.00, "A8"),(7458.62, "A#8"),(7902.13, "B8")
)

NOTES_C_HEADER = """#include <notes.h>

Note NOTES[108] = {
"""

NOTES_C_FOOTER = """};"""

def read_font(skip_fonts=0):
    with open("fonts.c", "r") as fonts_file:
        lines = fonts_file.readlines()

        def skip_until_font(start, no_font=False):
            for i in range(start, len(lines)):
                if (lines[i].startswith("0x") ^ no_font):
                    return i

        font_start = skip_until_font(0)
        for _ in range(skip_fonts):
            font_start = skip_until_font(font_start, True)
            font_start = skip_until_font(font_start)
        font_end = skip_until_font(font_start, True)

        font_dict = dict()
        for line_i, char_ord in zip(range(font_start, font_end), range(32, 127)):
            font_dict[char_ord] = np.array(list(map(
                lambda hex_str: list(map(int, bin(int(hex_str, 16)).split("b")[1].rjust(16, "0"))),
                lines[line_i].split("/")[0].split(",")[:-1]
            )))
        
        return font_dict

def place_font_at_coords(img, font, char, coords):
    h, w, fontface = font[0], font[1], font[2][ord(char)]
    img[coords[0]:coords[0]+h, coords[1]:coords[1]+w] = fontface[:h, :w]
    return coords[0]+h, coords[1]+w

def place_v_sep(img, i, j, v_sep):
    img[0:i,j:j+v_sep] = 0
    return i, j + v_sep

def to_bitstr(img, typebits=32, height=26):
    h, w = height, typebits
    img = img[:h,:w]
    on = np.zeros((h, w))
    for i in range(h):
        for j in range(w):
            if img[i, j] == 1:
                on[i, j] = 1
    bit_strs = []
    for line in on:
        bits = "".join(map(lambda x: str(int(x)), line))
        bit_strs.append("0x" + hex(int(bits, 2)).split("x")[1].rjust(typebits // 4, "0").upper())

    for _ in range(height - h):
        bit_strs.append("0x" + "".rjust(typebits // 4, "0"))

    return ",".join(bit_strs)

def main():
    # screen resolution is 64x128
    img = np.zeros((32, 64))

    font7x10 = [10, 7, read_font(0)]
    font11x18 = [18, 11, read_font(1)]
    font16x26 = [26, 16, read_font(2)]

    with open("notes.c", "w") as output_file:
        output_file.write(NOTES_C_HEADER)
        notes = []
        for note_freq, note_str in NOTES:
            img[:,:] = 0.1
            letter, has_diez = note_str[0], len(note_str) == 3
            num = note_str[2] if has_diez else note_str[1]

            i, j = place_font_at_coords(img, font16x26, letter, (0, 0))
            i, j = place_v_sep(img, i, j, 2)
            if has_diez:
                place_font_at_coords(img, font7x10, "#", (0, j))
            place_font_at_coords(img, font7x10, num, (i-10, j))

            # using uint32_t, height = 26
            notes.append(f'    {"{"}{note_freq}, "{note_str}", {"{"}{to_bitstr(img, 32, 26)}{"}"}{"}"}')

            # plt.imshow(img)
            # plt.show()
        output_file.write(",\n".join(notes) + "\n")
        output_file.write(NOTES_C_FOOTER)

if __name__ == "__main__":
    main()