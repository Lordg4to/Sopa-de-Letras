#ifndef TABLERO_H
#define TABLERO_H

#include <vector>
#include <string>
#include <random>
#include "Config.h"
#include "Palabra.h"

namespace SopaDeLetras {

/**
 * @brief Clase que representa el tablero de la sopa de letras
 * Maneja la colocación de palabras, relleno de espacios vacíos y visualización
 */
class Tablero {
private:
    int tamano_;
    std::vector<std::vector<char>> grid_;
    std::vector<Palabra> palabras_;
    mutable std::mt19937 generador_;

    // Direcciones: 0=→, 1=←, 2=↓, 3=↑, 4=↘, 5=↙, 6=↗, 7=↖
    static constexpr int DF[8] = {0, 0, 1, -1, 1, 1, -1, -1};
    static constexpr int DC[8] = {1, -1, 0, 0, 1, -1, 1, -1};

public:
    /**
     * @brief Constructor
     * @param tamano Tamaño del tablero (N x N)
     */
    explicit Tablero(int tamano = Config::TABLERO_DEFAULT)
        : tamano_(tamano), generador_(std::random_device{}()) {
        inicializar();
    }

    /**
     * @brief Inicializa el tablero con espacios vacíos
     */
    void inicializar() {
        grid_.assign(tamano_, std::vector<char>(tamano_, ' '));
        palabras_.clear();
    }

    /**
     * @brief Cambia el tamaño del tablero
     */
    void establecerTamano(int tamano) {
        tamano_ = std::clamp(tamano, Config::TABLERO_MIN, Config::TABLERO_MAX);
        inicializar();
    }

    int obtenerTamano() const { return tamano_; }
    const std::vector<Palabra>& obtenerPalabras() const { return palabras_; }
    std::vector<Palabra>& obtenerPalabrasModificables() { return palabras_; }
    const std::vector<std::vector<char>>& obtenerGrid() const { return grid_; }

    /**
     * @brief Verifica si una palabra puede colocarse en la posición y dirección dadas
     */
    bool puedeColocar(const std::string& texto, int fila, int col, int dir_idx) const {
        int df = DF[dir_idx];
        int dc = DC[dir_idx];
        int largo = static_cast<int>(texto.length());

        // Verificar límites
        int fila_fin = fila + df * (largo - 1);
        int col_fin = col + dc * (largo - 1);

        if (fila_fin < 0 || fila_fin >= tamano_ || col_fin < 0 || col_fin >= tamano_) {
            return false;
        }

        // Verificar colisiones (solo permite si la celda está vacía o tiene la misma letra)
        for (int k = 0; k < largo; ++k) {
            int f = fila + k * df;
            int c = col + k * dc;
            char actual = grid_[f][c];
            if (actual != ' ' && actual != texto[k]) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Coloca una palabra en el tablero
     * @return true si se colocó correctamente
     */
    bool colocarPalabra(Palabra& palabra) {
        const std::string& texto = palabra.obtenerTexto();
        const int MAX_INTENTOS = 1000;
        std::uniform_int_distribution<int> dist_fila(0, tamano_ - 1);
        std::uniform_int_distribution<int> dist_col(0, tamano_ - 1);
        std::uniform_int_distribution<int> dist_dir(0, 7);

        // Primer intento: aleatorio
        for (int intento = 0; intento < MAX_INTENTOS; ++intento) {
            int fila = dist_fila(generador_);
            int col = dist_col(generador_);
            int dir = dist_dir(generador_);

            if (puedeColocar(texto, fila, col, dir)) {
                int df = DF[dir];
                int dc = DC[dir];

                // Colocar la palabra en el grid
                for (int k = 0; k < static_cast<int>(texto.length()); ++k) {
                    grid_[fila + k * df][col + k * dc] = texto[k];
                }

                // Guardar información de la palabra
                palabra.establecerPosicion(fila, col, df, dc);
                palabras_.push_back(palabra);
                return true;
            }
        }

        // Segundo intento: búsqueda sistemática
        for (int fila = 0; fila < tamano_; ++fila) {
            for (int col = 0; col < tamano_; ++col) {
                for (int dir = 0; dir < 8; ++dir) {
                    if (puedeColocar(texto, fila, col, dir)) {
                        int df = DF[dir];
                        int dc = DC[dir];

                        for (int k = 0; k < static_cast<int>(texto.length()); ++k) {
                            grid_[fila + k * df][col + k * dc] = texto[k];
                        }

                        palabra.establecerPosicion(fila, col, df, dc);
                        palabras_.push_back(palabra);
                        return true;
                    }
                }
            }
        }

        return false; // No se pudo colocar
    }

    /**
     * @brief Coloca múltiples palabras en el tablero
     * @return Número de palabras colocadas exitosamente
     */
    int colocarPalabras(const std::vector<std::string>& textos_palabras) {
        int colocadas = 0;
        for (const auto& texto : textos_palabras) {
            Palabra p(texto);
            if (colocarPalabra(p)) {
                ++colocadas;
            }
        }
        return colocadas;
    }

    /**
     * @brief Rellena los espacios vacíos con letras aleatorias
     */
    void rellenarVacios() {
        std::uniform_int_distribution<int> dist_letra(0, 25);
        for (int i = 0; i < tamano_; ++i) {
            for (int j = 0; j < tamano_; ++j) {
                if (grid_[i][j] == ' ') {
                    grid_[i][j] = 'A' + dist_letra(generador_);
                }
            }
        }
    }

    /**
     * @brief Genera una nueva sopa de letras completa
     * @param textos_palabras Lista de palabras a colocar
     * @return true si todas las palabras se colocaron
     */
    bool generar(const std::vector<std::string>& textos_palabras) {
        inicializar();
        int colocadas = colocarPalabras(textos_palabras);
        rellenarVacios();
        return colocadas == static_cast<int>(textos_palabras.size());
    }

    /**
     * @brief Reinicia el estado de "encontrada" de todas las palabras
     */
    void reiniciarPalabras() {
        for (auto& p : palabras_) {
            p.marcarEncontrada(); // Para que se muestren en verde si se reinicia la partida
        }
    }

    /**
     * @brief Obtiene el carácter en una posición
     */
    char obtenerCelda(int fila, int col) const {
        if (fila >= 0 && fila < tamano_ && col >= 0 && col < tamano_) {
            return grid_[fila][col];
        }
        return ' ';
    }

    /**
     * @brief Verifica si las coordenadas forman una dirección válida
     * @return true si es horizontal, vertical o diagonal
     */
    static bool direccionValida(int f1, int c1, int f2, int c2, int& df, int& dc) {
        if (f1 == f2 && c1 == c2) return false;

        df = 0; dc = 0;
        if (f2 > f1) df = 1;
        else if (f2 < f1) df = -1;
        if (c2 > c1) dc = 1;
        else if (c2 < c1) dc = -1;

        int df_abs = std::abs(f2 - f1);
        int dc_abs = std::abs(c2 - c1);
        return (f1 == f2) || (c1 == c2) || (df_abs == dc_abs);
    }

    /**
     * @brief Verifica si la selección del jugador corresponde a una palabra
     * @param f1, c1 Coordenadas de inicio
     * @param f2, c2 Coordenadas de fin
     * @return Índice de la palabra encontrada, -1 si no coincide
     */
    int verificarSeleccion(int f1, int c1, int f2, int c2) {
        int df, dc;
        if (!direccionValida(f1, c1, f2, c2, df, dc)) {
            return -2; // Dirección inválida
        }

        int largo = (std::max)(std::abs(f2 - f1), std::abs(c2 - c1)) + 1;

        for (size_t i = 0; i < palabras_.size(); ++i) {
            if (palabras_[i].estaEncontrada()) continue;
            if (palabras_[i].obtenerLongitud() != largo) continue;

            // Verificar en dirección normal
            bool coincide = true;
            for (int k = 0; k < largo; ++k) {
                int f = f1 + k * df;
                int c = c1 + k * dc;
                if (f < 0 || f >= tamano_ || c < 0 || c >= tamano_ ||
                    grid_[f][c] != palabras_[i].obtenerTexto()[k]) {
                    coincide = false;
                    break;
                }
            }
            if (coincide) {
                palabras_[i].marcarEncontrada();
                return static_cast<int>(i);
            }

            // Verificar en dirección inversa
            coincide = true;
            for (int k = 0; k < largo; ++k) {
                int f = f1 + k * df;
                int c = c1 + k * dc;
                if (f < 0 || f >= tamano_ || c < 0 || c >= tamano_ ||
                    grid_[f][c] != palabras_[i].obtenerTexto()[largo - 1 - k]) {
                    coincide = false;
                    break;
                }
            }
            if (coincide) {
                palabras_[i].marcarEncontrada();
                return static_cast<int>(i);
            }
        }
        return -1; // No se encontró ninguna palabra
    }

    /**
     * @brief Verifica si todas las palabras han sido encontradas
     */
    bool todasEncontradas() const {
        for (const auto& p : palabras_) {
            if (!p.estaEncontrada()) return false;
        }
        return true;
    }

    /**
     * @brief Cuenta cuántas palabras han sido encontradas
     */
    int contarEncontradas() const {
        int count = 0;
        for (const auto& p : palabras_) {
            if (p.estaEncontrada()) ++count;
        }
        return count;
    }

    /**
     * @brief Obtiene una palabra aleatoria no encontrada para dar pista
     * @return Índice de la palabra o -1 si todas están encontradas
     */
    int obtenerPalabraParaPista() const {
        std::vector<int> pendientes;
        for (size_t i = 0; i < palabras_.size(); ++i) {
            if (!palabras_[i].estaEncontrada()) {
                pendientes.push_back(static_cast<int>(i));
            }
        }
        if (pendientes.empty()) return -1;

        std::uniform_int_distribution<int> dist(0, static_cast<int>(pendientes.size()) - 1);
        return pendientes[dist(generador_)];
    }
};

} // namespace SopaDeLetras

#endif // TABLERO_H