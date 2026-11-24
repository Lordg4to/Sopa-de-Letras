#ifndef JUEGO_H
#define JUEGO_H

#include <memory>
#include <thread>
#include <chrono>
#ifdef _WIN32
#include <windows.h>
#endif
#include "Config.h"
#include "Tablero.h"
#include "Jugador.h"
#include "Palabra.h"
#include "GestorPuntuaciones.h"
#include "ValidadorEntrada.h"

namespace SopaDeLetras {

/**
 * @brief Clase principal del juego - Orquesta todos los componentes
 * Implementa el bucle principal, menús y lógica de alto nivel
 */
class Juego {
private:
    std::unique_ptr<Tablero> tablero_;
    std::unique_ptr<Jugador> jugador_;
    std::unique_ptr<GestorPalabras> gestor_palabras_;
    std::unique_ptr<GestorPuntuaciones> gestor_puntuaciones_;
    SopaDeLetras::ConfiguracionPartida config_;
    bool jugando_ = false;

    /**
     * @brief Limpia la pantalla de consola
     */
    void limpiarPantalla() const {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
        std::cout << SopaDeLetras::Config::CLEAR_SCREEN;
    }

    /**
     * @brief Pequeña pausa en milisegundos
     */
    void sleepMs(int ms) const {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

    /**
     * @brief Muestra el encabezado con información del jugador
     */
    void mostrarEncabezado() const {
        using namespace SopaDeLetras::Config;
        std::cout << BOLD << YELLOW << "SOPA DE LETRAS " << tablero_->obtenerTamano() << "x" << tablero_->obtenerTamano() << "\n" << RESET;
        std::cout << std::string(35, '-') << "\n\n";

        if (jugador_) {
            std::cout << GREEN << "Jugador: " << BOLD << jugador_->obtenerNombre() << RESET
                      << "  |  " << YELLOW << "Puntos: " << BOLD << jugador_->obtenerPuntos() << "/" << config_.num_palabras << RESET
                      << "  |  " << RED << "Errores: " << BOLD << jugador_->obtenerErrores() << RESET
                      << "  |  " << CYAN << "Racha: " << BOLD << jugador_->obtenerRachaActual() << RESET << "\n\n";
        }
    }

    /**
     * @brief Muestra el tablero con palabras encontradas resaltadas
     */
    void mostrarTablero() const {
        using namespace SopaDeLetras::Config;
        int N = tablero_->obtenerTamano();
        const auto& grid = tablero_->obtenerGrid();
        const auto& palabras = tablero_->obtenerPalabras();

        // Encabezado de columnas
        std::cout << "     ";
        for (int j = 0; j < N; ++j) {
            std::cout << BLUE << std::setw(2) << j << " " << RESET;
        }
        std::cout << "\n";

        // Filas del tablero
        for (int i = 0; i < N; ++i) {
            std::cout << BLUE << std::setw(3) << i << " " << RESET;

            for (int j = 0; j < N; ++j) {
                char ch = grid[i][j];
                bool resaltada = false;

                // Verificar si esta celda pertenece a una palabra encontrada
                for (const auto& p : palabras) {
                    if (p.contieneCoordenada(i, j)) {
                        resaltada = true;
                        break;
                    }
                }

                if (resaltada) {
                    std::cout << BG_GREEN << BOLD << " " << ch << " " << RESET;
                } else {
                    std::cout << " " << ch << " ";
                }
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    /**
     * @brief Muestra la leyenda
     */
    void mostrarLeyenda() const {
        using namespace SopaDeLetras::Config;
        std::cout << BOLD << "Leyenda:" << RESET << "  " << BG_GREEN << BOLD << " VERDE " << RESET << "=Encontrada  "
                  << BLUE << "Azul" << RESET << "=Coordenadas\n\n";
    }

    /**
     * @brief Muestra las palabras pendientes/encontradas
     */
    void mostrarPalabras() const {
        using namespace SopaDeLetras::Config;
        const auto& palabras = tablero_->obtenerPalabras();

        std::cout << BOLD << MAGENTA << "Palabras:\n" << RESET;

        for (size_t i = 0; i < palabras.size(); ++i) {
            if (palabras[i].estaEncontrada()) {
                std::cout << GREEN << "[X] " << palabras[i].obtenerTexto() << RESET << "  ";
            } else {
                std::cout << YELLOW << "[ ] " << palabras[i].obtenerTexto() << RESET << "  ";
            }

            // Salto de línea cada 3 palabras para mejor visualización
            if ((i + 1) % 3 == 0) std::cout << "\n";
        }
        std::cout << "\n\n";
    }

    /**
     * @brief Muestra el menú principal
     */
    void mostrarMenu() const {
        using namespace SopaDeLetras::Config;
        std::cout << BOLD << BLUE << "MENU:\n" << RESET;
        std::cout << GREEN << "1." << RESET << " Buscar palabra\n";
        std::cout << GREEN << "2." << RESET << " Ver pista" << (config_.pistas_habilitadas ? "" : " (deshabilitada)") << "\n";
        std::cout << GREEN << "3." << RESET << " Mostrar solucion" << (config_.mostrar_solucion ? "" : " (deshabilitada)") << "\n";
        std::cout << GREEN << "4." << RESET << " Reiniciar partida\n";
        std::cout << GREEN << "5." << RESET << " Ver records\n";
        std::cout << GREEN << "6." << RESET << " Cambiar dificultad\n";
        std::cout << GREEN << "7." << RESET << " Salir\n\n";
    }

    /**
     * @brief Inicializa una nueva partida
     */
    void nuevaPartida() {
        // Obtener palabras según dificultad
        std::vector<std::string> palabras = gestor_palabras_->obtenerPalabras(config_.dificultad, config_.num_palabras);

        // Generar tablero
        bool exito = tablero_->generar(palabras);

        if (!exito) {
            std::cout << SopaDeLetras::Config::RED << "[!] Advertencia: No se pudieron colocar todas las palabras." << SopaDeLetras::Config::RESET << "\n";
        }

        // Iniciar estadísticas del jugador
        jugador_->iniciarPartida(static_cast<int>(config_.dificultad));
        jugando_ = true;
    }

    /**
     * @brief Maneja la opción de buscar palabra
     */
    void buscarPalabra() {
        using namespace SopaDeLetras::Config;
        int N = tablero_->obtenerTamano();

        std::cout << CYAN << "\nCOORDENADAS DE INICIO (0-" << N-1 << "):\n" << RESET;
        int f1, c1, f2, c2;

        if (!ValidadorEntrada::leerCoordenada("  Fila inicial: ", N, f1)) return;
        if (!ValidadorEntrada::leerCoordenada("  Columna inicial: ", N, c1)) return;

        std::cout << CYAN << "\nCOORDENADAS DE FIN (0-" << N-1 << "):\n" << RESET;
        if (!ValidadorEntrada::leerCoordenada("  Fila final: ", N, f2)) return;
        if (!ValidadorEntrada::leerCoordenada("  Columna final: ", N, c2)) return;

        // Verificar selección
        int resultado = tablero_->verificarSeleccion(f1, c1, f2, c2);

        if (resultado == -2) {
            std::cout << RED << "\n[X] Direccion invalida. Solo horizontal, vertical o diagonal." << RESET << "\n";
            jugador_->registrarError();
        } else if (resultado >= 0) {
            jugador_->registrarAcierto();
            const auto& palabras = tablero_->obtenerPalabras();
            std::cout << GREEN << BOLD << "\n[OK] CORRECTO! Encontraste: " << palabras[resultado].obtenerTexto() << " (+1 punto)" << RESET << "\n";
        } else {
            std::cout << RED << BOLD << "\n[X] INCORRECTO. Esa no es una palabra valida." << RESET << "\n";
            jugador_->registrarError();
        }

        ValidadorEntrada::pausar();
    }

    /**
     * @brief Maneja la opción de ver pista
     */
    void verPista() {
        using namespace SopaDeLetras::Config;
        if (!config_.pistas_habilitadas) {
            std::cout << RED << "\n[X] Las pistas estan deshabilitadas en esta dificultad." << RESET << "\n";
            ValidadorEntrada::pausar();
            return;
        }

        int idx = tablero_->obtenerPalabraParaPista();
        if (idx == -1) {
            std::cout << GREEN << "\nYa encontraste todas las palabras!" << RESET << "\n";
        } else {
            const auto& palabras = tablero_->obtenerPalabras();
            std::cout << YELLOW << "\nPISTA: " << RESET << palabras[idx].obtenerPista() << "\n";
            jugador_->registrarPista();
        }
        ValidadorEntrada::pausar();
    }

    /**
     * @brief Maneja la opción de mostrar solución
     */
    void mostrarSolucion() {
        using namespace SopaDeLetras::Config;
        if (!config_.mostrar_solucion) {
            std::cout << RED << "\n[X] La solucion esta deshabilitada en esta dificultad." << RESET << "\n";
            ValidadorEntrada::pausar();
            return;
        }

        const auto& palabras = tablero_->obtenerPalabras();
        std::cout << RED << BOLD << "\n[!] SOLUCION COMPLETA\n" << RESET;

        for (const auto& p : palabras) {
            if (!p.estaEncontrada()) {
                std::cout << YELLOW << p.obtenerSolucion() << RESET << "\n";
            }
        }
        std::cout << "\n";
        jugador_->registrarSolucion();
        ValidadorEntrada::pausar();
    }

    /**
     * @brief Reinicia la partida actual
     */
    void reiniciarPartida() {
        if (ValidadorEntrada::leerConfirmacion("\nSeguro que quieres reiniciar la partida?")) {
            nuevaPartida();
            std::cout << SopaDeLetras::Config::GREEN << "\nPartida reiniciada!" << SopaDeLetras::Config::RESET << "\n";
            sleepMs(1000);
        }
    }

    /**
     * @brief Muestra los records
     */
    void verRecords() {
        gestor_puntuaciones_->mostrarRecords(15);
        ValidadorEntrada::pausar();
    }

    /**
     * @brief Cambia la dificultad
     */
    void cambiarDificultad() {
        SopaDeLetras::Dificultad nueva_dificultad;
        if (ValidadorEntrada::leerDificultad(nueva_dificultad)) {
            config_.dificultad = nueva_dificultad;
            switch (nueva_dificultad) {
                case SopaDeLetras::Dificultad::FACIL:
                    config_.tamano_tablero = 12;
                    config_.num_palabras = SopaDeLetras::Config::PALABRAS_FACIL;
                    config_.pistas_habilitadas = true;
                    config_.mostrar_solucion = true;
                    break;
                case SopaDeLetras::Dificultad::MEDIO:
                    config_.tamano_tablero = 15;
                    config_.num_palabras = SopaDeLetras::Config::PALABRAS_MEDIO;
                    config_.pistas_habilitadas = true;
                    config_.mostrar_solucion = false;
                    break;
                case SopaDeLetras::Dificultad::DIFICIL:
                    config_.tamano_tablero = 20;
                    config_.num_palabras = SopaDeLetras::Config::PALABRAS_DIFICIL;
                    config_.pistas_habilitadas = false;
                    config_.mostrar_solucion = false;
                    break;
            }
            tablero_->establecerTamano(config_.tamano_tablero);
            nuevaPartida();
            std::cout << SopaDeLetras::Config::GREEN << "\nDificultad cambiada y nueva partida iniciada!" << SopaDeLetras::Config::RESET << "\n";
            sleepMs(1000);
        }
    }

    /**
     * @brief Verifica si el juego terminó (todas las palabras encontradas)
     */
    bool verificarFinJuego() {
        if (tablero_->todasEncontradas()) {
            limpiarPantalla();
            mostrarEncabezado();
            mostrarTablero();

            long long tiempo = jugador_->finalizarPartida(true);
            int puntuacion = jugador_->calcularPuntuacionFinal(config_.num_palabras);

            using namespace SopaDeLetras::Config;
            std::cout << BOLD << YELLOW << "\n=== JUEGO TERMINADO ===" << RESET << "\n";
            std::cout << std::string(25, '-') << "\n";
            std::cout << "Jugador: " << BOLD << jugador_->obtenerNombre() << RESET << "\n";
            std::cout << "Palabras encontradas: " << BOLD << config_.num_palabras << "/" << config_.num_palabras << RESET << "\n";
            std::cout << "Puntuacion final: " << BOLD << puntuacion << RESET << " puntos\n";
            std::cout << "Errores/Pistas: " << RED << jugador_->obtenerErrores() << RESET << "\n";
            std::cout << "Mejor racha: " << CYAN << jugador_->obtenerMejorRacha() << RESET << "\n";
            std::cout << "Tiempo: " << BLUE << Jugador::formatearTiempo(tiempo) << RESET << "\n";

            // Guardar en records
            int posicion = gestor_puntuaciones_->agregarDesdeJugador(*jugador_, config_.num_palabras, true);
            if (posicion > 0) {
                std::cout << GREEN << BOLD << "\nNUEVO RECORD! Posicion #" << posicion << " en la tabla!" << RESET << "\n";
            }

            std::cout << GREEN << BOLD << "\nFELICIDADES! GANASTE!\n" << RESET;
            ValidadorEntrada::pausar();
            return true;
        }
        return false;
    }

    /**
     * @brief Muestra la pantalla de bienvenida
     */
    void mostrarBienvenida() {
        using namespace SopaDeLetras::Config;
        limpiarPantalla();
        std::cout << BOLD << YELLOW << "=====================================\n";
        std::cout << "       SOPA DE LETRAS PRO\n";
        std::cout << "=====================================\n" << RESET;
        std::cout << "Encuentra las palabras ocultas en el tablero.\n";
        std::cout << "Palabras: horizontal, vertical o diagonal.\n";
        std::cout << "Ingresa coordenadas de inicio y fin (0-" << SopaDeLetras::Config::TABLERO_MAX-1 << ").\n\n";

        // Preguntar si quiere ver records anteriores
        if (ValidadorEntrada::leerConfirmacion("Ver tabla de records?")) {
            gestor_puntuaciones_->mostrarRecords(10);
        }
    }

public:
    Juego() {
        // Inicializar componentes
        gestor_palabras_ = std::make_unique<GestorPalabras>();
        gestor_puntuaciones_ = std::make_unique<GestorPuntuaciones>();
        config_ = SopaDeLetras::ConfiguracionPartida(SopaDeLetras::Dificultad::MEDIO);
        tablero_ = std::make_unique<Tablero>(config_.tamano_tablero);
        jugador_ = std::make_unique<Jugador>("Jugador");
    }

    /**
     * @brief Ejecuta el bucle principal del juego
     */
    void ejecutar() {
        // Configurar consola para UTF-8 en Windows
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif

        // Semilla aleatoria
        std::srand(static_cast<unsigned>(std::time(nullptr)));

        // Pantalla de bienvenida
        mostrarBienvenida();

        // Obtener nombre del jugador
        std::string nombre;
        if (ValidadorEntrada::leerNombreJugador(nombre)) {
            jugador_->establecerNombre(nombre);
        }

        // Seleccionar dificultad inicial
        ValidadorEntrada::leerDificultad(config_.dificultad);
        switch (config_.dificultad) {
            case SopaDeLetras::Dificultad::FACIL:
                config_.tamano_tablero = 12;
                config_.num_palabras = SopaDeLetras::Config::PALABRAS_FACIL;
                config_.pistas_habilitadas = true;
                config_.mostrar_solucion = true;
                break;
            case SopaDeLetras::Dificultad::MEDIO:
                config_.tamano_tablero = 15;
                config_.num_palabras = SopaDeLetras::Config::PALABRAS_MEDIO;
                config_.pistas_habilitadas = true;
                config_.mostrar_solucion = false;
                break;
            case SopaDeLetras::Dificultad::DIFICIL:
                config_.tamano_tablero = 20;
                config_.num_palabras = SopaDeLetras::Config::PALABRAS_DIFICIL;
                config_.pistas_habilitadas = false;
                config_.mostrar_solucion = false;
                break;
        }
        tablero_->establecerTamano(config_.tamano_tablero);

        // Iniciar primera partida
        nuevaPartida();

        // Bucle principal
        while (jugando_) {
            limpiarPantalla();
            mostrarEncabezado();
            mostrarTablero();
            mostrarLeyenda();
            mostrarPalabras();
            mostrarMenu();

            int opcion;
            if (!ValidadorEntrada::leerOpcionMenu(1, 7, opcion)) {
                continue;
            }

            switch (opcion) {
                case 1: buscarPalabra(); break;
                case 2: verPista(); break;
                case 3: mostrarSolucion(); break;
                case 4: reiniciarPartida(); break;
                case 5: verRecords(); break;
                case 6: cambiarDificultad(); break;
                case 7:
                    jugando_ = false;
                    break;
            }

            // Verificar fin de juego después de cada acción
            if (jugando_) {
                verificarFinJuego();
            }
        }

        // Despedida
        if (jugador_->obtenerPuntos() > 0) {
            jugador_->finalizarPartida(false);
            gestor_puntuaciones_->agregarDesdeJugador(*jugador_, jugador_->obtenerPuntos(), false);
        }

        limpiarPantalla();
        using namespace SopaDeLetras::Config;
        std::cout << YELLOW << "\nGracias por jugar, " << jugador_->obtenerNombre() << "!\n" << RESET;
        std::cout << "Tu puntuacion final: " << BOLD << jugador_->calcularPuntuacionFinal(config_.num_palabras) << RESET << " puntos\n\n";
    }
};

} // namespace SopaDeLetras

#endif // JUEGO_H