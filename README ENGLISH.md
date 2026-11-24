# Sopa de Letras Pro

[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/std/the-standard)
[![CMake](https://img.shields.io/badge/CMake-3.15+-green.svg)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Windows%20|%20Linux-lightgrey.svg)](https://github.com)

> **Professional Word Search Game developed in C++17** for Software Engineering Portfolio.
> Modular architecture, difficulty levels, persistent scoring system and robust validation.

---

## Table of Contents

- [Features](#características)
- [Screen Captures](#screen-captures)
- [Requirements](#requirements)
- [Installation and Building](#installation-and-building)
- [Execution](#execution)
- [Project Structure](#project-structure)
- [Architecture and Design](#architecture-and-design)
- [How to Play](#how-to-play)
- [Scoring System](#scoring-system)
- [Contributing](#contributing)
- [License](#license)

---

## Features

### Main Game

- **3 Difficulty Levels**: Easy (12x12, 5 words), Medium (15x15, 8 words), Hard (20x20, 12 words)
- **8 Directions**: Horizontal, vertical and diagonal (both directions)
- **Thematic dictionaries** by difficulty (programming, AI, engineering, etc.)
- **Colored console interface** with highlighted found words

### Scoring and Records

- **Base Score**: 100 pts per word found
- **Bonuses**: For streak (50 pts/word), speed (up to 500 pts)
- **Penalties**: -20 pts per error/hint, -60 pts for showing solution
- **Persistent Records** in JSON (Top 50 global + per level + per player)
- **Statistics**: Wins, best time, best streak, % wins

### User Experience

- **Robust validation** of all inputs (coordinates, names, menus)
- **Interactive Menu**: Search, Hint, Solution, Restart, Records, Change difficulty
- **Confirmations** for destructive actions
- **Welcome screen** and final statistics

### Code Quality

- **Modular Architecture**: Clear separation (Board, Player, Words, Scores, Validation)
- **Modern C++17**: RAII, smart pointers, std::chrono, <random>
- **Doxygen Documentation** on all public classes and methods
- **Error handling** with exceptions and defensive validation
- **Cross-platform**: Windows (UTF-8) and Linux/macOS

---

## Screen Captures

```
     SOPA DE LETRAS 15x15
    -----------------------------------

    Jugador: Juan  |  Puntos: 3/8  |  Errores: 1  |  Racha: 2

     0  1  2  3  4  5  6  7  8  9 10 11 12 13 14
   0   A  B  P  R  O  G  R  A  M  A  C  I  O  N  X
   1   X  Y  Z  O  M  P  U  T  A  D  O  R  A  Q  W
   2   U  N  I  V  E  R  S  I  D  A  D  E  R  T  Y
 ...

Leyenda:  VERDE =Encontrada  Azul=Coordenadas

Palabras:
[X] PROGRAMACION   [X] COMPUTADORA   [ ] UNIVERSIDAD
[ ] INGENIERIA     [ ] MATRICES      [ ] ALGORITMO
[ ] DESARROLLO     [ ] COMPILADOR

MENU:
1. Buscar palabra
2. Ver pista
3. Mostrar solucion
4. Reiniciar partida
5. Ver records
6. Cambiar dificultad
7. Salir

Elige una opcion:
```

---

## Requirements

### To Compile

| Tool | Minimum Version | Notes |
|------|----------------|-------|
| **CMake** | 3.15+ | Build generator |
| **C++ Compiler** | C++17 | MSVC 19.20+, GCC 7+, Clang 5+ |
| **Visual Studio** | 2019 16.6+ | For Windows development |

### To Run

- **Windows**: Console with ANSI support (Windows 10+ native, or Windows Terminal)
- **Linux/macOS**: Standard terminal

---

## Installation and Building

### Option 1: Visual Studio (Recommended on Windows)

1. **Open the project**:
   ```
   File → Open → CMake... → Select SopaDeLetras folder
   ```

2. **Configure** (automatically detected):
   - Build Type: `Release` or `Debug`
   - Architecture: `x64` (recommended)

3. **Build**:
   ```
   Build → Build All (Ctrl+Shift+B)
   ```

4. **Run**:
   ```
   Debug → Start Without Debugging (Ctrl+F5)
   ```

### Option 2: Command Line (CMake)

```bash
# Clone and enter directory
git clone <your-repo>
cd SopaDeLetras

# Configure (once)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --config Release

# Run
./build/SopaDeLetras          # Linux/macOS
build\Release\SopaDeLetras.exe  # Windows
```

### Option 3: Visual Studio Code + CMake Tools

1. Install **CMake Tools** extension
2. `Ctrl+Shift+P` → `CMake: Configure`
3. `Ctrl+Shift+P` → `CMake: Build`
4. `Ctrl+F5` to run

---

## Execution

### From the build/Release folder (Windows)

```bash
SopaDeLetras.exe
```

### From terminal (Linux/macOS)

```bash
./SopaDeLetras
```

### Game Flow

1. **Welcome** → Optional view of records
2. **Name** → Enter name (1-30 chars, alphanumeric)
3. **Difficulty** → Choose Easy/Medium/Hard
4. **Play** → Enter start and end coordinates (0-N-1)
5. **Menu** → Options 1-7 as needed
6. **Victory** → Statistics + save record
7. **Exit** → Final summary

---

## Project Structure

```
SopaDeLetras/
├── CMakeLists.txt          # Build configuration
├── README.md               # This file
├── LICENSE                 # MIT License
├── .gitignore              # Git ignored files
├── include/                # Public headers
│   ├── Config.h            # Constants, enums, global structs
│   ├── Palabra.h           # Word class and WordManager
│   ├── Tablero.h           # Board class (grid logic)
│   ├── Jugador.h           # Player class (stats, history)
│   ├── GestorPuntuaciones.h # Records persistence (JSON)
│   ├── ValidadorEntrada.h  # Robust input validation
│   └── Juego.h             # Main orchestrator
├── src/
│   └── main.cpp            # Entry point
├── data/                   # Runtime data (auto-created)
│   ├── puntuaciones.json   # Saved records
│   └── config.json         # Future configuration
└── build/                  # Generated by CMake (ignore in Git)
```

---

## Architecture and Design

### Class Diagram (Simplified)

```
┌─────────────┐
│    Juego    │ ◄── Main orchestrator
└──────┬──────┘
       │ uses
       ▼
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│  Tablero    │────▶│  Palabra    │     │GestorPalabras│
│  (grid NxN) │     │  (info)     │     │ (dictionaries)│
└─────────────┘     └─────────────┘     └─────────────┘
       │
       ▼
┌─────────────┐     ┌─────────────────┐
│  Jugador    │     │GestorPuntuaciones│
│ (stats, pts)│     │   (JSON I/O)    │
└─────────────┘     └─────────────────┘
       │
       ▼
┌─────────────────┐
│ ValidadorEntrada│
│  (input safe)   │
└─────────────────┘
```

### Applied Patterns

| Pattern | Use |
|---------|-----|
| **RAII** | Automatic memory management with `unique_ptr` |
| **Separation of responsibilities** | Each class has a unique purpose |
| **Dependency injection** | `Juego` receives configuration, doesn't create it |
| **Value Semantics** | `PalabraInfo`, `RecordEntry` are value types |
| **Fail-fast validation** | Early validation in `ValidadorEntrada` |

### Design Decisions

1. **Headers-only for small classes**: `Config.h`, `Palabra.h` are header-only for simplicity
2. **Manual simple JSON**: No external dependencies (nlohmann/json) for portability
3. **ANSI Console**: Colors via escape codes, compatible Windows 10+/Linux/macOS
4. **`std::mt19937`**: Modern random generator (not `rand()`)
5. **`std::chrono`**: Precise and portable time measurement

---

## How to Play

### Controls

| Action | Input |
|--------|-------|
| Search word | Option 1 → Start row, Start col, End row, End col |
| Request hint | Option 2 (costs 1 error) |
| Show solution | Option 3 (costs 3 errors) |
| Restart | Option 4 (confirms) |
| View records | Option 5 |
| Change difficulty | Option 6 (new game) |
| Exit | Option 7 |

### Coordinate Rules

- **Range**: 0 to N-1 (e.g., 0-14 for 15x15 board)
- **Valid directions**: Horizontal, Vertical, Diagonal (8 directions)
- **Direction**: Both ways (left→right and right→left, etc.)

### Example

```
Word: PROGRAMACION (12 letters)
Start: (0, 2)  →  End: (0, 13)  ✓ Horizontal right
Start: (0, 13) →  End: (0, 2)   ✓ Horizontal left
Start: (0, 2)  →  End: (11, 2)  ✓ Vertical down
```

---

## Scoring System

### Formula

```
SCORE = (Words × 100) + (Maximum Streak × 50) - (Errors × 20) + Speed Bonus
```

### Speed Bonus

- Maximum 500 extra points
- `max(0, 500 - best_time_seconds / 10)`
- Encourages fast play without over-penalizing

### Records Table (JSON)

```json
[
  {
    "nombre": "Juan",
    "puntuacion": 1250,
    "nivel": 2,
    "palabras_encontradas": 8,
    "errores": 2,
    "tiempo_segundos": 180,
    "fecha": "2026-11-24 15:30",
    "victoria": true
  }
]
```

---

## Contributing

1. Fork the repository
2. Create branch: `git checkout -b feature/new-functionality`
3. Commit: `git commit -m 'feat: add X'`
4. Push: `git push origin feature/new-functionality`
5. Open Pull Request

### Code Standards

- C++17, approximately Google/LLVM style
- Document public API with Doxygen
- Tests for new logic (future)
- Semantic commits: `feat:`, `fix:`, `docs:`, `refactor:`

---

## License

This project is under the **MIT License**. See [LICENSE](LICENSE) for details.

```
MIT License - Free use, modification and distribution.
Include license copy in redistributions.
```

---

## Author

Developed as a Software Engineering portfolio project.
Demonstrates: Modern C++, clean architecture, persistence, console UX, build system.

---

⭐ **Give a star if you like the project!**