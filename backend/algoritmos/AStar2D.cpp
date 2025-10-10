#include "AStar2D.h"
#include <queue>
#include <set>
#include <chrono>
#include <cmath>
#include <limits>

struct NodoAStar2D {
    int x, y;
    double g_cost;
    double f_cost;
    
    bool operator>(const NodoAStar2D& otro) const {
        return f_cost > otro.f_cost;
    }
    
    bool operator<(const NodoAStar2D& otro) const {
        return f_cost < otro.f_cost;
    }
};

Resultado2D AStar2D::buscar(Grid2D& grid, bool allowDiagonal, CallbackVisualizacion callback) {
    auto tiempoInicio = std::chrono::high_resolution_clock::now();
    Resultado2D resultado;
    resultado.algoritmo = "A*";
    
    grid.resetVisited();
    
    auto start = grid.getStart();
    auto goal = grid.getGoal();
    
    if (start.first == -1 || goal.first == -1) {
        resultado.exito = false;
        return resultado;
    }
    
    std::priority_queue<NodoAStar2D, std::vector<NodoAStar2D>, std::greater<NodoAStar2D>> openSet;
    std::set<std::pair<int, int>> closedSet;
    
    // Initialize start node
    grid.getCell(start.first, start.second).g_cost = 0;
    grid.getCell(start.first, start.second).f_cost = grid.getHeuristic(start.first, start.second, goal.first, goal.second);
    openSet.push({start.first, start.second, 0, grid.getHeuristic(start.first, start.second, goal.first, goal.second)});
    
    while (!openSet.empty()) {
        NodoAStar2D current = openSet.top();
        openSet.pop();
        
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
            resultado.costo = current.g_cost;
            break;
        }
        
        closedSet.insert({current.x, current.y});
        grid.getCell(current.x, current.y).visited = true;
        grid.setCellType(current.x, current.y, CellType::VISITED);
        resultado.nodosVisitados.push_back({current.x, current.y});
        resultado.nodosExpandidos++;
        
        // Get neighbors
        auto neighbors = grid.getNeighbors(current.x, current.y, allowDiagonal);
        
        for (auto& neighbor : neighbors) {
            int nx = neighbor.first, ny = neighbor.second;
            
            // Skip if already processed or obstacle
            if (closedSet.count({nx, ny}) || !grid.isWalkable(nx, ny)) {
                continue;
            }
            
            // Calculate tentative g_cost
            double tentative_g = current.g_cost + grid.getDistance(current.x, current.y, nx, ny);
            
            // Check if this path to neighbor is better
            if (tentative_g < grid.getCell(nx, ny).g_cost || grid.getCell(nx, ny).g_cost == 0) {
                grid.getCell(nx, ny).parent_x = current.x;
                grid.getCell(nx, ny).parent_y = current.y;
                grid.getCell(nx, ny).g_cost = tentative_g;
                grid.getCell(nx, ny).f_cost = tentative_g + grid.getHeuristic(nx, ny, goal.first, goal.second);
                
                // Add to frontier for visualization
                if (grid.getCellType(nx, ny) != CellType::VISITED) {
                    grid.setCellType(nx, ny, CellType::FRONTIER);
                    resultado.frontera.push_back({nx, ny});
                }
                
                openSet.push({nx, ny, tentative_g, grid.getCell(nx, ny).f_cost});
                
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
