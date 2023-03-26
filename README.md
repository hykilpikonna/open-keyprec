<h1 align="center">
  <br>
  <br>
  Open-KeyPrec
  <h4 align="center">
  Open-Sourced Electric Keyboard Percussion Instrument
  </h4>
  <h5 align="center">
<a href="#code">Code</a>&nbsp;&nbsp;
<a href="#build">Build</a>&nbsp;&nbsp;
<a href="#usage">Usage</a>&nbsp;&nbsp;
<a href="#license">License</a>
</h5>
  <br>
  <br>
  <br>
</h1>

## About

Open-KeyPrec is an open-sourced, pressure-sensitive electric keyboard percussion (mallet) MIDI instrument. It has 61 keys, spanning 5 octaves of playing range from C2 to C7 like a standard Marimba. It also has a MIDI control panel with 16 buttons, 12 knobs (8 × 280° potentiometers and 4 × 360° rotary encoders).

The sensors are controlled by a development board, and the output is converted into a standard MIDI device. You can use audio workstation softwares like GarageBand or Reaper (or open-source alternatives like LMMS, Zrythm, or Ardour) to simulate the sounds of all kinds of different instruments.

## Code

**Source code structure**

* `/src` - PC driver written in Rust
* `/firmware` - Development board firmware written in C++
* `/circuits` - Printed circuit boards
* `/models` - 3D-printable models

**Toolchains & Environments Used**

* EasyEDA: Web-editor for creating circuit boards
* FreeCAD: Software for designing mechanical 3D models
* CLion: IDE for Rust & C++
* PlatformIO: Firmware development toolkit

**Pin Configuration**

| Pins | I/O | Description                               |
|------|-----|-------------------------------------------|
| 4    | O   | Mux Select for C2-B6                      |
| 5    | IA  | Mux Input for C2-B6                       |
| 1    | IA  | Input for C7                              |
| 8    | I   | (Panel) Input for 4 rotary encoders       |
| 3    | O   | Mux Select for buttons and potentiometers |
| 2    | I   | Mux Input for buttons                     |
| 1    | IA  | Mux Input for potentiometers              |
| 3    | O   | RGB LED Control                           |

Total ADC channels required: 7
Total GPIO required: 23

## Build

### Materials Required

!!!!!!!!!! TODO !!!!!!!!!!

### Tools Required

* FDM 3D Printer (~$300 Ender 3 is fine)
* Soldering kit (iron, solder, flux)
* Wood Saw
* A computer

### 1. Order PCBs

I ordered my PCBs from JLC, but any PCB service would work. They should accept the exported Gerber zip files in `circuits/Exports` folder.

I created two circuit boards for this keyboard, one is the multiplexer ([Mux v2.zip](circuits/Mux%20v2.zip)) which uses the CD74HC4067 1:16 multiplexer ic to convert the 61 separate analog inputs requirement into only 5 analog inputs and 4 digital switch outputs. 
It also haso diodes and resistors for voltage protection. The other pcb ([OKP-Panel-SMT.zip](circuits/OKP-Panel-SMT.zip)) is for the MIDI control panel containing the buttons and knobs.

1. Go to jlcpcb.com (Or jlc.com if you read Chinese)
2. Upload the Gerber file
3. Set parameters: FR-4, 2 Layers, Color, leave default for other options

**PCB Assembly (SMT)**

If you're not comfortable soldering small SMD components, you can order PCB Assembly from JLC. 
You can also select which parts you would like to SMT based on your soldering abilities. 
For example, in the Mux pcb, I ordered SMT for only the 0603 Zener diodes and the CD74HC4067 ic, and hand-soldered everything else.

1. Order "PCB Assembly" (SMT) and click confirm
2. In the SMT menu, upload the BOM and CPL (PickAndPlace) files
3. After uploading, remove the parts you want to manually solder (they're pretty expensive, since someone in the factory still have to manually solder them for you)
4. Order manually-soldered parts somewhere else (e.g. szlcsc.com)

## Usage

### Flash Firmware

1. Install PlatformIO Core ([Guide](https://platformio.org/install/cli))
2. Open the `firmware` directory
3. In your preferred IDE, edit `src/config.h` with your pin configuration.
4. Edit `platformio.ini` to match your dev board.
5. Connect dev board, build and upload firmware using `platformio run --target upload`

### Setup MIDI Device

!!!!!!!!!! TODO !!!!!!!!!!

### Setup Audio Softwares

A MIDI keyboard doesn't make sound on its own. To actually play an instrument, you would need an audio workstation software and some VST sound sources.

Notation: 🍎 - macOS, 🪟 - Windows, 🐧 - Linux

#### Free Mallet Sound Sources

**Built-in Sources**

* GarageBand comes with marimba, vibraphone, and glockenspiel sound sources. It's free but macOS only and does not have VST support.
* LMMS comes with several mallet instruments.

**External Plugins**

| Name & Link | Author | Platforms | Formats |
| --- | --- | --- | --- |
| [Alto Glockenspiel](https://www.steinberg.net/vst-instruments/alto-glockenspiel/) | Steinberg | 🍎 🪟 | VST, AU |
| [Marimbaphonic](http://www.alanvista.com/marimbaphonic/) | Alan Vista | 🍎 🪟 | VST, AU |
| [VibroMaster](http://www.alanvista.com/vibromaster/) | Alan Vista | 🍎 🪟 | VST, AU |
| [DSK Virtuoso](https://www.dskmusic.com/dsk-virtuoso/) (Celesta, Glockenspiel, ...) | DSK Music | 🪟 | VST |

Unfortunately there aren't any free VSTs supporting linux that I can find. You have to use Windows for this one if you don't have a mac/hackintosh. If you found any additional VSTs, feel free to make a PR!

Other free VST Plugins: https://github.com/webprofusion/OpenAudio