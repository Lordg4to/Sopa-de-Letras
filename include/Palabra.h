#ifndef PALABRA_H
#define PALABRA_H

#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include "Config.h"

namespace SopaDeLetras {

/**
 * @brief Clase para gestionar una palabra individual en la sopa de letras
 * Maneja su posición, dirección y estado de encontrada
 */
class Palabra {
private:
    PalabraInfo info_;
    int direccion_fila_ = 0;
    int direccion_col_ = 0;

public:
    // Constructores
    Palabra() = default;
    explicit Palabra(const std::string& texto) : info_(texto) {}
    Palabra(const std::string& texto, int f_inicio, int c_inicio, int df, int dc)
        : info_(texto), direccion_fila_(df), direccion_col_(dc) {
        info_.fila_inicio = f_inicio;
        info_.col_inicio = c_inicio;
        int largo = static_cast<int>(texto.length());
        info_.fila_fin = f_inicio + (largo - 1) * df;
        info_.col_fin = c_inicio + (largo - 1) * dc;
    }

    // Getters
    const std::string& obtenerTexto() const { return info_.texto; }
    bool estaEncontrada() const { return info_.encontrada; }
    int obtenerFilaInicio() const { return info_.fila_inicio; }
    int obtenerColInicio() const { return info_.col_inicio; }
    int obtenerFilaFin() const { return info_.fila_fin; }
    int obtenerColFin() const { return info_.col_fin; }
    int obtenerDireccionFila() const { return direccion_fila_; }
    int obtenerDireccionCol() const { return direccion_col_; }
    int obtenerLongitud() const { return static_cast<int>(info_.texto.length()); }

    // Setters
    void establecerPosicion(int f_inicio, int c_inicio, int df, int dc) {
        info_.fila_inicio = f_inicio;
        info_.col_inicio = c_inicio;
        direccion_fila_ = df;
        direccion_col_ = dc;
        int largo = static_cast<int>(info_.texto.length());
        info_.fila_fin = f_inicio + (largo - 1) * df;
        info_.col_fin = c_inicio + (largo - 1) * dc;
    }

    void marcarEncontrada() { info_.encontrada = true; }
    void reiniciar() {
        info_.encontrada = false;
        info_.fila_inicio = info_.col_inicio = info_.fila_fin = info_.col_fin = -1;
        direccion_fila_ = direccion_col_ = 0;
    }

    /**
     * @brief Verifica si una coordenada pertenece a esta palabra
     */
    bool contieneCoordenada(int fila, int col) const {
        if (!estaEncontrada()) return false;

        int df = direccion_fila_;
        int dc = direccion_col_;
        int largo = obtenerLongitud();

        for (int k = 0; k < largo; ++k) {
            int pf = info_.fila_inicio + k * df;
            int pc = info_.col_inicio + k * dc;
            if (pf == fila && pc == col) return true;
        }
        return false;
    }

    /**
     * @brief Obtiene la pista de la palabra (posición inicial)
     */
    std::string obtenerPista() const {
        return "La palabra \"" + info_.texto + "\" empieza en ("
               + std::to_string(info_.fila_inicio) + ", "
               + std::to_string(info_.col_inicio) + ")";
    }

    /**
     * @brief Obtiene la solución completa de la palabra
     */
    std::string obtenerSolucion() const {
        return info_.texto + ": ("
               + std::to_string(info_.fila_inicio) + ","
               + std::to_string(info_.col_inicio) + ") -> ("
               + std::to_string(info_.fila_fin) + ","
               + std::to_string(info_.col_fin) + ")";
    }
};

/**
 * @brief Clase gestora de palabras - carga y selecciona palabras según dificultad
 */
class GestorPalabras {
private:
    std::vector<std::string> diccionario_facil_ = {
        "CASA", "SOL", "LUNA", "RIO", "MAR", "FLOR", "ARBOL", "CIELO",
        "TIERRA", "FUEGO", "AGUA", "AIRE", "ROCA", "PLAYA", "BOSQUE"
    };

    std::vector<std::string> diccionario_medio_ = {
        "PROGRAMACION", "COMPUTADORA", "UNIVERSIDAD", "INGENIERIA",
        "MATRICES", "ALGORITMO", "DESARROLLO", "COMPILADOR",
        "SOFTWARE", "HARDWARE", "RED", "SERVIDOR", "CLIENTE",
        "BASEDATOS", "SEGURIDAD", "ENCRIPTAR"
    };

    std::vector<std::string> diccionario_dificil_ = {
        "INTELIGENCIA", "ARTIFICIAL", "APRENDIZAJE", "PROFUNDO",
        "REDES", "NEURONALES", "PROCESAMIENTO", "LENGUAJE",
        "NATURAL", "VISION", "COMPUTACIONAL", "ROBOTICA",
        "AUTOMATIZACION", "ALGORITMICA", "COMPLEJIDAD",
        "OPTIMIZACION", "PARALELISMO", "CONCURRENCIA",
        "DISTRIBUIDO", "ESCALABILIDAD", "MICROSERVICIOS",
        "CONTENEDORES", "ORQUESTACION", "DEVOPS", "CIBERSEGURIDAD"
    };

    // Generador aleatorio para mezclar
    mutable std::mt19937 gen_{std::random_device{}()};

public:
    /**
     * @brief Obtiene palabras aleatorias según la dificultad
     * @param dificultad Nivel de dificultad
     * @param cantidad Número de palabras a obtener
     * @return Vector con las palabras seleccionadas
     */
    std::vector<std::string> obtenerPalabras(Dificultad dificultad, int cantidad) const {
        const std::vector<std::string>* diccionario = nullptr;

        switch (dificultad) {
            case Dificultad::FACIL:
                diccionario = &diccionario_facil_;
                break;
            case Dificultad::MEDIO:
                diccionario = &diccionario_medio_;
                break;
            case Dificultad::DIFICIL:
                diccionario = &diccionario_dificil_;
                break;
        }

        if (!diccionario || diccionario->empty()) {
            return {};
        }

        // Crear una copia y mezclar con std::shuffle (C++17)
        std::vector<std::string> seleccionadas = *diccionario;
        std::shuffle(seleccionadas.begin(), seleccionadas.end(), gen_);

        // Tomar solo la cantidad solicitada
        if (static_cast<int>(seleccionadas.size()) > cantidad) {
            seleccionadas.resize(cantidad);
        }

        return seleccionadas;
    }

    /**
     * @brief Carga palabras desde un archivo externo
     * @param ruta_archivo Ruta al archivo de palabras
     * @param dificultad Dificultad a la que pertenecen
     * @return true si se cargaron correctamente
     */
    bool cargarDesdeArchivo(const std::string& ruta_archivo, Dificultad dificultad) {
        // Implementación futura para cargar desde archivo
        (void)ruta_archivo;
        (void)dificultad;
        return false;
    }

    /**
     * @brief Agrega una palabra personalizada al diccionario
     */
    void agregarPalabraPersonalizada(const std::string& palabra, Dificultad dificultad) {
        std::string upper = palabra;
        std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

        switch (dificultad) {
            case Dificultad::FACIL:
                diccionario_facil_.push_back(upper);
                break;
            case Dificultad::MEDIO:
                diccionario_medio_.push_back(upper);
                break;
            case Dificultad::DIFICIL:
                diccionario_dificil_.push_back(upper);
                break;
        }
    }
};

} // namespace SopaDeLetras

#endif // PALABRA_H