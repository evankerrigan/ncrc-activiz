#pragma once

#include "NcrcVizConfig.h"
#include <stdint.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


#define DEFAULT_SOUND_VALUE 512

//Sound sampling and comparison
#define NUM_SAMPLES_IN_EACH_SLOT 10000 
#define NUM_PREVIOUS_SAMPLES 10

//Thresold
#define T_SOUND_SUDDEN_INC 400	//MAX: 512
#define T_SOUND_SUDDEN_DEC 100	//MAX: 512
#define T_SOUND_SUDDEN_INC_SQ T_SOUND_SUDDEN_INC*T_SOUND_SUDDEN_INC
#define T_SOUND_SUDDEN_DEC_SQ T_SOUND_SUDDEN_DEC*T_SOUND_SUDDEN_DEC
#define T_SOUND_REMAIN_RANGE 3	// Should test the real value
#define T_SOUND_REMAIN_RANGE_SQ T_SOUND_REMAIN_RANGE*T_SOUND_REMAIN_RANGE

#define T_SOUND_HUMAN_VOICE 50	//Thresold for FFT analysis, default = 70

//Event
#define E_SOUND_REMAIN 0
#define E_SOUND_SUDDEN_INC 1
#define E_SOUND_SUDDEN_DEC 2
#define E_SOUND_INC 3
#define E_SOUND_DEC 4
#define E_SOUND_PEAK 5
#define EVENT_DETECT_HUMAN_VOICE 1

class SoundHandler
{
public:
	SoundHandler();
	byte getEvent(unsigned long avgSquaredMicValue, unsigned long minSquaredMicValue, unsigned long maxSquaredMicValue);
	bool containHumanVoice( uint16_t* spectrum, int length);
	bool debug;
};