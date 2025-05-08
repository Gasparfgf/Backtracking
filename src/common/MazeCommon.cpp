#include "MazeCommon.hpp"

void MazeSolver::readMazeFiles(const std::vector<std::string>& fileNames) {
    levels.resize(fileNames.size());
    
    for (size_t i = 0; i < fileNames.size(); ++i) {
        std::ifstream file(fileNames[i]);
        if(!file) {
            std::cerr << "❌ Erreur lors de l'ouverture du fichier " << fileNames[i] << std::endl;
            continue;
        }
        
        std::string line;
        while(std::getline(file, line)) {
            levels[i].maze.push_back(std::vector<char>(line.begin(), line.end()));
        }
        file.close();
        
        levels[i].rows = levels[i].maze.size();
        levels[i].cols = levels[i].maze[0].size();
        levels[i].visited.resize(levels[i].rows, std::vector<bool>(levels[i].cols, false));
    }
}

void MazeSolver::resetMaze() {
    path.clear();
    for (auto& level : levels) {
        for (auto& row : level.visited) {
            std::fill(row.begin(), row.end(), false);
        }
        
        for (auto& row : level.maze) {
            for (char& cell : row) {
                if (cell == '*') cell = ' ';
            }
        }
    }
}

bool MazeSolver::isValidMove(int level, int x, int y) const {
    return x >= 0 && x < levels[level].rows && 
           y >= 0 && y < levels[level].cols && 
           levels[level].maze[x][y] != '#' && 
           levels[level].maze[x][y] != 'M' && 
           !levels[level].visited[x][y];
}

Position MazeSolver::findPosition(int level, char target) const {
    for (int i = 0; i < levels[level].rows; ++i) {
        for (int j = 0; j < levels[level].cols; ++j) {
            if (levels[level].maze[i][j] == target) {
                return {i, j, level};
            }
        }
    }
    return {-1, -1, -1}; // Position non trouvée
}

void MazeSolver::markPath() {
    for (const auto& pos : path) {
        char& cell = levels[pos.level].maze[pos.x][pos.y];
        if (cell != 'D' && cell != 'A' && cell != 'B' && cell != 'C' && cell != 'E' &&
            !(cell >= '0' && cell <= '9') && cell != 'T') {
            cell = '*';
        }
    }
}

void MazeSolver::printMaze(int level) const {
    for (const auto &row : levels[level].maze) {
        for (char cell : row) {
            if (cell == '*') {
                std::cout << "\033[32m" << cell << "\033[0m "; // Affichage en vert
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
        }
        std::cout << levelName << " :" << std::endl;
        printMaze(i);
    }
}

Position MazeSolver::getStartPosition() const {
    return findPosition(LEVEL_0, 'D');
}