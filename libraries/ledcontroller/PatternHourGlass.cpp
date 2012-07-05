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
:PatternSineWave(bgColor), patBarPlotToBarPlot(0, 0, bgColor, color1, 1000)
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
	
	
	// Update color index
	byte tempColorIndex = currentColorIndex;
	currentColorIndex = (actualValueBeingStored / (maxValueCanBePresentedOnHourGlass/indicatorUnit)) % DEFAULT_MAX_COLORS;
	
	// Check whether the state should be change to *transition state*
	if(tempColorIndex != currentColorIndex){
		// HourGlass transform, from one color to another, 
		// and the indicator will go down to zero
		// To make it looks more organic, and pretty, add PatternBarPlotToBarPlot
		// Also, should lock the indicator when we are in transition state
		inTransition = true;
		patBarPlotToBarPlot = PatternBarPlotToBarPlot(indicator, 
								0, bgColor, colors[tempColorIndex], 1000);
		
	}
	if(!inTransition)	//lock the indicator when the pattern is in transition state
	{
		// Update indicator
		indicator = actualValueBeingStored % (maxValueCanBePresentedOnHourGlass/indicatorUnit);
		indicator *= indicatorUnit;
	}
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
	
	// The pattern has two states, inTransition and normal
	// everytime when the indicaotr reach the highest point, a transitional pattern
	// will show up for certain seconds, after the transitional pattern is animated
	// the pattern will change back to the normal state.
	
	if(inTransition){
		// if the transitional animation is finised, change to render the pattern of normal state
		if(patBarPlotToBarPlot.isExpired()){
					inTransition = false;
					patBarPlotToBarPlot.setExpired(false);
		}
		
		patBarPlotToBarPlot.updateSine();	
		patBarPlotToBarPlot.update();
		patBarPlotToBarPlot.apply(stripColors);

				
	} else {
		// Normal State
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
	}// End Normal State
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

