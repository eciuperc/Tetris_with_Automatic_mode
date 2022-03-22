#include "TetrisGame.h"
#include <math.h>

// Same as "%", but always returns values between 0 and y-1. Useful for updating Tetromino's rotationstate variable.
int mod(int x, int y)
{
	int r = x % y;
	if (r >= 0)
		return r;
	else
		return r + y;
}

// For calculating TetrisGame's "framespermove" variable
int Frames_Per_Move(int level)
{

	if (level <= 9)
		return FRAME_PER_MOVE[level];

	if (level >= 10 && level <= 12)
		return FRAME_PER_MOVE[10];

	if (level >= 13 && level <= 15)
		return FRAME_PER_MOVE[11];

	if (level >= 16 && level <= 18)
		return FRAME_PER_MOVE[12];

	if (level >= 19 && level <= 28)
		return FRAME_PER_MOVE[13];

	if (level >= 29)
		return FRAME_PER_MOVE[14];
}

void DisplayArray(int* array, int size)
{
	for (int i = 0; i < size; ++i)
	{
		std::cout << array[i] << ' ';
	}
	std::cout << std::endl;
}



TetrisGame::TetrisGame(int level, sf::Texture* TileTexture1, sf::Texture* TileTexture2, sf::Texture* TileTexture3, sf::Texture* TileTexture4, sf::Texture* TileTexture5, sf::Texture* TileTexture6, sf::Texture* TileTexture7,
	sf::Texture* TetrominoTexture1, sf::Texture* TetrominoTexture2, sf::Texture* TetrominoTexture3, sf::Texture* TetrominoTexture4, sf::Texture* TetrominoTexture5, sf::Texture* TetrominoTexture6, sf::Texture* TetrominoTexture7)
{
	Tetrismat = new int* [M];
	for (int i = 0; i < M; ++i)
	{
		Tetrismat[i] = new int[N];
		for (int j = 0; j < N; ++j)
		{
			Tetrismat[i][j] = 0;
		}
	}

	shuf = { 1,2,3,4,5,6,7 };
	spawnarray = new int[14];
	unsigned num = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(shuf.begin(), shuf.end(), std::default_random_engine(num));
	int ind = 7;
	for (int& x : shuf)
	{
		spawnarray[ind] = x;
		ind += 1;
	}
	indspawn = 7;
	heldtet = 0;

	this->TileTexture1 = TileTexture1;
	this->TileTexture2 = TileTexture2;
	this->TileTexture3 = TileTexture3;
	this->TileTexture4 = TileTexture4;
	this->TileTexture5 = TileTexture5;
	this->TileTexture6 = TileTexture6;
	this->TileTexture7 = TileTexture7;
	this->TetrominoTexture1 = TetrominoTexture1;
	this->TetrominoTexture2 = TetrominoTexture2;
	this->TetrominoTexture3 = TetrominoTexture3;
	this->TetrominoTexture4 = TetrominoTexture4;
	this->TetrominoTexture5 = TetrominoTexture5;
	this->TetrominoTexture6 = TetrominoTexture6;
	this->TetrominoTexture7 = TetrominoTexture7;
	score = 0;
	this->level = level;
	framespermove = Frames_Per_Move(level);
	linescleared = 0;

	timeint = 0;
	if (level <= 29)
		maxintframerate = max_framerate[level];
	else
	{
		maxintframerate = max_framerate[29];
	}
	timeintstarted = false;

	safemode = false;
}




// Update functions

void TetrisGame::UpdateSpawnArray()
// spawnarray is an array of size 14. When we apply this function, its 7 last values becomes its 7 first values
// and its 7 last values are replaced by a random permutation of {1,2,3,4,5,6,7}
{
	unsigned num = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(shuf.begin(), shuf.end(), std::default_random_engine(num));
	int ind = 0;
	for (int& x : shuf)
	{
		spawnarray[ind] = spawnarray[7 + ind];
		spawnarray[7 + ind] = x;
		ind += 1;
	}
}



void TetrisGame::ClearLines(int* lines)
{
	int nblines = 0;

	// Detects which line numbers are full and stocks these numbers in "lines"

	for (int i = M - 1; i >= 0; --i)
	{
		bool cleari = true;
		for (int j = 0; j < N; ++j)
		{
			if (Tetrismat[i][j] == 0)
			{
				cleari = false;
			}
		}

		if (cleari)
		{
			lines[nblines] = i;
			nblines += 1;
			for (int j = 0; j < N; ++j)
			{
				Tetrismat[i][j] = 0;
			}
		}
	}

	// We descend the rest of the matrix

	for (int i = 0; i < nblines; ++i)
	{
		for (int ii = lines[i] - 1; ii > lines[i + 1]; --ii)
		{
			for (int j = 0; j < N; ++j)
			{
				Tetrismat[ii + i + 1][j] = Tetrismat[ii][j];
			}
		}
	}
	for (int i = 0; i < nblines; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
			Tetrismat[i][j] = 0;
		}
	}



	// Update score

	if (lines[0] != -1 && lines[1] != -1 && lines[2] != -1 && lines[3] != -1)
	{
		score += level * 800;
		linescleared += 4;
	}
	else
	{
		for (int i = 0; i < 4; ++i)
		{
			if (lines[i] != -1)
			{
				score += level * 100;
				linescleared += 1;
			}
			if (lines[i + 1] != -1 && lines[i + 1] == lines[i] - 1)
			{
				score += level * 100;
			}
		}
	}

	// Update level and game's difficulty

	level = linescleared / 10 + 1;
	framespermove = Frames_Per_Move(level);
	if (level <= 29)
		maxintframerate = max_framerate[level];
	else
	{
		maxintframerate = max_framerate[29];
	}
}



int TetrisGame::Swap(int type)
// Swap between the current tetromino and the held tetromino (if there is one). Returns the type of the new current tetromino after the operation
{
	timeintstarted = false;
	int temp = heldtet;
	if (temp == 0)
	{
		heldtet = type;
		spawnarray[indspawn] = type;
		indspawn += 1;
		return spawnarray[indspawn];
	}
	else
	{
		heldtet = type;
		spawnarray[indspawn] = type;
		return temp;
	}
}



void TetrisGame::Draw(sf::RenderWindow& window, int imin)
// Draws the game starting at line imin (usually imin = 0, except in the "DisplayFallingAnimation" function)
{
	// Drawing Tetmatrix

	for (int i = imin; i < M; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
			if (Tetrismat[i][j] > 0)
			{
				sf::RectangleShape tile;
				tile.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
				tile.setPosition(372 + TILE_SIZE * j, 60 + TILE_SIZE * i);
				switch (Tetrismat[i][j])
				{
				case(1):
					tile.setTexture(TileTexture1); break;
				case(2):
					tile.setTexture(TileTexture2); break;
				case(3):
					tile.setTexture(TileTexture3); break;
				case(4):
					tile.setTexture(TileTexture4); break;
				case(5):
					tile.setTexture(TileTexture5); break;
				case(6):
					tile.setTexture(TileTexture6); break;
				case(7):
					tile.setTexture(TileTexture7); break;
				}

				if (safemode)
					tile.setTexture(TileTexture3);

				window.draw(tile);
			}
		}
	}






	// Drawing the 3 next tetrominos

	for (int i = 1; i <= 3; ++i)
	{
		sf::RectangleShape nexttet;

		int tettype = spawnarray[indspawn + i];
		switch (tettype)
		{
		case(1):
		{
			nexttet.setSize(sf::Vector2f(4 * TILE_SIZE, TILE_SIZE));
			nexttet.setOrigin(2 * TILE_SIZE, TILE_SIZE / 2.f);
			break;
		}
		case(2):
		{
			nexttet.setSize(sf::Vector2f(2 * TILE_SIZE, 2 * TILE_SIZE));
			nexttet.setOrigin(TILE_SIZE, TILE_SIZE);
			break;
		}
		default:
		{
			nexttet.setSize(sf::Vector2f(3 * TILE_SIZE, 2 * TILE_SIZE));
			nexttet.setOrigin(3.f * TILE_SIZE / 2.f, TILE_SIZE);
			break;
		}
		}

		nexttet.setPosition((811 + 977) / 2.f, (1.5f + 3.f * (i - 1)) / 9 * 427 + (9 - (1.5f + 3.f * (i - 1))) / 9 * 138);

		switch (tettype)
		{
		case(1):
			nexttet.setTexture(TetrominoTexture1); break;
		case(2):
			nexttet.setTexture(TetrominoTexture2); break;
		case(3):
			nexttet.setTexture(TetrominoTexture3); break;
		case(4):
			nexttet.setTexture(TetrominoTexture4); break;
		case(5):
			nexttet.setTexture(TetrominoTexture5); break;
		case(6):
			nexttet.setTexture(TetrominoTexture6); break;
		case(7):
			nexttet.setTexture(TetrominoTexture7); break;
		}

		if (safemode)
			nexttet.setTexture(TileTexture3);

		window.draw(nexttet);

	}




	// Drawing held tetromino

	if (heldtet > 0)
	{
		sf::RectangleShape heldtetro;

		switch (heldtet)
		{
		case(1):
		{
			heldtetro.setSize(sf::Vector2f(4 * TILE_SIZE, TILE_SIZE));
			heldtetro.setOrigin(2 * TILE_SIZE, TILE_SIZE / 2.f);
			break;
		}
		case(2):
		{
			heldtetro.setSize(sf::Vector2f(2 * TILE_SIZE, 2 * TILE_SIZE));
			heldtetro.setOrigin(TILE_SIZE, TILE_SIZE);
			break;
		}
		default:
		{
			heldtetro.setSize(sf::Vector2f(3 * TILE_SIZE, 2 * TILE_SIZE));
			heldtetro.setOrigin(3.f * TILE_SIZE / 2.f, TILE_SIZE);
			break;
		}
		}

		heldtetro.setPosition((116 + 286) / 2.f, (135 + 249) / 2.f);

		switch (heldtet)
		{
		case(1):
			heldtetro.setTexture(TetrominoTexture1); break;
		case(2):
			heldtetro.setTexture(TetrominoTexture2); break;
		case(3):
			heldtetro.setTexture(TetrominoTexture3); break;
		case(4):
			heldtetro.setTexture(TetrominoTexture4); break;
		case(5):
			heldtetro.setTexture(TetrominoTexture5); break;
		case(6):
			heldtetro.setTexture(TetrominoTexture6); break;
		case(7):
			heldtetro.setTexture(TetrominoTexture7); break;
		}

		if (safemode)
			heldtetro.setTexture(TileTexture3);

		window.draw(heldtetro);
	}
}



void TetrisGame::TetrisGameClear()
{
	for (int i = 0; i < M; ++i)
	{
		delete[]Tetrismat[i];
	}
	delete[]Tetrismat;
}








Tetromino::Tetromino(int type, int pos1i, int pos1j, int pos2i, int pos2j, int pos3i, int pos3j, int pos4i, int pos4j)
{
	this->type = type;

	rotationstate = 0;
	this->pos1i = pos1i;
	this->pos1j = pos1j;
	this->pos2i = pos2i;
	this->pos2j = pos2j;
	this->pos3i = pos3i;
	this->pos3j = pos3j;
	this->pos4i = pos4i;
	this->pos4j = pos4j;

	switch (type)
	{
	case(1):
	{
		posrci = pos3i + 1;
		posrcj = pos3j;
		break;
	}
	case(2):
	{
		posrci = pos4i;
		posrcj = pos4j;
		break;
	}
	case(3): case(6): case(7):
	{
		posrci = pos3i + 0.5f;
		posrcj = pos3j + 0.5f;
		break;
	}
	case(4, 5):
	{
		posrci = pos2i + 0.5f;
		posrcj = pos2j + 0.5f;
		break;
	}
	default:
	{
		posrci = pos2i + 0.5f;
		posrcj = pos2j + 0.5f;
		break;
	}
	}
}



void Tetromino::Rotate(bool Clockwise, TetrisGame& tetrisgame)
// Rotation mecanisms obtained on https://tetris.fandom.com/wiki/SRS
{
	int oldpos1i = pos1i;
	int oldpos1j = pos1j;
	int oldpos2i = pos2i;
	int oldpos2j = pos2j;
	int oldpos3i = pos3i;
	int oldpos3j = pos3j;
	int oldpos4i = pos4i;
	int oldpos4j = pos4j;

	if (Clockwise)
	{
		rotationstate = mod(rotationstate + 1, 4);
		float posc1i = pos1i + 0.5f;
		float posc1j = pos1j + 0.5f;
		float posc2i = pos2i + 0.5f;
		float posc2j = pos2j + 0.5f;
		float posc3i = pos3i + 0.5f;
		float posc3j = pos3j + 0.5f;
		float posc4i = pos4i + 0.5f;
		float posc4j = pos4j + 0.5f;

		float temp1 = posc1i;
		posc1i = posrci + posc1j - posrcj;
		posc1j = posrcj - temp1 + posrci;

		float temp2 = posc2i;
		posc2i = posrci + posc2j - posrcj;
		posc2j = posrcj - temp2 + posrci;

		float temp3 = posc3i;
		posc3i = posrci + posc3j - posrcj;
		posc3j = posrcj - temp3 + posrci;

		float temp4 = posc4i;
		posc4i = posrci + posc4j - posrcj;
		posc4j = posrcj - temp4 + posrci;

		pos1i = floor(posc1i);
		pos1j = floor(posc1j);
		pos2i = floor(posc2i);
		pos2j = floor(posc2j);
		pos3i = floor(posc3i);
		pos3j = floor(posc3j);
		pos4i = floor(posc4i);
		pos4j = floor(posc4j);
	}

	else
	{
		rotationstate = mod(rotationstate - 1, 4);
		float posc1i = pos1i + 0.5f;
		float posc1j = pos1j + 0.5f;
		float posc2i = pos2i + 0.5f;
		float posc2j = pos2j + 0.5f;
		float posc3i = pos3i + 0.5f;
		float posc3j = pos3j + 0.5f;
		float posc4i = pos4i + 0.5f;
		float posc4j = pos4j + 0.5f;

		float temp1 = posc1i;
		posc1i = posrcj - posc1j + posrci;
		posc1j = posrcj + temp1 - posrci;

		float temp2 = posc2i;
		posc2i = posrcj - posc2j + posrci;
		posc2j = posrcj + temp2 - posrci;

		float temp3 = posc3i;
		posc3i = posrcj - posc3j + posrci;
		posc3j = posrcj + temp3 - posrci;

		float temp4 = posc4i;
		posc4i = posrcj - posc4j + posrci;
		posc4j = posrcj + temp4 - posrci;

		pos1i = floor(posc1i);
		pos1j = floor(posc1j);
		pos2i = floor(posc2i);
		pos2j = floor(posc2j);
		pos3i = floor(posc3i);
		pos3j = floor(posc3j);
		pos4i = floor(posc4i);
		pos4j = floor(posc4j);
	}

	bool canrotate = true;

	if (pos1i < 0 || pos1i >= M || pos1j < 0 || pos1j >= N || tetrisgame.Tetrismat[pos1i][pos1j] > 0)
		canrotate = false;
	if (pos2i < 0 || pos2i >= M || pos2j < 0 || pos2j >= N || tetrisgame.Tetrismat[pos2i][pos2j] > 0)
		canrotate = false;
	if (pos3i < 0 || pos3i >= M || pos3j < 0 || pos3j >= N || tetrisgame.Tetrismat[pos3i][pos3j] > 0)
		canrotate = false;
	if (pos4i < 0 || pos4i >= M || pos4j < 0 || pos4j >= N || tetrisgame.Tetrismat[pos4i][pos4j] > 0)
		canrotate = false;

	if (canrotate)
	{
		tetrisgame.timeintstarted = false;
		tetrisgame.timeint = 0;
	}
	else                // If rotation doesn't work, we try wallkicks
	{
		bool wallkick = false;
		int wallkickx[4] = { 0,0,0,0 };
		int wallkicky[4] = { 0,0,0,0 };

		if (type == 1)
		{
			switch (rotationstate)
			{
			case(0):
			{
				if (Clockwise)
				{
					wallkickx[0] = -2; wallkicky[0] = 0;
					wallkickx[1] = 1; wallkicky[1] = 0;
					wallkickx[2] = -2; wallkicky[2] = -1;
					wallkickx[3] = 1; wallkicky[3] = 2;
				}
				else
				{
					wallkickx[0] = -1; wallkicky[0] = 0;
					wallkickx[1] = 2; wallkicky[1] = 0;
					wallkickx[2] = -1; wallkicky[2] = 2;
					wallkickx[3] = 2; wallkicky[3] = -1;
				}
				break;
			}
			case(1):
			{
				if (Clockwise)
				{
					wallkickx[0] = -1; wallkicky[0] = 0;
					wallkickx[1] = 2; wallkicky[1] = 0;
					wallkickx[2] = -1; wallkicky[2] = 2;
					wallkickx[3] = 2; wallkicky[3] = -1;
				}
				else
				{
					wallkickx[0] = 2; wallkicky[0] = 0;
					wallkickx[1] = -1; wallkicky[1] = 0;
					wallkickx[2] = 2; wallkicky[2] = 1;
					wallkickx[3] = -1; wallkicky[3] = -2;
				}
				break;
			}
			case(2):
			{
				if (Clockwise)
				{
					wallkickx[0] = 2; wallkicky[0] = 0;
					wallkickx[1] = -1; wallkicky[1] = 0;
					wallkickx[2] = 2; wallkicky[2] = 1;
					wallkickx[3] = -1; wallkicky[3] = -2;
				}
				else
				{
					wallkickx[0] = 1; wallkicky[0] = 0;
					wallkickx[1] = -2; wallkicky[1] = 0;
					wallkickx[2] = 1; wallkicky[2] = -2;
					wallkickx[3] = -2; wallkicky[3] = 1;
				}
				break;
			}
			case(3):
			{
				if (Clockwise)
				{
					wallkickx[0] = 1; wallkicky[0] = 0;
					wallkickx[1] = -2; wallkicky[1] = 0;
					wallkickx[2] = 1; wallkicky[2] = -2;
					wallkickx[3] = -2; wallkicky[3] = 1;
				}
				else
				{
					wallkickx[0] = -2; wallkicky[0] = 0;
					wallkickx[1] = 1; wallkicky[1] = 0;
					wallkickx[2] = -2; wallkicky[2] = -1;
					wallkickx[3] = 1; wallkicky[3] = 2;
				}
				break;
			}
			default:
			{
				break;
			}
			}
		}

		else if (type >= 3 && type <= 7)
		{
			switch (rotationstate)
			{
			case(0):
			{
				if (Clockwise)
				{
					wallkickx[0] = -1; wallkicky[0] = 0;
					wallkickx[1] = -1; wallkicky[1] = 1;
					wallkickx[2] = 0; wallkicky[2] = -2;
					wallkickx[3] = -1; wallkicky[3] = -2;
				}
				else
				{
					wallkickx[0] = 1; wallkicky[0] = 0;
					wallkickx[1] = 1; wallkicky[1] = 1;
					wallkickx[2] = 0; wallkicky[2] = -2;
					wallkickx[3] = 1; wallkicky[3] = -2;
				}
				break;
			}
			case(1):
			{
				if (Clockwise)
				{
					wallkickx[0] = 1; wallkicky[0] = 0;
					wallkickx[1] = 1; wallkicky[1] = -1;
					wallkickx[2] = 0; wallkicky[2] = 2;
					wallkickx[3] = 1; wallkicky[3] = 2;
				}
				else
				{
					wallkickx[0] = 1; wallkicky[0] = 0;
					wallkickx[1] = 1; wallkicky[1] = -1;
					wallkickx[2] = 0; wallkicky[2] = 2;
					wallkickx[3] = 1; wallkicky[3] = 2;
				}
				break;
			}
			case(2):
			{
				if (Clockwise)
				{
					wallkickx[0] = 1; wallkicky[0] = 0;
					wallkickx[1] = 1; wallkicky[1] = 1;
					wallkickx[2] = 0; wallkicky[2] = -2;
					wallkickx[3] = 1; wallkicky[3] = -2;
				}
				else
				{
					wallkickx[0] = -1; wallkicky[0] = 0;
					wallkickx[1] = -1; wallkicky[1] = 1;
					wallkickx[2] = 0; wallkicky[2] = -2;
					wallkickx[3] = -1; wallkicky[3] = -2;
				}
				break;
			}
			case(3):
			{
				if (Clockwise)
				{
					wallkickx[0] = -1; wallkicky[0] = 0;
					wallkickx[1] = -1; wallkicky[1] = -1;
					wallkickx[2] = 0; wallkicky[2] = 2;
					wallkickx[3] = -1; wallkicky[3] = 2;
				}
				else
				{
					wallkickx[0] = -1; wallkicky[0] = 0;
					wallkickx[1] = -1; wallkicky[1] = -1;
					wallkickx[2] = 0; wallkicky[2] = 2;
					wallkickx[3] = -1; wallkicky[3] = 2;
				}
				break;
			}
			default:
			{
				break;
			}
			}
		}

		for (int i = 0; i < 4; ++i)
		{
			if (CanMove(-wallkicky[i], wallkickx[i], tetrisgame) && !wallkick)
			{
				Move(-wallkicky[i], wallkickx[i], tetrisgame);
				wallkick = true;
			}
		}

		if (!wallkick)
		{
			pos1i = oldpos1i;
			pos1j = oldpos1j;
			pos2i = oldpos2i;
			pos2j = oldpos2j;
			pos3i = oldpos3i;
			pos3j = oldpos3j;
			pos4i = oldpos4i;
			pos4j = oldpos4j;
		}
		else
		{
			tetrisgame.timeintstarted = false;
			tetrisgame.timeint = 0;
		}
	}

}



void Tetromino::ForceRotate(bool Clockwise)
// We rotate no matter if it is physically possible or not. Useful for automatic mode to try every possibilities
{
	if (Clockwise)
	{
		rotationstate = mod(rotationstate + 1, 4);
		float posc1i = pos1i + 0.5f;
		float posc1j = pos1j + 0.5f;
		float posc2i = pos2i + 0.5f;
		float posc2j = pos2j + 0.5f;
		float posc3i = pos3i + 0.5f;
		float posc3j = pos3j + 0.5f;
		float posc4i = pos4i + 0.5f;
		float posc4j = pos4j + 0.5f;

		float temp1 = posc1i;
		posc1i = posrci + posc1j - posrcj;
		posc1j = posrcj - temp1 + posrci;

		float temp2 = posc2i;
		posc2i = posrci + posc2j - posrcj;
		posc2j = posrcj - temp2 + posrci;

		float temp3 = posc3i;
		posc3i = posrci + posc3j - posrcj;
		posc3j = posrcj - temp3 + posrci;

		float temp4 = posc4i;
		posc4i = posrci + posc4j - posrcj;
		posc4j = posrcj - temp4 + posrci;

		pos1i = floor(posc1i);
		pos1j = floor(posc1j);
		pos2i = floor(posc2i);
		pos2j = floor(posc2j);
		pos3i = floor(posc3i);
		pos3j = floor(posc3j);
		pos4i = floor(posc4i);
		pos4j = floor(posc4j);
	}

	else
	{
		rotationstate = mod(rotationstate - 1, 4);
		float posc1i = pos1i + 0.5f;
		float posc1j = pos1j + 0.5f;
		float posc2i = pos2i + 0.5f;
		float posc2j = pos2j + 0.5f;
		float posc3i = pos3i + 0.5f;
		float posc3j = pos3j + 0.5f;
		float posc4i = pos4i + 0.5f;
		float posc4j = pos4j + 0.5f;

		float temp1 = posc1i;
		posc1i = posrcj - posc1j + posrci;
		posc1j = posrcj + temp1 - posrci;

		float temp2 = posc2i;
		posc2i = posrcj - posc2j + posrci;
		posc2j = posrcj + temp2 - posrci;

		float temp3 = posc3i;
		posc3i = posrcj - posc3j + posrci;
		posc3j = posrcj + temp3 - posrci;

		float temp4 = posc4i;
		posc4i = posrcj - posc4j + posrci;
		posc4j = posrcj + temp4 - posrci;

		pos1i = floor(posc1i);
		pos1j = floor(posc1j);
		pos2i = floor(posc2i);
		pos2j = floor(posc2j);
		pos3i = floor(posc3i);
		pos3j = floor(posc3j);
		pos4i = floor(posc4i);
		pos4j = floor(posc4j);
	}
}



bool Tetromino::CanMove(int i, int j, TetrisGame tetrisgame)
// Can we move the tetromino in tetrisgame by an (i,j) offset ?
{
	if (pos1i + i < 0 || pos1i + i >= M || pos1j + j < 0 || pos1j + j >= N || tetrisgame.Tetrismat[pos1i + i][pos1j + j] > 0)
		return false;
	if (pos2i + i < 0 || pos2i + i >= M || pos2j + j < 0 || pos2j + j >= N || tetrisgame.Tetrismat[pos2i + i][pos2j + j] > 0)
		return false;
	if (pos3i + i < 0 || pos3i + i >= M || pos3j + j < 0 || pos3j + j >= N || tetrisgame.Tetrismat[pos3i + i][pos3j + j] > 0)
		return false;
	if (pos4i + i < 0 || pos4i + i >= M || pos4j + j < 0 || pos4j + j >= N || tetrisgame.Tetrismat[pos4i + i][pos4j + j] > 0)
		return false;

	return true;
}



void Tetromino::Move(int i, int j, TetrisGame& tetrisgame)
// If possible, moves the tetromino in tetrisgame by an (i,j) offset
{
	if (CanMove(i, j, tetrisgame))
	{
		tetrisgame.timeintstarted = false;
		tetrisgame.timeint = 0;

		pos1i += i;
		pos1j += j;
		pos2i += i;
		pos2j += j;
		pos3i += i;
		pos3j += j;
		pos4i += i;
		pos4j += j;

		posrci += i;
		posrcj += j;
	}
}



void Tetromino::ForceMove(int i, int j)
// Moves the tetromino by an (i,j) offset no matter whether it is possible physically. Useful for automatic mode, for checking every solution.
{
	pos1i += i;
	pos1j += j;
	pos2i += i;
	pos2j += j;
	pos3i += i;
	pos3j += j;
	pos4i += i;
	pos4j += j;

	posrci += i;
	posrcj += j;
}



void Tetromino::ForceChange(int type, int pos1i, int pos1j, int pos2i, int pos2j, int pos3i, int pos3j, int pos4i, int pos4j)
// Sets the tetromino directly at a given position. Useful for automatic mode, if instant mode is on.
{
	this->type = type;

	// <Attention!!> rotationstate is set to 0 but the position set to tetromino can correspond to another value of rotationstate
	rotationstate = 0;

	this->pos1i = pos1i;
	this->pos1j = pos1j;
	this->pos2i = pos2i;
	this->pos2j = pos2j;
	this->pos3i = pos3i;
	this->pos3j = pos3j;
	this->pos4i = pos4i;
	this->pos4j = pos4j;

	switch (type)
	{
	case(1):
	{
		posrci = pos3i + 1;
		posrcj = pos3j;
		break;
	}
	case(2):
	{
		posrci = pos4i;
		posrcj = pos4j;
		break;
	}
	case(3): case(6): case(7):
	{
		posrci = pos3i + 0.5f;
		posrcj = pos3j + 0.5f;
		break;
	}
	case(4, 5):
	{
		posrci = pos2i + 0.5f;
		posrcj = pos2j + 0.5f;
		break;
	}
	default:
	{
		posrci = pos2i + 0.5f;
		posrcj = pos2j + 0.5f;
		break;
	}
	}
}
void Tetromino::ForceChange(int type, int* posarray)
{
	this->type = type;

	// <Attention!!> rotationstate is set to 0 but the position set to tetromino can correspond to another value of rotationstate
	rotationstate = 0;

	this->pos1i = posarray[0];
	this->pos1j = posarray[1];
	this->pos2i = posarray[2];
	this->pos2j = posarray[3];
	this->pos3i = posarray[4];
	this->pos3j = posarray[5];
	this->pos4i = posarray[6];
	this->pos4j = posarray[7];

	switch (type)
	{
	case(1):
	{
		posrci = pos3i + 1;
		posrcj = pos3j;
		break;
	}
	case(2):
	{
		posrci = pos4i;
		posrcj = pos4j;
		break;
	}
	case(3): case(6): case(7):
	{
		posrci = pos3i + 0.5f;
		posrcj = pos3j + 0.5f;
		break;
	}
	case(4, 5):
	{
		posrci = pos2i + 0.5f;
		posrcj = pos2j + 0.5f;
		break;
	}
	default:
	{
		posrci = pos2i + 0.5f;
		posrcj = pos2j + 0.5f;
		break;
	}
	}
}



void Tetromino::SetAtSpawn(int type)
// Respawns another tetromino of a given type
{
	this->type = type;
	rotationstate = 0;

	switch (type)
	{
	case(1): default:
		pos1i = 1; pos1j = 3; pos2i = 1; pos2j = 4; pos3i = 1; pos3j = 5; pos4i = 1; pos4j = 6; break;
	case(2):
		pos1i = 0; pos1j = 4; pos2i = 0; pos2j = 5; pos3i = 1; pos3j = 4; pos4i = 1; pos4j = 5; break;
	case(3):
		pos1i = 0; pos1j = 3; pos2i = 1; pos2j = 3; pos3i = 1; pos3j = 4; pos4i = 1; pos4j = 5; break;
	case(4):
		pos1i = 1; pos1j = 3; pos2i = 1; pos2j = 4; pos3i = 1; pos3j = 5; pos4i = 0; pos4j = 5; break;
	case(5):
		pos1i = 1; pos1j = 3; pos2i = 1; pos2j = 4; pos3i = 0; pos3j = 4; pos4i = 0; pos4j = 5; break;
	case(6):
		pos1i = 0; pos1j = 4; pos2i = 1; pos2j = 3; pos3i = 1; pos3j = 4; pos4i = 1; pos4j = 5; break;
	case(7):
		pos1i = 0; pos1j = 3; pos2i = 0; pos2j = 4; pos3i = 1; pos3j = 4; pos4i = 1; pos4j = 5; break;
	}

	switch (type)
	{
	case(1):
	{
		posrci = pos3i + 1;
		posrcj = pos3j;
		break;
	}
	case(2):
	{
		posrci = pos4i;
		posrcj = pos4j;
		break;
	}
	case(3): case(6): case(7):
	{
		posrci = pos3i + 0.5f;
		posrcj = pos3j + 0.5f;
		break;
	}
	case(4, 5):
	{
		posrci = pos2i + 0.5f;
		posrcj = pos2j + 0.5f;
		break;
	}
	default:
	{
		posrci = pos2i + 0.5f;
		posrcj = pos2j + 0.5f;
		break;
	}
	}
}



void Tetromino::Integrate(TetrisGame& tetrisgame)
// Integrate the tetromino to tetrisgame
{
	tetrisgame.timeintstarted = false;
	tetrisgame.timeint = 0;

	tetrisgame.Tetrismat[pos1i][pos1j] = type;
	tetrisgame.Tetrismat[pos2i][pos2j] = type;
	tetrisgame.Tetrismat[pos3i][pos3j] = type;
	tetrisgame.Tetrismat[pos4i][pos4j] = type;

	tetrisgame.indspawn += 1;
	if (tetrisgame.indspawn >= 11)
	{
		tetrisgame.UpdateSpawnArray();
		tetrisgame.indspawn -= 7;
	}

}



void Tetromino::Draw(sf::RenderWindow& window, TetrisGame tetrisgame)
// Draws the tetromino
{
	sf::Texture* texture;

	switch (type)
	{
	case(1):
		texture = tetrisgame.TileTexture1; break;
	case(2):
		texture = tetrisgame.TileTexture2; break;
	case(3):
		texture = tetrisgame.TileTexture3; break;
	case(4):
		texture = tetrisgame.TileTexture4; break;
	case(5):
		texture = tetrisgame.TileTexture5; break;
	case(6):
		texture = tetrisgame.TileTexture6; break;
	case(7):
		texture = tetrisgame.TileTexture7; break;
	default:
		texture = tetrisgame.TileTexture7; break;
	}

	sf::RectangleShape tile1;
	tile1.setTexture(texture);
	tile1.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
	tile1.setPosition(372 + TILE_SIZE * pos1j, 60 + TILE_SIZE * pos1i);


	sf::RectangleShape tile2;
	tile2.setTexture(texture);
	tile2.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
	tile2.setPosition(372 + TILE_SIZE * pos2j, 60 + TILE_SIZE * pos2i);


	sf::RectangleShape tile3;
	tile3.setTexture(texture);
	tile3.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
	tile3.setPosition(372 + TILE_SIZE * pos3j, 60 + TILE_SIZE * pos3i);


	sf::RectangleShape tile4;
	tile4.setTexture(texture);
	tile4.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
	tile4.setPosition(372 + TILE_SIZE * pos4j, 60 + TILE_SIZE * pos4i);

	if (tetrisgame.safemode)
	{
		texture = tetrisgame.TileTexture3;
		tile1.setTexture(texture);
		tile2.setTexture(texture);
		tile3.setTexture(texture);
		tile4.setTexture(texture);
	}

	window.draw(tile1);
	window.draw(tile2);
	window.draw(tile3);
	window.draw(tile4);




	//// To draw the rotation center

	//sf::RectangleShape center;
	//center.setFillColor(sf::Color::Red);
	//center.setSize(sf::Vector2f(-4, -4));
	//center.setOrigin(sf::Vector2f(-2, -2));
	//center.setPosition(372 + TILE_SIZE * posrcj, 60 + TILE_SIZE * posrci);

	//window.draw(center);
}



bool Tetromino::CheckDeath(TetrisGame tetrisgame)
// Checks whether we just died
{
	if (tetrisgame.Tetrismat[pos1i][pos1j] != 0 || tetrisgame.Tetrismat[pos2i][pos2j] != 0 || tetrisgame.Tetrismat[pos3i][pos3j] != 0 || tetrisgame.Tetrismat[pos4i][pos4j] != 0)
		return true;
	else
		return false;
}