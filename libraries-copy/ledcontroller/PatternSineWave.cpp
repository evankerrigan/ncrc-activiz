//
//  PatternSineWave.cpp
//  
//
//  Created by mac on 6/21/12.
//  Author: Chuan-Che Huang. chuanche@umich.edu
//

#include "PatternSineWave.h"
#include "Config.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define DEFAULT_SINE_WAVE_FRAMES 60
#define DEFAULT_SINE_WAVE_ORIGIN 0.45	//min: 0.0 , max: 1.0
#define DEFAULT_SINE_WAVE_AMPLITUDE 0.4	// origin + aplitude can not go over 1.0

LED_CONTROLLER_NAMESPACE_USING

PatternSineWave::PatternSineWave(const Color& bgColor) :
	amplitude(DEFAULT_SINE_WAVE_AMPLITUDE),
	origin(DEFAULT_SINE_WAVE_ORIGIN)
{
    this->bgColor = bgColor;
    restart();
}

bool PatternSineWave::update()
{
    return advance();
}

bool PatternSineWave::advance()
{
    sineWaveFrameCounter++;
	if(sineWaveFrameCounter >= DEFAULT_SINE_WAVE_FRAMES){
		sineWaveFrameCounter = 0;
	}
    return false;
}

void PatternSineWave::restart()
{
	setOrigin(DEFAULT_SINE_WAVE_ORIGIN);
	sineWaveFrameCounter = 0;
}

float PatternSineWave::calculateScale(byte aLedIndex)
{
	float phaseShift = float(aLedIndex)/STRIP_LENGTH;
	float scale = origin + 
		amplitude*sin( (float(sineWaveFrameCounter)/DEFAULT_SINE_WAVE_FRAMES + phaseShift)*2*PI);
	return scale;
}

void PatternSineWave::apply(Color* stripColors){
    for(int i = 0; i < STRIP_LENGTH; i++){
		float scale = calculateScale(i);
        stripColors[i].add(bgColor.scaled(scale));
    }
}

bool PatternSineWave::updateSine()
{
	sineWaveFrameCounter++;
	if(sineWaveFrameCounter >= DEFAULT_SINE_WAVE_FRAMES){
		sineWaveFrameCounter = 0;
	}
}

void PatternSineWave::setAmplitude(float amplitude)
{
	if(amplitude > 1.0 )
		this->amplitude = 1.0;
	else if (amplitude < 0)
		this->amplitude = 0;
	else 
		this->amplitude = amplitude;
}

void PatternSineWave::setOrigin(float origin)
{
	if(origin > 1.0 )
		this->origin = 1.0;
	else if (origin < 0)
		this->origin = 0;
	else 
		this->origin = origin;
	
}