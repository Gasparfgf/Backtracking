# Backtracking - Parallel Programming

## Introduction
Solving a maze using backtracking with object collection in C++.

In this project, i develop an algorithm to solve a maze using a backtracking approach. The maze is represented by a grid of characters, in which various entities are present.

## Goal
The objective of the program is to find a path from starting point D to ending point A, passing through the three objects mentioned (crown, shield, and sword for C, B, and E, respectively).

## Characteristics
- The program take as input a text file describing the maze and display the path to that section.
- A sequential backtracking algorithm.
- A parallel backtracking algorithm using the C++11 thread library.
- The function explores possible paths from D.
- The path is valid only if it reaches A after collecting all three objects.

## Structure des répertoires du projet

src/<br>
├── common/<br>
│   ├── MazeCommon.hpp<br>
│   └── MazeCommon.cpp<br>
├── labyrinthe<br>
 |   ├── maze0.txt<br>
 |   ├── maze1-1.txt<br>
 |   ├── maze1-2.txt<br>
 |   └── maze2.txt<br>
├── parallel_direction/<br>
│   ├── ParallelDirectionSolver.hpp<br>
│   └── ParallelDirectionSolver.cpp<br>
├── parallel_level/<br>
│   ├── ParallelLevelSolver.hpp<br>
│   └── ParallelLevelSolver.cpp<br>
└──  sequential/<br>
    ├── SequentialSolver.hpp<br>
    └── SequentialSolver.cpp<br>
main.cpp<br>
.gitignore<br>
README.md<br>

## Running the project

1. Go to the root of the src folder (src/)

2. Run the command :
```bash
$ g++ -std=c++11 -pthread main.cpp parallel_direction/ParallelDirectionSolver.cpp parallel_level/ParallelLevelSolver.cpp common/MazeCommon.cpp sequential/SequentialSolver.cpp -o test
```

3. Puis lancez la commande :
```bash
$ ./test
```
