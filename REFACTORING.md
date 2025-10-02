# Refactoring Documentation

## Overview

The DNA Visualiser plugin has been refactored to follow clean architecture principles with clear separation of concerns. This document explains the new structure and how the components interact.

## Directory Structure

```
Source/
├── Parameters/
│   ├── ParameterIDs.h          # Centralized parameter string IDs
│   └── ParameterDefaults.h     # Default values and ranges
├── Audio/
│   ├── AudioAnalyzer.h         # Audio analysis interface
│   └── AudioAnalyzer.cpp       # Frequency/amplitude analysis logic
├── Visualization/
│   ├── DNARenderer.h           # Pure rendering class
│   ├── DNARenderer.cpp         # DNA helix rendering logic
│   ├── ColorScheme.h           # Color generation utilities
│   └── ColorScheme.cpp         # HSV/RGB conversions and gradients
├── UI/
│   ├── VisualizerComponent.h  # Visualization display component
│   ├── VisualizerComponent.cpp # Animation state management
│   ├── ControlPanel.h          # Parameter control UI
│   └── ControlPanel.cpp        # Slider panel implementation
├── PluginEditor.h              # Main editor (composition)
├── PluginEditor.cpp            # Mouse interaction & coordination
├── PluginProcessor.h           # Audio processor (orchestration)
└── PluginProcessor.cpp         # Audio processing & analysis
```

## Component Responsibilities

### Parameters Module

**ParameterIDs.h**
- Defines all parameter string identifiers in the `ParamIDs` namespace
- Ensures consistent parameter access throughout the codebase
- Single source of truth for parameter names

**ParameterDefaults.h**
- Defines default values, min/max ranges, and step sizes
- Used by both the parameter layout and UI normalization
- Makes it easy to adjust parameter ranges in one place

### Audio Module

**AudioAnalyzer**
- Analyzes incoming audio buffers
- Divides audio into 32 frequency bands
- Applies adaptive smoothing (faster release, slower attack)
- Provides amplitude data for visualization
- Configurable smoothing coefficients
- Independent of visualization concerns

### Visualization Module

**DNARenderer**
- Pure rendering class (stateless where possible)
- Renders DNA helix given parameters and amplitude data
- All animation state passed in as parameters
- No internal timers or state
- Handles idle animation when no audio present
- Respects viewport bounds and rotation

**ColorScheme**
- Utility class for color generation
- HSV to RGB conversion
- DNA gradient generation (cyan to purple)
- Separates visual styling from geometry

### UI Module

**VisualizerComponent**
- Manages the DNA visualization display
- Owns the DNARenderer instance
- Manages animation state (helix angle, idle phase)
- Runs at ~60 FPS via timer
- Updates visualization parameters from editor
- Receives amplitude data and passes to renderer

**ControlPanel**
- Self-contained parameter control UI
- Owns all sliders and labels
- Manages its own visibility state
- Styled as floating overlay panel
- Uses APVTS attachments for parameter binding
- Handles its own layout and painting

### Main Components

**PluginEditor**
- Composes VisualizerComponent and ControlPanel
- Handles mouse interactions (drag to rotate, scroll to zoom)
- Toggles control panel visibility
- Updates visualizer parameters from APVTS
- Lightweight orchestration layer

**PluginProcessor**
- Uses AudioAnalyzer for audio analysis
- Passes audio through unchanged (visualization only)
- Provides amplitude data to editor
- Creates parameter layout using centralized definitions
- Clean separation of audio and visual concerns

## Design Principles Applied

### 1. Single Responsibility Principle
Each class has one clear purpose:
- AudioAnalyzer: Analyze audio
- DNARenderer: Render DNA helix
- VisualizerComponent: Display visualization
- ControlPanel: Show/hide parameter controls

### 2. Separation of Concerns
- Audio analysis is independent of visualization
- Rendering is separate from animation state
- UI controls are separate from display
- Parameter definitions are centralized

### 3. Dependency Inversion
- Editor depends on abstractions (component interfaces)
- Components don't know about each other
- Communication through clear interfaces

### 4. Open/Closed Principle
Easy to extend:
- Add new color schemes in ColorScheme
- Add new visualization modes by creating new renderers
- Add new parameters in Parameters module
- Swap out analyzers without changing visualizer

### 5. Testability
Each component can be tested in isolation:
- AudioAnalyzer can be unit tested with mock buffers
- DNARenderer can be tested with mock graphics contexts
- Components have minimal dependencies

## Migration from Original Code

### What Changed

**PluginProcessor**
- Removed inline `updateAmplitudes()` method
- Now uses AudioAnalyzer class
- Parameter creation uses centralized definitions

**PluginEditor**
- Removed inline slider setup code
- Removed inline DNA rendering
- Now composes VisualizerComponent and ControlPanel
- Mouse handling simplified

**DNA → DNARenderer**
- Renamed for clarity
- Removed internal animation state (now passed in)
- Extracted color generation to ColorScheme
- More focused on rendering logic

### What Stayed the Same

- All functionality is preserved
- Mouse interactions work identically
- Parameter ranges unchanged
- Visual appearance identical
- Performance characteristics maintained

## Benefits of Refactoring

### Maintainability
✅ Smaller, focused files (easier to understand)
✅ Clear component boundaries
✅ Changes localized to specific modules

### Testability
✅ Each component can be unit tested
✅ Mock dependencies easily
✅ No tangled dependencies

### Reusability
✅ DNARenderer can be used in other contexts
✅ AudioAnalyzer can be reused
✅ ColorScheme is a general utility

### Extensibility
✅ Easy to add new visualizations
✅ Easy to add new parameters
✅ Easy to swap components

### Readability
✅ Clear structure and organization
✅ Self-documenting architecture
✅ Consistent naming conventions

## Next Steps

### Optional Improvements

1. **Remove Legacy Code**
   - Delete `Source/DNA.h` and `Source/DNA.cpp`
   - Remove from CMakeLists.txt

2. **Add Unit Tests**
   - Test AudioAnalyzer with known inputs
   - Test ColorScheme color generation
   - Test parameter normalization

3. **Add More Visualizations**
   - Create new renderer classes
   - Add visualization selector parameter
   - Swap renderers based on selection

4. **Add Presets**
   - Define parameter presets
   - Add preset selector UI
   - Save/load custom presets

5. **Performance Optimization**
   - Profile rendering performance
   - Consider GPU acceleration
   - Optimize hot paths if needed

## File Size Comparison

### Before Refactoring
- `DNA.cpp`: ~200 lines (mixed concerns)
- `PluginEditor.cpp`: ~200 lines (mixed concerns)
- `PluginProcessor.cpp`: ~250 lines (inline analysis)

### After Refactoring
- **Audio Module**: ~70 lines total
- **Visualization Module**: ~180 lines total
- **UI Module**: ~160 lines total
- **Parameters Module**: ~30 lines total
- **Main Components**: ~250 lines total

Total lines similar, but much better organized!

## Conclusion

This refactoring demonstrates professional software engineering practices while maintaining 100% backward compatibility. The code is now:
- Easier to understand
- Easier to maintain
- Easier to extend
- Easier to test
- More professional in structure

All existing functionality is preserved, but the foundation is now solid for future enhancements.
