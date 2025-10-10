@echo off
echo Compilando Visualizador de Algoritmos de Pathfinding...

REM Crear directorios si no existen
if not exist "obj" mkdir obj
if not exist "bin" mkdir bin

REM Compilar archivos objeto
echo Compilando archivos fuente...
g++ -std=c++17 -Wall -Wextra -O2 -c Grid2D.cpp -o obj/Grid2D.o
g++ -std=c++17 -Wall -Wextra -O2 -c AStar2D.cpp -o obj/AStar2D.o
g++ -std=c++17 -Wall -Wextra -O2 -c Dijkstra2D.cpp -o obj/Dijkstra2D.o
g++ -std=c++17 -Wall -Wextra -O2 -c DStarLite2D.cpp -o obj/DStarLite2D.o
g++ -std=c++17 -Wall -Wextra -O2 -c BMSSP2D.cpp -o obj/BMSSP2D.o
g++ -std=c++17 -Wall -Wextra -O2 -c VisualizadorServer.cpp -o obj/VisualizadorServer.o
g++ -std=c++17 -Wall -Wextra -O2 -c HttpServer.cpp -o obj/HttpServer.o
g++ -std=c++17 -Wall -Wextra -O2 -c main_visualizacion.cpp -o obj/main_visualizacion.o

REM Compilar programa original
echo Compilando programa original...
g++ -std=c++17 -Wall -Wextra -O2 -c Grafo.cpp -o obj/Grafo.o
g++ -std=c++17 -Wall -Wextra -O2 -c AStar.cpp -o obj/AStar.o
g++ -std=c++17 -Wall -Wextra -O2 -c Dijkstra.cpp -o obj/Dijkstra.o
g++ -std=c++17 -Wall -Wextra -O2 -c DStarLite.cpp -o obj/DStarLite.o
g++ -std=c++17 -Wall -Wextra -O2 -c BMSSP.cpp -o obj/BMSSP.o
g++ -std=c++17 -Wall -Wextra -O2 -c Utilidades.cpp -o obj/Utilidades.o
g++ -std=c++17 -Wall -Wextra -O2 -c main.cpp -o obj/main.o

REM Enlazar visualizador
echo Enlazando visualizador...
g++ -std=c++17 -Wall -Wextra -O2 -o bin/main_visualizacion.exe obj/Grid2D.o obj/AStar2D.o obj/Dijkstra2D.o obj/DStarLite2D.o obj/BMSSP2D.o obj/VisualizadorServer.o obj/HttpServer.o obj/main_visualizacion.o -lws2_32 -lpthread

REM Enlazar programa original
echo Enlazando programa original...
g++ -std=c++17 -Wall -Wextra -O2 -o bin/main.exe obj/Grafo.o obj/AStar.o obj/Dijkstra.o obj/DStarLite.o obj/BMSSP.o obj/Utilidades.o obj/main.o

echo.
echo Compilacion completada!
echo.
echo Archivos generados:
echo   bin/main_visualizacion.exe - Visualizador 2D
echo   bin/main.exe - Programa original
echo.
echo Para ejecutar el visualizador:
echo   cd bin
echo   main_visualizacion.exe
echo.
echo Para ejecutar el programa original:
echo   cd bin
echo   main.exe
echo.
pause
