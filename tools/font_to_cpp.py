#!/usr/bin/env python3

import math
import subprocess
from argparse import ArgumentParser
from pathlib import Path
from dataclasses import dataclass
from PIL import ImageFont


OLED_PAGE_HEIGHT = 8


@dataclass
class Glyph:
    char: int
    width: int
    height: int
    x_off: int
    y_off: int
    advance: int
    data: bytearray



def load_font(font_name: str, font_size: int) -> ImageFont:
    # Try the name directly
    try:
        font = ImageFont.truetype(font = font_name, size = font_size)
        return font
    except:
        pass

    # Try fontconfig fc-match
    try:
        path = subprocess.check_output(["fc-match", "--format", "%{file}", font_name]).decode('utf-8')
        font = ImageFont.truetype(font = path, size = font_size)
        return font
    except:
        pass

    raise Exception("Unable to load font")


def render_glyphs(font: ImageFont, glyph_range):
    glyphs = []
    for i in range(glyph_range[0], glyph_range[1]):
        ch = chr(i)
        length = round(font.getlength(ch))

        mask, off = font.getmask2(ch, mode="1")
        if not mask:
            glyphs.append(Glyph(
                char = i,
                width=0,
                height=0,
                x_off=0,
                y_off=0,
                advance=length,
                data=None
            ))
            continue

        mask_w, mask_h = mask.size

        off_x = off[0]
        pix_w = mask_w

        off_y = 0
        pix_h = off[1]+mask_h
        oy = off[1]
        while oy>=8:
            oy-=8
            pix_h -= 8
            off_y += 8

        # Round height up to nearest 8
        pix_h = math.ceil(pix_h/8)*8

        data = bytearray(pix_w * (pix_h//8))

        for y in range(mask.size[1]):
            doff = pix_w*((y+oy) // 8) 
            soff = mask.size[0]*y
            bit = ((y+oy) % 8)
            #print(f"{y}: {bit}")
            for x in range(mask.size[0]):
                data[doff+x] |= (1 if mask[soff+x]>127 else 0) << bit

        #print(f"{ch} {i:02x}: len={length:2d}    pix={mask.size}    off={off} ({oy})    {off_x},{off_y} -> {pix_w}x{pix_h}   b={len(data)}")

        glyphs.append(Glyph(
            char=i,
            width=pix_w,
            height=pix_h,
            x_off=off_x,
            y_off=off_y,
            advance=length,
            data=data
        ))


    return glyphs



def write_header(font_name: str, font: ImageFont, font_height: int, glyph_range, glyphs: list[Glyph], header_path: Path):
    descent, ascent = font.getmetrics()
    
    header_path.parent.mkdir(exist_ok=True)
    with open(f'{header_path}', 'wt') as file:
        file.write(f"#pragma once\n")
        file.write(f"\n")
        file.write(f"#include <ssd1306/font.h>\n")
        file.write(f"\n")
        file.write(f"namespace OLED::Resource::Font {{\n")
        file.write(f"\n")

        file.write(f"    static constexpr ::SSD1306::Font::column_type _{font_name}_data[] = {{\n")
        for glyph in glyphs:
            if not glyph.data: 
                continue
            file.write(f"        ")
            file.write(", ".join([f'{b:#04x}' for b in glyph.data]))
            file.write(f",\n")

        file.write(f"    }};\n")
        file.write(f"\n")

        file.write(f"    static constexpr ::SSD1306::Font::Glyph _{font_name}_glyphs[] = {{\n")
        off = 0
        for glyph in glyphs:
            file.write(f"        {{ 0x{off:04x}, {glyph.advance}, {glyph.width}, {glyph.height//8}, {glyph.x_off}, {glyph.y_off//8} }}, // 0x{glyph.char:02x} ('{chr(glyph.char)}')\n")

            if glyph.data:
                off += len(glyph.data)

        file.write(f"    }};\n")
        file.write(f"\n")
        file.write(f"    static constexpr ::SSD1306::DynamicFont {font_name} {{\n")
        file.write(f"        {font_size}, // Size\n")
        file.write(f"        {descent}, // Descent\n")
        file.write(f"        {ascent}, // Ascent\n")
        file.write(f"        0x{glyph_range[0]:02x}u, // First char\n")
        file.write(f"        0x{glyph_range[1]:02x}u, // Last char\n")
        file.write(f"        _{font_name}_glyphs,\n")
        file.write(f"        _{font_name}_data\n")
        file.write(f"    }};\n")
        file.write(f"\n")
        file.write(f"}}\n")
        file.write(f"\n")



if __name__ == '__main__':
    parser = ArgumentParser(description='Convert image c++ header')
    parser.add_argument('font_name', type=str)
    parser.add_argument('font_size', type=int)
    parser.add_argument('out_name', type=str)
    parser.add_argument('header_path', type=Path)
    
    args = parser.parse_args()

    font_size = args.font_size

    font = load_font(args.font_name, font_size)

    glyph_range = (0x20, 0x7F)
    glyphs = render_glyphs(font, glyph_range)
    out_name = args.out_name

    write_header(out_name, font, font_size, glyph_range, glyphs, args.header_path)

