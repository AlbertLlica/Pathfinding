#ifndef RESULTADO_BUSQUEDA_H
#define RESULTADO_BUSQUEDA_H

#include <vector>

struct ResultadoBusqueda {
    std::vector<int> camino;
    double costo;
    long long tiempoMicrosegundos;
    int nodosExpandidos;
};

#endif // RESULTADO_BUSQUEDA_H