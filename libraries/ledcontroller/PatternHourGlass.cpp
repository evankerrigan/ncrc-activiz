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

#define DEFAULT_TRANSITION_TIME 2000
LED_CONTROLLER_NAMESPACE_USING

PatternHourGlass::PatternHourGlass(const Color& bgColor, const Color& color1, const Color& color2, bool allowTransition)
:PatternSineWave(bgColor), transitionEachActionInterval(DEFAULT_TRANSITION_TIME)
{
	this->bgColor = bgColor;
	maxValueCanBePresentedOnHourGlass = 30;
	reverse = false;
	currentColorIndex = 0;
	indicatorUnit = 1;
	colors[0] = color1;
	colors[1] = color2;
	indicator = actualValueBeingStored % (maxValueCanBePresentedOnHourGlass/indicatorUnit);
	indicator *= indicatorUnit;
	this->allowTransition = allowTransition;
	restart();
}

PatternHourGlass::PatternHourGlass(const Color& bgColor, const Color& color1, const Color& color2)
:PatternSineWave(bgColor), transitionEachActionInterval(DEFAULT_TRANSITION_TIME)
{
	this->bgColor = bgColor;
	maxValueCanBePresentedOnHourGlass = 30;
	reverse = false;
	currentColorIndex = 0;
	indicatorUnit = 1;
	colors[0] = color1;
	colors[1] = color2;
	indicator = actualValueBeingStored % (maxValueCanBePresentedOnHourGlass/indicatorUnit);
	indicator *= indicatorUnit;
	this->allowTransition = true;
	restart();
}



void PatternHourGlass::restart()
{
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
		
		inTransition = (allowTransition && true);
		//inTransition = true;
		
		transitionColorIndex = tempColorIndex;
		iniTransition();
		
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
		if(isTransitionExpired()){
			inTransition = false;
			setTransitionExpired(false);
			
			// reset the frozen indicator to it's current value
			indicator = actualValueBeingStored % (maxValueCanBePresentedOnHourGlass/indicatorUnit);
			indicator *= indicatorUnit;
		}
		
	 	transitionUpdate();
		transitionApply(stripColors);

				
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
	
	actualValueBeingStored = value; // actualValueBeingStored will be added 1 when update,  
									  // so we store value-1 here, it is a tricky thing.
	
	// Change the value stored will influence the indicator and indicator color
	indicator = actualValueBeingStored % (maxValueCanBePresentedOnHourGlass/indicatorUnit);
	indicator *= indicatorUnit;
	byte tempColorIndex = currentColorIndex;
	currentColorIndex = (actualValueBeingStored / (maxValueCanBePresentedOnHourGlass/indicatorUnit)) % DEFAULT_MAX_COLORS;
	
	//update();
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

byte PatternHourGlass::getIndicator()
{
	return indicator;
}
Color PatternHourGlass::getBgColor()
{
	return bgColor;
}
Color PatternHourGlass::getIndicatorColor()
{
	return colors[currentColorIndex];
}


void PatternHourGlass::iniTransition()
{
	this->transitionStartPosition = indicator;
	this->transitionEndPosition = 0;
	this->transitionExpired = false;
	byte diff = abs(transitionEndPosition-transitionStartPosition);
	if(diff == 0){
		diff = 1;
	}
	unsigned long eachActionTimeInMilliSec = DEFAULT_TRANSITION_TIME/diff;
	transitionEachActionInterval.setInterval(eachActionTimeInMilliSec);
	transitionIncrement = (transitionStartPosition > transitionEndPosition) ? -1: 1;
	transitionCurrentPosition = transitionStartPosition;
	
}

bool PatternHourGlass::transitionUpdate()
{
	transitionEachActionInterval.update();
	if(transitionEachActionInterval.isExpired()){
		transitionEachActionInterval.clearExpired();
		transitionAdvance();
		return true;
	} else {
		return false;
	}
	
}

void PatternHourGlass::transitionAdvance()
{
	if(transitionCurrentPosition != transitionEndPosition)
		transitionCurrentPosition += transitionIncrement;
	
	// Once the current position reaches the end point
	// set the Led Animation state to 'expired'
	if(transitionCurrentPosition == transitionEndPosition){
		this->transitionExpired = true;
	}
	
	
}

void PatternHourGlass::transitionApply(Color* stripColors)
{
	if(isReverse())
	{
		for(byte i = STRIP_LENGTH-1; i > (STRIP_LENGTH-1)-transitionCurrentPosition; i--){
			float scale = calculateScale(i);
			stripColors[i].add(colors[transitionColorIndex].scaled(scale));	
		}
		for(byte i = (STRIP_LENGTH-1) - transitionCurrentPosition; i >= 0; i--){
			float scale = calculateScale(i);
			stripColors[i].add(bgColor.scaled(scale));
		}
	} else {
		for(byte i = 0; i < transitionCurrentPosition; i++){
			float scale = calculateScale(i);
			stripColors[i].add(colors[transitionColorIndex].scaled(scale));
		}
		for(byte i = transitionCurrentPosition; i < STRIP_LENGTH; i++ ){
			float scale = calculateScale(i);
			stripColors[i].add(bgColor.scaled(scale));
		}
	}
	
}

bool PatternHourGlass::isTransitionExpired()
{
	return transitionExpired;
}
void PatternHourGlass::setTransitionExpired(bool expired)
{
	transitionExpired = expired;
}

void PatternHourGlass::transitionRestart()
{
	transitionExpired = false;
	transitionCurrentPosition = transitionStartPosition;
	transitionEachActionInterval.clearExpired();
}

