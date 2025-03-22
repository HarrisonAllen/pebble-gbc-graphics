import os
import sys
from PIL import Image, ImageDraw
import shutil

TILE_WIDTH = 8
TILE_HEIGHT = 8
IMAGE_TRANSFORMATIONS = [
    (False, False),
    (True, False),
    (False, True),
    (True, True)
]
COLORS_PER_PALETTE = 16
NUM_PALETTES = 8
PALETTE_PIXEL_SIZE = 8

def colors_are_the_same(image_1, image_2):
    return list(image_1.convert('RGB').getdata()) == list(image_2.convert('RGB').getdata())

def alphas_are_the_same(image_1, image_2):
    return [i[-1] for i in image_1.convert('RGBA').getdata()] == [i[-1] for i in image_2.convert('RGBA').getdata()]

# Will return a tuple of:
# - index - the index of the image (None if not found)
# - x_flip - if the image is flipped in the x direction
# - y_flip - if the image is flipped in the y direction
def find_image(image_to_find, images_to_search):
    for x_flip, y_flip in IMAGE_TRANSFORMATIONS:
        flipped_image_to_find = image_to_find
        if x_flip:
            flipped_image_to_find = flipped_image_to_find.transpose(Image.FLIP_LEFT_RIGHT)
        if y_flip:
            flipped_image_to_find = flipped_image_to_find.transpose(Image.FLIP_TOP_BOTTOM)

        for image_index in range(len(images_to_search)):
            if colors_are_the_same(images_to_search[image_index], flipped_image_to_find) and alphas_are_the_same(images_to_search[image_index], flipped_image_to_find):
                return (image_index, x_flip, y_flip)
    
    return (None, None, None)

# Modifies tile_bank by adding the tile if it's unique (for unique_only)
def extract_tiles(image_filename, tile_bank, unique_only):
    image = Image.open(image_filename)
    dims = image.size
    
    for y in range(0, dims[1], TILE_HEIGHT):
        for x in range(0, dims[0], TILE_WIDTH):
            tile = image.crop((x, y, x + TILE_WIDTH, y + TILE_HEIGHT))
            if unique_only:
                image_index, _x_flip, _y_flip = find_image(tile, tile_bank)
                if image_index is None:
                    tile_bank.append(tile)
            else:
                tile_bank.append(tile)

def extract_palettes_from_images(images):
        palettes = []
        for i in range(NUM_PALETTES):
            palettes.append(set())
            palettes[-1].add((0, 0, 0, 0))
        for image in images:
            colors = image.convert("RGBA").getcolors()
            image_colors = set()
            for _count, color in colors:
                if color[3] == 0:
                    color = (0, 0, 0, 0)
                image_colors.add(color)
            
            for palette in palettes:
                if len(image_colors | palette) <= COLORS_PER_PALETTE:
                    palette |= image_colors
                    break
        return [sorted(list(p)) for p in palettes]

    
def assign_palettes_to_images(images, palettes):
    assignments = []
    
    for image in images:
        colors = image.convert("RGBA").getcolors()
        image_colors = set()
        for _count, color in colors:
            if color[3] == 0:
                color = (0, 0, 0, 0)
            image_colors.add(color)
        
        for i in range(len(palettes)):
            if image_colors.issubset(set(palettes[i])):
                assignments.append(i)
                break
    return assignments

def generate_tilesheet(tiles, palettes, palette_assignments):
    tilesheet_bytestring = ""
    for tile, palette_num in zip(tiles, palette_assignments):
        tile_bytestring = ""
        for y in range(tile.size[1]):
            for x in range(tile.size[0]):
                pixel = tile.convert("RGBA").getpixel((x,y))
                if pixel[3] == 0:
                    pixel = (0, 0, 0, 0)

                tile_bytestring += format(palettes[palette_num].index(pixel), '04b')
        tilesheet_bytestring += tile_bytestring
    return int(tilesheet_bytestring, 2).to_bytes((len(tilesheet_bytestring) + 7) // 8, byteorder="big")

def write_tilesheet_to_file(tilesheet, filename):
    with open(filename, "wb") as f_out:
        f_out.write(tilesheet)
        
def convert_rgba_to_pebble_color(color):
    pebble_color = 0b11000000
    pebble_color += (color[0] // 85) << 4 # R
    pebble_color += (color[1] // 85) << 2 # G
    pebble_color += (color[2] // 85) << 0 # Bs
    return pebble_color

def save_tilesheet_as_image(tiles, filename):
        dims = (len(tiles) * TILE_WIDTH, TILE_HEIGHT)

        tilesheet = Image.new("RGBA", dims)
        for x in range(len(tiles)):
            tilesheet.paste(tiles[x], (x * TILE_WIDTH, 0))

        tilesheet.save(filename)
        
def save_palettes_as_image(palettes, filename):
    palette_img = Image.new('RGB', (COLORS_PER_PALETTE * PALETTE_PIXEL_SIZE, NUM_PALETTES * PALETTE_PIXEL_SIZE))
    palette_draw = ImageDraw.Draw(palette_img)
    for p in range(NUM_PALETTES):
        palette = palettes[p]
        for c in range(COLORS_PER_PALETTE):
            if c >= len(palette):
                color = (0, 0, 0, 0)
            else:
                color = palette[c]
            color = color[:3] # remove alpha

            start = (c * PALETTE_PIXEL_SIZE, p * PALETTE_PIXEL_SIZE)
            end = (c * PALETTE_PIXEL_SIZE + PALETTE_PIXEL_SIZE, p * PALETTE_PIXEL_SIZE + PALETTE_PIXEL_SIZE)
            palette_draw.rectangle((start, end), fill=color, width=0)
    palette_img.save(filename)

def extract_tilesheet_tiles(tilesheet_image, block_width):
    tiles = []
    dims = tilesheet_image.size

    for x_block in range(0, dims[0], TILE_WIDTH * block_width):
        block = tilesheet_image.crop((x_block, 0, x_block + TILE_WIDTH * block_width, dims[1]))
        block_dims = block.size
        for y in range(0, block_dims[1], TILE_HEIGHT):
            for x in range(0, block_dims[0], TILE_WIDTH):
                tile = block.crop((x, y, x + TILE_WIDTH, y + TILE_HEIGHT))
                tiles.append(tile)
    return tiles
        
def convert_rgba_to_pebble_color_string(color):
    pebble_color = "0b11" # A
    pebble_color += format(color[0] // 85, '02b') # R
    pebble_color += format(color[1] // 85, '02b') # G
    pebble_color += format(color[2] // 85, '02b') # B
    return pebble_color

def convert_rgba_to_bw_pebble_color_string(color):
    color_sum = 0
    color_sum += color[0] / 85 # R
    color_sum += color[1] / 85 # G
    color_sum += color[2] / 85 # B
    color_average = round(color_sum / 3)
    if color_average > 0:
        color_average = min(color_average + 1, 0b11)
    bw_pebble_color = '0b' + format(color_average, '02b')
    return bw_pebble_color

def add_palette_data_structure_to_palettes_h(palettes_h_filename, palettes, group_name, only_use_first_palette=False):
    palette_data_structure = f"uint8_t {group_name.upper()}_PALETTES[][{COLORS_PER_PALETTE}] = {{\n"
    palette_data_structure += f"#if defined(PBL_COLOR)\n"
    for palette in palettes:
        palette_data_structure += "    {"
        for i in range(COLORS_PER_PALETTE):
            if i >= len(palette):
                color = (0, 0, 0, 0)
            else:
                color = palette[i]
            palette_data_structure += f"{convert_rgba_to_pebble_color_string(color)}, "
        palette_data_structure += "},\n"
        if only_use_first_palette:
            break

    palette_data_structure += f"#else\n"
    for palette in palettes:
        palette_data_structure += "    {"
        for i in range(COLORS_PER_PALETTE):
            if i >= len(palette):
                color = (0, 0, 0, 0)
            else:
                color = palette[i]
            palette_data_structure += f"{convert_rgba_to_bw_pebble_color_string(color)}, "
        palette_data_structure += "},\n"
        if only_use_first_palette:
            break

    palette_data_structure += f"#endif\n"
    palette_data_structure += "};\n\n"
    
    with open(palettes_h_filename, "a") as palettes_h_file:
        palettes_h_file.write(palette_data_structure)

# directory: directory tilesheet is in
# filename: the name of the tilesheet
# block_width: the width (in tiles) of any larger blocks
def convert_tilesheet(directory, filename, block_width):
    # Setup output directory
    print("Setting up output directory...")
    output_directory = os.path.join(directory, "Output")
    if os.path.exists(output_directory):
        shutil.rmtree(output_directory)
    os.makedirs(output_directory)
    output_reference_directory = os.path.join(output_directory, "References")
    os.makedirs(output_reference_directory)
    
    tilesheet_path = os.path.join(directory, filename)
    palettes_h_filename = os.path.join(output_directory, "palettes.h")

    tilesheet_image = Image.open(tilesheet_path)
    tilesheet_palettes = extract_palettes_from_images([tilesheet_image])
    tilesheet_tiles = extract_tilesheet_tiles(tilesheet_image, block_width)
    tilesheet_image_palettes = assign_palettes_to_images(tilesheet_tiles, tilesheet_palettes)


    print(f"        Extracted {len(tilesheet_tiles)} tiles.")
    assert len(tilesheet_tiles) <= 256, "    Too many tiles! Aborting..."

    # Generate tilesheet
    print("    Generating tilesheet...")
    tilesheet = generate_tilesheet(tilesheet_tiles, tilesheet_palettes, tilesheet_image_palettes)
  
    # Export tilesheet
    print("    Exporting tilesheet...")
    write_tilesheet_to_file(tilesheet, os.path.join(output_directory, "Tilesheet.bin"))
    save_tilesheet_as_image(tilesheet_tiles, os.path.join(output_reference_directory, "Tilesheet.png"))

    # Export palettes
    print("    Exporting palettes...")
    add_palette_data_structure_to_palettes_h(palettes_h_filename, tilesheet_palettes, "Tilesheet", only_use_first_palette=True)
    save_palettes_as_image(tilesheet_palettes, os.path.join(output_reference_directory, "Palettes.png"))

    print("Full output at ", output_directory)


if __name__ == "__main__":
    convert_tilesheet(sys.argv[1], sys.argv[2], int(sys.argv[3]))