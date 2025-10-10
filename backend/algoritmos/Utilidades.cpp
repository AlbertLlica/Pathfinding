#include <iostream>
#include <iomanip>
#include <algorithm>
#include "Utilidades.h"

void Utilidades::imprimirResultado(const std::string& nombre, const ResultadoBusqueda& resultado) {
    std::cout << "\n" << nombre << ":\n";
    std::cout << "  Tiempo: " << resultado.tiempoMicrosegundos << " microsegundos (" 
         << std::fixed << std::setprecision(2) << resultado.tiempoMicrosegundos / 1000.0 << " ms)\n";
    std::cout << "  Costo del camino: " << std::fixed << std::setprecision(2) << resultado.costo << "\n";
    std::cout << "  Longitud del camino: " << resultado.camino.size() << " nodos\n";
    std::cout << "  Nodos expandidos: " << resultado.nodosExpandidos << "\n";
    
    if (resultado.camino.size() <= 20 && resultado.camino.size() > 0) {
        std::cout << "  Camino: ";
        for (size_t i = 0; i < resultado.camino.size(); i++) {
            std::cout << resultado.camino[i];
            if (i < resultado.camino.size() - 1) std::cout << " -> ";
        }
        std::cout << "\n";
    }
}

void Utilidades::imprimirResumenComparativo(
    const ResultadoBusqueda& r1,
    const ResultadoBusqueda& r2,
    const ResultadoBusqueda& r3,
    const ResultadoBusqueda& r4
) {
    std::cout << "\n--- RESUMEN COMPARATIVO ---\n";
    
    std::vector<std::pair<std::string, long long>> tiempos = {
        {"Dijkstra", r1.tiempoMicrosegundos},
        {"A*", r2.tiempoMicrosegundos},
        {"D* Lite", r3.tiempoMicrosegundos},
        {"BMSSP", r4.tiempoMicrosegundos}
    };
    auto masRapido = std::min_element(tiempos.begin(), tiempos.end(), 
        [](const auto& a, const auto& b) { return a.second < b.second; });
    std::cout << "Algoritmo mas rapido: " << masRapido->first 
              << " (" << masRapido->second << " microsegundos)\n";
    
    std::vector<std::pair<std::string, int>> expansiones = {
        {"Dijkstra", r1.nodosExpandidos},
        {"A*", r2.nodosExpandidos},
        {"D* Lite", r3.nodosExpandidos},
        {"BMSSP", r4.nodosExpandidos}
    };
    auto menosNodos = std::min_element(expansiones.begin(), expansiones.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; });
    std::cout << "Menos nodos expandidos: " << menosNodos->first 
              << " (" << menosNodos->second << " nodos)\n";
}