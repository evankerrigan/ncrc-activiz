//
//  ProgressBarSine.cpp
//  
//
//  Created by mac on 6/19/12.
//  Author: Chuan-Che Huang. chuanche@umich.edu
//

#include "ProgressBarSine.h"
#include "Config.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define DEFAULT_SINE_WAVE_INTERVAL 10
#define DEFAULT_SINE_WAVE_FRAMES 60
#define DEFAULT_SINE_WAVE_ORIGIN 0.45 // min: 0.0, max: 1.0
#define DEFAULT_SINE_WAVE_AMPLITUDE 0.4
LED_CONTROLLER_NAMESPACE_USING

ProgressBarSine::ProgressBarSine(const Color& bgColor, const Color& barColor) :
	amplitude(DEFAULT_SINE_WAVE_AMPLITUDE),
	origin(DEFAULT_SINE_WAVE_ORIGIN)
{
    this->bgColor = bgColor;
	this->barColor = barColor;
	
	sineWaveFrameCounter = 0;
    restart();
}

bool ProgressBarSine::update()
{
    return advance();
}

bool ProgressBarSine::advance()
{
    position += increment;
    if(position >= STRIP_LENGTH) {
		restart();
        return true;
    }
    return false;
}

void ProgressBarSine::restart()
{
	increment = 1;
    position = 0;
}

void ProgressBarSine::apply(Color* stripColors){
    for(int i = 0; i < position+1; i++){
		float phaseShift = float(i)/STRIP_LENGTH;
		float scale = origin + 
			amplitude*sin((float(sineWaveFrameCounter)/DEFAULT_SINE_WAVE_FRAMES + phaseShift)*2*PI);
        stripColors[i].add(barColor.scaled(scale));
    }
    for(int i = position+1; i < STRIP_LENGTH; i ++){
		float phaseShift = float(i)/STRIP_LENGTH;
		float scale = origin + 
			amplitude*sin((float(sineWaveFrameCounter)/DEFAULT_SINE_WAVE_FRAMES + phaseShift)*2*PI);
        stripColors[i].add(bgColor.scaled(scale));
    }
}

bool ProgressBarSine::updateSine()
{
	sineWaveFrameCounter++;
	if(sineWaveFrameCounter >= DEFAULT_SINE_WAVE_FRAMES){
		sineWaveFrameCounter = 0;
	}
}


