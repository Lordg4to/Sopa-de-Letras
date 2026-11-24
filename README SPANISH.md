# Sopa de Letras Pro

[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/std/the-standard)
[![CMake](https://img.shields.io/badge/CMake-3.15+-green.svg)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-lightgrey.svg)](https://github.com)

> **Juego profesional de Sopa de Letras desarrollado en C++17** para portafolio de Ingeniería de Software.
> Arquitectura modular, niveles de dificultad, sistema de puntuación persistente y validación robusta.

---

## Tabla de Contenidos

- [Características](#características)
- [Capturas de Pantalla](#capturas-de-pantalla)
- [Requisitos](#requisitos)
- [Instalación y Compilación](#instalación-y-compilación)
- [Ejecución](#ejecución)
- [Estructura del Proyecto](#estructura-del-proyecto)
- [Arquitectura y Diseño](#arquitectura-y-diseño)
- [Cómo Jugar](#cómo-jugar)
- [Sistema de Puntuación](#sistema-de-puntuación)
- [Contribuir](#contribuir)
- [Licencia](#licencia)

---

## Características

### Juego Principal
- **3 Niveles de Dificultad**: Fácil (12×12, 5 palabras), Medio (15×15, 8 palabras), Difícil (20×20, 12 palabras)
- **8 Direcciones**: Horizontal, vertical y diagonal (ambas direcciones)
- **Diccionarios temáticos** por dificultad (programación, IA, ingeniería, etc.)
- **Interfaz de consola coloreada** con resaltado de palabras encontradas

### Sistema de Puntuación y Récords
- **Puntuación base**: 100 pts por palabra encontrada
- **Bonificaciones**: Por racha (50 pts/palabra), velocidad (hasta 500 pts)
- **Penalizaciones**: -20 pts por error/pista, -60 pts por ver solución
- **Récords persistentes** en JSON (Top 50 global + por nivel + por jugador)
- **Estadísticas**: Victorias, mejor tiempo, mejor racha, % victorias

### Experiencia de Usuario
- **Validación robusta** de todas las entradas (coordenadas, nombres, menús)
- **Menú interactivo**: Buscar, Pista, Solución, Reiniciar, Records, Cambiar dificultad
- **Confirmaciones** para acciones destructivas
- **Pantalla de bienvenida** y estadísticas finales

### Calidad de Código
- **Arquitectura modular**: Separación clara (Tablero, Jugador, Palabras, Puntuaciones, Validación)
- **C++17 moderno**: RAII, smart pointers, std::chrono, <random>
- **Documentación Doxygen** en todas las clases y métodos públicos
- **Manejo de errores** con excepciones y validación defensiva
- **Cross-platform**: Windows (UTF-8) y Linux/macOS

---

## Capturas de Pantalla

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

## Requisitos

### Para Compilar
| Herramienta | Versión Mínima | Notas |
|-------------|----------------|-------|
| **CMake** | 3.15+ | Generador de build |
| **Compilador C++** | C++17 | MSVC 19.20+, GCC 7+, Clang 5+ |
| **Visual Studio** | 2019 16.6+ | Para desarrollo en Windows |

### Para Ejecutar
- **Windows**: Consola con soporte ANSI (Windows 10+ nativo, o Windows Terminal)
- **Linux/macOS**: Terminal estándar

---

## Instalación y Compilación

### Opción 1: Visual Studio (Recomendado en Windows)

1. **Abrir el proyecto**:
   ```
   File → Open → CMake... → Seleccionar carpeta SopaDeLetras
   ```

2. **Configurar** (se detecta automáticamente):
   - Build Type: `Release` o `Debug`
   - Architecture: `x64` (recomendado)

3. **Compilar**:
   ```
   Build → Build All (Ctrl+Shift+B)
   ```

4. **Ejecutar**:
   ```
   Debug → Start Without Debugging (Ctrl+F5)
   ```

### Opción 2: Línea de Comandos (CMake)

```bash
# Clonar y entrar al directorio
git clone <tu-repo>
cd SopaDeLetras

# Configurar (una sola vez)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Compilar
cmake --build build --config Release

# Ejecutar
./build/SopaDeLetras          # Linux/macOS
build\Release\SopaDeLetras.exe  # Windows
```

### Opción 3: Visual Studio Code + CMake Tools

1. Instalar extensión **CMake Tools**
2. `Ctrl+Shift+P` → `CMake: Configure`
3. `Ctrl+Shift+P` → `CMake: Build`
4. `Ctrl+F5` para ejecutar

---

## Ejecución

### Opción 1: Ejecutable ya compilado (recomendado)

El ejecutable ya está compilado en `build\SopaDeLetras.exe`. Para ejecutarlo:

```powershell
cd C:\Users\Usuario\Desktop\CLAUDE\SopaDeLetra\build
.\SopaDeLetras.exe
```

Al ejecutarse, el juego te pedirá:
1. Ver si quieres ver records anteriores
2. Ingresar tu nombre
3. Seleccionar dificultad (Fácil/Medio/Difícil)
4. Jugar buscando palabras en el tablero

### Opción 2: Compilar desde el código fuente

```powershell
# Compilar
cl.exe /std:c++17 /EHsc /I include /c src\main.cpp /Fo build\main.obj
link build\main.obj /OUT:build\SopaDeLetras.exe

# Ejecutar
build\SopaDeLetras.exe
```

### Flujo de Juego

1. **Bienvenida** → Ver records opcional
2. **Nombre** → Ingresar nombre (1-30 chars, alfanumérico)
3. **Dificultad** → Elegir Fácil/Medio/Difícil
4. **Jugar** → Ingresar coordenadas inicio y fin (0-N-1)
5. **Menú** → Opciones 1-7 según necesidad
6. **Victoria** → Estadísticas + guardar record
7. **Salida** → Resumen final

---

## Estructura del Proyecto

```
SopaDeLetras/
├── CMakeLists.txt          # Configuración de build
├── README.md               # Este archivo
├── LICENSE                 # Licencia MIT
├── .gitignore              # Archivos ignorados por Git
├── include/                # Headers públicos
│   ├── Config.h            # Constantes, enums, structs globales
│   ├── Palabra.h           # Clase Palabra y GestorPalabras
│   ├── Tablero.h           # Clase Tablero (lógica del grid)
│   ├── Jugador.h           # Clase Jugador (stats, historial)
│   ├── GestorPuntuaciones.h # Récords persistentes (JSON)
│   ├── ValidadorEntrada.h  # Validación robusta de entrada
│   └── Juego.h             # Orquestador principal
├── src/
│   └── main.cpp            # Punto de entrada
├── data/                   # Datos en tiempo de ejecución (creado auto)
│   ├── puntuaciones.json   # Récords guardados
│   └── config.json         # Configuración futura
└── build/                  # Generado por CMake (ignorar en Git)
```

---

## Arquitectura y Diseño

### Diagrama de Clases (Simplificado)

```
┌─────────────┐
│    Juego    │ ◄── Orquestador principal
└──────┬──────┘
       │ usa
       ▼
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│  Tablero    │────▶│  Palabra    │     │GestorPalabras│
│  (grid NxN) │     │  (info)     │     │ (diccionarios)│
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

### Patrones Aplicados

| Patrón | Uso |
|--------|-----|
| **RAII** | Gestión automática de memoria con `unique_ptr` |
| **Separación de responsabilidades** | Cada clase tiene un propósito único |
| **Inyección de dependencias** | `Juego` recibe configuración, no la crea |
| **Value Semantics** | `PalabraInfo`, `RecordEntry` son tipos valor |
| **Fail-fast validation** | Validación temprana en `ValidadorEntrada` |

### Decisiones de Diseño

1. **Headers-only para clases pequeñas**: `Config.h`, `Palabra.h` son header-only para simplicidad
2. **JSON manual simple**: Sin dependencias externas (nlohmann/json) para portabilidad
3. **Consola ANSI**: Colores via escape codes, compatible Windows 10+/Linux/macOS
4. **`std::mt19937`**: Generador aleatorio moderno (no `rand()`)
5. **`std::chrono`**: Medición de tiempo precisa y portable

---

## Cómo Jugar

### Controles
| Acción | Entrada |
|--------|---------|
| Buscar palabra | Opción 1 → Fila ini, Col ini, Fila fin, Col fin |
| Pedir pista | Opción 2 (cuesta 1 error) |
| Ver solución | Opción 3 (cuesta 3 errores) |
| Reiniciar | Opción 4 (confirma) |
| Ver records | Opción 5 |
| Cambiar dificultad | Opción 6 (nueva partida) |
| Salir | Opción 7 |

### Reglas de Coordenadas
- **Rango**: 0 a N-1 (ej. 0-14 para tablero 15×15)
- **Direcciones válidas**: Horizontal, Vertical, Diagonal (8 direcciones)
- **Sentido**: Ambos (izq→der y der→izq, etc.)

### Ejemplo
```
Palabra: PROGRAMACION (12 letras)
Inicio: (0, 2)  →  Fin: (0, 13)  ✓ Horizontal derecha
Inicio: (0, 13) →  Fin: (0, 2)   ✓ Horizontal izquierda
Inicio: (0, 2)  →  Fin: (11, 2)  ✓ Vertical abajo
```

---

## Sistema de Puntuación

### Fórmula
```
PUNTUACIÓN = (Palabras × 100) + (Racha_Máxima × 50) - (Errores × 20) + Bonificación_Velocidad
```

### Bonificación de Velocidad
- Máximo 500 puntos extra
- `max(0, 500 - mejor_tiempo_segundos / 10)`
- Fomenta juego rápido sin penalizar demasiado

### Tabla de Records (JSON)
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

## Contribuir

1. Fork del repositorio
2. Crear rama: `git checkout -b feature/nueva-funcionalidad`
3. Commit: `git commit -m 'feat: agregar X'`
4. Push: `git push origin feature/nueva-funcionalidad`
5. Abrir Pull Request

### Estándares de Código
- C++17, estilo Google/LLVM aproximadamente
- Documentar API pública con Doxygen
- Tests para nueva lógica (futuro)
- Commits semánticos: `feat:`, `fix:`, `docs:`, `refactor:`

---

## Licencia

Este proyecto está bajo la **Licencia MIT**. Ver [LICENSE](LICENSE) para detalles.

```
MIT License - Libre uso, modificación y distribución.
Incluir copia de la licencia en redistribuciones.
```

---

## Autor

Desarrollado como proyecto de portafolio para **Ingeniería de Software**.
Demuestra: C++ moderno, arquitectura limpia, persistencia, UX de consola, build system.

---

⭐ **¡Dale una estrella si te gusta el proyecto!**