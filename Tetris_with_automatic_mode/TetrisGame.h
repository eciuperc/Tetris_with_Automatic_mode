#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <stdint.h>
#include <random>
#include <vector>
#include <chrono>

const unsigned int M = 20;                     //Vertical size of the game
const unsigned int N = 10;                     //Horizontal size of the game
const float FIELD_HEIGHT = 762 - 60;
const float FIELD_WIDTH = N * FIELD_HEIGHT / M;
const float TILE_SIZE = FIELD_HEIGHT / M;
// frames per move in function of the level
//                                0   1   2   3   4   5   6   7  8  9   10-12   13-15   16-18    19-28    29+     
const int FRAME_PER_MOVE[15] = { 48, 43, 38, 33, 28, 23, 18, 13, 8, 6,      5,      4,      3,       2,     1 };
// number of frames we have to wait when a tetromino can't go don't anymore until he is integrated to the tetris game
//                               0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29+ 
const int max_framerate[30] = { 96, 86, 81, 76, 70, 63, 50, 47, 44, 40, 38, 36, 34, 33, 30, 27, 24, 21, 18, 16, 14, 13, 12, 11, 10,  8,  7,  6,  5,   4 };

class TetrisGame
{
public:
	TetrisGame(int level, sf::Texture* TileTexture1, sf::Texture* TileTexture2, sf::Texture* TileTexture3, sf::Texture* TileTexture4, sf::Texture* TileTexture5, sf::Texture* TileTexture6, sf::Texture* TileTexture7,
		sf::Texture* TetrominoTexture1, sf::Texture* TetrominoTexture2, sf::Texture* TetrominoTexture3, sf::Texture* TetrominoTexture4, sf::Texture* TetrominoTexture5, sf::Texture* TetrominoTexture6, sf::Texture* TetrominoTexture7);

	// Update functions
	void UpdateSpawnArray();
	void ClearLines(int* lines);

	int Swap(int type);          // Swap between the current tetromino and the held tetromino (if there is one).

	// Draws the game starting at line imin (usually imin = 0, except in the "DisplayFallingAnimation" function)
	void Draw(sf::RenderWindow& window, int imin);

	// Clears the matrix
	void TetrisGameClear();

public:
	//Textures
	sf::Texture* TileTexture1;
	sf::Texture* TileTexture2;
	sf::Texture* TileTexture3;
	sf::Texture* TileTexture4;
	sf::Texture* TileTexture5;
	sf::Texture* TileTexture6;
	sf::Texture* TileTexture7;
	sf::Texture* TetrominoTexture1;
	sf::Texture* TetrominoTexture2;
	sf::Texture* TetrominoTexture3;
	sf::Texture* TetrominoTexture4;
	sf::Texture* TetrominoTexture5;
	sf::Texture* TetrominoTexture6;
	sf::Texture* TetrominoTexture7;

	int** Tetrismat;      // Game's matrix
	int* spawnarray;      // Next tetrominos that will come (useful for showing the next tetrominos)
	int indspawn;         // Index of the currenttetromino in spawnarray
	int heldtet;          // Type of the held tetromino

	int level;
	int64_t score;
	int linescleared;
	int framespermove;    // Number of frames we have to wait before the tetromino falls by one block. Depends on the level.

	// For calculating the time between when a tetromino can't go down anymore and when it is integrated to the game
	float timeint;
	bool timeintstarted;
	int maxintframerate;

	std::vector<int> shuf;    // For generatic random tetromino spawning

	bool safemode;    // This mode is for automatic mode with instantmode on. It always displays the tiles in blue, which doesn't hurt your eyes.
};


class Tetromino
{
public:
	Tetromino(int type, int pos1i, int pos1j, int pos2i, int pos2j, int pos3i, int pos3j, int pos4i, int pos4j);

	// Tetromino's movement
	void Rotate(bool Clockwise, TetrisGame& tetrisgame);
	void ForceRotate(bool Clockwise);
	bool CanMove(int i, int j, TetrisGame tetrisgame);
	void Move(int i, int j, TetrisGame& tetrisgame);
	void ForceMove(int i, int j);
	void ForceChange(int type, int pos1i, int pos1j, int pos2i, int pos2j, int pos3i, int pos3j, int pos4i, int pos4j);
	void ForceChange(int type, int* posarray);
	void SetAtSpawn(int type);

	// Integrate to the TetrisGame
	void Integrate(TetrisGame& tetrisgame);
	// Check if we lost due to the tetromino
	bool CheckDeath(TetrisGame tetrisgame);

	// Draw the tetromino in the TetrisGame
	void Draw(sf::RenderWindow& window, TetrisGame tetrisgame);

public:
	int type;

	// Position and rotation state
	int rotationstate;
	int pos1i;
	int pos1j;
	int pos2i;
	int pos2j;
	int pos3i;
	int pos3j;
	int pos4i;
	int pos4j;

private:
	// Position of the rotation center (see https://tetris.fandom.com/wiki/SRS)
	float posrci;
	float posrcj;
};