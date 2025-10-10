#ifndef RESULTADO2D_H
#define RESULTADO2D_H

#include <vector>
#include <string>
#include "Grid2D.h"

struct Resultado2D {
    std::vector<std::pair<int, int>> camino;
    std::vector<std::pair<int, int>> nodosVisitados;
    std::vector<std::pair<int, int>> frontera;
    double costo;
    long long tiempoMicrosegundos;
    int nodosExpandidos;
    bool exito;
    std::string algoritmo;
    
    Resultado2D() : costo(0), tiempoMicrosegundos(0), nodosExpandidos(0), exito(false), algoritmo("") {}
};

// Callback function type for step-by-step visualization
using CallbackVisualizacion = std::function<void(const Grid2D&, const std::vector<std::pair<int, int>>&, const std::vector<std::pair<int, int>>&)>;

#endif // RESULTADO2D_H
