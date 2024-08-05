This an archive of the code that created the demo NUMBERS for the Assembly 2024 Real Wild competition.
The demo placed first. There was much celebration.

If you want to build the demo for yourself, use the snapshot of the mtek-gdl framework here: [MTEK-GDL-muffintrap nova branch snapshot](https://github.com/MuffinTrap/mtek-gdl/releases/tag/Snapshot)
The snapshot is the version of the framework that was used to build the demo.

# NUMBERS

## Credits
- Group: Marmot
- Code & Design: Muffinhop
- Code: Muffintrap
- Music & Logos: RaccoonViolet

## Development

### Frameworks and libraries
- [MTEK-GDL-muffintrap fork](https://github.com/MuffinTrap/mtek-gdl)
- [Rocket](https://github.com/rocket/rocket)

### Tools:
- [KDevelop](https://kdevelop.org/)
- [Font conversion tool](https://stmn.itch.io/font2bitmap)


## Windows instructions

### Development environment and libraries
1. Install [MSYS2](https://www.msys2.org/) (This is a different version from the one that comes with DevkitPro) [Detailed instructions for install](https://www.freecodecamp.org/news/how-to-install-c-and-cpp-compiler-on-windows/)
2. Launch the UCRT64 variant and update the packages as explained in the Detailed instructions above.
3. Install the compiler 
<code>pacman -S mingw-w64-ucrt-x86_64-gcc</code>
4. Install the required libraries:
<code>pacman -S mingw-w64-ucrt-x86_64-openal mingw-w64-ucrt-x86_64-freeglut mingw-w64-ucrt-x86_64-libpng mingw-w64-ucrt-x86_64-libsndfile mingw-w64-ucrt-x86_64-glm make</code>

### Compiling the demo and getting the .dll files
1. Navigate to the github repository.
2. Run command <code>make -f Makefile_computer</code>

####  Get or link to DLL files
**Link to files**
- Add the folder <code>C:\msys64\ucrt64\bin</code> to PATH on windows. That way windows will look there for the dll files.

**Get the files**
1. Create a new folder __outside__ the repository and name it something like win_build.
2. Copy the **tools/copy_dll_files.cmd** to win_build
3. Edit the CMD file to have the repository path
4. Run the CMD file to copy all the dlls.
5. Also copy the asset files from the **data/** folder to **win_buid/data/** so that the demo finds them

#### These are the dll files that the demo needs to run.
- libgcc_s_seh-1.dll
- libfreeglut.dll
- libopenal-1.dll
- libpng16-16.dll
- libsndfile-1.dll
- libstdc++-6.dll
- libwinpthread-1.dll
- zlib1.dll
- libFLAC.dll
- libmpg123-0.dll
- libmp3lame-0.dll
- libogg-0.dll
- libvorbis-0.dll
- libopus-0.dll
- libvorbisenc-2.dll


## Libraries needed for Wii
- mtek-gdl  (see Frameworks above)
- ppc-glm (from devkitpro pacman)

## Libraries needed for Mac port:
- glm [OpenGL Maths Library](https://formulae.brew.sh/formula/glm)

## Libraries needed for Linux port:

### Demo
- libglm-dev [OpenGL Maths Library](https://github.com/g-truc/glm)

### Rocket editor
- libqt5websockets5-dev
- qml-module-qtwebsockets

### Rocket integration 
- libsndfile1-dev
- libopenal-dev
- freeglut3-dev

## Links:

### Inspiration for effects:
- [Ryoji Ikeda - Supersymmetry trailer](https://www.youtube.com/watch?v=pni0vKgbeJE)
- [Ryoji Ikeda - Supersymmetry live ](https://www.youtube.com/watch?v=wUJB2PE1UI4)

### Palette references:
- [Obra Dinn](https://lospec.com/palette-list/obra-dinn-ibm-8503)
- [Laser Lab](https://lospec.com/palette-list/laser-lab)
- [1bit Monitor Glow](https://lospec.com/palette-list/1bit-monitor-glow)

### Font candidates:
- [Andvari](https://www.dafont.com/isl-andvari.font) -> sd_card/andvari30x30.png
- [PCbius](https://www.1001freefonts.com/pcbius.font)
- [Touchscreen](https://www.1001freefonts.com/touchscreen.font)



