# CMakeLists.txt para Sopa de Letras
# Compatible con Visual Studio 2019/2022 y CMake 3.15+

cmake_minimum_required(VERSION 3.15)

project(SopaDeLetras
    VERSION 1.0.0
    LANGUAGES CXX
    DESCRIPTION "Juego profesional de Sopa de Letras para portafolio de Ingeniería de Software"
)

# Configuración de C++ estándar
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configuración de compilación
if(MSVC)
    # Configuración para Visual Studio
    add_compile_options(/W4 /EHsc /permissive-)
    # Suprimir warnings específicos de MSVC que son ruido
    add_compile_options(/wd4244 /wd4267 /wd4100 /wd4101)
else()
    # Configuración para GCC/Clang
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Configuración de Release/Debug
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd" )
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD" )

# Directorio de includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Archivos fuente
set(SOURCES
    src/main.cpp
)

# Crear ejecutable
add_executable(SopaDeLetras ${SOURCES})

# Propiedades del target
set_target_properties(SopaDeLetras PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    OUTPUT_NAME "SopaDeLetras"
    DEBUG_POSTFIX "_d"
)

# Instalación (opcional)
install(TARGETS SopaDeLetras
    RUNTIME DESTINATION bin
)

# Habilitar testing (para futuro)
enable_testing()

# Paquetes CPack para distribución (opcional)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)