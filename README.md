#  Navra - Route Optimizer

**An intelligent routing and tour-planning engine for optimal campus navigation.**

---

## Table of Contents
- [Overview](#-overview)
- [Features](#-features)
- [System Architecture](#-system-architecture)
- [Algorithms Implemented](#-algorithms-implemented)
- [Installation](#-installation)
- [Usage Guide](#-usage-guide)
- [Input Format](#-input-format)
- [Example Scenarios](#-example-scenarios)
- [Performance](#-performance)
- [Project Structure](#-project-structure)
- [Future Enhancements](#-future-enhancements)
- [Contributing](#-contributing)
- [License](#-license)
- [Acknowledgments](#-acknowledgments)
- [Contact & Support](#-contact--support)

---

## Overview

The **Route Optimizer** is a sophisticated routing system designed to compute the most efficient walking paths between locations within a campus environment. Built with advanced graph algorithms and optimization techniques, it provides intelligent route planning for students, visitors, and navigation systems.

### Why This Project?
- **Smart Navigation** – Automatically finds optimal paths  
- **Flexible Planning** – Multiple routing modes  
- **Real-World Application** – Built for campus use  
- **Educational** – Demonstrates practical algorithm design  

---

## Features

### Three Routing Modes

| Mode | Description | Best For | Algorithm |
|------|-------------|----------|-----------|
| **Flexible Order** | Finds optimal visiting order | When order doesn't matter | TSP (Greedy + 2-opt + DP) |
| **Fixed Order** | Computes shortest path in given order | Prescribed route | Dijkstra |
| **Hamiltonian Traversal** | Complete traversal | Exploratory tours | Kruskal MST → DFS → A* |

### Core Capabilities
- ✅ Accurate path reconstruction  
- ✅ DSU-based connectivity validation  
- ✅ Haversine heuristic for A*  
- ✅ Modular architecture  
- ✅ Time estimation  
- ✅ Robust error handling  

---

## System Architecture

### High-Level Flow

```
┌─────────────────┐
│   User Input    │
└──────┬──────────┘
       ▼
┌─────────────────┐
│ Validation (DSU)│
└──────┬──────────┘
       ▼
┌─────────────────┐
│ Route Optimizer │
└──────┬──────────┘
       ▼
┌─────────────────┐
│   Algorithms    │
└──────┬──────────┘
       ▼
┌─────────────────┐
│  Path Output    │
└─────────────────┘
```

### Module Architecture

```
main.cpp
├── CLI, input parsing, DSU checks

RouteOptimizer.cpp
├── Mode selection, orchestration, path building

dijkstra.cpp, astar.cpp, tsp.cpp, kruskal.cpp
├── Dijkstra, A*, TSP (Greedy + 2-opt + DP), Kruskal, DFS

graph.cpp
├── CSV loading, adjacency list, ID mapping

dsu.cpp
└── Disjoint Set Union
```

---

## Algorithms Implemented

### **1. Dijkstra's Algorithm**
- Shortest path between any two nodes  
- Time: `O(E log V)`  
- Min-heap + parent tracking  

### **2. A* Search**
- Haversine heuristic  
- Faster than Dijkstra for geo-distance  

**Formula:**
```
f(n) = g(n) + h(n)
h(n) = Haversine distance
```

### **3. Travelling Salesman Problem**
- Flexible order routing  
- Multi-level:
  - Greedy (`O(n²)`)
  - 2-opt (`O(n²)`)
  - DP (`O(n²·2ⁿ)`)

### **4. Kruskal MST**
- Backbone generation for full tour  
- Time: `O(E log E)`  

### **5. DFS**
- Produces tour order from MST  

---

## Installation

### Prerequisites
- C++17 compiler  
- Make  
- STL  

### Build Instructions

```sh
git clone https://github.com/bhavishy2801/Navra.git
cd Navra
make
./route_optimizer
```

### Manual Compilation

```sh
g++ -std=c++17 -O2 -o route_optimizer \
    main.cpp RouteOptimizer.cpp algorithms.cpp graph.cpp dsu.cpp
```

---

## Usage Guide

### Interactive Menu

```text
=== IIT Jodhpur Campus Route Optimizer ===

Select Routing Mode:
1. Flexible Order (TSP)
2. Fixed Order (Shortest Path)
3. Hamiltonian Traversal
4. Exit
```

### Mode 1 — Flexible Order

```text
Enter choice: 1
Enter number of locations: 4
Enter location names:
Library
Hostel
Mess
Lecture Hall Complex
```

### Output

```text
Algorithm: TSP (Greedy + 2-opt)
Total Distance: 1420 m
Total Time: 14.2 min

Optimal Visit Order:
1. Library
2. Hostel
3. Mess
4. Lecture Hall Complex
```

---

## Input Format

### attractions.csv

```csv
id,name,latitude,longitude,visitDuration,type
1,Library,26.478321,73.821456,10,Academic
2,Hostel-A,26.479123,73.824789,5,Residential
3,Mess,26.477890,73.823456,15,Amenity
4,Lecture Hall Complex,26.478567,73.822345,20,Academic
5,Sports Complex,26.476789,73.825678,30,Recreation
```

### roads.csv

```csv
src,dst,distance
1,2,140
2,3,210
1,4,180
3,5,320
4,5,450
```

---

## Example Scenarios

### Scenario 1 — New Student Tour
- Visit major locations
- Output includes full path + time

### Scenario 2 — Class-to-Class Navigation
- Fastest path with intermediate nodes

### Scenario 3 — Orientation Program
- Full traversal of campus

---

## Performance

### Complexity Overview

| Operation | Complexity | Runtime |
|-----------|------------|---------|
| Graph Loading | O(V + E) | <10ms |
| DSU Check | O(V α(V)) | <1ms |
| Dijkstra | O(E log V) | <5ms |
| TSP | O(n²·2ⁿ) / O(n²) | 10–100ms |
| Full Tour | O(E log E + V²) | 50–200ms |

---

## Project Structure

```
navra/
│
├── backend/
│   ├── include/
│   │   ├── algorithms.h
│   │   ├── api.h
│   │   ├── attraction.h
│   │   ├── dsu.h
│   │   ├── graph.h
│   │   ├── json.hpp
│   │   └── route_optimizer.h
│   │
│   ├── src/
│   │   ├── api.cpp
│   │   ├── astar.cpp
│   │   ├── dijkstra.cpp
│   │   ├── dsu.cpp
│   │   ├── graph.cpp
│   │   ├── kruskal.cpp
│   │   ├── route_optimizer.cpp
│   │   └── tsp.cpp
│   │
│   ├── attractions.csv
│   ├── roads.csv
│   ├── main_api.cpp
│   ├── server.js
│   ├── Makefile
│   ├── optimizer.exe
│   ├── package.json
│   └── package-lock.json
│
│
├── backend_cli/
│   ├── include/
│   │   ├── algorithms.h
│   │   ├── attraction.h
│   │   ├── dsu.h
│   │   ├── graph.h
│   │   ├── json.hpp
│   │   └── route_optimizer.h
│   │
│   ├── src/
│   │   ├── astar.cpp
│   │   ├── dijkstra.cpp
│   │   ├── dsu.cpp
│   │   ├── graph.cpp
│   │   ├── kruskal.cpp
│   │   ├── route_optimizer.cpp
│   │   └── tsp.cpp
│   │
│   ├── attractions.csv
│   ├── roads.csv
│   ├── main.cpp
│   ├── Makefile
│   ├── optimizer.exe
│
│
├── website/
│   ├── public/
│   │   └── logo.svg
│   │
│   ├── src/
│   │   ├── assets/
│   │   │   ├── attractions.csv
│   │   │   ├── logo.svg
│   │   │   └── roads.csv
│   │   │
│   │   ├── components/
│   │   │   ├── AlgorithmCard.jsx
│   │   │   ├── AnimatedBackground.jsx
│   │   │   ├── ControlPanel.jsx
│   │   │   ├── ErrorDisplay.jsx
│   │   │   ├── Footer.jsx
│   │   │   ├── GlassCard.jsx
│   │   │   ├── GraphVisualization.jsx
│   │   │   ├── LoadingSpinner.jsx
│   │   │   ├── LocationSelector.jsx
│   │   │   ├── Navbar.jsx
│   │   │   ├── RouteDisplay.jsx
│   │   │   └── ThemeToggle.jsx
│   │   │
│   │   ├── context/
│   │   │   └── ThemeContext.jsx
│   │   │
│   │   ├── pages/
│   │   │   ├── About.jsx
│   │   │   ├── Algorithms.jsx
│   │   │   ├── Home.jsx
│   │   │   └── Optimizer.jsx
│   │   │
│   │   ├── styles/
│   │   │   ├── about.css
│   │   │   ├── algorithms.css
│   │   │   ├── control-panel.css
│   │   │   ├── error.css
│   │   │   ├── footer.css
│   │   │   ├── home.css
│   │   │   ├── loading.css
│   │   │   ├── location-selector.css
│   │   │   ├── navbar.css
│   │   │   ├── optimizer.css
│   │   │   ├── route-display.css
│   │   │   ├── variables.css
│   │   │
│   │   ├── App.css
│   │   ├── App.jsx
│   │   ├── index.css
│   │   └── main.jsx
│   │
│   └── eslint.config.js
│   ├── index.html
│   ├── package.json
│   ├── package-lock.json
│   ├── README.md
│   └── vite.config.js
│
└── .gitignore
└── LICENSE
└── package-lock.json
├── README.md

```

---

## Future Enhancements

### Short-term
- Web interface (Leaflet.js)
- React Native app
- Real-time edge weights
- Multi-criteria optimization
- Accessibility routing

### Long-term
- ML-based route prediction
- Indoor navigation
- Multi-modal routing
- Social features
- RESTful API

---

## Contributing

### Ways to Contribute
- Report bugs
- Suggest features
- Submit PRs
- Improve documentation
- Write tests

### Contribution Guide

```sh
git clone https://github.com/bhavishy2801/Navra.git
git checkout -b feature/your-feature
make
./route_optimizer
git commit -am "Add: your feature"
git push origin feature/your-feature
```

---

## License

**MIT License**

Copyright (c) 2025

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

---

## Acknowledgments

- **Robert Tarjan** — Union-Find
- **Edsger Dijkstra** — Shortest path algorithm
- **A\* creators** — Heuristic search
- **Joseph Kruskal** — Minimum spanning tree
- **STL & C++ community**

---

## Team Members

| S No | Name | Roll Number | Contributions |
|------|------|-------------|---------------|
| 1 | Bhavishy Agrawal | B24CS1023 | Integrated the website with the backend server, optimized the backend functionalities and implemented A\* pathfinding using adjacency-list graph representations, priority queues (min-heap) for frontier management, and heuristic-driven traversal across attraction–road networks |
| 2 | Husain Mohammad Ali | B24CS1084 | Implemented Dijkstra for shortest paths and optimized traversal pipelines using condition-aware graph algorithms. Used a precomputed DSU to verify that the induced subgraph of requested locations remains connected. Designed Hamiltonian traversal via A\* heuristic search combined with Kruskal-derived MST cost bounds and DFS exploration |
| 3 | Hayagriv Desikan | B24CS1092 | Implemented DSU and Kruskal’s algorithm for MST, added DSU pre-computation to verify whether the selected attractions induce a connected subgraph, contributed to styling and UI design for the website’s frontend, and helped integrate the IIT Jodhpur campus model by mapping attractions and roads to graph vertices and edges |
| 4 | Arsh Goyal | B24EE1007 | Implemented the backend API routes and backend server connecting the executable file for the optimizer and implemented a menu driven program combining all the algorithms and features of the project |

---

## Contact & Support

- **Issues:** [Issues](https://github.com/bhavishy2801/Navra/issues)
- **Discussions:** [Discussions](https://github.com/bhavishy2801/Navra/dicussions)
- **Email:** [Bhavishy Agrawal](bhavishyrocker2801@gmail.com)

---

**Made with ❤️ for IIT Jodhpur**