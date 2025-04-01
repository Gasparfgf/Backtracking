#include <iostream>
#include <fstream>
#include <vector>

using namespace std;


const string MAZE_FILE = "maze.txt";
const int MAZE_SIZE = 20;
vector<vector<char>> maze;

void readMazeFile(string fileName) {
    ifstream file(fileName);
    if(!file.is_open()) {
        cerr << "Erreur lors de l'ouverture du fichier " << fileName << endl;
        return;
    }
    string line;
    while(getline(file, line)) {
        maze.push_back(vector<char>(line.begin(), line.end()));
    }
    file.close();
}

void printMaze() {
    for(int i = 0; i < MAZE_SIZE; i++) {
        for(int j = 0; j < MAZE_SIZE; j++) {
            cout << maze[i][j];
        }
        cout << endl;
    }
}

int main() {
    readMazeFile(MAZE_FILE);
    printMaze();
    return 0;
}