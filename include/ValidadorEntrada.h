#ifndef VALIDADOR_ENTRADA_H
#define VALIDADOR_ENTRADA_H

#include <string>
#include <iostream>
#include <limits>
#include <cctype>
#include "Config.h"

namespace SopaDeLetras {

/**
 * @brief Clase para validar y leer entradas del usuario de forma robusta
 * Maneja errores de formato, rangos, y entradas vacías
 */
class ValidadorEntrada {
public:
    /**
     * @brief Lee un entero del usuario con validación completa
     * @param prompt Mensaje a mostrar
     * @param min Valor mínimo permitido
     * @param max Valor máximo permitido
     * @param valor Variable donde almacenar el resultado
     * @return true si se leyó correctamente, false si el usuario canceló
     */
    static bool leerEntero(const std::string& prompt, int min, int max, int& valor) {
        while (true) {
            std::cout << prompt;
            std::string input;
            if (!std::getline(std::cin, input)) {
                // Fin de entrada estándar (EOF): cancelar
                return false;
            }

            // Verificar entrada vacía
            if (input.empty()) {
                std::cout << Config::RED << "[X] Entrada vacia. Intenta de nuevo." << Config::RESET << "\n";
                continue;
            }

            // Verificar si el usuario quiere cancelar (opcional)
            if (input == "q" || input == "Q" || input == "salir" || input == "exit") {
                return false;
            }

            // Validar que solo contenga dígitos (y opcionalmente signo)
            bool valido = true;
            size_t inicio = 0;
            if (input[0] == '+' || input[0] == '-') {
                inicio = 1;
                if (input.length() == 1) valido = false;
            }

            for (size_t i = inicio; i < input.length() && valido; ++i) {
                if (!std::isdigit(static_cast<unsigned char>(input[i]))) {
                    valido = false;
                }
            }

            if (!valido) {
                std::cout << Config::RED << "[X] Entrada invalida. Ingresa solo numeros enteros." << Config::RESET << "\n";
                continue;
            }

            try {
                long long temp = std::stoll(input);
                if (temp < min || temp > max) {
                    std::cout << Config::RED << "[X] Valor fuera de rango [" << min << "-" << max << "]." << Config::RESET << "\n";
                    continue;
                }
                valor = static_cast<int>(temp);
                return true;
            } catch (const std::exception&) {
                std::cout << Config::RED << "[X] Numero demasiado grande. Intenta de nuevo." << Config::RESET << "\n";
            }
        }
    }

    /**
     * @brief Lee un entero con valores por defecto para el tablero
     */
    static bool leerCoordenada(const std::string& prompt, int max, int& valor) {
        return leerEntero(prompt, 0, max - 1, valor);
    }

    /**
     * @brief Lee un string no vacío con longitud máxima
     */
    static bool leerTexto(const std::string& prompt, int max_longitud, std::string& valor,
                          bool permitir_vacio = false) {
        while (true) {
            std::cout << prompt;
            if (!std::getline(std::cin, valor)) {
                // Fin de entrada estándar (EOF): cancelar
                return false;
            }

            if (valor.empty()) {
                if (permitir_vacio) return true;
                std::cout << Config::RED << "[X] Entrada vacia. Intenta de nuevo." << Config::RESET << "\n";
                continue;
            }

            if (static_cast<int>(valor.length()) > max_longitud) {
                std::cout << Config::RED << "[X] Texto demasiado largo (max " << max_longitud << " caracteres)." << Config::RESET << "\n";
                continue;
            }

            // Limpiar: solo alfanuméricos, espacios y algunos símbolos
            bool valido = true;
            for (char c : valor) {
                if (!std::isalnum(static_cast<unsigned char>(c)) && c != ' ' && c != '_' && c != '-') {
                    valido = false;
                    break;
                }
            }

            if (!valido) {
                std::cout << Config::RED << "[X] Caracteres no permitidos. Usa solo letras, numeros, espacios, _ y -." << Config::RESET << "\n";
                continue;
            }

            return true;
        }
    }

    /**
     * @brief Lee el nombre del jugador
     */
    static bool leerNombreJugador(std::string& nombre) {
        std::string prompt = std::string(Config::CYAN) + "Ingresa tu nombre: " + Config::RESET;
        return leerTexto(prompt, Config::MAX_NOMBRE_JUGADOR, nombre, false);
    }

    /**
     * @brief Lee una opción de menú (1-N)
     */
    static bool leerOpcionMenu(int min, int max, int& opcion) {
        std::string prompt = std::string(Config::CYAN) + "Elige una opcion: " + Config::RESET;
        return leerEntero(prompt, min, max, opcion);
    }

    /**
     * @brief Lee confirmación S/N
     */
    static bool leerConfirmacion(const std::string& prompt) {
        while (true) {
            std::cout << prompt << " (S/N): ";
            std::string input;
            if (!std::getline(std::cin, input)) {
                // Fin de entrada estándar (EOF): cancelar (equivale a No)
                return false;
            }

            if (input.empty()) continue;

            char c = std::toupper(static_cast<unsigned char>(input[0]));
            if (c == 'S' || c == 'Y') return true;
            if (c == 'N') return false;

            std::cout << Config::RED << "[X] Responde S (si) o N (no)." << Config::RESET << "\n";
        }
    }

    /**
     * @brief Lee selección de dificultad
     */
    static bool leerDificultad(SopaDeLetras::Dificultad& dificultad) {
        using namespace SopaDeLetras::Config;
        std::cout << BOLD << BLUE << "\n=== SELECCIONAR DIFICULTAD ===" << RESET << "\n";
        std::cout << GREEN << "1." << RESET << " Facil (12x12, 5 palabras)\n";
        std::cout << GREEN << "2." << RESET << " Medio (15x15, 8 palabras)\n";
        std::cout << GREEN << "3." << RESET << " Dificil (20x20, 12 palabras)\n\n";

        int opcion;
        if (!leerOpcionMenu(1, 3, opcion)) return false;

        switch (opcion) {
            case 1: dificultad = SopaDeLetras::Dificultad::FACIL; break;
            case 2: dificultad = SopaDeLetras::Dificultad::MEDIO; break;
            case 3: dificultad = SopaDeLetras::Dificultad::DIFICIL; break;
        }
        return true;
    }

    /**
     * @brief Pausa hasta que el usuario presione ENTER
     */
    static void pausar() {
        std::cout << Config::CYAN << "\nPresiona ENTER para continuar..." << Config::RESET;
        std::string dummy;
        if (!std::getline(std::cin, dummy)) {
            // Fin de entrada estándar: no continuar esperando
            std::cin.clear();
        }
    }

    /**
     * @brief Limpia el buffer de entrada
     */
    static void limpiarBuffer() {
        std::cin.clear();
        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    }

    /**
     * @brief Verifica si la entrada estándar está en buen estado
     */
    static bool entradaValida() {
        return std::cin.good();
    }
};
} // namespace SopaDeLetras

#endif // VALIDADOR_ENTRADA_H