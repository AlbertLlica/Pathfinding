#include "VisualizadorServer.h"
#include "AStar2D.h"
#include "Dijkstra2D.h"
#include "DStarLite2D.h"
#include "BMSSP2D.h"
#include <sstream>
#include <chrono>
#include <thread>

VisualizadorServer::VisualizadorServer(int width, int height) 
    : grid(width, height), diagonalHabilitado(false), ejecutando(false) {
}

VisualizadorServer::~VisualizadorServer() {
    if (ejecutando) {
        detenerAlgoritmo();
    }
}

std::string VisualizadorServer::getGridJSON() const {
    return grid.toJSON();
}

void VisualizadorServer::setCellType(int x, int y, CellType type) {
    grid.setCellType(x, y, type);
}

void VisualizadorServer::toggleObstacle(int x, int y) {
    grid.toggleObstacle(x, y);
}

void VisualizadorServer::setStart(int x, int y) {
    grid.setStart(x, y);
}

void VisualizadorServer::setGoal(int x, int y) {
    grid.setGoal(x, y);
}

void VisualizadorServer::clearGrid() {
    grid.clear();
}

void VisualizadorServer::clearPath() {
    grid.clearPath();
}

void VisualizadorServer::generateRandomObstacles(double ratio) {
    grid.generateRandomObstacles(ratio);
}

void VisualizadorServer::ejecutarAlgoritmo(const std::string& algoritmo) {
    if (ejecutando) {
        detenerAlgoritmo();
    }
    
    algoritmoActual = algoritmo;
    ejecutando = true;
    
    // Start algorithm in separate thread
    hiloAlgoritmo = std::thread(&VisualizadorServer::ejecutarAlgoritmoEnHilo, this, algoritmo);
}

void VisualizadorServer::detenerAlgoritmo() {
    if (ejecutando) {
        ejecutando = false;
        if (hiloAlgoritmo.joinable()) {
            hiloAlgoritmo.join();
        }
    }
}

void VisualizadorServer::ejecutarAlgoritmoEnHilo(const std::string& algoritmo) {
    grid.clearPath();
    
    // Callback for step-by-step visualization
    auto callback = [this](const Grid2D& g, const std::vector<std::pair<int, int>>& visitados, const std::vector<std::pair<int, int>>& frontera) {
        // Here we would send updates to the frontend
        // For now, we'll just add a small delay to simulate visualization
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    };
    
    Resultado2D resultado;
    
    if (algoritmo == "A*" || algoritmo == "AStar") {
        resultado = AStar2D::buscar(grid, diagonalHabilitado, callback);
    } else if (algoritmo == "Dijkstra") {
        resultado = Dijkstra2D::buscar(grid, diagonalHabilitado, callback);
    } else if (algoritmo == "D* Lite" || algoritmo == "DStarLite") {
        resultado = DStarLite2D::buscar(grid, diagonalHabilitado, callback);
    } else if (algoritmo == "BMSSP") {
        resultado = BMSSP2D::buscar(grid, diagonalHabilitado, callback);
    }
    
    ejecutando = false;
}

std::string VisualizadorServer::getUltimoResultado() const {
    // This would return the last result as JSON
    // For now, return a placeholder
    return "{\"status\":\"completed\"}";
}

std::string VisualizadorServer::resultadoToJSON(const Resultado2D& resultado) const {
    std::stringstream ss;
    ss << "{";
    ss << "\"exito\":" << (resultado.exito ? "true" : "false") << ",";
    ss << "\"algoritmo\":\"" << resultado.algoritmo << "\",";
    ss << "\"costo\":" << resultado.costo << ",";
    ss << "\"tiempoMicrosegundos\":" << resultado.tiempoMicrosegundos << ",";
    ss << "\"nodosExpandidos\":" << resultado.nodosExpandidos << ",";
    ss << "\"longitudCamino\":" << resultado.camino.size();
    ss << "}";
    return ss.str();
}
