
# Blake4SDL ~ by KS-Presto

# Original Blake Stone games by JAM Productions


Blake4SDL is an open-source port of JAM Productions' first-person
shooter Blake Stone: Planet Strike to the cross-platform multimedia
library [Simple DirectMedia Layer (SDL)](http://www.libsdl.org). It is
meant to keep the original feel while taking advantage of some
improvements mentioned below.

## Main features

 - Currently supported operating systems (little endian only):
 	- Windows XP and later (32 & 64 bit)
 	- Linux (compiles and runs but not yet tested)

 - Currently supported Blake Stone versions:
 	- Only Blake Stone: Planet Strike is currently supported.

 - Modern control scheme support: The popular WASD + mouse control
scheme is available by default, with arrow keys + alt for strafing still
available for the old schoolers. And yes, the **HORRIBLE** hard coded
special keys are no longer a thing, there is a new menu where you can
redefine them!

 - Higher screen resolutions: Aside from the original 320x200
resolution, Blake4SDL currently supports any resolutions being multiples
of 320x200 or 320x240. Blake4SDL does NOT apply any bilinear or similar
filtering, so the graphics are NOT blurred but pixelated just as we love
it. A new display menu has been added to allow changes from within the
game.

 - Multichannel digitized sounds: Digitized sounds play on 8 channels
instead of the original 1.

 - Fully playable with only a game controller: Blake4SDL can be played
completely without a keyboard. At least two buttons are required
(shoot/YES and open door/NO), but five or more are recommended (run,
strafe, ESC).

## How to play

To play Blake Stone with Blake4SDL, you just have to copy the original
data files (e.g. \*.VSI) into the same directory as the Blake4SDL
executable. Make sure that you use the correct version of the executable
with the according data files as the differences are hardcoded into the
binary!

On Windows, SDL2.dll and SDL2_mixer.dll must also be copied into this
directory, or include their location in the Path environment variable.

The SDL2 & SDL2_mixer Runtimes can be acquired here:

- [SDL2](https://github.com/libsdl-org/SDL/releases/download/release-2.30.2/SDL2-2.30.2-win32-x86.zip)
- [SDL2_mixer](https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.8.0/SDL2_mixer-2.8.0-win32-x86.zip)

### Note: These are the x86 (32 bit) binaries; only download the x64 binaries if you recompile the source for 64 bit!

When playing in windowed mode, press ScrollLock or F11 to grab/release
mouse input.

## Usage

See Options.txt for a list of available startup parameters.

## Compiling the source code

The source code is available on [GitHub](https://github.com/KS-Presto/Blake4SDL).


To compile the source code you need the SDL2 & SDL2_mixer development
libraries:
- [SDL2](https://github.com/libsdl-org/SDL/releases)
- [SDL2_mixer](https://github.com/libsdl-org/SDL_mixer/releases)

### Note: Be sure to use the files from the i686 (32 bit) folders unless you want to compile for 64 bit.

## Known bugs

 - Demo playback gets unsynced (they're currently disabled for this
reason)

 - Original Planet Strike bugs:
 	- Morphing enemies don't call FirstSighting, which makes them slow until they take damage (where they'll even play their alert sound again)
	- Enemy morph animations sometimes display the wrong graphics
	- Crates don't make an explosion sound
	- Arc barriers cannot be destroyed by BFG
 	- Animated shapes in the Text Presenter overdraw each other when coming back from the 2nd to last page

#### If you find any not listed here, please let me know!

## Credits

- Special thanks to Apogee Entertainment (formerly Apogee Software) for releasing the Blake Stone
source code
- Special thanks to the MAME developer team for providing
the source code of the OPL2 emulator
- Special thanks to Moritz
"Ripper" Kroll  ([Chaos-Software](https://web.archive.org/web/20111005155620/http://www.chaos-software.de.vu/)) for code taken from the Wolf4SDL port (where the
Blake4SDL name was taken from). Wherever you are, you'll always be a
legend! ^^+
- Thanks to members of the [Die-Hard Wolfers community](https://dhw.wolfenstein3d.com) for providing useful guides/tutorials
for some of the Blake Stone file formats

## Licenses

 - The original source code of Blake Stone:
 	- Blake Stone source code license.doc
 	- license-gpl.txt
 - The OPL2 emulator:
 	- license-mame.txt (fmopl.c)

## Contact me

Drop me an email at <kspresto@protonmail.com> if you have any
bug reports/questions/etc.
