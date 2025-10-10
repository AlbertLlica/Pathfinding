#include "BMSSP2D.h"
#include <queue>
#include <set>
#include <chrono>
#include <cmath>

struct NodoBMSSP2D {
    int x, y;
    double costo;
    double heuristica;
    
    bool operator>(const NodoBMSSP2D& otro) const {
        return (costo + heuristica) > (otro.costo + otro.heuristica);
    }
};

Resultado2D BMSSP2D::buscar(Grid2D& grid, bool allowDiagonal, CallbackVisualizacion callback) {
    auto tiempoInicio = std::chrono::high_resolution_clock::now();
    Resultado2D resultado;
    resultado.algoritmo = "BMSSP";
    
    grid.resetVisited();
    
    auto start = grid.getStart();
    auto goal = grid.getGoal();
    
    if (start.first == -1 || goal.first == -1) {
        resultado.exito = false;
        return resultado;
    }
    
    std::priority_queue<NodoBMSSP2D, std::vector<NodoBMSSP2D>, std::greater<NodoBMSSP2D>> openSet;
    std::set<std::pair<int, int>> processed;
    
    // Initialize distances
    for (int y = 0; y < grid.getHeight(); y++) {
        for (int x = 0; x < grid.getWidth(); x++) {
            grid.getCell(x, y).g_cost = std::numeric_limits<double>::infinity();
        }
    }
    
    grid.getCell(start.first, start.second).g_cost = 0;
    double h = grid.getHeuristic(start.first, start.second, goal.first, goal.second);
    openSet.push({start.first, start.second, 0, h});
    
    // Set search limit (bounded search)
    double limite = h * 2.0;
    
    while (!openSet.empty()) {
        NodoBMSSP2D current = openSet.top();
        openSet.pop();
        
        // Skip if already processed
        if (processed.count({current.x, current.y})) {
            continue;
        }
        
        // Apply bounded search limit
        if (current.costo > limite) {
            continue;
        }
        
        processed.insert({current.x, current.y});
        grid.getCell(current.x, current.y).visited = true;
        grid.setCellType(current.x, current.y, CellType::VISITED);
        resultado.nodosVisitados.push_back({current.x, current.y});
        resultado.nodosExpandidos++;
        
        // Check if we reached the goal
        if (current.x == goal.first && current.y == goal.second) {
            // Reconstruct path
            int x = current.x, y = current.y;
            while (x != -1 && y != -1) {
                resultado.camino.push_back({x, y});
                grid.setCellType(x, y, CellType::PATH);
                
                int parent_x = grid.getCell(x, y).parent_x;
                int parent_y = grid.getCell(x, y).parent_y;
                x = parent_x;
                y = parent_y;
            }
            std::reverse(resultado.camino.begin(), resultado.camino.end());
            resultado.exito = true;
            resultado.costo = current.costo;
            break;
        }
        
        // Get neighbors
        auto neighbors = grid.getNeighbors(current.x, current.y, allowDiagonal);
        
        for (auto& neighbor : neighbors) {
            int nx = neighbor.first, ny = neighbor.second;
            
            // Skip if already processed or obstacle
            if (processed.count({nx, ny}) || !grid.isWalkable(nx, ny)) {
                continue;
            }
            
            double nuevaDist = current.costo + grid.getDistance(current.x, current.y, nx, ny);
            
            // Apply bounded search limit
            if (nuevaDist < grid.getCell(nx, ny).g_cost && nuevaDist <= limite) {
                grid.getCell(nx, ny).g_cost = nuevaDist;
                grid.getCell(nx, ny).parent_x = current.x;
                grid.getCell(nx, ny).parent_y = current.y;
                
                double h = grid.getHeuristic(nx, ny, goal.first, goal.second);
                openSet.push({nx, ny, nuevaDist, h});
                
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
    
    auto tiempoFin = std::chrono::high_resolution_clock::now();
    resultado.tiempoMicrosegundos = std::chrono::duration_cast<std::chrono::microseconds>(tiempoFin - tiempoInicio).count();
    
    return resultado;
}
