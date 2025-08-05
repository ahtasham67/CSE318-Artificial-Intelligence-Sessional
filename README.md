# 🤖 Artificial Intelligence(CSE-318) OFFLINES

Welcome to my comprehensive collection of AI algorithms and implementations from CSE 318 - Artificial Intelligence Sessional course. This repository showcases various fundamental AI concepts including search algorithms, game theory, machine learning, and optimization techniques.

## 📚 Offlines Overview

| Offline                                             | Description                                | Language | Key Algorithms                           |
| --------------------------------------------------- | ------------------------------------------ | -------- | ---------------------------------------- |
| [🧩 N-Puzzle A\*](#-n-puzzle-a-search)              | Sliding puzzle solver using A\* search     | C++      | A\* Search, Manhattan/Hamming Heuristics |
| [⚛️ Chain Reaction](#%EF%B8%8F-chain-reaction-game) | Strategic board game with AI opponents     | Python   | Minimax, Alpha-Beta Pruning              |
| [🌳 Decision Tree](#-decision-tree-classifier)      | Machine learning classifier implementation | C++      | Information Gain, Gini Index             |
| [📊 Max Cut Problem](#-max-cut-optimization)        | Graph optimization using metaheuristics    | C++      | GRASP, Local Search, Greedy Algorithms   |

---

## 🧩 N-Puzzle A\* Search

A sophisticated implementation of the classic sliding puzzle problem using A\* search algorithm with multiple heuristic functions.

### 🎯 Features

- **Multiple Heuristics**: Manhattan, Hamming, Euclidean, and Linear Conflict
- **Optimal Solutions**: Guaranteed shortest path using A\* algorithm
- **Performance Metrics**: Tracks nodes explored and expanded
- **Flexible Input**: Supports various puzzle sizes

### 🚀 Quick Start

```bash
cd N-puzzle_A*
g++ 2105067.cpp -o main
./main                # Default heuristic
./main 1             # Manhattan distance
./main 2             # Hamming distance
./main 3             # Euclidean distance
```

### 📁 Files

- `2105067.cpp` - Main implementation with A\* algorithm
- `input.txt` - Sample puzzle configurations
- `main` - Compiled executable

---

## ⚛️ Chain Reaction Game

An intelligent implementation of the Chain Reaction board game featuring AI opponents with advanced minimax algorithms.

### 🎯 Features

- **Smart AI Players**: Multiple difficulty levels with different heuristics using minimax alpha-beta pruning algorithm
- **Interactive GUI**: Pygame-based visual interface
- **Strategic Depth**: Minimax with alpha-beta pruning
- **Multiple Heuristics**:
  - Basic orb counting
  - Weighted position evaluation
  - Cell control analysis
  - Stability assessment

### 🚀 Quick Start

```bash
cd chainReaction
python frontend.py    # Launch GUI game
python backend.py     # Run backend logic
```

### 📁 Files

- `frontend.py` - Pygame GUI implementation
- `backend.py` - Game logic and AI algorithms
- `2105067.pdf` - offline report

---

## 🌳 Decision Tree Classifier

A from-scratch implementation of decision tree algorithms with multiple splitting criteria for classification tasks.

### 🎯 Features

- **Multiple Criteria**: Information Gain (IG), Information Gain Ratio (IGR), Normalized Weighted Information Gain (NWIG)
- **Pruning Support**: Maximum depth limiting
- **Dataset Support**: Works with CSV datasets (Iris, Adult)
- **Performance Analysis**: Accuracy metrics and tree visualization

### 🚀 Quick Start

```bash
cd decisionTree
chmod +x run.sh
./run.sh IG 5        # Information Gain, max depth 5
./run.sh IGR 0       # Information Gain Ratio, no pruning
./run.sh NWIG 3      # Normalized WIG, max depth 3
```

### 📁 Files

- `main.cpp` - Main program entry point
- `tree.hpp` - Decision tree implementation
- `data.hpp` - Data handling utilities
- `sample.hpp` - Sample data structures
- `Datasets/` - Training datasets (Iris, Adult)

---

## 📊 Max Cut Optimization

Advanced graph optimization offline implementing various metaheuristic algorithms to solve the Maximum Cut problem.

### 🎯 Features

- **Multiple Algorithms**:
  - Randomized heuristic
  - Greedy construction
  - Semi-greedy with α parameter
  - GRASP (Greedy Randomized Adaptive Search)
  - Local search optimization
- **Performance Analysis**: Statistical evaluation and benchmarking
- **Graph Datasets**: 54 test instances included

### 🚀 Quick Start

```bash
cd Max_Cut
chmod +x run.sh
./run.sh set1/g01.rud 1    # Random algorithm
./run.sh set1/g01.rud 2    # Greedy algorithm
./run.sh set1/g01.rud 3    # Semi-greedy
./run.sh set1/g01.rud 4    # GRASP
```

### 📁 Files

- `main.cpp` - Main program controller
- `graph.hpp` - Graph data structure
- `GRASP.hpp` - GRASP algorithm implementation
- `localSearch.hpp` - Local search methods
- `ConstractiveAlgo.hpp` - Construction heuristics
- `statistics.hpp` - Performance analysis
- `set1/` - 54 graph test instances

---

## 🛠️ Technologies Used

![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Python](https://img.shields.io/badge/Python-3776AB?style=for-the-badge&logo=python&logoColor=white)
![Pygame](https://img.shields.io/badge/Pygame-FF6B6B?style=for-the-badge&logo=python&logoColor=white)

## 📈 Algorithms Implemented

- **Search Algorithms**: A\*, Minimax with Alpha-Beta Pruning
- **Machine Learning**: Decision Trees with various splitting criteria
- **Optimization**: GRASP, Local Search, Greedy Heuristics
- **Heuristics**: Manhattan Distance, Hamming Distance, Information Gain
- **Game Theory**: Strategic game playing with evaluation functions

## 📊 offline Statistics

- **Total offlines**: 4
- **Languages Used**: C++, Python
- **Lines of Code**: 1000+ across all offlines
- **Algorithms Implemented**: 15+
- **Test Cases**: 54+ graph instances

## 🚀 Getting Started

1. **Clone the repository**:

   ```bash
   git clone [your-repo-url]
   cd AI
   ```

2. **Navigate to any offline**:

   ```bash
   cd [offline-name]
   ```

3. **Follow offline-specific instructions** in each section above

## 📝 Reports and Documentation

Each offline includes comprehensive PDF reports with:

- Algorithm explanations
- Implementation details
- Performance analysis
- Test results and benchmarks

<div align="center">

**Course**: CSE 318 - Artificial Intelligence Sessional  
**Institution**: Bangladesh University of Engineering and Technology (BUET)  
**StudentID:2105067**
**Semester**: [Level 3, term 1](https://www.buet.ac.bd/)

</div>
