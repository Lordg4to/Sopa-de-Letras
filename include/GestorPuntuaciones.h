#ifndef GESTOR_PUNTUACIONES_H
#define GESTOR_PUNTUACIONES_H

#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include "Config.h"
#include "Jugador.h"

namespace SopaDeLetras {

/**
 * @brief Estructura para una entrada en la tabla de récords
 */
struct RecordEntry {
    std::string nombre;
    int puntuacion;
    int nivel;
    int palabras_encontradas;
    int errores;
    long long tiempo_segundos;
    std::string fecha;
    bool victoria;

    RecordEntry() = default;
    RecordEntry(const std::string& n, int p, int niv, int pe, int e, long long t, bool v)
        : nombre(n), puntuacion(p), nivel(niv), palabras_encontradas(pe),
          errores(e), tiempo_segundos(t), victoria(v) {
        auto ahora = std::chrono::system_clock::now();
        auto tiempo_t = std::chrono::system_clock::to_time_t(ahora);
        char buffer[80];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", std::localtime(&tiempo_t));
        fecha = buffer;
    }

    // Para ordenar: mayor puntuación primero, luego menor tiempo
    bool operator<(const RecordEntry& other) const {
        if (puntuacion != other.puntuacion) {
            return puntuacion > other.puntuacion; // Mayor puntuación primero
        }
        return tiempo_segundos < other.tiempo_segundos; // Menor tiempo primero
    }
};

/**
 * @brief Clase para gestionar la persistencia de puntuaciones y récords
 * Guarda y carga desde archivo JSON simple
 */
class GestorPuntuaciones {
private:
    std::string archivo_puntuaciones_;
    std::vector<RecordEntry> records_;
    const int MAX_RECORDS = 50;

    /**
     * @brief Serializa un RecordEntry a formato JSON simple
     */
    std::string serializar(const RecordEntry& r) const {
        std::ostringstream oss;
        oss << "  {\n"
            << "    \"nombre\": \"" << escaparJson(r.nombre) << "\",\n"
            << "    \"puntuacion\": " << r.puntuacion << ",\n"
            << "    \"nivel\": " << r.nivel << ",\n"
            << "    \"palabras_encontradas\": " << r.palabras_encontradas << ",\n"
            << "    \"errores\": " << r.errores << ",\n"
            << "    \"tiempo_segundos\": " << r.tiempo_segundos << ",\n"
            << "    \"fecha\": \"" << r.fecha << "\",\n"
            << "    \"victoria\": " << (r.victoria ? "true" : "false") << "\n"
            << "  }";
        return oss.str();
    }

    /**
     * @brief Escapa caracteres especiales para JSON
     */
    std::string escaparJson(const std::string& input) const {
        std::string output;
        output.reserve(input.length());
        for (char c : input) {
            switch (c) {
                case '"': output += "\\\""; break;
                case '\\': output += "\\\\"; break;
                case '\b': output += "\\b"; break;
                case '\f': output += "\\f"; break;
                case '\n': output += "\\n"; break;
                case '\r': output += "\\r"; break;
                case '\t': output += "\\t"; break;
                default:
                    if (c >= 0 && c < 32) {
                        char buf[7];
                        std::snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned char>(c));
                        output += buf;
                    } else {
                        output += c;
                    }
            }
        }
        return output;
    }

    /**
     * @brief Parsea una línea JSON simple para extraer un valor
     */
    std::string extraerValorJson(const std::string& linea, const std::string& clave) const {
        std::string buscar = "\"" + clave + "\"";
        size_t pos = linea.find(buscar);
        if (pos == std::string::npos) return "";

        pos = linea.find(':', pos);
        if (pos == std::string::npos) return "";

        pos = linea.find_first_not_of(" \t", pos + 1);
        if (pos == std::string::npos) return "";

        if (linea[pos] == '"') {
            // Valor string
            size_t inicio = pos + 1;
            size_t fin = linea.find('"', inicio);
            if (fin == std::string::npos) return "";
            return linea.substr(inicio, fin - inicio);
        } else {
            // Valor numérico o booleano
            size_t fin = linea.find_first_of(",}", pos);
            if (fin == std::string::npos) fin = linea.length();
            return linea.substr(pos, fin - pos);
        }
    }

public:
    /**
     * @brief Constructor
     * @param archivo Ruta al archivo de puntuaciones
     */
    explicit GestorPuntuaciones(const std::string& archivo = SopaDeLetras::Config::ARCHIVO_PUNTUACIONES)
        : archivo_puntuaciones_(archivo) {
        cargar();
    }

    /**
     * @brief Carga las puntuaciones desde el archivo
     */
    bool cargar() {
        records_.clear();

        std::ifstream archivo(archivo_puntuaciones_);
        if (!archivo.is_open()) {
            return false; // Archivo no existe, empezar vacío
        }

        std::string linea;
        std::string contenido;
        while (std::getline(archivo, linea)) {
            contenido += linea + "\n";
        }

        // Parser simple para nuestro formato JSON
        size_t pos = 0;
        while (true) {
            pos = contenido.find("{", pos);
            if (pos == std::string::npos) break;

            size_t fin = contenido.find("}", pos);
            if (fin == std::string::npos) break;

            std::string entry_str = contenido.substr(pos, fin - pos + 1);
            RecordEntry entry;

            entry.nombre = extraerValorJson(entry_str, "nombre");
            entry.puntuacion = std::stoi(extraerValorJson(entry_str, "puntuacion"));
            entry.nivel = std::stoi(extraerValorJson(entry_str, "nivel"));
            entry.palabras_encontradas = std::stoi(extraerValorJson(entry_str, "palabras_encontradas"));
            entry.errores = std::stoi(extraerValorJson(entry_str, "errores"));
            entry.tiempo_segundos = std::stoll(extraerValorJson(entry_str, "tiempo_segundos"));
            entry.fecha = extraerValorJson(entry_str, "fecha");
            entry.victoria = (extraerValorJson(entry_str, "victoria") == "true");

            if (!entry.nombre.empty()) {
                records_.push_back(entry);
            }

            pos = fin + 1;
        }

        // Ordenar y limitar
        std::sort(records_.begin(), records_.end());
        if (records_.size() > MAX_RECORDS) {
            records_.resize(MAX_RECORDS);
        }

        return true;
    }

    /**
     * @brief Guarda las puntuaciones en el archivo
     */
    bool guardar() const {
        // Crear directorio si no existe (C++17 filesystem)
        std::filesystem::path path(archivo_puntuaciones_);
        if (path.has_parent_path()) {
            std::filesystem::create_directories(path.parent_path());
        }

        std::ofstream archivo(archivo_puntuaciones_);
        if (!archivo.is_open()) {
            return false;
        }

        archivo << "[\n";
        for (size_t i = 0; i < records_.size(); ++i) {
            archivo << serializar(records_[i]);
            if (i < records_.size() - 1) archivo << ",";
            archivo << "\n";
        }
        archivo << "]\n";

        return true;
    }

    /**
     * @brief Agrega una nueva puntuación
     * @return Posición en el ranking (1-based), 0 si no entró en el top
     */
    int agregarPuntuacion(const RecordEntry& entry) {
        records_.push_back(entry);
        std::sort(records_.begin(), records_.end());

        // Encontrar posición de la nueva entrada
        int posicion = 0;
        for (size_t i = 0; i < records_.size(); ++i) {
            if (records_[i].nombre == entry.nombre &&
                records_[i].puntuacion == entry.puntuacion &&
                records_[i].fecha == entry.fecha) {
                posicion = static_cast<int>(i) + 1;
                break;
            }
        }

        // Mantener solo los mejores
        if (records_.size() > MAX_RECORDS) {
            records_.resize(MAX_RECORDS);
        }

        guardar();
        return posicion;
    }

    /**
     * @brief Agrega puntuación desde un jugador
     */
    int agregarDesdeJugador(const Jugador& jugador, int palabras_encontradas, bool victoria) {
        int puntuacion = jugador.calcularPuntuacionFinal(palabras_encontradas);
        RecordEntry entry(jugador.obtenerNombre(), puntuacion, jugador.obtenerNivel(),
                         palabras_encontradas, jugador.obtenerErrores(),
                         jugador.obtenerMejorTiempo(), victoria);
        return agregarPuntuacion(entry);
    }

    /**
     * @brief Obtiene los mejores récords
     */
    const std::vector<RecordEntry>& obtenerRecords(int cantidad = 10) const {
        return records_;
    }

    /**
     * @brief Obtiene los mejores récords por nivel
     */
    std::vector<RecordEntry> obtenerRecordsPorNivel(int nivel, int cantidad = 5) const {
        std::vector<RecordEntry> filtrados;
        for (const auto& r : records_) {
            if (r.nivel == nivel) {
                filtrados.push_back(r);
                if (static_cast<int>(filtrados.size()) >= cantidad) break;
            }
        }
        return filtrados;
    }

    /**
     * @brief Obtiene los récords de un jugador específico
     */
    std::vector<RecordEntry> obtenerRecordsJugador(const std::string& nombre) const {
        std::vector<RecordEntry> filtrados;
        for (const auto& r : records_) {
            if (r.nombre == nombre) {
                filtrados.push_back(r);
            }
        }
        return filtrados;
    }

    /**
     * @brief Muestra la tabla de récords formateada
     */
    void mostrarRecords(int cantidad = 10) const {
        using namespace SopaDeLetras::Config;
        std::cout << BOLD << YELLOW << "\n=== TABLA DE RECORDS ===" << RESET << "\n";
        std::cout << std::left << std::setw(4) << "#"
                  << std::setw(20) << "JUGADOR"
                  << std::setw(10) << "PUNTOS"
                  << std::setw(6) << "NIVEL"
                  << std::setw(8) << "PALAB."
                  << std::setw(8) << "ERRORES"
                  << std::setw(10) << "TIEMPO"
                  << "FECHA" << "\n";
        std::cout << std::string(75, '-') << "\n";

        int count = 0;
        for (const auto& r : records_) {
            if (count >= cantidad) break;
            std::cout << std::left << std::setw(4) << (count + 1)
                      << std::setw(20) << r.nombre.substr(0, 19)
                      << std::setw(10) << r.puntuacion
                      << std::setw(6) << r.nivel
                      << std::setw(8) << r.palabras_encontradas
                      << std::setw(8) << r.errores
                      << std::setw(10) << Jugador::formatearTiempo(r.tiempo_segundos)
                      << r.fecha << "\n";
            ++count;
        }

        if (records_.empty()) {
            std::cout << CYAN << "No hay récords aún. ¡Sé el primero!" << RESET << "\n";
        }
        std::cout << "\n";
    }

    /**
     * @brief Limpia todos los récords (para testing)
     */
    void limpiarRecords() {
        records_.clear();
        guardar();
    }
};

} // namespace SopaDeLetras

#endif // GESTOR_PUNTUACIONES_H