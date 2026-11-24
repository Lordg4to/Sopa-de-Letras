#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>

namespace SopaDeLetras {

/**
 * @brief Configuración centralizada del juego
 * Contiene todas las constantes y valores configurables
 */
namespace Config {
    // Dimensiones del tablero
    constexpr int TABLERO_MIN = 10;
    constexpr int TABLERO_MAX = 30;
    constexpr int TABLERO_DEFAULT = 15;

    // Número de palabras por dificultad
    constexpr int PALABRAS_FACIL = 5;
    constexpr int PALABRAS_MEDIO = 8;
    constexpr int PALABRAS_DIFICIL = 12;

    // Direcciones válidas (8 direcciones: H, V, D y sus inversas)
    constexpr int DIRECCIONES = 8;

    // Configuración de colores para consola
    constexpr const char* RESET = "\033[0m";
    constexpr const char* BOLD = "\033[1m";
    constexpr const char* RED = "\033[31m";
    constexpr const char* GREEN = "\033[32m";
    constexpr const char* YELLOW = "\033[33m";
    constexpr const char* BLUE = "\033[34m";
    constexpr const char* MAGENTA = "\033[35m";
    constexpr const char* CYAN = "\033[36m";
    constexpr const char* BG_GREEN = "\033[42m";
    constexpr const char* CLEAR_SCREEN = "\033[2J\033[H";

    // Archivos de datos
    constexpr const char* ARCHIVO_PUNTUACIONES = "data/puntuaciones.json";
    constexpr const char* ARCHIVO_CONFIG = "data/config.json";
    constexpr const char* ARCHIVO_PALABRAS = "data/palabras.txt";

    // Límites de validación
    constexpr int MAX_NOMBRE_JUGADOR = 30;
    constexpr int MIN_NOMBRE_JUGADOR = 1;
    constexpr int MAX_PUNTOS_HISTORIAL = 100;
}

/**
 * @brief Niveles de dificultad del juego
 */
enum class Dificultad {
    FACIL = 1,
    MEDIO = 2,
    DIFICIL = 3
};

/**
 * @brief Estructura para almacenar información de una palabra
 */
struct PalabraInfo {
    std::string texto;
    bool encontrada = false;
    int fila_inicio = -1, col_inicio = -1;
    int fila_fin = -1, col_fin = -1;

    PalabraInfo() = default;
    PalabraInfo(const std::string& t) : texto(t) {}
};

/**
 * @brief Estructura para almacenar estadísticas del jugador
 */
struct EstadisticasJugador {
    std::string nombre;
    int puntos = 0;
    int errores = 0;
    int nivel = 1;
    long long tiempo_total = 0; // en segundos
    std::string fecha_partida;

    EstadisticasJugador() = default;
    EstadisticasJugador(const std::string& n) : nombre(n) {}
};

/**
 * @brief Configuración de una partida
 */
struct ConfiguracionPartida {
    Dificultad dificultad = Dificultad::MEDIO;
    int tamano_tablero = Config::TABLERO_DEFAULT;
    int num_palabras = Config::PALABRAS_MEDIO;
    bool pistas_habilitadas = true;
    bool mostrar_solucion = false;

    ConfiguracionPartida() = default;
    ConfiguracionPartida(Dificultad d) : dificultad(d) {
        switch (d) {
            case Dificultad::FACIL:
                tamano_tablero = 12;
                num_palabras = Config::PALABRAS_FACIL;
                break;
            case Dificultad::MEDIO:
                tamano_tablero = 15;
                num_palabras = Config::PALABRAS_MEDIO;
                break;
            case Dificultad::DIFICIL:
                tamano_tablero = 20;
                num_palabras = Config::PALABRAS_DIFICIL;
                break;
        }
    }
};

} // namespace SopaDeLetras

#endif // CONFIG_H