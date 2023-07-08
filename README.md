# pebble-gbc-graphics
This is the repository and tutorial for the GBC Graphics library for the Pebble smartwatch.

Current version of pebble-gbc-graphics: **v1.2.0**

Current version of pebble-gbc-graphics-advanced: **v1.5.0**
For Aplite projects, there are RAM limitations:
* Maximum of 1 VRAM bank
* Maximum of 3 background layers
Changelogs:
* v1.0.0
    * Tiles are now 4bpp, i.e. 16 colors per tile/palette
* v1.1.0
    * Background layers now use transparency for the 0th pixel
* v1.2.0
    * Removed window layer
    * Increased number of background layers to 4
    * Disabled priority, sprites always render on top (temporary until sprite update)
* v1.3.0
    * Sprites now render on a layer
    * The sprite layer can now be positioned between layers (instead of priority)
    * LCDC now has individual background enable bits
    * 8x16 bit removed from LCDC
    * Sprites have additional byte defining dimensions: width and height

    | width/height | 0 | 1 | 2 | 3 |
    | --- | --- | --- | --- | --- |
    | 0 | 1x1 tiles<br>8x8 pixels | 2x1 tiles<br>16x8 pixels | 4x1 tiles<br>32x8 pixels | 8x1 tiles<br>64x8 pixels |
    | 1 | 1x tiles<br>8x2 pixels | 2x2 tiles<br>16x16 pixels | 4x2 tiles<br>32x16 pixels | 8x2 tiles<br>64x16 pixels |
    | 2 | 1x tiles<br>8x4 pixels | 2x4 tiles<br>16x32 pixels | 4x4 tiles<br>32x32 pixels | 8x4 tiles<br>64x32 pixels |
    | 3 | 1x tiles<br>8x8 pixels | 2x8 tiles<br>16x64 pixels | 4x8 tiles<br>32x64 pixels | 8x8 tiles<br>64x64 pixels |

    * Sprites draw from the tile bank starting at the sprite tile offset then left to right, top to bottom. So for a 2x4 tile sprite, it looks like this:

    | 0 | 1 |
    | --- | --- |
    | 2 | 3 |
    | 4 | 5 |
    | 6 | 7 |
* v1.4.0
    * Adds alpha blending modes
        * Mode 0: Normal rendering
        * Mode 1: Add - adds two colors together
        * Mode 2: Subtract - subtracts alpha color from lower color
        * Mode 3: Average - averages two colors together
        * Mode 4: AND - performs bitwise AND (&) on two colors
        * Mode 5: OR - performs bitwise OR (|) on two colors
        * Mode 6: XOR - performs bitwise XOR (^) on two colors
    * Adds sprite mosaic effects
    * Optimization notes:
        * Disable alpha blending
        * Enable fewer backgrounds (e.g. disable a menu background when doing gameplay)
* v1.5.0
    * Changes how sprite width and height work:
        * Now they are just number of extra tiles.
            * e.g. a width of 5 would make a sprite 6 tiles wide
        * Maximum of 16x16 tiles (or 128x128 pixels)
    * Increases maximum x and y to allow for large 128x128 pixel sprites to be off-screen

Want to create an app or watchface for the Pebble, but don't know where to start? Come check out the [Rebble Discord](https://rebble.io/discord) and we'll help you get set up.

You can check out my published [Pebble projects here!](https://apps.rebble.io/en_US/developer/5e28ea923dd3109f151c7e08/1)

# Table of Contents

[**Downloads**](https://github.com/HarrisonAllen/pebble-gbc-graphics#downloads)

[**Introduction**](https://github.com/HarrisonAllen/pebble-gbc-graphics#introduction)
* [In This Repo](https://github.com/HarrisonAllen/pebble-gbc-graphics#in-this-repo)
* [Other Projects](https://github.com/HarrisonAllen/pebble-gbc-graphics#other-projects)
* [Background Information](https://github.com/HarrisonAllen/pebble-gbc-graphics#background-information)
* [Key Differences](https://github.com/HarrisonAllen/pebble-gbc-graphics#key-differences)
* [Other Notes](https://github.com/HarrisonAllen/pebble-gbc-graphics#other-notes)

[**Understanding the Engine**](https://github.com/HarrisonAllen/pebble-gbc-graphics#understanding-the-engine)
* [Understanding Tiles](https://github.com/HarrisonAllen/pebble-gbc-graphics#understanding-tiles)
* [Understanding VRAM](https://github.com/HarrisonAllen/pebble-gbc-graphics#understanding-vram)
* [Understanding Attributes](https://github.com/HarrisonAllen/pebble-gbc-graphics#understanding-attributes)
* [Understanding Layers](https://github.com/HarrisonAllen/pebble-gbc-graphics#understanding-layers)
    * [The Background Layer](https://github.com/HarrisonAllen/pebble-gbc-graphics#the-background-layer)
    * [The Window Layer](https://github.com/HarrisonAllen/pebble-gbc-graphics#the-window-layer)
    * [The Sprite Layer](https://github.com/HarrisonAllen/pebble-gbc-graphics#the-sprite-layer)
* [Understanding OAM](https://github.com/HarrisonAllen/pebble-gbc-graphics#understanding-oam)

[**Using the Library**](https://github.com/HarrisonAllen/pebble-gbc-graphics#using-the-library)
* [Quick Start](https://github.com/HarrisonAllen/pebble-gbc-graphics#quick-start)
* [Using a GBC_Graphics Object](https://github.com/HarrisonAllen/pebble-gbc-graphics#using-a-gbc_graphics-object)
* [Adjusting the Viewport](https://github.com/HarrisonAllen/pebble-gbc-graphics#adjusting-the-viewport)
* [Manipulating the VRAM](https://github.com/HarrisonAllen/pebble-gbc-graphics#manipulating-the-vram)
* [Palettes](https://github.com/HarrisonAllen/pebble-gbc-graphics#palettes)
* [LCDC](https://github.com/HarrisonAllen/pebble-gbc-graphics#lcdc)
* [STAT](https://github.com/HarrisonAllen/pebble-gbc-graphics#stat)
* [Attributes](https://github.com/HarrisonAllen/pebble-gbc-graphics#attributes)
* [Background Layer](https://github.com/HarrisonAllen/pebble-gbc-graphics#background-layer)
* [Window Layer](https://github.com/HarrisonAllen/pebble-gbc-graphics#window-layer)
* [OAM](https://github.com/HarrisonAllen/pebble-gbc-graphics#oam)
* [Other](https://github.com/HarrisonAllen/pebble-gbc-graphics#other)

[**Creating Tilesheets**](https://github.com/HarrisonAllen/pebble-gbc-graphics#creating-tilesheets)
* [Tiny Pilot Tilesheets](https://github.com/HarrisonAllen/pebble-gbc-graphics#tiny-pilot-tilesheets)

[**Closing Remarks**](https://github.com/HarrisonAllen/pebble-gbc-graphics#closing-remarks)

# Downloads
You can visit the [releases tab](https://github.com/HarrisonAllen/pebble-gbc-graphics/releases) for the library (.zip), Tiny Pilot (.pbw), and the Starter Project (.pbw). You can also go to the [downloads directory](https://github.com/HarrisonAllen/pebble-gbc-graphics/tree/main/downloads) and download them there.

[*Back to Table of Contents*](https://github.com/HarrisonAllen/pebble-gbc-graphics#table-of-contents)

# Introduction
This library allows you to create graphics for your watchface, app, or game (almost) exactly like the Game Boy Color renders them. It is compatible with all Pebbles.

*NEW* `pebble-gbc-graphics-advanced` provides more features, closer to what the GBA offers

## In This Repo
* [Starter Project](https://github.com/HarrisonAllen/pebble-gbc-graphics/tree/main/starter-project) - App - pebble-gbc-graphics v1.2.0
    * A barebones implementation of the graphics library, you can use this as a template!
    * ![Starter Project](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/StarterProject.png) ![Starter Project - bw](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/StarterProject-bw.png)
* [Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/tree/main/tiny-pilot) - App - pebble-gbc-graphics v1.2.0
    * A well-documented, full game that demonstrates most of the features in the library
    * ![Tiny Pilot](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/TinyPilot.png) ![Tiny Pilot - bw](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/TinyPilot-bw.png)
* [Starter Project Advanced](https://github.com/HarrisonAllen/pebble-gbc-graphics/tree/main/starter-project-advanced) - App - pebble-gbc-graphics-advanced v1.4.2
    * A barebones implementation of the advanced graphics library, you can use this as a template!
    * ![Starter Project Advanced](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/StarterProjectAdvanced-1.4.png) ![Starter Project Advanced - bw](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/StarterProjectAdvanced-1.4-bw.png)
    
[*Back to Table of Contents*](https://github.com/HarrisonAllen/pebble-gbc-graphics#table-of-contents)

## Other Projects
* [Pebblemon](https://github.com/HarrisonAllen/pebble-gbc-graphics-demo/tree/master/pebblemon) - App - pebble-gbc-graphics v1.0.0
    * ![Pebblemon](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/Pebblemon.png) ![Pebblemon - bw](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/Pebblemon-bw.png)
    * A mini Pokemon adventure that does some very neat things using the engine
    * The codebase is too large to fit on the Aplite platform, there isn't enough ram available.
* [Pebblemon Time](https://github.com/HarrisonAllen/pebblemon-watchface) - Watchface - pebble-gbc-graphics v1.1.0
    * ![Pebblemon Time](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/PebblemonTime.png) ![Pebblemon Time - bw](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/PebblemonTime-bw.png)
    * A stripped down version of Pebblemon that demonstrates a less-frequent draw cycle, as well as combining standard Pebble graphics with the engine.
    
[*Back to Table of Contents*](https://github.com/HarrisonAllen/pebble-gbc-graphics#table-of-contents)

## Background Information
If you are unfamiliar with how graphics worked on the Game Boy Color, I recommend you check out the following links:
* [How Graphics worked on the Nintendo Game Boy Color | MVG](https://youtu.be/ZEJVOH6Pxkg?t=201) (started from 3:21)
    * This is an excellent rundown of, as the title says, how the graphics worked on the gameboy color
* [Pan Docs](http://bgb.bircd.org/pandocs.htm#videodisplay)
    * For a low level analysis, check this one out. You don't need to really understand this to use the library, but it's useful to know if you want to do some more advanced techniques
If you want to get more information about graphics on the Game Boy Advance:
* [How Graphics worked on the Nintendo Game Boy Advance | MVB](https://www.youtube.com/watch?v=mpNWEbZdXNw)
    * Like the GBC rundown but for GBA
* [Tonc](https://www.coranac.com/tonc/text/toc.htm)
    * This is a bit more geared toward programming for a GBA, but there's written descriptions of how the graphics work as well.

[*Back to Table of Contents*](https://github.com/HarrisonAllen/pebble-gbc-graphics#table-of-contents)

## Key Differences
Some key differences between the Game Boy Color and this graphics engine:
* This engine contains up to 4 VRAM banks with 256 tiles each
    * The VRAM itself works mostly the same, it just has a different structure
* LCDC byte has a different mapping
* STAT byte has a different mapping
* Attribute bytes have a different mapping
* Dimensions of the viewport (screen) can be set and changed at runtime
* The 10-sprites-per-line limitation that was present on the GBC has been resolved in this library

[*Back to Table of Contents*](https://github.com/HarrisonAllen/pebble-gbc-graphics#table-of-contents)

## Other Notes
* For black and white pebbles (Pebble Classic, Pebble Steel, Pebble 2), there are 3 colors available: Black, White, and a ditherered Gray
* You can create separate tilesheets for color and black and white Pebbles, just use the `tilesheet~color.bin` and `tilesheet~bw.bin` file naming scheme.

[*Back to Table of Contents*](https://github.com/HarrisonAllen/pebble-gbc-graphics#table-of-contents)

# Understanding the Engine
Let's get started! First, I'll go over some basics about the engine.

## Understanding Tiles
The Game Boy Color uses tiles to render its graphics. A tile is an 8 pixel by 8 pixel image that can have 4 colors. These colors are defined in a palette.

The tile itself is stored in a format known as 2 bits per pixel, or 2bpp. A pixel corresponding to 00 will take the first color in a palette, a pixel corresponding to 01 will take the second color in a palette, etc. If we map those pixels to various shades of gray (00 = black, 01 = dark gray, 10 = light gray, 11 = white), then we can come up with a visual representation of the tile.

Let's take an example of a small tree, and see what it would look like before and after we apply a palette.

![Palettes](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/Mockups/PalettesExplained.png)

Put together, [we can create tilesheets](https://github.com/HarrisonAllen/pebble-gbc-graphics#creating-tilesheets) to store multiple tiles in memory, and load them when necessary.

[*Back to Table of Contents*](https://github.com/HarrisonAllen/pebble-gbc-graphics#table-of-contents)

## Understanding VRAM
VRAM is where the tiles are stored in RAM. Since we don't have to fetch the tiles from storage, this allows quick access for a variety of operations.

Accessing a tile in a VRAM bank is as simple as setting the index to where the tile is sitting in VRAM. Since you will be creating or generating the tilesheets, then loading them into the VRAM buffer, you should know what these indexes are.

For the background and window layers, tiles are placed on a 32 by 32 tile tilemap. For the sprite layer, each sprite contains an index into the VRAM for its corresponding tile(s).

Each bank in the VRAM contains 256 tile slots, and each tile takes up 16 bytes. This means that one VRAM bank takes up 4kb of RAM. 

[*Back to Table of Contents*](https://github.com/HarrisonAllen/pebble-gbc-graphics#table-of-contents)

## Understanding Attributes
While the VRAM contains the tiles, it doesn't store info regarding their color palette, orientation, or anything like that. Furthermore, how does the tile know which VRAM bank it belongs to? That's where tile attributes come into play.

The attributes a tile has are as follows:
* **Palette Number** (0-7): The palette to apply to this tile.
* **VRAM Bank** (0-3): The VRAM bank to load the tile from. (While this can go up to 3, this is limited by the number of VRAM banks allocated by the user.)
* **X Flip**: Will flip the tile horizontally, across the Y-axis.
* **Y Flip**: Will flip the tile vertically, across the X-axis. For 8x16 sprites, this will also swap the top and bottom sprites.
* **Background Priority**: Whether or not the background or window tile has rendering priority over the sprite layer.
    * If the priority bit is set on a background or window tile, then any pixel in that tile that is not 00 will render above the sprite layer.
    * If the priority bit is set on a sprite tile, then any pixel on the background or window layers below the sprite tile that is not 00 will render above the sprite tile.
    * If the priority bit is not set on a sprite or the tiles underneath, then the sprite will render on top as usual.
    * ![Priority](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/Mockups/BackgroundPriority.png)
    
For the background and window layers, tiles are placed on a 32 by 32 tile attribute map (or attrmap). The attrmap corresponds directly to the tilemap. So a tile at position (2,4) on the tilemap has its corresponding attribute at (2,4) on the attrmap. For the sprite layer, each sprite contains its own attributes.

[*Back to Table of Contents*](https://github.com/HarrisonAllen/pebble-gbc-graphics#table-of-contents)

## Understanding Layers
The Game Boy Color (and this library) use 3 layers for rendering. The background layer, the window layer, and the sprite layer.

![Layers](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/Mockups/Layers.png)

[*Back to Table of Contents*](https://github.com/HarrisonAllen/pebble-gbc-graphics#table-of-contents)

### The Background Layer

![Background Layer](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/Mockups/BackgroundExplained.png)

The background layer consists of a 32 tile by 32 tile tilemap. The background layer itself doesn't move, but rather a viewport above the tilemap is moved via a scroll x and a scroll y. By default, this viewport is the size of the watch you are using. It can be adjusted to any size and position you want to use.

You may notice that the score bar isn't included in the viewport. Check out the [STAT](https://github.com/HarrisonAllen/pebble-gbc-graphics#stat) section to see how the viewport can be moved mid-frame.

[*Back to Table of Contents*](https://github.com/HarrisonAllen/pebble-gbc-graphics#table-of-contents)

### The Window Layer

![Window Layer](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/Mockups/WindowExplained.png)

The window layer also consists of a 32 tile by 32 tile tilemap. The entire window layer moves relative to the viewport, via window offset x and window offset y. The window layer is rendered on top of the background layer, and has no transparency.

[*Back to Table of Contents*](https://github.com/HarrisonAllen/pebble-gbc-graphics#table-of-contents)

### The Sprite Layer

![Sprite Layer](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/readme_resources/Mockups/SpritesExplained.png)

The sprite layer is really a rendering space for the sprites. Sprites data is stored in the OAM.

The sprite layer has transparency! This means that any pixels that use the 1st palette color (00) will be rendered as transparency, no matter what the palette color actually is.

The sprite layer actually starts off-screen, with the x position being 8 pixels left of the origin, and the y position being 16 pixels above the origin.

[*Back to Table of Contents*](https://github.com/HarrisonAllen/pebble-gbc-graphics#table-of-contents)

## Understanding OAM
The OAM stores data for the sprites. The OAM can store up to 40 sprites.

Each sprite in the OAM stores the following data:
* The sprite's x location on-screen
* The sprite's y location on-screen
* The sprite's tile offset in the VRAM
* The sprite's attributes

A sprite can either consist of one tile (8px by 8px), or have two tiles (8px by 16px) such as in Tiny Pilot. If in the 8x16 mode, the sprite will take the tile at the tile offset for the top, and the tile at the next offset (+1) for the bottom.

When two sprites overlap, the sprite with the lower index in OAM will be rendered on top. For example, sprite 0 will render on top of sprite 7.

[*Back to Table of Contents*](https://github.com/HarrisonAllen/pebble-gbc-graphics#table-of-contents)

# Using the Library
Now, let's get more technical. I'm going to go through various things that you may want to do, and provide examples shown in the [Starter Project](https://github.com/HarrisonAllen/pebble-gbc-graphics/tree/main/starter-project) and [Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/tree/main/tiny-pilot).

## Quick Start
The [Starter Project](https://github.com/HarrisonAllen/pebble-gbc-graphics/tree/main/starter-project) has a basic setup for you to get started with. It demonstrates loading a tilesheet, setting palettes, and placing tiles on the background layer. Be sure to change the `uuid` in `project.json`!

[*Back to Table of Contents*](https://github.com/HarrisonAllen/pebble-gbc-graphics#table-of-contents)

## Using a GBC_Graphics Object
The first thing you'll want to do is create a [`GBC_Graphics`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L121-L237) object using [`GBC_Graphics_ctor`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L239-L251).
* The first argument is the `Window` that the app/watchface is using, and the second is the number of VRAM banks you want to use. For Aplite, I recommend only using one VRAM bank due to the limited RAM available.
* Create the `GBC_Graphics` object *after* creating the app/watchface window, such as in a window_load function

Once you're done with the `GBC_Graphics` object, call [`GBC_Graphics_destroy`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L253-L258)
* *Always* call the destroy function when the app is closed, such as in a window_unload function. Otherwise you've got kilobytes of leftover allocated RAM.

As in `GBC_Graphics_destroy`, calling the functions in the library will require passing the generated `GBC_Graphics` object as the the first argument, unless otherwise noted.

To get the graphics to display, you must call [`GBC_Graphics_render`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L480-L485). This will render the graphics at the next available opportunity (just like [`layer_mark_dirty`](https://developer.rebble.io/developer.pebble.com/docs/c/User_Interface/Layers/index.html#layer_mark_dirty))

The library should be exhaustive in doing everything you could want to do with a `GBC_Graphics` object, but you can always directly access the variables inside the object in case you want to do something way more advanced.

Related functions:
* [`GBC_Graphics_ctor`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L239-L251) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/game.c#L241) | [Starter Project](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/starter-project/src/c/main.c#L78))
* [`GBC_Graphics_destroy`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L253-L258) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/game.c#L466) | [Starter Project](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/starter-project/src/c/main.c#L91))
* [`GBC_Graphics_render`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L480-L485) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/game.c#L203) | [Starter Project](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/starter-project/src/c/main.c#L86))

[*Back to Table of Contents*](https://github.com/HarrisonAllen/pebble-gbc-graphics#table-of-contents)

## Adjusting the Viewport
The viewport (or screen) is the space in which the GBC Graphics will be displayed on the Pebble's screen. By default, the viewport is set to fit the dimensions of the entire Pebble. You can use [`GBC_Graphics_set_screen_bounds`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L260-L267) and pass in a `GRect` with the boundaries you want the screen to have.
For your convenience, I have [defined some useful screen boundaries](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L96-L113). One example is `GBC_SCREEN_BOUNDS_SQUARE`, which creates a 144px by 144px (or 18 tile by 18 tile) square in the center of the Pebble's screen. This allows for a consistent viewing experience across all Pebbles.

Related functions:
* Setters
    * [`GBC_Graphics_set_screen_bounds`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L260-L267) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/game.c#L243-L259))
    * [`GBC_Graphics_set_screen_origin_x`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L269-L275)
    * [`GBC_Graphics_set_screen_origin_y`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L277-L283)
    * [`GBC_Graphics_set_screen_width`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L285-L292)
    * [`GBC_Graphics_set_screen_height`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L294-L301)
* Getters
    * [`GBC_Graphics_get_screen_bounds`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L303-L310)
    * [`GBC_Graphics_get_screen_x_origin`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L312-L319)
    * [`GBC_Graphics_get_screen_y_origin`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L321-L328)
    * [`GBC_Graphics_get_screen_width`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L330-L337) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/graphics/window.c#L32))
    * [`GBC_Graphics_get_screen_height`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L339-L346) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/graphics/window.c#L32) | [Starter Project](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/starter-project/src/c/main.c#L70))

[*Back to Table of Contents*](https://github.com/HarrisonAllen/pebble-gbc-graphics#table-of-contents)

## Manipulating the VRAM
The VRAM is useful because it stores the tiles we want to render in memory. To load the tiles from a tilesheet (see [Creating Tilesheets](https://github.com/HarrisonAllen/pebble-gbc-graphics#creating-tilesheets)), we want to use [`GBC_Graphics_load_from_tilesheet_into_vram`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L362-L373). The Tiny Pilot example demonstrates loading from multiple tilesheets while keeping track of where they're stored in VRAM.

If you want to directly access a VRAM bank (Pebblemon does this when decompressing sprites), then [`GBC_Graphics_get_vram_bank`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L375-L382) is the function you want to call. 

Related functions:
* [`GBC_Graphics_vram_move_tiles`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L348-L360)
* [`GBC_Graphics_load_from_tilesheet_into_vram`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L362-L373) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/util.c#L125-L186) | [Starter Project](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/starter-project/src/c/main.c#L11-L25))
* [`GBC_Graphics_get_vram_bank`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L375-L382)

[*Back to Table of Contents*](https://github.com/HarrisonAllen/pebble-gbc-graphics#table-of-contents)

## Palettes
Palettes are the source of color for your tiles. Each palette has 4 tiles. The background and window layers share 8 palettes, and the sprite layer gets its own 8 palettes.
* For color Pebbles, use the `ARGB8` version of the GColors. For example, instead of `GColorGreen`, use `GColorGreenARGB8`. Alternatively, you can create your own ARGB 8 bit color (each color, including alpha, uses 2 bits). For example, to create [`GColorLavenderIndigo`](https://developer.rebble.io/developer.pebble.com/guides/tools-and-resources/color-picker/index.html#AA55FF), which is `#AA55FF` in HTML, we would use `0b11100111`, where transparency is always `11`, red is `AA->10`, blue is `55->01`, and green is `FF->11`.
* For black and white Pebbles, I have [defined three colors for you to use](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L115-L118): `GBC_COLOR_BLACK`, `GBC_COLOR_GRAY`, and `GBC_COLOR_WHITE`.

Palettes can be set individually or by copying from an array.

Related functions:
* Background/Window Palettes
    * [`GBC_Graphics_set_bg_palette`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L384-L394) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/graphics/road.c#L3-L33) | [Starter Project](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/starter-project/src/c/main.c#L27-L50))
    * [`GBC_Graphics_set_bg_palette_array`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L396-L403) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/graphics/clouds.c#L70))
    * [`GBC_Graphics_set_bg_palette_color`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L405-L413) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/graphics/fuel.c#L8-L29))
    * [`GBC_Graphics_copy_one_bg_palette`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L415-L422)
    * [`GBC_Graphics_copy_all_bg_palettes`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L424-L430)
* Sprite Palettes
    * [`GBC_Graphics_set_sprite_palette`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L432-L442) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/actors/player.c#L93-L97))
    * [`GBC_Graphics_set_sprite_palette_array`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L444-L451)
    * [`GBC_Graphics_set_sprite_palette_color`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L453-L461) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/actors/hiscore.c#L104))
    * [`GBC_Graphics_copy_one_sprite_palette`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L463-L470)
    * [`GBC_Graphics_copy_all_sprite_palettes`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L472-L478)

[*Back to Table of Contents*](https://github.com/HarrisonAllen/pebble-gbc-graphics#table-of-contents)

## LCDC
The [LCDC byte](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L124-L136) contains data regarding rendering. The data it contains is:
* Should rendering be enabled?
* Should the background layer be enabled?
* Should the window layer be enabled?
* Should sprites be enabled?
* Should the sprite mode be 8x8 or 8x16?

Related functions:
* Setters
    * [`GBC_Graphics_lcdc_set`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L487-L493)
    * [`GBC_Graphics_lcdc_set_enabled`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L495-L501)
    * [`GBC_Graphics_lcdc_set_bg_layer_enabled`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L503-L509)
    * [`GBC_Graphics_lcdc_set_window_layer_enabled`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L511-L517)
    * [`GBC_Graphics_lcdc_set_sprite_layer_enabled`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L519-L525)
    * [`GBC_Graphics_lcdc_set_8x16_sprite_mode_enabled`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L527-L533) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/game.c#L261-L266))

[*Back to Table of Contents*](https://github.com/HarrisonAllen/pebble-gbc-graphics#table-of-contents)

## STAT
The [STAT byte](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L214-L225) handles information related to the draw cycle.
4 parameters are read-only flags:
* HBlank flag (set after each line is drawn)
* VBlank flag (set after all lines are rendered)
* Line Compare flag (set when the line being drawn is equal to the line set for the line compare)
* OAM flag (set after all sprites are drawn)

4 parameters are set by the user:
* HBlank Interrupt Enabled
* VBlank Interrupt Enabled
* Line Compare Interrupt Enabled
* OAM Interrupt Enabled

An incredibly useful feature is being able to set callbacks. For example, the line compare interrupt can allow you to do things like change palettes mid-frame, change the background scroll, etc. Tiny Pilot [has a great example](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/graphics/background.c#L41-L117) of using the line interrupt to do a couple things:
* The first is rendering the score bar. If you recall from the [Background Layer section](https://github.com/HarrisonAllen/pebble-gbc-graphics#the-background-layer), the score bar isn't within the viewport. To account for this, a line interrupt is set for line 0. On this line, we set the background scroll to (0, 0), or right on top of the score bar. Another line interrupt is then set for line 8, which is at the end of the score bar. Once this next line interrupt fires, then the score bar has been rendered, so we return to the original scroll position.
* The second is creating a parallax effect, where we create a 2.5D effect by moving foreground layers faster than the background. Line interrupts are set at the top of the trees behind the road, on the top line of the road, and on the top line of the trees below the road. At each one of these line interrupts, I increase the scroll position (i.e. increasing the speed), which makes it seem as though each part is moving faster.
* These are just two examples of what you can do with interrupts!

Note that callback functions for the interrupts should have one argument that is the `GBC_Graphics` object.

Related functions:
* Setters
    * [`GBC_Graphics_stat_set`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L589-L595)
    * [`GBC_Graphics_stat_set_hblank_interrupt_enabled`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L597-L603)
    * [`GBC_Graphics_stat_set_vblank_interrupt_enabled`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L605-L611)
    * [`GBC_Graphics_stat_set_line_compare_interrupt_enabled`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L613-L619) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/graphics/background.c#L128))
    * [`GBC_Graphics_stat_set_oam_interrupt_enabled`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L621-L627)
    * [`GBC_Graphics_stat_set_line_y_compare`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L629-L635) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/graphics/background.c#L127))
    * [`GBC_Graphics_set_hblank_interrupt_callback`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L637-L643)
    * [`GBC_Graphics_set_vblank_interrupt_callback`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L645-L651)
    * [`GBC_Graphics_set_line_compare_interrupt_callback`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L653-L659) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/graphics/background.c#L126))
    * [`GBC_Graphics_set_oam_interrupt_callback`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L661-L667)
* Getters
    * [`GBC_Graphics_stat_get_current_line`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L535-L542)
    * [`GBC_Graphics_stat_get_line_y_compare`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L544-L551) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/graphics/background.c#L68))
* Helper Functions
    * [`GBC_Graphics_stat_check_hblank_flag`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L553-L560)\*
    * [`GBC_Graphics_stat_check_vblank_flag`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L562-L569)\*
    * [`GBC_Graphics_stat_check_line_comp_flag`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L571-L578)\*
    * [`GBC_Graphics_stat_check_oam_flag`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L580-L587)\*

\*The check functions do not require the `GBC_Graphics` object as the first argument. Instead, they take in the STAT byte to check.

[*Back to Table of Contents*](https://github.com/HarrisonAllen/pebble-gbc-graphics#table-of-contents)

## Attributes
The [attribute byte](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L175-L186) describes different characteristics of a tile or sprite. You can read more about it [here](https://github.com/HarrisonAllen/pebble-gbc-graphics#understanding-attributes).

These functions are for working with attribute bytes directly. For setting attributes directly on a specific tile or sprite, see the [background](https://github.com/HarrisonAllen/pebble-gbc-graphics#background-layer), [window](https://github.com/HarrisonAllen/pebble-gbc-graphics#window-layer), and [OAM](https://github.com/HarrisonAllen/pebble-gbc-graphics#oam) sections.

Related functions:
* Helper Functions
    * [`GBC_Graphics_attr_make`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L669-L680)\* ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/graphics/clouds.c#L129))
    * [`GBC_Graphics_attr_get_palette_num`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L682-L689)\*
    * [`GBC_Graphics_attr_get_vram_bank`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L691-L698)\*
    * [`GBC_Graphics_attr_is_x_flipped`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L700-L707)\*
    * [`GBC_Graphics_attr_is_y_flipped`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L709-L716)\*
    * [`GBC_Graphics_attr_background_has_priority`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L718-L725)\*

\*These functions do not require the `GBC_Graphics` object as the first argument.

[*Back to Table of Contents*](https://github.com/HarrisonAllen/pebble-gbc-graphics#table-of-contents)

## Background Layer
Read more about the [background layer here](https://github.com/HarrisonAllen/pebble-gbc-graphics#the-background-layer). The main idea is that you move the viewport over the background. If the viewport moves past the edge of the background, then it will just wrap around.

You can place tiles on the background at x and y positions, as well as attributes. Both can be placed with one function. Attributes can also be set on an individual basis.

Related functions:
* Setters
    * [`GBC_Graphics_bg_move`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L767-L775) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/graphics/background.c#L120-L121), although not a direct example)
    * [`GBC_Graphics_bg_set_scroll_x`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L777-L783) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/graphics/background.c#L88-L89))
    * [`GBC_Graphics_bg_set_scroll_y`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L785-L791)
    * [`GBC_Graphics_bg_set_scroll_pos`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L793-L800) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/graphics/background.c#L75-L76))
    * [`GBC_Graphics_bg_set_tile`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L802-L810) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/graphics/road.c#L75-L78) | [Starter Project](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/starter-project/src/c/main.c#L60))
    * [`GBC_Graphics_bg_set_attrs`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L812-L820)
    * [`GBC_Graphics_bg_set_tile_and_attrs`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L822-L831) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/graphics/road.c#L57))
    * [`GBC_Graphics_bg_move_tile`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L883-L893)
* Attribute Setters
    * [`GBC_Graphics_bg_set_tile_palette`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L833-L841) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/graphics/text.c#L69) | [Starter Project](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/starter-project/src/c/main.c#L61))
    * [`GBC_Graphics_bg_set_tile_vram_bank`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L843-L851)
    * [`GBC_Graphics_bg_set_tile_x_flip`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L853-L861)
    * [`GBC_Graphics_bg_set_tile_y_flip`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L863-L871)
    * [`GBC_Graphics_bg_set_tile_priority`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L873-L881)
* Getters
    * [`GBC_Graphics_bg_get_scroll_x`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L727-L734)
    * [`GBC_Graphics_bg_get_scroll_y`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L736-L743) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/actors/player.c#L108-L109))
    * [`GBC_Graphics_bg_get_tile`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L745-L754)
    * [`GBC_Graphics_bg_get_attr`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L756-L765)

[*Back to Table of Contents*](https://github.com/HarrisonAllen/pebble-gbc-graphics#table-of-contents)

## Window Layer
Read more about the [window layer here](https://github.com/HarrisonAllen/pebble-gbc-graphics#the-window-layer). It will render on top of the background layer. The main idea is that you move the window layer relative to the viewport. It works on an offset, so it can only move down and right. To hide the window layer, just move it off-screen, or disable it.

Just like the background layer, you can place tiles on the background at x and y positions, as well as attributes. You can also place them both with one function. You can also set attributes individually.

Related functions:
* Setters
    * [`GBC_Graphics_window_move`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L935-L943)
    * [`GBC_Graphics_window_set_offset_x`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L945-L951) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/graphics/window.c#L93))
    * [`GBC_Graphics_window_set_offset_y`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L953-L959) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/graphics/window.c#L94) | [Starter Project](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/starter-project/src/c/main.c#L70))
    * [`GBC_Graphics_window_set_offset_pos`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L961-L968) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/graphics/window.c#L31-L32))
    * [`GBC_Graphics_window_set_tile`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L970-L978) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/graphics/text.c#L73))
    * [`GBC_Graphics_window_set_attrs`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L980-L988)
    * [`GBC_Graphics_window_set_tile_and_attrs`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L990-L999) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/graphics/window.c#L58))
    * [`GBC_Graphics_window_move_tile`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1051-L1061)
* Attribute Setters
    * [`GBC_Graphics_window_set_tile_palette`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1001-L1009) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/graphics/text.c#L72))
    * [`GBC_Graphics_window_set_tile_vram_bank`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1011-L1019)
    * [`GBC_Graphics_window_set_tile_x_flip`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1021-L1029) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/graphics/window.c#L61))
    * [`GBC_Graphics_window_set_tile_y_flip`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1031-L1039) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/graphics/window.c#L64))
    * [`GBC_Graphics_window_set_tile_priority`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1041-L1049) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/graphics/window.c#L119))
* Getters
    * [`GBC_Graphics_window_get_offset_x`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L895-L902) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/actors/items.c#L94-L95))
    * [`GBC_Graphics_window_get_offset_y`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L904-L911) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/actors/items.c#L94-L95))
    * [`GBC_Graphics_window_get_tile`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L913-L922)
    * [`GBC_Graphics_window_get_attr`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L924-L933)

[*Back to Table of Contents*](https://github.com/HarrisonAllen/pebble-gbc-graphics#table-of-contents)

## OAM
Read more about [the sprite layer](https://github.com/HarrisonAllen/pebble-gbc-graphics#the-sprite-layer) and [the OAM](https://github.com/HarrisonAllen/pebble-gbc-graphics#understanding-oam).

While one sprite can only be either 8x8 or 8x16 pixels, if you use some strategic programming, you can combine these single sprites into a larger object, like the plane in Tiny Pilot.

Related functions:
* Setters:
    * [`GBC_Graphics_oam_set_sprite`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1103-L1113) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/actors/player.c#L113))
    * [`GBC_Graphics_oam_move_sprite`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1115-L1123) 
    * [`GBC_Graphics_oam_set_sprite_x`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1125-L1132)
    * [`GBC_Graphics_oam_set_sprite_y`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1134-L1141)
    * [`GBC_Graphics_oam_set_sprite_pos`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1143-L1151) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/actors/hiscore.c#L79-L83)
    * [`GBC_Graphics_oam_hide_sprite`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1153-L1159) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/actors/items.c#L89-L91))
    * [`GBC_Graphics_oam_change_sprite_num`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1224-L1232)
    * [`GBC_Graphics_oam_swap_sprites`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1234-L1241)
    * [`GBC_Graphics_oam_swap_sprite_tiles`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1243-L1250)
    * [`GBC_Graphics_oam_swap_sprite_attrs`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1252-L1259)
    * [`GBC_Graphics_oam_swap_sprite_tiles_and_attrs`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1261-L1268)
* Attribute setters:
    * [`GBC_Graphics_oam_set_sprite_tile`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1161-L1168) ([Tiny Pilot](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/actors/items.c#L121))
    * [`GBC_Graphics_oam_set_sprite_attrs`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1170-L1177)
    * [`GBC_Graphics_oam_set_sprite_palette`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1179-L1186)
    * [`GBC_Graphics_oam_set_sprite_vram_bank`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1188-L1195)
    * [`GBC_Graphics_oam_set_sprite_x_flip`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1197-L1204)
    * [`GBC_Graphics_oam_set_sprite_y_flip`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1206-L1213)
    * [`GBC_Graphics_oam_set_sprite_priority`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1215-L1222)
* Getters:
    * [`GBC_Graphics_oam_get_sprite_x`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1063-L1071)
    * [`GBC_Graphics_oam_get_sprite_y`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1073-L1081)
    * [`GBC_Graphics_oam_get_sprite_tile`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1083-L1091)
    * [`GBC_Graphics_oam_get_sprite_attrs`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1093-L1101)

[*Back to Table of Contents*](https://github.com/HarrisonAllen/pebble-gbc-graphics#table-of-contents)

## Other
These apply to both the background and window layers, so I'm putting them here.

* [`GBC_Graphics_copy_background_to_window`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1270-L1276)
* [`GBC_Graphics_copy_window_to_background`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L1278-L1284)

[*Back to Table of Contents*](https://github.com/HarrisonAllen/pebble-gbc-graphics#table-of-contents)


# Creating Tilesheets
The process for creating tilesheets from an image has a few specific steps, I personally use [GIMP](https://www.gimp.org/):
1. Create the spritesheet using the [2bpp palette](https://raw.githubusercontent.com/HarrisonAllen/pebble-gbc-graphics/main/assets/2bpp.gpl)
    * For the easiest setup, just modify [SampleTilesheet.xcf](https://github.com/HarrisonAllen/pebble-gbc-graphics/raw/main/assets/tilesheets/SampleTilesheet.xcf). The image should already be in an indexed color mode, and the palette should already exist as `Colormap of image #xx` in the Palettes window (`Windows`->`Dockable Dialogs`->`Palettes`).
    * To convert a preexisting image to a 2bpp palette, you can change the image color mode by going to `image`->`mode`->`indexed`->`Use custom palette`->Select `2bpp` from the list. [This guide](https://daviesmediadesign.com/how-to-install-custom-palettes-in-gimp/) may or may not do the trick if you want to add the palette to your installation
    * I prefer to create my tilesheets as a ribbon (all tiles in a line), but they can be in any shape. Just keep in mind that the conversion script will scan your tilesheet from left to right, top to bottom, one tile at a time.
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
5. To load in the tilesheet, [see 'Manipulating the VRAM'](https://github.com/HarrisonAllen/pebble-gbc-graphics#manipulating-the-vram)

As long as you create a binary file in a 2bpp format, where each tile is 16 bytes (2 bits per pixel * 8 pixels wide * 8 pixels tall), then you can come up with your own methods to do so.

You can create different tilesheets for different platforms. Check out [this link on the Pebble developer docs](https://developer.rebble.io/developer.pebble.com/guides/app-resources/platform-specific/index.html) for more information.

[*Back to Table of Contents*](https://github.com/HarrisonAllen/pebble-gbc-graphics#table-of-contents)

## Tiny Pilot Tilesheets
These are the tilesheets I designed and generate for Tiny Pilot. All of these can be found [here](https://github.com/HarrisonAllen/pebble-gbc-graphics/tree/main/assets/tilesheets), if you wish to edit or duplicate them.
* **Base Tilesheet** 
    * Contains the basic one-color tiles, as well as the frames for the window layer
    * ![Base Tilesheet](https://github.com/HarrisonAllen/pebble-gbc-graphics/raw/main/assets/tilesheets/BaseTilesheet.png)
* **Cloud Tilesheet**
    * Contains all of the cloud tiles
    * There is a [data structure](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/graphics/clouds.c#L9) that keeps track of the dimensions and position on the tilesheet of each cloud.
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

[*Back to Table of Contents*](https://github.com/HarrisonAllen/pebble-gbc-graphics#table-of-contents)

# Closing Remarks
I hope you're able to make some cool things with this engine! Feel free to modify it, upgrade it, do whatever you'd like with it. I'm sure there are ways to further optimize the [rendering script](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/main/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.c#L243-L517) (removing what branching is left, reducing number of operations, some other random C optimizations that I'm unfamiliar with, etc.).

Pebblemon does a lot of cool things that aren't quite as well documented. These include a script for converting an image of a map into a playable route, changing tiles on the background tilemap as the player moves, storing 502 Pokémon sprites in a compressed format and then decompressing them (using the same compression/decompression algorithm that the games used), screen interrupts for battle transitions. It's worth exploring if you're trying to do something similar.

Feel free to reach out if you have any questions or problems with the library! My email is [harrisonallen555@gmail.com](mailto:harrisonallen555@gmail.com). You can also open an issue.

[*Back to Table of Contents*](https://github.com/HarrisonAllen/pebble-gbc-graphics#table-of-contents)
