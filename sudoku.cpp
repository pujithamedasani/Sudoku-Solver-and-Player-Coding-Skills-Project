#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
using namespace std;

const int N = 9;

void printBoard(const vector<vector<int>>& board) {
    for (int i = 0; i < N; i++) {
        if (i % 3 == 0 && i != 0)
            cout << "------+-------+------\n";
        for (int j = 0; j < N; j++) {
            if (j % 3 == 0 && j != 0) cout << " | ";
            cout << (board[i][j] == 0 ? "." : to_string(board[i][j]));
            if (j < N - 1 && j % 3 != 2) cout << " ";
        }
        cout << "\n";
    }
}

bool isValid(const vector<vector<int>>& board, int row, int col, int num) {
    for (int j = 0; j < N; j++)
        if (board[row][j] == num) return false;
    for (int i = 0; i < N; i++)
        if (board[i][col] == num) return false;
    int sr = (row / 3) * 3, sc = (col / 3) * 3;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (board[sr + i][sc + j] == num) return false;
    return true;
}

bool solve(vector<vector<int>>& board) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (board[i][j] == 0) {
                for (int num = 1; num <= 9; num++)
                    if (isValid(board, i, j, num)) {
                        board[i][j] = num;
                        if (solve(board)) return true;
                        board[i][j] = 0;
                    }
                return false;
            }
    return true;
}

bool fillBoard(vector<vector<int>>& board, mt19937& rng) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (board[i][j] == 0) {
                vector<int> nums = {1,2,3,4,5,6,7,8,9};
                shuffle(nums.begin(), nums.end(), rng);
                for (int num : nums)
                    if (isValid(board, i, j, num)) {
                        board[i][j] = num;
                        if (fillBoard(board, rng)) return true;
                        board[i][j] = 0;
                    }
                return false;
            }
    return true;
}

int countSolutions(vector<vector<int>> board, int limit = 2) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (board[i][j] == 0) {
                int count = 0;
                for (int num = 1; num <= 9; num++)
                    if (isValid(board, i, j, num)) {
                        board[i][j] = num;
                        count += countSolutions(board, limit - count);
                        if (count >= limit) return count;
                    }
                return count;
            }
    return 1;
}

void digHoles(vector<vector<int>>& board, int clues, mt19937& rng) {
    vector<int> positions(81);
    iota(positions.begin(), positions.end(), 0);
    shuffle(positions.begin(), positions.end(), rng);

    int removed = 0, target = 81 - clues;
    for (int pos : positions) {
        if (removed >= target) break;
        int r = pos / 9, c = pos % 9;
        int backup = board[r][c];
        board[r][c] = 0;
        if (countSolutions(board) == 1)
            removed++;
        else
            board[r][c] = backup;
    }
}

struct Difficulty { string name; int clues; };
const Difficulty EASY   = {"Easy",   36};
const Difficulty MEDIUM = {"Medium", 30};
const Difficulty HARD   = {"Hard",   25};

int main() {
    auto seed = chrono::high_resolution_clock::now().time_since_epoch().count();
    mt19937 rng(seed);

    Difficulty diff = MEDIUM;

    cout << "Generating " << diff.name << " Sudoku...\n\n";

    vector<vector<int>> solved(N, vector<int>(N, 0));
    fillBoard(solved, rng);

    vector<vector<int>> puzzle = solved;
    digHoles(puzzle, diff.clues, rng);

    cout << "Puzzle (" << diff.name << "):\n\n";
    printBoard(puzzle);

    // ── Interactive loop ──────────────────────────────────────────────────────
    cout << "\nCommands: [s] Show solution  [n] New game  [q] Quit\n";

    char cmd;
    bool solutionShown = false;

    while (true) {
        cout << "\n> ";
        cin >> cmd;

        if (cmd == 's') {
            if (solutionShown) {
                cout << "Solution already shown above.\n";
            } else {
                vector<vector<int>> solution = puzzle;
                solve(solution);
                cout << "\nSolution:\n\n";
                printBoard(solution);
                solutionShown = true;
            }
        }
        else if (cmd == 'n') {
            // Generate a brand new game
            seed = chrono::high_resolution_clock::now().time_since_epoch().count();
            rng.seed(seed);

            solved.assign(N, vector<int>(N, 0));
            fillBoard(solved, rng);
            puzzle = solved;
            digHoles(puzzle, diff.clues, rng);
            solutionShown = false;

            cout << "\nNew Puzzle (" << diff.name << "):\n\n";
            printBoard(puzzle);
            cout << "\nCommands: [s] Show solution  [n] New game  [q] Quit\n";
        }
        else if (cmd == 'q') {
            cout << "Goodbye!\n";
            break;
        }
        else {
            cout << "Unknown command. Use s / n / q.\n";
        }
    }

    return 0;
}