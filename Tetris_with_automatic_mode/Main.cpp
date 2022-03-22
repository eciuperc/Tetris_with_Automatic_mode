#include <SFML/Audio.hpp>
#include <conio.h>
#include <Windows.h>
#include <time.h>
#include "TetrisGame.h"
#include "KeyboardVariables.h"
#include "AutomaticInstructions.h"
#include <list>


const int FRAMES_PER_SECOND = 60;                   // Like in the NES version, a second has 60 frames
const float FRAME_TIME = 1000.f / FRAMES_PER_SECOND;
const float SLEEP_TIME = FRAME_TIME;
const float VIEW_HEIGHT = 820.f;



void ResizeView(const sf::RenderWindow& window, sf::View& view)
{
    float aspectRatio = float(window.getSize().x) / float(window.getSize().y);
    view.setSize(VIEW_HEIGHT * aspectRatio, VIEW_HEIGHT);
    view.setCenter(548, 411);
}

std::string to_string_withcommas(int64_t a)
// Converts an integer to a string, adding a comma every 3 digit
{
    if (a == 0)
        return "0";

    std::string rep = { };

    int64_t b = a;
    int c;
    int i = 0;
    while (b > 0)
    {
        i += 1;
        if (i % 3 == 1 && i != 1)
            rep.insert(0, 1, ',');
        c = b - b / 10 * 10;
        rep.insert(0, 1, c + '0');
        b = b / 10;
    }

    return rep;
}

void DisplayBackground(sf::RenderWindow& window, sf::Texture* texture, TetrisGame tetrisgame)
{
    window.clear(sf::Color::White);

    // Background texture

    sf::RectangleShape background;
    background.setTexture(texture);
    background.setSize(sf::Vector2f(texture->getSize().x, texture->getSize().y));
    background.setPosition(0, 0);

    window.draw(background);


    sf::Font font;
    font.loadFromFile("arial.ttf");


    // Level

    sf::Text textlevel;

    textlevel.setString(to_string_withcommas(tetrisgame.level));
    textlevel.setFont(font);

    textlevel.setCharacterSize(25);

    sf::FloatRect textlevelRect = textlevel.getLocalBounds();
    textlevel.setOrigin(textlevelRect.left + textlevelRect.width / 2.0f,
        textlevelRect.top + textlevelRect.height / 2.0f);
    textlevel.setPosition((119 + 284) / 2.f, (597 + 638) / 2.f);

    textlevel.setFillColor(sf::Color::White);

    window.draw(textlevel);


    // Lines cleared

    sf::Text textllines;

    textllines.setString(to_string_withcommas(tetrisgame.linescleared));
    textllines.setFont(font);

    textllines.setCharacterSize(25);

    sf::FloatRect textllinesRect = textllines.getLocalBounds();
    textllines.setOrigin(textllinesRect.left + textllinesRect.width / 2.0f,
        textllinesRect.top + textllinesRect.height / 2.0f);
    textllines.setPosition((119 + 284) / 2.f, (678 + 719) / 2.f);

    textllines.setFillColor(sf::Color::White);

    window.draw(textllines);



    // Score

    sf::Text textscore;

    textscore.setString(to_string_withcommas(tetrisgame.score));
    textscore.setFont(font);

    textscore.setCharacterSize(25);

    sf::FloatRect textscoreRect = textscore.getLocalBounds();
    textscore.setOrigin(textscoreRect.left + textscoreRect.width / 2.0f,
        textscoreRect.top + textscoreRect.height / 2.0f);
    textscore.setPosition((119 + 284) / 2.f, (516 + 557) / 2.f);

    textscore.setFillColor(sf::Color::White);

    window.draw(textscore);
}

void DisplayFallingAnimation(sf::RenderWindow& window, sf::View view, sf::Texture* TextureBackground, TetrisGame tetrisgame, int* lines, float framenumber, float timeduration)
// Displays the animation of how tiles fall when a line is cleared. Timeduration is the duration of the animation and framenumber is the number of frames we display
{
    int firstline = lines[0] + 1;

    for (int frame = 1; frame <= framenumber; ++frame)
    {
        DisplayBackground(window, TextureBackground, tetrisgame);

        // We display tetrisgame only until the first cleared line
        tetrisgame.Draw(window, firstline);

        // Display falling tiles
        for (int ind = 0; ind < 4; ++ind)
        {
            for (int i = lines[ind + 1] + 1; i <= lines[ind] - 1; ++i)
            {
                for (int j = 0; j < N; ++j)
                {
                    if (tetrisgame.Tetrismat[i + ind + 1][j] > 0)
                    {
                        sf::RectangleShape tile;
                        tile.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
                        tile.setPosition(372 + TILE_SIZE * j, 60 + TILE_SIZE * i + (frame - 1) / (framenumber - 1) * (ind + 1) * TILE_SIZE);
                        switch (tetrisgame.Tetrismat[i + ind + 1][j])
                        {
                        case(1):
                            tile.setTexture(tetrisgame.TileTexture1); break;
                        case(2):
                            tile.setTexture(tetrisgame.TileTexture2); break;
                        case(3):
                            tile.setTexture(tetrisgame.TileTexture3); break;
                        case(4):
                            tile.setTexture(tetrisgame.TileTexture4); break;
                        case(5):
                            tile.setTexture(tetrisgame.TileTexture5); break;
                        case(6):
                            tile.setTexture(tetrisgame.TileTexture6); break;
                        case(7):
                            tile.setTexture(tetrisgame.TileTexture7); break;
                        }

                        if (tetrisgame.safemode)
                            tile.setTexture(tetrisgame.TileTexture3);

                        window.draw(tile);
                    }
                }
            }
        }

        window.setView(view);
        window.display();

        Sleep(timeduration / framenumber);
    }
}

int SumArray(int* array, int size)
{
    int rep = 0;
    for (int i = 0; i < size; ++i)
    {
        rep += array[i];
    }
    return rep;
}

int main()
{
    bool automaticmode;
    char dec = 'a';
    while (dec != 'y' && dec != 'n')
    {
        std::cout << "Do you want to play with automatic mode? (y/n) : ";
        std::cin >> dec;
    }
    if (dec == 'y')
        automaticmode = true;
    else
        automaticmode = false;

    bool instantauto = false;       bool safemode = false;    // Safemode is a mode where we display the tiles only in blue, which is useful when we have instantmode activated not to hurt looker's eyes.
    float dispautoevery = 10.f;                              // For automatic mode: number of seconds you have to wait until you display. For speeding up computer performance.



    // Initialising window, view, etc...

    sf::RenderWindow window(sf::VideoMode(600, 600), "Tetris");
    ::ShowWindow(window.getSystemHandle(), SW_MAXIMIZE);
    sf::Image icon;
    icon.loadFromFile("Tetris_icon.png");
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    sf::View view(sf::Vector2f(200.f, FIELD_HEIGHT / 2), sf::Vector2f(300.f, VIEW_HEIGHT));
    sf::Music music;

    if (!music.openFromFile("Tetris.ogg.ogx"))
    {
        std::cout << "ERROR MUSIC NOT FOUND" << std::endl;
    }

    // Offset and length of the loop calculated precisely for the music to keep playing endlessly without sudden change in the music

    sf::Time off = sf::milliseconds(5500);
    sf::Time len = sf::milliseconds(77150);
    sf::Music::TimeSpan Timepoints(off, len);
    music.setLoopPoints(Timepoints);
    music.setLoop(true);

    music.play();




    // Initialising tetrisgame

    sf::Texture TextureBackground;
    TextureBackground.loadFromFile("Background.png");
    sf::Texture TileText1;
    TileText1.loadFromFile("Cyan_Tile.png");
    sf::Texture TileText2;
    TileText2.loadFromFile("Yellow_Tile.png");
    sf::Texture TileText3;
    TileText3.loadFromFile("Blue_Tile.png");
    sf::Texture TileText4;
    TileText4.loadFromFile("Orange_Tile.png");
    sf::Texture TileText5;
    TileText5.loadFromFile("Lime_Tile.png");
    sf::Texture TileText6;
    TileText6.loadFromFile("Purple_Tile.png");
    sf::Texture TileText7;
    TileText7.loadFromFile("Red_Tile.png");
    sf::Texture TetrominoTexture1;
    TetrominoTexture1.loadFromFile("Cyan_Tetromino.png");
    sf::Texture TetrominoTexture2;
    TetrominoTexture2.loadFromFile("Yellow_Tetromino.png");
    sf::Texture TetrominoTexture3;
    TetrominoTexture3.loadFromFile("Blue_Tetromino.png");
    sf::Texture TetrominoTexture4;
    TetrominoTexture4.loadFromFile("Orange_Tetromino.png");
    sf::Texture TetrominoTexture5;
    TetrominoTexture5.loadFromFile("Lime_Tetromino.png");
    sf::Texture TetrominoTexture6;
    TetrominoTexture6.loadFromFile("Purple_Tetromino.png");
    sf::Texture TetrominoTexture7;
    TetrominoTexture7.loadFromFile("Red_Tetromino.png");

    TetrisGame tetrisgame(1, &TileText1, &TileText2, &TileText3, &TileText4, &TileText5, &TileText6, &TileText7,
        &TetrominoTexture1, &TetrominoTexture2, &TetrominoTexture3, &TetrominoTexture4, &TetrominoTexture5, &TetrominoTexture6, &TetrominoTexture7);
    tetrisgame.safemode = safemode;





    // Initialising variables

    Tetromino currenttetromino(tetrisgame.spawnarray[tetrisgame.indspawn], 0, 0, 0, 0, 0, 0, 0, 0);
    currenttetromino.SetAtSpawn(tetrisgame.spawnarray[tetrisgame.indspawn]);
    KeyboardVariables kvar{};
    sf::Clock clockframes;
    float timeframes = 0;
    sf::Clock clockintegrate;
    sf::Clock clockauto;
    float timeauto = 0;
    int lines[5] = { -1,-1,-1,-1,-1 };


    // For modes

    int gamestate = 1;     // 1 = Alive, 2 = Dead, 3 = Pause


    // For checking performances

    sf::Clock clockfps;           // Number of fps
    int framefps = 0;
    sf::Clock clocktimeloop;      // Duration of a loop
    float timeloop = 0;



    AutomaticInstructions autoinstrcurrent(instantauto);
    AutomaticInstructions autoinstrheld(instantauto);




    // Beginning of the program

    while (window.isOpen())
    {
    beginboucle:

        clocktimeloop.restart();


        // For detecting resize, close, etc...

        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::Resized:
                ResizeView(window, view);
                break;
            }
        }


        switch (gamestate)
        {
        case(1): default:            // Normal gamestate
        {

            // Frames calculations

            framefps += 1;
            timeauto += clockauto.restart().asSeconds();
            //if (clockfps.getElapsedTime().asSeconds() >= 1)
            //{
            //    clockfps.restart();
            //    std::cout << framefps << std::endl;
            //    framefps = 0;
            //}




            // Detect KeyPress

            kvar.UpdateTime();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::P) && !kvar.ppressed)                  // Pause
            {
                kvar.ppressed = true;
                gamestate = 3;
            }
            else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::P))
                kvar.ppressed = false;

            if (!automaticmode)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::C) && !kvar.exchanged)               // Swap with held tetromino
                {
                    kvar.exchanged = true;
                    int nexttype = tetrisgame.Swap(currenttetromino.type);
                    currenttetromino.SetAtSpawn(nexttype);
                    clockframes.restart();
                    timeframes = 0;
                    goto beginboucle;
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !kvar.spacebarpressed)     // Hard drop
                {
                    kvar.spacebarpressed = true;
                    while (currenttetromino.CanMove(1, 0, tetrisgame))
                    {
                        currenttetromino.Move(1, 0, tetrisgame);
                    }
                    kvar.Reset();
                    clockframes.restart();
                    timeframes = 0;
                    currenttetromino.Integrate(tetrisgame);
                    currenttetromino.SetAtSpawn(tetrisgame.spawnarray[tetrisgame.indspawn]);
                }
                if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
                {
                    kvar.spacebarpressed = false;
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))                                 // Move left
                {
                    if (kvar.PermitMoveLeft())
                        currenttetromino.Move(0, -1, tetrisgame);
                }
                else
                {
                    kvar.LeftPermitReset();
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))                                // Move right
                {
                    if (kvar.PermitMoveRight())
                        currenttetromino.Move(0, 1, tetrisgame);
                }
                else
                {
                    kvar.RightPermitReset();
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))                                  // Soft drop
                {
                    if (kvar.PermitMoveDown())
                    {
                        if (currenttetromino.CanMove(1, 0, tetrisgame))
                        {
                            currenttetromino.Move(1, 0, tetrisgame);
                        }
                        else
                        {
                            kvar.Reset();
                            clockframes.restart();
                            timeframes = 0;
                            currenttetromino.Integrate(tetrisgame);
                            currenttetromino.SetAtSpawn(tetrisgame.spawnarray[tetrisgame.indspawn]);
                        }
                    }
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::E) && !kvar.rotatingclock)           // Clockwise rotation
                {
                    currenttetromino.Rotate(true, tetrisgame);
                    kvar.rotatingclock = true;
                }
                if (!sf::Keyboard::isKeyPressed(sf::Keyboard::E))
                {
                    kvar.rotatingclock = false;
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) && !kvar.rotatingtrigo)           // Anti-clockwise rotation
                {
                    currenttetromino.Rotate(false, tetrisgame);
                    kvar.rotatingtrigo = true;
                }
                if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
                {
                    kvar.rotatingtrigo = false;
                }
            }
            else                    // Moving the tetromino for automatic mode
            {
                if (autoinstrcurrent.firstframe)                // We automatically hold the first tetromino
                {
                    autoinstrcurrent.firstframe = false;
                    int nexttype = tetrisgame.Swap(currenttetromino.type);
                    currenttetromino.SetAtSpawn(nexttype);
                    clockframes.restart();
                    timeframes = 0;
                    goto beginboucle;
                }
                else
                {
                    autoinstrcurrent.UpdateNextMove(tetrisgame, currenttetromino.type);

                    autoinstrheld.UpdateNextMove(tetrisgame, tetrisgame.heldtet);

                    if (instantauto)           // Instant automatic mode, for checking performances
                    {
                        if (autoinstrcurrent.GetScore() <= autoinstrheld.GetScore())        // Checking whether we must swap
                        {
                            currenttetromino.ForceChange(currenttetromino.type, autoinstrcurrent.GetNextPos());
                            currenttetromino.Integrate(tetrisgame);
                            currenttetromino.SetAtSpawn(tetrisgame.spawnarray[tetrisgame.indspawn]);
                            clockframes.restart();
                            timeframes = 0;
                        }
                        else
                        {
                            int nexttype = tetrisgame.Swap(currenttetromino.type);
                            currenttetromino.ForceChange(nexttype, autoinstrheld.GetNextPos());
                            currenttetromino.Integrate(tetrisgame);
                            currenttetromino.SetAtSpawn(tetrisgame.spawnarray[tetrisgame.indspawn]);
                            clockframes.restart();
                            timeframes = 0;
                        }
                    }
                    else                        // Basic automatic mode
                    {
                        if (autoinstrcurrent.GetScore() <= autoinstrheld.GetScore())          // Checking whether we must swap
                        {
                            int rotinst = autoinstrcurrent.RotationInstruction(currenttetromino);
                            if (rotinst == 1)
                                currenttetromino.Rotate(true, tetrisgame);
                            else if (rotinst == -1)
                                currenttetromino.Rotate(false, tetrisgame);
                            int moveinst = autoinstrcurrent.MoveInstruction(currenttetromino);
                            if (moveinst != 0)
                                currenttetromino.Move(0, moveinst, tetrisgame);
                            else
                            {
                                if (currenttetromino.CanMove(1, 0, tetrisgame))
                                    currenttetromino.Move(1, 0, tetrisgame);
                                else
                                {
                                    clockframes.restart();
                                    timeframes = 0;
                                    currenttetromino.Integrate(tetrisgame);
                                    currenttetromino.SetAtSpawn(tetrisgame.spawnarray[tetrisgame.indspawn]);
                                }
                            }
                        }
                        else
                        {
                            int nexttype = tetrisgame.Swap(currenttetromino.type);
                            currenttetromino.SetAtSpawn(nexttype);
                            clockframes.restart();
                            timeframes = 0;
                            goto beginboucle;
                        }
                    }
                }
            }



            // Descending the Tetromino after a given time

            timeframes += clockframes.restart().asMilliseconds();
            if (timeframes >= tetrisgame.framespermove * FRAME_TIME)
            {
                timeframes -= tetrisgame.framespermove * FRAME_TIME;
                if (currenttetromino.CanMove(1, 0, tetrisgame))
                {
                    currenttetromino.Move(1, 0, tetrisgame);
                }
                else
                {
                    if (!tetrisgame.timeintstarted)
                    {
                        tetrisgame.timeintstarted = true;
                        tetrisgame.timeint = 0;
                        clockintegrate.restart();
                    }
                }
            }
            if (tetrisgame.timeintstarted)
            {
                tetrisgame.timeint += clockintegrate.restart().asMilliseconds();
                if (tetrisgame.timeint >= tetrisgame.maxintframerate * FRAME_TIME)
                {
                    kvar.Reset();
                    currenttetromino.Integrate(tetrisgame);
                    currenttetromino.SetAtSpawn(tetrisgame.spawnarray[tetrisgame.indspawn]);
                }
            }



            // Checking upon death

            if (currenttetromino.CheckDeath(tetrisgame))
            {
                std::cout << "You're dead. Score : " << to_string_withcommas(tetrisgame.score) << std::endl;
                gamestate = 2;
            }



            // Updating tetrisgame

            tetrisgame.ClearLines(lines);




            // Displaying everything

            int firstline = lines[0] + 1;

            if (!instantauto || (instantauto && timeauto >= dispautoevery) || gamestate == 2 || kvar.ppressed)
            {
                timeauto -= dispautoevery;
                if (firstline == 0)
                {
                    DisplayBackground(window, &TextureBackground, tetrisgame);
                    tetrisgame.Draw(window, firstline);
                }
                else
                {
                    int numberofframes;
                    if (automaticmode && instantauto)
                        numberofframes = 0;
                    else if (automaticmode && !instantauto)
                        numberofframes = (35 - tetrisgame.level) / 10;
                    else
                        numberofframes = (35 - tetrisgame.level) / 5;
                    DisplayFallingAnimation(window, view, &TextureBackground, tetrisgame, lines, numberofframes, numberofframes * FRAME_TIME);
                    DisplayBackground(window, &TextureBackground, tetrisgame);
                    tetrisgame.Draw(window, 0);
                }
                currenttetromino.Draw(window, tetrisgame);
                window.setView(view);
                window.display();

            }
            lines[0] = -1; lines[1] = -1; lines[2] = -1; lines[3] = -1; lines[4] = -1;

            break;
        }

        case(2):                            // Dead mode
            window.close();
            break;

        case(3):                            // Pause mode
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::P) && !kvar.ppressed)
            {
                kvar.ppressed = true;
                gamestate = 1;
                clockframes.restart();
            }
            else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::P))
                kvar.ppressed = false;
        }
        }

        //std::cout << "Time loop : " << clocktimeloop.restart().asMilliseconds() << std::endl;

        if (!(automaticmode && instantauto && gamestate == 1))
            Sleep(SLEEP_TIME);
    }
}