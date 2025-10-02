#pragma once

namespace ParamDefaults
{
    // Height Gain
    constexpr float heightGainDefault = 3.5f;
    constexpr float heightGainMin = 0.1f;
    constexpr float heightGainMax = 5.0f;
    constexpr float heightGainStep = 0.01f;
    
    // Rotation
    constexpr float rotationDefault = 90.0f;
    constexpr float rotationMin = -180.0f;
    constexpr float rotationMax = 180.0f;
    constexpr float rotationStep = 0.1f;
    
    // Zoom
    constexpr float zoomDefault = 1.5f;
    constexpr float zoomMin = 0.5f;
    constexpr float zoomMax = 2.0f;
    constexpr float zoomStep = 0.01f;
    
    // Thickness
    constexpr float thicknessDefault = 1.0f;
    constexpr float thicknessMin = 0.5f;
    constexpr float thicknessMax = 3.0f;
    constexpr float thicknessStep = 0.01f;
}
