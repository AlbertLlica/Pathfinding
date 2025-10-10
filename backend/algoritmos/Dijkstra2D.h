#ifndef DIJKSTRA2D_H
#define DIJKSTRA2D_H

#include "Grid2D.h"
#include "Resultado2D.h"
#include <functional>

class Dijkstra2D {
public:
    static Resultado2D buscar(Grid2D& grid, bool allowDiagonal = false, CallbackVisualizacion callback = nullptr);
};

#endif // DIJKSTRA2D_H
