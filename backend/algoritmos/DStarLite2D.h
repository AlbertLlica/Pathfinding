#ifndef DSTARLITE2D_H
#define DSTARLITE2D_H

#include "Grid2D.h"
#include "Resultado2D.h"
#include <functional>

class DStarLite2D {
public:
    static Resultado2D buscar(Grid2D& grid, bool allowDiagonal = false, CallbackVisualizacion callback = nullptr);
};

#endif // DSTARLITE2D_H
