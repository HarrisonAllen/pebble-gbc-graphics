# pebble-gbc-graphics
This is the repository and tutorial for the GBC Graphics library for the Pebble smartwatch.

Current version of pebble-gbc-graphics: **v1.2.0**

Want to create an app or watchface for the Pebble, but don't know where to start? Come check out the [Rebble Discord](https://discord.com/invite/aRUAYFN) and we'll help you get set up.

(insert table of contents here)

# Introduction
This library allows you to create graphics for your watchface, app, or game (almost) exactly like the Game Boy Color renders them.

In this repo:
* [Starter Project](https://github.com/HarrisonAllen/pebble-gbc-graphics/tree/main/starter-project) - App - pebble-gbc-graphics v1.2.0
    * A barebones implementation of the grpahics library, you can use this as a template!
* [Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/tree/main/tiny-pilot) - App - pebble-gbc-graphics v1.2.0
    * A well-documented, full game that demonstrate most of the features in the library

![Tiny Pilot](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/TinyPilot.png)

Some other projects using this library:
* [Pebblemon](https://github.com/HarrisonAllen/pebble-gbc-graphics-demo/tree/master/pebblemon) - App - pebble-gbc-graphics v1.0.0

![Pebblemon](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/Pebblemon.png)

* [Pebblemon Time](https://github.com/HarrisonAllen/pebblemon-watchface) - Watchface - pebble-gbc-graphics v1.1.0

![Pebblemon Time](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/PebblemonTime.png)

The complexities of rendering are abstracted away for the user. For example, the viewport over the background can be moved simply by using the function [`GBC_Graphics_bg_move`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L766).

If you are unfamiliar with how graphics worked on the Game Boy Color, I recommend you check out the following links:
* [How Graphics worked on the Nintendo Game Boy Color | MVG](https://youtu.be/ZEJVOH6Pxkg?t=201) (started from 3:21)
    * This is an excellent rundown of, as the title says, how the graphics worked on the gameboy color
* [Pan Docs](http://bgb.bircd.org/pandocs.htm#videodisplay)
    * For a low level analysis, check this one out. You don't need to really understand this to use the library, but it's useful to know if you want to do some more advanced techniques

Some key differences between the Game Boy Color and this graphics engine:
* This engine contains up to 4 VRAM banks with 256 tiles each [link to code](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L143)
    * The VRAM itself works te same, it just has a different structure
* LCDC byte has a different mapping [link to code](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L136)
* STAT byte has a different mapping [link to code](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L225)
* Attribute bytes have a different mapping [link to code](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L186)
* Dimensions of the viewport (screen) can be set and changed at runtime [link to code](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L267)
* The 10-sprites-per-line limitation has been removed

Other notes:
* For black and white pebbles (Pebble Classic, Pebble Steel, Pebble 2), there are 3 colors available: Black, White, and a ditherered Gray [link to code](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L115)

# Creating Tilesheets
The process for creating tilesheets from an image has a few specific steps, I personally use [GIMP](https://www.gimp.org/):
1. Create the spritesheet using the [2bpp palette](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/2bpp.gpl)
    * For the easiest setup, just modify [SampleTilesheet.xcf](https://github.com/HarrisonAllen/pebble-gbc-graphics/raw/main/assets/tilesheets/SampleTilesheet.xcf). The image should already be in an indexed color mode, and the palette should already exist as `Colormap of image #xx` in the Palettes window (`Windows`->`Dockable Dialogs`->`Palettes`).
    * Here is an example of what a tile will look like on a tilesheet (left), the palette that will be applied to the tile in-game (middle), and the resulting paletted tile that will be rendered in-game
    * ![Palette Example](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/PaletteExample.png)
    * To convert a preexisting image to a 2bpp palette, you can change the image color mode by going to `image`->`mode`->`indexed`->`Use custom palette`->Select `2bpp` from the list. [This guide](https://daviesmediadesign.com/how-to-install-custom-palettes-in-gimp/) may or may not do the trick if you want to add the palette to your installation
    * I prefer to create my tilesheets in a ribbon, but they can be in any shape. Just keep in mind that the conversion script will scan your tilesheet from left to right, top to bottom, one tile at a time.
2. Export the tilesheet as a `.png` file
    * ![SampleTilesheet.png](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/tilesheets/SampleTilesheet.png)
3. From the command line, run the [`convert_tilesheet.py`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/assets/helper-scripts/convert_tilesheet.py) script with your filename as arguments
    * For the Sample Tilesheet:
        1. `cd` to the `assets/helper-scripts/` directory
        2. Run `python convert_tilesheet.py ../tilesheets/SampleTilesheet.png`
    * This script requires Python 3 and [Pillow](https://pypi.org/project/Pillow/)
    * You can convert multiple files at once, just add on multiple filenames as arguments (for example `python convert_tilesheet.py tilesheet1.png tilesheet2.png tilesheet3.png`)
    * If you get an error such as: `TypeError: unsupported operand type(s) for <<: 'tuple' and 'int'`, this probably means your image was not converted to the 2bpp color mode.
    * This script will generate a `.bin` file that you will place in your project, and a `.png` of what the generated tilesheet looks like. Be sure to glance at the `.png` to make sure the tiles are in the you think they should be!
4. Now, take the `.bin` file from `assets/helper-scripts/Output` and place it into your project resources
    * [SampleTilesheet.bin](https://github.com/HarrisonAllen/pebble-gbc-graphics/raw/main/assets/helper-scripts/Output/SampleTilesheet.bin)
    * I like to place the tilesheets under `[project name]/resources/data`, but you can place it anywhere in the `resources` folder
5. To load in the tilesheet, you can follow the process in the [Sample Project](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/starter-project/src/c/main.c)