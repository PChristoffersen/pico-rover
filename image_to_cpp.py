#!/usr/bin/env python3

from email import header
import math
from argparse import ArgumentParser
from pathlib import Path
from PIL import Image


OLED_PAGE_HEIGHT = 8


def load_image(img_path: Path) -> tuple[bytearray,int,int]:
    try:
        im = Image.open(img_path)
    except OSError:
        raise Exception("Oops! The image could not be opened.")

    if not (im.mode == "1" or im.mode == "L"):
        im = im.convert("L")
        #    raise Exception("Image must be grayscale only")

    img_width = im.size[0]
    img_height = im.size[1]

    img_height_data = im.size[1]
    img_height = math.ceil(img_height/OLED_PAGE_HEIGHT)*OLED_PAGE_HEIGHT

    # black or white
    out = im.convert("1")
    pixels = list(out.getdata())

    # swap white for black and swap (255, 0) for (1, 0)
    pixels = [0 if x > 127 else 1 for x in pixels]

    # Convert the pixel array to OLED pages
    img_data = bytearray()
    for i in range(img_height // OLED_PAGE_HEIGHT):
        start_index = i*img_width*OLED_PAGE_HEIGHT
        for j in range(img_width):
            out_byte = 0
            for k in range(OLED_PAGE_HEIGHT):
                if i*OLED_PAGE_HEIGHT+k < img_height_data:
                    out_byte |= pixels[k*img_width + start_index + j] << k
            img_data.append(out_byte)



    return img_width, img_height, img_data


def write_header(img_name: str, img_width: int, img_height: int, img_data: bytearray, header_path: Path):
    header_path.parent.mkdir(exist_ok=True)
    with open(f'{header_path}', 'wt') as file:
        file.write(f"#pragma once\n")
        file.write(f"\n")
        file.write(f"#include <oled/image.h>\n")
        file.write(f"\n")
        file.write(f"namespace OLED::Resource::Image {{\n")
        file.write(f"\n")
        file.write(f"    static constexpr uint _{img_name}_width {{ {img_width} }};\n")
        file.write(f"    static constexpr uint _{img_name}_height {{ {img_height} }};\n")
        file.write(f"    static constexpr ::OLED::Image::column_type _{img_name}_data[{img_width*(img_height//OLED_PAGE_HEIGHT)}] = {{\n")
        for row in range(int(img_height/OLED_PAGE_HEIGHT)):
            file.write(f"        ")
            data = img_data[row*img_width:row*img_width+img_width]
            file.write(", ".join([f'{b:#04x}' for b in data]))
            file.write(f",\n")
        file.write(f"    }};\n")
        file.write(f"\n")
        file.write(f"    static constexpr ::OLED::Image {img_name} {{ _{img_name}_width, _{img_name}_height, _{img_name}_data }};\n")
        file.write(f"\n")
        file.write(f"}}\n")
        file.write(f"\n")



if __name__ == '__main__':
    parser = ArgumentParser(description='Convert image c++ header')
    parser.add_argument('image_path', type=Path)
    parser.add_argument('out_name', type=str)
    parser.add_argument('header_path', type=Path)
    
    args = parser.parse_args()

    img_name = args.image_path.stem

    img_width, img_height, img_data = load_image(args.image_path)
    
    write_header(args.out_name, img_width, img_height, img_data, args.header_path)

