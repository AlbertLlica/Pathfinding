#include "HttpServer.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "=== VISUALIZADOR 2D DE ALGORITMOS DE PATHFINDING ===" << std::endl;
    std::cout << "Algoritmos disponibles: A*, Dijkstra, D* Lite, BMSSP" << std::endl;
    std::cout << "Características:" << std::endl;
    std::cout << "- Obstáculos dinámicos (click para bloquear/desbloquear)" << std::endl;
    std::cout << "- Puntos de inicio y destino configurables" << std::endl;
    std::cout << "- Visualización paso a paso de cada algoritmo" << std::endl;
    std::cout << "- Comparación en tiempo real" << std::endl;
    std::cout << std::endl;
    
    try {
        HttpServer servidor(8080);
        servidor.iniciar();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
