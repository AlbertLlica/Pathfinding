class PathfindingVisualizer {
    constructor() {
        // We'll compute grid dimensions so the full grid fits in the visualization area
        this.gridWidth = 60; // initial hint, will be adjusted
        this.gridHeight = 36; // initial hint
        this.cellSize = 16; // base cell size in px (used as fallback)
        this.grid = [];
        this.mode = 'navigate'; // 'navigate', 'setStart', 'setGoal'
        this.animationSpeed = 50; // ms per cell (faster)
        this.pathDuration = 1000; // ms for path animation (faster)
        this.isRunning = false;
        this.animationQueue = [];
        this.currentAnimations = new Set();
        this.executionStartTime = null;
        this.individualViewMode = false;
        this.currentAlgorithmIndex = 0;
        
        this.initializeGrid();
        this.setupEventListeners();
        this.loadGrid();
    }
    
    initializeGrid() {
        const gridContainer = document.getElementById('grid');
        gridContainer.innerHTML = '';
    // compute cellSize to make grid fit available container without scroll
    const parentRect = gridContainer.parentElement.getBoundingClientRect();
    const maxWidth = Math.min(parentRect.width - 24, 1100); // keep reasonable
    const maxHeight = Math.max(parentRect.height - 24, 200);

    // try to keep aspect close to initial ratio, but ensure cells are >=8px
    const tentativeCellW = Math.floor(maxWidth / this.gridWidth);
    const tentativeCellH = Math.floor(maxHeight / this.gridHeight);
    this.cellSize = Math.max(8, Math.min(tentativeCellW || tentativeCellH, tentativeCellH || tentativeCellW));

    // expose animation speed and computed cell size / columns to CSS to keep animations and layout in sync
    document.documentElement.style.setProperty('--animation-ms', `${this.animationSpeed}ms`);
    document.documentElement.style.setProperty('--cell-size', `${this.cellSize}px`);
    document.documentElement.style.setProperty('--columns', `${this.gridWidth}`);
        
        this.grid = [];
        for (let y = 0; y < this.gridHeight; y++) {
            this.grid[y] = [];
            for (let x = 0; x < this.gridWidth; x++) {
                const cell = document.createElement('div');
                cell.className = 'cell empty';
                cell.dataset.x = x;
                cell.dataset.y = y;
                
                // During runtime we still want clicks to toggle obstacles
                cell.addEventListener('click', (e) => this.handleCellClick(e));
                cell.addEventListener('contextmenu', (e) => this.handleCellRightClick(e));
                
                gridContainer.appendChild(cell);
                this.grid[y][x] = cell;
            }
        }
        // Update grid-size UI
        document.getElementById('grid-size').textContent = `${this.gridWidth}×${this.gridHeight}`;
    }
    
    setupEventListeners() {
        // Control buttons
        document.getElementById('set-start').addEventListener('click', () => this.setMode('setStart'));
        document.getElementById('set-goal').addEventListener('click', () => this.setMode('setGoal'));
        document.getElementById('clear-path').addEventListener('click', () => this.clearPath());
        document.getElementById('clear-all').addEventListener('click', () => this.clearAll());
        document.getElementById('random-obstacles').addEventListener('click', () => this.generateRandomObstacles());
        document.getElementById('toggle-individual').addEventListener('click', () => this.toggleIndividualView());
        
        // Algorithm buttons / multi-run
        document.getElementById('run-selected').addEventListener('click', () => this.runSelected());
        
        // Speed control
        document.getElementById('speed').addEventListener('input', (e) => {
            this.animationSpeed = parseInt(e.target.value);
            document.getElementById('speed-value').textContent = `${this.animationSpeed} ms`;
            document.documentElement.style.setProperty('--animation-ms', `${this.animationSpeed}ms`);
        });
        
        // Path duration control
        document.getElementById('pathDuration').addEventListener('input', (e) => {
            this.pathDuration = parseInt(e.target.value) * 1000;
            document.getElementById('path-duration-value').textContent = `${e.target.value}s`;
        });
        
        // Diagonal toggle
        document.getElementById('diagonal').addEventListener('change', (e) => {
            // This would be sent to backend
            console.log('Diagonal movement:', e.target.checked);
        });
    }
    
    setMode(newMode) {
        this.mode = newMode;
        const modeElement = document.getElementById('mode');
        
        switch(newMode) {
            case 'setStart':
                modeElement.textContent = 'Establecer Inicio';
                break;
            case 'setGoal':
                modeElement.textContent = 'Establecer Meta';
                break;
            default:
                modeElement.textContent = 'Navegación';
        }
        
        // Update button states
        document.querySelectorAll('.btn').forEach(btn => btn.classList.remove('active'));
        if (newMode !== 'navigate') {
            document.getElementById(`set-${newMode === 'setStart' ? 'start' : 'goal'}`).classList.add('active');
        }
    }
    
    handleCellClick(event) {
        event.preventDefault();
        const cell = event.target;
        const x = parseInt(cell.dataset.x);
        const y = parseInt(cell.dataset.y);
        
        if (this.mode === 'setStart') {
            this.setStart(x, y);
            this.setMode('navigate');
        } else if (this.mode === 'setGoal') {
            this.setGoal(x, y);
            this.setMode('navigate');
        } else {
            // allow toggling obstacles even while algorithm runs
            this.toggleObstacle(x, y);
        }
    }
    
    handleCellRightClick(event) {
        event.preventDefault();
        const cell = event.target;
        const x = parseInt(cell.dataset.x);
        const y = parseInt(cell.dataset.y);
        
        // Right click to set start/goal
        if (cell.classList.contains('start')) {
            this.setGoal(x, y);
        } else if (cell.classList.contains('goal')) {
            this.setStart(x, y);
        } else {
            this.setStart(x, y);
        }
    }
    
    setStart(x, y) {
        // Clear existing start
        document.querySelectorAll('.cell.start').forEach(cell => {
            cell.classList.remove('start');
            cell.classList.add('empty');
            cell.style.background = '';
            cell.style.border = '';
        });
        
        // Set new start with special color
        const cell = this.grid[y][x];
        if (!cell.classList.contains('goal')) {
            cell.classList.remove('empty', 'obstacle');
            cell.classList.add('start');
            cell.style.background = '#17becf';
            cell.style.border = '2px solid #17becf';
            this.updateStatus(`Inicio establecido en (${x}, ${y})`);
        }
    }
    
    setGoal(x, y) {
        // Clear existing goal
        document.querySelectorAll('.cell.goal').forEach(cell => {
            cell.classList.remove('goal');
            cell.classList.add('empty');
            cell.style.background = '';
            cell.style.border = '';
        });
        
        // Set new goal with special color
        const cell = this.grid[y][x];
        if (!cell.classList.contains('start')) {
            cell.classList.remove('empty', 'obstacle');
            cell.classList.add('goal');
            cell.style.background = '#d62728';
            cell.style.border = '2px solid #d62728';
            this.updateStatus(`Meta establecida en (${x}, ${y})`);
        }
    }
    
    toggleObstacle(x, y) {
        const cell = this.grid[y][x];
        if (cell.classList.contains('start') || cell.classList.contains('goal')) {
            return;
        }
        
        if (cell.classList.contains('obstacle')) {
            cell.classList.remove('obstacle');
            cell.classList.add('empty');
        } else {
            cell.classList.remove('empty');
            cell.classList.add('obstacle');
        }
    }
    
    clearPath() {
        const toClear = Array.from(document.querySelectorAll('.cell')).filter(cell =>
            cell.classList.contains('visited') ||
            cell.classList.contains('frontier') ||
            cell.classList.contains('path') ||
            cell.classList.contains('alg-0') ||
            cell.classList.contains('alg-1') ||
            cell.classList.contains('alg-2') ||
            cell.classList.contains('alg-3')
        );

        if (toClear.length === 0) {
            this.updateStatus('No hay camino que limpiar');
            return;
        }

        // Clear immediately without animation
            toClear.forEach(cell => {
            // Remove all algorithm-related classes
            cell.classList.remove('visited', 'frontier', 'path', 'alg-0', 'alg-1', 'alg-2', 'alg-3');
                cell.classList.add('empty');
                cell.style.transition = '';
            cell.style.background = '';
            cell.style.boxShadow = '';
            cell.style.border = '';
            cell.style.transform = '';
            cell.style.opacity = '';
            cell.style.setProperty('--visited-color', '');
            cell.style.setProperty('--path-color', '');
        });

            this.updateStatus('Camino limpiado');
    }
    
    clearAll() {
        const ok = window.confirm('¿Estás seguro? Esto borrará obstáculos, inicio y meta.');
        if (!ok) return;

        document.querySelectorAll('.cell').forEach(cell => {
            cell.className = 'cell empty';
            cell.style = '';
        });
        this.updateStatus('Grid limpiado');
    }
    
    generateRandomObstacles() {
        this.clearAll();
        
        for (let y = 0; y < this.gridHeight; y++) {
            for (let x = 0; x < this.gridWidth; x++) {
                if (Math.random() < 0.3) { // 30% chance
                    this.toggleObstacle(x, y);
                }
            }
        }
        this.updateStatus('Obstáculos aleatorios generados');
    }

    toggleIndividualView() {
        this.individualViewMode = !this.individualViewMode;
        const button = document.getElementById('toggle-individual');
        
        if (this.individualViewMode) {
            button.innerHTML = '<span class="btn-icon">👥</span> Vista Simultánea';
            button.classList.add('active');
            this.updateStatus('Modo vista individual activado');
        } else {
            button.innerHTML = '<span class="btn-icon">👁️</span> Vista Individual';
            button.classList.remove('active');
            this.updateStatus('Modo vista simultánea activado');
        }
    }

    showIndividualAlgorithm(algorithmIndex, results) {
        if (algorithmIndex >= results.length) return;
        
        const res = results[algorithmIndex];
        const algorithmColors = {
            'Dijkstra': {
                visited: '#1f77b4',
                visitedTransparent: 'rgba(31, 119, 180, 0.3)',
                path: '#1f77b4',
                name: 'Dijkstra'
            },
            'A*': {
                visited: '#ff7f0e',
                visitedTransparent: 'rgba(255, 127, 14, 0.3)',
                path: '#ff7f0e',
                name: 'A*'
            },
            'BMSSP': {
                visited: '#9467bd',
                visitedTransparent: 'rgba(148, 103, 189, 0.3)',
                path: '#9467bd',
                name: 'BMSSP'
            },
            'D* Lite': {
                visited: '#2ca02c',
                visitedTransparent: 'rgba(44, 160, 44, 0.3)',
                path: '#2ca02c',
                name: 'D* Lite'
            }
        };
        
        const colors = algorithmColors[res.algoritmo] || { 
            visited: '#6b7280', 
            visitedTransparent: 'rgba(107, 114, 128, 0.3)',
            path: '#374151', 
            name: res.algoritmo 
        };
        
        // Clear all algorithm visualizations first
        this.clearPath();
        
        // Show only this algorithm
        this.animateVisitedNodesSimultaneous(res.nodosVisitados, colors, `alg-${algorithmIndex}`).then(() => {
            if (res.camino && res.camino.length > 0) {
                return this.animatePathSimultaneous(res.camino, colors, `alg-${algorithmIndex}`);
            }
        });
        
        // Highlight the corresponding result box
        const resultBoxes = document.querySelectorAll('.result-item');
        resultBoxes.forEach((box, index) => {
            if (index === algorithmIndex) {
                box.style.transform = 'scale(1.02)';
                box.style.boxShadow = '0 8px 25px rgba(0,0,0,0.15)';
            } else {
                box.style.transform = '';
                box.style.boxShadow = '';
                box.style.opacity = '0.6';
            }
        });
        
        this.updateStatus(`Mostrando: ${colors.name}`);
    }

    showAllAlgorithms(results) {
        // Reset all result boxes
        const resultBoxes = document.querySelectorAll('.result-item');
        resultBoxes.forEach(box => {
            box.style.transform = '';
            box.style.boxShadow = '';
            box.style.opacity = '1';
        });
        
        this.updateStatus('Mostrando todos los algoritmos');
    }
    
    async runAlgorithm(algorithm) {
        if (this.isRunning) {
            this.updateStatus('Algoritmo ya en ejecución (puedes editar obstáculos)');
            // allow running same algorithm twice? we simply continue
        }
        
        const startCell = document.querySelector('.cell.start');
        const goalCell = document.querySelector('.cell.goal');
        
        if (!startCell || !goalCell) {
            this.updateStatus('Establece inicio y meta primero');
            return;
        }
        
        this.isRunning = true;
        this.clearPath();
        this.updateStatus(`Ejecutando ${algorithm}...`);
        
        // Disable algorithm buttons
        document.querySelectorAll('.btn-algorithm').forEach(btn => btn.disabled = true);
        
        try {
            // Simulate algorithm execution with animation
            await this.simulateAlgorithm(algorithm);
            this.updateStatus(`${algorithm} completado`);
        } catch (error) {
            this.updateStatus(`Error ejecutando ${algorithm}`);
            console.error(error);
        } finally {
            this.isRunning = false;
            document.querySelectorAll('.btn-algorithm').forEach(btn => btn.disabled = false);
        }
    }

    // New: run multiple selected algorithms by calling backend
    async runSelected() {
        if (this.isRunning) return;
        const checks = Array.from(document.querySelectorAll('.alg-check'));
        const selected = checks.filter(c => c.checked).map(c => c.value);
        if (selected.length === 0) {
            this.updateStatus('Selecciona al menos un algoritmo');
            return;
        }

        const startCell = document.querySelector('.cell.start');
        const goalCell = document.querySelector('.cell.goal');
        if (!startCell || !goalCell) {
            this.updateStatus('Establece inicio y meta primero');
            return;
        }

        this.isRunning = true;
        this.executionStartTime = Date.now();
        this.clearPath();
        this.updateStatus('Ejecutando algoritmos seleccionados...');
        
        // Reset execution time display
        document.getElementById('execution-time').textContent = '--';

        const payload = {
            algorithms: selected,
            allowDiagonal: document.getElementById('diagonal').checked,
            grid: this.getGridData()
        };

        try {
            // Show loading state
            const runButton = document.getElementById('run-selected');
            const originalText = runButton.innerHTML;
            runButton.innerHTML = '<span class="btn-icon">⏳</span> Ejecutando...';
            runButton.disabled = true;
            
            const resp = await fetch('http://127.0.0.1:8000/api/run', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(payload)
            });
            
            if (!resp.ok) throw new Error(`API error: ${resp.status}`);
            const results = await resp.json();
            
            // Update status and start animations
            this.updateStatus('Animando resultados...');
            await this.renderResults(results);
            
            // Calculate total execution time
            const totalTime = Date.now() - this.executionStartTime;
            document.getElementById('execution-time').textContent = `${(totalTime / 1000).toFixed(1)}s`;
            
            this.updateStatus('Ejecuciones completadas');
            
        } catch (err) {
            console.error(err);
            this.updateStatus('Error al llamar al backend');
            document.getElementById('execution-time').textContent = 'Error';
        } finally {
            this.isRunning = false;
            this.executionStartTime = null;
            
            // Restore button
            const runButton = document.getElementById('run-selected');
            runButton.innerHTML = '<span class="btn-icon">▶️</span> Ejecutar Algoritmos';
            runButton.disabled = false;
        }
    }

    async renderResults(results) {
        // Clear any previous path/visited classes (but keep obstacles/start/goal)
        document.querySelectorAll('.cell').forEach(cell => {
            cell.classList.remove('visited','frontier','path','alg-path');
            cell.style.background = '';
            cell.style.opacity = '';
            cell.style.setProperty('--visited-color', '');
            cell.style.setProperty('--path-color', '');
        });

        const algorithmColors = {
            'Dijkstra': {
                visited: '#1f77b4',
                visitedTransparent: 'rgba(31, 119, 180, 0.3)',
                path: '#1f77b4',
                name: 'Dijkstra'
            },
            'A*': {
                visited: '#ff7f0e',
                visitedTransparent: 'rgba(255, 127, 14, 0.3)',
                path: '#ff7f0e',
                name: 'A*'
            },
            'BMSSP': {
                visited: '#9467bd',
                visitedTransparent: 'rgba(148, 103, 189, 0.3)',
                path: '#9467bd',
                name: 'BMSSP'
            },
            'D* Lite': {
                visited: '#2ca02c',
                visitedTransparent: 'rgba(44, 160, 44, 0.3)',
                path: '#2ca02c',
                name: 'D* Lite'
            }
        };

        const specialColors = {
            start: '#17becf',
            goal: '#d62728'
        };

        const resultsContainer = document.getElementById('results');
        resultsContainer.innerHTML = '';

        // Create result boxes for all algorithms first
        const algorithmPromises = results.map((res, index) => {
            const colors = algorithmColors[res.algoritmo] || { 
                visited: '#6b7280', 
                visitedTransparent: 'rgba(107, 114, 128, 0.3)',
                path: '#374151', 
                name: res.algoritmo 
            };
            
            // Add result box with enhanced metrics
            const box = document.createElement('div');
            box.className = 'result-item';
            box.style.borderLeftColor = colors.path;
            
            const successIcon = res.exito ? '✅' : '❌';
            const costo = res.costo ? res.costo.toFixed(2) : '-';
            const tiempo = res.tiempoEjecucion || '-';
            const nodosExp = res.nodosExpandidos || '-';
            const profundidad = res.profundidad || '-';
            const eficiencia = res.eficiencia || '-';
            
            box.innerHTML = `
                <div class="result-algorithm" style="color: ${colors.path}">${colors.name}</div>
                <div class="result-stats">
                    <div class="stat-row">
                        <span class="stat-item">${successIcon} Éxito</span>
                        <span class="stat-item">💰 ${costo}</span>
                        <span class="stat-item">⏱️ ${tiempo}</span>
                    </div>
                    <div class="stat-row">
                        <span class="stat-item">🔍 ${nodosExp} nodos</span>
                        <span class="stat-item">📏 ${profundidad} pasos</span>
                        <span class="stat-item">⚡ ${eficiencia}</span>
                    </div>
                </div>
            `;
            
            // Add click handler for individual view
            if (this.individualViewMode) {
                box.style.cursor = 'pointer';
                box.addEventListener('click', () => this.showIndividualAlgorithm(index, results));
            }
            
            resultsContainer.appendChild(box);
            
            // Return the algorithm data with colors for simultaneous execution
            return { res, colors, index };
        });

        // Execute algorithms based on view mode
        if (this.individualViewMode) {
            this.updateStatus('Modo vista individual: ejecutando secuencialmente...');
            
            // Execute algorithms one by one for individual view
            for (let i = 0; i < algorithmPromises.length; i++) {
                const { res, colors, index } = algorithmPromises[i];
                this.showIndividualAlgorithm(index, results);
                await this.executeAlgorithmSimultaneously(res, colors, index);
                
                // Small delay between algorithms
                if (i < algorithmPromises.length - 1) {
                    await this.sleep(500);
                }
            }
        } else {
            this.updateStatus('Ejecutando algoritmos simultáneamente...');
            
            const allPromises = algorithmPromises.map(({ res, colors, index }) => {
                return this.executeAlgorithmSimultaneously(res, colors, index);
            });
            
            // Wait for all algorithms to complete
            await Promise.all(allPromises);
        }
        
        this.updateStatus('Todas las ejecuciones completadas');
    }

    async executeAlgorithmSimultaneously(res, colors, algorithmIndex) {
        // Add algorithm identifier to cells to avoid conflicts
        const algorithmId = `alg-${algorithmIndex}`;
        
        // Animate visited nodes
        await this.animateVisitedNodesSimultaneous(res.nodosVisitados, colors, algorithmId);
        
        // Animate path
        if (res.camino && res.camino.length > 0) {
            await this.animatePathSimultaneous(res.camino, colors, algorithmId);
        }
    }

    async animateVisitedNodesSimultaneous(nodes, colors, algorithmId) {
        const visitedNodes = nodes.filter(([x, y]) => {
            const cell = this.grid[y]?.[x];
            return cell && !cell.classList.contains('start') && 
                   !cell.classList.contains('goal') && 
                   !cell.classList.contains('obstacle');
        });

        // Faster animation - larger waves, shorter delays
        const waveSize = Math.max(1, Math.floor(visitedNodes.length / 8));
        for (let i = 0; i < visitedNodes.length; i += waveSize) {
            const wave = visitedNodes.slice(i, i + waveSize);
            
            // Animate wave simultaneously with shorter delays
            const promises = wave.map(([x, y], index) => {
                return new Promise(resolve => {
                    setTimeout(() => {
                const cell = this.grid[y][x];
                        if (cell) {
                            cell.classList.add('visited', algorithmId);
                            cell.style.background = colors.visitedTransparent;
                            cell.style.border = `1px solid ${colors.visited}`;
                            resolve();
                        }
                    }, index * 10); // Reduced from 30ms to 10ms
                });
            });
            
            await Promise.all(promises);
            await this.sleep(this.animationSpeed / 2); // Faster wave progression
        }
    }

    async animatePathSimultaneous(path, colors, algorithmId) {
        if (!path || path.length === 0) return;

        // Filter path nodes (exclude start and goal)
        const pathNodes = path.filter(([x, y]) => {
            const cell = this.grid[y]?.[x];
            return cell && !cell.classList.contains('start') && !cell.classList.contains('goal');
        });

        if (pathNodes.length === 0) return;

        // Calculate delay per node to achieve desired duration (faster)
        const delayPerNode = Math.max(20, this.pathDuration / pathNodes.length); // Minimum 20ms per node
        
        // Animate path with smooth progression
        for (let i = 0; i < pathNodes.length; i++) {
            const [x, y] = pathNodes[i];
            const cell = this.grid[y][x];
            
            if (cell) {
                // Remove visited class and add path class with algorithm ID
                cell.classList.remove('visited');
                cell.classList.add('path', algorithmId);
                cell.style.background = colors.path;
                cell.style.border = `2px solid ${colors.path}`;
                
                // Add a subtle glow effect
                cell.style.boxShadow = `0 0 15px ${colors.path}60`;
                
                // Update execution time display
                if (this.executionStartTime) {
                    const elapsed = Date.now() - this.executionStartTime;
                    document.getElementById('execution-time').textContent = `${(elapsed / 1000).toFixed(1)}s`;
                }
                
                await this.sleep(delayPerNode);
            }
        }
        
        // Final cleanup - remove glow effects after animation
        setTimeout(() => {
            pathNodes.forEach(([x, y]) => {
                const cell = this.grid[y]?.[x];
                if (cell) {
                    cell.style.boxShadow = '';
                }
            });
        }, 1000);
    }
    
    async simulateAlgorithm(algorithm) {
        const startCell = document.querySelector('.cell.start');
        const goalCell = document.querySelector('.cell.goal');
        
        const startX = parseInt(startCell.dataset.x);
        const startY = parseInt(startCell.dataset.y);
        const goalX = parseInt(goalCell.dataset.x);
        const goalY = parseInt(goalCell.dataset.y);
        
        // Simple pathfinding simulation
        const visited = new Set();
        const path = [];
        
        // Simulate exploration
        const explore = async (x, y, parentX = -1, parentY = -1) => {
            if (x < 0 || x >= this.gridWidth || y < 0 || y >= this.gridHeight) return false;
            if (visited.has(`${x},${y}`)) return false;
            
            const cell = this.grid[y][x];
            if (cell.classList.contains('obstacle')) return false;
            
            visited.add(`${x},${y}`);
            
            if (!cell.classList.contains('start') && !cell.classList.contains('goal')) {
                cell.classList.remove('empty');
                cell.classList.add('visited');
            }
            
            // respect dynamic animation speed
            await this.sleep(this.animationSpeed);
            
            // Check if we reached the goal
            if (x === goalX && y === goalY) {
                path.push({x, y, parentX, parentY});
                return true;
            }
            
            // Explore neighbors (4-directional)
            const neighbors = [
                {x: x+1, y}, {x: x-1, y}, {x, y: y+1}, {x, y: y-1}
            ];
            
            for (const neighbor of neighbors) {
                if (await explore(neighbor.x, neighbor.y, x, y)) {
                    path.push({x, y, parentX, parentY});
                    return true;
                }
            }
            
            return false;
        };
        
        const found = await explore(startX, startY);
        
        if (found) {
            // Highlight the path
            for (const node of path) {
                const cell = this.grid[node.y][node.x];
                if (!cell.classList.contains('start') && !cell.classList.contains('goal')) {
                    cell.classList.remove('visited');
                    cell.classList.add('path');
                }
                await this.sleep(Math.max(60, this.animationSpeed / 1.5));
            }
        }
    }
    
    sleep(ms) {
        return new Promise(resolve => setTimeout(resolve, ms));
    }
    
    updateStatus(message) {
        const statusElement = document.getElementById('status');
        statusElement.textContent = message;
        
        // Add a subtle animation to status updates
        statusElement.style.transform = 'scale(1.05)';
        statusElement.style.transition = 'transform 200ms ease';
        
        setTimeout(() => {
            statusElement.style.transform = 'scale(1)';
        }, 200);
        
        console.log(message);
    }
    
    loadGrid() {
        // Load initial grid state from backend
        this.updateStatus('Cargando grid...');
        
        // Simulate loading
        setTimeout(() => {
            this.updateStatus('Grid cargado - Listo');
        }, 500);
    }
    
    // Utility methods for backend communication
    async fetchGrid() {
        try {
            const response = await fetch('/api/grid');
            const data = await response.json();
            return data;
        } catch (error) {
            console.error('Error fetching grid:', error);
            return null;
        }
    }
    
    async updateGrid() {
        try {
            const gridData = this.getGridData();
            const response = await fetch('/api/grid', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(gridData)
            });
            return response.ok;
        } catch (error) {
            console.error('Error updating grid:', error);
            return false;
        }
    }
    
    getGridData() {
        const data = {
            width: this.gridWidth,
            height: this.gridHeight,
            cells: []
        };
        
        for (let y = 0; y < this.gridHeight; y++) {
            for (let x = 0; x < this.gridWidth; x++) {
                const cell = this.grid[y][x];
                let type = 'empty';
                
                if (cell.classList.contains('start')) type = 'start';
                else if (cell.classList.contains('goal')) type = 'goal';
                else if (cell.classList.contains('obstacle')) type = 'obstacle';
                
                data.cells.push({x, y, type});
            }
        }
        
        return data;
    }
}

// Initialize the visualizer when the page loads
document.addEventListener('DOMContentLoaded', () => {
    new PathfindingVisualizer();
});
