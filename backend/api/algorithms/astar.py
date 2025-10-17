import heapq
import time
from .grid import Grid, CellType


def astar_search(grid: Grid, allow_diagonal=False):
    start = grid.start
    goal = grid.goal
    resultado = {
        'algoritmo': 'A*',
        'exito': False,
        'costo': None,
        'tiempoMicrosegundos': 0,
        'nodosExpandidos': 0,
        'camino': [],
        'nodosVisitados': [],
        'frontera': []
    }
    if start == (-1,-1) or goal == (-1,-1):
        return resultado

    t0 = time.time()

    sx, sy = start
    gx, gy = goal

    open_heap = []
    grid.cells[sy][sx].g = 0
    grid.cells[sy][sx].f = grid.get_heuristic(sx, sy, gx, gy)
    heapq.heappush(open_heap, (grid.cells[sy][sx].f, (sx, sy)))

    closed = set()

    while open_heap:
        f, (x,y) = heapq.heappop(open_heap)
        if (x,y) in closed:
            continue
        closed.add((x,y))
        grid.cells[y][x].visited = True
        resultado['nodosVisitados'].append((x,y))
        resultado['nodosExpandidos'] += 1

        if (x,y) == (gx,gy):
            # reconstruct
            path = []
            cx, cy = x, y
            while (cx,cy) != (-1,-1):
                path.append((cx,cy))
                px, py = grid.cells[cy][cx].parent
                cx, cy = px, py
            path.reverse()
            resultado['camino'] = path
            resultado['exito'] = True
            resultado['costo'] = grid.cells[gy][gx].g
            break

        for nx, ny in grid.get_neighbors(x,y, allow_diagonal):
            if not grid.is_walkable(nx, ny) or (nx,ny) in closed:
                continue
            tentative_g = grid.cells[y][x].g + grid.get_distance(x,y,nx,ny)
            cell = grid.cells[ny][nx]
            if tentative_g < cell.g:
                cell.g = tentative_g
                cell.parent = (x,y)
                cell.f = tentative_g + grid.get_heuristic(nx,ny,gx,gy)
                heapq.heappush(open_heap, (cell.f, (nx,ny)))
                if cell.type != CellType.VISITED:
                    resultado['frontera'].append((nx,ny))

    t1 = time.time()
    resultado['tiempoMicrosegundos'] = int((t1-t0)*1e6)
    return resultado
