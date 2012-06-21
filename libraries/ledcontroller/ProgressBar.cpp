//
//  ProgressBar.cpp
//  
//
//  Created by mac on 6/19/12.
//  Author: Chuan-Che Huang. chuanche@umich.edu
//

#include "ProgressBar.h"
#include "Config.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

LED_CONTROLLER_NAMESPACE_USING

ProgressBar::ProgressBar(const Color& bgColor, const Color& barColor)
{
    this->bgColor = bgColor;
    this->barColor = barColor;
    restart();
}

bool ProgressBar::update()
{
    return advance();
}

bool ProgressBar::advance()
{
    position += increment;
    if(position >= STRIP_LENGTH) {
        position = 0;
        return true;
    }
    return false;
}

void ProgressBar::restart()
{
    increment = 1;
    position = 0;
}

void ProgressBar::apply(Color* stripColors){
    for(int i = 0; i < position+1; i++){
        stripColors[i].add(barColor);
    }
    for(int i = position+1; i < STRIP_LENGTH; i ++){
        stripColors[i].add(bgColor);
    }
}


