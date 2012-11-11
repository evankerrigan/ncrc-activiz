//
// CanvasPatternFireworks.h
//
// Created by Chuan-Che Huang. chuanche@umich.edu
//

#include "Namespace.h"
#include "Color.h"
#include "Interval.h"
#include "Config.h"

// In the future, I should implement a Parent class for all the 
// CanvasPattern: a pattern that combine together multiple LED strips

LED_CONTROLLER_NAMESPACE_ENTER

class CanvasPatternFireworks {

private:
	// An array of base color
	Color myBaseColors[STRIP_NUM][STRIP_LENGTH];

	// An array of firework 
	Color myFireworkColors[STRIP_NUM][STRIP_LENGTH];

	// baseColor
	Color myBaseColor;

	// fireworkColor
	Color myFireworkColor;

	// Amplitude and Base for sinosoidal wave
	// f = ( A +/- base)
	// float, Amplitude for sinosoidal scale ( 0 - 1.0)
	float myWaveAmplitude;
	float myWaveOrigin;

	// LED strips Pin, An array of dataPins
	const int myDataPins[STRIP_LENGTH];

	// LED strips, An Array of clockPins
	const int myClockPins[STRIP_LENGTH];

	// reverse
	const bool myReverse;


public:
	/**
	 * Constructor: 
	 *  - baseColor
	 *	- fireworkColor
	 *	- bindPins ( An array of pins? )
	 */
	 CanvasPatternFireworks(int dataPins [], int clockPins [], 
	 	bool reverse=false, Color baseColor=(0x0000FF), 
	 	Color fireworkColor=(0xFF0000));




	 // setup all
	 void setup();

	 // clear
	 // clear all, or clear a single strip
	 void clear();

	 // render the color on the LED rods
	 void render();

	 // update 
	 void update();

	 // updateFirework
	 void updateFirework();
	 
	 void updateBase();

	 void addFirework();

	 void setWave(float origin, float amplitude);

	 void setBaseColor(Color baseColor);

	 void setFireworkColor(Color fireworkColor)

};

LED_CONTROLLER_NAMESPACE_EXIT