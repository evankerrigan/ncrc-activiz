//
//  ProgressBarSine.h
//  
//
//  Created by mac on 6/19/12.
//  Author: Chuan-Che Huang. chuanche@umich.edu
//


#include "Namespace.h"
#include "Color.h"
#include "Interval.h"
#include "Pattern.h"

LED_CONTROLLER_NAMESPACE_ENTER

class ProgressBarSine : public Pattern {
private:
    Color bgColor;  // background color
    Color barColor; // progress bar color
    //float intensity;
    int position; 
    int increment;
	float amplitude;
	float origin;
	byte sineWaveFrameCounter;
	//Interval sineWaveMovingInterval;
	

    /**
     *  Moving LED strip pattern to next state
     * @return whether the progress bar reach the top LED.
     */
    bool advance();
public:
    /**
     * Create a new progress bar. It starts at the 0 
     * end of the strip, moving up one grid every update
     */
    ProgressBarSine(const Color& bgColor, const Color& barColor);
    
    /**
     * Update the indicator's position.
     * @return whether it reach the top point, 
     *  which is the (STRIP_LENGTH -1)th LED.
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
     * Add progress bar's color to the strip, for output.
     *    
     */
    void apply(Color* stripColors);

    /**
     * Restart the Progress bar from the 0 end of the strip.
     */
    void restart();

	void setSineInterval();

};

LED_CONTROLLER_NAMESPACE_EXIT
