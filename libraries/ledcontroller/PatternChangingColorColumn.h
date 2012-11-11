//
//  PatternChangingColorColumn.h
//  
//
//  Created by mac on 6/21/12.
//  Author: Chuan-Che Huang. chuanche@umich.edu
//

#include "Namespace.h"
#include "Color.h"
//#include "Interval.h"
#include "Pattern.h"

#define DEFAULT_MAX_COLORS 5
LED_CONTROLLER_NAMESPACE_ENTER
/**
 *	The specified LED strip will changing the colors sequentially according 
 *	to the assigned Colors. 
 * 	The assigned Colors will be store in an Array, and the LED strip will 
 *  iteratelly display the Colors in the Array. All the LEDs on the strip
 * 	will have the same color.
 */

class PatternChangingColorColumn : public Pattern
{
private:
	// Color Array
	Color colorArray[DEFAULT_MAX_COLORS];
	byte colorArrayTop;
	float amplitude;
	float origin;
	byte sineWaveFrameCounter;
	byte numOfFrames;
	
	// The index of the color being displayed now
	byte currentDisplayedColorIndex;
	
	/*
	*	Move the current color on the strip to next color.
	*	The difference between update and advance is that, 
	*	the actual color changing task is done by "advance",
	*	and "update" provides a layer for developers to control
	*	when should the color be changed. 
	*/
	void advance();
	

public:
	/*
	*	Create a new ChangingColorColumn pattern with sinosoidal wave
	*/
	PatternChangingColorColumn();
	PatternChangingColorColumn(const Color& defaultColor);
	
	/*
	*	Update the current color to next color, once the iterator for 
	* 	the colorArray reach the last index, it will go back to index 0.
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
	
	/*
	*	Apply the color and sinosoidal pattern to a LED strip
	*	NOTE: need to use calculateScale function to show the sinosoidal
	*		  pattern manually. I know it is not the best design now.		 
	*
	*/
	void apply(Color* stripColors);
	
	/*
	*	Restart the Color from the first color being assigned
	*/
	void restart();
	
	/**
	*	Calculate the brightness for each LED according to it's 
	*	position
	*/
	float calculateScale(byte aLedIndex);
	// For Color Array (Stack)
	/*
	*	Add a Color to the Color Array
	*	@return, whether successfully add the color or not.
	*/
	bool addColor(const Color& color);
	
	/*
	*	Delete the latest color being added to the Color Array
	*	@return whether it delete the color.
	*/
	bool deleteColor(const Color& color);
	
	/*
	*	Delete all the color in the color Array
	*/
	bool deleteAll();
	
	/*
	*
	*/
	bool isFull();
	bool isEmpty();
	
	/*
	*	Set the number of frames for the sine wave
	*	increase the frame will decrease the changing speed of the sine wave pattern
	*	decrease the frame will increase the changing speed of the sine wave pattern
	*/
	void setNumOfFrames(byte frame);
};

LED_CONTROLLER_NAMESPACE_EXIT