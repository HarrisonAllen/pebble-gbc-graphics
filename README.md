# pebble-gbc-graphics
This is the repository for the GBC Graphics engine for the Pebble smartwatch, with a small demo to get you started

I highly recommend checking out [this video](https://www.youtube.com/watch?v=ZEJVOH6Pxkg) that explains how graphics work on the Game Boy Color. This library recreates the graphics as they appear on the Game Boy Color.

# Using the library
Please refer to the `pebble-gbc-graphics` directory for an example of how to use the library.

# Projects using the pebble-gbc-graphics library
* [Pebblemon](https://github.com/HarrisonAllen/pebble-gbc-graphics-demo/tree/master/pebblemon) - App
* [Pebblemon Time](https://github.com/HarrisonAllen/pebblemon-watchface) - Watchface

# Creating tilesheets
The process for creating tilesheets has a few specific steps, I personally use [GIMP](https://www.gimp.org/):
1. Create the spritesheet using the [2bpp palette](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/helper-scripts/2bpp.gpl)
    * You can change the image color mode by going to `image`->`mode`->`indexed`->`Use custom palette`->Select `2bpp` from the list
    * [This is the example tilesheet I created](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/helper-scripts/SampleTilesheet.xcf)
2. Export the tilesheet as a `.png` file
    * [SampleTilesheet.png](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/helper-scripts/SampleTilesheet.png)
3. From the command line, run the `convert_tilesheet.py` script with your filename as arguments
    * For this sample tilesheet, the process is `python convert_tilesheet.py SampleTilesheet.png`
    * If you don't convert your image to the `2bpp` palette, then you will get the following error: `TypeError: unsupported operand type(s) for <<: 'tuple' and 'int'`
4. Now, take the `.bin` file and place it into your project resources
    * [SampleTilesheet_tilesheet.bin](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/helper-scripts/Output/SampleTilesheet_tilesheet.bin)
    * I like to place the tilesheets under `resources/data`, but you can place it anywhere
5. Now you just follow the same procedure as in this example project!