class PathfindingVisualizer {
    constructor() {
        this.gridWidth = 30;
        this.gridHeight = 20;
        this.cellSize = 15;
        this.grid = [];
        this.mode = 'navigate'; // 'navigate', 'setStart', 'setGoal'
        this.animationSpeed = 50;
        this.isRunning = false;
        
        this.initializeGrid();
        this.setupEventListeners();
        this.loadGrid();
    }
    
    initializeGrid() {
        const gridContainer = document.getElementById('grid');
        gridContainer.innerHTML = '';
        gridContainer.style.gridTemplateColumns = `repeat(${this.gridWidth}, ${this.cellSize}px)`;
        
        this.grid = [];
        for (let y = 0; y < this.gridHeight; y++) {
            this.grid[y] = [];
            for (let x = 0; x < this.gridWidth; x++) {
                const cell = document.createElement('div');
                cell.className = 'cell empty';
                cell.dataset.x = x;
                cell.dataset.y = y;
                
                cell.addEventListener('click', (e) => this.handleCellClick(e));
                cell.addEventListener('contextmenu', (e) => this.handleCellRightClick(e));
                
                gridContainer.appendChild(cell);
                this.grid[y][x] = cell;
            }
        }
    }
    
    setupEventListeners() {
        // Control buttons
        document.getElementById('set-start').addEventListener('click', () => this.setMode('setStart'));
        document.getElementById('set-goal').addEventListener('click', () => this.setMode('setGoal'));
        document.getElementById('clear-path').addEventListener('click', () => this.clearPath());
        document.getElementById('clear-all').addEventListener('click', () => this.clearAll());
        document.getElementById('random-obstacles').addEventListener('click', () => this.generateRandomObstacles());
        
        // Algorithm buttons
        document.querySelectorAll('.btn-algorithm').forEach(btn => {
            btn.addEventListener('click', (e) => this.runAlgorithm(e.target.dataset.algorithm));
        });
        
        // Speed control
        document.getElementById('speed').addEventListener('input', (e) => {
            this.animationSpeed = parseInt(e.target.value);
            document.getElementById('speed-value').textContent = `${this.animationSpeed}ms`;
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
        });
        
        // Set new start
        const cell = this.grid[y][x];
        if (!cell.classList.contains('goal')) {
            cell.classList.remove('empty', 'obstacle');
            cell.classList.add('start');
            this.updateStatus(`Inicio establecido en (${x}, ${y})`);
        }
    }
    
    setGoal(x, y) {
        // Clear existing goal
        document.querySelectorAll('.cell.goal').forEach(cell => {
            cell.classList.remove('goal');
            cell.classList.add('empty');
        });
        
        // Set new goal
        const cell = this.grid[y][x];
        if (!cell.classList.contains('start')) {
            cell.classList.remove('empty', 'obstacle');
            cell.classList.add('goal');
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
        document.querySelectorAll('.cell').forEach(cell => {
            if (cell.classList.contains('visited') || 
                cell.classList.contains('frontier') || 
                cell.classList.contains('path')) {
                cell.classList.remove('visited', 'frontier', 'path');
                cell.classList.add('empty');
            }
        });
        this.updateStatus('Camino limpiado');
    }
    
    clearAll() {
        document.querySelectorAll('.cell').forEach(cell => {
            cell.className = 'cell empty';
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
    
    async runAlgorithm(algorithm) {
        if (this.isRunning) {
            this.updateStatus('Algoritmo ya en ejecución');
            return;
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
                await this.sleep(this.animationSpeed / 2);
            }
        }
    }
    
    sleep(ms) {
        return new Promise(resolve => setTimeout(resolve, ms));
    }
    
    updateStatus(message) {
        document.getElementById('status').textContent = message;
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
