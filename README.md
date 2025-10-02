# DNA Visualiser

A real-time audio visualization plugin that creates a stunning DNA helix effect driven by audio input. Built with JUCE framework.

![Version](https://img.shields.io/badge/version-0.0.1-blue)
![Platform](https://img.shields.io/badge/platform-macOS%20%7C%20Windows%20%7C%20Linux-lightgrey)
![License](https://img.shields.io/badge/license-MIT-green)

## Features

- **Real-time Audio Visualization**: Dynamic DNA helix that responds to audio input
- **Multiple Plugin Formats**: VST3, AU, and Standalone
- **Smooth Animations**: Adaptive smoothing with separate attack and release curves

## Screenshots

![DNA Visualiser in Action](docs/screenshot.png)

*The DNA helix visualization responding to audio input with adjustable parameters for Gain, Rotation, Zoom, and Thickness*


## Installation

### Pre-built Binaries

1. Download the latest release from the [Releases](https://github.com/mapleleafjack/DnaVST/releases) page
2. Extract the archive
3. **macOS Only**: Remove the quarantine attribute to allow the plugin to load:
   ```bash
   xattr -cr "DNA Visualiser.vst3"
   xattr -cr "DNA Visualiser.component"
   ```
4. Copy the plugin files to your plugin directory:
   - **macOS VST3**: `~/Library/Audio/Plug-Ins/VST3/`
   - **macOS AU**: `~/Library/Audio/Plug-Ins/Components/`
5. Restart your DAW

### Building from Source

#### Build Instructions

1. **Clone the repository**
   ```bash
   git clone https://github.com/mapleleafjack/DnaVST.git
   cd DnaVST
   ```

2. **Build the plugin**

   **macOS/Linux:**
   ```bash
   cmake -B build -DCMAKE_BUILD_TYPE=Release
   cmake --build build --config Release -j8
   ```

   **Windows:**
   ```bash
   cmake -B build
   cmake --build build --config Release
   ```
## Usage

### As a Plugin

1. Load "DNA Visualiser" in your DAW as an insert effect on any audio track
2. The plugin is an audio pass-through effect, so it won't alter your audio

### Building for Different Platforms

The CMake configuration supports cross-platform builds. Simply run the build commands on your target platform.

## Technology Stack

- **Framework**: [JUCE](https://juce.com/) 7.x
- **Build System**: CMake
- **Package Manager**: CPM (CMake Package Manager)
- **Language**: C++17

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Support

For issues, questions, or suggestions:
- Open an issue on [GitHub](https://github.com/mapleleafjack/DnaVST/issues)
- Contact: mapleleafjack@gmail.com

## Roadmap

- [ ] Add color customization options
- [ ] Implement preset system
- [ ] Add more visualization modes
- [ ] Performance optimizations
- [ ] MIDI control support
- [ ] Additional audio analysis modes

---

Made with ❤️ using JUCE
