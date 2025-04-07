#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;


 // puisque vector est considéré comme un tableau dynamique
vector<vector<bool>> visitedBoxes;
vector<vector<char>> maze;
int size1, size2;

// Directions possibles (haut, bas, gauche, droite)
const int NUMBER_OF_DIRECTIONS = 4;
int dx[NUMBER_OF_DIRECTIONS] = {-1, 1, 0, 0};
int dy[NUMBER_OF_DIRECTIONS] = {0, 0, -1, 1};

struct Position {
    int x, y;
};


void printMaze() {
    for (const auto &row : maze) {
        for (char cell : row)
            cout << cell << " ";
        cout << endl;
    }
}

void readMazeFile(string fileName) {
    ifstream file(fileName);
    if(!file) {
        cerr << "❌ Erreur lors de l'ouverture du fichier " << fileName << endl;
        return;
    }
    string line;
    while(getline(file, line)) {
        maze.push_back(vector<char>(line.begin(), line.end()));
    }
    file.close();
    size1 = maze.size();
    size2 = maze[0].size();
    visitedBoxes.resize(size1, vector<bool>(size2, false));
}

bool isValidMove(int x, int y) {
    return x >= 0 && x < size1 && y >= 0 && y < size2 && maze[x][y] != '#' && !visitedBoxes[x][y];
}

bool solveMazeSequentially(Position pos, bool pickedUpB, bool pickedUpC, bool pickedUpE) {
    int x = pos.x;
    int y = pos.y;

    if (!isValidMove(x, y)) return false;

    visitedBoxes[x][y] = true;

    if (maze[x][y] == 'B' && !pickedUpB) pickedUpB = true;
    if (maze[x][y] == 'C' && !pickedUpC) pickedUpC = true;
    if (maze[x][y] == 'E' && !pickedUpE) pickedUpE = true;

    if (maze[x][y] == 'A') {
        if (pickedUpB && pickedUpC && pickedUpE) return true;
        else return false;
    }

    for (int direction = 0; direction < NUMBER_OF_DIRECTIONS; direction++) {
        Position nextPosition = {x + dx[direction], y + dy[direction]};
        if(solveMazeSequentially(nextPosition, pickedUpB, pickedUpC, pickedUpE)) return true;
    }

    visitedBoxes[x][y] = false;
    return false;
}

bool canStartWithD(Position &start) {
    bool foundStart = false;
    for (int i = 0; i < size1; i++) {
        for (int j = 0; j < size2; j++) {
            if (maze[i][j] == 'D') {
                start = {i, j};
                foundStart = true;
                break;
            }
        }
        if (foundStart) break;
    }
    return foundStart;
}


bool solveMazeInParallel(Position start) {
    bool found = false;
    thread threads[NUMBER_OF_DIRECTIONS];
    bool results[NUMBER_OF_DIRECTIONS] = {false, false, false, false};

    for (int direction = 0; direction < NUMBER_OF_DIRECTIONS; direction++) {
        threads[direction] = thread([&, direction]() {
            bool hasC = false, hasB = false, hasE = false;
            Position newPos = {start.x + dx[direction], start.y + dy[direction]};
            results[direction] = solveMazeSequentially(newPos, hasC, hasB, hasE);
        });
    }

    for (int i = 0; i < NUMBER_OF_DIRECTIONS; i++) {
        threads[i].join();
        if (results[i]) found = true;
    }

    return found;
}



int main() {
    readMazeFile("maze.txt");
    
    if (maze.empty()) {
        cerr << "Erreur ❌ : Le labyrinthe est vide !" << endl 
        << "Veuillez fournir un fichier de labyrinthe valide !" << endl;
        return 1;
    }

    Position start;
    bool foundStart = canStartWithD(start);

    if (!foundStart) {
        cerr << "Erreur ❌ : Pas de point de départ 'D' trouvé dans le labyrinthe !" << endl;
        return 1;
    }

    bool found = solveMazeSequentially(start, false, false, false);
    if (found) {
        cout << "Mode séquentiel ✅ : Solution trouvée !" << endl;
    } else {
        cout << "Mode séquentiel ❌ : Aucune solution trouvée." << endl;
    }

    visitedBoxes.assign(size1, vector<bool>(size2, false));

    found = solveMazeInParallel(start);

    if (found) {
        cout << "Mode parallèle ✅ : Solution trouvée !" << endl;
    } else {
        cout << "Monde parallèle ❌ : Aucune solution trouvée." << endl;
    }

    return 0;
}
