from enum import Enum
import random
import math


class CellType(str, Enum):
    EMPTY = 'empty'
    OBSTACLE = 'obstacle'
    START = 'start'
    GOAL = 'goal'
    VISITED = 'visited'
    PATH = 'path'
    FRONTIER = 'frontier'


class Cell:
    def __init__(self, x, y, type=CellType.EMPTY):
        self.x = x
        self.y = y
        self.type = type
        self.g = float('inf')
        self.f = float('inf')
        self.parent = (-1, -1)
        self.visited = False


class Grid:
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.cells = [[Cell(x, y) for x in range(width)] for y in range(height)]
        self.start = (-1, -1)
        self.goal = (-1, -1)

    def is_valid(self, x, y):
        return 0 <= x < self.width and 0 <= y < self.height

    def set_obstacle(self, x, y, val=True):
        if not self.is_valid(x, y):
            return
        self.cells[y][x].type = CellType.OBSTACLE if val else CellType.EMPTY

    def set_start(self, x, y):
        if not self.is_valid(x, y):
            return
        if self.start != (-1, -1):
            sx, sy = self.start
            self.cells[sy][sx].type = CellType.EMPTY
        self.start = (x, y)
        self.cells[y][x].type = CellType.START

    def set_goal(self, x, y):
        if not self.is_valid(x, y):
            return
        if self.goal != (-1, -1):
            gx, gy = self.goal
            self.cells[gy][gx].type = CellType.EMPTY
        self.goal = (x, y)
        self.cells[y][x].type = CellType.GOAL

    def is_walkable(self, x, y):
        if not self.is_valid(x, y):
            return False
        return self.cells[y][x].type != CellType.OBSTACLE

    def get_neighbors(self, x, y, allow_diagonal=False):
        dirs = [(1,0),(-1,0),(0,1),(0,-1)]
        if allow_diagonal:
            dirs += [(1,1),(1,-1),(-1,1),(-1,-1)]
        result = []
        for dx, dy in dirs:
            nx, ny = x+dx, y+dy
            if self.is_valid(nx, ny):
                result.append((nx, ny))
        return result

    def get_distance(self, x1, y1, x2, y2):
        # Euclidean distance for diagonal
        return math.hypot(x2-x1, y2-y1)

    def get_heuristic(self, x1, y1, x2, y2):
        # Use Manhattan for simplicity
        return abs(x2-x1) + abs(y2-y1)

    def clear_path(self):
        for row in self.cells:
            for cell in row:
                if cell.type in (CellType.VISITED, CellType.PATH, CellType.FRONTIER):
                    cell.type = CellType.EMPTY
                cell.g = float('inf')
                cell.f = float('inf')
                cell.parent = (-1, -1)
                cell.visited = False

    def generate_random_obstacles(self, ratio=0.3):
        for y in range(self.height):
            for x in range(self.width):
                if random.random() < ratio:
                    self.set_obstacle(x,y,True)

    def to_dict(self):
        cells = []
        for y in range(self.height):
            for x in range(self.width):
                cells.append({'x':x,'y':y,'type':self.cells[y][x].type.value})
        return {'width':self.width,'height':self.height,'cells':cells}
