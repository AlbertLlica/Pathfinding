#include "Grid2D.h"
#include <cmath>
#include <random>
#include <sstream>
#include <algorithm>
#include <limits>

Grid2D::Grid2D(int width, int height) : width(width), height(height), start_x(-1), start_y(-1), goal_x(-1), goal_y(-1) {
    grid.resize(height);
    for (int y = 0; y < height; y++) {
        grid[y].resize(width);
        for (int x = 0; x < width; x++) {
            grid[y][x] = Cell(x, y);
        }
    }
}

void Grid2D::clear() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            grid[y][x] = Cell(x, y);
        }
    }
    start_x = start_y = goal_x = goal_y = -1;
}

void Grid2D::resetVisited() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            grid[y][x].visited = false;
            grid[y][x].g_cost = 0;
            grid[y][x].f_cost = 0;
            grid[y][x].parent_x = -1;
            grid[y][x].parent_y = -1;
            
            // Reset path and visited cells to empty
            if (grid[y][x].type == CellType::VISITED || grid[y][x].type == CellType::PATH || grid[y][x].type == CellType::FRONTIER) {
                grid[y][x].type = CellType::EMPTY;
            }
        }
    }
}

void Grid2D::setCellType(int x, int y, CellType type) {
    if (!isValid(x, y)) return;
    
    // Don't overwrite start and goal unless explicitly changing them
    if (type == CellType::START) {
        if (start_x != -1 && start_y != -1) {
            grid[start_y][start_x].type = CellType::EMPTY;
        }
        start_x = x;
        start_y = y;
    } else if (type == CellType::GOAL) {
        if (goal_x != -1 && goal_y != -1) {
            grid[goal_y][goal_x].type = CellType::EMPTY;
        }
        goal_x = x;
        goal_y = y;
    }
    
    grid[y][x].type = type;
}

void Grid2D::toggleObstacle(int x, int y) {
    if (!isValid(x, y)) return;
    
    // Don't allow toggling start or goal positions
    if ((x == start_x && y == start_y) || (x == goal_x && y == goal_y)) {
        return;
    }
    
    if (grid[y][x].type == CellType::OBSTACLE) {
        grid[y][x].type = CellType::EMPTY;
    } else {
        grid[y][x].type = CellType::OBSTACLE;
    }
}

CellType Grid2D::getCellType(int x, int y) const {
    if (!isValid(x, y)) return CellType::OBSTACLE;
    return grid[y][x].type;
}

bool Grid2D::isValid(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

bool Grid2D::isWalkable(int x, int y) const {
    if (!isValid(x, y)) return false;
    return grid[y][x].type != CellType::OBSTACLE;
}

void Grid2D::setStart(int x, int y) {
    if (!isValid(x, y) || !isWalkable(x, y)) return;
    setCellType(x, y, CellType::START);
}

void Grid2D::setGoal(int x, int y) {
    if (!isValid(x, y) || !isWalkable(x, y)) return;
    setCellType(x, y, CellType::GOAL);
}

std::vector<std::pair<int, int>> Grid2D::getNeighbors(int x, int y, bool allowDiagonal) const {
    std::vector<std::pair<int, int>> neighbors;
    
    // 4-directional movement
    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};
    
    for (int i = 0; i < 4; i++) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        if (isValid(nx, ny)) {
            neighbors.push_back({nx, ny});
        }
    }
    
    // Diagonal movement
    if (allowDiagonal) {
        int diag_dx[] = {-1, -1, 1, 1};
        int diag_dy[] = {-1, 1, -1, 1};
        
        for (int i = 0; i < 4; i++) {
            int nx = x + diag_dx[i];
            int ny = y + diag_dy[i];
            if (isValid(nx, ny)) {
                neighbors.push_back({nx, ny});
            }
        }
    }
    
    return neighbors;
}

double Grid2D::getDistance(int x1, int y1, int x2, int y2) const {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

double Grid2D::getHeuristic(int x1, int y1, int x2, int y2) const {
    // Manhattan distance for 4-directional movement
    return abs(x2 - x1) + abs(y2 - y1);
}

std::string Grid2D::toJSON() const {
    std::stringstream ss;
    ss << "{";
    ss << "\"width\":" << width << ",";
    ss << "\"height\":" << height << ",";
    ss << "\"start\":[" << start_x << "," << start_y << "],";
    ss << "\"goal\":[" << goal_x << "," << goal_y << "],";
    ss << "\"grid\":[";
    
    for (int y = 0; y < height; y++) {
        ss << "[";
        for (int x = 0; x < width; x++) {
            ss << static_cast<int>(grid[y][x].type);
            if (x < width - 1) ss << ",";
        }
        ss << "]";
        if (y < height - 1) ss << ",";
    }
    
    ss << "]";
    ss << "}";
    return ss.str();
}

void Grid2D::fromJSON(const std::string& json) {
    // Simple JSON parsing - in a real implementation, use a proper JSON library
    // For now, this is a placeholder
}

void Grid2D::generateRandomObstacles(double obstacleRatio) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (dis(gen) < obstacleRatio) {
                // Don't place obstacles on start or goal
                if ((x != start_x || y != start_y) && (x != goal_x || y != goal_y)) {
                    grid[y][x].type = CellType::OBSTACLE;
                }
            }
        }
    }
}

void Grid2D::clearPath() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (grid[y][x].type == CellType::VISITED || 
                grid[y][x].type == CellType::PATH || 
                grid[y][x].type == CellType::FRONTIER) {
                grid[y][x].type = CellType::EMPTY;
            }
            grid[y][x].visited = false;
            grid[y][x].g_cost = 0;
            grid[y][x].f_cost = 0;
            grid[y][x].parent_x = -1;
            grid[y][x].parent_y = -1;
        }
    }
}
