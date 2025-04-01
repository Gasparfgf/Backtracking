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
