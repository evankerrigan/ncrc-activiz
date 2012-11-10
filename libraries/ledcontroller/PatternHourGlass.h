//
//  PatternHourGlass.h
//  
//
//  Created by mac on 6/21/12.
//  Author: Chuan-Che Huang. chuanche@umich.edu
//

#pragma once

#include "Namespace.h"
#include "Color.h"
#include "PatternSineWave.h"
#include "PatternBarPlotToBarPlot.h"
#include "Interval.h"

#define DEFAULT_MAX_COLORS 2
LED_CONTROLLER_NAMESPACE_ENTER
/**
 *	HourGlass Pattern:
 *	A pattern similar to progress bar, 
 *	LED strip:
 *		 0: xxxxxxxxxxx====================== :31
 *  x := indicator for the information you want to visualize, 
 *		 the color can be specified by constructor or setter
 *  = := background color
 */

class PatternHourGlass : public PatternSineWave
{
private:
	byte indicator;							// 0 to 31
	byte indicatorUnit;
	byte actualValueBeingStored;			// 0 to 63
	byte maxValueCanBePresentedOnHourGlass;	// Default: 32
	byte maxValueCanBeStoredInHourGlass;	// max: 255, default = 60
	bool reverse;
	Color bgColor;
	Color colors[DEFAULT_MAX_COLORS];
	byte currentColorIndex;
	bool inTransition;
	bool allowTransition;

private: //Variables for Embedded PatternBarPlotToBarPlot
	byte transitionStartPosition;
	byte transitionEndPosition;
	Interval transitionEachActionInterval;
	byte transitionCurrentPosition;
	bool transitionExpired;
	byte transitionIncrement;
	byte transitionColorIndex;
	
	
	//Interval colorTransitionInterval;
	
	/**
	*	Increase the indicator
	*/
	void advance();
	
	
public:
	/**
	*	Create a new Hour Glass Pattern
	*/
	PatternHourGlass(const Color& bgColor, const Color& color1, const Color& color2, bool allowTransition);
	PatternHourGlass(const Color& bgColor, const Color& color1, const Color& color2);
	/**
	*	Update the indicator.
	*	@return success or not
	*/
	bool update();
	
	/*
	*	Apply the color to the LED strip
	*/
	void apply(Color* stripColors);
	
	/*
	*	Reset the indicator and actualValueBeingStored
	*/
	void restart();
	
	/*
	*	Set the maximum value used for indicator
	*	LED strip usually has 32 LEDs, but we might not need to use
	*	all the LED as our indicator.
	*/
	void setMaxValueCanBePresentedOnHourGlass(byte maxValue);
	
	/*
	*	Get the data stored by hour glass
	*/
	byte getActualValueBeingStored();
	
	/*
	*	Set the data stored by hour glass
	*/
	void setActualValueBeingStored(byte value);
	
	/*
	*	Setter & Getter for reverse
	*/
	void setReverse(bool reverse);
	bool isReverse();
	
	/*
	*	Setter & Getter for indicatorUnit,
	*	max: 32
	*	min: 1
	*/
	void setIndicatorUnit(byte unit);
	byte getIndicatorUnit();

	byte getIndicator();
	Color getBgColor();
	Color getIndicatorColor();

	/**
	*	Methods for Transition
	*/
private:
	
	void iniTransition();
	
	/**
	*	@return whether a single action is being performed
	*	multiple actions = one total animation
	*/
	bool transitionUpdate();
	
	void transitionAdvance();
	
	void transitionApply(Color* stripColors);
	
	/*
	*	@return true if the whole animation is finished 
	*	which means all the actions are finished
	*	one action = turn on/off one individual led
	*/
	bool isTransitionExpired();
	void setTransitionExpired(bool expired);
	
	void transitionRestart();
	
	
	
};

LED_CONTROLLER_NAMESPACE_EXIT