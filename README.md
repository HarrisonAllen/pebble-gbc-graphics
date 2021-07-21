# pebble-gbc-graphics
This is the repository and tutorial for the GBC Graphics engine for the Pebble smartwatch.

(insert table of contents here)

# Introduction
This library allows you to create graphics for your watchface, app, or game (almost) exactly like the Game Boy Color renders them.

Some other projects using this library:
* [Pebblemon](https://github.com/HarrisonAllen/pebble-gbc-graphics-demo/tree/master/pebblemon) - App - pebble-gbc-graphics v1.0.0
* [Pebblemon Time](https://github.com/HarrisonAllen/pebblemon-watchface) - Watchface - pebble-gbc-graphics v1.1.0

The complexities of rendering are abstracted away for the user. For example, the viewport over the background can be moved simply by using the function [`GBC_Graphics_bg_move`](https://github.com/HarrisonAllen/pebble-gbc-graphics/blob/14cc40ab5e61bffaad3381c670b154d5f46bd7a5/tiny-pilot/src/c/pebble-gbc-graphics/pebble-gbc-graphics.h#L766).

If you are unfamiliar with how graphics worked on the Game Boy Color, I recommend you check out the following links:
* [How Graphics worked on the Nintendo Game Boy Color | MVG](https://youtu.be/ZEJVOH6Pxkg?t=201) (started from 3:21)
    * This is an excellent rundown of, as the title says, how the graphics worked on the gameboy color
* [Pan Docs](http://bgb.bircd.org/pandocs.htm#videodisplay)
    * For a low level analysis, check this one out. You don't need to really understand this to use a library, just for some fun reading

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