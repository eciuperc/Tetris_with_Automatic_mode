#include "KeyboardVariables.h"

KeyboardVariables::KeyboardVariables()
{
    rotatingclock = false, rotatingtrigo = false;
    spacebarpressed = false, ppressed = false;

    clockl.restart(); clockr.restart(); clockd.restart();
    timel = 0, timer = 0, timed = 0;
    clocklstarted = false, clockrstarted = false, clockdstarted = false;
    firstmovedl = false, firstmovedr = false;

    exchanged = false;
}



void KeyboardVariables::Reset()
{
    clockl.restart(); clockr.restart(); clockd.restart();
    timel = 0, timer = 0, timed = 0;
    clocklstarted = false, clockrstarted = false, clockdstarted = false;
    firstmovedl = false, firstmovedr = false;

    exchanged = false;
}



void KeyboardVariables::UpdateTime()
{
    timel += clockl.restart().asSeconds();
    timer += clockr.restart().asSeconds();
    timed += clockd.restart().asSeconds();
}



bool KeyboardVariables::PermitMoveLeft()
// Can we move left?
{
    if (!clocklstarted)
    {
        timel = 0;
        clocklstarted = true;
        return true;
    }
    if (clocklstarted && timel >= waitlr)
    {
        timel = 0;
        firstmovedl = true;
        return true;
    }
    if (clocklstarted && firstmovedl && timel >= mtimelr)
    {
        timel = 0;
        return true;
    }
    return false;
}



void KeyboardVariables::LeftPermitReset()
// When "left" key is released
{
    clocklstarted = false;
    firstmovedl = false;
}



bool KeyboardVariables::PermitMoveRight()
// Can we move right?
{
    if (!clockrstarted)
    {
        timer = 0;
        clockrstarted = true;
        return true;
    }
    if (clockrstarted && timer >= waitlr)
    {
        timer = 0;
        firstmovedr = true;
        return true;
    }
    if (clockrstarted && firstmovedr && timer >= mtimelr)
    {
        timer = 0;
        return true;
    }
    return false;
}



void KeyboardVariables::RightPermitReset()
// When "right" key is released
{
    clockrstarted = false;
    firstmovedr = false;
}



bool KeyboardVariables::PermitMoveDown()
// Can we move down?
{
    if (timed >= mtimed)
    {
        timed = 0;
        return true;
    }
    else
        return false;
}