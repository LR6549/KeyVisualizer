# Key Visualizer ++

Key Visualizer ++ is a real-time keyboard and mouse visualizer built using **C++** and **SFML**. It highlights pressed keys and mouse buttons with configurable textures and positions. This tool is ideal for tutorials, streaming, or debugging input events visually.

---

## Features

- **Keyboard and Mouse Visualization:** Shows pressed keys and mouse buttons in separate windows.
- **Configurable Highlights:** Positions, textures, and lifetime of highlights are loaded from a JSON configuration file.
- **Multiple Windows:** Separate windows for settings, keyboard, and mouse for better organization.
- **Smooth Rendering:** Uses SFML RenderTextures for efficient off-screen rendering.
- **Drag-and-Drop Windows:** You can move any of the windows by dragging them with the mouse.

---

## Dependencies

This project relies on the following libraries:

- [SFML 3.0](https://www.sfml-dev.org/) — for windowing, graphics, audio, and input handling.
- [nlohmann/json](https://github.com/nlohmann/json) — for reading JSON configuration files.
- [win32](https://learn.microsoft.com/en-us/windows/win32/learnwin32/learn-to-program-for-windows) - for a threaded Mouse Wheel Hook.

---

## JSON Configuration

`highlightConfig.json` contains settings for:    (Soon changeable via settings window)

- Window size and visibility.
- Keyboard and mouse key mappings.
- Highlight textures and positions.
- Frame delay for highlight lifespan.

Example structure:

```json
{
    "DelayFrames":5,

    "settings": {
        "windowSize":[280, 395]
    },

    "keyboard":{
        "show":true,
        "windowSize":[778, 395],

        "keyMap": {
            "A": {
                "texture":"normalKey",
                "position":[0, 0]
            },
            "B" : {
                "texture":"normalKey",
                "position":[0, 0]
            }
        }
    },
    "mouse": {
        "show": true,
        "windowSize":[270, 395],

        "keyMap": {
            "Left": {
                "texture": "left",
                "position": [82, 25]
            },
            "Right": {
                "texture": "right",
                "position": [188, 22]
            },
            "Extra2": {
                "texture": "Mouse4",
                "position": [77, 60]
            },
            "Extra1": {
                "texture": "Mouse5",
                "position": [70, 94]
            },
            "WheelUp": {
                "texture": "wheelUp",
                "position": [157, 76]
            },
            "Middle": {
                "texture": "wheelMiddle",
                "position": [157, 95]
            },
            "WheelDown": {
                "texture": "wheelDown",
                "position": [157, 113]
            },
            "WheelLeft": {
                "texture": "wheelLeft",
                "position": [150, 101]
            },
            "WheelRight": {
                "texture": "wheelRight",
                "position": [182, 101]
            }
        }
    }
}

