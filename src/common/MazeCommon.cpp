#include "MazeCommon.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

void MazeSolver::readMazeFiles(const std::vector<std::string>& fileNames) {
    levels.resize(fileNames.size());
    
    for (size_t i = 0; i < fileNames.size(); ++i) {
        std::ifstream file(fileNames[i]);
        if (!file) {
            std::stringstream error;
            error << "❌ Erreur: Erreur lors de l'ouverture du fichier " << fileNames[i];
            throw MazeException(error.str());
        }
        
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) {  // Ignorer les lignes vides
                levels[i].maze.push_back(std::vector<char>(line.begin(), line.end()));
            }
        }
        file.close();
        
        if (levels[i].maze.empty()) {
            std::stringstream error;
            error << "❌ Erreur: Le fichier " << fileNames[i] << " ne contient pas de labyrinthe valide";
            throw MazeException(error.str());
        }
        
        // Vérifier que toutes les lignes ont la même longueur
        size_t firstRowLength = levels[i].maze[0].size();
        for (size_t r = 1; r < levels[i].maze.size(); ++r) {
            if (levels[i].maze[r].size() != firstRowLength) {
                std::stringstream error;
                error << "❌ Erreur: Format invalide dans le fichier " << fileNames[i] 
                      << " : toutes les lignes doivent avoir la même longueur";
                throw MazeException(error.str());
            }
        }
        
        levels[i].rows = levels[i].maze.size();
        levels[i].cols = levels[i].maze[0].size();
        levels[i].visited.resize(levels[i].rows, std::vector<bool>(levels[i].cols, false));
    }
}

void MazeSolver::resetMaze() {
    path.clear();
    for (auto& level : levels) {
        level.resetVisits();
        for (auto& row : level.maze) {
            for (char& cell : row) {
                if (cell == PATH) cell = EMPTY;
            }
        }
    }
}

bool MazeSolver::isValidMove(int level, int x, int y) const {
    if (level < 0 || level >= static_cast<int>(levels.size())) {
        return false;
    }
    
    return x >= 0 && x < levels[level].rows &&
           y >= 0 && y < levels[level].cols &&
           levels[level].maze[x][y] != WALL &&
           levels[level].maze[x][y] != MONSTER &&
           !levels[level].visited[x][y];
}

Position MazeSolver::findPosition(int level, char target) const {
    if (level < 0 || level >= static_cast<int>(levels.size())) {
        return Position(-1, -1, -1);
    }
    
    for (int i = 0; i < levels[level].rows; ++i) {
        for (int j = 0; j < levels[level].cols; ++j) {
            if (levels[level].maze[i][j] == target) {
                return Position(i, j, level);
            }
        }
    }
    return Position(-1, -1, -1); // Position non trouvée
}

bool MazeSolver::hasAllKeys(const Position& pos, bool& hasB, bool& hasC, bool& hasE) const {
    char current = levels[pos.level].maze[pos.x][pos.y];
    
    if (current == KEY_B) hasB = true;
    if (current == KEY_C) hasC = true;
    if (current == KEY_E) hasE = true;
    
    if (current == END) {
        return (hasB && hasC && hasE);
    }
    
    return false;
}

void MazeSolver::markPath() {
    for (const auto& pos : path) {
        char& cell = levels[pos.level].maze[pos.x][pos.y];
        if (cell != START && cell != END && 
            cell != KEY_B && cell != KEY_C && cell != KEY_E &&
            cell != DOOR_1 && cell != DOOR_2 && cell != TELEPORT) {
            cell = PATH;
        }
    }
}

void MazeSolver::printMaze(int level) const {
    if (level < 0 || level >= static_cast<int>(levels.size())) {
        std::cerr << "❌ Erreur: Niveau invalide -> " << level << std::endl;
        return;
    }
    
    for (const auto &row : levels[level].maze) {
        for (char cell : row) {
            if (cell == PATH) {
                std::cout << COLOR_BLUE << cell << COLOR_RESET << " ";
            } else if (cell == WALL || cell == MONSTER) {
                std::cout << COLOR_RED << cell << COLOR_RESET << " ";
            } else if (cell == DOOR_1 || cell == DOOR_2 || cell == TELEPORT) {
                std::cout << COLOR_GREEN << cell << COLOR_RESET << " ";
            } else if (cell == START || cell == END) {
                std::cout << COLOR_YELLOW << cell << COLOR_RESET << " ";
            } else {
                std::cout << cell << " ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void MazeSolver::printAllLevels() const {
    for (size_t i = 0; i < levels.size(); ++i) {
        std::string levelName;
        switch(i) {
            case LEVEL_0: levelName = "Étage 0"; break;
            case LEVEL_1_MAIN: levelName = "Étage 1 (Pièce principale)"; break;
            case LEVEL_1_SEC: levelName = "Étage 1 (Pièce secondaire)"; break;
            case LEVEL_2: levelName = "Étage 2"; break;
            default: levelName = "Niveau " + std::to_string(i);
        }
        std::cout << levelName << " :" << std::endl;
        printMaze(i);
    }
}

Position MazeSolver::getStartPosition() const {
    Position start = findPosition(LEVEL_0, START);
    if (start.x == -1) {
        throw MazeException("❌ Erreur: Pas de point de départ 'D' trouvé dans le labyrinthe !");
    }
    return start;
}

Position MazeSolver::getEndPosition() const {
    for (size_t level = 0; level < levels.size(); ++level) {
        Position end = findPosition(level, END);
        if (end.x != -1) {
            return end;
        }
    }
    throw MazeException("❌ Erreur: Pas de point d'arrivée 'A' trouvé dans le labyrinthe !");
}

bool MazeSolver::isExitReachable(bool hasB, bool hasC, bool hasE) const {
    return (hasB && hasC && hasE);
}

bool MazeSolver::checkMazeIntegrity() const {
    if (levels.empty()) {
        std::cerr << "❌ Erreur: Aucun niveau de labyrinthe chargé" << std::endl;
        return false;
    }
    
    try {
        Position start = getStartPosition();
        Position end = getEndPosition();
        bool foundB = false, foundC = false, foundE = false;
        
        for (size_t level = 0; level < levels.size(); ++level) {
            if (findPosition(level, KEY_B).x != -1) foundB = true;
            if (findPosition(level, KEY_C).x != -1) foundC = true;
            if (findPosition(level, KEY_E).x != -1) foundE = true;
        }
        
        if (!foundB) {
            std::cerr << "⚠️ Avertissement : La clé 'B' n'est pas présente dans le labyrinthe" << std::endl;
        }
        if (!foundC) {
            std::cerr << "⚠️ Avertissement : La clé 'C' n'est pas présente dans le labyrinthe" << std::endl;
        }
        if (!foundE) {
            std::cerr << "⚠️ Avertissement : La clé 'E' n'est pas présente dans le labyrinthe" << std::endl;
        }
        
        // Vérifier la connectivité entre les niveaux
        bool hasLevel0To1 = findPosition(LEVEL_0, DOOR_1).x != -1 && findPosition(LEVEL_1_MAIN, DOOR_1).x != -1;
        bool hasLevel1To2 = (findPosition(LEVEL_1_MAIN, DOOR_2).x != -1 || findPosition(LEVEL_1_SEC, DOOR_2).x != -1) 
                           && findPosition(LEVEL_2, DOOR_2).x != -1;
        bool hasMainToSec = findPosition(LEVEL_1_MAIN, TELEPORT).x != -1 && findPosition(LEVEL_1_SEC, DOOR_1).x != -1;
        
        if (!hasLevel0To1) {
            std::cerr << "⚠️ Avertissement : Aucune porte '1' reliant le niveau 0 au niveau 1" << std::endl;
        }
        if (!hasLevel1To2) {
            std::cerr << "⚠️ Avertissement : Aucune porte '2' reliant le niveau 1 au niveau 2" << std::endl;
        }
        if (!hasMainToSec) {
            std::cerr << "⚠️ Avertissement : Aucun téléporteur reliant les pièces du niveau 1" << std::endl;
        }
        
        return true;
    } catch (const MazeException& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}