import os
import glob
from PIL import Image
import sys

CHUNK_SIZE = 32
BLOCK_SIZE = 16
TILE_SIZE = 8
SPRITE_SIZE = 16

def convert_tile_to_bytes(tile):
    result = []
    colors = []
    for y in range(tile.size[1]):
        for x in range(tile.size[0]):
            colors.append(tile.getpixel((x, y)))
            if len(colors) == 4:
                b = colors[0] << 6 | colors[1] << 4 | colors[2] << 2 | colors[3]
                result.append(b)
                colors = []
    return bytes(result)
    
def convert_tilesheet_to_2bpp(sheet_filename, out_filename, img_out_filename=None):
    input_img = Image.open(sheet_filename)
    dims = input_img.size
    x_sprites = dims[0] // SPRITE_SIZE
    y_sprites = dims[1] // SPRITE_SIZE
    output_img = Image.new("RGB", (dims[0] * dims[1] // TILE_SIZE, TILE_SIZE))
    i = 0
    with open(out_filename, 'wb') as out_file:
        for y_sprite in range(y_sprites):
            for x_sprite in range(x_sprites):
                for y_tile_root in (y_sprite * SPRITE_SIZE, y_sprite * SPRITE_SIZE + TILE_SIZE):
                    for x_tile_root in (x_sprite * SPRITE_SIZE, x_sprite * SPRITE_SIZE + TILE_SIZE):
                        tile_root = (x_tile_root, y_tile_root)
                        tile_bounds = (tile_root[0], tile_root[1],
                                    tile_root[0] + TILE_SIZE, tile_root[1] + TILE_SIZE)
                        tile = input_img.crop(tile_bounds)
                        output_img.paste(tile, (i * TILE_SIZE, 0))
                        i += 1
                        out_file.write(convert_tile_to_bytes(tile))
    print(f'Size reduced from {dims[0] * dims[1]} to {os.path.getsize(out_filename)}')
    if img_out_filename:
        output_img.save(img_out_filename)
    else:
        output_img.show()
        
if __name__ == "__main__":
    input_args = sys.argv[1:]

    if input_args:
        if not os.path.exists("Output/"):
            os.makedirs("Output/")
        for name in input_args:
            base_name = os.path.splitext(os.path.basename(name))[0]
            convert_tilesheet_to_2bpp(name, "Output/" + base_name + "_tilesheet.bin", "Output/" + base_name + "_tilesheet.png")
    else:
        print('Please enter the filename of the tilesheet you would like to convert')