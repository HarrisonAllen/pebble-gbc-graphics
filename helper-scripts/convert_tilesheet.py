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
    x_tiles = dims[0] // TILE_SIZE
    y_tiles = dims[1] // TILE_SIZE
    output_img = Image.new("RGB", (dims[0] * dims[1] // TILE_SIZE, TILE_SIZE))
    tile_num = 0
    with open(out_filename, 'wb') as out_file:
        for y_tile in range(y_tiles):
            for x_tile in range(x_tiles):
                tile_root = (x_tile * TILE_SIZE, y_tile * TILE_SIZE)
                tile_bounds = (tile_root[0], tile_root[1],
                            tile_root[0] + TILE_SIZE, tile_root[1] + TILE_SIZE)
                tile = input_img.crop(tile_bounds)

                output_img.paste(tile, (tile_num * TILE_SIZE, 0))
                tile_num += 1
                out_file.write(convert_tile_to_bytes(tile))
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