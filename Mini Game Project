/*
    TIC TAC TOE - Console Mini Game
    ---------------------------------
    A 2-player console game demonstrating:
      - Arrays (the 3x3 board)
      - Loops (game turns, board printing)
      - Conditional logic (win/draw checking)
      - Replay functionality

    Player 1 = X, Player 2 = O. Players take turns entering
    a position (1-9) corresponding to the board layout:

         1 | 2 | 3
        -----------
         4 | 5 | 6
        -----------
         7 | 8 | 9
*/

#include <iostream>
using namespace std;

// ---------------------------------------------------
// Print the current board using the array of 9 cells
// ---------------------------------------------------
void printBoard(char board[10]) {
    cout << "\n";
    cout << " " << board[1] << " | " << board[2] << " | " << board[3] << "\n";
    cout << "-----------\n";
    cout << " " << board[4] << " | " << board[5] << " | " << board[6] << "\n";
    cout << "-----------\n";
    cout << " " << board[7] << " | " << board[8] << " | " << board[9] << "\n";
    cout << "\n";
}

// ---------------------------------------------------
// Check if the given player symbol has won
// ---------------------------------------------------
bool checkWin(char board[10], char player) {
    // All 8 possible winning lines (rows, columns, diagonals)
    int winPatterns[8][3] = {
        {1, 2, 3}, {4, 5, 6}, {7, 8, 9},  // rows
        {1, 4, 7}, {2, 5, 8}, {3, 6, 9},  // columns
        {1, 5, 9}, {3, 5, 7}              // diagonals
    };

    for (int i = 0; i < 8; i++) {
        int a = winPatterns[i][0];
        int b = winPatterns[i][1];
        int c = winPatterns[i][2];

        if (board[a] == player && board[b] == player && board[c] == player) {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------
// Check if the board is completely full (used for draw detection)
// ---------------------------------------------------
bool isBoardFull(char board[10]) {
    for (int i = 1; i <= 9; i++) {
        if (board[i] != 'X' && board[i] != 'O') {
            return false; // found an empty cell
        }
    }
    return true;
}

// ---------------------------------------------------
// Play one full game. Returns nothing - just runs until someone
// wins or it's a draw.
// ---------------------------------------------------
void playGame() {
    char board[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    // board[0] is unused - we use positions 1-9 to match player-facing numbers

    char currentPlayer = 'X';
    int move;
    bool gameOver = false;

    while (!gameOver) {
        printBoard(board);

        cout << "Player " << currentPlayer << ", enter your move (1-9): ";
        cin >> move;

        // Validate the move
        if (move < 1 || move > 9) {
            cout << "Invalid position! Choose a number between 1 and 9.\n";
            continue;
        }
        if (board[move] == 'X' || board[move] == 'O') {
            cout << "That spot is already taken! Try again.\n";
            continue;
        }

        // Place the move
        board[move] = currentPlayer;

        // Check win
        if (checkWin(board, currentPlayer)) {
            printBoard(board);
            cout << "*** Player " << currentPlayer << " wins! ***\n";
            gameOver = true;
        }
        // Check draw
        else if (isBoardFull(board)) {
            printBoard(board);
            cout << "*** It's a draw! ***\n";
            gameOver = true;
        }
        // Otherwise switch turns
        else {
            currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
        }
    }
}

// ---------------------------------------------------
// Main - runs the game, then offers replay
// ---------------------------------------------------
int main() {
    char playAgain = 'y';

    cout << "===== TIC TAC TOE =====\n";

    while (playAgain == 'y' || playAgain == 'Y') {
        playGame();

        cout << "Play again? (y/n): ";
        cin >> playAgain;
    }

    cout << "Thanks for playing! Goodbye.\n";
    return 0;
}
