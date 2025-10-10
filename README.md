# 🧭 Visualizador 2D de Algoritmos de Pathfinding

Un sistema interactivo para visualizar y comparar algoritmos de búsqueda de caminos en un entorno 2D dinámico.

## 🚀 Características

- **4 Algoritmos Implementados**: A*, Dijkstra, D* Lite, BMSSP
- **Entorno 2D Dinámico**: Obstáculos que se pueden bloquear/desbloquear en tiempo real
- **Visualización Paso a Paso**: Ve cómo cada algoritmo explora el espacio
- **Configuración Flexible**: Puntos de inicio y destino configurables
- **Interfaz Moderna**: UI responsiva y atractiva
- **Comparación en Tiempo Real**: Ejecuta y compara múltiples algoritmos

## 📁 Estructura del Proyecto

```
2D Dinamic/
├── backend/
│   └── algoritmos/
│       ├── Grid2D.h/.cpp          # Sistema de grid 2D
│       ├── AStar2D.h/.cpp         # Algoritmo A* 2D
│       ├── Dijkstra2D.h/.cpp      # Algoritmo Dijkstra 2D
│       ├── DStarLite2D.h/.cpp     # Algoritmo D* Lite 2D
│       ├── BMSSP2D.h/.cpp         # Algoritmo BMSSP 2D
│       ├── VisualizadorServer.h/.cpp # Servidor de visualización
│       ├── HttpServer.h/.cpp      # Servidor HTTP simple
│       ├── main_visualizacion.cpp # Programa principal
│       └── Makefile              # Compilación
└── frontend/
    ├── index.html                # Interfaz principal
    ├── style.css                 # Estilos
    └── script.js                 # Lógica del frontend
```

## 🛠️ Instalación y Compilación

### Requisitos
- Compilador C++17 (GCC, Clang, o MSVC)
- Navegador web moderno

### Compilación del Backend

```bash
cd backend/algoritmos
make all
```

### Comandos Disponibles
```bash
make run           # Compilar y ejecutar visualizador
make run-original  # Ejecutar programa original de comparación
make clean         # Limpiar archivos compilados
make help          # Mostrar ayuda
```

## 🎮 Uso

### 1. Iniciar el Servidor
```bash
cd backend/algoritmos
make run
```

### 2. Abrir el Frontend
Abre tu navegador y ve a: `http://localhost:8080`

### 3. Configurar el Entorno
- **Establecer Inicio**: Click en el botón "🎯 Establecer Inicio" y luego click en una celda
- **Establecer Meta**: Click en el botón "🏁 Establecer Meta" y luego click en una celda
- **Obstáculos**: Click izquierdo en celdas vacías para crear obstáculos, click nuevamente para removerlos

### 4. Ejecutar Algoritmos
Click en cualquiera de los botones de algoritmos:
- **A***: Búsqueda heurística con costo optimizado
- **Dijkstra**: Búsqueda de costo mínimo
- **D* Lite**: Algoritmo dinámico para entornos cambiantes
- **BMSSP**: Búsqueda limitada (bounded search)

### 5. Configuraciones
- **Movimiento Diagonal**: Checkbox para permitir movimiento en 8 direcciones
- **Velocidad**: Slider para controlar la velocidad de animación
- **Obstáculos Aleatorios**: Genera obstáculos aleatorios en el grid

## 🎯 Controles

| Acción | Método |
|--------|--------|
| Bloquear/Desbloquear obstáculo | Click izquierdo |
| Establecer inicio/meta | Click derecho o usar botones |
| Limpiar camino | Botón "🧹 Limpiar Camino" |
| Limpiar todo | Botón "🗑️ Limpiar Todo" |
| Generar obstáculos aleatorios | Botón "🎲 Obstáculos Aleatorios" |

## 🔬 Algoritmos Implementados

### A* (A Star)
- **Tipo**: Búsqueda heurística
- **Características**: Optimiza usando f(n) = g(n) + h(n)
- **Ventajas**: Rápido y encuentra caminos óptimos
- **Uso**: Ideal para la mayoría de casos

### Dijkstra
- **Tipo**: Búsqueda de costo uniforme
- **Características**: Garantiza el camino de menor costo
- **Ventajas**: Siempre encuentra la solución óptima
- **Uso**: Cuando el costo es más importante que la velocidad

### D* Lite
- **Tipo**: Algoritmo dinámico
- **Características**: Recalcula eficientemente cuando cambia el entorno
- **Ventajas**: Ideal para entornos dinámicos
- **Uso**: Cuando los obstáculos pueden aparecer/desaparecer

### BMSSP (Bounded Multi-Source Shortest Path)
- **Tipo**: Búsqueda limitada
- **Características**: Busca dentro de un límite predefinido
- **Ventajas**: Controla el tiempo de búsqueda
- **Uso**: Cuando necesitas controlar el tiempo de ejecución

## 🎨 Tipos de Celda

| Color | Tipo | Descripción |
|-------|------|-------------|
| 🟢 Verde | Inicio | Punto de partida |
| 🔴 Rojo | Meta | Punto de destino |
| ⚫ Negro | Obstáculo | Celda bloqueada |
| 🔵 Azul | Visitado | Celda explorada |
| 🟠 Naranja | Frontera | Celda en cola de exploración |
| 🟣 Morado | Camino | Camino encontrado |

## 🔧 Personalización

### Modificar el Grid
Edita las variables en `frontend/script.js`:
```javascript
this.gridWidth = 30;   // Ancho del grid
this.gridHeight = 20;  // Alto del grid
this.cellSize = 15;    // Tamaño de cada celda
```

### Ajustar Velocidad
El slider de velocidad controla el delay entre animaciones:
- Mínimo: 1ms (muy rápido)
- Máximo: 100ms (muy lento)

### Cambiar Puerto
Modifica el puerto en `backend/algoritmos/main_visualizacion.cpp`:
```cpp
HttpServer servidor(8080);  // Cambiar 8080 por el puerto deseado
```

## 🐛 Solución de Problemas

### Error de Compilación
- Verifica que tienes C++17 instalado
- En Windows, asegúrate de tener las librerías de Winsock

### Puerto en Uso
- Cambia el puerto en el código fuente
- O termina el proceso que está usando el puerto 8080

### Problemas de Rendimiento
- Reduce el tamaño del grid
- Aumenta la velocidad de animación
- Usa algoritmos más simples (Dijkstra vs D* Lite)

## 📚 Referencias

- [A* Pathfinding](https://en.wikipedia.org/wiki/A*_search_algorithm)
- [Dijkstra's Algorithm](https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm)
- [D* Lite](https://en.wikipedia.org/wiki/D*)
- [BMSSP](https://en.wikipedia.org/wiki/Shortest_path_problem)

## 🤝 Contribuciones

¡Las contribuciones son bienvenidas! Algunas ideas:
- Implementar más algoritmos (JPS, Theta*, etc.)
- Mejoras en la UI/UX
- Optimizaciones de rendimiento
- Nuevas características de visualización

## 📄 Licencia

Este proyecto es de código abierto. Úsalo libremente para aprendizaje y desarrollo.
