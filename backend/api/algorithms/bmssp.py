import heapq
import time
from .grid import Grid, CellType

def bmssp_search(grid: Grid, allow_diagonal=False):
    """
    Algoritmo Bounded Multi-Source Shortest Path (BMSSP) minimal y funcional.
    Busca el camino más corto desde el inicio al objetivo usando varios puntos pivote.
    """
    start = grid.start
    goal = grid.goal
    resultado = {
        'algoritmo': 'BMSSP',
        'exito': False,
        'costo': None,
        'tiempoMicrosegundos': 0,
        'nodosExpandidos': 0,
        'camino': [],
        'nodosVisitados': [],
        'frontera': []
    }

    if start == (-1, -1) or goal == (-1, -1):
        return resultado

    t0 = time.time()

    # Reset grid state
    for y in range(grid.height):
        for x in range(grid.width):
            grid.cells[y][x].g = float('inf')
            grid.cells[y][x].parent = (-1, -1)
            grid.cells[y][x].visited = False

    # Multi-source: inicio y vecinos
    sx, sy = start
    sources = [start]
    for dx in [-1, 0, 1]:
        for dy in [-1, 0, 1]:
            if dx == 0 and dy == 0:
                continue
            nx, ny = sx + dx, sy + dy
            if 0 <= nx < grid.width and 0 <= ny < grid.height and grid.is_walkable(nx, ny):
                sources.append((nx, ny))

    # Pivotes: esquinas y centro
    pivotes = [start, goal]
    pivotes += [(0, 0), (grid.width-1, 0), (0, grid.height-1), (grid.width-1, grid.height-1)]
    pivotes += [(grid.width//2, grid.height//2)]
    pivotes = [p for p in pivotes if grid.is_walkable(p[0], p[1])]

    # Inicialización heap
    heap = []
    for s in sources + pivotes:
        grid.cells[s[1]][s[0]].g = 0 if s == start else float('inf')
        heapq.heappush(heap, (grid.cells[s[1]][s[0]].g, s))

    visitados = set()

    while heap:
        costo, actual = heapq.heappop(heap)
        x, y = actual
        if actual in visitados:
            continue
        visitados.add(actual)
        grid.cells[y][x].visited = True
        resultado['nodosVisitados'].append(actual)
        resultado['nodosExpandidos'] += 1
        if actual == goal:
            resultado['exito'] = True
            resultado['costo'] = costo
            # reconstruir camino
            camino = []
            cur = actual
            while cur != (-1, -1):
                camino.append(cur)
                cur = grid.cells[cur[1]][cur[0]].parent
            camino.reverse()
            resultado['camino'] = camino
            for cx, cy in camino:
                grid.cells[cy][cx].type = CellType.PATH
            break
        for nx, ny in grid.get_neighbors(x, y, allow_diagonal):
            if not grid.is_walkable(nx, ny):
                continue
            nuevo_costo = costo + grid.get_distance(x, y, nx, ny)
            if nuevo_costo < grid.cells[ny][nx].g:
                grid.cells[ny][nx].g = nuevo_costo
                grid.cells[ny][nx].parent = (x, y)
                heapq.heappush(heap, (nuevo_costo, (nx, ny)))
                resultado['frontera'].append((nx, ny))

    t1 = time.time()
    resultado['tiempoMicrosegundos'] = int((t1 - t0) * 1e6)
    return resultado