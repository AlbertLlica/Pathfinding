#include <cmath>
#include <algorithm>
#include "Grafo.h"
Grafo::Grafo(int nodos) : n(nodos), adyacencia(nodos), coordenadas(nodos) {}

void Grafo::agregarArista(int origen, int destino, double peso) {
    adyacencia[origen].push_back({destino, peso});
}

void Grafo::generarGrafoAleatorio(int aristas, int semilla) {
    std::mt19937 generador(semilla);
    std::uniform_int_distribution<> distNodos(0, n - 1);
    std::uniform_real_distribution<> distPesos(1.0, 100.0);
    std::uniform_real_distribution<> distCoord(0.0, 1000.0);
    
    // Generar coordenadas para todos los nodos
    for (int i = 0; i < n; i++) {
        coordenadas[i] = {distCoord(generador), distCoord(generador)};
    }
    
    // Asegurar conectividad creando un árbol generador
    for (int i = 1; i < n; i++) {
        int origen = distNodos(generador) % i;
        double peso = distPesos(generador);
        agregarArista(origen, i, peso);
        agregarArista(i, origen, peso);
    }
    
    // Agregar aristas adicionales
    int aristasAdicionales = aristas - (n - 1);
    for (int i = 0; i < aristasAdicionales; i++) {
        int origen = distNodos(generador);
        int destino = distNodos(generador);
        if (origen != destino) {
            double peso = distPesos(generador);
            agregarArista(origen, destino, peso);
        }
    }
}

double Grafo::heuristica(int desde, int hasta) const {
    double dx = coordenadas[desde].first - coordenadas[hasta].first;
    double dy = coordenadas[desde].second - coordenadas[hasta].second;
    return std::sqrt(dx * dx + dy * dy);
}
