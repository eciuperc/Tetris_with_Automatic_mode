#pragma once
#include "TetrisGame.h"

class AutomaticInstructions
{
public:
    AutomaticInstructions(bool instantmode);

    void UpdateNextMove(TetrisGame tetrisgame, int type);        // Updates OptimalScore and NextPos

    int GetScore();                                              // Returns the optimal score
    int* GetNextPos();                                           // Returns the optimal position

    // Instructions to know how the tetromino must rotate and move
    int RotationInstruction(Tetromino tetromino);
    int MoveInstruction(Tetromino tetromino);

private:
    // Updates possibilities and scores
    void UpdatePossibilities(TetrisGame tetrisgame, int type);
    void UpdateScores(TetrisGame tetrisgame, int type);

public:
    bool firstframe;

private:
    // Variables for determining what to do
    int*** possibilities;           // Every Position possible for every rotation state and every j position of the first tile (a position value of -1 means the position is not possible)
    int** createdholes;              // Holes created by every possibility
    int** jumpscreated;             // Jumps created (ie measure of "smoothness") by every possibility
    int** erasedlines;              // Lines erased by every possibility
    int** maxheight;                // Maximum height reached by every possibility
    int** matcopy;                  // Copy of the TetrisGame's matrix

    int* OptimalScores;             // Optimal values of Holes created, jumps created, lines erased and maximum height for the position retained 
    int* NextPos;                   // Position retained satisfying optimal value according to the Weight function
    int NextRot;
};