/**
 * @file main.cpp
 * @brief Punto de entrada principal del juego Sopa de Letras
 *
 * Juego de sopa de letras profesional con:
 * - Múltiples niveles de dificultad (Fácil, Medio, Difícil)
 * - Sistema de puntuación y récords persistentes
 * - Validación robusta de entradas
 * - Interfaz de consola con colores
 *
 * @author Portfolio Project
 * @date 2026
 * @version 1.0
 */

#include <iostream>
#include "Juego.h"

int main() {
    try {
        SopaDeLetras::Juego juego;
        juego.ejecutar();
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error fatal: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "Error desconocido" << std::endl;
        return 1;
    }
}