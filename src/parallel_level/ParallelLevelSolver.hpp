#ifndef PARALLEL_LEVEL_SOLVER_H
#define PARALLEL_LEVEL_SOLVER_H

#include "../common/MazeCommon.hpp"
#include <functional>
#include <thread>
#include <vector>

/**
 * @class ParallelLevelSolver
 * @brief Classe qui implémente la résolution parallèle par niveau du labyrinthe
 * 
 * Cette classe utilise un algorithme de backtracking parallèle par niveau
 * où chaque niveau est exploré en parallèle.
 */
class ParallelLevelSolver : public MazeSolver {
public:
    /**
     * @brief Résout le labyrinthe en parallèle par niveau
     * @return true si une solution a été trouvée, false sinon
     */
    bool solveMaze() override;

protected:
    /**
     * @brief Résout un niveau spécifique du labyrinthe
     * @param levelIdx Index du niveau à résoudre
     * @param hasB Indique si la clé B a été trouvée
     * @param hasC Indique si la clé C a été trouvée
     * @param hasE Indique si la clé E a été trouvée
     * @param localPath Vecteur pour stocker le chemin trouvé
     * @return true si une solution a été trouvée pour ce niveau
     */
    bool solveLevel(int levelIdx, bool hasB, bool hasC, bool hasE, std::vector<Position>& localPath);
    
    /**
     * @brief Méthode récursive de backtracking pour la résolution d'un niveau
     * @param pos Position actuelle
     * @param hasB Indique si la clé B a été trouvée
     * @param hasC Indique si la clé C a été trouvée
     * @param hasE Indique si la clé E a été trouvée
     * @param localPath Vecteur pour stocker le chemin trouvé
     * @param levelIdx Index du niveau en cours de résolution
     * @return true si une solution a été trouvée à partir de cette position
     */
    bool solveLocalMaze(Position pos, bool hasB, bool hasC, bool hasE, 
                      std::vector<Position>& localPath, int levelIdx);
};

#endif // PARALLEL_LEVEL_SOLVER_H