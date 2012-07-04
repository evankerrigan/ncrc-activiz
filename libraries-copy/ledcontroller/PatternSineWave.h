//
//  PatternSineWave.h
//  
//
//  Created by mac on 6/21/12.
//  Author: Chuan-Che Huang. chuanche@umich.edu
//
#pragma once

#include "Namespace.h"
#include "Color.h"
#include "Interval.h"
#include "Pattern.h"

LED_CONTROLLER_NAMESPACE_ENTER

/**
 *	Each LED's brightness on the specified LED strip will increase and decrease
 *	with the time pass, followed a sinosoidal wave pattern. 
 *	Two adjacent LED's sine wave contains a phase shift, 
 *	 	DEFAULT_PHASE_SHIFT = (1/STRIP_LENGTH)*2*PI;
*/

class PatternSineWave : public Pattern
{
private:
	Color bgColor;		
	float amplitude;
	float origin;
	byte sineWaveFrameCounter;
	
	
	/**
	*	Moving LED strip pattern to next state
	*/
	bool advance();
	
protected:
	/**
	*	Calculate the brightness for each LED according to it's 
	*	position
	*/
	float calculateScale(byte aLedIndex);

public:
		
	/**
	*	Create a new sinosoidal wave pattern
	*/
	PatternSineWave(const Color& bgColor);
	
	/**
	*	Update the Sinosoidal Wave pattern, this function could be override 
	*	by the inheriter to other customized update function.
	*/
	bool update();
	
	/**
	*	Update the Sinosoidal Wave pattern of each LED to next state
	*	Every single LED runs a Sine wave with same amplitude and 
	*	period, but with different phase shift. The phase shift between 
	* 	two adjacent LEDs is (1/STRIP_LENGTH)*2*PI.
	*	@return true if successfully update the sine wave.
	*/
	bool updateSine();
	
	/**
	*	apply the sinosoidal wave to each LED.
	*	NOTE: when inherited this method, 
	*/
	void apply(Color* stripColors);
	
	/**
	*	Restart the Sine Wave from the 0 frame
	*/
	void restart();	
	
	/**
	*	Set the Amplitude of the sinosoidal wave
	*/
	void setAmplitude(float amplitude);
	
	/*
	*	Set the Origin of the sinosoidal wave
	*/
	void setOrigin(float origin);
};

LED_CONTROLLER_NAMESPACE_EXIT