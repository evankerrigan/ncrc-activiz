#include "SoundHandler.h"

SoundHandler::SoundHandler()
{
	debug = DEBUG_FLAG;
	// Use a circular array to store previous samples
	// e.g., [ current-2  ,current-1  ,current, current-4, current-3 ]
	for(int i=0; i < NUM_PREVIOUS_SAMPLES-1; i++){
		previousMicValues[i] = 0;
		previousMicValueDiffs[i] = 0;
	}
	previousMicValues[NUM_PREVIOUS_SAMPLES-1] = 0;
	currentSampleIndex = 0;
}

bool SoundHandler::containHumanVoice( uint16_t * spectrum, int length = 64)
{
	//debug && Serial.println("contain Human Voice start");
	
	int energyDetectedAcrossHumanVoiceRange = 0;
	for(byte i = 2; i < 15; i++){
		debug && Serial.println(spectrum[i]);
		energyDetectedAcrossHumanVoiceRange += spectrum[i];
	}
	
	//debug && Serial.println("energy detected:");
	debug && Serial.println(energyDetectedAcrossHumanVoiceRange);
	
	
	if(energyDetectedAcrossHumanVoiceRange >= T_SOUND_HUMAN_VOICE){
		return true;
	} else {
		return false;
	}
}

byte SoundHandler::getEvent(unsigned long avgSquaredMicValue, unsigned long minSquaredMicValue, unsigned long maxSquaredMicValue)
{
	debug && Serial.println("getEvent");
	debug && Serial.println(avgSquaredMicValue);
	debug && Serial.println(minSquaredMicValue);
	debug && Serial.println(maxSquaredMicValue);
	
	// Check if remain the same volumn
	unsigned long avgPreviousMicValue = 0;
	debug && Serial.println("Break Point - line: 25");
	
	for(int i= 0; i < NUM_PREVIOUS_SAMPLES; i++){
		debug && Serial.println("avgPreviousMicValue");
		debug && Serial.println(avgPreviousMicValue);
		
		avgPreviousMicValue += previousMicValues[i];
	}
	
	debug && Serial.println("Break Point - line: 29");
	avgPreviousMicValue /= NUM_PREVIOUS_SAMPLES;
	

	
	//Add current mic values to the previous mic values array
	previousMicValues[currentSampleIndex] = avgSquaredMicValue;
	currentSampleIndex++;
	if(currentSampleIndex >= NUM_PREVIOUS_SAMPLES){
		currentSampleIndex = 0;
	}
	
	long micValueDiff = avgSquaredMicValue - avgPreviousMicValue;
	
	debug && Serial.println("avgPreviousMicValue");
	debug && Serial.println(avgPreviousMicValue);
	debug && Serial.println("currentMicValue");
	debug && Serial.println(avgSquaredMicValue);
	debug && Serial.println("Diff");
	debug && Serial.println(micValueDiff);
	debug && Serial.println("max squared mic value");
	debug && Serial.println(maxSquaredMicValue);
	
	
	debug && Serial.println("Break Point - line: 37");
	
	if( maxSquaredMicValue - avgSquaredMicValue > T_SOUND_SUDDEN_INC_SQ ){
		//peak
		return E_SOUND_PEAK;
	} 
	debug && Serial.println("Not Peak");
	
	if( micValueDiff > 0 && micValueDiff > T_SOUND_REMAIN_RANGE_SQ ){
		// might be increase or sudden increase, return increase now
		return E_SOUND_INC;
	} else if (micValueDiff < 0 && -micValueDiff > T_SOUND_REMAIN_RANGE_SQ){ 
		// might be decrease or sudden decrease, return decrease now
		return E_SOUND_DEC;
	} 
	
	debug && Serial.println("Break Point - line: 51");
	
	return E_SOUND_REMAIN;

}