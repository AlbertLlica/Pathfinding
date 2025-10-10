# Makefile para el Visualizador de Algoritmos de Pathfinding

# Compilador y flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS = -lpthread

# En Windows, usar winsock
ifeq ($(OS),Windows_NT)
    LDFLAGS += -lws2_32
endif

# Directorios
SRC_DIR = .
OBJ_DIR = obj
BIN_DIR = bin

# Archivos fuente
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Targets principales
TARGETS = main_visualizacion.exe main.exe

# Regla por defecto
all: $(TARGETS)

# Crear directorios
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Compilar objeto de archivos individuales
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compilar visualizador principal
main_visualizacion.exe: $(filter-out $(OBJ_DIR)/main.o, $(OBJECTS)) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $(BIN_DIR)/$@ $^ $(LDFLAGS)

# Compilar programa original
main.exe: $(filter-out $(OBJ_DIR)/main_visualizacion.o $(OBJ_DIR)/HttpServer.o $(OBJ_DIR)/VisualizadorServer.o, $(OBJECTS)) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $(BIN_DIR)/$@ $^ $(LDFLAGS)

# Limpiar archivos compilados
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Ejecutar visualizador
run: main_visualizacion.exe
	./$(BIN_DIR)/main_visualizacion.exe

# Ejecutar programa original
run-original: main.exe
	./$(BIN_DIR)/main.exe

# Ayuda
help:
	@echo "Comandos disponibles:"
	@echo "  make all           - Compilar todos los programas"
	@echo "  make run           - Compilar y ejecutar visualizador"
	@echo "  make run-original  - Compilar y ejecutar programa original"
	@echo "  make clean         - Limpiar archivos compilados"
	@echo "  make help          - Mostrar esta ayuda"

.PHONY: all clean run run-original help
