from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
from typing import List, Optional, Literal
import time
import logging
from algorithms.grid import Grid, CellType
from algorithms.astar import astar_search
from algorithms.dijkstra import dijkstra_search
from algorithms.bmssp import bmssp_search
from algorithms.dstar_lite import dstar_lite_search

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

app = FastAPI(
    title='Pathfinding Visualizer API',
    description='API para visualización de algoritmos de pathfinding en tiempo real',
    version='2.0.0'
)

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)


class CellSchema(BaseModel):
    x: int
    y: int
    type: Literal['empty', 'obstacle', 'start', 'goal']


class GridSchema(BaseModel):
    width: int
    height: int
    cells: List[CellSchema]


class RunRequest(BaseModel):
    algorithms: List[Literal['astar','dijkstra','bmssp','dstar']]
    grid: GridSchema
    allowDiagonal: Optional[bool] = False


class AlgorithmResult(BaseModel):
    algoritmo: str
    exito: bool
    costo: Optional[float] = None
    tiempoMicrosegundos: Optional[int] = None
    nodosExpandidos: Optional[int] = None
    nodosGenerados: Optional[int] = None
    profundidad: Optional[int] = None
    factorRamificacion: Optional[float] = None
    camino: List[List[int]] = []
    nodosVisitados: List[List[int]] = []
    frontera: List[List[int]] = []
    tiempoEjecucion: Optional[str] = None
    eficiencia: Optional[str] = None


@app.get('/api/health')
def health():
    return {
        'status': 'ok',
        'version': '2.0.0',
        'algorithms': ['astar', 'dijkstra', 'bmssp', 'dstar'],
        'features': ['real_time_visualization', 'animated_paths', 'performance_metrics']
    }

@app.get('/api/algorithms')
def get_algorithms():
    """Obtiene información sobre los algoritmos disponibles"""
    return {
        'algorithms': {
            'astar': {
                'name': 'A*',
                'description': 'Algoritmo de búsqueda heurística óptimo',
                'complexity': 'O(b^d)',
                'optimal': True,
                'complete': True
            },
            'dijkstra': {
                'name': 'Dijkstra',
                'description': 'Algoritmo de camino más corto',
                'complexity': 'O((V+E) log V)',
                'optimal': True,
                'complete': True
            },
            'bmssp': {
                'name': 'BMSSP',
                'description': 'Búsqueda múltiple de caminos más cortos',
                'complexity': 'O(k(V+E) log V)',
                'optimal': True,
                'complete': True
            },
            'dstar': {
                'name': 'D* Lite',
                'description': 'Algoritmo adaptativo para entornos dinámicos',
                'complexity': 'O(log V)',
                'optimal': True,
                'complete': True
            }
        }
    }


@app.post('/api/run', response_model=List[AlgorithmResult])
def run_algorithms(req: RunRequest):
    """Ejecuta los algoritmos seleccionados y retorna resultados detallados"""
    
    def build_grid_from_request():
        """Construye el grid desde la petición del frontend"""
        g = Grid(req.grid.width, req.grid.height)
        for c in req.grid.cells:
            if c.type == 'obstacle':
                g.set_obstacle(c.x, c.y, True)
            elif c.type == 'start':
                g.set_start(c.x, c.y)
            elif c.type == 'goal':
                g.set_goal(c.x, c.y)
        return g

    def calculate_metrics(res):
        """Calcula métricas adicionales para el algoritmo"""
        nodos_visitados = len(res.get('nodosVisitados', []))
        nodos_expandidos = res.get('nodosExpandidos', nodos_visitados)
        profundidad = len(res.get('camino', []))
        
        # Factor de ramificación promedio
        factor_ramificacion = nodos_expandidos / max(profundidad, 1) if profundidad > 0 else 0
        
        # Eficiencia del algoritmo
        if nodos_visitados > 0 and profundidad > 0:
            eficiencia = (profundidad / nodos_visitados) * 100
            if eficiencia > 50:
                eficiencia_str = "Excelente"
            elif eficiencia > 25:
                eficiencia_str = "Buena"
            elif eficiencia > 10:
                eficiencia_str = "Regular"
            else:
                eficiencia_str = "Baja"
        else:
            eficiencia_str = "No disponible"
        
        return {
            'nodosGenerados': nodos_visitados + len(res.get('frontera', [])),
            'profundidad': profundidad,
            'factorRamificacion': round(factor_ramificacion, 2),
            'eficiencia': eficiencia_str
        }

    logger.info(f"Ejecutando algoritmos: {req.algorithms}")
    results = []
    
    for alg in req.algorithms:
        try:
            # Crear un grid fresco para cada algoritmo
            grid = build_grid_from_request()
            
            # Medir tiempo de ejecución
            start_time = time.time()
            
            if alg == 'astar':
                res = astar_search(grid, req.allowDiagonal)
            elif alg == 'dijkstra':
                res = dijkstra_search(grid, req.allowDiagonal)
            elif alg == 'bmssp':
                res = bmssp_search(grid, req.allowDiagonal)
            elif alg == 'dstar':
                res = dstar_lite_search(grid, req.allowDiagonal)
            else:
                raise HTTPException(status_code=400, detail=f'Algoritmo desconocido: {alg}')
            
            # Calcular tiempo de ejecución
            execution_time = time.time() - start_time
            tiempo_microsegundos = int(execution_time * 1_000_000)
            
            # Calcular métricas adicionales
            metrics = calculate_metrics(res)
            
            # Formatear tiempo de ejecución
            if execution_time < 0.001:
                tiempo_str = f"{tiempo_microsegundos} μs"
            elif execution_time < 1:
                tiempo_str = f"{execution_time * 1000:.1f} ms"
            else:
                tiempo_str = f"{execution_time:.3f} s"
            
            # Crear resultado enriquecido
            result = AlgorithmResult(
                algoritmo=res.get('algoritmo', alg.upper()),
                exito=res.get('exito', False),
                costo=res.get('costo'),
                tiempoMicrosegundos=tiempo_microsegundos,
                nodosExpandidos=res.get('nodosExpandidos'),
                nodosGenerados=metrics['nodosGenerados'],
                profundidad=metrics['profundidad'],
                factorRamificacion=metrics['factorRamificacion'],
                camino=[[x,y] for (x,y) in res.get('camino',[])],
                nodosVisitados=[[x,y] for (x,y) in res.get('nodosVisitados',[])],
                frontera=[[x,y] for (x,y) in res.get('frontera',[])],
                tiempoEjecucion=tiempo_str,
                eficiencia=metrics['eficiencia']
            )
            
            results.append(result)
            logger.info(f"Algoritmo {alg} completado: {result.exito}, tiempo: {tiempo_str}")
            
        except Exception as e:
            logger.error(f"Error ejecutando algoritmo {alg}: {str(e)}")
            # Crear resultado de error
            error_result = AlgorithmResult(
                algoritmo=alg.upper(),
                exito=False,
                tiempoEjecucion="Error",
                eficiencia="Error"
            )
            results.append(error_result)
    
    logger.info(f"Ejecución completada: {len(results)} algoritmos procesados")
    return results
