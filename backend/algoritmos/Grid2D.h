#ifndef GRID2D_H
#define GRID2D_H

#include <vector>
#include <string>
#include <functional>

enum class CellType {
    EMPTY = 0,
    OBSTACLE = 1,
    START = 2,
    GOAL = 3,
    VISITED = 4,
    PATH = 5,
    FRONTIER = 6
};

struct Cell {
    int x, y;
    CellType type;
    double g_cost;
    double f_cost;
    int parent_x, parent_y;
    bool visited;
    
    Cell() : x(0), y(0), type(CellType::EMPTY), g_cost(0), f_cost(0), 
             parent_x(-1), parent_y(-1), visited(false) {}
    Cell(int x, int y) : x(x), y(y), type(CellType::EMPTY), g_cost(0), f_cost(0), 
                         parent_x(-1), parent_y(-1), visited(false) {}
};

class Grid2D {
private:
    int width, height;
    std::vector<std::vector<Cell>> grid;
    int start_x, start_y;
    int goal_x, goal_y;
    
public:
    Grid2D(int width, int height);
    
    // Grid management
    void clear();
    void resetVisited();
    void setCellType(int x, int y, CellType type);
    void toggleObstacle(int x, int y);
    CellType getCellType(int x, int y) const;
    bool isValid(int x, int y) const;
    bool isWalkable(int x, int y) const;
    
    // Start/Goal management
    void setStart(int x, int y);
    void setGoal(int x, int y);
    std::pair<int, int> getStart() const { return {start_x, start_y}; }
    std::pair<int, int> getGoal() const { return {goal_x, goal_y}; }
    
    // Grid properties
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    Cell& getCell(int x, int y) { return grid[y][x]; }
    const Cell& getCell(int x, int y) const { return grid[y][x]; }
    
    // Pathfinding helpers
    std::vector<std::pair<int, int>> getNeighbors(int x, int y, bool allowDiagonal = false) const;
    double getDistance(int x1, int y1, int x2, int y2) const;
    double getHeuristic(int x1, int y1, int x2, int y2) const;
    
    // Visualization helpers
    std::string toJSON() const;
    void fromJSON(const std::string& json);
    
    // Utility functions
    void generateRandomObstacles(double obstacleRatio = 0.3);
    void clearPath();
};

#endif // GRID2D_H
