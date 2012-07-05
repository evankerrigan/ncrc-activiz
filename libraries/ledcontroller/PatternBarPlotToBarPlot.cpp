//
//  PatternBarPlotToBarPlot.cpp
//  
//
//  Created by mac on 6/21/12.
//  Author: Chuan-Che Huang. chuanche@umich.edu
//

#include "PatternBarPlotToBarPlot.h"
#include "Config.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


LED_CONTROLLER_NAMESPACE_USING

PatternBarPlotToBarPlot::PatternBarPlotToBarPlot(byte startPosition, byte endPosition, 
	const Color& bgColor, const Color& barColor, unsigned long milliSecForInterval)
	:PatternSineWave(bgColor)
{
	this->bgColor = bgColor;
	this->barColor = barColor;
	this->startPosition = startPosition;
	this->endPosition = endPosition;
	byte diff = abs(endPosition-startPosition);
	unsigned long eachActionTimeInMilliSec = milliSecForInterval/diff;
	eachActionInterval.setInterval(eachActionTimeInMilliSec);
	moveInterval.setInterval(milliSecForInterval);
	reverse = 0;
	increment = (startPosition > endPosition) ? -1: 1;
	currentPosition = startPosition;
}


bool PatternBarPlotToBarPlot::update()
{
	eachActionInterval.update();
	if(eachActionInterval.isExpired()){
		eachActionInterval.clearExpired();
		advance();
		return true;
	} else {
		return false;
	}
	
}

void PatternBarPlotToBarPlot::advance()
{
	
	
}

void PatternBarPlotToBarPlot::apply(Color* stripColors)
{
	
}

void PatternBarPlotToBarPlot::setReverse()
{
	
}

bool PatternBarPlotToBarPlot::isReverse()
{
	
}
