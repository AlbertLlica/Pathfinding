# Pathfinding 2D Dinamic

Este proyecto implementa y visualiza algoritmos de búsqueda de caminos en un entorno 2D, útil para robótica y simulaciones educativas.

## Estructura del proyecto

- `backend/algoritmos/` — Algoritmos en C++ (A*, Dijkstra, BMSSP, D* Lite, etc.)
- `backend/api/` — API en Python para interactuar con los algoritmos y la grilla
- `frontend/` — Interfaz web para visualización y control

## Algoritmos principales

- **A***: Búsqueda óptima usando heurística
- **Dijkstra**: Búsqueda de costo mínimo sin heurística
- **BMSSP**: Bounded Multi-Source Shortest Path, busca caminos usando múltiples fuentes y pivotes
- **D* Lite**: Algoritmo incremental para entornos dinámicos

## Uso rápido

1. Instala dependencias:
   ```bash
   pip install -r backend/api/requirements.txt
   ```
2. Ejecuta el servidor backend:
   ```bash
   cd backend/algoritmos
   ./run_visualizer.bat
   ```
3. Abre `frontend/index.html` en tu navegador

## Estructura de archivos clave

- `backend/api/algorithms/astar.py` — Algoritmo A*
- `backend/api/algorithms/dijkstra.py` — Algoritmo Dijkstra
- `backend/api/algorithms/bmssp.py` — Algoritmo BMSSP
- `backend/api/algorithms/grid.py` — Estructura de la grilla y celdas
- `frontend/index.html` — Visualizador web

## Personalización

- Puedes modificar los algoritmos en `backend/api/algorithms/` para experimentar con variantes
- Los parámetros de la grilla y obstáculos se pueden ajustar desde el frontend

## Créditos

Desarrollado por AlbertLlica y colaboradores para fines educativos y de investigación en robótica y algoritmos de búsqueda.
