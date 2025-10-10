#include "DStarLite2D.h"
#include <queue>
#include <set>
#include <chrono>
#include <cmath>
#include <limits>

struct NodoDStar2D {
    int x, y;
    double clave1;
    double clave2;
    
    bool operator>(const NodoDStar2D& otro) const {
        if (clave1 != otro.clave1) return clave1 > otro.clave1;
        return clave2 > otro.clave2;
    }
};

Resultado2D DStarLite2D::buscar(Grid2D& grid, bool allowDiagonal, CallbackVisualizacion callback) {
    auto tiempoInicio = std::chrono::high_resolution_clock::now();
    Resultado2D resultado;
    resultado.algoritmo = "D* Lite";
    
    grid.resetVisited();
    
    auto start = grid.getStart();
    auto goal = grid.getGoal();
    
    if (start.first == -1 || goal.first == -1) {
        resultado.exito = false;
        return resultado;
    }
    
    // Initialize g and rhs values
    std::vector<std::vector<double>> g(grid.getHeight(), std::vector<double>(grid.getWidth(), std::numeric_limits<double>::infinity()));
    std::vector<std::vector<double>> rhs(grid.getHeight(), std::vector<double>(grid.getWidth(), std::numeric_limits<double>::infinity()));
    
    rhs[goal.second][goal.first] = 0;
    
    std::priority_queue<NodoDStar2D, std::vector<NodoDStar2D>, std::greater<NodoDStar2D>> openSet;
    
    auto calcularClave = [&](int x, int y) -> std::pair<double, double> {
        double g_val = g[y][x];
        double rhs_val = rhs[y][x];
        double minVal = std::min(g_val, rhs_val);
        double h_val = grid.getHeuristic(x, y, start.first, start.second);
        return {minVal + h_val, minVal};
    };
    
    auto clave = calcularClave(goal.first, goal.second);
    openSet.push({goal.first, goal.second, clave.first, clave.second});
    
    while (!openSet.empty()) {
        NodoDStar2D current = openSet.top();
        openSet.pop();
        
        resultado.nodosExpandidos++;
        grid.getCell(current.x, current.y).visited = true;
        grid.setCellType(current.x, current.y, CellType::VISITED);
        resultado.nodosVisitados.push_back({current.x, current.y});
        
        // Check if we reached the start and path is consistent
        if (current.x == start.first && current.y == start.second && g[current.y][current.x] == rhs[current.y][current.x]) {
            break;
        }
        
        // Update g value
        if (g[current.y][current.x] > rhs[current.y][current.x]) {
            g[current.y][current.x] = rhs[current.y][current.x];
        } else {
            g[current.y][current.x] = std::numeric_limits<double>::infinity();
            rhs[current.y][current.x] = std::numeric_limits<double>::infinity();
        }
        
        // Get neighbors
        auto neighbors = grid.getNeighbors(current.x, current.y, allowDiagonal);
        
        for (auto& neighbor : neighbors) {
            int nx = neighbor.first, ny = neighbor.second;
            
            if (!grid.isWalkable(nx, ny)) continue;
            
            // Update rhs value for neighbor
            double newRhs = g[current.y][current.x] + grid.getDistance(current.x, current.y, nx, ny);
            
            if (newRhs < rhs[ny][nx]) {
                rhs[ny][nx] = newRhs;
                grid.getCell(nx, ny).parent_x = current.x;
                grid.getCell(nx, ny).parent_y = current.y;
                
                auto nuevaClave = calcularClave(nx, ny);
                openSet.push({nx, ny, nuevaClave.first, nuevaClave.second});
                
                // Add to frontier for visualization
                if (grid.getCellType(nx, ny) != CellType::VISITED) {
                    grid.setCellType(nx, ny, CellType::FRONTIER);
                    resultado.frontera.push_back({nx, ny});
                }
                
                // Callback for visualization
                if (callback) {
                    callback(grid, resultado.nodosVisitados, resultado.frontera);
                }
            }
        }
    }
    
    // Reconstruct path
    if (g[start.second][start.first] != std::numeric_limits<double>::infinity()) {
        int x = start.first, y = start.second;
        while (x != -1 && y != -1) {
            resultado.camino.push_back({x, y});
            grid.setCellType(x, y, CellType::PATH);
            
            int parent_x = grid.getCell(x, y).parent_x;
            int parent_y = grid.getCell(x, y).parent_y;
            x = parent_x;
            y = parent_y;
            
            // Avoid infinite loops
            if (x == start.first && y == start.second) break;
        }
        resultado.exito = true;
        resultado.costo = g[start.second][start.first];
    }
    
    auto tiempoFin = std::chrono::high_resolution_clock::now();
    resultado.tiempoMicrosegundos = std::chrono::duration_cast<std::chrono::microseconds>(tiempoFin - tiempoInicio).count();
    
    return resultado;
}
