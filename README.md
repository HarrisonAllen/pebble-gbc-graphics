# pebble-gbc-graphics
This is the repository and tutorial for the GBC Graphics library for the Pebble smartwatch.

Current version of pebble-gbc-graphics: **v1.2.0**

Want to create an app or watchface for the Pebble, but don't know where to start? Come check out the [Rebble Discord](https://discord.com/invite/aRUAYFN) and we'll help you get set up.

# Table of Contents

### [Introduction](https://github.com/HarrisonAllen/pebble-gbc-graphics#introduction)
* [In This Repo](https://github.com/HarrisonAllen/pebble-gbc-graphics#in-this-repo)
* [Other Projects](https://github.com/HarrisonAllen/pebble-gbc-graphics#other-projects)
* [Background Information](https://github.com/HarrisonAllen/pebble-gbc-graphics#background-information)
* [Key Differences](https://github.com/HarrisonAllen/pebble-gbc-graphics#key-differences)
* [Other Notes](https://github.com/HarrisonAllen/pebble-gbc-graphics#other-notes)

### [The Tutorial](https://github.com/HarrisonAllen/pebble-gbc-graphics#the-tutorial)
* [Quick Start](https://github.com/HarrisonAllen/pebble-gbc-graphics#quick-start)
* [Creating Tilesheets](https://github.com/HarrisonAllen/pebble-gbc-graphics#creating-tilesheets)
    * [Tiny Pilot Tilesheets](https://github.com/HarrisonAllen/pebble-gbc-graphics#tiny-pilot-tilesheets)

# Introduction
This library allows you to create graphics for your watchface, app, or game (almost) exactly like the Game Boy Color renders them. However, these rendering techniques are wrapped into an easy to use library. It is compatible with all Pebbles.

## In This Repo
* [Starter Project](https://github.com/HarrisonAllen/pebble-gbc-graphics/tree/main/starter-project) - App - pebble-gbc-graphics v1.2.0
    * A barebones implementation of the graphics library, you can use this as a template!
    * ![Starter Project](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/StarterProject.png) ![Starter Project - bw](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/StarterProject-bw.png)
* [Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/tree/main/tiny-pilot) - App - pebble-gbc-graphics v1.2.0
    * A well-documented, full game that demonstrates most of the features in the library
    * ![Tiny Pilot](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/TinyPilot.png) ![Tiny Pilot - bw](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/TinyPilot-bw.png)

## Other Projects
* [Pebblemon](https://github.com/HarrisonAllen/pebble-gbc-graphics-demo/tree/master/pebblemon) - App - pebble-gbc-graphics v1.0.0
    * ![Pebblemon](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/Pebblemon.png) ![Pebblemon - bw](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/Pebblemon-bw.png)
    * A mini Pokemon adventure that does some very complex things with the engine, but isn't as easy to understand or reverse engineer.
    * The codebase is too large to fit on the Aplite platform, there isn't enough ram available.
* [Pebblemon Time](https://github.com/HarrisonAllen/pebblemon-watchface) - Watchface - pebble-gbc-graphics v1.1.0
    * ![Pebblemon Time](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/PebblemonTime.png) ![Pebblemon Time - bw](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/PebblemonTime-bw.png)
    * A stripped down version of Pebblemon that demonstrates a less-frequent draw cycle, as well as combining standard Pebble graphics with the engine.

## Background Information
If you are unfamiliar with how graphics worked on the Game Boy Color, I recommend you check out the following links:
* [How Graphics worked on the Nintendo Game Boy Color | MVG](https://youtu.be/ZEJVOH6Pxkg?t=201) (started from 3:21)
    * This is an excellent rundown of, as the title says, how the graphics worked on the gameboy color
* [Pan Docs](http://bgb.bircd.org/pandocs.htm#videodisplay)
    * For a low level analysis, check this one out. You don't need to really understand this to use the library, but it's useful to know if you want to do some more advanced techniques

## Key Differences
Some key differences between the Game Boy Color and this graphics engine:
* This engine contains up to 4 VRAM banks with 256 tiles each [link to code](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L143)
    * The VRAM itself works mostly the same, it just has a different structure
* LCDC byte has a different mapping [link to code](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L124-L136)
* STAT byte has a different mapping [link to code](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L214-L225)
* Attribute bytes have a different mapping [link to code](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L175-L186)
* Dimensions of the viewport (screen) can be set and changed at runtime [link to code](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L260-L346)
* The 10-sprites-per-line limitation on the GBC has been removed

## Other Notes
* For black and white pebbles (Pebble Classic, Pebble Steel, Pebble 2), there are 3 colors available: Black, White, and a ditherered Gray [link to code](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L115)
* You can create separate tilesheets for color and black and white Pebbles, just use the `tilesheet~color.bin` and `tilesheet~bw.bin` file naming scheme.

# Understanding the Engine
Let's get started! First, I'll go over some basics about the engine.

## Understanding Tiles
The Game Boy Color uses tiles to render its graphics. A tile is an 8 pixel by 8 pixel image that can have 4 colors. These colors are defined in a palette.

The tile itself is stored in a format known as 2 bits per pixel, or 2bpp. A pixel corresponding to 00 will take the first color in a palette, a pixel corresponding to 01 will take the second color in a palette, etc. If we map those pixels to various shades of gray (00 = black, 01 = dark gray, 10 = light gray, 11 = white), then we can come up with a visual representation of the tile.

Let's take an example of a small tree, and see what it would look like before and after we apply a palette.

![Palettes](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/Mockups/PalettesExplained.png)

Put together, [we can create tilesheets](https://github.com/HarrisonAllen/pebble-gbc-graphics#creating-tilesheets) to store multiple tiles in memory, and load them when necessary.

## Understanding VRAM
VRAM is where the tiles are stored in RAM. Since we don't have to fetch the tiles from storage, this allows quick access for a variety of operations.

Accessing a tile in a VRAM bank is as simple as setting the index to where the tile is sitting in VRAM. Since you will be creating or generating the tilesheets, then loading them into the VRAM buffer, you should know what these indexes are.

For the background and window layers, tiles are placed on a 32 by 32 tile tilemap. For the sprite layer, each sprite contains an index into the VRAM for its corresponding tile(s).

Each bank in the VRAM contains 256 tile slots, and each tile takes up 16 bytes. This means that one VRAM bank takes up 4kb of RAM. 

## Understanding Attributes
While the VRAM contains the tiles, it doesn't store info regarding their color palette, orientation, or anything like that. Furthermore, how does the tile know which VRAM bank it belongs to? That's where tile attributes come into play.

The attributes a tile has are as follows:
* **Palette Number** (0-7): The palette to apply to this tile.
* **VRAM Bank** (0-3): The VRAM bank to load the tile from. (While this can go up to 3, this is limited by the number of VRAM banks allocated by the user.)
* **X Flip**: Will flip the tile horizontally, across the Y-axis.
* **Y Flip**: Will flip the tile vertically, across the X-axis.
* **Background Priority**: Whether or not the background or window tile has rendering priority over the sprite layer.
    * If the priority bit is set on a background or window tile, then any pixel in that tile that is not 00 will render above the sprite layer.
    * If the priority bit is set on a sprite tile, then any pixel on the background or window layrs below the sprite tile that is not 00 will render above the sprite tile.
    * ![Priority](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/Mockups/BackgroundPriority.png)
    
For the background and window layers, tiles are placed on a 32 by 32 tile attribute map (or attrmap). The attrmap corresponds directly to the tilemap. So a tile at position (2,4) on the tilemap has its corresponding attribute at (2,4) on the attrmap. For the sprite layer, each sprite contains its own attributes.

## Understanding Layers
The Game Boy Color (and this library) use 3 layers for rendering. The background layer, the window layer, and the sprite layer.

![Layers](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/Mockups/Layers.png)

### The Background Layer

![Background Layer](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/Mockups/BackgroundExplained.png)

The background layer consists of a 32 tile by 32 tile tilemap. The background layer itself doesn't move, but rather a viewport above the tilemap is moved via a scroll x and a scroll y. By default, this viewport is the size of the watch you are using. It can be adjusted to any size and position you want to use.

You may notice that the scorebar isn't included in the viewport. We'll get into how the viewport can be moved mid-frame later.

### The Window Layer

![Window Layer](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/Mockups/WindowExplained.png)

The window layer also consists of a 32 tile by 32 tile tilemap. The entire window layer moves relative to the viewport, via window offset x and window offset y. The window layer is rendered on top of the background layer, and has no transparency.

### The Sprite Layer

![Sprite Layer](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/Mockups/SpritesExplained.png)

The sprite layer is really a rendering space for the sprites. Sprites data is stored in the OAM.

The sprite layer has transparency! This means that any pixels that use the 1st palette color (00) will be rendered as transparency, no matter what the palette color actually is.

The sprite layer actually starts off-screen, with the x position being 8 pixels left of the origin, and the y position being 16 pixels above the origin.

## Understanding OAM
The OAM stores data for the sprites. The OAM can store up to 40 sprites.

Each sprite in the OAM stores the following data:
* The sprite's x location on-screen
* The sprite's y location on-screen
* The sprite's tile offset in the VRAM
* The sprite's attributes

A sprite can either consist of one tile (8px by 8px), or have two tiles (8px by 16px) such as in Tiny Pilot. If in the 8x16 mode, the sprite will take the tile at the tile offset for the top, and the tile at the next offset (+1) for the bottom.

When two sprites overlap, the sprite with the lower index in OAM will be rendered on top. For example, sprite 0 will render on top of sprite 7.

# The Tutorial
Now, let's get more technical. I'm going to go through various things that you may want to do, and provide examples shown in the [Starter Project](https://github.com/HarrisonAllen/pebble-gbc-graphics/tree/main/starter-project) and [Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/tree/main/tiny-pilot).

## Quick Start
The [Starter Project](https://github.com/HarrisonAllen/pebble-gbc-graphics/tree/main/starter-project) has a basic setup for you to get started with. It demonstrates loading a tilesheet, setting palettes, and placing tiles on the background layer.

## Using a GBC_Graphics Object
The first thing you'll want to do is create a [`GBC_Graphics`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/1928c854e7def04bd4cbb5b083a2cd0a5c65b1f0/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L121-L237) object using [`GBC_Graphics_ctor`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/1928c854e7def04bd4cbb5b083a2cd0a5c65b1f0/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L239-L251).
* The first argument is the `Window` that the app/watchface is using, and the second is the number of VRAM banks you want to use. For Aplite, I recommend only using one VRAM bank due to the limited RAM available.
* Create the `GBC_Graphics` object *after* creating the app/watchface window, such as in a window_load function

Once you're done with the `GBC_Graphics` object, call [`GBC_Graphics_destroy`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/1928c854e7def04bd4cbb5b083a2cd0a5c65b1f0/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L253-L258)
* *Always* call the destroy function when the app is closed, such as in a window_unload function. Otherwise you've got kilobytes of leftover allocated RAM.

As in `GBC_Graphics_destroy`, calling the functions in the library will require passing the generated `GBC_Graphics` object as the the first argument, unless otherwise noted.

Examples;
* `GBC_Graphics_ctor` ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/850c4169b41fae2bc9b7103d5b209cd6610204e3/tiny-pilot/src/c/game.c#L241) | [Starter Project](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/f64dad0d4075f45cf23316f8f031161f40cdb9a2/starter-project/src/c/main.c#L78))
* `GBC_Graphics_destroy` ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/850c4169b41fae2bc9b7103d5b209cd6610204e3/tiny-pilot/src/c/game.c#L462) | [Starter Project](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/f64dad0d4075f45cf23316f8f031161f40cdb9a2/starter-project/src/c/main.c#L88))

## Adjusting the Viewport
The viewport (or screen) is the space in which the GBC Graphics will be displayed on the Pebble's screen. By default, the viewport is set to fit the dimensions of the entire Pebble. You can use [`GBC_Graphics_set_screen_bounds`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L260-L267) and pass in a `GRect` with the boundaries you want the screen to have.
For your convenience, I have [defined some useful screen boundaries](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L96-L113). One example is `GBC_SCREEN_BOUNDS_SQUARE`, which creates a 144px by 144px (or 18 tile by 18 tile) square in the center of the Pebble's screen. This allows for a consistent viewing experience across all Pebbles.

Related functions:
* [`GBC_Graphics_set_screen_origin_x](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L269-L275)
* [`GBC_Graphics_set_screen_origin_y](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L277-L283)
* [`GBC_Graphics_set_screen_width](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L285-L292)
* [`GBC_Graphics_set_screen_height](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L294-L301)
* [`GBC_Graphics_get_screen_bounds](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L303-L310)
* [`GBC_Graphics_set_screen_x_origin](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L312-L319)
* [`GBC_Graphics_set_screen_y_origin](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L321-L328)
* [`GBC_Graphics_set_screen_width](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L330-L337)
* [`GBC_Graphics_set_screen_height](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L339-L346)
(talk about setting the screen bounds, the predefined boundaries, etc)
(link is to the block of code)

## Manipulating the VRAM
(loading tilesheets, moving tiles, direct access)

## Palettes

## LCDC (enables, sprite mode)

## STAT (callbacks)

## Background Layer

## Window Layer

## OAM

## Misc

## Creating Tilesheets
The process for creating tilesheets from an image has a few specific steps, I personally use [GIMP](https://www.gimp.org/):
1. Create the spritesheet using the [2bpp palette](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/2bpp.gpl)
    * For the easiest setup, just modify [SampleTilesheet.xcf](https://github.com/HarrisonAllen/pebble-gbc-graphics/raw/main/assets/tilesheets/SampleTilesheet.xcf). The image should already be in an indexed color mode, and the palette should already exist as `Colormap of image #xx` in the Palettes window (`Windows`->`Dockable Dialogs`->`Palettes`).
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
5. To load in the tilesheet, [see below](https://github.com/HarrisonAllen/pebble-gbc-graphics#loading-tilesheets)

As long as you create a binary file in a 2bpp format, where each tile is 16 bytes (2 bits per pixel * 8 pixels wide * 8 pixels tall), then you can come up with your own methods to do so.

You can create different tilesheets for different platforms. Check out [this link on the Pebble developer docs](https://developer.rebble.io/developer.pebble.com/guides/app-resources/platform-specific/index.html) for more information.

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
    * Tiny Pilot uses the 8x16 sprite mode, so each sprite uses 2 tiles
    * ![Sprites Tilesheet](https://github.com/HarrisonAllen/pebble-gbc-graphics/raw/main/assets/tilesheets/SpritesTilesheet.png)
* **Text Tilesheet**
    * Contains the tiles for text
    * ![Text Tilesheet](https://github.com/HarrisonAllen/pebble-gbc-graphics/raw/main/assets/tilesheets/TextTilesheet.png)

## Using The Library
Here, I'm going to go through various functions, how they are implemented in Tiny Pilot and the Starter Project, and various tips or notes. All functions (except for `GBC_Graphics_ctor`) require you to pass in the `GBC_Graphics` object, so be sure to keep track of it!

### Creating a GBC_Graphics Object
* Function: [`GBC_Graphics_ctor`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/850c4169b41fae2bc9b7103d5b209cd6610204e3/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L251)
* To use, pass in the window you want to render the graphics into (generated from `window_create`, part of the standard app/watchface setup), and the number of VRAM banks you wish to use (each VRAM bank holds 256 tiles and takes up 4kB).
* [Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/850c4169b41fae2bc9b7103d5b209cd6610204e3/tiny-pilot/src/c/game.c#L241) creates and keeps a reference to the GBC_Graphics object in a separate file
* [Starter Project](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/f64dad0d4075f45cf23316f8f031161f40cdb9a2/starter-project/src/c/main.c#L78) creates and keeps a reference to the GBC_Graphics object in a window_load function
* Create the `GBC_Graphics` object *after* creating the app/watchface window, such as in a window_load function

### Destroying a GBC_Graphics Object
* Function: [`GBC_Graphics_destroy`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/850c4169b41fae2bc9b7103d5b209cd6610204e3/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L258)
* *Always* call the destroy function when the app is closed, such as in a window_unload function. Otherwise you've got kilobytes of leftover allocated RAM.
* [Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/850c4169b41fae2bc9b7103d5b209cd6610204e3/tiny-pilot/src/c/game.c#L462) destroys the object in a different file
* [Starter Project](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/f64dad0d4075f45cf23316f8f031161f40cdb9a2/starter-project/src/c/main.c#L88) destroys the object in a window_unload function

### Changing the Boundaries of the Screen/Viewport
* Function: [`GBC_Graphics_set_screen_bounds`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/850c4169b41fae2bc9b7103d5b209cd6610204e3/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L267)
* I have defined some useful screen boundaries for your convenience [here](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/850c4169b41fae2bc9b7103d5b209cd6610204e3/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L96), but you can define your own boundaries.
* [Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/850c4169b41fae2bc9b7103d5b209cd6610204e3/tiny-pilot/src/c/game.c#L243) uses different boundaries for Round vs Rectangular Pebbles in order to keep the score bar visible. It also makes the [window background color](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/f64dad0d4075f45cf23316f8f031161f40cdb9a2/tiny-pilot/src/c/main.c#L7) the same as the sky color to give the illusion of fullscreen.

### Loading Tilesheets
* Function: [`GBC_Graphics_load_from_tilesheet_into_vram`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/850c4169b41fae2bc9b7103d5b209cd6610204e3/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L372)
* The examples below provide a way to load entire tilesheets. However, you can load portions of tilesheets (e.g. 4 tiles for a sprite from a larger sprite tilesheet) by adjusting the `tilesheet_tile_offset` and `tiles_to_load`.
* [Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/f64dad0d4075f45cf23316f8f031161f40cdb9a2/tiny-pilot/src/c/util.c#L126) demonstrates loading multiple tileshets at once, and keeps track of their offsets in order to access specific tilesheets later.
* [Starter Project](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/f64dad0d4075f45cf23316f8f031161f40cdb9a2/starter-project/src/c/main.c#L14) loads the Sample Tilesheet that we created earlier.

### Editing Tilemaps and Attrmaps
* Tilemaps define a 32x32 layer of tiles, 

### Setting Background and Window Tiles
* Function [`GBC_Graphics_bg_set_tile`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/850c4169b41fae2bc9b7103d5b209cd6610204e3/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L801)
* You can also set the Window layer tiles with [`GBC_Graphics_window_set_tile`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/850c4169b41fae2bc9b7103d5b209cd6610204e3/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L969)
* [Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/f64dad0d4075f45cf23316f8f031161f40cdb9a2/tiny-pilot/src/c/graphics/text.c#L70) uses this sometimes, but mostly uses `GBC_Graphics_bg_set_tile_and_attrs`
* [Starter Project](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/f64dad0d4075f45cf23316f8f031161f40cdb9a2/starter-project/src/c/main.c#L60) demonstrates a basic setting of tiles based on x position.

### Creating Attributes
* Function [`GBC_Graphics_attr_make`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/850c4169b41fae2bc9b7103d5b209cd6610204e3/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L680)
* This is a convenience function to create an attribute byte. 
* Attribute bytes define the following characteristics of a tile:
    * The palette the tile should use
    * The VRAM bank the tile is in
    * If the tile should be flipped horizontally or vertically
    * Whether or not the background/window tile has rendering priority over the sprite layer
* Attribute bytes for each layer are defined in these locations: [background attributes](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/850c4169b41fae2bc9b7103d5b209cd6610204e3/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L175), [window attributes](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/850c4169b41fae2bc9b7103d5b209cd6610204e3/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L187), and [sprite attributes](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/850c4169b41fae2bc9b7103d5b209cd6610204e3/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L154).
* [Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/f64dad0d4075f45cf23316f8f031161f40cdb9a2/tiny-pilot/src/c/actors/player.c#L106) does this quite often, and this example shows the creation of the attributes for the plane sprites.

### Setting Background and Window Attributes
* Function [`GBC_Graphics_bg_set_attrs`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/850c4169b41fae2bc9b7103d5b209cd6610204e3/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L811)
* You can also set the Window layer attributes with [`GBC_Graphics_window_set_attrs`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/850c4169b41fae2bc9b7103d5b209cd6610204e3/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L979)
* In general, I [create an attribute](https://github.com/HarrisonAllen/pebble-gbc-graphics#creating-tilesheets) using the convenience function and pass the result into `set_attrs`.
* You can also set each attribute individually. For example, the [Starter Project](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/f64dad0d4075f45cf23316f8f031161f40cdb9a2/starter-project/src/c/main.c#L61) directly sets the tile palettes based on y position.
* You can set both tiles and attributes simultaneously, using [`GBC_Graphics_bg_set_tile_and_attrs`]()