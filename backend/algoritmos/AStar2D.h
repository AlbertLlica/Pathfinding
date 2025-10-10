#ifndef ASTAR2D_H
#define ASTAR2D_H

#include "Grid2D.h"
#include "Resultado2D.h"
#include <functional>

class AStar2D {
public:
    static Resultado2D buscar(Grid2D& grid, bool allowDiagonal = false, CallbackVisualizacion callback = nullptr);
};

#endif // ASTAR2D_H
