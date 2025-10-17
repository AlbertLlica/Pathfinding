import heapq
import time
from .grid import Grid, CellType


def dijkstra_search(grid: Grid, allow_diagonal=False):
    start = grid.start
    goal = grid.goal
    resultado = {
        'algoritmo': 'Dijkstra',
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

    for y in range(grid.height):
        for x in range(grid.width):
            grid.cells[y][x].g = float('inf')

    grid.cells[sy][sx].g = 0
    heap = [(0, (sx, sy))]
    visited = set()

    while heap:
        dist, (x,y) = heapq.heappop(heap)
        if (x,y) in visited:
            continue
        visited.add((x,y))
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
            resultado['costo'] = dist
            break

        for nx, ny in grid.get_neighbors(x,y, allow_diagonal):
            if not grid.is_walkable(nx, ny) or (nx,ny) in visited:
                continue
            nd = dist + grid.get_distance(x,y,nx,ny)
            cell = grid.cells[ny][nx]
            if nd < cell.g:
                cell.g = nd
                cell.parent = (x,y)
                heapq.heappush(heap, (nd, (nx,ny)))
                # Only add to frontier if not already in visited set
                if (nx,ny) not in visited:
                    resultado['frontera'].append((nx,ny))

    t1 = time.time()
    resultado['tiempoMicrosegundos'] = int((t1-t0)*1e6)
    return resultado
