#ifndef UTILIDADES_H
#define UTILIDADES_H

#include "ResultadoBusqueda.h"
#include <string>

class Utilidades {
public:
    static void imprimirResultado(const std::string& nombre, const ResultadoBusqueda& resultado);
    static void imprimirResumenComparativo(
        const ResultadoBusqueda& r1,
        const ResultadoBusqueda& r2,
        const ResultadoBusqueda& r3,
        const ResultadoBusqueda& r4
    );
};

#endif