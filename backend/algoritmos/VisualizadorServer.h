#ifndef VISUALIZADOR_SERVER_H
#define VISUALIZADOR_SERVER_H

#include "Grid2D.h"
#include "Resultado2D.h"
#include <string>
#include <functional>
#include <thread>
#include <atomic>

class VisualizadorServer {
private:
    Grid2D grid;
    std::string algoritmoActual;
    bool diagonalHabilitado;
    std::atomic<bool> ejecutando;
    std::thread hiloAlgoritmo;
    
public:
    VisualizadorServer(int width = 30, int height = 20);
    ~VisualizadorServer();
    
    // Grid management
    std::string getGridJSON() const;
    void setCellType(int x, int y, CellType type);
    void toggleObstacle(int x, int y);
    void setStart(int x, int y);
    void setGoal(int x, int y);
    void clearGrid();
    void clearPath();
    void generateRandomObstacles(double ratio = 0.3);
    
    // Algorithm execution
    void ejecutarAlgoritmo(const std::string& algoritmo);
    void detenerAlgoritmo();
    bool isEjecutando() const { return ejecutando; }
    
    // Configuration
    void setDiagonal(bool enabled) { diagonalHabilitado = enabled; }
    bool isDiagonal() const { return diagonalHabilitado; }
    
    // Results
    std::string getUltimoResultado() const;
    
private:
    void ejecutarAlgoritmoEnHilo(const std::string& algoritmo);
    std::string resultadoToJSON(const Resultado2D& resultado) const;
};

#endif // VISUALIZADOR_SERVER_H
