# Architecture Diagram

## Component Relationships

```
┌─────────────────────────────────────────────────────────────────┐
│                        PluginProcessor                          │
│                                                                 │
│  ┌──────────────────┐         ┌─────────────────────────────┐ │
│  │ AudioAnalyzer    │         │ ParameterLayout             │ │
│  │                  │         │  (uses ParamIDs &           │ │
│  │ - analyzeBuffer()│         │   ParamDefaults)            │ │
│  │ - getAmplitudes()│         │                             │ │
│  └──────────────────┘         └─────────────────────────────┘ │
│                                                                 │
└─────────────────────────┬───────────────────────────────────────┘
                          │
                          │ provides amplitudes
                          │ & APVTS
                          ↓
┌─────────────────────────────────────────────────────────────────┐
│                        PluginEditor                             │
│                                                                 │
│  ┌──────────────────────────────┐  ┌──────────────────────┐   │
│  │   VisualizerComponent        │  │   ControlPanel       │   │
│  │                              │  │                      │   │
│  │  ┌──────────────────────┐   │  │  - Sliders & Labels  │   │
│  │  │   DNARenderer        │   │  │  - APVTS Attachments │   │
│  │  │                      │   │  │  - Visibility Toggle │   │
│  │  │  uses ColorScheme    │   │  │                      │   │
│  │  │  for gradients       │   │  └──────────────────────┘   │
│  │  │                      │   │                              │
│  │  │  - render()          │   │                              │
│  │  └──────────────────────┘   │                              │
│  │                              │                              │
│  │  - Animation State           │                              │
│  │  - Timer (60 FPS)            │                              │
│  │  - setAmplitudes()           │                              │
│  │  - setParameters()           │                              │
│  └──────────────────────────────┘                              │
│                                                                 │
│  Mouse Handling:                                                │
│  - Drag to rotate                                               │
│  - Scroll to zoom                                               │
│  - Click to toggle controls                                     │
└─────────────────────────────────────────────────────────────────┘
```

## Data Flow

```
Audio Buffer
     │
     ↓
┌──────────────────┐
│ AudioAnalyzer    │
│ (in Processor)   │
└────────┬─────────┘
         │ amplitude data
         ↓
┌──────────────────┐
│ PluginEditor     │
│ (timer callback) │
└────────┬─────────┘
         │ setAmplitudes()
         ↓
┌──────────────────────┐
│ VisualizerComponent  │
│ (owns DNARenderer)   │
└────────┬─────────────┘
         │ render()
         ↓
┌──────────────────┐
│ DNARenderer      │ ←─── ColorScheme
│ (pure rendering) │      (gradients)
└──────────────────┘
```

## Parameter Flow

```
User Interaction
(sliders or mouse)
     │
     ↓
┌─────────────────────┐
│ APVTS               │ ←─── ParamIDs
│ (Audio Processor    │      ParamDefaults
│  Value Tree State)  │
└──────────┬──────────┘
           │
           ├─────────────────────┐
           │                     │
           ↓                     ↓
    ┌──────────────┐    ┌──────────────────┐
    │ ControlPanel │    │ PluginEditor     │
    │ (displays)   │    │ (reads & passes) │
    └──────────────┘    └────────┬─────────┘
                                 │
                                 ↓
                        ┌──────────────────────┐
                        │ VisualizerComponent  │
                        │ (applies to render)  │
                        └──────────────────────┘
```

## Module Dependencies

```
┌─────────────────────┐
│ Parameters Module   │ (no dependencies)
│ - ParameterIDs      │
│ - ParameterDefaults │
└──────────┬──────────┘
           │
           │ used by ↓
           │
┌──────────┴──────────┬──────────────────────┬────────────────┐
│                     │                      │                │
│ PluginProcessor     │ PluginEditor         │ ControlPanel   │
│ - uses Audio        │ - uses UI            │ - uses JUCE    │
│   Analyzer          │   Components         │   Components   │
└─────────────────────┴──────────────────────┴────────────────┘

┌──────────────────┐
│ Audio Module     │ (depends on JUCE audio)
│ - AudioAnalyzer  │
└──────────────────┘

┌──────────────────┐
│ Visualization    │ (depends on JUCE graphics)
│ - DNARenderer    │
│ - ColorScheme    │
└────────┬─────────┘
         │
         │ used by ↓
         │
┌────────┴────────────┐
│ UI Module           │ (depends on Visualization)
│ - VisualizerComp    │
│ - ControlPanel      │
└─────────────────────┘
```

## Responsibility Matrix

| Component            | Audio | Rendering | Animation | UI Controls | Parameters |
|---------------------|-------|-----------|-----------|-------------|------------|
| AudioAnalyzer       |   ✓   |           |           |             |            |
| DNARenderer         |       |     ✓     |           |             |            |
| ColorScheme         |       |     ✓     |           |             |            |
| VisualizerComponent |       |     ✓     |     ✓     |             |            |
| ControlPanel        |       |           |           |      ✓      |            |
| PluginEditor        |       |           |           |      ✓      |            |
| PluginProcessor     |   ✓   |           |           |             |      ✓     |
| ParameterIDs        |       |           |           |             |      ✓     |
| ParameterDefaults   |       |           |           |             |      ✓     |

## Testing Strategy

Each module can be tested independently:

```
┌──────────────────┐     ┌────────────────────┐
│ AudioAnalyzer    │ ←── │ Mock AudioBuffer   │
└──────────────────┘     └────────────────────┘

┌──────────────────┐     ┌────────────────────┐
│ DNARenderer      │ ←── │ Mock Graphics      │
└──────────────────┘     │ Known Parameters   │
                         └────────────────────┘

┌──────────────────┐     ┌────────────────────┐
│ ColorScheme      │ ←── │ Known HSV Values   │
└──────────────────┘     └────────────────────┘

┌──────────────────┐     ┌────────────────────┐
│ VisualizerComp   │ ←── │ Mock Amplitudes    │
└──────────────────┘     └────────────────────┘
```
