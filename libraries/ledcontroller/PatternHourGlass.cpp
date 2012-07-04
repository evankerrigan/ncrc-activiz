//
//  PatternHourGlass.cpp
//  
//
//  Created by mac on 6/21/12.
//  Author: Chuan-Che Huang. chuanche@umich.edu
//

#include "PatternHourGlass.h"
#include "Config.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define DEFAULT_MAX_LEDS_USED_BY_INDICATOR 32

// Parameters for sine wave control
#define DEFAULT_SINE_WAVE_ORIGIN 0.45	//min: 0.0 , max: 1.0
#define DEFAULT_SINE_WAVE_AMPLITUDE 0.4	// origin + aplitude can not go over 1.0
#define DEFAULT_BG_COLOR 0x000000
LED_CONTROLLER_NAMESPACE_USING

PatternHourGlass::PatternHourGlass(const Color& bgColor, const Color& color1, const Color& color2)
:PatternSineWave(bgColor)
{
	this->bgColor = bgColor;
	maxValueCanBePresentedOnHourGlass = 32;
	reverse = false;
	currentColorIndex = 0;
	indicatorUnit = 4;
	colors[0] = color1;
	colors[1] = color2;
	restart();
}

void PatternHourGlass::restart()
{
	indicator = 0;
	actualValueBeingStored = 0;
	currentColorIndex = 0;
}

void PatternHourGlass::advance()
{
	// Value being stored can not go over one byte, prevent overflow
	if(actualValueBeingStored+1 >= 255){
		actualValueBeingStored = 255;
	} else {
		actualValueBeingStored++;
	}
	
	// Update indicator
	indicator = actualValueBeingStored % (maxValueCanBePresentedOnHourGlass/indicatorUnit);
	indicator *= indicatorUnit; 
	
	// Update color index
	currentColorIndex = (actualValueBeingStored / (maxValueCanBePresentedOnHourGlass/indicatorUnit)) % DEFAULT_MAX_COLORS;
}

bool PatternHourGlass::update()
{
	advance();
	return true;
}

void PatternHourGlass::apply(Color* stripColors)
{
	//	Need to handle HourGlass Transition animation, 
	//	from indicator color i to indicator color (i+1)
	//	...code
	
	
	if(isReverse())
	{
		for(byte i = STRIP_LENGTH-1; i > (STRIP_LENGTH-1)-indicator; i--){
			float scale = calculateScale(i);
			stripColors[i].add(colors[currentColorIndex].scaled(scale));	
		}
		for(byte i = (STRIP_LENGTH-1) - indicator; i >= 0; i--){
			float scale = calculateScale(i);
			stripColors[i].add(bgColor.scaled(scale));
		}
	} else {
		for(byte i = 0; i < indicator; i++){
			float scale = calculateScale(i);
			stripColors[i].add(colors[currentColorIndex].scaled(scale));
		}
		for(byte i = indicator; i < STRIP_LENGTH; i++ ){
			float scale = calculateScale(i);
			stripColors[i].add(bgColor.scaled(scale));
		}
	}
}

void PatternHourGlass::setMaxValueCanBePresentedOnHourGlass(byte maxValue)
{
	if(maxValue > STRIP_LENGTH)
		maxValueCanBePresentedOnHourGlass = STRIP_LENGTH;
	else if(maxValue < 1)
		maxValueCanBePresentedOnHourGlass = 1;
	else 
		maxValueCanBePresentedOnHourGlass = maxValue;
}

byte PatternHourGlass::getActualValueBeingStored()
{
	return actualValueBeingStored;
}

void PatternHourGlass::setActualValueBeingStored(byte value)
{
	actualValueBeingStored = value;
}

void PatternHourGlass::setReverse(bool reverse)
{
	this->reverse = reverse;
}

bool PatternHourGlass::isReverse()
{
	return reverse;
}

void PatternHourGlass::setIndicatorUnit(byte unit)
{
	if(unit > 32)
		indicatorUnit = 32;
	else if (unit < 1)
		indicatorUnit = 1;
	else 
		indicatorUnit = unit;
}

byte PatternHourGlass::getIndicatorUnit()
{
	return indicatorUnit;
}

