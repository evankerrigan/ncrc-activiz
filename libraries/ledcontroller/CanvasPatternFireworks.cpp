//
// CanvasPatternFireworks.cpp
//
// Created by Chuan-Che Huang. chuanche@umich.edu
//

#include "CanvasPatternFireworks.h"
#include "Config.h"
	// Config.h has the definition of number of led each strip

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// Put the re-defined parameters here
// They are the ones that could play with


// ... End of predefine parameters

LED_CONTROLLER_NAMESPACE_USING
// from now on, when use this name space, the rest of the 
// code will be exposed


CanvasPatternFireworks::CanvasPatternFireworks(int dataPins [], int clockPins [], bool reverse, Color baseColor, Color fireworkColor)
	 	: myDataPins(dataPins), myClockPins(clockPins), myReverse(reverse), myBaseColor(baseColor), myFireworkColor(fireworkColor) 
	 	{ }




// setup all
void CanvasPatternFireworks::setup();

// clear
// clear all, or clear a single strip
void CanvasPatternFireworks::clear();

// render the color on the LED rods
void CanvasPatternFireworks::render();

// update 
void CanvasPatternFireworks::update();

// updateFirework
void CanvasPatternFireworks::updateFirework();

void CanvasPatternFireworks::updateBase();

void CanvasPatternFireworks::addFirework();

void CanvasPatternFireworks::setWave(float origin, float amplitude);

void CanvasPatternFireworks::setBaseColor(Color baseColor);

void CanvasPatternFireworks::setFireworkColor(Color fireworkColor)


