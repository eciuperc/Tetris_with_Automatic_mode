#pragma once
#include "TetrisGame.h"

// Constants for horizontal movement
const float waitlr = 0.125f;              // Time between first and second movement (in sec.)
const float mtimelr = 0.025f;             // Time between two other consecutive movements (in sec.)
// Constant for vertical movement
const float mtimed = mtimelr;             // Time between two other consecutive movements (in sec.)

class KeyboardVariables
{
public:
    KeyboardVariables();

    void Reset();
    void UpdateTime();

    //Checks whether we can move horziontally or vertically, using the constant variables defined above
    bool PermitMoveLeft();
    void LeftPermitReset();
    bool PermitMoveRight();
    void RightPermitReset();
    bool PermitMoveDown();

public:
    // Prevents exchanging twice the same tetromino
    bool exchanged;
    // If we just press spacebar (resp P key) just once, the program will take it into account just once.
    bool spacebarpressed, ppressed;
    // Same for rotation keys
    bool rotatingclock, rotatingtrigo;

private:
    // Variables for calculating horizontal and vertical movement
    sf::Clock clockl, clockr, clockd;
    float timel, timer, timed;
    bool clocklstarted, clockrstarted, clockdstarted;
    bool firstmovedl, firstmovedr;
};