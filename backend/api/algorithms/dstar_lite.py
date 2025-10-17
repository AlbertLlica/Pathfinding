import heapq
import time
from .grid import Grid, CellType
class PriorityQueue:
    def __init__(self):
        self.elements = []
        self.entry_finder = {}
        self.REMOVED = object()
        self._counter = 0

    def add_or_update(self, node, priority):
        # priority is a tuple (k1,k2). Use a counter to avoid comparing nodes when priorities
        # are equal.
        if node in self.entry_finder:
            self.remove(node)
        entry = [priority, self._counter, node]
        self._counter += 1
        self.entry_finder[node] = entry
        heapq.heappush(self.elements, entry)

    def remove(self, node):
        entry = self.entry_finder.pop(node, None)
        if entry is not None:
            entry[-1] = self.REMOVED

    def pop(self):
        while self.elements:
            priority, _, node = heapq.heappop(self.elements)
            if node is not self.REMOVED:
                self.entry_finder.pop(node, None)
                return node, priority
        raise KeyError('pop from an empty priority queue')

    def top_key(self):
        while self.elements:
            priority, _, node = self.elements[0]
            if node is self.REMOVED:
                heapq.heappop(self.elements)
            else:
                return priority
        return (float('inf'), float('inf'))

    def empty(self):
        return len(self.entry_finder) == 0


def heuristic(a, b):
    return abs(a[0] - b[0]) + abs(a[1] - b[1])


def a_star_trace(grid: Grid, allow_diagonal=False):
    """Run A* on the provided grid and return a result dict (same contract as other algos)."""
    start = grid.start
    goal = grid.goal
    resultado = {
        'algoritmo': 'A* (internal)',
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
    sx, sy = start
    gx, gy = goal

    open_heap = []
    grid.cells[sy][sx].g = 0
    grid.cells[sy][sx].f = grid.get_heuristic(sx, sy, gx, gy)
    heapq.heappush(open_heap, (grid.cells[sy][sx].f, (sx, sy)))

    closed = set()

    while open_heap:
        f, (x, y) = heapq.heappop(open_heap)
        if (x, y) in closed:
            continue
        closed.add((x, y))
        grid.cells[y][x].visited = True
        resultado['nodosVisitados'].append((x, y))
        resultado['nodosExpandidos'] += 1

        if (x, y) == (gx, gy):
            # reconstruct
            path = []
            cx, cy = x, y
            while (cx, cy) != (-1, -1):
                path.append((cx, cy))
                px, py = grid.cells[cy][cx].parent
                cx, cy = px, py
            path.reverse()
            resultado['camino'] = path
            resultado['exito'] = True
            resultado['costo'] = grid.cells[gy][gx].g
            break

        for nx, ny in grid.get_neighbors(x, y, allow_diagonal):
            if not grid.is_walkable(nx, ny) or (nx, ny) in closed:
                continue
            tentative_g = grid.cells[y][x].g + grid.get_distance(x, y, nx, ny)
            cell = grid.cells[ny][nx]
            if tentative_g < cell.g:
                cell.g = tentative_g
                cell.parent = (x, y)
                cell.f = tentative_g + grid.get_heuristic(nx, ny, gx, gy)
                heapq.heappush(open_heap, (cell.f, (nx, ny)))
                if cell.type != CellType.VISITED:
                    resultado['frontera'].append((nx, ny))

    t1 = time.time()
    resultado['tiempoMicrosegundos'] = int((t1 - t0) * 1e6)
    return resultado


def dstar_lite_search(grid: Grid, allow_diagonal=False, simulate_discoveries=True):
    """Full D* Lite implementation adapted for grid graphs.

    If simulate_discoveries is True, after the initial shortest path is found
    we deterministically mark a few cells along the path as newly discovered
    obstacles (to trigger replanning) so the frontend gets a visible
    incremental replan trace. If False, the function performs a single run
    of the algorithm to compute the shortest path under the current map.
    """
    start = grid.start
    goal = grid.goal
    resultado = {
        'algoritmo': 'D* Lite',
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

    width = grid.width
    height = grid.height

    # utility functions
    def cost(u, v):
        # infinite cost if v is obstacle
        if not grid.is_walkable(v[0], v[1]):
            return float('inf')
        return grid.get_distance(u[0], u[1], v[0], v[1])

    def successors(u):
        return grid.get_neighbors(u[0], u[1], allow_diagonal)

    def predecessors(u):
        return grid.get_neighbors(u[0], u[1], allow_diagonal)

    # initialize
    g = { (x,y): float('inf') for y in range(height) for x in range(width) }
    rhs = { (x,y): float('inf') for y in range(height) for x in range(width) }
    km = 0.0
    queue = PriorityQueue()

    def calculate_key(s, start_coord):
        val = min(g[s], rhs[s])
        return (val + heuristic(start_coord, s) + km, val)

    def update_vertex(u, start_coord):
        if u != goal:
            rhs[u] = min((cost(u, s) + g[s]) for s in successors(u))
        queue.remove(u)
        if g[u] != rhs[u]:
            queue.add_or_update(u, calculate_key(u, start_coord))

    def compute_shortest_path(start_coord):
        # expands nodes until consistency wrt start
        while queue.top_key() < calculate_key(start_coord, start_coord) or rhs[start_coord] != g[start_coord]:
            u, k_old = queue.pop()
            # record expansion
            ux, uy = u
            if not grid.cells[uy][ux].visited:
                grid.cells[uy][ux].visited = True
                resultado['nodosVisitados'].append(u)
            resultado['nodosExpandidos'] += 1

            if g[u] > rhs[u]:
                g[u] = rhs[u]
            else:
                g[u] = float('inf')
                update_vertex(u, start_coord)
            for s in predecessors(u):
                update_vertex(s, start_coord)

    # set rhs(goal)=0 and push goal
    rhs[goal] = 0.0
    queue.add_or_update(goal, calculate_key(goal, start))

    t0 = time.time()
    # initial planning
    compute_shortest_path(start)

    # reconstruct helper
    def reconstruct(start_coord):
        path = []
        current = start_coord
        if g.get(current, float('inf')) == float('inf'):
            return []
        while current != goal:
            path.append(current)
            # choose successor with minimal cost
            best = None
            best_cost = float('inf')
            for s in successors(current):
                c = cost(current, s) + g.get(s, float('inf'))
                if c < best_cost:
                    best_cost = c
                    best = s
            if best is None or best_cost == float('inf'):
                return []
            current = best
        path.append(goal)
        # write parents/g into grid cells for visualization
        for i in range(len(path)-1, -1, -1):
            x,y = path[i]
            grid.cells[y][x].g = g.get((x,y), float('inf'))
            if i > 0:
                px, py = path[i-1]
                grid.cells[y][x].parent = (px, py)
            else:
                grid.cells[y][x].parent = (-1, -1)
        return path

    initial_path = reconstruct(start)

    # Optionally simulate discoveries to force replanning (deterministic)
    if simulate_discoveries and initial_path:
        interval = max(2, len(initial_path) // 4)
        discoveries = []
        for i in range(interval, len(initial_path) - 1, interval):
            x, y = initial_path[i]
            # pick a walkable neighbor deterministically
            for nx, ny in grid.get_neighbors(x, y, allow_diagonal):
                if grid.is_walkable(nx, ny) and (nx, ny) != start and (nx, ny) != goal:
                    discoveries.append((nx, ny))
                    break

        # apply discoveries and replan
        for disc in discoveries:
            dx, dy = disc
            # mark obstacle
            grid.set_obstacle(dx, dy, True)
            # when map changes, we need to update rhs for affected nodes
            # For simplicity, update rhs of neighbors of changed cell
            affected = set()
            for n in grid.get_neighbors(dx, dy, allow_diagonal):
                affected.add(n)
            # update rhs for affected nodes and push them
            for u in affected:
                update_vertex(u, start)
            # re-run planning
            compute_shortest_path(start)
            # reconstruct candidate path
            p = reconstruct(start)
            if p:
                resultado['camino'] = p
                resultado['exito'] = True
                resultado['costo'] = g.get(start, None)

    else:
        resultado['camino'] = initial_path
        resultado['exito'] = len(initial_path) > 0
        resultado['costo'] = g.get(start, None)

    t1 = time.time()
    resultado['tiempoMicrosegundos'] = int((t1 - t0) * 1e6)
    # frontier: current queue entries
    resultado['frontera'] = [k for k in queue.entry_finder.keys()]
    return resultado
