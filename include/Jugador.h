#ifndef JUGADOR_H
#define JUGADOR_H

#include <string>
#include <chrono>
#include <vector>
#include "Config.h"

namespace SopaDeLetras {

/**
 * @brief Clase que representa a un jugador y sus estadísticas
 * Maneja puntuación, errores, tiempo y récords
 */
class Jugador {
private:
    std::string nombre_;
    int puntos_ = 0;
    int errores_ = 0;
    int nivel_actual_ = 1;
    int racha_actual_ = 0;
    int mejor_racha_ = 0;

    std::chrono::steady_clock::time_point inicio_partida_;
    std::chrono::steady_clock::time_point inicio_turno_actual_;

    long long tiempo_total_segundos_ = 0;
    long long mejor_tiempo_segundos_ = -1;

    // Historial de partidas
    struct PartidaRecord {
        std::string fecha;
        int puntos;
        int errores;
        int nivel;
        long long tiempo_segundos;
        bool victoria;

        PartidaRecord() = default;
        PartidaRecord(const std::string& f, int p, int e, int n, long long t, bool v)
            : fecha(f), puntos(p), errores(e), nivel(n), tiempo_segundos(t), victoria(v) {}
    };
    std::vector<PartidaRecord> historial_;

public:
    Jugador() = default;
    explicit Jugador(const std::string& nombre) : nombre_(nombre) {}

    // Getters
    const std::string& obtenerNombre() const { return nombre_; }
    int obtenerPuntos() const { return puntos_; }
    int obtenerErrores() const { return errores_; }
    int obtenerNivel() const { return nivel_actual_; }
    int obtenerRachaActual() const { return racha_actual_; }
    int obtenerMejorRacha() const { return mejor_racha_; }
    long long obtenerTiempoTotal() const { return tiempo_total_segundos_; }
    long long obtenerMejorTiempo() const { return mejor_tiempo_segundos_; }
    const std::vector<PartidaRecord>& obtenerHistorial() const { return historial_; }

    // Setters
    void establecerNombre(const std::string& nombre) {
        if (!nombre.empty() && nombre.length() <= Config::MAX_NOMBRE_JUGADOR) {
            nombre_ = nombre;
        }
    }

    /**
     * @brief Inicia una nueva partida
     */
    void iniciarPartida(int nivel = 1) {
        nivel_actual_ = nivel;
        puntos_ = 0;
        errores_ = 0;
        racha_actual_ = 0;
        inicio_partida_ = std::chrono::steady_clock::now();
        inicio_turno_actual_ = inicio_partida_;
    }

    /**
     * @brief Registra una palabra encontrada correctamente
     */
    void registrarAcierto() {
        ++puntos_;
        ++racha_actual_;
        if (racha_actual_ > mejor_racha_) {
            mejor_racha_ = racha_actual_;
        }
    }

    /**
     * @brief Registra un error (pista usada o intento fallido)
     */
    void registrarError() {
        ++errores_;
        racha_actual_ = 0; // Rompe la racha
    }

    /**
     * @brief Registra el uso de una pista (penalización menor)
     */
    void registrarPista() {
        ++errores_;
        racha_actual_ = 0;
    }

    /**
     * @brief Registra mostrar la solución (penalización mayor)
     */
    void registrarSolucion() {
        errores_ += 3;
        racha_actual_ = 0;
    }

    /**
     * @brief Finaliza la partida y actualiza estadísticas
     * @param victoria true si ganó la partida
     * @return Tiempo total en segundos
     */
    long long finalizarPartida(bool victoria) {
        auto fin = std::chrono::steady_clock::now();
        long long duracion = std::chrono::duration_cast<std::chrono::seconds>(fin - inicio_partida_).count();
        tiempo_total_segundos_ += duracion;

        // Guardar en historial
        auto ahora = std::chrono::system_clock::now();
        auto tiempo_t = std::chrono::system_clock::to_time_t(ahora);
        char buffer[80];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", std::localtime(&tiempo_t));
        std::string fecha(buffer);

        historial_.emplace_back(fecha, puntos_, errores_, nivel_actual_, duracion, victoria);

        // Mantener solo los últimos registros
        if (historial_.size() > Config::MAX_PUNTOS_HISTORIAL) {
            historial_.erase(historial_.begin());
        }

        // Actualizar mejor tiempo si ganó
        if (victoria && (mejor_tiempo_segundos_ == -1 || duracion < mejor_tiempo_segundos_)) {
            mejor_tiempo_segundos_ = duracion;
        }

        return duracion;
    }

    /**
     * @brief Formatea tiempo en minutos y segundos
     */
    static std::string formatearTiempo(long long segundos) {
        int minutos = static_cast<int>(segundos / 60);
        int segs = static_cast<int>(segundos % 60);
        return std::to_string(minutos) + "m " + std::to_string(segs) + "s";
    }

    /**
     * @brief Calcula la puntuación final con bonificaciones
     */
    int calcularPuntuacionFinal(int total_palabras) const {
        int base = puntos_ * 100;
        int bonificacion_racha = mejor_racha_ * 50;
        int penalizacion_errores = errores_ * 20;
        int bonificacion_velocidad = 0;

        if (mejor_tiempo_segundos_ > 0) {
            // Bonificación por velocidad (máximo 500 puntos)
            bonificacion_velocidad = (std::max)(0, 500 - static_cast<int>(mejor_tiempo_segundos_ / 10));
        }

        return base + bonificacion_racha - penalizacion_errores + bonificacion_velocidad;
    }

    /**
     * @brief Obtiene estadísticas resumidas para mostrar
     */
    struct EstadisticasResumen {
        std::string nombre;
        int partidas_jugadas;
        int victorias;
        int mejor_puntuacion;
        long long mejor_tiempo;
        int mejor_racha;
        double porcentaje_victorias;
    };

    EstadisticasResumen obtenerResumen() const {
        EstadisticasResumen res;
        res.nombre = nombre_;
        res.partidas_jugadas = static_cast<int>(historial_.size());
        res.victorias = 0;
        res.mejor_puntuacion = 0;
        res.mejor_tiempo = mejor_tiempo_segundos_;
        res.mejor_racha = mejor_racha_;

        for (const auto& p : historial_) {
            if (p.victoria) ++res.victorias;
            int punt = p.puntos * 100 - p.errores * 20;
            if (punt > res.mejor_puntuacion) res.mejor_puntuacion = punt;
        }

        res.porcentaje_victorias = res.partidas_jugadas > 0
            ? (static_cast<double>(res.victorias) / res.partidas_jugadas * 100.0)
            : 0.0;

        return res;
    }

    /**
     * @brief Reinicia todas las estadísticas (nuevo jugador)
     */
    void reiniciarEstadisticas() {
        puntos_ = 0;
        errores_ = 0;
        nivel_actual_ = 1;
        racha_actual_ = 0;
        mejor_racha_ = 0;
        tiempo_total_segundos_ = 0;
        mejor_tiempo_segundos_ = -1;
        historial_.clear();
    }
};

} // namespace SopaDeLetras

#endif // JUGADOR_H