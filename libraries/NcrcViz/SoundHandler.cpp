#include "SoundHandler.h"

SoundHandler::SoundHandler()
{
	debug = DEBUG_FLAG;
	// Use a circular array to store previous samples
	// e.g., [ current-2  ,current-1  ,current, current-4, current-3 ]
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