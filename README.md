# pebble-gbc-graphics
This is the repository and tutorial for the GBC Graphics library for the Pebble smartwatch.

Current version of pebble-gbc-graphics: **v1.2.0**

Want to create an app or watchface for the Pebble, but don't know where to start? Come check out the [Rebble Discord](https://discord.com/invite/aRUAYFN) and we'll help you get set up.

# Table of Contents


# Introduction
This library allows you to create graphics for your watchface, app, or game (almost) exactly like the Game Boy Color renders them. However, these rendering techniques are wrapped into an easy to use library. It is compatible with all Pebbles, but Aplite (Pebble Classic and Pebble Steel) has a much smaller RAM than the others, meaning you can only really use 1 VRAM bank with it.

### In This Repo
* [Starter Project](https://github.com/HarrisonAllen/pebble-gbc-graphics/tree/main/starter-project) - App - pebble-gbc-graphics v1.2.0
    * A barebones implementation of the graphics library, you can use this as a template!
    * ![Starter Project](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/StarterProject.png) ![Starter Project - bw](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/StarterProject-bw.png)
* [Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/tree/main/tiny-pilot) - App - pebble-gbc-graphics v1.2.0
    * A well-documented, full game that demonstrate most of the features in the library
    * ![Tiny Pilot](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/TinyPilot.png) ![Tiny Pilot - bw](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/TinyPilot-bw.png)

### Other Projects
* [Pebblemon](https://github.com/HarrisonAllen/pebble-gbc-graphics-demo/tree/master/pebblemon) - App - pebble-gbc-graphics v1.0.0
    * ![Pebblemon](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/Pebblemon.png) ![Pebblemon - bw](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/Pebblemon-bw.png)
    * A mini Pokemon adventure that does some very complex things with the engine, but isn't as easy to understand or reverse engineer.
    * The codebase is too large to fit on the Aplite platform, there isn't enough ram available.
* [Pebblemon Time](https://github.com/HarrisonAllen/pebblemon-watchface) - Watchface - pebble-gbc-graphics v1.1.0
    * ![Pebblemon Time](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/PebblemonTime.png) ![Pebblemon Time - bw](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/PebblemonTime-bw.png)
    * A stripped down version of Pebblemon that demonstrates a less-frequent draw cycle, as well as combining standard Pebble graphics with the engine.

### Background Information
If you are unfamiliar with how graphics worked on the Game Boy Color, I recommend you check out the following links:
* [How Graphics worked on the Nintendo Game Boy Color | MVG](https://youtu.be/ZEJVOH6Pxkg?t=201) (started from 3:21)
    * This is an excellent rundown of, as the title says, how the graphics worked on the gameboy color
* [Pan Docs](http://bgb.bircd.org/pandocs.htm#videodisplay)
    * For a low level analysis, check this one out. You don't need to really understand this to use the library, but it's useful to know if you want to do some more advanced techniques

### Key Differences
Some key differences between the Game Boy Color and this graphics engine:
* This engine contains up to 4 VRAM banks with 256 tiles each [link to code](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L143)
    * The VRAM itself works mostly the same, it just has a different structure
* LCDC byte has a different mapping [link to code](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L136)
* STAT byte has a different mapping [link to code](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L225)
* Attribute bytes have a different mapping [link to code](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L186)
* Dimensions of the viewport (screen) can be set and changed at runtime [link to code](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L267)
* The 10-sprites-per-line limitation on the GBC has been removed

### Other Notes
* For black and white pebbles (Pebble Classic, Pebble Steel, Pebble 2), there are 3 colors available: Black, White, and a ditherered Gray [link to code](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L115)
* You can create separate tilesheets for color and black and white Pebbles, just use the `tilesheet~color.bin` and `tilesheet~bw.bin` file naming scheme.

# The Tutorial
Let's get started! This tutorial will take a look at code from both the [Starter Project](https://github.com/HarrisonAllen/pebble-gbc-graphics/tree/main/starter-project) and [Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/tree/main/tiny-pilot).

### Quick Start
The [Starter Project](https://github.com/HarrisonAllen/pebble-gbc-graphics/tree/main/starter-project) has a basic setup for you to get started with. It demonstrates loading a tilesheet, setting palettes, and placing tiles on the background layer.

### Creating Tilesheets
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
    * This script will generate a `.bin` file that you will place in your project, and a `.png` of what the generated tilesheet looks like. Be sure to glance at the `.png` to make sure the tiles are in the order that you think they should be!
4. Now, take the `.bin` file from `assets/helper-scripts/Output` and place it into your project resources
    * [SampleTilesheet.bin](https://github.com/HarrisonAllen/pebble-gbc-graphics/raw/main/assets/helper-scripts/Output/SampleTilesheet.bin)
    * I like to place the tilesheets under `[project name]/resources/data`, but you can place it anywhere in the `resources` folder
5. To load in the tilesheet, you can follow the process in the [Starter Project](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/starter-project/src/c/main.c)

As long as you create a binary file in a 2bpp format, where each tile is 16 bytes (2 bits per pixel * 8 pixels wide * 8 pixels tall), then you can come up with your own methods to do so.

### Tiny Pilot Tilesheets
These are the tilesheets I designed and generate for Tiny Pilot. All of these can be found [here](https://github.com/HarrisonAllen/pebble-gbc-graphics/tree/main/assets/tilesheets), if you wish to edit or duplicate them.
* **Base Tilesheet** 
    * Contains the basic one-color tiles, as well as the frames for the window layer
    * ![Base Tilesheet](https://github.com/HarrisonAllen/pebble-gbc-graphics/raw/main/assets/tilesheets/BaseTilesheet.png)
* **Cloud Tilesheet**
    * Contains all of the cloud tiles
    * There is a [data structure](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/054ceeb92cf16f522b330313959ff4b0bf47bf3f/tiny-pilot/src/c/graphics/clouds.c#L9) that keeps track of the dimensions and position on the tilesheet of each cloud.
    * There are actually two tilesheets, `CloudTilesheet~color.png` and `CloudTilesheet~bw.png`. The black and white tilesheet features a border around the clouds since there is no contrast between the white of the clouds and the white of the background layer.
    * Color: ![Cloud Tilesheet - Color](https://github.com/HarrisonAllen/pebble-gbc-graphics/raw/main/assets/tilesheets/CloudTilesheet~color.png)
    * Black and white: ![Cloud Tilesheet - bw](https://github.com/HarrisonAllen/pebble-gbc-graphics/raw/main/assets/tilesheets/CloudTilesheet~bw.png)
* **Fuel Bar Tilesheet**
    * Contains the tiles for the fuel bar at the top of the score bar
    * ![Fuel Bar Tilesheet](https://github.com/HarrisonAllen/pebble-gbc-graphics/raw/main/assets/tilesheets/FuelBarTilesheet.png)
* **Road Tilesheet**
    * Contains the tiles for the road, trees, and cars at the bottom
    * ![Road Tilesheet](https://github.com/HarrisonAllen/pebble-gbc-graphics/raw/main/assets/tilesheets/RoadTilesheet.png)
* **Sprites Tilesheet**
    * Contains the tiles for the plane, balloon, +1, fuel, +F, and new high score sprites
    * ![Sprites Tilesheet](https://github.com/HarrisonAllen/pebble-gbc-graphics/raw/main/assets/tilesheets/SpritesTilesheet.png)
* **Text Tilesheet**
    * Contains the tiles for text
    * ![Text Tilesheet](https://github.com/HarrisonAllen/pebble-gbc-graphics/raw/main/assets/tilesheets/TextTilesheet.png)

##