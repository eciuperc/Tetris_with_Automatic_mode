#include "AutomaticInstructions.h"

int modd(int x, int y)
{
    int r = x % y;
    if (r >= 0)
        return r;
    else
        return r + y;
}

int CountHoles(int** Tetmatrix, int* lines)
// Function for couting the holes in a tetrisgame, after the lines given in input are erased
{
    int rep = 0;

    bool crashed = false;

    for (int j = 0; j < N; ++j)
    {
        crashed = false;
        for (int ind = 4; ind > 0; --ind)
        {
            for (int i = lines[ind] + 1; i <= lines[ind - 1] - 1; ++i)
            {
                if (Tetmatrix[i][j] > 0)
                {
                    crashed = true;
                }
                if (crashed)
                    break;
                else
                {
                    rep += 1;
                }
            }
        }

        for (int i = lines[0] + 1; i < M; ++i)
        {
            if (Tetmatrix[i][j] > 0)
            {
                crashed = true;
            }
            if (crashed)
                break;
            else
            {
                rep += 1;
            }
        }
    }

    return rep;
}

int JumpsNumber(int** Tetmatrix, int* lines, int type)
// Function for couting the lines erased in a tetrisgame, after the lines given in input are erased
// the "type" variable given in argument is not used here, but can be used to create discrimnation between the tetromino's types, 
// for example if we want to make more tetrises ( ie. 4 lines cleared at the same time) thank to the I tetrominos
{

    // We simulate tetrisgame's update after lines disappeared, just like in TetrisGame's "ClearLines" function

    int** Tetmatrixcopy = new int* [M];
    for (int i = 0; i < M; ++i)
    {
        Tetmatrixcopy[i] = new int[N];
        for (int j = 0; j < N; ++j)
        {
            Tetmatrixcopy[i][j] = Tetmatrix[i][j];
        }
    }

    int nblines = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (lines[i] >= 0)
            nblines += 1;
    }

    for (int i = 0; i < 4; ++i)
    {
        for (int ii = lines[i] - 1; ii > lines[i + 1]; --ii)
        {
            for (int j = 0; j < N; ++j)
            {
                Tetmatrixcopy[ii + i + 1][j] = Tetmatrixcopy[ii][j];
            }
        }
    }

    for (int i = 0; i < nblines; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            Tetmatrixcopy[i][j] = 0;
        }
    }




    // We now calculate the number of jumps

    int* highestpos = new int[N];

    for (int j = 0; j < N; ++j)
    {
        int i = 0;
        while (i < M && Tetmatrixcopy[i][j] == 0)
        {
            i += 1;
        }
        highestpos[j] = i;
    }

    int rep = 0;

    for (int j = 0; j < N - 1; ++j)
    {
        rep += abs(highestpos[j] - highestpos[j + 1]);
    }

    //// If we want to create discrimination between lines

    //if (type == 1)
    //{
    //    for (int j = 0; j < N - 2; ++j)
    //    {
    //        if (highestpos[j] >= highestpos[N - 1] - 4)
    //            rep -= 1;
    //    }
    //}
    //for (int j = 0; j < N - 2; ++j)
    //{
    //    rep += abs(highestpos[j] - highestpos[j + 1]);
    //}
    //rep += std::max(highestpos[N - 2] - highestpos[N - 1], -4);

    for (int i = 0; i < M; ++i)
    {
        delete[]Tetmatrixcopy[i];
    }
    delete[]Tetmatrixcopy;
    delete[]highestpos;

    return rep;
}

// Function for affecting a score to each position
int Score(int a, int b, int c, int d)
{
    if (M - 1 - d <= 5)
        return 1000 * a - 20 * b + 200 * c + (M - 1 - d);
    else
        return 1000 * a - 20 * b + 350 * c + (M - 1 - d);
}






AutomaticInstructions::AutomaticInstructions(bool instantmode)
{
    possibilities = new int** [4];
    for (int i = 0; i < 4; ++i)
    {
        possibilities[i] = new int* [N];
        for (int j = 0; j < N; ++j)
        {
            possibilities[i][j] = new int[8];
            for (int k = 0; k < 8; ++k)
            {
                possibilities[i][j][k] = -1;
            }
        }
    }

    createdholes = new int* [4];
    for (int i = 0; i < 4; ++i)
    {
        createdholes[i] = new int[N];
    }
    jumpscreated = new int* [4];
    for (int i = 0; i < 4; ++i)
    {
        jumpscreated[i] = new int[N];
    }
    erasedlines = new int* [4];
    for (int i = 0; i < 4; ++i)
    {
        erasedlines[i] = new int[N];
    }
    maxheight = new int* [4];
    for (int i = 0; i < 4; ++i)
    {
        maxheight[i] = new int[N];
    }

    matcopy = new int* [M];
    for (int i = 0; i < M; ++i)
    {
        matcopy[i] = new int[N];
        for (int j = 0; j < N; ++j)
        {
            matcopy[i][j] = 0;
        }
    }

    NextPos = new int[8];
    NextRot = 0;
    OptimalScores = new int[4];

    firstframe = true;
}



void AutomaticInstructions::UpdatePossibilities(TetrisGame tetrisgame, int type)
// Calcuates every possible position a given tetromino type can access by starting on top of the screen (on any column) and only moving down
{
    Tetromino tetromino(type, 0, 0, 0, 0, 0, 0, 0, 0);
    tetromino.SetAtSpawn(type);

    for (int i = 0; i < 4; ++i)      // Every rotation
    {
        for (int j = 0; j < N; ++j)     // Every position of pos1j
        {
            for (int k = 0; k < 8; ++k)
            {
                possibilities[i][j][k] = -1;           // If a configuration is physically not possible, possibilities[i][j][k] will remain at -1
            }
        }
    }


    for (int rot = 0; rot < 4; ++rot)
    {
        // Tetromino of the same type, with rotation rot and that spawns in the upper left corner

        Tetromino crashtetromino(type, 0, 0, 0, 0, 0, 0, 0, 0);
        crashtetromino.SetAtSpawn(type);

        for (int i = 0; i < rot; ++i)
        {
            crashtetromino.ForceRotate(true);
        }
        while (!(crashtetromino.pos1i <= 0 || crashtetromino.pos2i <= 0 || crashtetromino.pos3i <= 0 || crashtetromino.pos4i <= 0))
        {
            crashtetromino.ForceMove(-1, 0);
        }

        while (!(crashtetromino.pos1j <= 0 || crashtetromino.pos2j <= 0 || crashtetromino.pos3j <= 0 || crashtetromino.pos4j <= 0))
        {
            crashtetromino.ForceMove(0, -1);
        }

        // Calculates how many times the tetromino can move down for each possible value of pos1j 

        for (int j = 0; j < N; ++j)
        {
            if (crashtetromino.pos1j <= N - 1 && crashtetromino.pos2j <= N - 1 && crashtetromino.pos3j <= N - 1 && crashtetromino.pos4j <= N - 1)
            {
                int nbdown = 0;
                while (crashtetromino.CanMove(1, 0, tetrisgame))
                {
                    crashtetromino.Move(1, 0, tetrisgame);
                    nbdown += 1;
                }

                if (tetrisgame.Tetrismat[crashtetromino.pos1i][crashtetromino.pos1j] == 0 && tetrisgame.Tetrismat[crashtetromino.pos2i][crashtetromino.pos2j] == 0 && tetrisgame.Tetrismat[crashtetromino.pos3i][crashtetromino.pos3j] == 0 && tetrisgame.Tetrismat[crashtetromino.pos4i][crashtetromino.pos4j] == 0)
                {
                    possibilities[rot][crashtetromino.pos1j][0] = crashtetromino.pos1i;
                    possibilities[rot][crashtetromino.pos1j][1] = crashtetromino.pos1j;
                    possibilities[rot][crashtetromino.pos1j][2] = crashtetromino.pos2i;
                    possibilities[rot][crashtetromino.pos1j][3] = crashtetromino.pos2j;
                    possibilities[rot][crashtetromino.pos1j][4] = crashtetromino.pos3i;
                    possibilities[rot][crashtetromino.pos1j][5] = crashtetromino.pos3j;
                    possibilities[rot][crashtetromino.pos1j][6] = crashtetromino.pos4i;
                    possibilities[rot][crashtetromino.pos1j][7] = crashtetromino.pos4j;
                }

                crashtetromino.ForceMove(-nbdown, 1);
            }

        }
    }

}



void AutomaticInstructions::UpdateScores(TetrisGame tetrisgame, int type)
// Updates the scores associated to every possibility previously calculated
{
    for (int i = 0; i < M; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            matcopy[i][j] = tetrisgame.Tetrismat[i][j];
        }
    }

    int lines[5] = { -1, -1, -1, -1, -1 };
    // Holes and jumps before adding the tetromino
    int holesbeginning = CountHoles(matcopy, lines);
    int jumpsbeginning = JumpsNumber(matcopy, lines, type);
    int indlines = 0;

    for (int rot = 0; rot < 4; ++rot)
    {
        for (int i = 0; i < N; ++i)
        {
            if (possibilities[rot][i][0] == -1)
            {
                createdholes[rot][i] = M * N;
                jumpscreated[rot][i] = M * N;
                erasedlines[rot][i] = -(int)M;
                maxheight[rot][i] = -(int)M;
            }
            else
            {
                for (int k = 0; k < 4; ++k)
                {
                    matcopy[possibilities[rot][i][2 * k]][possibilities[rot][i][2 * k + 1]] = 1;
                }

                // Calcultes the full lines

                int nblines = 0;
                indlines = 0;
                lines[0] = -1; lines[1] = -1; lines[2] = -1; lines[3] = -1; lines[4] = -1;

                for (int i = M - 1; i >= 0; --i)
                {
                    bool fulline = true;
                    for (int j = 0; j < N; ++j)
                    {
                        if (matcopy[i][j] == 0)
                            fulline = false;
                    }
                    if (fulline)
                    {
                        nblines += 1;
                        lines[indlines] = i;
                        indlines += 1;
                    }
                }

                // Calculates the 4 quantities useful for determining the score at each position

                erasedlines[rot][i] = nblines;
                createdholes[rot][i] = holesbeginning - CountHoles(matcopy, lines) - 4;
                jumpscreated[rot][i] = JumpsNumber(matcopy, lines, type) - jumpsbeginning;
                maxheight[rot][i] = std::min(std::min(possibilities[rot][i][0], possibilities[rot][i][2]), std::min(possibilities[rot][i][4], possibilities[rot][i][6]));

                // We reset matcopy for the next step

                for (int k = 0; k < 4; ++k)
                {
                    matcopy[possibilities[rot][i][2 * k]][possibilities[rot][i][2 * k + 1]] = 0;
                }
            }
        }
    }
}



void AutomaticInstructions::UpdateNextMove(TetrisGame tetrisgame, int type)
// We calculate the optimal position for the next move, as the position that has the best score between every possibilities. We also calculate its corresponding score
{
    UpdatePossibilities(tetrisgame, type);
    UpdateScores(tetrisgame, type);

    int rotsel = 0;
    int possel = 0;

    int minweight = INT_MAX;

    for (int rot = 0; rot < 4; ++rot)
    {
        for (int i = 0; i < N; ++i)
        {
            if (possibilities[rot][i][0] != -1 && Score(createdholes[rot][i], erasedlines[rot][i], jumpscreated[rot][i], maxheight[rot][i]) < minweight)
            {
                minweight = Score(createdholes[rot][i], erasedlines[rot][i], jumpscreated[rot][i], maxheight[rot][i]);
                rotsel = rot;
                possel = i;
            }
        }
    }


    for (int k = 0; k < 8; ++k)
    {
        NextPos[k] = possibilities[rotsel][possel][k];
    }
    NextRot = rotsel;
    OptimalScores[0] = createdholes[rotsel][possel];
    OptimalScores[1] = erasedlines[rotsel][possel];
    OptimalScores[2] = jumpscreated[rotsel][possel];
    OptimalScores[3] = maxheight[rotsel][possel];
}



int AutomaticInstructions::GetScore()
{
    return Score(OptimalScores[0], OptimalScores[1], OptimalScores[2], OptimalScores[3]);
}



int* AutomaticInstructions::GetNextPos()
{
    return NextPos;
}



int AutomaticInstructions::RotationInstruction(Tetromino tetromino)
// How do we have to rotate to get where we want?
{
    int rotationdifference = modd(NextRot - tetromino.rotationstate, 4);

    switch (rotationdifference)
    {
    case(0): return 0; break;
    case(1, 2): return 1; break;
    case(3): return -1; break;
    }
}



int AutomaticInstructions::MoveInstruction(Tetromino tetromino)
// How do we have to move to get where we want?
{
    if (NextPos[1] > tetromino.pos1j)
        return 1;
    else if (NextPos[1] < tetromino.pos1j)
        return -1;
    else
        return 0;
}