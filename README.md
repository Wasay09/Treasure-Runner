# Treasure Runner

A terminal-based dungeon exploration game built with a C backend and Python curses frontend, following the Model-View-Controller (MVC) architecture.

The player navigates rooms, collects treasure, pushes obstacles, and traverses portals, all rendered in a curses-based terminal UI with persistent player profiles.

## Features

- **Curses terminal UI** with room rendering, status bar, message area, and controls legend
- **Player profiles** stored as JSON with stats tracking (games played, max treasure, most rooms completed)
- **MVC architecture** — C owns all game state; Python handles UI and persistence via ctypes bindings
- **Room navigation** with portals, pushable objects, and collectible treasure
- **Victory condition** — win by collecting all reachable treasure across all rooms
- **Undo/Reset** — undo your last move or reset the current room to its initial state

## Architecture

```
python/
├── run_game.py                  # Entry point (--config, --profile args)
└── treasure_runner/
    ├── bindings/bindings.py     # ctypes wrapper for libbackend.so
    ├── models/
    │   ├── game_engine.py       # Controller — game logic coordination
    │   ├── player.py            # Player model
    │   └── exceptions.py        # C status code → Python exception mapping
    └── ui/game_ui.py            # View — curses rendering and input

c/
├── src/                         # C source files (game engine, rooms, player, graph)
├── include/                     # Header files
└── lib/                         # Built shared library (libbackend.so)
```

## Getting Started

### Prerequisites

- Docker Desktop
- VS Code with the [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) extension

### Setup

1. Clone the repository:
   ```bash
   git clone https://github.com/YOUR_USERNAME/treasure-runner.git
   cd treasure-runner
   ```

2. Open in VS Code and reopen in the dev container:
   - Open the folder in VS Code
   - `Cmd+Shift+P` (or `Ctrl+Shift+P`) → **Dev Containers: Open in Container**

3. Build the project:
   ```bash
   make dist
   ```

4. Run the game:
   ```bash
   cd python
   python3 run_game.py --config ../assets/world.ini --profile ../assets/profile.json
   ```

## Controls

| Key | Action |
|-----|--------|
| `W` / `↑` | Move up |
| `A` / `←` | Move left |
| `S` / `↓` | Move down |
| `D` / `→` | Move right |
| `>` | Enter portal |
| `U` | Undo last move |
| `R` | Reset current room |
| `Q` | Quit game |

## Built With

- **C** — game engine, state management, room/player/graph logic
- **Python 3** — curses UI, JSON persistence, ctypes bindings
- **ctypes** — Python-to-C interop via shared library (`libbackend.so`)