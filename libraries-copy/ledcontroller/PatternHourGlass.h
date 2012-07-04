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
	bool reverse;
	Color bgColor;
	Color colors[DEFAULT_MAX_COLORS];
	byte currentColorIndex;
	
	//Interval colorTransitionInterval;
	
	/**
	*	Increase the indicator
	*/
	void advance();
	
	
public:
	/**
	*	Create a new Hour Glass Pattern
	*/
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
	
	
};

LED_CONTROLLER_NAMESPACE_EXIT