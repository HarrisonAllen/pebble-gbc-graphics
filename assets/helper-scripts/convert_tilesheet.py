import os
from PIL import Image
import sys

TILE_WIDTH = 8 # The width of a tile, in pixels
TILE_HEIGHT = 8 # The height of a tile, in pixels

""" 
Converts a tile (or any image, really) to a byte array

This assumes that the tile uses a 2bpp palette, i.e. the
color are 0b00, 0b01, 0b10, and 0b11. You can change this to
work however you want
"""
def convert_tile_to_bytes(tile):
    result = []
    colors = []
    for y in range(tile.size[1]):
        for x in range(tile.size[0]):
            colors.append(tile.getpixel((x, y)))
            if len(colors) == 4: # 4 sets of 2 bits == 1 byte
                # This assumes a 2bpp palette. If you want to use other colors
                # (e.g. grayscale), you would have to change this line to make
                # colors 2 bits each depending on your setup
                b = colors[0] << 6 | colors[1] << 4 | colors[2] << 2 | colors[3]
                result.append(b)
                colors = []
    return bytes(result)
    
"""
Converts a tilesheet to a 2bpp binary file

This assumes that you've exported the image as png with a 2bpp palette
"""
def convert_tilesheet_to_2bpp(sheet_filename, out_filename, img_out_filename=None):
    input_img = Image.open(sheet_filename)
    dims = input_img.size
    x_tiles = dims[0] // TILE_WIDTH
    y_tiles = dims[1] // TILE_HEIGHT
    output_img = Image.new("RGB", (dims[0] * dims[1] // TILE_WIDTH, TILE_HEIGHT)) # A long ribbon of tiles
    tile_num = 0
    with open(out_filename, 'wb') as out_file:
        for y_tile in range(y_tiles):
            for x_tile in range(x_tiles):
                # cut a tile out of the base image
                tile_root = (x_tile * TILE_WIDTH, y_tile * TILE_HEIGHT)
                tile_bounds = (tile_root[0], tile_root[1],
                            tile_root[0] + TILE_WIDTH, tile_root[1] + TILE_HEIGHT)
                tile = input_img.crop(tile_bounds)

                # Paste the tile onto the ribbon
                output_img.paste(tile, (tile_num * TILE_WIDTH, 0))
                tile_num += 1

                # And convert the tile, save it to the output file
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
            print(f'Converting {os.path.basename(name)}...')
            convert_tilesheet_to_2bpp(name, "Output/" + base_name + ".bin", "Output/" + base_name + ".png")
            print(f'Complete!')
    else:
        print('Please enter the filename of the tilesheet you would like to convert')