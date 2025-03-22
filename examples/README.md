This directory contains some minimal examples for using the library

[**Example: Background Tile Example**](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/examples#background-tile-animation)

[**Tilesheet generation**](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/examples#tilesheet-generation)

## Background tile example

This example demonstrates how to display and animate tiles on a background. This project also demonstrates an implementation of "blocks", which are groups of tiles to represent a larger object.

[Background Tile Example](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/examples/background-tile-example)

![Example tilesheet](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/examples/background-tile-animation/animation.gif)

## Tilesheet generation

`tilesheet_converter.py` is used for generating tilesheets from PNGs. You can use any image, really, transparency will be applied to the resulting tiles.

### Requirements
* Python 3
* Pip libraries:
    * Pillow (8.1.0)

### Arguments
* Directory of the tilesheet to convert
* Filename of the tilesheet
* Width of blocks on the tilesheet in tiles
    * Just set to 1 if you're just using normal tiles
    * Blocks will have a height of the tilesheet

### Outputs
* `Tilesheet.bin` - The data file for the tilesheet
* `palettes.h` - A C array containing the palettes for the tilesheet (it should only contain one)

### Example
Let's take this tilesheet as our example:
![Example tilesheet](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/examples/background-tile-animation/BackgroundTiles.png)

We can run the script with `python tilesheet_converter.py background-tile-animation/ BackgroundTiles.png 2`
* The blocks are 2x2 tiles, 2 block wide as specified here, and the tilesheet is 2 tiles tall

This yields a tilesheet and a palette file. You'll add the tilesheet to your app resources, and the palettes to a file within your source code.

For reference, this is what the resulting tilesheet becomes: 
![Resulting tilesheet](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/examples/background-tile-animation/Output/References/Tilesheet.png)
