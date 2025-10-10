#ifndef GRAFO_H
#define GRAFO_H

#include <vector>
#include <utility>
#include <random>

struct Arista {
    int destino;
    double peso;
};

class Grafo {
public:
    int n; // número de nodos
    std::vector<std::vector<Arista>> adyacencia;
    std::vector<std::pair<double, double>> coordenadas;
    
    Grafo(int nodos);
    void agregarArista(int origen, int destino, double peso);
    void generarGrafoAleatorio(int aristas, int semilla = 42);
    double heuristica(int desde, int hasta) const;
};

#endif // GRAFO_H